// Encoder04.cpp

/*
 * Copyright (C) 2014, by Author: Jeffrey A. Whitlatch (JAW) - ko7m
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
#include "A1Main.h"

#ifdef USE_ENCODER04

#include "Encoder04.h"
#include "debug.h"

// Pin change interrupts
// Digital pin     PC Pin     Vector      Ctl Reg Port PCICR Bit Pin Mask Reg
// D0-D7           PCINT16-23 PCINT2_vect PCIR2   PD   PCIE2     PCMSK2
// D8-D13          PCINT0-5   PCINT0_vect PCIR0   PB   PCIE0     PCMSK0
// D14-D19 (A0-A5) PCINT8-13  PCINT1_vect PCIR1   PC   PCIE1     PCMSK1

// Define the interrupt vector, interrupt enable bit, PCINTpin values
#if (ENC_A_PIN) >= 0 && (ENC_A_PIN) <= 7
  #define VECTOR    PCINT2_vect
  #define PCMask    PCMSK2
  #define PCICRbit  (1<<PCIE2)
  #define PCINTpinA (1<<(ENC_A_PIN))
  #define PCINTpinB (1<<(ENC_B_PIN))
#endif
#if (ENC_A_PIN) >= 8 && (ENC_A_PIN) <= 13
  #define VECTOR    PCINT0_vect
  #define PCMask    PCMSK0
  #define PCICRbit  (1<<PCIE0)
  #define PCINTpinA (1<<(ENC_A_PIN-8))
  #define PCINTpinB (1<<(ENC_B_PIN-8))
#endif
#if (ENC_A_PIN) >= 14 && (ENC_A_PIN) <= 19
  #define VECTOR    PCINT1_vect
  #define PCMask    PCMSK1
  #define PCICRbit  (1<<PCIE1)
  #define PCINTpinA (1<<(ENC_A_PIN-14))
  #define PCINTpinB (1<<(ENC_B_PIN-14))
#endif

// Encoder patterns for clockwise and anti-clockwise rotation
static const char CCW_DIR[] = {0x01, 0x03, 0x00, 0x02};
static const char  CW_DIR[] = {0x02, 0x00, 0x03, 0x01};

static int      last_code;  // Last stable encoder output
static int  encoder_count;  // tracks incremental rotation CW (+), CCW (-)

// Used to track pin state in ISR
typedef struct 
{
  union 
  {
    byte encoder;
    struct 
    {
      int enc_A:1;
      int enc_B:1;
    };
  };
} PINSTATUS_t;

// Returns current encoder pin reading normalized to 0..3 value.  The encoder outputs
// a Gray code with the sequence determined by the direction of rotation.
static inline int getEncoderValue(void)
{
  int value = 0;
  
  if (digitalRead(ENC_A_PIN)) value += 1;
  if (digitalRead(ENC_B_PIN)) value += 2;
    
  return value;
}

// Encoder pin change ISR - If a pin changed, reset debounce timer
ISR(VECTOR)
{
  static PINSTATUS_t last;
  byte prior;
	
  prior = last.encoder;
  last.enc_A = digitalRead(ENC_A_PIN);  // Get encoder current pin values
  last.enc_B = digitalRead(ENC_B_PIN);
  
  if (prior != last.encoder)
  {
    TCNT1  = 0;
    TIMSK1 |= (1 << OCIE1A);
  } 
}

// Timer ISR - We get here if the debounce timer times out.  Compare the new code with
// the previous one to see which direction the encoder has turned.  Update the rotation
// counter to reflect the change.  The timer and interrupt are left disabled.  I can get
// four counts per detent and effectively divide this by 4 to get one tick per detent.
ISR(TIMER1_COMPA_vect)
{
  int new_code = getEncoderValue();
  
  if (new_code != last_code)
  {
    if (new_code == CW_DIR[last_code]) {
      if (0x00 == new_code) encoder_count++;
    } else if (new_code == CCW_DIR[last_code]) {
      if (0x00 == new_code) encoder_count--;
    }
    last_code = new_code;
  }
  TIMSK1 &= ~(1 << OCIE1A);  // Disable Timer
}

// Initialize the encoder
void initEncoder()
{
  cli();
  
  // Encoder pins
  pinMode(ENC_A_PIN, INPUT_PULLUP);
  pinMode(ENC_B_PIN, INPUT_PULLUP);
  
  // Set pin change interrupt enable and pin mask for encoder pins
  PCMask |= PCINTpinA;    // Set pin mask for A and B inputs
  PCMask |= PCINTpinB;
  PCICR  |= PCICRbit;     // Enable interrupts on correct port
  
  // Set timer 1 for 3 ms debounce timeout
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register 3 ms
  OCR1A = 48000;
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set bits for no prescaler
  TCCR1B |= (1 << CS10); 
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  sei();
  
  // Set the initial value from the encoder
  last_code = getEncoderValue();
  encoder_count = 0;
}

// Return encoder count.  Negative - Anti-clockwise, positive - clockwise
int getEncoderDir()
{
  uint8_t oldSREG = SREG;
  cli();
  int val = (encoder_count > 0) - (encoder_count < 0);
  #ifdef USE_DIAL_MOMENTUM
      encoder_count -= val;
  #else
      encoder_count = 0;
  #endif // USE_DIAL_MOMENTUM
  SREG = oldSREG;
  return val;
} 

#endif // USE_ENCODER04
// End


