// Encoder03.cpp

/*
 * Copyright (C) 2014, by Author: Eldon R. Brown (ERB) - WA0UWH
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of  MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#include <Arduino.h>
#include "A1Main.h"

#ifdef USE_ENCODER03


#define NO_PORTD_PINCHANGES
#define NO_PORTB_PINCHANGES
#define NO_PORTC_PINCHANGES

// One of the following "NO_PORTX_PINCHANGES" will be Undefined, Note: ENC_A_PIN must be a number.
#if defined (ENC_A_PIN)
    #if (ENC_A_PIN) >= 0 && (ENC_A_PIN) <= 7
        #undef NO_PORTD_PINCHANGES
    #endif
    #if (ENC_A_PIN) >= 8 && (ENC_A_PIN) <= 13
        #undef NO_PORTB_PINCHANGES
    #endif
    #if (ENC_A_PIN) >= 14 && (ENC_A_PIN) <= 19
        #undef NO_PORTC_PINCHANGES
    #endif
#endif


#define NO_PIN_STATE
#define NO_PIN_NUMBER
#define DISABLE_PCINT_MULTI_SERVICE

#include "PinChangeInt.h"

#include "Encoder03.h"
#include "debug.h"

volatile char knob;


// ###############################################################################
// ###############################################################################
void encoderISR() {
    int pin = ENC_B_PIN;
    // See: https://code.google.com/p/oopinchangeint/source/browse/Logic.wiki?repo=wiki
    static unsigned long startTime = 0;
    unsigned long tigermillis;
     
    uint8_t oldSREG = SREG;

    cli();
    tigermillis = millis();
    SREG = oldSREG;
    
    if (tigermillis - startTime <= ISR_DEBOUNCE_TIMEOUT) return;
    startTime = tigermillis;

    
    // 47K Pull-up, and 4.7K switch resistors,
    // Val should be approximately = 1024*BtnN*4700/(47000+(BtnN*4700))
    // Where N = (button-1)
    
    // 1024L*b*4700L/(47000L+(b*4700L))   >>>  1024*b/(10+b);
    // Btn = 8; Val = 1024*(Btn-1)/(10+(Btn-1)) = 421
    // Btn = 9; Val = 1024*(Btn-1)/(10+(Btn-1)) = 455
      
    cli();
    knob += analogRead(pin) < 440 ? -1 : +1;
    SREG = oldSREG;

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
    
    //debug("%s/%d: Knob= %d", __func__, __LINE__, knob); 
       
    uint8_t oldSREG = SREG;
    cli();     
    int val = (knob > 0) - (knob < 0);
    #ifdef USE_DIAL_MOMENTUM
        knob -= val;
    #else
        knob = 0;
    #endif // USE_DIAL_MOMENTUM
    SREG = oldSREG;
    return val;
}

#endif // USE_ENCODER03
// End

