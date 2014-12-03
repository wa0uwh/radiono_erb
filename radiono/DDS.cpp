// DDS.cpp

// Sine wave generator using DDS techniques

/*
 * Copyright (C) 2014, by Author: Jeff Whitlatch - ko7m, and
 * Copyright (C) 2014, by Author: Eldon R. Brown (ERB) - WA0UWH
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of  MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
 
 
#include <Arduino.h>
#include "avr/pgmspace.h"
#include "A1Main.h"
#include "DDS.h"
#include "debug.h"


#ifdef USE_DDS_SIDETONE


#ifndef SIDETONE_PIN 
  #define SIDETONE_PIN (PD3) // Or,
  //#define SIDETONE_PIN (PB3)
#endif // SIDETONE_PIN

/*
#define OC2RX  // NoOp
#if (SIDETONE_PIN) == (PB3)
  #define OCR2x OCR2A
#elif (SIDETONE_PIN) == (PD3)
  #define OCR2x OCR2B
#endif
*/

#define LO_FREQ (100)
#define HI_FREQ (2000)
#define LO_AMP (0)
#define HI_AMP (100)


// Single 1/4 period (quadrant) sine wave table.
// Amplitudes values are 0 to 127.  64 samples per period/4.
PROGMEM const char sineTab[64+1] = {
      0,   3,   6,   9,  12,  16,  19,  22, 
     25,  28,  31,  34,  37,  40,  43,  46, 
     49,  51,  54,  57,  60,  63,  65,  68, 
     71,  73,  76,  78,  81,  83,  85,  88, 
     90,  92,  94,  96,  98, 100, 102, 104, 
    106, 107, 109, 111, 112, 113, 115, 116, 
    117, 118, 120, 121, 122, 122, 123, 124, 
    125, 125, 126, 126, 126, 127, 127, 127, 128,
};


// Useful macros for setting and resetting bits
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |=  _BV(bit))

// DDS frequency and reference clock
const double ref_frequency = (16 * MHz / 500);

// Initializaton Stataus
boolean ddsInitialized = false;

// These must all be marked as volatile as they are used in an interrupt service routine
volatile byte dds_amplitude = HI_AMP;
volatile  int dds_frequency = 1000;

volatile uint16_t phase_accumulator;
volatile uint16_t tuning_word;

// These are fore Implementing Attack/Decay and Reverse Phase
volatile byte QuadPrev = 0;
volatile int rampRate = 5; // Ramp applies to this number of cycles, Times 2 for Phase Shifted Output
volatile int rampCur = 0;
volatile int rampVal = 0;
volatile int rampDenom = 0;

volatile boolean phaseShift = false;




// ###############################################################################
// ###############################################################################
// ###############################################################################

// ###############################################################################
// Setup timer2 with prescaler = 1, PWM mode to phase correct PWM
// See the ATMega328 datasheet for all the gory details
void timer2Setup() {
#define DEBUG(x ...)
//#define DEBUG(x ...) debugUnique(x)    // UnComment for Debug

    DEBUG(P("%s/%d:"), __func__, __LINE__);
 
    TIMSK2 = 0;
    TCCR2A = _BV(COM2A1) | _BV(COM2B1) | _BV(WGM20);
    TCCR2B = _BV(CS20);
}


// ###############################################################################
  // PWM output for timer2 on an ATMega328 (such as the UNO) is pin 11 (PD5)
  // If you use an ATMega2560 you need to make this pin 10
  // See spreadsheet here
void ddsInit() {
#define DEBUG(x ...)
//#define DEBUG(x ...) debugUnique(x)    // UnComment for Debug

    DEBUG(P("\n%s/%d: Pin: %d"), __func__, __LINE__, SIDETONE_PIN);

    pinMode(SIDETONE_PIN, OUTPUT);  // Timer 2 PWM output on mega328 PB3 or PD3
    timer2Setup();                  // Set up timer2 to a phase correct 32kHz clock
    ddsInitialized = true;
}
       
// ###############################################################################
void ddsPause() {
#define DEBUG(x ...)
//#define DEBUG(x ...) debugUnique(x)    // UnComment for Debug

    int i = 0;

    DEBUG(P("%s/%d: rampRate: %d"), __func__, __LINE__, rampRate);
 
    //uint8_t oldSREG = SREG;
    //cli();
       OCR2B = 128;
       rampRate = 0;
       DEBUG(P("%s/%d: %d, %dR, %dC"), __func__, __LINE__, i, rampRate, rampCur);
    //SREG = oldSREG;
    
    DEBUG("\n");

}
       
// ###############################################################################
void ddsShutDown() {
#define DEBUG(x ...)
//#define DEBUG(x ...) debugUnique(x)    // UnComment for Debug

    DEBUG(P("%s/%d:"), __func__, __LINE__);

    ddsPause();
    TIMSK2 = 0; 
    ddsInitialized = false;
}

// ###############################################################################
void ddsAdjAmplitude(byte amp){
#define DEBUG(x ...)
//#define DEBUG(x ...) debugUnique(x)    // UnComment for Debug

    DEBUG(P("%s/%d: Amplitude= %d"), __func__, __LINE__, amp);

    uint8_t oldSREG = SREG;
    cli();
      dds_amplitude = constrain(amp, LO_AMP, HI_AMP);
    SREG = oldSREG;
}

// ###############################################################################
void ddsAdjFreq(int freq){
#define DEBUG(x ...)
//#define DEBUG(x ...) debugUnique(x)    // UnComment for Debug

    DEBUG(P("%s/%d: Freq= %d"), __func__, __LINE__, freq);

    uint8_t oldSREG = SREG;
    cli();
      dds_frequency = constrain (freq, LO_FREQ, HI_FREQ);
      // Set up initial DDS frequency and calculate the timing word
      tuning_word = (pow(2,16) * dds_frequency) / ref_frequency;
    SREG = oldSREG;
}

// ###############################################################################
void ddsAdjRampRate(int rate){
#define DEBUG(x ...)
//#define DEBUG(x ...) debugUnique(x)    // UnComment for Debug

    DEBUG(P("%s/%d: Rate= %d"), __func__, __LINE__, rate);

    uint8_t oldSREG = SREG;
    cli();
        rampRate = rate;
    SREG = oldSREG;
}
// ###############################################################################
void ddsPhaseShift() { // Shift Phase
#define DEBUG(x ...)
//#define DEBUG(x ...) debugUnique(x)    // UnComment for Debug

    DEBUG(P("%s/%d:"), __func__, __LINE__);

    ddsAdjRampRate(-rampRate);
}

// ###############################################################################
void ddsTone() {
#define DEBUG(x ...)
//#define DEBUG(x ...) debugUnique(x)    // UnComment for Debug

    DEBUG(P("%s/%d:"), __func__, __LINE__);

    ddsTone(dds_frequency);  
}

// ###############################################################################
void ddsTone(int freq) {
#define DEBUG(x ...)
//#define DEBUG(x ...) debugUnique(x)    // UnComment for Debug

    DEBUG(P("%s/%d: Freq: %d"), __func__, __LINE__, freq);

    if (!dds_amplitude) ddsAdjAmplitude(HI_AMP);
    ddsTone(freq, dds_amplitude);  
}

// ###############################################################################
void ddsTone(int freq, byte amp){
#define DEBUG(x ...)
//#define DEBUG(x ...) debugUnique(x)    // UnComment for Debug

    DEBUG(P("%s/%d: Freq: %d, Amp: %d"), __func__, __LINE__, freq, amp);

    if (!freq) { ddsPause(); return; } // Pause Tone
    if (!ddsInitialized) ddsInit();
  
    ddsAdjAmplitude(amp);
    ddsAdjFreq(freq);
    
    ddsAdjFreq(600);    // FOR DEBUG ===========================
    ddsAdjRampRate(6);  // FOR DEBUG ===========================
    
    DEBUG(P("%s/%d: ddaFreq: %d, ddsAmp: %d"), __func__, __LINE__, dds_frequency, dds_amplitude);
  
    sbi (TIMSK2,TOIE2);    // Enable timer 2.
}


// ###############################################################################
// Timer 2 interrupt service routine (ISR) is used to generate
// the timebase reference clock for the DDS generator at 32kHz.
ISR(TIMER2_OVF_vect) {

    // Update phase accumulator and extract the sine table index from it
    phase_accumulator += tuning_word;
    volatile byte sine_table_index = phase_accumulator >> 8;  // Use upper 8 bits as index
    
    volatile byte Tmp = 0;
    volatile byte Quadrant = sine_table_index >> 6;
    volatile byte Index = sine_table_index % 64;
    
    switch (Quadrant) { // Compute Table Indes
        case 0: case 2: Tmp = 00 + Index; break;
        case 1: case 3: Tmp = 63 - Index; break;
    }
    
    Tmp = pgm_read_byte_near(sineTab + Tmp ); // Get Table Value
    Tmp = (Tmp * dds_amplitude) / HI_AMP;     // Apply Level Control

    // The following code implements Amplitude Ramps at Tone Ends, and at Phase Shift
    // Apply Attack/Decay, using Ramp Computed Functions
    // Compute RampVal at First Data Point, then apply to all succesive Data Points of Output Tone Cycle
    if(QuadPrev == 3 && Quadrant == 0) {  
        
        if(!rampRate && !rampCur) { // Finish Tone at Zero Crossover
            rampVal = 0;
            rampDenom = 0;
        }
        else 
            if (rampRate == rampCur) {  // Ramp Finished when rampRate and rampCur are equal, at +/-rampRate
                rampVal = 128;
                rampDenom = 0;
            }
            else { // Compute and Apply Ramp, And Apply Phase Shift
                rampVal = 128;  // Assume 100% rampVal
       
                if (!rampDenom) rampDenom = rampRate ? rampRate : rampCur;

                //int Delta = rampRate - rampCur; rampCur += (Delta > 0) - (Delta < 0);
                rampCur += (rampRate > rampCur) - (rampRate < rampCur); // Inc rampCur toward rampRate
                
                if(rampRate && !rampCur) phaseShift = !phaseShift; // At Zero Crossing, Toggle PhaseShift
                
                //if(rampDenom) rampVal = (128 * abs(rampCur)) / abs(rampDenom); // Compute Linear Ramp                
                if(rampDenom) rampVal = (abs(rampCur)<<7) / abs(rampDenom); // Compute Linear Ramp
                
                // Convert Linear Ramp to Shaped Ramp
                
                // Get Ramp Shape from Sin Table
                // Bulge
                //rampVal = pgm_read_byte_near(sineTab + (rampVal / 2) ); // Get Table Value to Shape Ramp
                rampVal = pgm_read_byte_near(sineTab + (rampVal>>1) ); // Get Table Value to Shape Ramp
                
                // Or, Concave
                //rampVal = 128 - pgm_read_byte_near(sineTab + (64-(rampVal>>1)) ); // Get Table Value to Shape Ramp
                
                
                
                // Or, build Shape from this code
                //switch (abs(rampCur)) {
                    /*   // Bulge
                    case 0: rampVal = 0; break;
                    case 1: rampVal = 60; break;
                    case 2: rampVal = 90; break;
                    case 3: rampVal = 110; break;
                    case 4: rampVal = 128; break;
                    default: rampVal = 128;
                    */
                    /*    // Or, Concave
                    case 0: rampVal = 0; break;
                    case 1: rampVal = 8; break;
                    case 2: rampVal = 16; break;
                    case 3: rampVal = 64; break;
                    case 4: rampVal = 128; break;
                    default: rampVal = 128;
                    */
                //}
             }
    }
    
    //Tmp = (Tmp * rampVal) / 128;  // Apply Ramp Control Value
    Tmp = (Tmp * rampVal)>>7;  // Apply Ramp Control Value

    if (phaseShift) Tmp = -Tmp;  // Apply Phase Shift
    
    switch (Quadrant) { // Update to PWM
        case 0: case 1: OCR2B = 128 + Tmp; break;
        case 2: case 3: OCR2B = 128 - Tmp; break;
    } 
    
    QuadPrev = Quadrant;
}


// ###############################################################################
// #  MENU System  ###############################################################
// ###############################################################################

// ###############################################################################
void dds_Menu_Freq_Adj(int dir) {
    dds_frequency = constrain ((int) dds_frequency + dir * 10, LO_FREQ, HI_FREQ);
    ddsAdjFreq(dds_frequency);
}
    
void dds_Menu_Amplitude_Adj(int dir) {
    dds_amplitude = (byte) constrain ((int) dds_amplitude + dir, LO_AMP, HI_AMP);
    ddsAdjAmplitude(dds_amplitude);
}


// ###############################################################################

/*
void dds_Menu_Freq_Display(int menuNum, boolean editMode) {
    sprintf(c, P("%0.2d: SideTone"), menuNum);
    printLineCEL(MENU_PROMPT_LINE, c);
    sprintf(c, P("Hz: %4d"), dds_frequency);
    if (editMode) sprintf(c, P2("%s<"), c);
    printLineCEL(MENU_ITEM_LINE, c);
}
                               
void dds_Menu_Amplitude_Display(int menuNum, boolean editMode) {
    sprintf(c, P("%0.2d: SideTone"), menuNum);
    printLineCEL(MENU_PROMPT_LINE, c);
    sprintf(c, P("Amp: %3d%%"), dds_amplitude);
    if (editMode) sprintf(c, P2("%s<"), c);
    printLineCEL(MENU_ITEM_LINE, c);
}
*/


// This will be moved to Menu.cpp
void menu_Prompt(int menuNum, int intVal, char *prompt1, char *prompt2, boolean editMode) {
    sprintf(c, prompt1, menuNum);
    printLineCEL(MENU_PROMPT_LINE, c);
    sprintf(c, prompt2, intVal);
    if (editMode) sprintf(c, "%s<", c);
    printLineCEL(MENU_ITEM_LINE, c);
}

// This will be moved to Menu.cpp
void menu_Prompt(int menuNum, byte byteVal, char *prompt1, char *prompt2, boolean editMode) {
    menu_Prompt(menuNum, (int) byteVal, prompt1, prompt2, editMode);
}



void dds_Menu_Amplitude_Display(int menuNum, boolean editMode) {
    menu_Prompt(menuNum, dds_amplitude, P("%0.2d: SideTone"), P2("Amp: %3d%%"), editMode);
}
              
void dds_Menu_Freq_Display(int menuNum, boolean editMode) {
    menu_Prompt(menuNum, dds_frequency, P("%0.2d: SideTone"), P2("Hz: %4d"), editMode);
}

#endif // USE_DDS_SIDETONE

// End
