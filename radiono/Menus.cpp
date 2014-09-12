// Menus.cpp

#include <Arduino.h>
#include "A1Main.h"
#include "Menus.h"
#include "debug.h"


// ###############################################################################
void doMenus(int menu) {
    updateDisplayMenu(menu);
    checkButtonMenu();
}

// ###############################################################################
void updateDisplayMenu(int menu) {
  if (refreshDisplay) {
      refreshDisplay = max(--refreshDisplay, 0);
      sprintf(c, P("Menu%0.2d:"), menu);
      printLineCEL(MEMU_PROMPT_LINE, c);
      printLineCEL(MENU_ITEM_LINE, P(" "));
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
  refreshDisplay++;
  updateDisplayMenu(menuActive);
  deDounceBtnRelease(); // Wait for Button Release
}
