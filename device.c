#include "device.h"
#include "display.h"
#include "interrupts.h"

#define PCM3070_Device_Address  0x18   //7-bit PCM3070 I2C address

TSBState sbState;

void SBInit(void)
{
  sbState.mode = analog;
  sbState.nInput = 1;
  sbState.nSurround = SURROUND_NONE;
  sbState.nVolume = 8;
  sbState.bMute = false;
  sbState.bShutdown = true;
  sbState.bDisplayOn = false;
  sbState.nDisplayTimeCount = 0;
  
    //set PCA9535 pins all as output 
  unsigned char out[] = {0x00, 0x00};
  I2C_multiWrite(0x20, 0x06, 2, out);  //setting Port0 and Port1 as OUTPUT
}

const char LDAC[]={0x9C,0xB0,0xC4,0xD8,0xEC,0xF4,0xFA,0x00,0x02,0x04,0x06,0x08,0x0A,0x0C,0x0E,0x10};
const char RDAC[]={0x9C,0xB0,0xC4,0xD8,0xEC,0xF4,0xFA,0x00,0x02,0x04,0x06,0x08,0x0A,0x0C,0x0E,0x10};
//const char pVol28[] = {0x00, 0x00, 0x00, 0x02, 0x06, 0x0B, 0x16, 0x20, 0x2D, 0x40, 0x47, 0x50, 0x5A, 0x65, 0x71, 0x7F};
//const char pVol29[] = {0x10, 0x33, 0xA3, 0x06, 0x66, 0x61, 0xB5, 0x13, 0x4E, 0x00, 0xCF, 0x92, 0x67, 0x6E, 0xCF, 0xB2};
//const char pVol2A[] = {0x62, 0xCF, 0xD7, 0x1B, 0x66, 0x88, 0x43, 0x73, 0xFB, 0x00, 0x26, 0x3B, 0x03, 0xE3, 0x54, 0x60};

void SetVolume(int nVolume)
{
  if (nVolume > 15) nVolume = 15;
  if (nVolume <= 0) nVolume = 0;
  
  //unsigned char mstVolume[] = {pVol28[nVolume], pVol29[nVolume], pVol2A[nVolume]};
  unsigned char mstVolume[] = {LDAC[nVolume], RDAC[nVolume]};
 
  I2C_singleWrite(PCM3070_Device_Address,0x00, 0x00);
  I2C_multiWrite(PCM3070_Device_Address, 0x41, 2, mstVolume);
  
  //I2C_singleWrite(PCM3070_Device_Address,0x00, 0x0B);   //page 11
  //I2C_multiWrite(PCM3070_Device_Address, 0x28, 3, mstVolume);    //Master Volume control
  //I2C_singleWrite(PCM3070_Device_Address, 0x00, 0x08);
  //I2C_singleWrite(PCM3070_Device_Address, 0x01, 0x05);
  //I2C_singleWrite(PCM3070_Device_Address,0x00, 0x0B); //page 11
  //I2C_multiWrite(PCM3070_Device_Address, 0x28, 3, mstVolume);
}

struct TRegisterData
{
  char cRegister;
  char cData;
};

void LoadRegisters(const TRegisterData *pRegisterData)
{
  unsigned int n = 0;
  TRegisterData registerData = pRegisterData[0];
  
  while ((registerData.cRegister != 0xFF) && (registerData.cData != 0xFF))
  {
    I2C_singleWrite(PCM3070_Device_Address, registerData.cRegister, registerData.cData);
    registerData = pRegisterData[n++];
  }
}

const TRegisterData pToAnalogMode[] = 
{
  {0x00, 0x00}, 
  {0x52, 0x88},
  {0x51, 0x00},
  {0x40, 0x0C},
  {0x3F, 0x14},
  {0x05, 0x11},
  
  {0x04, 0x07},  //set BCLK as PLL input clock
  {0x05, 0x93}, //Set PLL parameter R=3
  {0x06, 0x04}, //Set PLL parameter J = 4
  {0x07, 0x23}, 
  {0x08, 0x28}, //Set PLL parameter D = 9000
  
  {0x00, 0x00},
  {0x21, 0x44},  
  {0x1F, 0x14}, //sec_bclk path
  {0x37, 0x14},   
  {0x34, 0x20},
  {0x20, 0x0E}, //interface path
  {0x1D, 0x01}, //disable digital loopback
  {0x1B, 0x00}, //set wclk as output
  {0x1E, 0x82}, //power up Bclk divider, and set ratio=2
  
  {0x3C, 0x80},
  {0x3D, 0x00},
  {0x00, 0x08},
  {0x01, 0x04},
  {0x00, 0x2C},
  {0x01, 0x04},

  {0x00, 0x01}, //adjust PGA gain for Analog mode
  {0x3B, 0x00},
  {0x3C, 0x00},
  
  {0x00, 0x00}, //power up both ADCs
  {0x51, 0xC0},
  {0x52, 0x00}, //unMute both ADCs

  {0x3F, 0xD4}, //power up both DACs

  {0x40, 0x00}, //unMute both DACs
  
  {0x00, 0x01}, //Set LineOUT volume at 0dB
  {0x12, 0x00},
  {0x13, 0x00},
  
  //{0x00, 0x08}, //configure the Input Mux in miniDSP process flow
  //{0x1E, 0x02}, 
  //{0x00, 0x08},
  //{0x01, 0x05},
  //{0x00, 0x08},
  //{0x1E, 0x02},
  {0xFF, 0xFF}
};

//this function is used when switch from I2S mode to Analog mode
void PCM3070_toAnalogMode(void)
{
  LoadRegisters(pToAnalogMode);
  Delay_ms(200); 
  I2C_singleWrite(PCM3070_Device_Address, 0x00, 0x08); //configure the Input Mux in miniDSP process flow
  I2C_singleWrite(PCM3070_Device_Address, 0x1E, 0x02); 
  I2C_singleWrite(PCM3070_Device_Address, 0x01, 0x05); 
  I2C_singleWrite(PCM3070_Device_Address, 0x00, 0x08); 
  I2C_singleWrite(PCM3070_Device_Address, 0x1E, 0x02); 
}
  
const TRegisterData pToI2SMode[] = 
{
  //select page 0
  {0x00, 0x00},
  //Mute ADCs 
  {0x52, 0x88},
  //power down ADCs
  {0x51, 0x00},
  //Mute DACs
  {0x40, 0x0C},
  //power down both DACs
  {0x3F, 0x14},
  //power down PLL
  {0x05, 0x11},

  //configure PLL settings
  {0x04, 0x03},  //Select MCLK as the PLL input clock
  {0x05, 0x91},   //set R = 1
  {0x06, 0x04},  //set J = 4
  {0x07, 0x00}, 
  {0x08, 0x00}, //Set D = 0000
  
  //configure Audio interface
  {0x00, 0x00},
  {0x21, 0x00},  
  {0x1F, 0x12},  //sec_bclk path
  {0x37, 0x14},   
  {0x34, 0x20},
  {0x20, 0x00},   //interface path
  {0x1D, 0x01},//disable digital loopback
  {0x1E, 0x02}, //power down Bclk divider, and set ratio=2
  {0x1B, 0x00},
  
  {0x3C, 0x80},
  {0x3D, 0x00},
  {0x00, 0x08},
  {0x01, 0x04},
  {0x00, 0x2C},
  {0x01, 0x04},
  {0x00, 0x00},
   
  //power up both ADCs
  {0x51, 0xC0},
  //unMute both ADCs
  {0x52, 0x00},
  //power up both DACs
  {0x3F, 0xD4},
  //unMute both DACs
  {0x40, 0x00},
  
  //Set LineOUT volume at -3 dB
  {0x00, 0x01},
  {0x12, 0x3D},
  {0x13, 0x3D},
  {0x3C, 0x00},//Set DAC Gain 
  {0x3B, 0x00},
  //configure the Input Mux in miniDSP process flow
  //{0x00, 0x08},
  //{0x1E, 0x01}, 
  //{0x00, 0x08},
  //{0x01, 0x05},  //flip buffers
  //{0x00, 0x08},
  //{0x1E, 0x01},
  {0xFF, 0xFF}
};

//this function is used when switch from Analog mode to I2S mode
void PCM3070_toI2SMode(void)
{
  LoadRegisters(pToI2SMode);  
  Delay_ms(200);
  I2C_singleWrite(PCM3070_Device_Address, 0x00, 0x08); 
  I2C_singleWrite(PCM3070_Device_Address, 0x1E, 0x01); 
  I2C_singleWrite(PCM3070_Device_Address, 0x01, 0x05); 
  I2C_singleWrite(PCM3070_Device_Address, 0x00, 0x08); 
  I2C_singleWrite(PCM3070_Device_Address, 0x1E, 0x01); 
}

void PCM3070_USB_IN(void)
{ 
  SetShutdown(true);  

  sbState.mode = usb;  
  //switch from Analog mode to I2S mode
  PCM3070_toI2SMode();  
  
  I2C_singleWrite(PCM3070_Device_Address, 0x00, 0x01); //Increase USB Lineout gain
  I2C_singleWrite(PCM3070_Device_Address, 0x12, 0x06); 
  I2C_singleWrite(PCM3070_Device_Address, 0x00, 0x01); 
  I2C_singleWrite(PCM3070_Device_Address, 0x13, 0x06); 
  
  //configure the on-board hardware switch
  P3OUT_bit.P3OUT_7 = 0;   //switch_EN, enable the switch
  P3OUT_bit.P3OUT_6 = 0;   //switch_Logic low, USB path
  P2OUT_bit.P2OUT_2 = 0;   //CKSEL, DIR9001 in PLL mode
  
  SetShutdown(sbState.bShutdown);
}

void PCM3070_SPDIF_IN(void)
{
  SetShutdown(true);  
  sbState.mode = spdif;
  
  //switch from analog mode to I2S mode
  PCM3070_toI2SMode(); 
  
  //configure the on-board hardware switch
  P3OUT_bit.P3OUT_7 = 0;    //switch_EN, enable the switch
  P3OUT_bit.P3OUT_6 = 1;   //switch_Logic high, SPDIF path
  P2OUT_bit.P2OUT_2 = 0;   //CKSEL, DIR9001 in PLL mode
  
  SetShutdown(sbState.bShutdown);
}

void SetAnalogMode(int nInput)
{    
  
  SetShutdown(true);  
  unsigned char pMux[] = {0x40, 0x10, 0x04};
  unsigned char pgaGain[] = {0x00, 0x00, 0x0C};
  unsigned char cInput = 0x40;
  unsigned char gInput = {0x00};
  
  const TRegisterData pSwitchAnalog[] = { 
    {0x00, 0x01},
    {0x36, 0x40},
    {0x39, 0x40},
    {0xff, 0xff}};

  //configure GPIO pins
  P3OUT_bit.P3OUT_7 = 1;    //switch_EN, Disable the switch
  P2OUT_bit.P2OUT_2 = 1;    //CKSEL, non-PLL mode,only generate clocks for PCM3070
  
  //switch from I2S mode to Analog mode
  PCM3070_toAnalogMode();

  LoadRegisters(pSwitchAnalog);
  cInput = pMux[nInput - 1];  
  I2C_singleWrite(PCM3070_Device_Address,0x34, cInput); 
  I2C_singleWrite(PCM3070_Device_Address,0x37, cInput);
  
  gInput = pgaGain[nInput - 1];  
  I2C_singleWrite(PCM3070_Device_Address,0x00, 0x01);
  I2C_singleWrite(PCM3070_Device_Address,0x3B, gInput); //Increase PGA gain for HP IN
  I2C_singleWrite(PCM3070_Device_Address,0x3C, gInput);
  
  Delay_ms(100);
  SetShutdown(sbState.bShutdown);
}

void SetMute(bool bMute)
{
  unsigned char cData = 0x00;
  if (bMute) cData = 0x0c;
  
  I2C_singleWrite(PCM3070_Device_Address, 0x00, 0x00); //configure PCM3070
  I2C_singleWrite(PCM3070_Device_Address, 0x40, cData);
}

void SetShutdown(bool bShutdown)
{
  // shutdown TPA3110
  if (bShutdown)
    P3OUT_bit.P3OUT_0 = 0;  //Bring TPA3110 into Shutdown
  else
    P3OUT_bit.P3OUT_0 = 1; //Bring TPA3110 out of Shutdown
}

void SetSurround(int nSurround)
{
  //unsigned char pLargeBass[] = {0x33, 0x33, 0x33};
  //unsigned char pSmallBass[] = {0x20, 0x00, 0x00};
  //unsigned char *pBass = pSmallBass;
  //if (nSurround & SURROUND_0)    //large bass
  // pBass = pLargeBass;
  SetShutdown(true);  
  if (nSurround & SURROUND_0)  //Surround_0 toggles EQ
  {
  
  I2C_singleWrite(PCM3070_Device_Address, 0x00, 0x2C); //EQ MUX
  I2C_singleWrite(PCM3070_Device_Address, 0x0A, 0x01);
  I2C_singleWrite(PCM3070_Device_Address, 0x00, 0x2C);
  I2C_singleWrite(PCM3070_Device_Address, 0x01, 0x05);
  I2C_singleWrite(PCM3070_Device_Address, 0x00, 0x2C);
  I2C_singleWrite(PCM3070_Device_Address, 0x0A, 0x01);
  }
  
  else{
  I2C_singleWrite(PCM3070_Device_Address, 0x00, 0x2C); //EQ MUX
  I2C_singleWrite(PCM3070_Device_Address, 0x0A, 0x02);
  I2C_singleWrite(PCM3070_Device_Address, 0x00, 0x2C);
  I2C_singleWrite(PCM3070_Device_Address, 0x01, 0x05);
  I2C_singleWrite(PCM3070_Device_Address, 0x00, 0x2C);
  I2C_singleWrite(PCM3070_Device_Address, 0x0A, 0x02);
  }
  
  
  unsigned char srsTrue = 0x02;
  unsigned char srsFalse = 0x01; 

  if (nSurround & SURROUND_1)  //Surround_1 toggles SRS WOW HD
  {
  I2C_singleWrite(PCM3070_Device_Address, 0x00, 0xB);
  I2C_singleWrite(PCM3070_Device_Address, 0x2E, srsTrue);
  I2C_singleWrite(PCM3070_Device_Address, 0x00, 0x08);
  I2C_singleWrite(PCM3070_Device_Address, 0x01, 0x05);
  I2C_singleWrite(PCM3070_Device_Address, 0x00, 0xB);
  I2C_singleWrite(PCM3070_Device_Address, 0x2E, srsTrue);
  }
  
  else{
  I2C_singleWrite(PCM3070_Device_Address, 0x00, 0xB);
  I2C_singleWrite(PCM3070_Device_Address, 0x2E, srsFalse);
  I2C_singleWrite(PCM3070_Device_Address, 0x00, 0x08);
  I2C_singleWrite(PCM3070_Device_Address, 0x01, 0x05);
  I2C_singleWrite(PCM3070_Device_Address, 0x00, 0xB);
  I2C_singleWrite(PCM3070_Device_Address, 0x2E, srsFalse);
  }
  SetShutdown(sbState.bShutdown);
}
