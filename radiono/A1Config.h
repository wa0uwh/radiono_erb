// A1Config.h

#ifndef A1CONFIG_H
#define A1CONFIG_H

    // Optional USER Configurations         FLASH-SIZE
    //==========================================================================================
    //#define USE_PCA9546                1  //  214b - Option to Include PCA9546 support
    //#define USE_Si570_BFO              1  //  580b - Use second Si570 for BFO
      #define USE_DIAL_CAL               1  //  908b - Dial Calibration, use only with USE_Si570_BFO, otherwize use USE_EDITIF for DialCal
    //#define USE_I2C_LCD                1  //  ???b - Option to Include i2c LCD support
      #define USE_RF386                  1  //  272b - Option to Include RF386 support
    //#define USE_BANDPASS               1  //  104b - Option to Include Band Pass Filter Output Pins
      #define USE_BEACONS                1  // 1488b - Option to Include Beacons, CW and QRSS support
      #define USE_EEPROM                 1  // 1454b - Option to Include Load and Store to NonVolatile Memory (EEPROM) support
    //#define USE_AUTOSAVE_FACTORY_RESET 1  //    8b - Option to Automatically Save Factory Reset Values to NonVolatile Memory on Reset
      #define USE_MENUS                  1  // 4626b - Option to Include Menu support
      #define USE_AUTOSCANNER            1  //  362b - Option to Include Auto Freq Scan Mode; btn7 with Alt UP or DOWN, Knob sets rate
      #define USE_SHOW_WAVE_LENGTH       1  //  364b - Option to Display Wave Length, Computed (m) or Contrived (M) for Ham Bands
      #define USE_HAMBANDS               1  // 1552b - Option to Include Ham Band and Ham Band Limits
    //#define USE_BLINK_DIGIT_MODE       1  //  346b - Option to Bink Digit for Cursor, use when Hardware does not support an Underline Cursor
    //#define USE_KNOB_CAN_CHANGE_BANDS  1  //   34b - Option to Allow Knob Press to Change Bands, i.e, Cycle Cursor/Digit/Band
    //#define USE_DISPLAY_KNOB_MODE      1  //   52b - Option, A Training Mode, to Display Knob Mode for: Cursor/Digit/Band as C/D/B
      #define USE_TUNE2500_MODE          1  //   30b - Option to Include Tune2500Hz Mode
      #define USE_EDITIF                 1  //  842b - Option to Include Edit IF Frequency Function
    //#define USE_DIAL_MOMENTUM          1  //    6b - Option to Enable Dial Momentum, Spin the Dial watch the Digits Catch up (is slower).
    //#define USE_POT_KNOB               1  // 2304b - Option to Include Tuning POT support
    //#define USE_ENCODER01              1  // 2220b - Option to Include Simple analog pin Encoder01 support
    //#define USE_ENCODER02              1  // 2610b - Option to Include FULL Two Digital Pin ISR Encoder02 support
    //#define USE_ENCODER03              1  // 2604b - Option to Include ISR Encoder03 support On Tuning Pin
      #define USE_ENCODER04              1  // 1096b - Option to Include rotary encoder support from Jeff Whitlatch - ko7m
    //#define USE_PARK_CURSOR            1  //   24b - Option to Park Cursor when Mode Changes and/or Timeout
    //#define USE_HIDELEAST              1  //   84b - Option to Hide Least Digits to right of Cursor while Tuning
      #define USE_OPERATE_60M            1  //    6b - Will Include USE_HAMBANDS, Option to Operate and Support 60m Band Selection
      #define USE_10M_SECTIONS           1  //   40b - Split 10m into three subbands: 28.0 - 28.3 - 29.0 - 29.7MHz, Note: Change this Option, Forces Factory Reset
      #define USE_80M_SECTIONS           1  //   20b - Split 80m into 75m and 80m Bands. Note: Change this Option, Forces Factory Reset
      #define USE_DDS_SIDETONE           1  // 2530b - Used DDS for Side Tone Generations

#endif

// End
