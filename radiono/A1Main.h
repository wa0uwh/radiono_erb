// A1Main.h

#ifndef A1MAIN_H
#define A1MAIN_H

    // Optional USER Configurations
    //#define USE_PCA9546   1         // Define this symbol to include PCA9546 support
    //#define USE_I2C_LCD   1         // Define this symbol to include i2c LCD support
    #define USE_RF386       1       // Define this symbol to include RF386 support
    #define USE_BEACONS     1       // Define this symbol to include Beacons, CW and QRSS support
    #define USE_EEPROM      1       // Define this symbol to include Load and Store to NonVolatile Memory (EEPROM) support
    #define USE_MENUS       1       // Define this symbol to include Menu support
    #define USE_HAMBANDS    1       // Define this symbol to include Ham Band and Ham Band Limits
    //#define USE_POT_KNOB  1         // Define this symbol to include POT support
    #define USE_ENCODER01   1       // Define this symbol to include Simple Encoder01 support
    #define USE_PARK_CURSOR 1       // Define this symbol to Park Cursor when Mode Changes and/or Timeout
    //#define USE_HIDELEAST   1       // Define this symbol to Hide Least Digits to right of Cursor while Tuning

    // Set the following Conditional Compile Flags Above
    #ifdef USE_PCA9546
      #include "PCA9546.h"
    #endif
    
    #ifdef USE_HAMBANDS
      #include "HamBands.h"
    #endif // USE_HAMBANDS
    
    #ifdef USE_EEPROM
      #include "NonVol.h"
    #endif // USE_EEPROM
    
    #ifdef USE_RF386
      #include "Rf386.h"
    #endif // USE_RF386
    
    #ifdef USE_BEACONS
      #include "MorseCode.h"
      #include "Macro.h"
    #endif // USE_BEACONS
    
    #ifdef USE_POT_KNOB
      #include "PotKnob.h"
    #endif // USE_POT_KNOB
    
    #ifdef USE_MENUS
      #include "Menus.h"
    #endif // USE_MENUS
    
    #ifdef USE_ENCODER01
      #include "Encoder01.h"
    #endif // USE_ENCODER01
    
    #ifdef USE_PCA9546
      #define PCA9546_I2C_ADDRESS 0x70
    #endif // USE_PCA9546


    // Set up Units to make coding large numbers easier
    #define KILO (1000UL)
    #define MEG (KILO * KILO)
    
    #define SEC (1000UL)  // MSEC's 
    #define MIN (SEC * 60)
    #define HR  (MIN * 60)
    #define DAY (HR * 24)
    #define WK  (DAY * 7)

    #ifdef USE_PARK_CURSOR
       #define DEFAULT_BLINK_TIMEOUT (20 * SEC) // Set as desired
       #define DEFAULT_CURSOR_POSITION (0)     // Power Up Cursor Position, Park is Zero
    #else
       #define DEFAULT_BLINK_TIMEOUT (0)
       #define DEFAULT_CURSOR_POSITION (3)     // Power Up Cursor Position, Set as desired, Park is Zero
    #endif // USE_PARK_CURSOR
    
    #ifdef USE_HIDELEAST
       #define DEFAULT_BLINK_TIMEOUT (30 * MIN)
       #define DEFAULT_BLINK_RATIO (100)
       #define DEFAULT_BLINK_PERIOD (10 * SEC)
       #define DEFAULT_CURSOR_POSITION (3)     // Power Up Cursor Position, Set as desired, Park is Zero
    #endif // USE_HIDELEAST
    
    // The Number of Ham Bands
    #define BANDS (9)
    
    enum VFOs { // Available VFOs
        VFO_A = 0,
        VFO_B,
    };

    // Output Filter Control Lines
    #define BAND_HI_PIN (5)
    #define BAND_MD_PIN (6)
    #define BAND_LO_PIN (7)
    #define PA_BAND_CLK (7)
    
    // Pin Numbers for analog inputs
    #define FN_PIN (A3)
    #define ANALOG_TUNING (A2)
    #define ANALOG_KEYER (A1)


    enum LCD_DisplayLines {
        FIRST_LINE = 0,
        STATUS_LINE,
        MENU_PROMPT_LINE,
        MENU_ITEM_LINE
    };

  
    #define DEBUG(x ...)  // Default to NO debug
    //#define DEBUG(x ...) debugUnique(x)    // UnComment for Debug
 

    // Externally Available Variables
    extern unsigned long frequency;
    extern unsigned long iFreqUSB;
    extern unsigned long iFreqLSB;
    
    extern unsigned long vfoA, vfoB;
    extern unsigned long cwTimeout;
    extern boolean editIfMode;
    
    extern char b[], c[];  // General Buffers, used mostly for Formating message for LCD
    
    extern byte menuActive;
    extern byte menuPrev;
    extern byte refreshDisplay;
    extern unsigned long blinkTimeOut;
    extern int blinkPeriod;
    extern byte blinkRatio;
    extern unsigned long menuIdleTimeOut;

    /* tuning pot stuff */  
    extern int tuningDir;
    extern int knobPosition;
    extern int tune2500Mode;
    extern int freqUnStable;
    extern int knobPositionDelta;
    extern int cursorDigitPosition;
    extern int knobPositionPrevious;
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


    // Externally Available Functions
    extern void updateDisplay();   
    extern void cursorOff();
    extern void printLineXY(byte x, byte y, char const *c);
    extern void printLineCEL(int row, char const *c);
    extern void printLine(int row, char const *c);
    extern void startSidetone();
    extern void stopSidetone();
    extern void decodeSideband();
    extern void changeToTransmit();
    extern void changeToReceive();
    extern  int isKeyNowClosed();
    extern  int isPttPressed();
    extern void setFreq(unsigned long freq);
    
    // ERB - Buffers that Stores "const stings" to, and Reads from FLASH Memory via P()
    #define PBUFSIZE (66)
    extern char buf[PBUFSIZE];
    
    // ERB - Force format stings into FLASH Memory
    #define  P(x) strcpy_P(buf, PSTR(x))
    // PN can be used where Multiple small (1/2, 1/4, 1/8 size) Buffers are needed.
    #define P2(x) strcpy_P(buf + PBUFSIZE/2,   PSTR(x))
    #define P3(x) strcpy_P(buf + PBUFSIZE/8*5, PSTR(x))
    #define P4(x) strcpy_P(buf + PBUFSIZE/4*3, PSTR(x))
    #define P8(x) strcpy_P(buf + PBUFSIZE/8*7, PSTR(x))
    
    // Buffer Space Indexes
    // |-------|-------|-------|-------|-------|-------|-------|-------|
    // |-------------------------------|-------|-------|-------|-------|
    // P                               P       P       P       P
    //                                 2       3       4       8

#endif

// End
