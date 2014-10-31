// Encoder02.cpp

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
#include <util/atomic.h>

#ifdef USE_ENCODER02

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

#include "Encoder02.h"
#include "debug.h"

volatile int knob;


// ###############################################################################
// ###############################################################################
void encoderISR() {
    int pin = ENC_B_PIN;
    static unsigned long startTime = 0;
    unsigned long tigermillis;
  
    uint8_t oldSREG = SREG;

    cli();
    tigermillis = millis();
    SREG = oldSREG;
    
    if (tigermillis - startTime <= ISR_DEBOUNCE_TIMEOUT) return;
    startTime = tigermillis;
    
    cli();
    knob += digitalRead(ENC_B_PIN) ? -1 : +1;
    SREG = oldSREG;
    
    //debug("%s/%d: Pin= %d, Knob= %d", __func__, __LINE__, pin, knob);
}


// ###############################################################################
void initEncoder() {
    int pin = ENC_A_PIN;
     
    //debug("%s/%d: Pin= %d", __func__, __LINE__, pin);
    
    pinMode(ENC_A_PIN, INPUT_PULLUP);
    pinMode(ENC_B_PIN, INPUT_PULLUP);
 
    PCintPort::attachInterrupt(pin, &encoderISR, FALLING);
}


// ###############################################################################
int getEncoderDir() {
 
  uint8_t oldSREG = SREG;
  cli();
 
      #ifdef USE_DIAL_MOMENTUM
          int val = (knob > 0) - (knob < 0);
          knob -= val;
      #else 
          int val = knob;
          knob = 0;
      #endif // USE_DIAL_MOMENTUM
  
  SREG = oldSREG;
  return val;
} 

#endif // USE_ENCODER02
// End
