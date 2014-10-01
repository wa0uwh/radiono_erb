// HamBands.cpp


#include <Arduino.h>
#include "A1Main.h"
#include "HamBands.h"
#include "debug.h"


// PROGMEM is used to avoid using the small available variable space
const unsigned long bandLimits[BANDS*2] PROGMEM = {  // Lower and Upper Band Limits
      1.80  * MEG,   2.00  * MEG, // 160m
      3.50  * MEG,   4.00  * MEG, //  80m
      7.00  * MEG,   7.30  * MEG, //  40m
     10.10  * MEG,  10.15  * MEG, //  30m
     14.00  * MEG,  14.35  * MEG, //  20m
     18.068 * MEG,  18.168 * MEG, //  17m
     21.00  * MEG,  21.45  * MEG, //  15m
     24.89  * MEG,  24.99  * MEG, //  12m
     28.00  * MEG,  29.70  * MEG, //  10m
   //50.00  * MEG,  54.00  * MEG, //   6m - Will need New Low Pass Filter Support
   };

// An Array to save: A-VFO & B-VFO
unsigned long freqCache[BANDS*2] = { // Set Default Values for Cache
      1.825  * MEG,  1.825  * MEG,  // 160m - QRP SSB Calling Freq
      3.985  * MEG,  3.985  * MEG,  //  80m - QRP SSB Calling Freq
      7.285  * MEG,  7.285  * MEG,  //  40m - QRP SSB Calling Freq
     10.1387 * MEG, 10.1387 * MEG,  //  30m - QRP QRSS, WSPR and PropNET
     14.285  * MEG, 14.285  * MEG,  //  20m - QRP SSB Calling Freq
     18.130  * MEG, 18.130  * MEG,  //  17m - QRP SSB Calling Freq
     21.385  * MEG, 21.385  * MEG,  //  15m - QRP SSB Calling Freq
     24.950  * MEG, 24.950  * MEG,  //  12m - QRP SSB Calling Freq
     28.385  * MEG, 28.385  * MEG,  //  10m - QRP SSB Calling Freq
   //50.20   * MEG, 50.20   * MEG,  //   6m - QRP SSB Calling Freq
   };
byte sideBandModeCache[BANDS*2] = {0};


byte inBand = 0;


// ###############################################################################
int inBandLimits(unsigned long freq){
#define DEBUG(x...)
//#define DEBUG(x...) debugUnique(x)    // UnComment for Debug

    int upper, lower = 0;
    
       if (AltTxVFO) freq = (vfoActive == VFO_A) ? vfoB : vfoA;
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
    int j;
    
   if (editIfMode) return; // Do Nothing if in Edit-IF-Mode
    
    if(dir > 0) {  // For Band Change, Up
       for (int i = 0; i < BANDS; i++) {
         j = i*2 + vfoActive;
         if (frequency <= pgm_read_dword(&bandLimits[i*2+1])) {
           if (frequency >= pgm_read_dword(&bandLimits[i*2])) {
             // Save Current Ham frequency and sideBandMode
             freqCache[j] = frequency;
             sideBandModeCache[j] = sideBandMode;
           }
           // Load From Next Cache Up Band
           j += 2;
           frequency = freqCache[min(j,BANDS*2-1)];
           sideBandMode = sideBandModeCache[min(j,BANDS*2-1)];
           vfoActive == VFO_A ? vfoA = frequency : vfoB = frequency;
           break;
         }
       }
     } // End fi
     
     else { // For Band Change, Down
       for (int i = BANDS-1; i > 0; i--) {
         j = i*2 + vfoActive;
         if (frequency >= pgm_read_dword(&bandLimits[i*2])) {
           if (frequency <= pgm_read_dword(&bandLimits[i*2+1])) {
             // Save Current Ham frequency and sideBandMode
             freqCache[j] = frequency;
             sideBandModeCache[j] = sideBandMode;
           }
           // Load From Next Cache Down Band
           j -= 2;
           frequency = freqCache[max(j,vfoActive)];
           sideBandMode = sideBandModeCache[max(j,vfoActive)];
           vfoActive == VFO_A ? vfoA = frequency : vfoB = frequency;
           break;
         }
       }
     } // End else
     
   freqUnStable = 100; // Set to UnStable (non-zero) Because Freq has been changed
   inBandLimits(frequency);
   cursorDigitPosition = 0; 
   ritOn = ritVal = 0;
   decodeSideband();
}

// End
