// MorseCode.h

#ifndef MORSECODE_H
#define MORSECODE_H


    #define MOD_CW (0)
    #define MOD_QRSS (1)
    
    // CW Message Speed
    #define CW_WPM (13)
    
    // QRSS Message Speed
    #define QRSS_DIT_TIME (1) //  1 for Demo, Typically 3, 6, 12, 24 seconds, etc, 60 is very-very slow, negative values are ms
    #define QRSS_SHIFT (50)   // 50 for Demo, Typically 5 to 8 Hz Shift for QRSS
    
    
    void sendMorseMesg(int, char *c);
    void sendQrssMesg(long, int, char *c);

#endif

// End
