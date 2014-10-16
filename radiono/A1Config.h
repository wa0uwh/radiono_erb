// A1Config.h

#ifndef A1CONFIG_H
#define A1CONFIG_H

    // Optional USER Configurations             SIZE
    //==========================================================================================
    //#define USE_PCA9546                1  //  214b - Option to include PCA9546 support
    //#define USE_I2C_LCD                1  //  ???b - Option to include i2c LCD support
      #define USE_RF386                  1  //  272b - Option to include RF386 support
      #define USE_BANDPASS               1  //  104b - Option to include Band Pass Filter Output Pins
    //#define USE_BEACONS                1  // 1488b - Option to include Beacons, CW and QRSS support
      #define USE_EEPROM                 1  // 1454b - Option to include Load and Store to NonVolatile Memory (EEPROM) support
    //#define USE_AUTOSAVE_FACTORY_RESET 1  //    8b - Option to Automatically Save Factory Reset Values to NonVolatile Memory on Reset
      #define USE_MENUS                  1  // 4626b - Option to include Menu support
      #define USE_HAMBANDS               1  // 1552b - Option to include Ham Band and Ham Band Limits
      #define USE_KNOB_CAN_CHANGE_BANDS  1  //   34b - Option to Allow Knob to Change Bands, i.e, Cycle Cursor/Digit/Band
      #define USE_TUNE2500_MODE          1  //   30b - Option to include Tune2500Hz Mode
      #define USE_EDITIF                 1  //  842b - Option to include EditIF function
    //#define USE_POT_KNOB               1  // 2304b - Option to include POT support
    //#define USE_ENCODER01              1  // 2220b - Option to include Simple Encoder01 support
    //#define USE_ENCODER02              1  // 2610b - Option to include FULL Two Digital Pin ISR Encoder02 support
      #define USE_ENCODER03              1  // 2604b - Option to include ISR Encoder03 support On Tuning Pin
      #define USE_PARK_CURSOR            1  //   24b - Option to Park Cursor when Mode Changes and/or Timeout
    //#define USE_HIDELEAST              1  //   84b - Option to Hide Least Digits to right of Cursor while Tuning
      #define USE_OPERATE_60M            1  //    6b - Will Include USE_HAMBANDS, Option to Operate and Support 60m Band Selection

#endif

// End
