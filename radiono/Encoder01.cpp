// Encoder01.cpp

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

#ifdef USE_ENCODER01

#include "Encoder01.h"
#include "ButtonUtil.h"
#include "debug.h"

static char knob;


// ###############################################################################
// ###############################################################################
void initEncoder() {
     // NoOp
}


// ###############################################################################
int getEncoderDir() {
    char tmp = knob;
      
    if (tmp>0) { knob--; return +1;}
    if (tmp<0) { knob++; return -1;}
    return 0;
}

// ###############################################################################
void readEncoder(int btn) {
    static unsigned long startTime = 0;
    unsigned long tigermillis;
    
    tigermillis = millis();
    
    if (tigermillis-startTime <= ISR_DEBOUNCE_TIMEOUT) return;
    startTime=tigermillis;
 
    knob += analogRead(ANALOG_TUNING) > 50 ? -1 : +1;
    
    //debug("%s/%d: Knob= %d", __func__, __LINE__, knob);
    return;
}

#endif // USE_ENCODER01
// End
