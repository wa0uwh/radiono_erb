// Menus.h

#ifndef MENUS_H
#define MENUS_H

    #define MENU_IDLE_TIMEOUT_SEC (1*60)
    
    // Menu Indexes
    enum Menus {
        M_CW_WPM = 1,
        M_QRSS_DIT_TIME,
        //M_Blank1,
        M_BLINK_PERIOD,
        M_BLINK_RATIO,
        M_BLINK_TIMEOUT,
        //M_Blank2,
        M_TIMEOUT,
        MENUS
    };

    #define DEFAULT_MENU (M_BLINK_PERIOD)

    // Externally Available Variables
    extern byte menuActive;
    extern byte menuPrev;
    
    // Externally Available Functions
    extern void doMenus(int menu);

#endif

// End
