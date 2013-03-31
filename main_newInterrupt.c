#include "device.h"
//#include "EEPROM.h"

#define START_ADDRESS 0x2000

unsigned char GetNumApps(void)
{
  unsigned char nApps = 0;
  I2C_readEEPROM(0x50, START_ADDRESS, 1, &nApps);
  return nApps;
}

unsigned int GetAppAddress(unsigned char nApp)
{
  unsigned char pData[2];
  unsigned int nAddress = 0;
  if (nApp < 1) return 0;
  if (nApp > GetNumApps()) return 0;
  I2C_readEEPROM(0x50, START_ADDRESS + 1 + 2 * (nApp - 1), 2, pData);
  nAddress = pData[0];
  nAddress = nAddress << 8;
  nAddress |= pData[1];
  return nAddress;
}

unsigned char GetNumScripts(unsigned int nAppAddress)
{
  unsigned char nScripts = 0;
  I2C_readEEPROM(0x50, nAppAddress + 16, 1, &nScripts);
  return nScripts;
}

unsigned int GetScriptAddress(unsigned int nAppAddress, unsigned char nScript)
{
  unsigned char pData[2];
  unsigned int nAddress = 0;
  if (nScript < 1) return 0;
  if (nScript > GetNumScripts(nAppAddress)) return 0;
  I2C_readEEPROM(0x50, nAppAddress + 16 + 1 + 2 * (nScript - 1), 2, pData);
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
  I2C_readEEPROM(0x50, nScriptAddress + 16, 2, pCmds);
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
    I2C_readEEPROM(0x50, nAddress, 4, (unsigned char *) &cmd);
    nAddress += 4;
    switch (cmd.nType)
    {
      case 0x00: // write command
        for (n = 0; n < cmd.nCount; n += 32)
        {          
          nBytes = cmd.nCount - n;
          if (nBytes > 32) nBytes = 32;
          
          I2C_readEEPROM(0x50, nAddress, nBytes, pData);
          //Write nBytes of pData to AIC3254 at cmd.nRegAddr... 
          I2C_multiWrite(cmd.nI2CAddr >> 1, cmd.nRegAddr, nBytes, pData);
          nAddress += nBytes;
          cmd.nRegAddr += nBytes;
        }
      break;
      default:
        nAddress += cmd.nCount;
    }
  }
}

void ReadEEPROM(void)
{
  unsigned int nAppAddress = GetAppAddress(1);
  unsigned int nScriptAddress = GetScriptAddress(nAppAddress, 1);
  LoadScript(nScriptAddress);
//  unsigned char pData[16];
//  I2C_readEEPROM(0x50, START_ADDRESS, 4, pData);
//  I2C_readEEPROM(0x50, START_ADDRESS + 2, 4, pData);
}

/*  Global Variables  */
char i=0;                   // char values are used to preserve memory
char bcs_vals[3] = {7,9,2};
char dco_vals[3] = {3,5,6};

void main(void)
{
  InitOsc();     //Set MCU clock
  InitPeriph();  //Set MCU GPIOs 
  I2C_Tx_Init(0x14);  //Initialize I2C transmit
  TurnOffLED();  //Turn off all LEDs
  ReadEEPROM();  
__bis_SR_register(GIE);     //enable global interrupts
}

/******** IR Code *******************/
#define MAX_TIMEOUT 10000
#define MIN_ZERO 600
#define MIN_ONE 1000
#define MAX_ONE 2000
#define MAX_TIMINGS 33
#define NEC_ADDRESS 0
#define NEC_CMD_VOL_UP 0x82
#define NEC_CMD_VOL_DN 0xA2
#define NEC_CMD_MUTE 0x12

typedef struct
{
  unsigned char nAddress;
  unsigned char nCommand;
} TIR;

#define RC5_BITS 14
#define RC5_ADDRESS 0
#define RC5_CMD_VOL_UP 0x1E
#define RC5_CMD_VOL_DN 0x1C
#define RC5_CMD_MUTE 0x04
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
    pBits[nBit] = (~P1IN_bit.P1IN_2) & 0x01;
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
    while ((nCount < MAX_TIMEOUT) && !(P1IN_bit.P1IN_2))
      nCount++;
    if (nCount >= MAX_TIMEOUT) break; 
    
    while ((nCount < MAX_TIMEOUT) && (P1IN_bit.P1IN_2))
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
      button = volumeUp;
      Buttons();
      break;
    case NEC_CMD_VOL_DN:
      button = volumeDown;
      Buttons();
      break;
    case NEC_CMD_MUTE:
      button = muteMode;
      Buttons();
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
      button = volumeUp;
      Buttons();
      break;
    case RC5_CMD_VOL_DN:
      button = volumeDown;
      Buttons();
      break;
    case RC5_CMD_MUTE:
      button = muteMode;
      Buttons();
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

/*  Interrupt Service Routines for Port 1*/
#pragma vector = PORT1_VECTOR
__interrupt void P1_ISR(void)
{

  /* IR hook start */
  if (P1IFG & 0x04)
  {
    ProcessIR();
    P1IFG = 0x00;
    return;
  }
  /* IR hook end */
  
  switch(P1IFG){
        case VOLUP_BTN:
  		P1IFG &= ~VOLUP_BTN;  // clear the interrupt flag
			BCSCTL1 = bcs_vals[i];
			DCOCTL = dco_vals[i];                     
			if (++i == 3)
				i = 0;
                                button = volumeUp;
                                Buttons();
			return;
                        
        case VOLDOWN_BTN:
			P1IFG &= ~VOLDOWN_BTN;  // clear the interrupt flag
			BCSCTL1 = bcs_vals[i];
			DCOCTL = dco_vals[i];                     
			if (++i == 3)
				i = 0;
                                button = volumeDown;
                                Buttons();
			return;
      
        case USB_BTN:
			P1IFG &= ~USB_BTN;  // clear the interrupt flag
			BCSCTL1 = bcs_vals[i];
			DCOCTL = dco_vals[i];                     
			if (++i == 3)
				i = 0;
                                button = usbMode;
                                Buttons();
			return;
        case SPDIF_BTN:
			P1IFG &= ~SPDIF_BTN;  // clear the interrupt flag
			BCSCTL1 = bcs_vals[i];
			DCOCTL = dco_vals[i];                     
			if (++i == 3)
				i = 0;
                                button = spdifMode;
                                Buttons();
			return;
     
        case ANALOG_BTN:
			P1IFG &= ~ANALOG_BTN;  // clear the interrupt flag
			BCSCTL1 = bcs_vals[i];
			DCOCTL = dco_vals[i];                     
			if (++i == 3)
				i = 0;
                                button = analogMode;
                                Buttons();
			return;
        case MUTE_BTN:
			P1IFG &= ~MUTE_BTN;  // clear the interrupt flag
			BCSCTL1 = bcs_vals[i];
			DCOCTL = dco_vals[i];                     
			if (++i == 3)
				i = 0;
                                button = muteMode;
                                Buttons();
			return;
   
        case SD_BTN:
			P1IFG &= ~SD_BTN;  // clear the interrupt flag
			BCSCTL1 = bcs_vals[i];
			DCOCTL = dco_vals[i];                     
			if (++i == 3)
				i = 0;
                                button = shutdownMode;
                                Buttons();
			return;
                        
        default:
			P1IFG = 0;  // probably unnecessary, but if another flag occurs
			            // in P1, this will clear it.  No error handling is
			            // provided this way, though.
			return;                                       
  }
  P1IFG = 0x00;   //clear port 1 interrupt
} //P1_ISR
  

/*  Interrupt Service Routines for Port 2*/
#pragma vector = PORT2_VECTOR
__interrupt void P2_ISR(void) {
  
	switch(P2IFG) {
		case SRRD0_BTN:  //Tru_surround0 button pressed
			P2IFG &= ~SRRD0_BTN;  // clear the interrupt flag
                        BCSCTL1 = bcs_vals[i];
			DCOCTL = dco_vals[i];                     
			if (++i == 3)
				i = 0;
                                button = surround0;
                                Buttons();
			return;
  
                        
                case SRRD1_BTN:  //Tru_surround1 button pressed
                      P2IFG &= ~SRRD1_BTN;
                      BCSCTL1 = bcs_vals[i];
		      DCOCTL = dco_vals[i];                     
			if (++i == 3)
				i = 0;
                                button = surround1;
                                Buttons();
	                return;     
                        
		default:
			P2IFG = 0;  // probably unnecessary, but if another flag occurs
			            // in P1, this will clear it.  No error handling is
			            // provided this way, though.
			return;
	}
        P2IFG = 0x00;
} // P2_ISR
