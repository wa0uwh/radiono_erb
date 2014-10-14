// Encoder03.cpp


#include <Arduino.h>
#include "A1Main.h"

#ifdef USE_ENCODER03

// One of the following three lines must be commented out depending on Pins used for ISR.
#define NO_PORTB_PINCHANGES
//#define NO_PORTC_PINCHANGES
#define NO_PORTD_PINCHANGES

#define NO_PIN_STATE
#define NO_PIN_NUMBER
#define DISABLE_PCINT_MULTI_SERVICE

#include "PinChangeInt.h"

#include "Encoder03.h"
#include "debug.h"

volatile int knob;



// ###############################################################################
// ###############################################################################
void encoderISR() {
    int pin = ENC_B_PIN;
    
  // 47K Pull-up, and 4.7K switch resistors,
  // Val should be approximately = 1024*BtnN*4700/(47000+(BtnN*4700))
  // N = 0 to Number of button - 1
    
  // 1024L*b*4700L/(47000L+(b*4700L))   >>>  1024*b/(10+b);
  // Btn = 8; Val = 1024*(Btn-1)/(10+(Btn-1)) = 421
  // Btn = 9; Val = 1024*(Btn-1)/(10+(Btn-1)) = 455
  
    knob += analogRead(pin) < 445 ? -1 : +1;

    //debug("%s/%d: Pin= %d, Knob= %d", __func__, __LINE__, pin, knob);
}


// ###############################################################################
void initEncoder() {
    int pin = ENC_A_PIN;
     
    //debug("%s/%d: Pin= %d", __func__, __LINE__, pin);
    
    pinMode(ENC_A_PIN, INPUT_PULLUP);
  
    PCintPort::attachInterrupt(pin, &encoderISR, FALLING);
}


// ###############################################################################
int getEncoderDir() {
    int dir = 0;
    
    if (knob) {
        dir = knob > 0 ? +1 : knob < 1 ? -1 : 0;
        knob += -dir;
        //debug("%s/%d: Knob= %d, Dir= %d", __func__, __LINE__, knob, dir);
    }  
    return dir;
}


#endif // USE_ENCODER03
// End

