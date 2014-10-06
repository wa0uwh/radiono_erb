// Menus.cpp

#include <Arduino.h>
#include "A1Main.h"
#include "Menus.h"
#include "NonVol.h"
#include "PotKnob.h"
#include "Encoder01.h"
#include "ButtonUtil.h"
#include "MorseCode.h"
#include "debug.h"

byte menuActive = 0;
byte menuPrev = 0;
unsigned long menuIdleTimer = 0;
boolean menuCycle = true;

// Local Functions
void checkKnob(int menu);
void checkButtonMenu();
void updateDisplayMenu(int menu);

// ###############################################################################
void doMenus(int menu) {
#define DEBUG(x...)
//#define DEBUG(x...) debugUnique(x)    // UnComment for Debug

    DEBUG(P("%s/%d: Start Menu"), __func__, __LINE__);
    
    if (menuIdleTimeOut < 5 * SEC) menuIdleTimeOut = 0;

    if (!menuIdleTimer) menuIdleTimer = millis() + menuIdleTimeOut;

    DEBUG(P("%s/%d: menuIdleTimeOut= %ld"), __func__, __LINE__, menuIdleTimeOut);
    
    checkKnob(menu);
    checkButtonMenu();

    if (menuIdleTimeOut && menuIdleTimer && menuIdleTimer < millis()) { // If IdleTimeOut, Abort
      DEBUG(P("%s/%d: Stop Menu"), __func__, __LINE__);
      menuCycle = true;
      menuIdleTimer = 0;
      menuActive = 0;
      refreshDisplay++;
    }

    if(menuActive) updateDisplayMenu(menu);
}


// ###############################################################################
void checkKnob(int menu) {
#define DEBUG(x...)
//#define DEBUG(x...) debugUnique(x)    // UnComment for Debug
    int dir;
    
    DEBUG(P("%s/%d:"), __func__, __LINE__);
    
    dir = 0;
      
    #ifdef USE_POT_KNOB
        dir += getPotDir(); // Get Tuning Direction from POT Knob
    #endif // USE_POT_KNOB
      
    #ifdef USE_ENCODER01
        dir += getEncoderDir(); // Get Tuning Direction from Encoder Knob
    #endif // USE_ENCODER01
    
    if (!dir) return;
 
    menuIdleTimer = 0;
    
    if (menuCycle) { // Cycle or Page Through Menus
        menuActive = constrain (menuActive + dir, 1, MENUS-1);
        refreshDisplay++;
        updateDisplayMenu(menuActive);
        return;
    }
      
    DEBUG(P("%s/%d: Menu = %d"), __func__, __LINE__, menu);
    
    switch(menu) {
        case M_CW_WPM:
          cw_wpm += dir;        
          cw_wpm = constrain (cw_wpm, 1, 99);
          break;
        case M_QRSS_DIT_TIME:
          if (qrssDitTime > SEC) {
              qrssDitTime = (qrssDitTime + dir * SEC) / SEC * SEC;
              qrssDitTime = max(qrssDitTime, 1 * SEC);
          }
          else qrssDitTime += dir * 10;       
          qrssDitTime = constrain (qrssDitTime, 250, 60 * SEC); //MSECs
          break;
          
        case M_BLINK_TIMEOUT:
          if (blinkTimeOut > MIN) {
              blinkTimeOut = (blinkTimeOut + dir * MIN) / MIN * MIN;
              blinkTimeOut = max(blinkTimeOut, 1 * MIN);
          }
          else  blinkTimeOut += dir * 5 * SEC;
          if( blinkTimeOut > 3 * 60 * MIN) blinkTimeOut = 0;
          blinkTimeOut = blinkTimeOut / SEC * SEC;
          blinkTimeOut = constrain (blinkTimeOut, 0, 60 * MIN); //MSECs
          break;
          
        case M_BLINK_PERIOD:
          blinkPeriod += dir * 10;
          blinkPeriod = constrain (blinkPeriod, 100, 2 * SEC); //MSECs
          break;
        case M_BLINK_RATIO:
          blinkRatio += dir * 5;
          blinkRatio = constrain (blinkRatio, 20, 95); // Percent 
          break;
          
        case M_TIMEOUT:
          if (menuIdleTimeOut > MIN) {
              menuIdleTimeOut = (menuIdleTimeOut + dir * MIN) / MIN * MIN;
              menuIdleTimeOut = max(menuIdleTimeOut, 1 * MIN);
          }
          else  menuIdleTimeOut += dir * 5 * SEC;
          if( menuIdleTimeOut > 3 * 10 * MIN) menuIdleTimeOut = 0;
          menuIdleTimeOut = menuIdleTimeOut / SEC * SEC; 
          menuIdleTimeOut = constrain (menuIdleTimeOut, 0, 10 * MIN); //MSECs
          break;

        default:;
    }
    refreshDisplay++;
}


// ###############################################################################
void updateDisplayMenu(int menu) {
#define DEBUG(x...)
//#define DEBUG(x...) debugUnique(x)    // UnComment for Debug

    DEBUG(P("%s/%d:"), __func__, __LINE__);

  if (refreshDisplay > 0) {
      refreshDisplay--;

      //sprintf(c, P("%0.2d-Menu:"), menu);
      //printLineCEL(MENU_PROMPT_LINE, c);      
      DEBUG(P("%s/%d: Menu = %d"), __func__, __LINE__, menu);
      switch (menu) {
          case 0: // Exit Menu System
             sprintf(c, P("Exit Menu"), menu);
             printLineCEL(MENU_PROMPT_LINE, c);
             printLineCEL(MENU_ITEM_LINE, P(" "));
             break;

          case M_CW_WPM:
             sprintf(c, P("%0.2dMACRO CW SPD"), menu);
             printLineCEL(MENU_PROMPT_LINE, c);
             sprintf(c, P("WPM: %0.2d"), cw_wpm);
             if(!menuCycle) sprintf(c, P2("%s<"), c);
             printLineCEL(MENU_ITEM_LINE, c);
             break;
          case M_QRSS_DIT_TIME:
             sprintf(c, P("%0.2dMACRO QRSS DIT"), menu);
             printLineCEL(MENU_PROMPT_LINE, c);
             if (qrssDitTime > SEC) sprintf(c, P(" SECs: %0.2d"), qrssDitTime / SEC);
             else sprintf(c, P("MSECs: %d"), qrssDitTime);
             if(!menuCycle) sprintf(c, P2("%s<"), c);
             printLineCEL(MENU_ITEM_LINE, c);
             break;

          case M_BLINK_TIMEOUT:
             sprintf(c, P("%0.2dBlink TimeOut"), menu);
             printLineCEL(MENU_PROMPT_LINE, c);
             if(blinkTimeOut > MIN) sprintf(c, P("MINs: %0.2d"), blinkTimeOut / MIN);
             else sprintf(c, P("SECs: %d"), blinkTimeOut / SEC);
             if(!menuCycle) sprintf(c, P2("%s<"), c);
             if(!blinkTimeOut) sprintf(c, P2("%s - OFF"), c);
             printLineCEL(MENU_ITEM_LINE, c);
             break;
          case M_BLINK_PERIOD:
             sprintf(c, P("%0.2dBlink Period"), menu);
             printLineCEL(MENU_PROMPT_LINE, c);
             sprintf(c, P("MSECs: %d"), blinkPeriod);
             if(!menuCycle) sprintf(c, P2("%s<"), c);
             printLineCEL(MENU_ITEM_LINE, c);
             break;
          case M_BLINK_RATIO:
             sprintf(c, P("%0.2dBlink"), menu);
             printLineCEL(MENU_PROMPT_LINE, c);
             sprintf(c, P("Ratio: %d%%"), blinkRatio);
             if(!menuCycle) sprintf(c, P2("%s<"), c);
             printLineCEL(MENU_ITEM_LINE, c);
             break;
             
          case M_TIMEOUT:
             sprintf(c, P("%0.2dMenu TimeOut"), menu);
             printLineCEL(MENU_PROMPT_LINE, c);
             if(menuIdleTimeOut > MIN) sprintf(c, P("MINs: %0.2d"), menuIdleTimeOut / MIN);
             else sprintf(c, P("SECs: %d%"), menuIdleTimeOut / SEC);
             if(!menuCycle) sprintf(c, P2("%s<"), c);
             if(!menuIdleTimeOut) sprintf(c, P2("%s - OFF"), c);
             printLineCEL(MENU_ITEM_LINE, c);
             break;

          default: // A Blank Menu Item
             sprintf(c, P("%0.2dMenu:"), menu);
             printLineCEL(MENU_PROMPT_LINE, c);
             printLineCEL(MENU_ITEM_LINE, P(" -"));
             break;
      }
  }
}

// ###############################################################################
void checkButtonMenu() {
#define DEBUG(x ...)
//#define DEBUG(x ...) debugUnique(x)    // UnComment for Debug
//#define DEBUG(x ...) debug(x)    // UnComment for Debug
  int btn;
  
  btn = btnDown();
  if (btn) DEBUG(P("%s/%d: btn %d"), __func__, __LINE__, btn);

  menuPrev = menuActive;
  switch (btn) {
    case 0: return; // Abort
    case UP_BTN: menuCycle = true; menuActive = constrain (menuActive+1, 1, MENUS-1); break;
    case DN_BTN: menuCycle = true; menuActive = constrain (menuActive-1, 1, MENUS-1); break;
    case LT_BTN:
         switch (getButtonPushMode(btn)) { 
             #ifdef USE_EEPROM
                 case DOUBLE_PRESS: eePromIO(EEP_LOAD); break;
                 case LONG_PRESS:   eePromIO(EEP_SAVE); break;
             #endif // USE_EEPROM
             default: break; // Do nothing
         }
         break;
    case RT_BTN:
        switch (getButtonPushMode(btn)) {
            case MOMENTARY_PRESS: menuCycle = !menuCycle; break;
            case DOUBLE_PRESS:    menuCycle = true; menuActive = 0; refreshDisplay+=2; break; // Return to VFO Display Mode
            default: break; // Do nothing
        } 
        break;
     case ENC_KNOB: readEncoder(btn); break;
     default: decodeAux(btn); break;
  }
  DEBUG(P("%s/%d: MenuActive %d"), __func__, __LINE__, menuActive);
  menuIdleTimer = 0;
  refreshDisplay++;
  updateDisplayMenu(menuActive);
  deDounceBtnRelease(); // Wait for Button Release
}


// ENd
