#include "IR.h"
#include "system.h"
#include "io430x21x2.h"
#include "in430.h"
#include "buttons.h"

#define MAX_TIMEOUT 10000
#define MIN_ZERO 600
#define MIN_ONE 1000
#define MAX_ONE 2000
#define MAX_TIMINGS 33

typedef struct
{
  unsigned char nAddress;
  unsigned char nCommand;
} TIR;

TIR ReadIR_RC5(void)
{
  unsigned int n;
  unsigned int nBit;
  unsigned int pBits[RC5_BITS];
  TIR rc5Data;
  rc5Data.nAddress = 0;
  rc5Data.nCommand = 0;
  
  for (nBit = 0; nBit < RC5_BITS; nBit++)
  {
    pBits[nBit] = (~P1IN_bit.P2) & 0x01;
    for (n = 0; n < 2762; n++);
  }
  
  rc5Data.nAddress = 0;
  for (nBit = 0; nBit < 5; nBit++)
    rc5Data.nAddress |= pBits[7 - nBit] << nBit;
  
  rc5Data.nCommand = 0;
  for (nBit = 0; nBit < 6; nBit++)
    rc5Data.nCommand |= pBits[13 - nBit] << nBit;
    
  return rc5Data;
}

TIR ReadIR_NEC(void)
{
  unsigned int n;
  unsigned int nBit;
  unsigned int nData;
  unsigned int nCount;
  unsigned char pData[4];
  unsigned int pIRTiming[MAX_TIMINGS];
  TIR necData;
  necData.nAddress = 0;
  necData.nCommand = 0;
  
  for (n = 0; n < MAX_TIMINGS; n++)
  {
    pIRTiming[n] = 0;
  
    nCount = 0;
    while ((nCount < MAX_TIMEOUT) && !(P1IN_bit.P2))
      nCount++;
    if (nCount >= MAX_TIMEOUT) break; 
    
    while ((nCount < MAX_TIMEOUT) && (P1IN_bit.P2))
      nCount++;
    if (nCount >= MAX_TIMEOUT) break;

    pIRTiming[n] = nCount;
  }    
  
  if (n != MAX_TIMINGS) return necData; // a timeout has occurred -> not a valid NEC code
  
  for (n = 0; n < 4; n++)
  {
    nData = 0;
    for (nBit = 0; nBit < 8; nBit++)
    {
      nCount = pIRTiming[1 + n * 8 + nBit];
      if (nCount < MIN_ZERO) break; // not a valid zero -> not a valid NEC code
      if (nCount < MIN_ONE) continue; // a valid zero -> continue with next bit
      if (nCount > MAX_ONE) break; // not a valid one -> not a valid NEC code
      nData |= 1 << (7 - nBit);
    }
    if (nBit != 8) return necData; // bit timing not within margins -> not a valid NEC code
    
    pData[n] = nData;
  }
  
  if (pData[0] != (~(pData[1]) & 0xFF)) return necData; // test NEC address complement and return if not correct
  if (pData[2] != (~(pData[3]) & 0xFF)) return necData; // test NEC command complement and return if not correct
  
  necData.nAddress = pData[0];
  necData.nCommand = pData[2];
  return necData;
}

void DispatchIR_NEC(TIR necData)
{
  if (!necData.nAddress && !necData.nCommand) return;
  
  if (necData.nAddress != NEC_ADDRESS) return;

  switch (necData.nCommand)
  {
    case NEC_CMD_VOL_UP:
      Buttons(volumeUp);
      break;
    case NEC_CMD_VOL_DN:
      Buttons(volumeDown);
      break;
    case NEC_CMD_MUTE:
      Buttons(muteMode);
      break;
    default:;
  }
}

void DispatchIR_RC5(TIR rc5Data)
{
  if (!rc5Data.nAddress && !rc5Data.nCommand) return;
  
  if (rc5Data.nAddress != RC5_ADDRESS) return;

  switch (rc5Data.nCommand)
  {
    case RC5_CMD_VOL_UP:
      Buttons(volumeUp);
      break;
    case RC5_CMD_VOL_DN:
      Buttons(volumeDown);
      break;
    case RC5_CMD_MUTE:
      Buttons(muteMode);
      Delay_ms(1000);
      break;
    default:;
  }
}

void ProcessIR(void)
{
//  TIR necData = ReadIR_NEC();
//  DispatchIR_NEC(necData);
    TIR irData = ReadIR_RC5();
    DispatchIR_RC5(irData);
}
