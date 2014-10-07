// HamBands.h

#ifndef HAMBANDS_H
#define HAMBANDS_H


    // The Number of Ham Bands
    #define BANDS (9+5)
    
    enum HB60mChannel {
        HB60m1 = 2,
        HB60m2,
        HB60m3,
        HB60m4,
        HB60m5,
        HB60mChannels
    };
    
    
    // Externally Available Variables    
    // PROGMEM is used to avoid using the small available variable space
    extern const unsigned long bandLimits[BANDS*2] PROGMEM;
    
    // An Array to save: A-VFO & B-VFO
    extern unsigned long freqCache[BANDS*2];
    extern byte sideBandModeCache[BANDS*2];
    extern byte inBand;
    extern byte hamBands[BANDS];
    extern boolean operate60m;
    
    // Externally Available Functions
    extern int inBandLimits(unsigned long freq);
    extern void decodeBandUpDown(int dir);

#endif

// End
