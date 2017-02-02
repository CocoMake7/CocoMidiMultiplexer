/*
* The digitalWriteFastTiny library is made for fast writing on
* pins of an ATtiny
*
* How to use:
* 
* digitalWriteFast(pin, HIGH/LOW);
*
*/

#if !defined(digitalPinToPortReg)

// Standard Arduino Pins
#define digitalPinToPortReg(P) \
(((P) >= 0 && (P) <= 7) ? &PORTB : (((P) >= 8 && (P) <= 13) ? &PORTB : &PORTB))
#define digitalPinToBit(P) \
(((P) >= 0 && (P) <= 7) ? (P) : (P))


#endif

#if !defined(digitalWriteFast)
#define digitalWriteFast(P, V) \
if (__builtin_constant_p(P) && __builtin_constant_p(V)) { \
                bitWrite(*digitalPinToPortReg(P), digitalPinToBit(P), (V)); \
        } else { \
                digitalWrite((P), (V)); \
        }
#endif

