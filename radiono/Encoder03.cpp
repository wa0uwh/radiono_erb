// Encoder03.cpp


#include <Arduino.h>
#include "A1Main.h"

#ifdef USE_ENCODER03

#define NO_PORTB_PINCHANGES
//#define NO_PORTC_PINCHANGES
#define NO_PORTD_PINCHANGES
#include "PinChangeInt.h"

#define NO_PORTB_PINCHANGES

#include "Encoder03.h"
#include "debug.h"

volatile int knob;



// ###############################################################################
// ###############################################################################
void encoderISR() {
    int pin = ENC_B_PIN;
    
    knob += analogRead(pin) < 460 ? -1 : +1;
    
    //knob03 = analogRead(pin);
    
    debug("%s/%d: Pin= %d, Knob= %d", __func__, __LINE__, pin, knob);
}


// ###############################################################################
void initEncoder() {
    int pin = ENC_A_PIN;
     
    debug("%s/%d: Pin= %d", __func__, __LINE__, pin);
    
    pinMode(ENC_A_PIN, INPUT_PULLUP);
  
    PCintPort::attachInterrupt(pin, &encoderISR, FALLING);
}


// ###############################################################################
int getEncoderDir() {
    int dir = 0;
    
    if (knob) {
        //debug("%s/%d: Knob= %d, Dir= %d", __func__, __LINE__, knob, dir);
        dir = knob > 0 ? +1 : knob < 1 ? -1 : 0;
        knob += -dir;
    }  
    return dir;
}


#endif // USE_ENCODER03
// End

