#include  "device.h"

unsigned int Analog_counter = 1;
unsigned int Volume_counter = 4; 

unsigned int mute_status = 1;
unsigned int EQ_status = 1;
unsigned int SRSwow_status = 1;
unsigned int shutdown_status = 1;

unsigned char Volume;
unsigned char LED_p0;
unsigned char LED_p1;


void AIC3254_VOLUME_UP(void)
{
  if(Volume_counter < 10)
  {
    Volume_counter++;
    Volume = VOLUME[Volume_counter-1];
    I2C_singleWrite(AIC3254_Device_Address,0x00, 0x00);
    I2C_singleWrite(AIC3254_Device_Address,0x41,Volume);    //LDAC Volume
    I2C_singleWrite(AIC3254_Device_Address,0x42,Volume);     //RDAC Volume
    
    LED_p0 &= 0xE1;   //clear all volume LEDs
    
    if((Volume_counter>=1) && (Volume_counter<=3))
    {
      LED_p0 |= 0x10;
      unsigned char D[] = {LED_p0, LED_p1};
      I2C_multiWrite(0x20, 0x02, 2, D);
    }
    
    if((Volume_counter >= 4) && (Volume_counter <= 6))
    { 
      LED_p0 |= 0x18; 
      unsigned char D[] = {LED_p0, LED_p1};
      I2C_multiWrite(0x20, 0x02, 2,D);
    }
    
    if((Volume_counter >= 7) && (Volume_counter <= 9))
    {
      LED_p0 |= 0x1C;
      unsigned char D[] = {LED_p0, LED_p1};
      I2C_multiWrite(0x20, 0x02, 2, D);
    }      
    
    if(Volume_counter == 10)
    {
      LED_p0 |= 0x1E;
      unsigned char D[] = {LED_p0, LED_p1};
      I2C_multiWrite(0x20, 0x02, 2, D);
    }
  } 
   
 if(Volume_counter == 10)
 {
   Volume = VOLUME[9];
   I2C_singleWrite(AIC3254_Device_Address,0x00, 0x00);
   I2C_singleWrite(AIC3254_Device_Address,0x41,Volume);    //LDAC Volume
   I2C_singleWrite(AIC3254_Device_Address,0x42,Volume);     //RDAC Volume
 }
}
  

void AIC3254_VOLUME_DOWN(void)
{
  LED_p0 &= 0xE1;   //clear all the volume LEDs
   
  if(Volume_counter > 1)
  {
    Volume_counter--;
    Volume = VOLUME[Volume_counter-1];
    I2C_singleWrite(AIC3254_Device_Address,0x00, 0x00);
    I2C_singleWrite(AIC3254_Device_Address,0x41,Volume);
    I2C_singleWrite(AIC3254_Device_Address,0x42,Volume);
      
    if((Volume_counter>=1) && (Volume_counter<=3))
    {
      LED_p0 |= 0x10;
      unsigned char D[] = {LED_p0, LED_p1};
      I2C_multiWrite(0x20, 0x02, 2, D);
    }
    
    if((Volume_counter >= 4) && (Volume_counter <= 6))
    {
      LED_p0 |= 0x18;
      unsigned char D[] = {LED_p0, LED_p1};
      I2C_multiWrite(0x20, 0x02, 2, D);
    }
    
    if((Volume_counter >= 7) && (Volume_counter <= 9))
    {
      LED_p0 |= 0x1C; 
      unsigned char D[] = {LED_p0, LED_p1};
      I2C_multiWrite(0x20, 0x02, 2, D);
    }      
    
    if(Volume_counter == 10)
    {
      LED_p0 |= 0x1E; 
      unsigned char D[] = {LED_p0, LED_p1};
      I2C_multiWrite(0x20, 0x02, 2, D);
    }
  }
  if(Volume_counter == 1)
  {
    Volume = VOLUME[0];
    I2C_singleWrite(AIC3254_Device_Address,0x00, 0x00);
    I2C_singleWrite(AIC3254_Device_Address,0x41,Volume);
    I2C_singleWrite(AIC3254_Device_Address,0x42,Volume);
  }
}


//this function is used when switch from I2S mode to Analog mode
void AIC3254_toAnalogMode(void)
{
  //select page 0
  I2C_singleWrite(AIC3254_Device_Address, 0x00, 0x00);
  //Mute ADCs 
  I2C_singleWrite(AIC3254_Device_Address, 0x52, 0x88);
  //power down ADCs
  I2C_singleWrite(AIC3254_Device_Address, 0x51, 0x00);
  //Mute DACs
  I2C_singleWrite(AIC3254_Device_Address, 0x40, 0x0C);
  //power down both DACs
  I2C_singleWrite(AIC3254_Device_Address, 0x3F, 0x14);
  //power down PLL
  I2C_singleWrite(AIC3254_Device_Address, 0x05, 0x11);

  //configure PLL settings
  unsigned char pllData[]={0x07, 0x91, 0x10};
  I2C_multiWrite(AIC3254_Device_Address, 0x04, 3, pllData);
  
  //configure Audio interface
  I2C_singleWrite(AIC3254_Device_Address,0x00, 0x00);
  I2C_singleWrite(AIC3254_Device_Address,0x21, 0x44);  
  I2C_singleWrite(AIC3254_Device_Address,0x1F, 0x14);  //sec_bclk path
  I2C_singleWrite(AIC3254_Device_Address,0x37, 0x14);   
  I2C_singleWrite(AIC3254_Device_Address,0x34, 0x20);
  I2C_singleWrite(AIC3254_Device_Address,0x20, 0x0E);   //interface path
  I2C_singleWrite(AIC3254_Device_Address,0x1D, 0x01);//disable digital loopback
  I2C_singleWrite(AIC3254_Device_Address,0x1B, 0x00);  //set wclk as output
  I2C_singleWrite(AIC3254_Device_Address,0x1E, 0x82); //power up Bclk divider, and set ratio=2
  
  I2C_singleWrite(AIC3254_Device_Address,0x3C, 0x80);
  I2C_singleWrite(AIC3254_Device_Address,0x3D, 0x00);
  I2C_singleWrite(AIC3254_Device_Address,0x00, 0x08);
  I2C_singleWrite(AIC3254_Device_Address,0x01, 0x04);
  I2C_singleWrite(AIC3254_Device_Address,0x00, 0x2C);
  I2C_singleWrite(AIC3254_Device_Address,0x01, 0x04);
  
  //adjust PGA gain for Analog mode
  I2C_singleWrite(AIC3254_Device_Address,0x00, 0x01);
  unsigned char pgaGain[] = {0x14, 0x14};   //8 dB
  I2C_multiWrite(AIC3254_Device_Address,0x3B,2, pgaGain);
  
  //power up both ADCs
  I2C_singleWrite(AIC3254_Device_Address,0x00, 0x00);
  I2C_singleWrite(AIC3254_Device_Address,0x51, 0xC0);
  //unMute both ADCs
  I2C_singleWrite(AIC3254_Device_Address,0x52, 0x00);
  
  //power up both DACs
  I2C_singleWrite(AIC3254_Device_Address,0x3F, 0xD4);
  //unMute both DACs
  I2C_singleWrite(AIC3254_Device_Address,0x40, 0x00);
  
  //configure the Input Mux in miniDSP process flow
  I2C_singleWrite(AIC3254_Device_Address, 0x00, 0x09);
  I2C_singleWrite(AIC3254_Device_Address, 0x6E, 0x02); 
  I2C_singleWrite(AIC3254_Device_Address, 0x00, 0x08);
  I2C_singleWrite(AIC3254_Device_Address, 0x01, 0x05);
  I2C_singleWrite(AIC3254_Device_Address, 0x00, 0x09);
  I2C_singleWrite(AIC3254_Device_Address, 0x6E, 0x02); 
}
  

//this function is used when switch from Analog mode to I2S mode
void AIC3254_toI2SMode(void)
{
  //select page 0
  I2C_singleWrite(AIC3254_Device_Address, 0x00, 0x00);
  //Mute ADCs 
  I2C_singleWrite(AIC3254_Device_Address, 0x52, 0x88);
  //power down ADCs
  I2C_singleWrite(AIC3254_Device_Address, 0x51, 0x00);
  //Mute DACs
  I2C_singleWrite(AIC3254_Device_Address, 0x40, 0x0C);
  //power down both DACs
  I2C_singleWrite(AIC3254_Device_Address, 0x3F, 0x14);
  //power down PLL
  I2C_singleWrite(AIC3254_Device_Address, 0x05, 0x11);

  //configure PLL settings
  unsigned char pllData[]={0x03, 0x91, 0x04};
  I2C_multiWrite(AIC3254_Device_Address, 0x04, 3, pllData);
  
  //configure Audio interface
  I2C_singleWrite(AIC3254_Device_Address,0x00, 0x00);
  I2C_singleWrite(AIC3254_Device_Address,0x21, 0x00);  
  I2C_singleWrite(AIC3254_Device_Address,0x1F, 0x12);  //sec_bclk path
  I2C_singleWrite(AIC3254_Device_Address,0x37, 0x14);   
  I2C_singleWrite(AIC3254_Device_Address,0x34, 0x20);
  I2C_singleWrite(AIC3254_Device_Address,0x20, 0x00);   //interface path
  I2C_singleWrite(AIC3254_Device_Address,0x1D, 0x01);//disable digital loopback
  I2C_singleWrite(AIC3254_Device_Address,0x1E, 0x02); //power down Bclk divider, and set ratio=2
  I2C_singleWrite(AIC3254_Device_Address,0x1B, 0x00);
  
  I2C_singleWrite(AIC3254_Device_Address,0x3C, 0x80);
  I2C_singleWrite(AIC3254_Device_Address,0x3D, 0x00);
  I2C_singleWrite(AIC3254_Device_Address,0x00, 0x08);
  I2C_singleWrite(AIC3254_Device_Address,0x01, 0x04);
  I2C_singleWrite(AIC3254_Device_Address,0x00, 0x2C);
  I2C_singleWrite(AIC3254_Device_Address,0x01, 0x04);
  I2C_singleWrite(AIC3254_Device_Address,0x00, 0x00);
   
  //power up both ADCs
  I2C_singleWrite(AIC3254_Device_Address,0x51, 0xC0);
  //unMute both ADCs
  I2C_singleWrite(AIC3254_Device_Address,0x52, 0x00);
  //power up both DACs
  I2C_singleWrite(AIC3254_Device_Address,0x3F, 0xD4);
  //unMute both DACs
  I2C_singleWrite(AIC3254_Device_Address,0x40, 0x00);
  
  //configure the Input Mux in miniDSP process flow
  I2C_singleWrite(AIC3254_Device_Address, 0x00, 0x09);
  I2C_singleWrite(AIC3254_Device_Address, 0x6E, 0x01); 
  I2C_singleWrite(AIC3254_Device_Address, 0x00, 0x08);
  I2C_singleWrite(AIC3254_Device_Address, 0x01, 0x05);  //flip buffers
  I2C_singleWrite(AIC3254_Device_Address, 0x00, 0x09);
  I2C_singleWrite(AIC3254_Device_Address, 0x6E, 0x01);
  
}

void AIC3254_USB_IN(void)
{ 
  LED_p0 &= 0x7F;  //clear SPDIF LED light
  LED_p1 &= 0x20;
  LED_p1 |= 0x08;
  unsigned char D[] = {LED_p0, LED_p1}; 
  I2C_multiWrite(0x20, 0x02, 2, D);    //turn on the LED lights
  
  //switch from Analog mode to I2S mode
  AIC3254_toI2SMode();  
  
  //configure the on-board hardware switch
  P3OUT_bit.P3OUT_7 = 0;   //switch_EN, enable the switch
  P3OUT_bit.P3OUT_6 = 0;   //switch_Logic low, USB path
  P2OUT_bit.P2OUT_2 = 0;   //CKSEL, DIR9001 in PLL mode
  
  VolumeReset();
}


void AIC3254_SPDIF_IN(void)
{
  LED_p0 |= 0x80;
  LED_p1 &= 0x20;
  unsigned char D[] = {LED_p0, LED_p1}; 
  I2C_multiWrite(0x20, 0x02, 2, D);   //turn on the LED lights
  
  //switch from analog mode to I2S mode
  AIC3254_toI2SMode(); 
  
  //configure the on-board hardware switch
  P3OUT_bit.P3OUT_7 = 0;    //switch_EN, enable the switch
  P3OUT_bit.P3OUT_6 = 1;   //switch_Logic high, SPDIF path
  P2OUT_bit.P2OUT_2 = 0;   //CKSEL, DIR9001 in PLL mode
  
  VolumeReset();
}


void AIC3254_ANALOG_IN(void)
{    
  LED_p0 &= 0x7F;   //clear the SPDIF LED light
  LED_p1 &= 0x20; 
  
  //configure GPIO pins
  P3OUT_bit.P3OUT_7 = 1;    //switch_EN, Disable the switch
  P2OUT_bit.P2OUT_2 = 1;    //CKSEL, non-PLL mode,only generate clocks for AIC3254
  
  //switch from I2S mode to Analog mode
  AIC3254_toAnalogMode();
  
  if (Analog_counter == 1)  
  { 
    LED_p1 |= 0x01;
    unsigned char D1[] = {LED_p0, LED_p1}; 
    I2C_multiWrite(0x20, 0x02, 2, D1);   //LED lights for Analog1
    
    I2C_singleWrite(AIC3254_Device_Address,0x00, 0x01);
    I2C_singleWrite(AIC3254_Device_Address,0x34, 0x80); //input rounting to IN1   
    I2C_singleWrite(AIC3254_Device_Address,0x36, 0x80);
    I2C_singleWrite(AIC3254_Device_Address,0x37, 0x80);
    I2C_singleWrite(AIC3254_Device_Address,0x39, 0x80);
  }
  
  if (Analog_counter == 2)  
  {
    LED_p1 |= 0x02;
    unsigned char D2[] = {LED_p0, LED_p1}; 
    I2C_multiWrite(0x20, 0x02, 2, D2);  //LED lights for Analog2
    
    I2C_singleWrite(AIC3254_Device_Address,0x00, 0x01);  
    I2C_singleWrite(AIC3254_Device_Address, 0x34, 0x20); //input rounting to IN2
    I2C_singleWrite(AIC3254_Device_Address, 0x36, 0x80);
    I2C_singleWrite(AIC3254_Device_Address, 0x37, 0x20);
    I2C_singleWrite(AIC3254_Device_Address, 0x39, 0x80);
  }
  
  
  if (Analog_counter == 3) 
  {
    LED_p1 |= 0x04;
    unsigned char D3[] = {LED_p0, LED_p1}; 
    I2C_multiWrite(0x20, 0x02, 2, D3);  //LED lights for Analog3
   
    I2C_singleWrite(AIC3254_Device_Address,0x00, 0x01); 
    I2C_singleWrite(AIC3254_Device_Address,0x34, 0x08);  //input rounting to IN3
    I2C_singleWrite(AIC3254_Device_Address,0x36, 0x80);
    I2C_singleWrite(AIC3254_Device_Address,0x37, 0x08);
    I2C_singleWrite(AIC3254_Device_Address,0x39, 0x80);
       
    Analog_counter = 0;   //clear the analog counter
  }
  
  Analog_counter++;
  VolumeReset();
}


void AIC3254_MUTE(void)   //toggle
{
/************MUTE AIC3254 DAC**********************************/
  if (mute_status)
  {
    LED_p1 |= 0x20;
    unsigned char D[] = {LED_p0, LED_p1}; 
    I2C_multiWrite(0x20, 0x02, 2, D);  // turn ON mute LED light
    
    I2C_singleWrite(AIC3254_Device_Address,0x00, 0x00); //configure AIC3254
    I2C_singleWrite(AIC3254_Device_Address, 0x40,0x0c);
    mute_status = 0;
  }
  else 
  {
    LED_p1 &= ~0x20; 
    unsigned char D[] = {LED_p0, LED_p1}; 
    I2C_multiWrite(0x20, 0x02, 2, D);    //turn OFF mute LED light
    
    I2C_singleWrite(AIC3254_Device_Address,0x00, 0x00);
    I2C_singleWrite(AIC3254_Device_Address, 0x40,0x00);
    mute_status = 1;
  } 
}


void AIC3254_SD(void)  //Toggle
{
/**************shutdown TPA3110******************/
  if(shutdown_status)
  {
    LED_p0 |= 0x01;
    unsigned char D[] = {LED_p0, LED_p1}; 
    I2C_multiWrite(0x20, 0x02, 2, D);   //turn ON SD LED 
    
    P3OUT_bit.P3OUT_0 = 0;  //Bring TPA3110 into Shutdown
    shutdown_status = 0;
  }
  else
  {
    LED_p0 &= ~0x01;
    unsigned char D[] = {LED_p0, LED_p1}; 
    I2C_multiWrite(0x20, 0x02, 2, D);   //turn OFF SD LED
    
    P3OUT_bit.P3OUT_0 = 1; //Bring TPA3110 out of Shutdown
    shutdown_status = 1;
  }  
}


void TurnOffLED(void)
{
  //set PCA9535 pins all as output 
  unsigned char Type[] = {0x00, 0x00};
  I2C_multiWrite(0x20, 0x06, 2, Type);  //setting Port0 and Port1 as OUTPUT
  
  LED_p0 &= 0x00;
  LED_p1 &= 0x00;
  unsigned char D[] = {LED_p0, LED_p1};
  I2C_multiWrite(0x20, 0x02, 2, D);
}


void VolumeReset(void)
{  
  I2C_singleWrite(AIC3254_Device_Address,0x00, 0x00);
  I2C_singleWrite(AIC3254_Device_Address,0x41,0xF4);    //LDAC Volume
  I2C_singleWrite(AIC3254_Device_Address,0x42,0xF4);     //RDAC Volume
  Volume_counter=4;
  
  LED_p0 &= 0xE1;
  LED_p0 |= 0x18;
  unsigned char D[] = {LED_p0, LED_p1};
  I2C_multiWrite(0x20, 0x02, 2, D);
}
  

void  AIC3254_SRRD0(void)     // button control for EQ ON/OFF, toggle
{
  if(EQ_status)    //both EQ ON
  {
    LED_p0 &= 0xBF;
    LED_p0 |= 0x40; 
    unsigned char D[] = {LED_p0, LED_p1}; 
    I2C_multiWrite(0x20, 0x02, 2, D);   //turn ON SD LED
    
    I2C_singleWrite(AIC3254_Device_Address, 0x00, 0x09);
    I2C_singleWrite(AIC3254_Device_Address, 0x72, 0x01);
    I2C_singleWrite(AIC3254_Device_Address, 0x00, 0x08);
    I2C_singleWrite(AIC3254_Device_Address, 0x01, 0x05);
    I2C_singleWrite(AIC3254_Device_Address, 0x00, 0x09);
    I2C_singleWrite(AIC3254_Device_Address, 0x72, 0x01);  //end of Mux B
    
    
    I2C_singleWrite(AIC3254_Device_Address, 0x00, 0x0c);
    I2C_singleWrite(AIC3254_Device_Address, 0x5A, 0x01);
    I2C_singleWrite(AIC3254_Device_Address, 0x00, 0x08);
    I2C_singleWrite(AIC3254_Device_Address, 0x01, 0x05);
    I2C_singleWrite(AIC3254_Device_Address, 0x00, 0x0c);
    I2C_singleWrite(AIC3254_Device_Address, 0x5A, 0x01);
       
    EQ_status = 0;
  }
  else
  {
    LED_p0 &= ~0x40; 
    unsigned char D[] = {LED_p0, LED_p1}; 
    I2C_multiWrite(0x20, 0x02, 2, D);   //turn OFF EQ LED
   
    I2C_singleWrite(AIC3254_Device_Address, 0x00, 0x09);
    I2C_singleWrite(AIC3254_Device_Address, 0x72, 0x02);
    I2C_singleWrite(AIC3254_Device_Address, 0x00, 0x08);
    I2C_singleWrite(AIC3254_Device_Address, 0x01, 0x05);
    I2C_singleWrite(AIC3254_Device_Address, 0x00, 0x09);
    I2C_singleWrite(AIC3254_Device_Address, 0x72, 0x02);   
    
    
    I2C_singleWrite(AIC3254_Device_Address, 0x00, 0x0c);
    I2C_singleWrite(AIC3254_Device_Address, 0x5A, 0x02);
    I2C_singleWrite(AIC3254_Device_Address, 0x00, 0x08);
    I2C_singleWrite(AIC3254_Device_Address, 0x01, 0x05);
    I2C_singleWrite(AIC3254_Device_Address, 0x00, 0x0c);
    I2C_singleWrite(AIC3254_Device_Address, 0x5A, 0x02);
    
    EQ_status = 1;
  }  
}



void  AIC3254_SRRD1(void)     //Turn SRS_WOW ON/OFF, toggle
{
  if(SRSwow_status)    //both EQ ON
  {
    LED_p0 &= 0xDF;
    LED_p0 |= 0x20;
    unsigned char D[] = {LED_p0, LED_p1}; 
    I2C_multiWrite(0x20, 0x02, 2, D);   //turn ON SD LED
    
    I2C_singleWrite(AIC3254_Device_Address, 0x00, 0x0c);
    I2C_singleWrite(AIC3254_Device_Address, 0x56, 0x02);
    I2C_singleWrite(AIC3254_Device_Address, 0x00, 0x08);
    I2C_singleWrite(AIC3254_Device_Address, 0x01, 0x05);
    I2C_singleWrite(AIC3254_Device_Address, 0x00, 0x0c);
    I2C_singleWrite(AIC3254_Device_Address, 0x56, 0x02);
    
    
    I2C_singleWrite(AIC3254_Device_Address, 0x00, 0x09);
    I2C_singleWrite(AIC3254_Device_Address, 0x72, 0x01);
    I2C_singleWrite(AIC3254_Device_Address, 0x00, 0x08);
    I2C_singleWrite(AIC3254_Device_Address, 0x01, 0x05);
    I2C_singleWrite(AIC3254_Device_Address, 0x00, 0x09);
    I2C_singleWrite(AIC3254_Device_Address, 0x72, 0x01);
    
    
    I2C_singleWrite(AIC3254_Device_Address, 0x00, 0x0c);
    I2C_singleWrite(AIC3254_Device_Address, 0x5A, 0x02);
    I2C_singleWrite(AIC3254_Device_Address, 0x00, 0x08);
    I2C_singleWrite(AIC3254_Device_Address, 0x01, 0x05);
    I2C_singleWrite(AIC3254_Device_Address, 0x00, 0x0c);
    I2C_singleWrite(AIC3254_Device_Address, 0x5A, 0x02);
    
    SRSwow_status = 0;
  }
 else
  {
    LED_p0 &= ~0x20;
    unsigned char D[] = {LED_p0, LED_p1}; 
    I2C_multiWrite(0x20, 0x02, 2, D);   //turn OFF EQ LED
    
    I2C_singleWrite(AIC3254_Device_Address, 0x00, 0x0c);
    I2C_singleWrite(AIC3254_Device_Address, 0x56, 0x01);
    I2C_singleWrite(AIC3254_Device_Address, 0x00, 0x08);
    I2C_singleWrite(AIC3254_Device_Address, 0x01, 0x05);
    I2C_singleWrite(AIC3254_Device_Address, 0x00, 0x0c);
    I2C_singleWrite(AIC3254_Device_Address, 0x56, 0x01);
    
    
    I2C_singleWrite(AIC3254_Device_Address, 0x00, 0x09);
    I2C_singleWrite(AIC3254_Device_Address, 0x72, 0x02);
    I2C_singleWrite(AIC3254_Device_Address, 0x00, 0x08);
    I2C_singleWrite(AIC3254_Device_Address, 0x01, 0x05);
    I2C_singleWrite(AIC3254_Device_Address, 0x00, 0x09);
    I2C_singleWrite(AIC3254_Device_Address, 0x72, 0x02);
    
    
    I2C_singleWrite(AIC3254_Device_Address, 0x00, 0x0c);
    I2C_singleWrite(AIC3254_Device_Address, 0x5A, 0x02);
    I2C_singleWrite(AIC3254_Device_Address, 0x00, 0x08);
    I2C_singleWrite(AIC3254_Device_Address, 0x01, 0x05);
    I2C_singleWrite(AIC3254_Device_Address, 0x00, 0x0c);
    I2C_singleWrite(AIC3254_Device_Address, 0x5A, 0x02);
    
    SRSwow_status = 1;
  }   
}
