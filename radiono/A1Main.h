// A1Main.h

#ifndef A1MAIN_H
#define A1MAIN_H

    #define BANDS (9)
    
    #define BAND_HI_PIN (5)
    #define PA_BAND_CLK (7)

    #define DEBUG(x ...)  // Default to NO debug
    //#define DEBUG(x ...) debugUnique(x)    // UnComment for Debug
    
    extern char b[], c[];
    extern char buf[];
    // ERB - Force format stings into FLASH Memory
    #define  P(x) strcpy_P(buf, PSTR(x))
    // PN can be used where Multiple small (1/2, 1/4, 1/8 size) Buffers are needed.
    #define P2(x) strcpy_P(buf + sizeof(buf)/2, PSTR(x))
    #define P3(x) strcpy_P(buf + sizeof(buf)/8*5, PSTR(x))
    #define P4(x) strcpy_P(buf + sizeof(buf)/4*3, PSTR(x))
    #define P8(x) strcpy_P(buf + sizeof(buf)/8*7, PSTR(x))
    
    // Buffer Space Indexes
    // |-------|-------|-------|-------|-------|-------|-------|-------|
    // |-------------------------------|-------|-------|-------|-------|
    // P                               P       P       P       P
    //                                 2       3       4       8

#endif

// End
