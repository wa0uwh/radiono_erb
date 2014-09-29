// Encoder01.cpp

#include <Arduino.h>
#include "A1Main.h"
#include "Encoder01.h"
#include "ButtonUtil.h"
#include "debug.h"

/*
#include <PinChangeInt.h>
#include <PinChangeIntConfig.h>
#include "/home/eldonb/Arduino/libraries/PinChangeInt/PinChangeInt.h"

//#define NO_PORTB_PINCHANGES // to indicate that port b will not be used for pin change interrupts
//#define NO_PORTC_PINCHANGES // to indicate that port c will not be used for pin change interrupts
//#define NO_PORTD_PINCHANGES // to indicate that port d will not be used for pin change interrupts
// if there is only one PCInt vector in use the code can be inlined
// reducing latency and code size
// define DISABLE_PCINT_MULTI_SERVICE below to limit the handler to servicing a single interrupt per invocation.
#define       DISABLE_PCINT_MULTI_SERVICE

volatile byte burp = 0;    // a counter to see how many times the pin has changed
volatile int localDir = 0;
*/
static int knob;

/*
// ############################################################################### 
void encoderISR()
{
  burp++;
  localDir = digitalRead(ENCODER_PINB)? -1 : +1;
  //debug("burp1: %d, localDir: %d", burp, localDir);
}
*/
// ###############################################################################
// ###############################################################################
void initEncoder() {
    
     pinMode(ENCODER_PINA, INPUT);     //set the pin to input
     digitalWrite(ENCODER_PINA, HIGH); //use the internal pullup resistor
        
     pinMode(ENCODER_PINB, INPUT);     //set the pin to input
     digitalWrite(ENCODER_PINB, HIGH); //use the internal pullup resistor
     
     // (RISING, FALLING and CHANGE all work with this library)
     // and execute the function encoderISR when that pin changes 
     // Attach a PinChange Interrupt to our pin on the FALLING edge
     //PCintPort::attachInterrupt(ENCODER_PINA, encoderISR, FALLING); 
}

/*
// ###############################################################################
int readEncoder2() {
    int dir = 0;
    dir = localDir;
    localDir = 0;
    return dir;
}

// ###############################################################################
int readEncoder3() {
    static int encoderA_Prev = 0;
    int dir = 0;
    
    if(encoderA_Prev && !digitalRead(ENCODER_PINA)) {
        dir = digitalRead(ENCODER_PINB)? -1 : +1;
        encoderA_Prev = 0;
    } else encoderA_Prev = digitalRead(ENCODER_PINA);
    return dir;
}

// ###############################################################################
int readEncoder4() {
    static int encoderA_Prev = 1;
    int dir = 0;
    //debugUnique("In readEncoder: %d",encoderA_Prev);

    if(encoderA_Prev && analogRead(ANALOG_TUNING) < 50) {
        //debugUnique("Got Low #####################################");
        dir = analogRead(FN_PIN) < 50? +1 : -1;
        //debugUnique("dir: %d", dir);
        while(analogRead(ANALOG_TUNING) < 50) true; 
        //debugUnique("Out read loop");
        encoderA_Prev = 0;
    } else encoderA_Prev = analogRead(ANALOG_TUNING) < 50;

    return dir;
}

// ###############################################################################
int readEncoder5() {
    static boolean encoderA_Prev = true;
    int dir = 0;
    int encoderB = 0;
    
    //debugUnique("In readEncoder: %d", encoderA_Prev);

    //debug("A: %d, %lu", analogRead(ANALOG_TUNING), millis());
    //debugUnique("A: %d", analogRead(ANALOG_TUNING));
    //if(encoderA_Prev && analogRead(ANALOG_TUNING) < 250) {
    debugUnique("A: %d", val);
    if(encoderA_Prev && val < 250) {
        //debugUnique("Got Low #####################################");
        encoderB = analogRead(FN_PIN);
        debugUnique("B: %d", encoderB);
        if (encoderB > 400) {
            dir = encoderB < 500 ? +1 : -1;
        }
        debugUnique("dir: %d", dir);
        while(analogRead(ANALOG_TUNING) < 260) true; 
        //debugUnique("Out read loop");
        encoderA_Prev = false;
    } else encoderA_Prev = analogRead(ANALOG_TUNING) < 250 ? true : false;
    return dir;
}
*/

// ###############################################################################
int getEncoderDir() {
    int dir = 0;
    
    if (knob) {
        dir = knob > 50 ? -1 : +1;
        //debug("%s btn %d", __func__, knob);    
        //debug("%s dir %d", __func__, dir);
        knob = 0;
    }  
    return dir;
}

// ###############################################################################
int getEncoderKnob(int btn) {
    
    if(btnDown() == ENC_KNOB) {
        knob = analogRead(ANALOG_TUNING);
        //debug("%s btn %d", __func__, btn);    
        //debug("%s val %d", __func__, knob);
        deDounceBtnRelease();
    }
}

// End
