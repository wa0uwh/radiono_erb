// Encoder01.h

#ifndef ENCODER01_H
#define ENCODER01_H

#ifndef ISR_DEBOUNCE_TIMEOUT
    #define ISR_DEBOUNCE_TIMEOUT (50 * MSECs) // Default
#endif

    // Externally Available Variables
    
    // Externally Available Functions
    extern void initEncoder();
    extern int getEncoderDir();
    extern void readEncoder(int btn);

#endif

// End
