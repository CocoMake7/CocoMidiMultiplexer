#include <CocoMidi.h>
#include <CocoTouch.h>
#include <CocoTouchFilterSettingDefault.h>

#define PIN_SELECT 0
#define NUM_CHANNEL 4

int value[8] = {0, 0, 0, 0, 0, 0, 0, 0};
uint16_t offset_adc[8] = {0, 0, 0, 0, 0, 0, 0, 0};

uint8_t note_off[8] = {1, 1, 1, 1, 1, 1, 1, 1};

CocoTouchFilterSetting filter_samp[8];

uint8_t pin_queue = 0;
//uint8_t multiplexer_mapping[8]  = {3, 0, 1, 2, 6, 7, 4, 4}; //remap multiplexer pin
//6
uint8_t multiplexer_mapping[8]  = {0, 2, 4, 6}; //remap multiplexer pin
unsigned long previousMillis = 0;        // will store last time LED was updated

unsigned long cocotouchsend_lastTime;

//cocoTouch pin
#define ADC_REF_PIN PB2
#define ADC_SENSE_PIN PB4

//define slower reading
#define CAP_SAMPLES_READ 6

static inline void setAnalogMultiplexCh(const uint8_t _pin_index)
{
  // set switch to output (not sure why, but must be set everytime..)
//  pinMode(PB1, OUTPUT);
//  pinMode(PB0, OUTPUT);

//  DDRB |=  _BV(PB1) | _BV(PB0);

  // set multiplexer, select channel
  digitalWrite(PB1, ((_pin_index >> 1) & 0x01));
  digitalWrite(PB0, ((_pin_index >> 2) & 0x01));

    //PORTB |= (((_pin_index>>1) & 0x01)<<PB1);
   // PORTB |= (((_pin_index>>2) & 0x01)<<PB0);
}

void setup() {

  pinMode(PB1, OUTPUT);
  pinMode(PB0, OUTPUT);
  
  // put your setup code here, to run once:
  CocoTouch.begin();
  CocoTouch.setAdcSpeed(4);
  CocoTouch.delay = 4;

  for (uint8_t i = 0; i < NUM_CHANNEL; i++)
  {
    setAnalogMultiplexCh(multiplexer_mapping[i]);
    offset_adc[i] = CocoTouch.sense(ADC_SENSE_PIN, ADC_REF_PIN, CAP_SAMPLES_READ + 1 );
    CocoMidi.delay(100);
  }

  CocoMidi.init();
}
int filtered_value = 0;

void loop() {

//  if (millis() - cocotouchsend_lastTime >= 5)  {
//    for (uint8_t i = 0; i < NUM_CHANNEL; i++)
//    {
//      setAnalogMultiplexCh(multiplexer_mapping[i]);
//      filtered_value = CocoTouchFilter_get(&filter_samp[0]);
//      CocoMidi.sendCCHires(filtered_value, 1);
//      value = CocoTouch.sense(ADC_SENSE_PIN, ADC_REF_PIN, 7 ) - offset_adc;
//      if (value > 0) CocoTouchFilter_put(&filter_samp[0], value);
//    }
//    cocotouchsend_lastTime = millis();
//  }
//
//  CocoMidi.update();

    // put your main code here, to run repeatedly:
    if (millis() - cocotouchsend_lastTime >= 7)  {
      for (uint8_t i = 0; i < NUM_CHANNEL; i++)
      {
        filtered_value = CocoTouchFilter_get(&filter_samp[i]);
        if (filtered_value >= 100 )
        {
          if (note_off[i] == 1)
          {
            CocoMidi.send(MIDI_NOTEON, i, 127 );
            note_off[i] = 0;
          }
        }
        else
        {
          if (note_off[i] == 0)
          {
            CocoMidi.send(MIDI_NOTEOFF, i, 127 );
            note_off[i] = 1;
          }
        }
      }
      cocotouchsend_lastTime = millis();
    }
  
    setAnalogMultiplexCh(multiplexer_mapping[pin_queue]);
    value[pin_queue] = CocoTouch.sense(ADC_SENSE_PIN,ADC_REF_PIN, CAP_SAMPLES_READ ) - offset_adc[pin_queue];
    if (value[pin_queue] > 0) CocoTouchFilter_put(&filter_samp[pin_queue], value[pin_queue]);
  
    pin_queue++;
    if (pin_queue > NUM_CHANNEL) pin_queue = 0;
    
    CocoMidi.update();
}
