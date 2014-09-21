// MorseCode.h

#ifndef MORSECODE_H
#define MORSECODE_H

    enum Modes {
        MOD_CW = 0,
        MOD_QRSS,
    };
    
    // CW Message Speed
    #define CW_WPM (15)
    
    // QRSS Message Speed
    #define QRSS_DIT_TIME (250)  //  250 for Demo, Typically MS 3000, 6000, 12000, 24000 seconds, etc, 6000 is very-very slow, negative values are ms
    #define QRSS_SHIFT (50)      //   50 for Demo, Typically 5 to 8 Hz Shift for QRSS
    
    extern byte cw_wpm;
    extern unsigned int qrssDitTime;
    
    void sendMorseMesg(int wpm, char *msg);
    void sendQrssMesg(long len, int freqShift, char *msg);

#endif

// End
