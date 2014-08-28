// Send Morse Code


#include <Arduino.h>
#define MOD_CW (0)
#define MOD_QRSS (1)

// Externs from main
extern unsigned long frequency;
extern unsigned long cwTimeout;
extern byte editIfMode;
extern byte keyDown;
extern byte inPtt;
extern byte inTx;
extern byte AltTxVFO;

extern void printLine2CEL(char const *);
extern void printLine2(char const *);
extern void startSidetone();
extern void stopSidetone();
extern void changeToTransmit();
extern void changeToReceive();
extern int inBandLimits(unsigned long);
extern int isKeyNowClosed();
extern int isPttPressed();
extern void setFreq(unsigned long);


// Local
long ditLen = 1200/13; // Default Speed
#include "MorseTable.h"


// ########################################################
// ########################################################
// ########################################################


// ########################################################
void watchDog(unsigned long timeOut) {
    
    timeOut += millis();
    while(timeOut > millis()) {
        if (isKeyNowClosed()) return; // Abort Message
        delay(20);
    }
}
// ########################################################
void sendBit(int mode, int freqShift, int mult) {
    
    switch (mode) {
    case MOD_CW:   startSidetone(); break;
    case MOD_QRSS: setFreq(frequency + freqShift); break;
    }
    
    watchDog(ditLen * mult);
    
    switch (mode) {
    case MOD_CW:   stopSidetone(); break;
    case MOD_QRSS: setFreq(frequency); break;
    }
}

void dit(int mode, int freqShift) {
    sendBit(mode, freqShift, 1);
}

void dah(int mode, int freqShift) {
    sendBit(mode, freqShift, 3);
}


// ########################################################
void sendMesg(int mode, int freqShift, char *c) {
    byte bits = 0;
    unsigned long timeOut;
    
    if (AltTxVFO) return; // Macros and Beacons not allowed in Split Mode, for now.
    if (editIfMode) return; // Do Nothing if in Edit-IF-Mode       
    if (!inBandLimits(frequency)) return; // Do nothing if TX is out-of-bounds
    if (isKeyNowClosed()) return; // Abort Message
    
    inTx = 1; 
    changeToTransmit();
    printLine2CEL(" "); // Clear Line 2
    delay(50);
     
    if(mode == MOD_QRSS) startSidetone();        
    printLine2CEL(c); // Scroll Message on Line 2
    watchDog(ditLen);
    
    while(*c) {
        if (isKeyNowClosed()) return; // Abort Message
        if (*c == ' ') {
           c++;
           printLine2CEL(c); // Scroll Message on Line 2
           watchDog(ditLen * 4); // 3 for previous character + 4 for word = 7 total
        }
        else {
            bits = pgm_read_byte(&morse[*c & 0x7F - 32]);
            while(bits > 1) {
                if (isKeyNowClosed()) return; // Abort Message
                bits & 1 ? dit(mode, freqShift) : dah(mode, freqShift);
                watchDog(ditLen);
                bits /= 2;
            }
            c++;
            printLine2CEL(c); // Scroll Message on Line 2
            watchDog(ditLen * 2); // 1 + 2 added between characters
        } 
        cwTimeout = ditLen * 10 + millis();
    } // main checkTX() will clean-up and stop Transmit
}

// ########################################################
void sendQrssMesg(long len, int freqShift, char *c) {
    
    ditLen = len < 0 ? -len : len * 1000;
    sendMesg(1, freqShift, c);
}

// ########################################################
void sendMorseMesg(int wpm, char *c) {
    
    ditLen = int(1200 / wpm); 
    sendMesg(0, 0, c);  
}

// End
  

