// DDS.h

#ifndef DDS_H
#define DDS_H

    // Externally Available Variables
    extern volatile  int dds_frequency;
    extern volatile byte dds_amplitude;

    // Externally Available Functions
    extern void ddsInit();
    extern void dddShutdown();
    extern void ddsPause();
    extern void ddsPhaseShift();
        
    extern void ddsAdjFreq(int freq);
    extern void ddsAdjAmplitude(byte amp);
    
    extern void ddsTone();
    extern void ddsTone(int freq);
    extern void ddsTone(int freq, byte amp);
    
    // The following is used by Menu System
    extern void dds_Menu_Freq_Adj(int dir);
    extern void dds_Menu_Amplitude_Adj(int dir);
    extern void dds_Menu_Freq_Display(int menuNum, boolean editMode);
    extern void dds_Menu_Amplitude_Display(int menuNum, boolean editMode);

#endif // DDS_H

// End
