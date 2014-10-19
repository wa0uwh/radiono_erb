// Encoder02.h


#ifndef ENCODER02_H
#define ENCODER02_H

#ifndef ISR_DEBOUNCE_TIMEOUT
    #define ISR_DEBOUNCE_TIMEOUT (5 * MSECs) // Default
#endif

    // Externally Available Variables
    
    // Externally Available Functions
    extern void initEncoder();
    extern  int getEncoderDir();

#endif

// End
