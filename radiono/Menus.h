// Menus.h

#ifndef MENUS_H
#define MENUS_H

    #define MENU_IDLE_TIMEOUT_SEC (1*60)
    
    // Menu Indexes
    enum Menus {
        M_TIMEOUT = 1,
        M_OPT60M,
        //M_Blank0,
        #ifdef USE_DDS_SIDETONE
          M_DDS_TONE,
          M_DDS_AMPLITUDE,
        #endif // USE_DDS_SIDETONE
        //M_Blank1,
        M_CW_WPM,
        M_QRSS_DIT_TIME,
        //M_Blank21,
        M_PARK_TIMEOUT,   // Cursor Park Time Out
        M_BLINK_PERIOD,
        M_BLINK_RATIO,
        //M_Blank3,
        MENUS
    };

    #define DEFAULT_MENU (M_TIMEOUT)

    // Externally Available Variables
    extern byte menuActive;
    extern byte menuPrev;
    
    // Externally Available Functions
    extern void doMenus(int menu);

#endif

// End
