// Menus.cpp

#include <Arduino.h>
#include "A1Main.h"
#include "Menus.h"
#include "PotKnob.h"
#include "ButtonUtil.h"
#include "MorseCode.h"
#include "debug.h"

unsigned long menuIdleTimeOut = 0;

// ###############################################################################
void checkKnob(int menu) {
    int dir;
    
    dir = doPotKnob();
    
    if (!dir) return;
    
    menuIdleTimeOut = 0;
    
    switch(menu) {
        case 6:
          debug(P("here"));
          cw_wpm += dir;        
          cw_wpm = constrain (cw_wpm, 1, 99);
          refreshDisplay++;
          break;
        case 7:
          if (qrssDitTime>1000) {
              qrssDitTime += dir * 1000;
              qrssDitTime = qrssDitTime/1000 * 1000;
              qrssDitTime = constrain (qrssDitTime, 1000, 60000);
          }
          else qrssDitTime += dir * 10;        
          qrssDitTime = constrain (qrssDitTime, 250, 60000);
          refreshDisplay++;
          break;
    }
}

// ###############################################################################
void doMenus(int menu) {
    
    if (!menuIdleTimeOut) menuIdleTimeOut = millis() + 1000L * MENU_IDLE_TIMEOUT_SEC;
        
    updateDisplayMenu(menu);
    checkKnob(menu);       
    checkButtonMenu();
    
    if (menuIdleTimeOut && menuIdleTimeOut < millis()) { // If IdleTimeOut, Abort
      menuIdleTimeOut = 0;
      menuActive = 0;
      refreshDisplay+=2;
    }
  
}

// ###############################################################################
void updateDisplayMenu(int menu) {
  if (refreshDisplay) {
      refreshDisplay = max(--refreshDisplay, 0);
      //sprintf(c, P("%0.2d-Menu:"), menu);
      //printLineCEL(MENU_PROMPT_LINE, c);
      switch (menu) {
          case 6:
             sprintf(c, P("%0.2dMACRO CW SPD"), menu);
             printLineCEL(MENU_PROMPT_LINE, c);
             sprintf(c, P("WPM: %0.2d"), cw_wpm);
             printLineCEL(MENU_ITEM_LINE, c);
             break;
          case 7:
             sprintf(c, P("%0.2dMACRO QRSS DIT"), menu);
             printLineCEL(MENU_PROMPT_LINE, c);
             if (qrssDitTime>=1000) sprintf(c, P(" SECs: %0.2d"), qrssDitTime/1000);
             else sprintf(c, P("MSECs: %d"), qrssDitTime);
             printLineCEL(MENU_ITEM_LINE, c);
             break;
          default: 
             sprintf(c, P("%0.2dMenu:"), menu);
             printLineCEL(MENU_PROMPT_LINE, c);
             printLineCEL(MENU_ITEM_LINE, P("---"));
             break;
      }
  }
}

// -------------------------------------------------------------------------------
void checkButtonMenu() {
#define DEBUG(x ...)
//#define DEBUG(x ...) debugUnique(x)    // UnComment for Debug
//#define DEBUG(x ...) debug(x)    // UnComment for Debug
  int btn;
  
  btn = btnDown();
  if (btn) DEBUG(P("%s %d: btn %d"), __func__, __LINE__, btn);

  menuPrev = menuActive;
  switch (btn) {
    case 0: return; // Abort
    case LT_CUR_BTN: printLineCEL(MENU_ITEM_LINE, P("Left"));  break;
    case RT_CUR_BTN: printLineCEL(MENU_ITEM_LINE, P("Right")); break;
    case UP_BTN: menuActive = constrain (menuActive+1, 1, MENUS); break;
    case DN_BTN: menuActive = constrain (menuActive-1, 1, MENUS); break;
    case RT_BTN: switch (getButtonPushMode(btn)) {
            case DOUBLE_PRESS: menuActive = 0; refreshDisplay+=2; break; // Return to VFO Display Mode
            default: break;
            }
  }
  DEBUG(P("%s %d: MenuActive %d"), __func__, __LINE__, menuActive);
  menuIdleTimeOut = 0;
  refreshDisplay++;
  updateDisplayMenu(menuActive);
  deDounceBtnRelease(); // Wait for Button Release
}


// ENd
