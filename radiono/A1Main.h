// A1Main.h

#ifndef A1MAIN_H
#define A1MAIN_H

    #define BANDS (9)
    
    #define BAND_HI_PIN (5)
    #define BAND_MD_PIN (6)
    #define BAND_LO_PIN (7)
    #define PA_BAND_CLK (7)

    #define DEBUG(x ...)  // Default to NO debug
    //#define DEBUG(x ...) debugUnique(x)    // UnComment for Debug
 
    extern unsigned long frequency;
    extern unsigned long iFreqUSB;
    extern unsigned long iFreqLSB;
    
    extern unsigned long vfoA, vfoB;
    extern unsigned long cwTimeout;
    extern boolean editIfMode;
    
    extern char b[], c[];  // General Buffers, used mostly for Formating message for LCD
    
    /* tuning pot stuff */
    extern byte refreshDisplay;
    
    extern int tuningDir;
    extern int tuningPosition;
    extern int tune2500Mode;
    extern int freqUnStable;
    extern int tuningPositionDelta;
    extern int cursorDigitPosition;
    extern int tuningPositionPrevious;
    extern int cursorCol, cursorRow, cursorMode;
    extern char* const sideBandText[] PROGMEM;
    extern byte sideBandMode;
    
    extern boolean tuningLocked; //the tuning can be locked: wait until Freq Stable before unlocking it
    extern boolean inTx, inPtt;
    extern boolean keyDown0;
    extern boolean isLSB;
    extern boolean vfoActive;
    
    /* modes */
    extern int ritVal;
    extern boolean ritOn;
    extern boolean AltTxVFO;
    extern boolean isAltVFO;
    
    // PROGMEM is used to avoid using the small available variable space
    extern const unsigned long bandLimits[BANDS*2] PROGMEM;
    
    // An Array to save: A-VFO & B-VFO
    extern unsigned long freqCache[BANDS*2];
    extern byte sideBandModeCache[BANDS*2];
    


    // Functions From Main    
    extern void cursorOff();
    extern void printLine1CEL(char const *);
    extern void printLine2CEL(char const *);
    extern void printLine2(char const *);
    extern void startSidetone();
    extern void stopSidetone();
    extern void changeToTransmit();
    extern void changeToReceive();
    extern int inBandLimits(unsigned long);
    extern int isKeyNowClosed();
    extern int isPttPressed();
    extern void setFreq(unsigned long);
    extern void deDounceBtnRelease();
    extern int btnDown();
    
    // ERB - Buffers that Stores "const stings" to, and Reads from FLASH Memory via P()
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
