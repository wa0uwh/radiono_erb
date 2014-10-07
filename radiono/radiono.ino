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
 * Modified by: Jeff Witlatch - KO7M - Copyright (C) 2014
 *   Added 1Hz VFO Resolution
 *   Added Support for PCA9546 I2C Mux
 *   Added Support for I2C LCD
 *
 * Modified by: Eldon R. Brown (ERB) - WA0UWH - Copyright (C) 2014
 *   Added An Alternate Tuning Method, with Cursor and POT
 *   Added Multiple Button Support
 *   Added RIT Function
 *   Added Manual Sideband Selection 
 *   Added Band Selection UP/DOWN
 *   Added Band Selection Memories for both VFO_A and VFO_B
 *   Added Band Limits for Transmit
 *   Added Alternate VFO for Transmit, Split Operation
 *   Added Rf386 Band Selecter Encoder
 *   Added Edit and Set IF Frequency for Dial Calibration for USB and LSB
 *   Added Support for 2500Hz Tuning Mode
 *   Added Non-Volatile Memory Storage
 *   Added Morse Code Functions, MACROs and Beacon 
 *   Added New Cursor Blink Strategy
 *   Added Some new LCD Display Format Functions
 *   Added Idle Timeout for Blinking Cursor
 *   Added Sideband Toggle while in Edit-IF-Mode
 *   Added Dial Cursor Movement via Tuning Knob
 *   Added Menu Support with Idle Timeout
 *   Added Suffixes KILO and MEG, to make Coding Large Freq Numbers easier
 *   Added SWL to Display when Outside of HAM Bands
 *   Added Optional USE_MENUS Support
 *   Added Optional USER Configuration Support, via #ifdef
 *   Added Initial Rotary Encoder01 Support
 *   Made  Park Cursor and Cursor Timeout Optional, User Can set Default Cursor Position
 *   Added Optional Compile Ham Bands, and Band Limits Support
 *   Added Optional Hide Least Digits while Tuning
 *   Added Optional Compile Tune2500 Mode
 *   Added Optional Compile EditIF Mode
 *
 */

void setup(); // # A Hack, An Arduino IED Compiler Preprocessor Fix


//#define RADIONO_VERSION "0.4"
#define RADIONO_VERSION "0.4.erb" // Modifications by: Eldon R. Brown - WA0UWH
#define INC_REV "ko7m-AC"         // Incremental Rev Code
#define INC_REV "ERB_GP_05"          // Incremental Rev Code

/*
 * Wire is only used from the Si570 module but we need to list it here so that
 * the Arduino environment knows we need it.
 */
#include <Wire.h>


#include <avr/io.h>
#include "A1Main.h"
#include "ButtonUtil.h"
#include "Si570.h"
#include "debug.h"

#define LCD_COL (16)
#define LCD_ROW (2)
//#define LCD_COL (20)
//#define LCD_ROW (4)
#define LCD_STR_CEL "%-16.16s"    // Fmt to implement Clear to End of Line
//#define LCD_STR_CEL "%-20.20s"  // For 20 Character LCD Display

// Set the following Conditional Compile Flags in the "A1Main.h" file.
#ifndef USE_I2C_LCD
  #include <LiquidCrystal.h>
#else
  #include <LiquidTWI.h>
#endif
    

#define SI570_I2C_ADDRESS   0x55

// Default Tune Frequency
#define DEFAULT_TUNE_FREQ (14.285 * MHz) //  20m - QRP SSB Calling Freq
// USB and LSB IF frequencies
#define IF_FREQ_USB   (19.997 * MHz)
#define IF_FREQ_LSB   (19.992 * MHz)

#define CW_TIMEOUT (600L) // in milliseconds, this is the parameter that determines how long the tx will hold between cw key downs

// Define MAX Tuning Range
#define MAX_FREQ (32.0 * MHz)

// Tuning POT Dead Zone
#define DEAD_ZONE (40)


// Pin Number for the digital output controls
#define PIN_LSB (2)
#define TX_RX (3)
#define CW_KEY (4)


#ifdef USE_PCA9546
  PCA9546 *mux;
#endif

Si570 *vfo;

#ifndef USE_I2C_LCD
  LiquidCrystal lcd(13, 12, 11, 10, 9, 8);
#else
  LiquidTWI lcd(0);   // I2C backpack display on 20x4 or 16x2 LCD display
#endif

unsigned long frequency = DEFAULT_TUNE_FREQ;
unsigned long iFreqUSB = IF_FREQ_USB;
unsigned long iFreqLSB = IF_FREQ_LSB;
boolean isLSB = USB;
byte sideBandMode = AutoSB_MODE;

unsigned long vfoA = frequency, vfoB = frequency;
unsigned long cwTimeout = 0;

char b[LCD_COL+6], c[LCD_COL+6];  // General Buffers, used mostly for Formating message for LCD
char blinkChar;

/* tuning pot stuff */
byte refreshDisplay = 0;
unsigned long blinkTimer = 0;
unsigned long blinkTimeOut = DEFAULT_BLINK_TIMEOUT; // Default Blink TimeOut, Milli Seconds
int blinkPeriod = 500;  // MSECs
byte blinkRatio = 75;   // Persent
unsigned long menuIdleTimeOut = 60 * SECs;

int tuningDir = 0;
int knobPosition = 0;
int freqUnStable = 1;
int knobPositionDelta = 0;
int cursorDigitPosition = DEFAULT_CURSOR_POSITION;
int knobPositionPrevious = 0;
int cursorCol, cursorRow, cursorMode;

boolean tuningLocked = 0; //the tuning can be locked: wait until Freq Stable before unlocking it
boolean dialCursorMode = 0;
boolean inTx = 0, inPtt = 0;
boolean keyDown = 0;
boolean vfoActive = VFO_A;

/* modes */
int ritVal = 0;
boolean ritOn = 0;
boolean AltTxVFO = 0;
boolean isAltVFO = 0;

// ERB - Buffers that Stores "const stings" to, and Reads from FLASH Memory via P()
char buf[PBUFSIZE];  // Note: PBUFSIZE must be set in A1Main.h


// ###############################################################################
// ###############################################################################
// ###############################################################################
// ###############################################################################

// ###############################################################################
/* display routines */
void printLineXY(byte col, byte row, char const *c) {   
    char lbuf[LCD_COL+2];
    
    //snprintf(lbuf, sizeof(lbuf)-col, "%s", c);
    strncpy(lbuf, c, sizeof(lbuf)-col);
    lcd.setCursor(col, row % LCD_ROW);
    lcd.print(lbuf);
}

// -------------------------------------------------------------------------------
void printLine(int row, char const *c){
    printLineXY(0, row, c);
}

// -------------------------------------------------------------------------------
// Print LCD Row with Clear to End of Line
void printLineCEL(int row, char const *c){
    char buf[16];  // Used for local P() Function
    char lbuf[LCD_COL+2];
    
    sprintf(lbuf, P(LCD_STR_CEL), c);
    printLineXY(0, row, lbuf);
}


// ###############################################################################
void updateDisplay(){
  char const *vfoStatus[] = { "ERR", "RDY", "BIG", "SML" };
  char d[6]; // Buffer for RIT Display Value
  char *vfoLabel;

  if (refreshDisplay) {
      if (refreshDisplay > 0) refreshDisplay--;
      blinkTimer = 0;
      
      // Create Label for Displayed VFO
      vfoLabel = vfoActive == VFO_A ?  P2("A") : P2("B");
      if (AltTxVFO)  *vfoLabel += 32; // Convert to Lower Case
      #ifdef USE_EDITIF
        if (editIfMode) vfoLabel = isLSB ? P2("L") : P2("U"); // Replace with IF Freq VFO
      #endif // USE_EDITIF
      
      // Top Line of LCD
      sprintf(d, P("%+03.3d"), ritVal);  
      sprintf(b, P("%08ld"), frequency);
      #ifdef USE_HIDELEAST
        if (cursorDigitPosition>1) b[10-cursorDigitPosition] = 0;
      #endif // USE_HIDELEAST
      sprintf(c, P("%1s:%.2s.%.6s%4.4s%s"), vfoLabel,
          b,  b+2,
          inTx ? P4(" ") : ritOn ? d : P4(" "),
          #ifdef USE_TUNE2500_MODE
              tune2500Mode ? P8("*"):
          #endif // USE_TUNE2500_MODE 
          P8(" ")
          );
      printLineCEL(FIRST_LINE, c);
      
      saveCursor(11 - (cursorDigitPosition + (cursorDigitPosition>6) ), 0);
      blinkChar = c[cursorCol];
      
      sprintf(c, P("%3s %-2s %3.3s"),
          sideBandMode == AutoSB_MODE ? 
          isLSB ? P2("LSB") : P2("USB") :
          isLSB ? P2("Lsb") : P2("Usb"),
          inTx ? (inPtt ? P3("PT") : P3("CW")) : P3("RX"),
          freqUnStable
          #ifdef USE_EDITIF
              || editIfMode 
          #endif // USE_EDITIF
          ? P4(" ") : 
          #ifdef USE_HAMBANDS
              inBand ? vfoStatus[vfo->status] : P4("SWL")
          #else
              vfoStatus[vfo->status]
          #endif // USE_HAMBANDS
          );
      printLineCEL(STATUS_LINE, c);
      
  }
  updateCursor();
}


// -------------------------------------------------------------------------------
void saveCursor(int col, int row) {  
  cursorCol = col;
  cursorRow = row;
}


// -------------------------------------------------------------------------------
void cursorOff() {    
  lcd.noBlink();
  lcd.noCursor();
}


// -------------------------------------------------------------------------------
void updateCursor() {
#define DEBUG(x...)
//#define DEBUG(x...) debugUnique(x)    // UnComment for Debug

  static unsigned long blinkInterval = 0;
  static boolean toggle = false;
  char blockChar = 0xFF;
    
  if (inTx) return;   // Don't Blink if inTx
  if (ritOn) return;  // Don't Blink if RIT is ON
  if (freqUnStable) return;  // Don't Blink if Frequency is UnStable
  #ifdef USE_TUNE2500_MODE
      if (tune2500Mode) blinkTimer = 0; // Blink does not Stop in tune2500Mode
  #endif // USE_TUNE2500_MODE

  if (!blinkTimer) blinkTimer = millis() + blinkTimeOut;
  
  DEBUG(P("\nStart Blink"));
  if (blinkInterval < millis()) { // Wink OFF
      DEBUG(P("Wink OFF"));
      blinkInterval = millis() + blinkPeriod;
      if (cursorDigitPosition) {
          lcd.setCursor(cursorCol, cursorRow); // Postion Cursor
          if (dialCursorMode) lcd.print(blockChar); 
          else lcd.print('_');
          //else lcd.print(blockChar);
      }
      toggle = true;
  } 
  else if ((blinkInterval - (blinkPeriod/100*blinkRatio)) < millis() && toggle) { // Wink ON
      DEBUG(P("Wink ON"));
      toggle = !toggle;
      lcd.setCursor(cursorCol, cursorRow); // Postion Cursor 
      lcd.print(blinkChar);
      if (blinkTimeOut && blinkTimer < millis()) {
          DEBUG(P("End Blink TIMED OUT"));
          cursorDigitPosition = 0;
          dialCursorMode = true;
          refreshDisplay++;
          updateDisplay();
      }
  }
  return;
}

// ###############################################################################
void decodeSideband(){

  #ifdef USE_EDITIF
    if (editIfMode) return;    // Do Nothing if in Edit-IF-Mode
  #endif // USE_EDITIF

  switch(sideBandMode) {
   // This was originally set to 10.0 Meg, Changed to avoid switching Sideband while tuning around WWV
    case AutoSB_MODE: isLSB = (frequency < 9.99 * MHz) ? 1 : 0 ; break; // Automatic Side Band Mode 
    case USB_MODE: isLSB = USB; break; // Force USB Mode
    case LSB_MODE: isLSB = LSB; break; // Force LSB Mode   
  }
  setSideband();
}

// -------------------------------------------------------------------------------
void setSideband(){  
  pinMode(PIN_LSB, OUTPUT); digitalWrite(PIN_LSB, isLSB);
}

// ###############################################################################
void setBandswitch(unsigned long freq){

  #ifdef USE_EDITIF
    if (editIfMode) return;    // Do Nothing if in Edit-IF-Mode
  #endif // USE_EDITIF

  // This was originally set to 15.0 Meg, Changed to avoid switching while tuning around WWV
  if (freq > 14.99 * MHz) digitalWrite(BAND_HI_PIN, 1);
  else digitalWrite(BAND_HI_PIN, 0);
}


// ###############################################################################
// An Alternate Tuning Strategy or Method
// This method somewhat emulates a normal Radio Tuning Dial
// Tuning Position by Switches on FN Circuit
// Author: Eldon R. Brown - WA0UWH, Apr 25, 2014
void checkTuning() {
  long deltaFreq;
  unsigned long newFreq;
  static int encoderA_Prev = 1;

  // Count Down to Freq Stable, i.e. Freq has not changed recently
  if (freqUnStable && freqUnStable < 5) {
      refreshDisplay++;
      #ifdef USE_HAMBANDS
          inBandLimits(frequency);
      #endif // USE_HAMBANDS
  }
  freqUnStable = max(freqUnStable - 1, 0);
  
  // Do Not Change Freq while in Transmit or button opperation
  // Allow Tuning knob to be recentered without changing Frequency
  if (tuningLocked) {
      knobPositionPrevious = knobPosition;
      return;
  }
  
  tuningDir = 0;

  #ifdef USE_POT_KNOB
      tuningDir += getPotDir(); // Get Tuning Direction from POT Knob
  #endif // USE_POT_KNOB
  
  #ifdef USE_ENCODER01
      tuningDir += getEncoderDir(); // Get Tuning Direction from Encoder Knob
  #endif // USE_ENCODER01

  if (!tuningDir) return;

  
  // Decode and implement RIT Tuning
  if (ritOn) {
      ritVal += tuningDir * 10;
      ritVal = constrain(ritVal, -990, +990);
      dialCursorMode = true;
      refreshDisplay++;
      updateDisplay();
      return;
  }
 
  blinkTimer = 0;  
  
  if (dialCursorMode) {
      decodeMoveCursor(-tuningDir); // Move the Cursor with the Dial
      return;
  }
  
  if (cursorDigitPosition < 1) {
     dialCursorMode = true;
     return; // Nothing to do here, Abort, Cursor is in Park position
  }
  
  #ifdef USE_TUNE2500_MODE
      // Select Tuning Mode; Digit or 2500 Step Mode
      if (tune2500Mode) {
          // Inc or Dec Freq by 2.5K, useful when tuning between SSB stations
          cursorDigitPosition = 3;
          deltaFreq += tuningDir * 2500;
          
          newFreq = (frequency / 2500) * 2500 + deltaFreq;
      }
      else
  #endif // USE_TUNE2500_MODE
  {
      // Compute deltaFreq based on current Cursor Position Digit
      deltaFreq = tuningDir;
      for (int i = cursorDigitPosition; i > 1; i-- ) deltaFreq *= 10;
  
      newFreq = frequency + deltaFreq;  // Save Least Digits Mode
      //newFreq = (frequency / abs(deltaFreq)) * abs(deltaFreq) + deltaFreq; // Zero Lesser Digits Mode
  }

  if (newFreq != frequency) {
      // Update frequency if within range of limits, 
      // Avoiding Nagative underRoll of UnSigned Long, and over-run MAX_FREQ  
      if (newFreq <= MAX_FREQ) {
        frequency = newFreq;
        #ifdef USE_EDITIF
           if (!editIfMode)
        #endif // USE_EDITIF 
           vfoActive == VFO_A ? vfoA = frequency : vfoB = frequency;
        refreshDisplay++;
      }
      freqUnStable = 25; // Set to UnStable (non-zero) Because Freq has been changed
  }
}

  
// ###############################################################################
void toggleAltVfo() {
#define DEBUG(x...)
//#define DEBUG(x...) debugUnique(x)    // UnComment for Debug
    
    DEBUG(P("%s %d: A,B: %lu, %lu"), __func__, __LINE__, vfoA, vfoB);
    vfoActive = (vfoActive == VFO_A) ? VFO_B : VFO_A;
    frequency = (vfoActive == VFO_A) ? vfoA : vfoB;
    DEBUG(P("%s %d: %s %lu"), __func__, __LINE__, vfoActive == VFO_A ? P2("A:"): P2("B:"), frequency);
    refreshDisplay++;
}
      
// ###############################################################################
void checkTX() {
#define DEBUG(x ...)
//#define DEBUG(x ...) debugUnique(x)    // UnComment for Debug

    if (freqUnStable) return;  // Do Nothing if Freq is UnStable
    #ifdef USE_EDITIF
      if (editIfMode) return;    // Do Nothing if in Edit-IF-Mode
    #endif // USE_EDITIF
    
    // DEBUG(P("%s %d:  Start Loop"), __func__, __LINE__);
    
    //if we have keyup for a longish time while in CW and PTT tx mode
    if (inTx && cwTimeout < millis()) {
        DEBUG(P("%s %d: TX to RX"), __func__, __LINE__);
        //Change the radio back to receive
        changeToReceive();
        inTx = inPtt = cwTimeout = tuningLocked = 0;
        if (AltTxVFO) toggleAltVfo();  // Clear Alt VFO if needed
        refreshDisplay++;
        return;
    }
  
    if (!keyDown && isKeyNowClosed()) { // New KeyDown
        #ifdef USE_HAMBANDS
            if (!inBandLimits(frequency)) return; // Do nothing if TX is out-of-bounds
        #endif // USE_HAMBANDS
        DEBUG(P("\nFunc: %s %d: Start KEY Dn"), __func__, __LINE__);
        if (!inTx){
            //put the  TX_RX line to transmit
            changeToTransmit();
            if (AltTxVFO) toggleAltVfo(); // Set Alt VFI if Needed
            refreshDisplay++;
            //give the T/R relays a few ms to settle
            delay(50);
        }
        inTx = keyDown = tuningLocked = 1;
        startSidetone(); //start the side-tone
        cwTimeout = CW_TIMEOUT + millis(); // Start the timer the key is down
        return;
    }
    
    //if (keyDown && analogRead(ANALOG_KEYER) > 150) { //if we have a keyup
    if (keyDown && isKeyNowOpen()) { //if we have a keyup
        DEBUG(P("%s %d: KEY Up"), __func__, __LINE__);
        keyDown = 0;
        stopSidetone(); // stop the side-tone
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
        if (!isPttPressed()) { // Is PTT Not pushed, then end PTT
            DEBUG(P("%s %d: Stop PTT"), __func__, __LINE__);
            inPtt = cwTimeout = 0;
        }
        return;       
    } 
    
    if (cwTimeout < millis() && !inTx) { // Check PTT
        DEBUG(P("%s %d: RX Idle"), __func__, __LINE__);
        // It is OK, to go into TX
        if (isPttPressed()) {
            #ifdef USE_HAMBANDS
                if (!inBandLimits(frequency)) return; // Do nothing if TX is out-of-bounds
            #endif // USE_HAMBANDS 
            DEBUG(P("\nFunc: %s %d: Start PTT"), __func__, __LINE__); 
            if (AltTxVFO) toggleAltVfo(); // Set Alt VFO if Needed
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

// -------------------------------------------------------------
int isPttPressed() {
    DEBUG(P("\nFunc: %s %d"), __func__, __LINE__);
    return !digitalRead(TX_RX); // Is PTT pushed  
}

void changeToReceive() {
    DEBUG(P("\nFunc: %s %d"), __func__, __LINE__);
    stopSidetone();
    pinMode(TX_RX, OUTPUT); digitalWrite(TX_RX, 1); //set the TX_RX pin back to input mode
    pinMode(TX_RX, INPUT);  digitalWrite(TX_RX, 1); // With pull-up!
}

void changeToTransmit() {
    DEBUG(P("\nFunc: %s %d"), __func__, __LINE__);
    pinMode(TX_RX, OUTPUT);
    digitalWrite(TX_RX, 0);
}

int isKeyNowClosed() {
    DEBUG(P("\nFunc: %s %d"), __func__, __LINE__);
    return analogRead(ANALOG_KEYER) < 50;
}

int isKeyNowOpen() {
    DEBUG(P("\nFunc: %s %d"), __func__, __LINE__);
    return analogRead(ANALOG_KEYER) > 150;
}

void startSidetone() {
    DEBUG(P("\nFunc: %s %d"), __func__, __LINE__);
    digitalWrite(CW_KEY, 1); // start the side-tone
}

void stopSidetone() {
    DEBUG(P("\nFunc: %s %d"), __func__, __LINE__);
    pinMode(CW_KEY, OUTPUT);
    digitalWrite(CW_KEY, 0); // stop the side-tone
}


// ###############################################################################
void checkButton() {
#define DEBUG(x ...)
//#define DEBUG(x ...) debugUnique(x)    // UnComment for Debug
  int btn;
  char buf[PBUFSIZE]; // A Local buf, used to pass mesg's to send messages
  
  if (inTx) return;    // Do Nothing if in TX-Mode
  
  btn = btnDown();
  if (btn) DEBUG(P("%s %d: btn %d"), __func__, __LINE__, btn);

  switch (btn) {
    case 0: return; // Abort
    case FN_BTN: decodeFN(btn); break;  
    case LT_CUR_BTN: dialCursorMode = false; decodeMoveCursor(+1); break;    
    case RT_CUR_BTN: dialCursorMode = false; decodeMoveCursor(-1); break;
    case LT_BTN:
        switch (getButtonPushMode(btn)) { 
            case MOMENTARY_PRESS:  decodeSideBandMode(btn); break;
            #ifdef USE_EEPROM
                case DOUBLE_PRESS:     eePromIO(EEP_LOAD); break;
                case LONG_PRESS:       eePromIO(EEP_SAVE); break;
            #endif // USE_EEPROM
                case ALT_PRESS_FN:     toggleAltTxVFO();  break;
            #ifdef USE_BEACONS
                case ALT_PRESS_LT_CUR: sendMorseMesg(cw_wpm, P(CW_MSG1));  break;
                case ALT_PRESS_RT_CUR: sendMorseMesg(cw_wpm, P(CW_MSG2));  break;
            #endif // USE_BEACONS
            default: break; // Do Nothing
            }
        break;
    #ifdef USE_HAMBANDS
        case UP_BTN: decodeBandUpDown(+1); break; // Band Up
        case DN_BTN: decodeBandUpDown(-1); break; // Band Down
    #endif // USE_HAMBANDS
    case RT_BTN: 
        switch (getButtonPushMode(btn)) {
            case MOMENTARY_PRESS:  dialCursorMode = !dialCursorMode; break;
            #ifdef USE_MENUS
                case DOUBLE_PRESS:     menuActive = menuPrev ? menuPrev : DEFAULT_MENU; cursorDigitPosition = 0; refreshDisplay+=2; break;
            #endif // USE_MENUS
            #ifdef USE_EDITIF
                case LONG_PRESS:       editIf(); break;
            #endif // USE_EDITIF
            #ifdef USE_TUNE2500_MODE
                case ALT_PRESS_LT:     decodeTune2500Mode(); break;
            #endif // USE_TUNE2500_MODE
            #ifdef USE_BEACONS
                case ALT_PRESS_LT_CUR: sendQrssMesg(qrssDitTime, QRSS_SHIFT, P(QRSS_MSG1));  break;
                case ALT_PRESS_RT_CUR: sendQrssMesg(qrssDitTime, QRSS_SHIFT, P(QRSS_MSG2));  break;
            #endif // USE_BEACONS
            default: break; // Do Nothing
            }
        break;
    #ifdef USE_ENCODER01
        case ENC_KNOB: readEncoder(btn); break;
    #endif // USE_ENCODER01
    default: decodeAux(btn); break;
  }
  if (btn) DEBUG(P("%s %d: btn %d"), __func__, __LINE__, btn);
  blinkTimer = 0;
  refreshDisplay++;
  updateDisplay();
  deDounceBtnRelease(); // Wait for Button Release
}


// ###############################################################################
void toggleAltTxVFO() {
    
    #ifdef USE_EDITIF
      if (editIfMode) return; // Do Nothing if in Edit-IF-Mode
    #endif // USE_EDITIF
    
    AltTxVFO = !AltTxVFO;
}


// ###############################################################################
void decodeSideBandMode(int btn) {
#define DEBUG(x ...)
//#define DEBUG(x ...) debugUnique(x)    // UnComment for Debug

    DEBUG(P("\nCurrent, isLSB %d"), isLSB);
    #ifdef USE_EDITIF
        if (editIfMode) { // Switch Sidebands
            frequency += ritVal;
            ritVal = 0;
            isLSB ? iFreqLSB = frequency : iFreqUSB = frequency;
            isLSB = !isLSB;
            frequency = isLSB ? iFreqLSB : iFreqUSB;
            setSideband();
        }
        else
    #endif // USE_EDITIF 
    {
        sideBandMode++;
        sideBandMode %= 3; // Limit to Three Modes
        decodeSideband();
    }

    DEBUG(P("Toggle, isLSB %d"), isLSB);
    refreshDisplay++;
    updateDisplay();
}


// ###############################################################################
void decodeMoveCursor(int dir) {

      knobPositionPrevious = knobPosition;
      #ifdef USE_TUNE2500_MODE
          if (tune2500Mode) { tune2500Mode = 0; return; } // Abort tune2500Mode if Cursor Button is pressed
      #endif // USE_TUNE2500_MODE
      
      cursorDigitPosition += dir;
      cursorDigitPosition = constrain(cursorDigitPosition, 0, 7);
      freqUnStable = 0;  // Set Freq is NOT UnStable, as it is Stable
      blinkTimer = 0;
      if (!cursorDigitPosition) dialCursorMode = true;
      refreshDisplay++;
}

// ###############################################################################
void decodeFN(int btn) {

  switch (getButtonPushMode(btn)) { 
    case MOMENTARY_PRESS:
       ritOn = !ritOn; ritVal = 0;
       break;
      
    case DOUBLE_PRESS:
       #ifdef USE_EDITIF
           if (editIfMode) { // Abort Edit IF Mode, Reload Active VFO
              editIfMode = false;    
              #ifdef USE_PARK_CURSOR
                cursorDigitPosition = 0;
              #endif // USE_PARK_CURSOR
              frequency = (vfoActive == VFO_A) ? vfoA : vfoB; break;
           } 
           else
       #endif // USE_EDITIF
       { // Save Current VFO, Load Other VFO
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
       #ifdef USE_EDITIF
         if (editIfMode) return; // Do Nothing if in Edit-IF-Mode
       #endif // USE_EDITIF
       switch (vfoActive) {
           case VFO_A :
              vfoB = frequency + ritVal;
              sprintf(c, P("A%sB"), ritVal ? P2("+RIT>"): P2(">"));
              printLineCEL(STATUS_LINE, c);
              break;
           default :
              vfoA = frequency + ritVal;
              sprintf(c, P("B%sA"), ritVal ? P2("+RIT>"): P2(">"));
              printLineCEL(STATUS_LINE, c);
              break;
       }
       delay(100);
       break;
    default : return; // Do Nothing
  }
  #ifdef USE_HAMBANDS
      inBandLimits(frequency);
  #endif // USE_NAMBANDS
  deDounceBtnRelease(); // Wait for Release
  refreshDisplay++;
  updateDisplay();
}


// ###############################################################################
void setFreq(unsigned long freq) {

    if (!inTx && ritOn) freq += ritVal;
    freq += isLSB ? iFreqLSB : iFreqUSB;
    vfo->setFrequency(freq);
}


// ###############################################################################
// ###############################################################################
// ###############################################################################
// ###############################################################################

// ###############################################################################
void setup() {
#define DEBUG(x ...)  // Default to NO debug    
//#define DEBUG(x ...) debugUnique(x)    // UnComment for Debug
  
  // Initialize the Serial port so that we can use it for debugging
  Serial.begin(115200);
  debug(P("%s Radiono - Rev: %s"), __func__, P2(RADIONO_VERSION));

#ifdef USE_PCA9546
  // Initialize the PCA9546 multiplexer and select channel 1 
  mux = new PCA9546(PCA9546_I2C_ADDRESS, PCA9546_CHANNEL_1);
  if (mux->status == PCA9546_ERROR) debug(P("PCA9546 init error"));
#endif

  lcd.begin(LCD_COL, LCD_ROW);
  cursorOff();
  printLine(FIRST_LINE, P("Farhan - Minima"));
  printLine(STATUS_LINE, P("  Tranceiver"));
  delay(2000);
  
  sprintf(b, P("Radiono %s"), P2(RADIONO_VERSION));
  printLineCEL(0, b);
  
  sprintf(b, P("Rev: %s"), P2(INC_REV));
  printLineCEL(STATUS_LINE, b);
  delay(2000);
  
  //sprintf(b, P("%s"), __DATE__); // Compile Date and Time
  //sprintf(c, "%3.3s%7.7s %5.5s", b, b+4, __TIME__);
  //printLineCEL(STATUS_LINE, c);
  //delay(2000); 
  
  // Print just the File Name, Added by ERB
  //sprintf(c, P("F: %-13.13s"), P2(__FILE__));
  //printLineCLE(STATUS_LINE, c);
  //delay(2000);


  // The library automatically reads the factory calibration settings of your Si570
  // but it needs to know for what frequency it was calibrated for.
  // Looks like most HAM Si570 are calibrated for 56.320 Mhz.
  // If yours was calibrated for another frequency, you need to change that here
  vfo = new Si570(SI570_I2C_ADDRESS, 56.32 * MHz);

  if (vfo->status == SI570_ERROR) {
    // The Si570 is unreachable. Show an error for 3 seconds and continue.
    printLineCEL(STATUS_LINE, P("Si570 comm error"));
    delay(3000);
  }
  
  printLineCEL(STATUS_LINE, P(" "));
 
  // This will print some debugging info to the serial console.
  vfo->debugSi570();

  // Setup the initial frequency
  vfo->setFrequency(frequency);
 
  // Setup with No SideTone
  stopSidetone();

  // Setup in Receive Mode
  changeToReceive();
  
  // Setup to read Tuning POT
  pinMode(ANALOG_TUNING, INPUT);
  digitalWrite(ANALOG_TUNING, 1); //old way to enable the built-in pull-ups
   
  #ifdef USE_POT_KNOB
    // Setup the First Tuning POT Position
    knobPositionPrevious = knobPosition = analogRead(ANALOG_TUNING);
  #endif // USE_POT_KNOB
  
  // Setup to read Buttons
  pinMode(FN_PIN, INPUT);
  digitalWrite(FN_PIN, 0); // Use an external pull-up of 47K ohm to AREF
  
  #ifdef USE_ENCODER01
     DEBUG(P("Init Encoder"));
     initEncoder(); // Initialize Simple Encoder
  #endif // USE_ENCODER01
  
  #ifdef USE_EEPROM
     DEBUG(P("Pre Load EEPROM"));
     loadUserPerferences();
  #endif // USE_EEPROM
 
  #ifndef USE_PARK_CURSOR
    blinkTimeOut = DEFAULT_BLINK_TIMEOUT;
    cursorDigitPosition = DEFAULT_CURSOR_POSITION;
  #endif // USE_PARK_CURSOR 
    
  #ifdef USE_HIDELEAST
    blinkTimeOut = DEFAULT_BLINK_TIMEOUT;
    blinkPeriod = DEFAULT_BLINK_PERIOD;
    blinkRatio = DEFAULT_BLINK_RATIO;
    cursorDigitPosition = DEFAULT_CURSOR_POSITION; 
  #endif // USE_HIDELEAST
  
  refreshDisplay++; 
}


// ###############################################################################
// ###############################################################################
void loop(){
  unsigned long freq;
  
  #ifdef USE_POT_KNOB
      readPot();
  #endif // USE_POT_KNOB
   
  #ifdef USE_ENCODER01
      readEncoder(ENC_KNOB);
  #endif // USE_ENCODER01
   
  #ifdef USE_MENUS
       // Check if in Menu Mode
      if (menuActive) { doMenus(menuActive); return; };
  #endif
  
  checkTuning();

  checkTX();
 
  checkButton();

  #ifdef USE_EDITIF
      if (editIfMode) {  // Set freq to Current Dial Trail IF Freq + VFO - Prev IF Freq
          freq = frequency;
          if (ritOn) freq += ritVal;
          freq += (vfoActive == VFO_A) ? vfoA : vfoB;
          vfo->setFrequency(freq);
      } else
  #endif // USE_EDITIF 
  setFreq(frequency);
  
  decodeSideband();
  
  setBandswitch(frequency);
  
  #ifdef USE_RF386
    setRf386BandSignal(frequency);
  #endif
  
  updateDisplay();
  
}

// ###############################################################################

//End
