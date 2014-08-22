/*
 * Radiono - The Minima's Main Arduino Sketch
 * Copyright (C) 2013 Ashar Farhan
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of  MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 *
 * Modified:
 * An Alternate Tuning Method by: Eldon R. Brown (ERB) - WA0UWH, Apr 25, 2014
 *
 */

void setup(); // #fix Hack

//#define RADIONO_VERSION "0.4"
#define RADIONO_VERSION "0.4.erb" // Modifications by: Eldon R. Brown - WA0UWH
#define INC_REV "EC"              // Incremental Rev Code


/*
 * Wire is only used from the Si570 module but we need to list it here so that
 * the Arduino environment knows we need it.
 */
#include <Wire.h>
#include <LiquidCrystal.h>

#define LCD_COL (16)
#define LCD_ROW (2)
#define LCD_STR_CEL "%-16.16s"
//#define LCD_STR_CEL "%-20.20s"  // For 20 Character LCD Display


#include <avr/pgmspace.h>
#include <avr/io.h>
#include "Si570.h"
#include "debug.h"
#include "Rf386.h"

#include <avr/eeprom.h>


/*
 The 16x2 LCD is connected as follows:
    LCD's PIN   Raduino's PIN  PURPOSE      ATMEGA328's PIN
    4           13             Reset LCD    19
    6           12             Enable       18
    11          10             D4           17
    12          11             D5           16
    13           9             D6           15
    14           8             D7           14
*/

#define SI570_I2C_ADDRESS 0x55
//#define IF_FREQ   (0)  // FOR debug ONLY
#define IF_FREQ   (19997000L) // this is for usb, we should probably have the USB and LSB frequencies separately
#define SB_OFFSET (1200L)     // this is used as a +/- Value for Sideband Offset

#define CW_TIMEOUT (600L) // in milliseconds, this is the parameter that determines how long the tx will hold between cw key downs

#define MAX_FREQ (30000000UL)

#define DEAD_ZONE (40)

#define CURSOR_MODE (1)
#define DIGIT_MODE (2)

#define NO_CURSOR (0)
#define UNDERLINE (1)
#define UNDERLINE_ WINK (2)
#define BLOCK_BLINK (3)

#define MOMENTARY_PRESS (1)
#define DOUBLE_PRESS (2)
#define LONG_PRESS (3)
#define ALT_PRESS (4)

#define FN_BTN (1)
#define LEFT_BTN (2)
#define RIGHT_BTN (3)

#define AUTO_SIDEBAND_MODE (0)
#define UPPER_SIDEBAND_MODE (1)
#define LOWER_SIDEBAND_MODE (2)

/* the digital controls */
#define LSB (2)
#define TX_RX (3)
#define CW_KEY (4)

#define BAND_HI (5)
#define PA_BAND_CLK (7)

#define FN_PIN (A3)
#define ANALOG_TUNING (A2)
#define ANALOG_KEYER (A1)

#define VFO_A (0)
#define VFO_B (1)

#define ID_FLAG (1408141747L)  // YYMMDDHHMM, Used for EEPROM Structure Revision Flag
    
#define EEP_LOAD (0)
#define EEP_SAVE (1)


Si570 *vfo;
LiquidCrystal lcd(13, 12, 11, 10, 9, 8);

unsigned long frequency = 14285000UL; //  20m - QRP SSB Calling Freq
unsigned long iFreq = IF_FREQ;
int dialFreqCalUSB = 0;
int dialFreqCalLSB = 0;

unsigned long vfoA = frequency, vfoB = frequency;
unsigned long cwTimeout = 0, pttTimeout = 0;
int editIfMode = false;

char b[LCD_COL+6], c[LCD_COL+6];  // General Buffers, used mostly for Formating message for LCD

/* tuning pot stuff */
byte refreshDisplay = 0;

int tuningDir = 0;
int tuningPosition = 0;
int tune2500Mode = 0;
int freqUnStable = 1;
int tuningPositionDelta = 0;
int cursorDigitPosition = 0;
int tuningPositionPrevious = 0;
int cursorCol, cursorRow, cursorMode;
int winkOn;
char* const sideBandText[] PROGMEM = {"Auto SB","USB","LSB"};
byte sideBandMode = 0;

byte tuningLocked = 0; //the tuning can be locked: wait until Freq Stable before unlocking it
byte inTx = 0, inPtt = 0;
byte keyDown = 0;
byte isLSB = 0;
byte vfoActive = VFO_A;

/* modes */
byte ritOn = 0;
int ritVal = 0;
byte AltTxVFO = 0;
byte isAltVFO = 0;

// PROGMEM is used to avoid using the small available variable space
const prog_uint32_t bandLimits[] PROGMEM = {  // Lower and Upper Band Limits
      1800000UL,  2000000UL, // 160m
      3500000UL,  4000000UL, //  80m
      7000000UL,  7300000UL, //  40m
     10100000UL, 10150000UL, //  30m
     14000000UL, 14350000UL, //  20m
     18068000UL, 18168000UL, //  17m
     21000000UL, 21450000UL, //  15m
     24890000UL, 24990000UL, //  12m
     28000000UL, 29700000UL, //  10m
   //50000000UL, 54000000UL, //   6m - Will need New Low Pass Filter Support
   };
   
#define BANDS (sizeof(bandLimits)/sizeof(prog_uint32_t)/2)

long idFlag = ID_FLAG;

// An Array to save: A-VFO & B-VFO
unsigned long freqCache[] = { // Set Default Values for Cache
      1825000UL, 1825000UL,  // 160m - QRP SSB Calling Freq
      3985000UL, 3985000UL,  //  80m - QRP SSB Calling Freq
      7285000UL, 7285000UL,  //  40m - QRP SSB Calling Freq
     10138700UL, 10138700UL, //  30m - QRP QRSS and WSPR
     14285000UL, 14285000UL, //  20m - QRP SSB Calling Freq
     18130000UL, 18130000UL, //  17m - QRP SSB Calling Freq
     21385000UL, 21385000UL, //  15m - QRP SSB Calling Freq
     24950000UL, 24950000UL, //  12m - QRP SSB Calling Freq
     28385000UL, 28385000UL, //  10m - QRP SSB Calling Freq
   //50200000UL, 50200000UL, //   6m - QRP SSB Calling Freq
   };
byte sideBandModeCache[BANDS*2] = {0};


// ERB - Buffers that Stores "const stings" to, and Reads from FLASH Memory
char buf[64+2];
// ERB - Force format stings into FLASH Memory
#define  P(x) strcpy_P(buf, PSTR(x))
// FLASH2 can be used where Two small (1/2 size) Buffers are needed.
#define P2(x) strcpy_P(buf + sizeof(buf)/2, PSTR(x))

#define DEBUG(x ...)  // Default to NO debug

// ###############################################################################
// ###############################################################################
// ###############################################################################
// ###############################################################################

// ###############################################################################
/* display routines */
void printLine1(char const *c){
    lcd.setCursor(0, 0);
    lcd.print(c);
}

void printLine2(char const *c){
    lcd.setCursor(0, 1);
    lcd.print(c);
}

// Print LCD Line1 with Clear to End of Line
void printLine1CEL(char const *c){
    char lbuf[LCD_COL+2];
    sprintf(lbuf, LCD_STR_CEL, c);
    printLine1(lbuf);
}

// Print LCD Line2 with Clear to End of Line
void printLine2CEL(char const *c){
    char lbuf[LCD_COL+2];
    sprintf(lbuf, LCD_STR_CEL, c);
    printLine2(lbuf);
}


// ###############################################################################
void updateDisplay(){
  char const *vfoStatus[] = { "ERR", "RDY", "BIG", "SML" };
  char d[6]; // Buffer for RIT Display Value
  
  if (refreshDisplay) {
      refreshDisplay = false;
      cursorOff();
        
      // Top Line of LCD
      sprintf(d, P("%+03.3d"), ritVal);  
      sprintf(b, P("%08ld"), frequency);
      sprintf(c, P("%1s:%.2s.%.6s%4.4s%s"),
          editIfMode ? "I" : (vfoActive == VFO_A ? (AltTxVFO ? "a": "A") : (AltTxVFO ? "b" : "B")) ,
          b,  b+2,
          inTx ? " " : ritOn ? d : " ",
          tune2500Mode ? "*": " "
          );
      printLine1CEL(c);
      
      sprintf(c, P("%3s%1s %-3s %3.3s"),
          isLSB ? "LSB" : "USB",
          sideBandMode > 0 ? "*" : " ",
          inTx ? (inPtt ? "PTT" : "CW") : "RX",
          freqUnStable ? " " : vfoStatus[vfo->status]
          );
      printLine2CEL(c);
      
      setCursorCRM(11 - (cursorDigitPosition + (cursorDigitPosition>6) ), 0, CURSOR_MODE);
  }
  updateCursor();
}


// -------------------------------------------------------------------------------
void setCursorCRM(int col, int row, int mode) {
  // mode 0 = underline
  // mode 1 = underline wink
  // move 2 = block blink
  // else   = no cursor
  cursorCol = col;
  cursorRow = row;
  cursorMode = mode;
}


// -------------------------------------------------------------------------------
void cursorOff() {
  lcd.noBlink();
  lcd.noCursor();
}


// -------------------------------------------------------------------------------
void updateCursor() {
  
  lcd.setCursor(cursorCol, cursorRow); // Postion Curesor
  
  // Set Cursor Display Mode, Wink On and OFF for DigitMode, Solid for CursorMode
  if (cursorMode == CURSOR_MODE) {
      if (millis() & 0x0200 ) { // Update only once in a while
        lcd.noBlink();
        lcd.cursor();
      }
  }
  else if (cursorMode == DIGIT_MODE) { // Winks Underline Cursor
      if (millis() & 0x0200 ) { // Update only once in a while
        if (winkOn == false) {
          lcd.cursor();
          winkOn = true;
        }
      } else {
        if (winkOn == true) {
          lcd.noCursor();
          winkOn = false;
        }
      }
  }
  else if (cursorMode == BLOCK_BLINK) {
      if (millis() & 0x0200 ) { // Update only once in a while
        lcd.blink();
        lcd.noCursor();
      }
  }
  else {
      if (millis() & 0x0200 ) { // Update only once in a while
        cursorOff();
      }
  }
 
}


// ###############################################################################
void setSideband(){
  
  switch(sideBandMode) {
    case AUTO_SIDEBAND_MODE: // Automatic Side Band Mode
      isLSB = (frequency < 10000000UL) ? 1 : 0 ; break;
    case UPPER_SIDEBAND_MODE: // Force USB Mode
      isLSB = 0; break;
    case LOWER_SIDEBAND_MODE: // Force LSB Mode
      isLSB = 1; break;
  } 
  digitalWrite(LSB, isLSB);
}

 
// ###############################################################################
void setBandswitch(unsigned long freq){
  
  if (freq >= 15000000UL) digitalWrite(BAND_HI, 1);
  else digitalWrite(BAND_HI, 0);
}


// ###############################################################################
void readTuningPot(){
    tuningPosition = analogRead(ANALOG_TUNING);
}


// ###############################################################################
// An Alternate Tuning Strategy or Method
// This method somewhat emulates a normal Radio Tuning Dial
// Tuning Position by Switches on FN Circuit
// Author: Eldon R. Brown - WA0UWH, Apr 25, 2014
void checkTuning() {
  long deltaFreq;
  unsigned long newFreq;

  // Count Down to Freq Stable, i.e. Freq has not changed recently
  if (freqUnStable == 1) refreshDisplay = true;
  freqUnStable = max(--freqUnStable, 0);
  
  // Do Not Change Freq while in Transmit or button opperation
  // Allow Tuning knob to be recentered without changing Frequency
  if (tuningLocked) {
      tuningPositionPrevious = tuningPosition;
      return;
  }
  
  // Compute tuningDaltaPosition from tuningPosition
  tuningPositionDelta = tuningPosition - tuningPositionPrevious;
  
  tuningDir = 0;  // Set Default Tuning Directon to neather Right nor Left

  // Check to see if Automatic Digit Change Action is Required, if SO, force the change
  if (tuningPosition < DEAD_ZONE * 2) { // We must be at the Low end of the Tuning POT
    tuningPositionDelta = -DEAD_ZONE;
    delay(100);
    if (tuningPosition > DEAD_ZONE ) delay(100);
  }
  if (tuningPosition > 1023 - DEAD_ZONE * 2) { // We must be at the High end of the Tuning POT
    tuningPositionDelta = DEAD_ZONE; 
    delay(100);
    if (tuningPosition < 1023 - DEAD_ZONE / 8) delay(100);
  }

  // Check to see if Digit Change Action is Required, Otherwise Do Nothing via RETURN 
  if (abs(tuningPositionDelta) < DEAD_ZONE) return;

  tuningDir = tuningPositionDelta < 0 ? -1 : tuningPositionDelta > 0 ? +1 : 0;  
  if (!tuningDir) return;  // If Neather Direction, Abort
  
  // Decode and implement RIT Tuning
  if (ritOn) {
      ritVal += tuningDir * 10;
      ritVal = max(ritVal, -990);
      ritVal = min(ritVal, +990);
      tuningPositionPrevious = tuningPosition; // Set up for the next Iteration
      refreshDisplay++;
      updateDisplay();
      return;
  }
  
  if (cursorDigitPosition < 1) return; // Nothing to do here, Abort, Cursor is in Park position

  // Select Tuning Mode; Digit or 2500 Step Mode
  if (tune2500Mode) {
      // Inc or Dec Freq by 2.5K, useful when tuning between SSB stations
      cursorDigitPosition = 3;
      deltaFreq += tuningDir * 2500;
      
      newFreq = (frequency / 2500) * 2500 + deltaFreq;
  }
  else {
      // Compute deltaFreq based on current Cursor Position Digit
      deltaFreq = tuningDir;
      for (int i = cursorDigitPosition; i > 1; i-- ) deltaFreq *= 10;
  
      newFreq = frequency + deltaFreq;  // Save Least Digits Mode
  }
  
  //newFreq = (frequency / abs(deltaFreq)) * abs(deltaFreq) + deltaFreq; // Zero Lesser Digits Mode 
  if (newFreq != frequency) {
      // Update frequency if within range of limits, 
      // Avoiding Nagative underRoll of UnSigned Long, and over-run MAX_FREQ  
      if (newFreq <= MAX_FREQ) {
        frequency = newFreq;
        vfoActive == VFO_A ? vfoA = frequency : vfoB = frequency;  
        refreshDisplay++;
      }
      freqUnStable = 25; // Set to UnStable (non-zero) Because Freq has been changed
      tuningPositionPrevious = tuningPosition; // Set up for the next Iteration
  }
}


// ###############################################################################
int inBandLimits(unsigned long freq){
#define DEBUG(x...)
//#define DEBUG(x...) debugUnique(x)    // UnComment for Debug
    static unsigned long freqPrev = 0;
    static byte bandPrev = 0;
    int upper, lower = 0;
    
       if (AltTxVFO) freq = (vfoActive == VFO_A) ? vfoB : vfoA;
       DEBUG(P("%s %d: A,B: %lu, %lu, %lu"), __func__, __LINE__, freq, vfoA, vfoB);
       
       //if (freq == freqPrev) return bandPrev;
       freqPrev = freq;
       
       for (int band = 0; band < BANDS; band++) {
         lower = band * 2;
         upper = lower + 1;
         if (freq >= pgm_read_dword(&bandLimits[lower]) &&
             freq <= pgm_read_dword(&bandLimits[upper]) ) {
             bandPrev = ++band;
             DEBUG(P("In Band %d"), band);
             return bandPrev;
             }
       }
       DEBUG(P("Out Of Band"));
       return 0;
}

  
// ###############################################################################
void toggleAltVfo(int mode) {
#define DEBUG(x...)
//#define DEBUG(x...) debugUnique(x)    // UnComment for Debug
    
    DEBUG(P("%s %d: A,B: %lu, %lu"), __func__, __LINE__, vfoA, vfoB);
    vfoActive = (vfoActive == VFO_A) ? VFO_B : VFO_A;
    frequency = (vfoActive == VFO_A) ? vfoA : vfoB;
    DEBUG(P("%s %d: %s %lu"), __func__, __LINE__, vfoActive == VFO_A ? "A:": "B:", frequency);
    refreshDisplay++;
}
      
// ###############################################################################
void checkTX() {
#define DEBUG(x ...)
//#define DEBUG(x ...) debugUnique(x)    // UnComment for Debug

    if (freqUnStable) return;  // Do Nothing if Freq is UnStable
    if (editIfMode) return;    // Do Nothing if in Edit-IF-Mode
    
    // DEBUG(P("%s %d:  Start Loop"), __func__, __LINE__);
    
    //if we have keyup for a longish time while in CW and PTT tx mode
    if (inTx && cwTimeout < millis()) {
        DEBUG(P("%s %d: TX to RX"), __func__, __LINE__);
        //Change the radio back to receive
        pinMode(TX_RX, OUTPUT); digitalWrite(TX_RX, 1); //set the TX_RX pin back to input mode
        pinMode(TX_RX, INPUT);  digitalWrite(TX_RX, 1); // With pull-up!
        inTx = inPtt = cwTimeout = tuningLocked = 0;
        if (AltTxVFO) toggleAltVfo(inTx);  // Clear Alt VFO if needed
        refreshDisplay++;
        return;
    }
  
    if (!keyDown && analogRead(ANALOG_KEYER) < 50) { // New KeyDown
        DEBUG(P("\n%s %d: KEY Dn"), __func__, __LINE__);
        if (!inBandLimits(frequency)) return; // Do nothing if TX is out-of-bounds
        if (!inTx){
            //put the  TX_RX line to transmit
            pinMode(TX_RX, OUTPUT); digitalWrite(TX_RX, 0);
            //give the T/R relays a few ms to settle
            inTx = keyDown = tuningLocked = 1;
            if (AltTxVFO) toggleAltVfo(inTx); // Set Alt VFI if Needed
            refreshDisplay++;
            delay(50);
        }
        digitalWrite(CW_KEY, 1); //start the side-tone
        //Start the timer the key is down
        cwTimeout = CW_TIMEOUT + millis();
        return;
    }
    
    if (keyDown && analogRead(ANALOG_KEYER) > 150) { //if we have a keyup
        DEBUG(P("%s %d: KEY Up"), __func__, __LINE__);
        keyDown = 0;
        digitalWrite(CW_KEY, 0); // stop the side-tone
        cwTimeout = CW_TIMEOUT + millis(); // Start timer for KeyUp Holdoff
        return;
    } 
    
    if (keyDown) {
        DEBUG(P("%s %d: KEY On"), __func__, __LINE__);
        cwTimeout = CW_TIMEOUT + millis(); // Restat timer
        return;
    } 
      
    if (inPtt && inTx) { // Check PTT
        DEBUG(P("%s %d: PTT"), __func__, __LINE__);
        cwTimeout = CW_TIMEOUT + millis(); // Restat timer
        // It is OK, to stop TX
        pinMode(TX_RX, INPUT); digitalWrite(TX_RX, 1); // With pull-up!
        if (digitalRead(TX_RX)) { // Is PTT Not pushed, then end PTT
            DEBUG(P("%s %d: Stop PTT"), __func__, __LINE__);
            inPtt = cwTimeout = 0;
        }
        return;       
    } 
    
    if (cwTimeout < millis() && !inTx) { // Check PTT
        DEBUG(P("%s %d: RX Idle"), __func__, __LINE__);
        // It is OK, to go into TX
        pinMode(TX_RX, INPUT); digitalWrite(TX_RX, 1); // With pull-up!
        if (!digitalRead(TX_RX)) { // Is PTT pushed  
            if (!inBandLimits(frequency)) return; // Do nothing if TX is out-of-bounds 
            DEBUG(P("\n%s %d: Start PTT"), __func__, __LINE__); 
            if (AltTxVFO) toggleAltVfo(inTx); // Set Alt VFO if Needed
            inTx = inPtt = tuningLocked = 1;
            delay(50);
            cwTimeout = CW_TIMEOUT + millis(); // Restat timer
            refreshDisplay++;
        }
        return;      
    }
    // Else, must be running out the cwTimeout
    DEBUG(P("%s %d: Holdiing"), __func__, __LINE__);
}


// ###############################################################################
int btnDown(){
#define DEBUG(x ...)
//#define DEBUG(x ...) debugUnique(x)    // UnComment for Debug
  int val = -1, val2 = -2;
  
  val = analogRead(FN_PIN);
  while (val != val2) { // DeBounce Button Press
      delay(10);
      val2 = val;
      val = analogRead(FN_PIN);
  }
  
  if (val>1000) return 0;
  
  tuningLocked = 1; // Holdoff Tuning until button is processed
  // 47K Pull-up, and 4.7K switch resistors,
  // Val should be approximately = (btnN×4700)÷(47000+4700)×1023

  DEBUG(P("%s %d: btn Val= %d"), __func__, __LINE__, val);

  if (val > 350) return 7;
  if (val > 300) return 6;
  if (val > 250) return 5;
  if (val > 200) return 4;
  if (val > 150) return 3;
  if (val >  50) return 2;
  return 1;
}


// -------------------------------------------------------------------------------
void deDounceBtnRelease() {
  int i = 2;
    
    while (i--) { // DeBounce Button Release, Check twice
      while (btnDown()) delay(20);
    }
    // The following allows the user to re-center the
    // Tuning POT during any Key Press-n-hold without changing Freq.
    readTuningPot();
    tuningPositionPrevious = tuningPosition;
    tuningLocked = 0; // Allow Tuning to Proceed
}


// ###############################################################################
void checkButton() {
#define DEBUG(x ...)
//#define DEBUG(x ...) debugUnique(x)    // UnComment for Debug
  int btn;
  btn = btnDown();
  if (btn) DEBUG(P("%s %d: btn %d"), __func__, __LINE__, btn);
  
  switch (btn) {
    case 0: return; // Abort
    case FN_BTN: decodeFN(btn); break;  
    case LEFT_BTN:  decodeMoveCursor(+1); break;    
    case RIGHT_BTN: decodeMoveCursor(-1); break;
    case 4: switch (getButtonPushMode(btn)) { 
            case MOMENTARY_PRESS:  decodeSideBandMode(btn); break;
            case DOUBLE_PRESS:     eePromIO(EEP_SAVE); break;
            case LONG_PRESS:       eePromIO(EEP_LOAD); break;
            case ALT_PRESS:        AltTxVFO = !AltTxVFO;  break;     
            default: return; // Do Nothing
            } break;
    case 5: decodeBandUpDown(+1); break; // Band Up
    case 6: decodeBandUpDown(-1); break; // Band Down
    case 7: switch (getButtonPushMode(btn)) { 
            case MOMENTARY_PRESS: decodeTune2500Mode(); break;
            case DOUBLE_PRESS:    decodeDialFreqCal(); break;
            case LONG_PRESS:      decodeEditIf(); break;    
            default: return; // Do Nothing
            } break;
    //case 7: decodeAux(7); break; // Report Un-Used as AUX Buttons
    default: return;
  }     
  refreshDisplay++;
  updateDisplay();
  deDounceBtnRelease(); // Wait for Button Release
}


// ###############################################################################
void decodeEditIf() {  // Set the IF Frequency
    static int vfoActivePrev = VFO_A;

    if (editIfMode) {  // Save IF Freq, Reload Previous VFO
        iFreq = frequency + ritVal;
        vfoActivePrev == VFO_A ? frequency = vfoA : frequency = vfoB;
    }
    else {  // Save Current VFO, Load IF Freq 
        vfoActive == VFO_A ? vfoA = frequency : vfoB = frequency;
        frequency = iFreq;
        vfoActivePrev = vfoActive;
    }
    editIfMode = !editIfMode;  // Toggle Edit IF Mode
    ritOn = ritVal = 0; 
}


// ###############################################################################
void decodeTune2500Mode() {
    
    if (editIfMode) return; // Do Nothing if in Edit-IF-Mode   
    if (ritOn) return; // Do Nothing if in RIT Mode
    
    cursorDigitPosition = 3; // Set default Tuning Digit
    tune2500Mode = !tune2500Mode;
    if (tune2500Mode) frequency = (frequency / 2500) * 2500;
}


// ###############################################################################
void decodeDialFreqCal() {
    if (ritOn) {
        isLSB ? dialFreqCalLSB += ritVal : dialFreqCalUSB += ritVal;
        ritVal = 0;
    }
    cursorOff();
    sprintf(c, P("%3.3s  CAL: %+04.4d"),
        isLSB ? "LSB" : "USB",
        isLSB ? dialFreqCalLSB : dialFreqCalUSB
    );
    printLine2CEL(c);
    delay(500);
    deDounceBtnRelease(); // Wait for Button Release
}


// ###############################################################################
void decodeBandUpDown(int dir) {
    int j;
    
   if (editIfMode) return; // Do Nothing if in Edit-IF-Mode
      
    if(dir > 0) {  // For Band Change, Up
       for (int i = 0; i < BANDS; i++) {
         j = i*2 + vfoActive;
         if (frequency <= pgm_read_dword(&bandLimits[i*2+1])) {
           if (frequency >= pgm_read_dword(&bandLimits[i*2])) {
             // Save Current Ham frequency and sideBandMode
             freqCache[j] = frequency;
             sideBandModeCache[j] = sideBandMode;
           }
           // Load From Next Cache Up Band
           j += 2;
           frequency = freqCache[min(j,BANDS*2-1)];
           sideBandMode = sideBandModeCache[min(j,BANDS*2-1)];
           vfoActive == VFO_A ? vfoA = frequency : vfoB = frequency;
           break;
         }
       }
     } // End fi
     
     else { // For Band Change, Down
       for (int i = BANDS-1; i > 0; i--) {
         j = i*2 + vfoActive;
         if (frequency >= pgm_read_dword(&bandLimits[i*2])) {
           if (frequency <= pgm_read_dword(&bandLimits[i*2+1])) {
             // Save Current Ham frequency and sideBandMode
             freqCache[j] = frequency;
             sideBandModeCache[j] = sideBandMode;
           }
           // Load From Next Cache Down Band
           j -= 2;
           frequency = freqCache[max(j,vfoActive)];
           sideBandMode = sideBandModeCache[max(j,vfoActive)];
           vfoActive == VFO_A ? vfoA = frequency : vfoB = frequency;
           break;
         }
       }
     } // End else
     
   freqUnStable = 25; // Set to UnStable (non-zero) Because Freq has been changed
   ritOn = 0;
   setSideband();
}


// ###############################################################################
void decodeSideBandMode(int btn) {
        
       sideBandMode++;
       sideBandMode %= 3; // Limit to Three Modes
       setSideband();
       cursorOff();
       printLine2CEL((char *)pgm_read_word(&sideBandText[sideBandMode]));
       delay(500);
       deDounceBtnRelease(); // Wait for Release
}


// ###############################################################################
void eePromIO(int mode) {   
   struct config_t {
        long idFlag;
        unsigned long frequency;
        int editIfMode;
        unsigned long iFreq;
        int dialFreqCalUSB;
        int dialFreqCalLSB;
        unsigned long vfoA;
        unsigned long vfoB;
        byte isLSB;
        byte vfoActive;
        unsigned long freqCache[BANDS*2];
        byte sideBandMode;
        byte sideBandModeCache[BANDS*2];
        byte checkSum;
    } E;
    byte checkSum = 0;
    byte *pb = (byte *)&E;   
    
    cursorOff();
   
    switch(mode) {
    case EEP_LOAD:
        // Read from Non-Volatile Memory and check for the correct ID
        eeprom_read_block((void*)&E, (void*)0, sizeof(E));
        if (E.idFlag != ID_FLAG) { sprintf(c, P("Load Failed ID")); break; }
        
        // Compute and Check the CheckSum
        for (int i = 0; i < sizeof(E); i++) checkSum += *pb++;
        if (checkSum != 0) { sprintf(c, P("Load Failed CSum")); break; }
        
        // Assign Values to Working Variables from eeProm Structure
        idFlag = E.idFlag; 
        frequency = E.frequency;
        editIfMode = E.editIfMode;
        dialFreqCalUSB = E.dialFreqCalUSB;
        dialFreqCalLSB = E.dialFreqCalLSB;
        vfoA = E.vfoA;
        vfoB = E.vfoB;
        isLSB = E.isLSB;
        vfoActive = E.vfoActive;
        memcpy(freqCache, E.freqCache, sizeof(E.freqCache));
        sideBandMode = E.sideBandMode;
        memcpy(sideBandModeCache, E.sideBandModeCache, sizeof(E.sideBandModeCache));
        checkSum = E.checkSum;
       
        sprintf(c, P("Loading %dB"), sizeof(E));      
        break;
        
    case EEP_SAVE :
        // Assign Working Variables to the eeProm Structure
        E.idFlag = ID_FLAG;
        E.frequency = frequency;
        E.editIfMode = editIfMode;
        E.dialFreqCalUSB = dialFreqCalUSB;
        E.dialFreqCalLSB = dialFreqCalLSB;
        E.vfoA = vfoA;
        E.vfoB = vfoB;
        E.isLSB = isLSB;
        E.vfoActive = vfoActive;
        memcpy(E.freqCache, freqCache, sizeof(freqCache));
        E.sideBandMode = sideBandMode;
        memcpy(E.sideBandModeCache, sideBandModeCache, sizeof(sideBandModeCache));
        E.checkSum = checkSum;   // Not necessary, used here as an Optical Place Holder
        
        // Compute and save the new Checksum of eeProm Struture
        for (int i = 0; i < sizeof(E) - sizeof(E.checkSum); i++) checkSum += *pb++;
        E.checkSum = -checkSum; // Apply CheckSum to Structure
        
        // Write the eeProm Strcture to Non-Volatile Memory
        eeprom_write_block((const void*)&E, (void*)0, sizeof(E));
        
        sprintf(c, P("Storing %dB"), sizeof(E));
        break;
    }
   
    printLine2CEL(c);
    delay(500);
    deDounceBtnRelease(); // Wait for Release
} 


// ###############################################################################
void decodeMoveCursor(int dir) {
  
      if (tune2500Mode) return; // Do not Move Cursor in this mode
    
      tuningPositionPrevious = tuningPosition;
      cursorDigitPosition += dir;
      cursorDigitPosition = min(cursorDigitPosition, 7);
      cursorDigitPosition = max(cursorDigitPosition, 0);
      freqUnStable = false;  // Set Freq is NOT UnStable, as it is Stable          
      refreshDisplay++;
}

// -------------------------------------------------------------------------------
void decodeAux(int btn) {
    //debug("%s btn %d", __func__, btn);
    cursorOff();
    sprintf(c, P("Btn: %.2d"), btn);
    printLine2CEL(c);
    delay(500);
    deDounceBtnRelease(); // Wait for Release
}


// ###############################################################################
int getButtonPushMode(int btn) {
  int t1, t2, tbtn;
  
  t1 = t2 = 0;

  // Time for first press
  tbtn = btnDown();
  while (t1 < 20 && btn == tbtn){
    tbtn = btnDown();
    if (btn != FN_BTN && tbtn == FN_BTN) return ALT_PRESS;
    delay(50);
    t1++;
  }
  // Time between presses
  while (t2 < 10 && !tbtn){
    tbtn = btnDown();
    if (btn != FN_BTN && tbtn == FN_BTN) return ALT_PRESS;
    delay(50);
    t2++;
  }

  if (t1 > 10) return LONG_PRESS;
  if (t2 < 7) return DOUBLE_PRESS; 
  return MOMENTARY_PRESS;
  
}


// ###############################################################################
void decodeFN(int btn) {

  switch (getButtonPushMode(btn)) { 
    case MOMENTARY_PRESS:
       ritOn = !ritOn; ritVal = 0;     
       refreshDisplay++;
       updateDisplay();
       break;
      
    case DOUBLE_PRESS:
       if (editIfMode) { // Abort Edit IF Mode, Reload Active VFO
          editIfMode = false;
          frequency = (vfoActive == VFO_A) ? vfoA : vfoB; break;
       } 
       else { // Save Current VFO, Load Other VFO
          if (vfoActive == VFO_A) {
            vfoA = frequency;
            vfoActive = VFO_B;
            frequency = vfoB;
          } 
          else {
            vfoB = frequency;
            vfoActive = VFO_A;
            frequency = vfoA;
          }
       }
       ritOn = ritVal = 0;     
       refreshDisplay++;
       updateDisplay();
       break;
      
    case LONG_PRESS:
       switch (vfoActive) {
       case VFO_A :
          vfoB = frequency + ritVal;
          sprintf(c,P("A%sB"), ritVal ? "+RIT>": ">");
          printLine2CEL(c);
          break;
       default :
          vfoA = frequency + ritVal;
          sprintf(c,P("B%sA"), ritVal ? "+RIT>": ">");
          printLine2CEL(c);
          break;
       }
       cursorOff();
       delay(500);
       break;
    default :
       return; // Do Nothing
  }
  deDounceBtnRelease(); // Wait for Release
}

// ###############################################################################
void preLoadUserPerferences() {
  // Check EEPROM for User Saved Preference, Load if available
  // Hold any Button at Power-ON or Processor Reset does a "Factory Reset" to Default Values
  printLine1CEL("User Pref:");
  if (!btnDown()) eePromIO(EEP_LOAD);
  else printLine2CEL(P("Factory Reset"));
  delay(500);
  deDounceBtnRelease(); // Wait for Button Release 
}


// ###############################################################################
// ###############################################################################
// ###############################################################################
// ###############################################################################

// ###############################################################################
void setup() {
  
  // Initialize the Serial port so that we can use it for debugging
  Serial.begin(115200);
  debug(P("%s Radiono - Rev: %s"), __func__, RADIONO_VERSION);

  lcd.begin(LCD_COL, LCD_ROW);
  printLine1(P("Farhan - Minima"));
  printLine2(P("  Tranceiver"));
  delay(2000);
  
  sprintf(b, P("Radiono %s"), RADIONO_VERSION);
  printLine1CEL(b);
  
  sprintf(b, P("Rev: %s"), INC_REV);
  printLine2CEL(b);
  delay(2000);
  
  //sprintf(b, P("%s"), __DATE__); // Compile Date and Time
  //sprintf(c, "%3.3s%7.7s %5.5s", b, b+4, __TIME__);
  //printLine2CEL(c);
  //delay(2000); 
  
  // Print just the File Name, Added by ERB
  //sprintf(c, P("F: %-13.13s"), P2(__FILE__));
  //printLine2CLE(c);
  //delay(2000);
  

  // The library automatically reads the factory calibration settings of your Si570
  // but it needs to know for what frequency it was calibrated for.
  // Looks like most HAM Si570 are calibrated for 56.320 Mhz.
  // If yours was calibrated for another frequency, you need to change that here
  vfo = new Si570(SI570_I2C_ADDRESS, 56320000);

  if (vfo->status == SI570_ERROR) {
    // The Si570 is unreachable. Show an error for 3 seconds and continue.
    printLine2CEL(P("Si570 comm error"));
    delay(3000);
  }
  
  printLine2CEL(" ");
 
  // This will print some debugging info to the serial console.
  vfo->debugSi570();

  //set the initial frequency
  vfo->setFrequency(26150000L);
 
  //set up the pins
  pinMode(LSB, OUTPUT);
  pinMode(CW_KEY, OUTPUT);
  pinMode(PA_BAND_CLK, OUTPUT);

  //set the side-tone off, put the transceiver to receive mode
  digitalWrite(CW_KEY, 0);
  
  pinMode(TX_RX, INPUT);
  digitalWrite(TX_RX, 1); //old way to enable the built-in pull-ups
  
  pinMode(ANALOG_TUNING, INPUT);
  digitalWrite(ANALOG_TUNING, 1); //old way to enable the built-in pull-ups
  
  pinMode(FN_PIN, INPUT);
  digitalWrite(FN_PIN, 0); // Use an external pull-up of 47K ohm to AREF
  
  preLoadUserPerferences();
  
  tuningPositionPrevious = tuningPosition = analogRead(ANALOG_TUNING);
  refreshDisplay = +1;
  
}


// ###############################################################################
// ###############################################################################
void loop(){
    unsigned long freq;
  
  readTuningPot();
  checkTuning();

  checkTX();
  
  checkButton();

  if (editIfMode) {  // Set freq to Curent VFO + Trail IF Freq
         freq = frequency - iFreq + (vfoActive == VFO_A) ? vfoA : vfoB;
  } else freq = frequency;
  
  freq += isLSB ? SB_OFFSET : -SB_OFFSET;
  freq += isLSB ? dialFreqCalLSB : dialFreqCalUSB;
  if (!inTx && ritOn) freq += ritVal;
  vfo->setFrequency(freq + iFreq);
  
  setSideband();
  setBandswitch(frequency);
  setRf386BandSignal(frequency);

  updateDisplay();
  
}

// ###############################################################################

//End
