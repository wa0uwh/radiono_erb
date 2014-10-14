// Encoder02.cpp


#include <Arduino.h>
#include "A1Main.h"

#ifdef USE_ENCODER02

// One of the following three lines must be commented out depending on Pins used for ISR.
#define NO_PORTB_PINCHANGES
#define NO_PORTC_PINCHANGES
////#define NO_PORTD_PINCHANGES

#define NO_PIN_STATE
#define NO_PIN_NUMBER
#define DISABLE_PCINT_MULTI_SERVICE

#include "PinChangeInt.h"

#include "Encoder02.h"
#include "debug.h"

volatile int knob;



// ###############################################################################
// ###############################################################################
void encoderISR() {
    int pin = ENC_B_PIN;
    
    knob += digitalRead(ENC_B_PIN) ? -1 : +1;
    
    //debug("%s/%d: Pin= %d, Knob= %d", __func__, __LINE__, knob);
}


// ###############################################################################
void initEncoder() {
    int pin = ENC_A_PIN;
     
    //debug("%s/%d: Pin= %d", __func__, __LINE__, pin);
    
    pinMode(ENC_A_PIN, INPUT_PULLUP);
    pinMode(ENC_B_PIN, INPUT_PULLUP);
    
    #ifdef USE_ENCODER02
        PCintPort::attachInterrupt(pin, &encoderISR, FALLING);
    #endif // USE_ENCODER02
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

#endif // USE_ENCODER02
// End
