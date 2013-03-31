#include "processflow.h"
#include "TI_USCI_I2C_master.h"

#define START_ADDRESS 0x0000

unsigned char GetNumApps(void)
{
  unsigned char nApps = 0;
  I2C_readEEPROM(0x51, START_ADDRESS, 1, &nApps);
  return nApps;
}

unsigned int GetAppAddress(unsigned char nApp)
{
  unsigned char pData[2];
  unsigned int nAddress = 0;
  if (nApp < 1) return 0;
  if (nApp > GetNumApps()) return 0;
  I2C_readEEPROM(0x51, START_ADDRESS + 1 + 2 * (nApp - 1), 2, pData);
  nAddress = pData[0];
  nAddress = nAddress << 8;
  nAddress |= pData[1];
  return nAddress;
}

unsigned char GetNumScripts(unsigned int nAppAddress)
{
  unsigned char nScripts = 0;
  I2C_readEEPROM(0x51, nAppAddress + 16, 1, &nScripts);
  return nScripts;
}

unsigned int GetScriptAddress(unsigned int nAppAddress, unsigned char nScript)
{
  unsigned char pData[2];
  unsigned int nAddress = 0;
  if (nScript < 1) return 0;
  if (nScript > GetNumScripts(nAppAddress)) return 0;
  I2C_readEEPROM(0x51, nAppAddress + 16 + 1 + 2 * (nScript - 1), 2, pData);
  nAddress = pData[0];
  nAddress = nAddress << 8;
  nAddress |= pData[1];
  return nAddress;
}

typedef struct
{
  unsigned char nType;
  unsigned char nI2CAddr;
  unsigned char nRegAddr;
  unsigned char nCount;
} TCMD;

unsigned int GetNumCmds(unsigned int nScriptAddress)
{
  unsigned char pCmds[2];
  unsigned int nCmds;
  I2C_readEEPROM(0x51, nScriptAddress + 16, 2, pCmds);
  nCmds = pCmds[0];
  nCmds = nCmds << 8;
  nCmds |= pCmds[1];
  return nCmds;
}

void LoadScript(unsigned int nScriptAddress)
{
  unsigned int nCmds = GetNumCmds(nScriptAddress);
  unsigned int nCmd;
  unsigned int nAddress = nScriptAddress + 16 + 2;
  TCMD cmd;
  unsigned char pData[32];
  unsigned int nBytes;
  unsigned int n;
  
  for (nCmd = 0; nCmd < nCmds; nCmd++)
  {
    I2C_readEEPROM(0x51, nAddress, 4, (unsigned char *) &cmd);
    nAddress += 4;
    switch (cmd.nType)
    {
      case 0x00: // write command
        for (n = 0; n < cmd.nCount; n += 32)
        {          
          nBytes = cmd.nCount - n;
          if (nBytes > 32) nBytes = 32;
          
          I2C_readEEPROM(0x51, nAddress, nBytes, pData);
          //Write nBytes of pData to AIC3254 at cmd.nRegAddr... 
          I2C_multiWrite(cmd.nI2CAddr >> 1, cmd.nRegAddr, nBytes, pData);
     //for (int b = 0; b < nBytes; b++)
    // I2C_singleWrite(cmd.nI2CAddr >> 1, cmd.nRegAddr + b, pData[b]);
          nAddress += nBytes;
          cmd.nRegAddr += nBytes;
        }
      break;
      default:
        nAddress += cmd.nCount;
      break;
    }
  }
}

void LoadProcessFlow(unsigned int nApplication, unsigned int nScript)
{
  unsigned int nAppAddress = GetAppAddress(nApplication);
  unsigned int nScriptAddress = GetScriptAddress(nAppAddress, nScript);
  LoadScript(nScriptAddress);
}
