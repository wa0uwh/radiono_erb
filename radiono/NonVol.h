// NonVol.h
// NonVolatile Memory I/O

#ifndef NONVOL_H
#define NONVOL_H

    #define EEP_LOAD (0)
    #define EEP_SAVE (1)
    
    void eePromIO(int mode);
    void preLoadUserPerferences();

#endif

// End
