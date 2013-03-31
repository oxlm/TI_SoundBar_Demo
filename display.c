#include "display.h"
#include "TI_USCI_I2C_master.h"
#include "interrupts.h"

#define DISPLAY_TIMER_PERIOD 65535

#define LED_ANA1  0x0100
#define LED_ANA2  0x0200
#define LED_ANA3  0x0400
#define LED_USB   0x0800
#define LED_SPDIF 0x0080
#define LED_SURR0 0x0040
#define LED_SURR1 0x0020
#define LED_VOL0  0x0010
#define LED_VOL1  0x0008
#define LED_VOL2  0x0004
#define LED_VOL3  0x0002
#define LED_SD    0x0001
#define LED_MUTE  0x2000

void UpdateDisplay(void)
{
  unsigned int nLED = 0;

  switch (sbState.mode)
  {
    case usb: nLED |= LED_USB; break;
    case analog:
      switch (sbState.nInput)
      {
        case 1: nLED |= LED_ANA1; break;
        case 2: nLED |= LED_ANA2; break;
        case 3: nLED |= LED_ANA3; break;
      }
      break;
    case spdif: nLED |= LED_SPDIF; break;
  }
  
  if (sbState.bShutdown) nLED |= LED_SD;
  if (sbState.bMute) nLED |= LED_MUTE;

  if (sbState.nSurround & SURROUND_0) nLED |= LED_SURR0;
  if (sbState.nSurround & SURROUND_1) nLED |= LED_SURR1;
  
  if (sbState.nVolume)
    switch ((sbState.nVolume - 1) >> 2)
    {
      case 3: nLED |= LED_VOL3;
      case 2: nLED |= LED_VOL2;
      case 1: nLED |= LED_VOL1;
      case 0: nLED |= LED_VOL0;
    }
  
  if (!sbState.bDisplayOn) nLED = 0;

  unsigned char pLED[] = {nLED & 0xFF, (nLED & 0xFF00) >> 8};
  I2C_multiWrite(0x20, 0x02, 2, pLED);
}

void StartDisplayTimer()
{
#ifdef ENABLE_DISPLAY_TIMER
  StartTimer(DISPLAY_TIMER_PERIOD);
#endif
}
