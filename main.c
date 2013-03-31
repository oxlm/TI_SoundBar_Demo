#include "device.h"
#include "processflow.h"
#include "display.h"
#include "IR.h"

void main(void)
{
  
  InitOsc();     //Set MCU clock
  Delay_ms(10000);  // let TAS1020b load its firmware
  InitPeriph();  //Set MCU GPIOs

  I2C_Tx_Init(0x14);  //Initialize I2C transmit
  SBInit();
  UpdateDisplay();
  
  LoadProcessFlow(1, 1);  

  sbState.bShutdown = false;
  sbState.bDisplayOn = true;
  SetShutdown(sbState.bShutdown);
  UpdateDisplay();

  StartDisplayTimer();
  
  __bis_SR_register(GIE);     //enable global interrupts
 
//while (1);
}
