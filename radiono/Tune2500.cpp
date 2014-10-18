// Tune2500.cpp

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
#include "Tune2500.h"
#include "debug.h"

int tune2500Mode = 0;

// ###############################################################################
void decodeTune2500Mode() {
    
    if (editIfMode) return; // Do Nothing if in Edit-IF-Mode   
    if (ritOn) return; // Do Nothing if in RIT Mode
    
    cursorDigitPosition = 3; // Set default Tuning Digit
    tune2500Mode = !tune2500Mode;
    knobMode = KNOB_CURSOR_MODE;
    if (tune2500Mode) frequency = (frequency / 2500) * 2500;
}

// End
