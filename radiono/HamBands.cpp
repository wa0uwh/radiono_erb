// HamBands.cpp


#include <Arduino.h>
#include "A1Main.h"
#include "HamBands.h"
#include "debug.h"


// PROGMEM is used to avoid using the small available variable space
const unsigned long bandLimits[BANDS*2] PROGMEM = {  // Lower and Upper Band Limits
      1.80  * MHz,   2.00  * MHz, // 160m
      3.50  * MHz,   4.00  * MHz, //  80m
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
     28.00  * MHz,  29.70  * MHz, //  10m
   //50.00  * MHz,  54.00  * MHz, //   6m - Will need New Low Pass Filter Support
   };

// An Array to save: A-VFO & B-VFO
unsigned long freqCache[BANDS*2] = { // Set Default Values for Cache
      1.825  * MHz,  1.825  * MHz,  // 160m - QRP SSB Calling Freq
      3.985  * MHz,  3.985  * MHz,  //  80m - QRP SSB Calling Freq
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
     28.385  * MHz, 28.385  * MHz,  //  10m - QRP SSB Calling Freq
   //50.20   * MHz, 50.20   * MHz,  //   6m - QRP SSB Calling Freq
   };
   
byte sideBandModeCache[BANDS*2] = {
      LSB,   LSB, // 160m
      LSB,   LSB, //  80m
      USB,   USB, //  60m Channel 1
      USB,   USB, //  60m Channel 2
      USB,   USB, //  60m Channel 3
      USB,   USB, //  60m Channel 4
      USB,   USB, //  60m Channel 5
      LSB,   LSB, //  40m
      USB,   USB, //  30m
      USB,   USB, //  20m
      USB,   USB, //  17m
      USB,   USB, //  15m
      USB,   USB, //  12m
      USB,   USB, //  10m
   // USB,   USB, //   6m - Will need New Low Pass Filter Support
};

byte hamBands[BANDS]  = {
     160,
      80,
      61, // 60m Channel 1, Note: Channel Number is encoded as least digit
      62, // 60m Channel 2
      63, // 60m Channel 3
      64, // 60m Channel 4
      65, // 60m Channel 5
      40,
      30,
      20,
      17,
      15,
      12,
      10,
   //  6, // Will need New Low Pass Filter Support
};

boolean operate60m = false;  // Operate on 60m Band
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
#define DEBUG(x...)
//#define DEBUG(x...) debugUnique(x)    // UnComment for Debug
    int j;
    
    #ifdef USE_EDITIF
      if (editIfMode) return; // Do Nothing if in Edit-IF-Mode
    #endif // USE_EDITIF

    DEBUG(P("%s/%d:"), __func__, __LINE__);
    
    if (dir > 0) {  // For Band Change, Up
       for (int i = 0; i < BANDS; i++) {
         if (!operate60m) while(i >= HB60m1 && i <= HB60m5) i++;
         j = i*2 + vfoActive;
         if (frequency <= pgm_read_dword(&bandLimits[i*2+1])) {
           if (frequency >= pgm_read_dword(&bandLimits[i*2])) {
             // Save Current Ham frequency and sideBandMode
             freqCache[j] = frequency;
             sideBandModeCache[j] = sideBandMode;
             i++;
           }
           if (!operate60m) while(i >= HB60m1 && i <= HB60m5) i++;
           // Load From Next Cache Up Band
           j = i*2 + vfoActive;
           frequency = freqCache[min(j,BANDS*2-1)];
           sideBandMode = sideBandModeCache[min(j,BANDS*2-1)];
           vfoActive == VFO_A ? vfoA = frequency : vfoB = frequency;
           break;
         }
       }
     } // End fi
     
     else { // For Band Change, Down
       for (int i = BANDS-1; i > 0; i--) {
         if (!operate60m) while(i >= HB60m1 && i <= HB60m5) i--;
         j = i*2 + vfoActive;
         if (frequency >= pgm_read_dword(&bandLimits[i*2])) {
           if (frequency <= pgm_read_dword(&bandLimits[i*2+1])) {
             // Save Current Ham frequency and sideBandMode
             freqCache[j] = frequency;
             sideBandModeCache[j] = sideBandMode;
             i--;
           }
           if (!operate60m) while(i >= HB60m1 && i <= HB60m5) i--;
           // Load From Next Cache Down Band
           j = i*2 + vfoActive;
           frequency = freqCache[max(j,vfoActive)];
           sideBandMode = sideBandModeCache[max(j,vfoActive)];
           vfoActive == VFO_A ? vfoA = frequency : vfoB = frequency;
           break;
         }
       }
     } // End else
  
   freqUnStable = 100; // Set to UnStable (non-zero) Because Freq has been changed
   inBandLimits(frequency);
   #ifdef USE_PARK_CURSOR
      cursorDigitPosition = 0;
   #endif // USE_PARK_CURSOR
   ritOn = ritVal = 0;
   decodeSideband();
}

// End
