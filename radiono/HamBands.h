// HamBands.h

#ifndef HAMBANDS_H
#define HAMBANDS_H

    
    // The Number of Ham Bands
    #define BANDS_BASE (9)  // 9 Standard HF Band
    #define BANDS_60M (5)   // 5 60m Freq's which are treated as bands here
    
    #ifdef USE_80M_SECTIONS
        #define LOWER_FREQ_80M_SECTION_01 (3.5 * MHz) // Bottom of Band
        #define UPPER_FREQ_80M_SECTION_01 (3.6 * MHz)
        #define UPPER_FREQ_80M_SECTION_02 (4.0 * MHz) // Top of Band
        #define ADDITIONAL_SECTIONS_IN_80M (1) // Additional 80m Sections
    #else
        #define ADDITIONAL_SECTIONS_IN_80M (0)
    #endif
    
    #ifdef USE_10M_SECTIONS
        #define LOWER_FREQ_10M_SECTION_01 (28.0 * MHz) // Bottom of Band
        #define UPPER_FREQ_10M_SECTION_01 (28.3 * MHz)
        #define UPPER_FREQ_10M_SECTION_02 (29.0 * MHz)
        #define UPPER_FREQ_10M_SECTION_03 (29.7 * MHz) // Top of Band
        #define ADDITIONAL_SECTIONS_IN_10M (2) // Additional 10m Sections
    #else
        #define ADDITIONAL_SECTIONS_IN_10M (0)
    #endif
    
    
    #define BANDS_FOR_6M (0)  // Set Default to 0
    #ifdef USE_OPERATE_6M
      #ifdef USE_6M_SECTIONS
          #define LOWER_FREQ_6M_SECTION_01 (50.0 * MHz) // Bottom of Band
          #define UPPER_FREQ_6M_SECTION_01 (51.0 * MHz)
          #define UPPER_FREQ_6M_SECTION_04 (52.0 * MHz) // Top of Band
          #define BANDS_FOR_6M (2) // Additional 6m Sections
      #else
          #define BANDS_FOR_6M (1)
      #endif 
    #endif 
    
    
    #define BANDS_FOR_4M (0)  // Set Default to 0
    #ifdef USE_OPERATE_4M
        #define BANDS_FOR_4M (1)
    #endif  
    
    
    #define BANDS_FOR_2M (0)  // Set Default to 0
    #ifdef USE_2M_SECTIONS
        #define LOWER_FREQ_2M_SECTION_01 (144.0 * MHz) // Bottom of Band
        #define UPPER_FREQ_2M_SECTION_01 (145.0 * MHz)
        #define UPPER_FREQ_2M_SECTION_04 (146.0 * MHz) // Top of Band
        #define BANDS_FOR_2M (2) // Additional 2m Sections
    #else
        #define BANDS_FOR_2M (1)
    #endif
    
    
    
    // BANDS is used many places in the code
    #define BANDS (BANDS_BASE + BANDS_60M + ADDITIONAL_SECTIONS_IN_10M + ADDITIONAL_SECTIONS_IN_80M + BANDS_FOR_6M + BANDS_FOR_4M + BANDS_FOR_2M)   
    
    
    
    // Externally Available Variables    
    // PROGMEM is used to avoid using the small available variable space
    extern const unsigned long bandLimits[BANDS*2] PROGMEM;
    
    // An Array to save: A-VFO & B-VFO
    extern unsigned long freqCache[BANDS*2];
    extern byte sideBandModeCache[BANDS*2];
    extern byte inBand;
    extern int hamBands[BANDS];
    extern boolean operate60m;
    
    // Externally Available Functions
    extern int inBandLimits(unsigned long freq);
    extern void decodeBandUpDown(int dir);

#endif

// End
