// DialCal.h

#ifndef DIALCAL_H
#define DIALCAL_H

    // Externally Available Variables
    extern boolean dialCalEditMode;
    extern long dialCalPP100M;
    
    // Externally Available Functions
    extern void toggleDialCal();
    extern void dialCalEditModeCancel();
    extern unsigned long dialCalLimits(unsigned long Freq);

#endif // DIALCAL_H

// End
