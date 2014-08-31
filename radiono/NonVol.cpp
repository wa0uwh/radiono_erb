// NonVol.cpp
// Store/Load User Preference to/from Non-Volatile Memory

#include <Arduino.h>
#include <avr/eeprom.h>
#include "A1Main.h"
#include "NonVol.h"

// Defines that MUST match Main  
// #define BANDS (9)

#define ID_FLAG (1408281941L)  // YYMMDDHHMM, Used for EEPROM Structure Revision Flag



// Externs Variables from Main
extern unsigned long frequency;
extern int editIfMode;
extern unsigned long iFreqUSB;
extern unsigned long iFreqLSB;
extern unsigned long vfoA;
extern unsigned long vfoB;
extern byte isLSB;
extern byte vfoActive;
extern unsigned long freqCache[];
extern byte sideBandMode;
extern byte sideBandModeCache[];


// Extern Functions from Main
extern void cursorOff();
extern void printLine1CEL(char const *);
extern void printLine2CEL(char const *);
extern void deDounceBtnRelease();
extern int btnDown();

// Local Varibles
long idFlag = ID_FLAG;

/*
// ERB - Buffers that Stores "const stings" to, and Reads from FLASH Memory
// ERB - Force format stings into FLASH Memory
#define  P(x) strcpy_P(buf, PSTR(x))
// FLASH2 can be used where Two small (1/2 size) Buffers are needed.
#define P2(x) strcpy_P(buf + sizeof(buf)/2, PSTR(x))

#define DEBUG(x ...)  // Default to NO debug
*/

// ###############################################################################
void eePromIO(int mode) {
   
   struct config_t {
        long idFlag;
        unsigned long frequency;
        int editIfMode;
        unsigned long iFreqUSB;
        unsigned long iFreqLSB;
        unsigned long vfoA;
        unsigned long vfoB;
        byte isLSB;
        byte vfoActive;
        unsigned long freqCache[BANDS*2];
        byte sideBandMode;
        byte sideBandModeCache[BANDS*2];
        byte checkSum;
    } E;
    byte checkSum = 0;
    byte *pb = (byte *)&E;
    
    if (editIfMode) return; // Do Nothing if in Edit-IF-Mode   
    
    cursorOff();
   
    switch(mode) {
    case EEP_LOAD:
        // Read from Non-Volatile Memory and check for the correct ID
        eeprom_read_block((void*)&E, (void*)0, sizeof(E));
        if (E.idFlag != ID_FLAG) { sprintf(c, P("Load Failed ID")); break; }
        
        // Compute and Check the CheckSum
        for (int i = 0; i < sizeof(E); i++) checkSum += *pb++;
        if (checkSum != 0) { sprintf(c, P("Load Failed CSum")); break; }
        
        // Assign Values to Working Variables from eeProm Structure
        idFlag = E.idFlag; 
        frequency = E.frequency;
        editIfMode = E.editIfMode;
        iFreqUSB = E.iFreqUSB;
        iFreqLSB = E.iFreqLSB;
        vfoA = E.vfoA;
        vfoB = E.vfoB;
        isLSB = E.isLSB;
        vfoActive = E.vfoActive;
        memcpy(freqCache, E.freqCache, sizeof(E.freqCache));
        sideBandMode = E.sideBandMode;
        memcpy(sideBandModeCache, E.sideBandModeCache, sizeof(E.sideBandModeCache));
        checkSum = E.checkSum;
       
        sprintf(c, P("Loading %dB"), sizeof(E));      
        break;
        
    case EEP_SAVE :
        // Assign Working Variables to the eeProm Structure
        E.idFlag = ID_FLAG;
        E.frequency = frequency;
        E.editIfMode = editIfMode;
        E.iFreqUSB = iFreqUSB;
        E.iFreqLSB = iFreqLSB;
        E.vfoA = vfoA;
        E.vfoB = vfoB;
        E.isLSB = isLSB;
        E.vfoActive = vfoActive;
        memcpy(E.freqCache, freqCache, sizeof(E.freqCache));
        E.sideBandMode = sideBandMode;
        memcpy(E.sideBandModeCache, sideBandModeCache, sizeof(E.sideBandModeCache));
        E.checkSum = checkSum;   // Not necessary, used here as an Optical Place Holder
        
        // Compute and save the new Checksum of eeProm Struture
        for (int i = 0; i < sizeof(E) - sizeof(E.checkSum); i++) checkSum += *pb++;
        E.checkSum = -checkSum; // Apply CheckSum to Structure
        
        // Write the eeProm Strcture to Non-Volatile Memory
        eeprom_write_block((const void*)&E, (void*)0, sizeof(E));
        
        sprintf(c, P("Storing %dB"), sizeof(E));
        break;
    }
   
    printLine2CEL(c);
    delay(500);
    deDounceBtnRelease(); // Wait for Release
} 


// ###############################################################################
void loadUserPerferences() {
      
    // Check EEPROM for User Saved Preference, Load if available
    // Hold any Button at Power-ON or Processor Reset does a "Factory Reset" to Default Values
    printLine1CEL(P("User Pref:"));
    if (!btnDown()) eePromIO(EEP_LOAD);
    else printLine2CEL(P("Factory Reset"));
    delay(500);
    deDounceBtnRelease(); // Wait for Button Release 
}


// End
