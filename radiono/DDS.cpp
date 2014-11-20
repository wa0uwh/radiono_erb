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

#define LOFREQ (100)
#define HIFREQ (2000)
#define LOAMP (0)
#define HIAMP (100)


// Single 1/4 period (quadrant) sine wave table.
// Amplitudes values are 0 to 127.  64 samples per period/4.
PROGMEM const char sine64[64] = {
      0,   3,   6,   9,  12,  16,  19,  22, 
     25,  28,  31,  34,  37,  40,  43,  46, 
     49,  51,  54,  57,  60,  63,  65,  68, 
     71,  73,  76,  78,  81,  83,  85,  88, 
     90,  92,  94,  96,  98, 100, 102, 104, 
    106, 107, 109, 111, 112, 113, 115, 116, 
    117, 118, 120, 121, 122, 122, 123, 124, 
    125, 125, 126, 126, 126, 127, 127, 127,
};

// Useful macros for setting and resetting bits
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |=  _BV(bit))

// DDS frequency and reference clock
//double dds_frequency = 0.0;
const double ref_frequency = (16 * MHz / 500);

// Initializaton Stataus
boolean ddsInitialized = false;

// These must all be marked as volatile as they are used in an interrupt service routine
volatile byte dds_amplitude = HIAMP;
volatile  int dds_frequency = 600;
volatile uint32_t phase_accumulator;
volatile uint32_t tuning_word;




// ###############################################################################
// ###############################################################################
// ###############################################################################

// ###############################################################################
// Setup timer2 with prescaler = 1, PWM mode to phase correct PWM
// See the ATMega328 datasheet for all the gory details
void timer2Setup() {
#define DEBUG(x ...)
#define DEBUG(x ...) debugUnique(x)    // UnComment for Debug

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
#define DEBUG(x ...) debugUnique(x)    // UnComment for Debug

    DEBUG(P("\n%s/%d: Pin: %d"), __func__, __LINE__, SIDETONE_PIN);

    pinMode(SIDETONE_PIN, OUTPUT);  // Timer 2 PWM output on mega328 PB3 or PD3
    timer2Setup();                  // Set up timer2 to a phase correct 32kHz clock
    ddsInitialized = true;
}
       
// ###############################################################################
void ddsPause() {
#define DEBUG(x ...)
#define DEBUG(x ...) debugUnique(x)    // UnComment for Debug

    DEBUG(P("%s/%d:"), __func__, __LINE__);
 
    TIMSK2 = 0; 
    OCR2B = 128;
}
       
// ###############################################################################
void ddsShutDown() {
#define DEBUG(x ...)
//#define DEBUG(x ...) debugUnique(x)    // UnComment for Debug

    DEBUG(P("%s/%d:"), __func__, __LINE__);

    ddsPause();
    ddsInitialized = false;
}

// ###############################################################################
void ddsAdjAmplitude(byte amp){
#define DEBUG(x ...)
#define DEBUG(x ...) debugUnique(x)    // UnComment for Debug

    DEBUG(P("%s/%d: Amplitude= %d"), __func__, __LINE__, amp);

    dds_amplitude = constrain(amp, LOAMP, HIAMP);
}

// ###############################################################################
void ddsAdjFreq(int freq){
#define DEBUG(x ...)
#define DEBUG(x ...) debugUnique(x)    // UnComment for Debug

    DEBUG(P("%s/%d: Freq= %d"), __func__, __LINE__, freq);

    dds_frequency = constrain (freq, LOFREQ, HIFREQ);
    // Set up initial DDS frequency and calculate the timing word
    tuning_word = pow(2,32) * (double) dds_frequency / ref_frequency;
}

// ###############################################################################
void ddsTone() {
#define DEBUG(x ...)
#define DEBUG(x ...) debugUnique(x)    // UnComment for Debug

    DEBUG(P("%s/%d:"), __func__, __LINE__);

    ddsTone(dds_frequency);  
}

// ###############################################################################
void ddsTone(int freq) {
#define DEBUG(x ...)
#define DEBUG(x ...) debugUnique(x)    // UnComment for Debug

    DEBUG(P("%s/%d:"), __func__, __LINE__);

    if (!dds_amplitude) dds_amplitude = HIAMP;
    ddsTone(freq, dds_amplitude);  
}

// ###############################################################################
void ddsTone(int freq, byte amp){
#define DEBUG(x ...)
#define DEBUG(x ...) debugUnique(x)    // UnComment for Debug

    DEBUG(P("%s/%d: Freq: %d, Amp: %d"), __func__, __LINE__, freq, amp);

    if (!freq) { ddsPause(); return; } // Pause Tone
    if (!ddsInitialized) ddsInit();
  
    ddsAdjAmplitude(amp);
    ddsAdjFreq(freq);
  
    sbi (TIMSK2,TOIE2);    // Enable timer 2.
}


// ###############################################################################
// Timer 2 interrupt service routine (ISR) is used to generate
// the timebase reference clock for the DDS generator at 32kHz.
ISR(TIMER2_OVF_vect) {
#define DEBUG(x ...)
//#define DEBUG(x ...) debugUnique(x)    // UnComment for Debug

    //DEBUG(P("%s/%d:"), "ISR", __LINE__);

    // Update phase accumulator and extract the sine table index from it
    phase_accumulator += tuning_word;
    volatile byte sine_table_index = phase_accumulator >> 24;  // Use upper 8 bits as index
    
    volatile byte Tmp = 0;
    volatile byte Quadrant = sine_table_index >> 6;
    volatile byte Index = sine_table_index % 64;
    
    switch (Quadrant) { // Compute Table Indes
       case 0: case 2: Tmp = 00 + Index; break;
       case 1: case 3: Tmp = 63 - Index; break;
    }
    
    Tmp = pgm_read_byte_near(sine64 + Tmp ); // Get Table Value
    Tmp = (Tmp * dds_amplitude) / HIAMP;       // Apply Level Control
     
    switch (Quadrant) { // Update to PWM
       case 0: case 1: OCR2B = 128 + Tmp; break;
       case 2: case 3: OCR2B = 128 - Tmp; break;
    } 
    
    DEBUG("Q%d: %3d, %3d", Quadrant, OCR2B, Tmp);
}


// ###############################################################################
// #  MENU System  ###############################################################
// ###############################################################################

// ###############################################################################
void dds_Menu_Freq_Adj(int dir) {
    dds_frequency = constrain ((int) dds_frequency + dir * 10, LOFREQ, HIFREQ);
    ddsAdjFreq(dds_frequency);
}
    
void dds_Menu_Amplitude_Adj(int dir) {
    dds_amplitude = (byte) constrain ((int) dds_amplitude + dir, LOAMP, HIAMP);
    ddsAdjAmplitude(dds_amplitude);
}


// ###############################################################################              
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

#endif // USE_DDS_SIDETONE

// End
