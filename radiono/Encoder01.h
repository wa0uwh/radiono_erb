// Encoder01.h

#ifndef ENCODER01_H
#define ENCODER01_H

    #define ENCODER_PINS PCINT21
    #define ENCODER_PINA PCINT22
    #define ENCODER_PINB PCINT23
    


    // Externally Available Variables
    
    // Externally Available Functions
    extern void initEncoder();
    extern int readEncoder();
    extern int getEncDir();
    extern int getKnob(int btn);

#endif

// End
