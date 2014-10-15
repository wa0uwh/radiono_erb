// Encoder01.h

#ifndef ENCODER01_H
#define ENCODER01_H

#ifndef ISR_DEBOUNCE_TIME_OUT
    #define ISR_DEBOUNCE_TIME_OUT (50 * MSECs) // Default
#endif

    // Externally Available Variables
    
    // Externally Available Functions
    extern void initEncoder();
    extern int getEncoderDir();
    extern void readEncoder(int btn);

#endif

// End
