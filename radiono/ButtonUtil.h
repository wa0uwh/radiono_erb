// ButtonUtil.h

#ifndef BUTTONUTIL_H
#define BUTTONUTIL_H

  enum ButtonPressModes { // Button Press Modes
        MOMENTARY_PRESS = 1,
        DOUBLE_PRESS,
        LONG_PRESS,
        ALT_PRESS_FN,
        ALT_PRESS_LT_CUR,
        ALT_PRESS_RT_CUR,
        ALT_PRESS_LT,
        ALT_PRESS_UP,
        ALT_PRESS_DN,
        ALT_PRESS_RT,
    };
    
    enum Buttons { // Button Numbers
        FN_BTN = 1,
        LT_CUR_BTN,
        RT_CUR_BTN,
        LT_BTN,
        UP_BTN,
        DN_BTN,
        RT_BTN,
        BUTTONS
    };

    extern  int btnDown();
    extern void deDounceBtnRelease();
    extern void decodeAux(int btn);
    extern  int getButtonPushMode(int btn);

#endif

// End
