// Scanner.h

#ifndef SCANNER_H
#define SCANNER_H

    enum ScanModes {
        SCAN_STOP_ON_CURRENT_FREQ = -2,
        SCAN_STOP_USE_PREV_VFO,
        SCAN_STOP,
        SCAN_SIMPLE,
        SCAN_BETWEEN_AB,
        SCAN_DN_AB,
        SCAN_UP_AB,
    };
    
    // Externally Available Variables
        extern char autoScanMode;
        extern int autoScanRate;
        extern char scanDir;

    // Externally Available Functions
        extern int autoScanInit(int scanMode, int dir);
        extern void autoScanStop(int mode);
        extern int getAutoScanDir();

#endif // SCANNER_H
// End
