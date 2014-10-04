// HamBands.h

#ifndef HAMBANDS_H
#define HAMBANDS_H


    // The Number of Ham Bands
    #define BANDS (9)
    
    // Externally Available Variables    
    // PROGMEM is used to avoid using the small available variable space
    extern const unsigned long bandLimits[BANDS*2] PROGMEM;
    
    // An Array to save: A-VFO & B-VFO
    extern unsigned long freqCache[BANDS*2];
    extern byte sideBandModeCache[BANDS*2];
    extern byte inBand;
    
    // Externally Available Functions
    extern int inBandLimits(unsigned long freq);
    extern void decodeBandUpDown(int dir);

#endif

// End
