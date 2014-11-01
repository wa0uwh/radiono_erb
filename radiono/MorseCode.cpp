// MorseCode.cpp
// Send Morse Code

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
#include "HamBands.h"
#include "MorseCode.h"
#include "MorseTable.h"

// Local
byte cw_wpm = CW_WPM;
unsigned int qrssDitTime = QRSS_DIT_TIME;
unsigned int ditLen = 1200/CW_WPM; // Default Speed
byte txSpeed = 0;



// ########################################################
// ########################################################
// ########################################################


// ########################################################
void bitTimer(unsigned long timeOut) {
    
    timeOut += millis();
    while(timeOut > millis()) {
        if (isKeyNowClosed()) return; // Abort Message
        delay(20);
    }
}
// ########################################################
void sendBit(int mode, int freqShift, int mult) {
    
    switch (mode) {
    case MOD_CW:   startSidetone(); break;
    case MOD_QRSS: setFreq(vfos[vfoActive] + freqShift); break;
    }
    
    bitTimer(ditLen * mult);
    
    switch (mode) {
    case MOD_CW:   stopSidetone(); break;
    case MOD_QRSS: setFreq(vfos[vfoActive]); break;
    }
}

void dit(int mode, int freqShift) {
    sendBit(mode, freqShift, 1);
}

void dah(int mode, int freqShift) {
    sendBit(mode, freqShift, 3);
}


// ########################################################
void sendMesg(int mode, int freqShift, char *msg) {
    byte bits = 0;
    unsigned long timeOut;
    
    if (AltTxVFO) return; // Macros and Beacons not allowed in Split Mode, for now.
    if (editIfMode) return; // Do Nothing if in Edit-IF-Mode 
    #ifdef USE_HAMBANDS   
        if (!inBandLimits(vfos[vfoActive])) return; // Do nothing if TX is out-of-bounds
    #endif // USE_HAMBANDS
    if (isKeyNowClosed()) return; // Abort Message
    
    inTx = 1; 
    changeToTransmit();
    
    printLineCEL(STATUS_LINE," "); // Clear Line
    sprintf(c, P("%s"), mode == MOD_QRSS ? P8("QR"): P8("CW"));
    if (mode == MOD_QRSS && ditLen < 1000) sprintf(c+1, P(".%2.2d"), ditLen/10);
    else sprintf(c, P("%s%02.2d"), c, txSpeed);
    printLineXY(12, FIRST_LINE, c);
    delay(50);
     
    if (mode == MOD_QRSS) startSidetone();        
    printLineCEL(STATUS_LINE, msg); // Scroll Message
    bitTimer(ditLen);
    
    while(*msg) {
        if (isKeyNowClosed()) return; // Abort Message
        if (*msg == ' ') {
           msg++;
           printLineCEL(STATUS_LINE, msg); // Scroll Message on Line #2
           bitTimer(ditLen * 4); // 3 for previous character + 4 for word = 7 total
        }
        else {
            bits = pgm_read_byte(&morse[*msg & 0x7F - 32]); // Read Code from FLASH
            while(bits > 1) {
                if (isKeyNowClosed()) return; // Abort Message
                bits & 1 ? dit(mode, freqShift) : dah(mode, freqShift);
                bitTimer(ditLen);
                bits /= 2;
            }
            msg++;
            printLineCEL(STATUS_LINE, msg); // Scroll Message on Line #2
            bitTimer(ditLen * 2); // 1 + 2 added between characters
        } 
        cwTimeout = ditLen * 10 + millis();
    } // main checkTX() will clean-up and stop Transmit
}

// ########################################################
void sendQrssMesg(long len, int freqShift, char *msg) {
    
    ditLen = len; // Len is MS
    txSpeed = ditLen / 1000;
    sendMesg(MOD_QRSS, freqShift, msg);
}

// ########################################################
void sendMorseMesg(int wpm, char *msg) {
    
    ditLen = int(1200 / wpm);
    txSpeed = wpm; 
    sendMesg(MOD_CW, 0, msg);  
}

// End
  

