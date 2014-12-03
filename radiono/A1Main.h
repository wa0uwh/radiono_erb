// A1Main.h

#ifndef A1MAIN_H
#define A1MAIN_H

    // Aliases for ATMEGA328P that work with/for both the PreCompiler and Runtime
    // This is a bit silly, but these make coding and Options much more User Friendly.  
    #define PD0 (0)
    #define PD1 (1)
    #define PD2 (2)
    #define PD3 (3)
    #define PD4 (4)
    #define PD5 (5)
    #define PD6 (6)
    #define PD7 (7)
    
    #define PB0 (8)
    #define PB1 (9)
    #define PB2 (10)
    #define PB3 (11)
    #define PB4 (12)
    #define PB5 (13)
    
    #define PC0 (14)
    #define PC1 (15)
    #define PC2 (16)
    #define PC3 (17)
    #define PC4 (18)
    #define PC5 (19)
    
    // Aliases of Aliases ?!
    #define A0 (PC0)
    #define A1 (PC1)
    #define A2 (PC2)
    #define A3 (PC3)
    #define A4 (PC4)
    #define A5 (PC5)
    
    #define SI  (PD0)
    #define SO  (PD1)
    #define SDA (PC4)
    #define SCL (PC5)


    #include "A1Config.h"
    
    // Set up Units to make coding large Frequency Numbers easier
    #define KILO  (1000UL)
    #define MEG  (KILO * KILO)
    #define GIG  (KILO *  MEG)
    
    // Parts Per ...
    #define PPM    (MEG)
    #define PP100M (100 * MEG)
    #define PPB    (GIG)
    
    // Frequencies in Hertz
    #define Hz   (1UL)
    #define KHz  (KILO * Hz)
    #define MHz  (MEG  * Hz)
    #define GHz  (GIG  * Hz)
    
    // Set up Units for Timers
    #define MSECs  (1UL) // Milli Seconds
    #define SECs  (1000 * MSECs) 
    #define MINs  (60   * SECs)
    #define HRs   (60   * MINs)
    #define DAYs  (24   * HRs)
    #define WKs   (7    * DAYs)

    enum Sideband { // Sidebands
        USB = 0,
        LSB,
        AutoSB
    };
    
    enum KnobModes {
        KNOB_CURSOR_MODE = 0,
        KNOB_DIGIT_MODE,
        #ifdef USE_KNOB_CAN_CHANGE_BANDS
            KNOB_BAND_MODE,
        #endif // USE_KNOB_CAN_CHANGE_BANDS
        KNOB_MODES
    };
 
    // Pin Numbers for analog inputs
    #define FN_PIN        (A3)
    #define ANALOG_TUNING (A2)
    #define ANALOG_KEYER  (A1)

    // Pin Number for the digital output controls
    #define LSB_PIN      (PD2) // SideBand Control
  //#define TR_PIN       (PD3) // Minima 1.0, TR Relay Control
  //#define SIDETONE_PIN (PD4) // Minima 1.0, SideTone Enable
    #define TR_PIN       (PD4) // Minima 2.0, TR Relay Control
    #define SIDETONE_PIN (PD3) // Minima 2.0, Can be used for SideTone DDS Output, NOTE: DDS Requires PD3 or PD5
    #define BAND_HI_PIN  (PD5) // Selects LP Filter
    
    // Originally these to pin were allocated for control of a PA Filters, now used for Encoder
  //#define BAND_MD_PIN  (PD6)
  //#define BAND_LO_PIN  (PD7) 
    
    // New - Pin for single wire Rf386 PA Filter Selector
    #define PA_BAND_CLK  (PC0)
      
    // Set the following Conditional Compile Flags Above
    #ifdef USE_OPERATE_60M
      #define USE_HAMBANDS 1
    #endif

    #ifdef USE_Si570_BFO
       #define USE_PCA9546 1
    #endif
    
    #ifdef USE_PCA9546
      #include "PCA9546.h"
    #endif // USE_PCA9546
    
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
        
    #ifdef USE_AUTOSCANNER
      #include "Scanner.h"
    #endif // USE_AUTOSCANNER 
    
    #ifdef USE_DIAL_CAL
      #include "DialCal.h"
    #endif // USE_DIAL_CAL
    
    #ifdef USE_ENCODER01
      #define USE_ENCODER 1
      #define ENC_A_PIN (ANALOG_TUNING)
      #define ENC_B_PIN (FN_PIN)
      #define ISR_DEBOUNCE_TIMEOUT (5 * MSECs)
      #include "Encoder01.h"
    #endif // USE_ENCODER01
     
    #ifdef USE_ENCODER02
      #define USE_ENCODER 2
      #define ENC_A_PIN (PD6)
      #define ENC_B_PIN (PD7)
      #include "Encoder02.h"
      #define ISR_DEBOUNCE_TIMEOUT (5 * MSECs)
    #endif // USE_ENCODER02
     
    #ifdef USE_ENCODER03
      #define USE_ENCODER 3
      #define ENC_A_PIN (ANALOG_TUNING)
      #define ENC_B_PIN (FN_PIN)
      #include "Encoder03.h"
      #define ISR_DEBOUNCE_TIMEOUT (5 * MSECs)
    #endif // USE_ENCODER03
    
    #ifdef USE_ENCODER04
      #define USE_ENCODER 4
      #define ENC_A_PIN (PD6)
      #define ENC_B_PIN (PD7)
      #include "Encoder04.h"
    #endif // USE_ENCODER04
       
    #ifdef USE_EDITIF
      #include "EditIF.h"
    #endif // USE_EDITIF
    
    #ifdef USE_PCA9546
      #define PCA9546_I2C_ADDRESS 0x70
    #endif // USE_PCA9546
    
    #ifdef USE_TUNE2500_MODE
      #include "Tune2500.h"
    #endif // USE_TUNE2500_MODE
   
    #ifdef USE_DDS_SIDETONE
      #include "DDS.h"
    #endif // USE_DDS_SIDETONE


    #ifdef USE_PARK_CURSOR
       #define DEFAULT_PARK_TIMEOUT (20 * SECs) // Set as desired
       #define DEFAULT_CURSOR_POSITION (0)       // Power Up Cursor Position, Park is Zero
    #else
       #define DEFAULT_PARK_TIMEOUT (4 * HRs)   // Set to Zero for Never
       #define DEFAULT_CURSOR_POSITION (3)       // Power Up Cursor Position, Set as desired, Park is Zero
    #endif // USE_PARK_CURSOR
    
    #ifdef USE_HIDELEAST
       #define DEFAULT_PARK_TIMEOUT (30 * MINs)
       #define DEFAULT_BLINK_RATIO (100)
       #define DEFAULT_BLINK_PERIOD (10 * SECs)
       #define DEFAULT_CURSOR_POSITION (3)       // Power Up Cursor Position, Set as desired, Park is Zero
    #endif // USE_HIDELEAST
    
    
    enum VFOs { // Available VFOs
        VFO_A = 0,
        VFO_B,
        VFO_C,
        VFO_D,
        VFO_U,
        VFO_L,
        VFO_S,
        MAXVFOS
    };

    enum LCD_DisplayLines {
        FIRST_LINE = 0,
        STATUS_LINE,
        MENU_PROMPT_LINE,
        MENU_ITEM_LINE
    };

  
    #define DEBUG(x ...)  // Default to NO debug
    //#define DEBUG(x ...) debugUnique(x)    // UnComment for Debug
 

    // Externally Available Variables
    extern unsigned long vfos[];
    
    extern unsigned long cwTimeout;
    
    extern char b[], c[];  // General Buffers, used mostly for Formating message for LCD
    
    //extern byte menuActive;
    //extern byte menuPrev;
    extern byte refreshDisplay;
    extern unsigned long parkTimeOut;
    extern int blinkPeriod;
    extern byte blinkRatio;
    extern unsigned long menuIdleTimeOut;

    /* tuning pot stuff */  
    extern int tuningDir;
    extern int knobPosition;
    extern int freqUnStable;
    extern int knobPositionDelta;
    extern int cursorDigitPosition;
    extern int knobPositionPrevious;
    //extern int cursorCol, cursorRow, cursorMode;
    //extern char* const sideBandText[] PROGMEM;
    extern byte sideBandMode;
    
    extern boolean tuningLocked; //the tuning can be locked: wait until Freq Stable before unlocking it
    extern byte knobMode;
    extern boolean inTx, inPtt;
    extern boolean keyDown0;
    extern boolean isLSB;
    extern boolean vfoActive;
    
    /* modes */
    extern int ritVal;
    extern boolean ritOn;
    extern boolean AltTxVFO;
    //extern boolean isAltVFO;
    
    // Dial Calibration
    extern byte vfoDialCalStash;
    extern boolean dialCalEditMode;
    extern long dialCalPP100M;

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
    extern void dialCalEditModeCancel();
    
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
