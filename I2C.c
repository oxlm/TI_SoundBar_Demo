#include  "device.h"

unsigned char Rx_counter;
unsigned char Tx_counter;
unsigned char *Rx_Pointer;
unsigned char *Tx_Pointer;

void I2C_Tx_Init(unsigned char I2C_master_speed)
{
  P3SEL |= SDA_PIN + SCL_PIN;                 // Assign I2C pins to USCI_B0
  UCB0CTL1 = UCSWRST;                         // Enable SW reset
  UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;       // I2C Master, synchronous mode
  UCB0CTL1 = UCSSEL_2 + UCSWRST;              // Use SMCLK, keep SW reset
  UCB0BR0 = I2C_master_speed;                 // set I2C speed
  UCB0BR1 = 0;                  
  UCB0CTL1 &= ~UCSWRST;                        // Clear SW reset, resume operation 
}

int I2CWait(void)
{
  unsigned long nCount = 0;
  
  while ((nCount < 10000) && !(IFG2 & UCB0TXIFG) && !(UCB0STAT & UCNACKIFG)) nCount++; // wait for TX interrupt flag
  if ((nCount >= 10000) || (UCB0STAT & UCNACKIFG))
    return 0;
  IFG2 &= ~UCB0TXIFG;    //clear interrupt flag
  return 1;
}

void I2CCheckBusy(void)
{
  unsigned long nCount = 0;

  while (UCB0STAT & UCBBUSY)
  {
    if (nCount >= 50000)
    {
      nCount = 0;
      I2C_Tx_Init(0x14);  //Initialize I2C transmit
    }
  }

}

void I2C_singleWrite(unsigned char deviceAddr, unsigned char registerAddr, unsigned char W_data)
{
//  if (deviceAddr == AIC3254_Device_Address) return;
  bool bI2COk = false;
  do
  {
    I2C_Tx_Init(0x14);  //Initialize I2C transmit
    I2CCheckBusy();
    UCB0I2CSA = deviceAddr;    //sending the device address
    UCB0CTL1 |= UCTR + UCTXSTT;     //I2C transmit, start condition
    UCB0TXBUF = registerAddr;       //sending the register address (1st byte)
  
    if (I2CWait())
    {
      UCB0TXBUF = W_data;               //sending the single-byte to transmit buffer
      bI2COk = I2CWait();
    }
      
    IFG2 &= ~UCB0TXIFG;   //clear interrupt flag
    UCB0CTL1 |= UCTXSTP;    //I2C stop
  } while (!bI2COk);
  
  Delay_ms(1);
}

void I2C_multiWrite(unsigned char deviceAddr, unsigned char registerAddr, unsigned char wordLength, unsigned char *W_bytes)
{
//  if (deviceAddr == AIC3254_Device_Address) return;
  bool bI2COk;
  do
  {
    I2C_Tx_Init(0x14);  //Initialize I2C transmit
    bI2COk = true;
    I2CCheckBusy();
    Tx_counter = wordLength;   //# of bytes
    Tx_Pointer = W_bytes;       
    UCB0I2CSA = deviceAddr;          //Device address
    UCB0CTL1 |= UCTR + UCTXSTT;      //I2C transmit, start condition
    
    
    if (I2CWait())
    {
      UCB0TXBUF = registerAddr;        //register address
      if (I2CWait())
      {  
        while ((Tx_counter) && bI2COk)
        {
          UCB0TXBUF = *Tx_Pointer;
          bI2COk = I2CWait();
          if (bI2COk)
          {
            Tx_Pointer++;
            Tx_counter--;
          }
        }
      }
    }
    UCB0CTL1 |= UCTXSTP;   //I2C stop 
    IFG2 &= ~UCB0TXIFG;    //clear interrupt flag
  } while (Tx_counter);
  
  Delay_ms(1);
}

void I2C_writeEEPROM(unsigned char deviceAddr, unsigned int nAddress, unsigned char wordLength, unsigned char const *W_bytes)
{
  while(UCB0STAT & UCBBUSY);  //wait for I2C bus to be free
  Tx_counter = wordLength;   //# of bytes
  unsigned char const *Tx_Pointer1 = W_bytes;       
  UCB0I2CSA = deviceAddr;          //Device address
  UCB0CTL1 |= UCTR + UCTXSTT;      //I2C transmit, start condition

  UCB0TXBUF = (nAddress & 0xFF00) >> 8;
  
  while(UCB0CTL1 & UCTXSTT);       //start condition sent?
  while((IFG2&UCB0TXIFG)!= UCB0TXIFG);

  while (!(IFG2&UCB0TXIFG));
  UCB0TXBUF = (nAddress & 0x00FF);
  while((IFG2&UCB0TXIFG)!= UCB0TXIFG);
    
  while(Tx_counter)
  {
    if(IFG2&UCB0TXIFG)
    {
      UCB0TXBUF = *Tx_Pointer1;
      while((IFG2&UCB0TXIFG)!= UCB0TXIFG);
      Tx_Pointer1++;
      Tx_counter--;
    }
  }
  UCB0CTL1 |= UCTXSTP;   //I2C stop 
  IFG2 &= ~UCB0TXIFG;    //clear interrupt flag
}

void I2C_readEEPROM(unsigned char deviceAddr, unsigned int nAddress, unsigned char wordLength, unsigned char *R_bytes)
{
  volatile unsigned char cDummy;
  while (IFG2&UCB0RXIFG)
  {
    cDummy = UCB0RXBUF;
    IFG2 &= ~UCB0RXIFG;
  }
  
  while(UCB0STAT & UCBBUSY);  //wait for I2C bus to be free
  unsigned char *Rx_Pointer = R_bytes;       
  UCB0I2CSA = deviceAddr;          //Device address
  UCB0CTL1 |= UCTR + UCTXSTT;      //I2C transmit, start condition

  UCB0TXBUF = (nAddress & 0xFF00) >> 8;
  
  while(UCB0CTL1 & UCTXSTT);       //start condition sent?
  while((IFG2&UCB0TXIFG)!= UCB0TXIFG);

  while (!(IFG2&UCB0TXIFG));
  UCB0TXBUF = (nAddress & 0x00FF);
  while((IFG2&UCB0TXIFG)!= UCB0TXIFG);

  UCB0CTL1 &= ~UCTR;
  //IFG2 &= ~UCB0RXIFG;

  UCB0CTL1 |= UCTXSTT;      //I2C transmit, start condition
  while(UCB0CTL1 & UCTXSTT);       //start condition sent?

  while(wordLength)
  {
    if(IFG2&UCB0RXIFG)
    {
      IFG2 &= ~UCB0RXIFG;
      *Rx_Pointer = UCB0RXBUF;
      Rx_Pointer++;
      wordLength--;
    }
  }
  
  UCB0CTL1 |= UCTXSTP;   //I2C stop 
  while(UCB0CTL1 & UCTXSTP);
  Delay_ms(1);
//  IFG2 &= ~UCB0RXIFG;    //clear interrupt flag
}
