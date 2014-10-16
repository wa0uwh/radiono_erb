// Encoder03.h

#ifndef ENCODER03_H
#define ENCODER03_H

#ifndef ISR_DEBOUNCE_TIMEOUT
    #define ISR_DEBOUNCE_TIMEOUT (50 * MSECs) // Default
#endif

    // Externally Available Variables
    
    // Externally Available Functions
    extern void initEncoder();
    extern  int getEncoderDir();

#endif

// End
