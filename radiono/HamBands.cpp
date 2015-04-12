// HamBands.cpp

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
#include "debug.h"


// PROGMEM is used to avoid using the small available variable space
const unsigned long bandLimits[BANDS*2] PROGMEM = {  // Lower and Upper Band Limits
      1.80  * MHz,   2.00  * MHz, // 160m
      
      #ifndef USE_80M_SECTIONS
         3.50  * MHz,   4.00  * MHz, //  80m
      #else // USE_80M_SECTIONS
         LOWER_FREQ_80M_SECTION_01, UPPER_FREQ_80M_SECTION_01-1, // 80m
         UPPER_FREQ_80M_SECTION_01, UPPER_FREQ_80M_SECTION_02,   // 75m
      #endif // USE_80M_SECTIONS
      
      5.3305* MHz,   5.3305* MHz, //  60m Channel 1
      5.3465* MHz,   5.3465* MHz, //  60m Channel 2
      5.3570* MHz,   5.3570* MHz, //  60m Channel 3
      5.3715* MHz,   5.3715* MHz, //  60m Channel 4
      5.4035* MHz,   5.4035* MHz, //  60m Channel 5
      
      7.00  * MHz,   7.30  * MHz, //  40m
     10.10  * MHz,  10.15  * MHz, //  30m
     14.00  * MHz,  14.35  * MHz, //  20m
     18.068 * MHz,  18.168 * MHz, //  17m
     21.00  * MHz,  21.45  * MHz, //  15m
     24.89  * MHz,  24.99  * MHz, //  12m
     
     #ifndef USE_10M_SECTIONS
         28.00  * MHz,  29.70  * MHz, //  10m
     #else // USE_10M_SECTIONS
         LOWER_FREQ_10M_SECTION_01, UPPER_FREQ_10M_SECTION_01-1,
         UPPER_FREQ_10M_SECTION_01, UPPER_FREQ_10M_SECTION_02-1,
         UPPER_FREQ_10M_SECTION_02, UPPER_FREQ_10M_SECTION_03,
     #endif // USE_10M_SECTIONS
  
     #ifdef USE_OPERATE_6M
       #ifdef USE_6M_SECTIONS
           LOWER_FREQ_6M_SECTION_01, UPPER_FREQ_6M_SECTION_01-1,
           UPPER_FREQ_6M_SECTION_01, UPPER_FREQ_6M_SECTION_02,
       #else // USE_6M_SECTIONS
           50.00  * MHz,  52.0  * MHz, //  6m
       #endif // USE_6M_SECTIONS
     #endif // USE_OPERATE_6M 
   
     #ifdef USE_OPERATE_4M
         70.00  * MHz,  70.5  * MHz, //  4m
     #endif // USE_OPERATE_4M 
     
     #ifdef USE_OPERATE_2M
       #ifdef USE_2M_SECTIONS
           LOWER_FREQ_2M_SECTION_01, UPPER_FREQ_2M_SECTION_01-1,
           UPPER_FREQ_2M_SECTION_01, UPPER_FREQ_2M_SECTION_02,
       #else // USE_2M_SECTIONS
           144.00  * MHz,  146.0  * MHz, //  2m
       #endif // USE_2M_SECTIONS
     #endif // USE_OPERATE_2M  

   };

// An Array to save: A-VFO & B-VFO
unsigned long freqCache[BANDS*2] = { // Set Default Values for Cache
      1.825  * MHz,  1.825  * MHz,  // 160m - QRP SSB Calling Freq
      
      #ifdef USE_80M_SECTIONS
          3.530  * MHz,  3.530  * MHz,  //  80m - QRP CW Calling Freq ???
      #endif // USE_80M_SECTIONS
      3.985  * MHz,  3.985  * MHz,  //  75m - QRP SSB Calling Freq
      
      5.3305 * MHz,  5.3305 * MHz,  //  60m Channel 1
      5.3465 * MHz,  5.3465 * MHz,  //  60m Channel 2
      5.3570 * MHz,  5.3570 * MHz,  //  60m Channel 3
      5.3715 * MHz,  5.3715 * MHz,  //  60m Channel 4
      5.4035 * MHz,  5.4035 * MHz,  //  60m Channel 5
      
      7.285  * MHz,  7.285  * MHz,  //  40m - QRP SSB Calling Freq
     10.1387 * MHz, 10.1387 * MHz,  //  30m - QRP QRSS, WSPR and PropNET
     14.285  * MHz, 14.285  * MHz,  //  20m - QRP SSB Calling Freq
     18.130  * MHz, 18.130  * MHz,  //  17m - QRP SSB Calling Freq
     21.385  * MHz, 21.385  * MHz,  //  15m - QRP SSB Calling Freq
     24.950  * MHz, 24.950  * MHz,  //  12m - QRP SSB Calling Freq
     
     #ifdef USE_10M_SECTIONS
         (28.030  * MHz), (28.030  * MHz),
         (28.385  * MHz), (28.385  * MHz),  //  10m - QRP SSB Calling Freq
         (29.030  * MHz), (29.030  * MHz),
     #else // USE_10M_SECTIONS
         (28.385  * MHz), (28.385  * MHz),  //  10m - QRP SSB Calling Freq
     #endif // USE_10M_SECTIONS
     
            
     #ifdef USE_OPERATE_6M        
       #ifdef USE_6M_SECTIONS
           (50.20  * MHz), (50.20  * MHz),
           (51.20  * MHz), (51.20  * MHz),
       #else // USE_6M_SECTIONS
           50.20  * MHz, 50.20  * MHz,  //  6m - QRP SSB Calling Freq
       #endif // USE_10M_SECTIONS
     #endif // USE_OPERATE_6M 
         
     #ifdef USE_OPERATE_4M    
       (70.20  * MHz), (70.20  * MHz),  //  4m - QRP SSB Calling Freq
     #endif // USE_OPERATE_4M         
           
     #ifdef USE_OPERATE_2M 
       #ifdef USE_2M_SECTIONS
           (144.20  * MHz), (144.20  * MHz),
           (145.20  * MHz), (145.20  * MHz),
       #else // USE_2M_SECTIONS
           144.20  * MHz, 144.2  * MHz,  //  2m - QRP SSB Calling Freq
       #endif // USE_2M_SECTIONS
     #endif // USE_OPERATE_2M 
     
   };
   
byte sideBandModeCache[BANDS*2] = {
      AutoSB,  AutoSB, // 160m
      
      AutoSB,  AutoSB, //  80m
      #ifdef USE_80M_SECTIONS
          AutoSB,  AutoSB, //  75m
      #endif // USE_80M_SECTIONS
      
      AutoSB,  AutoSB, //  60m Channel 1
      AutoSB,  AutoSB, //  60m Channel 2
      AutoSB,  AutoSB, //  60m Channel 3
      AutoSB,  AutoSB, //  60m Channel 4
      AutoSB,  AutoSB, //  60m Channel 5
      
      AutoSB,  AutoSB, //  40m
      AutoSB,  AutoSB, //  30m
      AutoSB,  AutoSB, //  20m
      AutoSB,  AutoSB, //  17m
      AutoSB,  AutoSB, //  15m
      AutoSB,  AutoSB, //  12m
      AutoSB,  AutoSB, //  10m, and Section 1
      #ifdef USE_10M_SECTIONS
         AutoSB,  AutoSB, //  10m Section 2
         AutoSB,  AutoSB, //  10m Section 3
      #endif // USE_10M_SECTIONS
      
                 
     #ifdef USE_OPERATE_6M 
        AutoSB,  AutoSB, //  6m, and Section 1
        #ifdef USE_6M_SECTIONS
            AutoSB,  AutoSB, //  6m Section 2
        #endif // USE_6M_SECTIONS
     #endif // USE_OPERATE_6M 
                   
     #ifdef USE_OPERATE_4M 
        AutoSB,  AutoSB, //  4m
     #endif // USE_OPERATE_4M     
                  
     #ifdef USE_OPERATE_2M 
        AutoSB,  AutoSB, //  2m, and Section 1
        #ifdef USE_2M_SECTIONS
            AutoSB,  AutoSB, //  2m Section 2
        #endif // USE_2M_SECTIONS
     #endif // USE_OPERATE_2M      
      
};

int hamBands[BANDS]  = {
     160,
     
      #ifdef USE_80M_SECTIONS
          80, // 80m
          75, //  75m
      #else
          80, // 80m
      #endif // USE_80M_SECTIONS
      
      6001, // 60m Channel 1, Note: Channel Number is encoded as least digit
      6002, // 60m Channel 2
      6003, // 60m Channel 3
      6004, // 60m Channel 4
      6005, // 60m Channel 5
      
      40,
      30,
      20,
      17,
      15,
      12,
      
      #ifdef USE_10M_SECTIONS
         1001, // 10m Section 1
         1002, // 10m Section 2
         1003, // 10m Section 3
      #else
         10,
      #endif // USE_10M_SECTIONS

    
     #ifdef USE_OPERATE_6M   
        #ifdef USE_6M_SECTIONS
           601, // 6m Section 1
           602, // 6m Section 2
        #else
           6,
        #endif // USE_6M_SECTIONS
     #endif // USE_OPERATE_6M
   
    
     #ifdef USE_OPERATE_4M 
        4,
     #endif // USE_OPERATE_4M
     
     
     #ifdef USE_OPERATE_2M 
        #ifdef USE_2M_SECTIONS
           201, // 2m Section 1
           202, // 2m Section 2
        #else
           2,
        #endif // USE_2M_SECTIONS 
     #endif // USE_OPERATE_2M    
      
};

boolean operate60m = false;  // Operate on 60m Band
byte inBand = 0;


// ###############################################################################
int inBandLimits(unsigned long freq){
#define DEBUG(x...)
//#define DEBUG(x...) debugUnique(x)    // UnComment for Debug

    int upper, lower = 0;
    
       if (AltTxVFO) freq = vfos[vfoActive];
       DEBUG(P("%s %d: A,B: %lu, %lu, %lu"), __func__, __LINE__, freq, vfoA, vfoB);
            
       inBand = 0;
       for (int band = 0; band < BANDS; band++) {
         lower = band * 2;
         upper = lower + 1;
         if (freq >= pgm_read_dword(&bandLimits[lower]) &&
             freq <= pgm_read_dword(&bandLimits[upper]) ) {
             band++;
             inBand = (byte) band;
             //bandPrev = band;
             DEBUG(P("In Band %d"), band);
             return band;
             }
       }
       DEBUG(P("Out Of Band"));
       return 0;
}

// ###############################################################################
void decodeBandUpDown(int dir) {
#define DEBUG(x...)
//#define DEBUG(x...) debugUnique(x)    // UnComment for Debug
    int j;
    
    #ifdef USE_EDITIF
      if (editIfMode) return; // Do Nothing if in Edit-IF-Mode
    #endif // USE_EDITIF

    DEBUG(P("%s/%d:"), __func__, __LINE__);
    
    if (dir > 0) {  // For Band Change, Up
       for (int i = 0; i < BANDS; i++) {
         if (!operate60m) while(hamBands[i] > 6000 && hamBands[i] < 6010) i++;
         j = i*2 + vfoActive;
         if (vfos[vfoActive] <= pgm_read_dword(&bandLimits[i*2+1])) {
           if (vfos[vfoActive] >= pgm_read_dword(&bandLimits[i*2])) {
             // Save Current Ham frequency and sideBandMode
             freqCache[j] = vfos[vfoActive];
             sideBandModeCache[j] = sideBandMode;
             i++;
           }
           if (!operate60m) while(hamBands[i] > 6000 && hamBands[i] < 6010) i++;
           // Load From Next Cache Up Band
           j = i*2 + vfoActive;
           vfos[vfoActive] = freqCache[min(j,BANDS*2-1)];
           sideBandMode = sideBandModeCache[min(j,BANDS*2-1)];
           break;
         }
       }
     } // End fi
     
     else { // For Band Change, Down
       for (int i = BANDS-1; i > 0; i--) {
         if (!operate60m) while(hamBands[i] > 6000 && hamBands[i] < 6010) i--;
         j = i*2 + vfoActive;
         if (vfos[vfoActive] >= pgm_read_dword(&bandLimits[i*2])) {
           if (vfos[vfoActive] <= pgm_read_dword(&bandLimits[i*2+1])) {
             // Save Current Ham frequency and sideBandMode
             freqCache[j] = vfos[vfoActive];
             sideBandModeCache[j] = sideBandMode;
             i--;
           }
           if (!operate60m) while(hamBands[i] > 6000 && hamBands[i] < 6010) i--;
           // Load From Next Cache Down Band
           j = i*2 + vfoActive;
           vfos[vfoActive] = freqCache[max(j,vfoActive)];
           sideBandMode = sideBandModeCache[max(j,vfoActive)];
           break;
         }
       }
     } // End else
  
   freqUnStable = 100; // Set to UnStable (non-zero) Because Freq has been changed
   inBandLimits(vfos[vfoActive]);
   #ifdef USE_PARK_CURSOR
      cursorDigitPosition = 0;
   #endif // USE_PARK_CURSOR
   ritOn = ritVal = 0;
   decodeSideband();
}

// End
