#ifndef __DEVICE_H
#define __DEVICE_H

/***************************************************************************************/
#include "io430x21x2.h"
#include "in430.h"
#include "intrinsics.h"
#include "TI_USCI_I2C_master.h"
/***************************************************************************************/

enum Mode {usb, analog, spdif};

#define SURROUND_NONE 0x00
#define SURROUND_0    0x01
#define SURROUND_1    0x02

struct TSBState
{
  Mode mode;
  int nInput;
  int nVolume;
  int nSurround;
  bool bMute;
  bool bShutdown;
  bool bDisplayOn;
  int nDisplayTimeCount;
};

extern TSBState sbState;

void SBInit(void);

void SetAnalogMode(int nInput);
void SetShutdown(bool bShutdown);
void SetVolume(int nVolume);
void SetMute(bool bMute);
void SetSurround(int nSurround);

void PCM3070_USB_IN(void);
void PCM3070_SPDIF_IN(void);

void InitOsc(void);
void InitPeriph(void);
void Delay_ms(int time);

#endif
