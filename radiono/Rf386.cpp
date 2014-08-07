// Support for RF386 Filter Selection

#include <Arduino.h>
#include "debug.h"

#define PA_BAND_CLK (7)

// ERB - Buffers that Stores "const stings" to, and Reads from FLASH Memory
extern char buf[];
// ERB - Force format stings into FLASH Memory
#define  P(x) strcpy_P(buf, PSTR(x))
// FLASH2 can be used where Two small (1/2 size) Buffers are needed.
#define P2(x) strcpy_P(buf + sizeof(buf)/2, PSTR(x))



void setRf386BandSignal(unsigned long freq){
  // This setup is compatable with the Minima RF386 RF Power Amplifier
  // See: http://www.hfsignals.org/index.php/RF386

  // Bitbang Clock Pulses to Change PA Band Filter
  int band;
  static int prevBand;
  static unsigned long prevFreq;

  if (freq == prevFreq) return;
  prevFreq = freq;
   
  if      (freq <  4000000UL) band = 4; //   3.5 MHz
  else if (freq < 10200000UL) band = 3; //  7-10 MHz
  else if (freq < 18200000UL) band = 2; // 14-18 MHz
  else if (freq < 30000000UL) band = 1; // 21-28 MHz
  else band = 1;

  //debug("Band Index = %d", band);
  
  if (band == prevBand) return;
  prevBand = band;
  
  debug(P("BandI = %d"), band);

  digitalWrite(PA_BAND_CLK, 1);  // Output Reset Pulse for PA Band Filter
  delay(500);
  digitalWrite(PA_BAND_CLK, 0);

  while (band-- > 1) { // Output Clock Pulse to Change PA Band Filter
     delay(50);
     digitalWrite(PA_BAND_CLK, 1);
     delay(50);
     digitalWrite(PA_BAND_CLK, 0);
  }
}

//End