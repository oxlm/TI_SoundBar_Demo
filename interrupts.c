#include "interrupts.h"
#include "buttons.h"
#include "system.h"
#include "IR.h"
#include "display.h"

#define DEBOUNCE 100

#pragma vector = PORT2_VECTOR        
__interrupt void P2_Interrupt(void)
{
  unsigned char P2IN_B3_Temp = P2IN_bit.P2IN_3;
  unsigned char P2IN_B4_Temp = P2IN_bit.P2IN_4;
  volatile unsigned int k;
  
  for (k = 0; k < DEBOUNCE; k++)   
  {
    if((P2IN_B3_Temp == P2IN_bit.P2IN_3) && (P2IN_B4_Temp == P2IN_bit.P2IN_4))
      Delay_ms(10);
    else
      break;
  }
  
  if (k >= DEBOUNCE)    //if no other interrupts, then start evaluating the current interrupt
  {
    switch(P2IFG & 0x18)    //P2IFG is the Port 2 interrupt flags
    {
    case SRRD0_BTN:    //Tru_surround0 button pressed
      Buttons(surround0);
      Delay_ms(100);
      break;
    
    case SRRD1_BTN:  //Tru_surround1 button pressed
      Buttons(surround1);
      Delay_ms(100);
      break;
    }
    
    P2IFG = 0x00;   //clear port 2 interrupt flags
  }
}

/********Port 1 Interrupts***********/
#pragma vector = PORT1_VECTOR
__interrupt void P1_Interrupt(void)
{
  volatile unsigned int i;
  unsigned char P1IN_Temp = P1IN;
    
  /* IR hook start */
  if (P1IFG & 0x04)
  {
    ProcessIR();
    P1IFG = 0x00;
    return;
  }
 /* IR hook end */  


  for (i = 0; i < DEBOUNCE; i++) 
  {
    if(P1IN_Temp == P1IN)
      Delay_ms(10);
    else
      break;
  }

  
  if (i >= DEBOUNCE)   //if no other inturrupts, then start evaluating the current interrupt
  {
    switch(P1IFG & 0xFF)  //P1IFG is the interrupt flags of Port 1
    {    
    case VOLUP_BTN:     //volume up button pressed
      Buttons(volumeUp);
      Delay_ms(100);
      break;
    
    case VOLDOWN_BTN:   //volume down button pressed
      Buttons(volumeDown);
      Delay_ms(100);
      break;
      
    case USB_BTN:     //USB button pressed 
      Buttons(usbMode);
      Delay_ms(100);
      break;
      
    case SPDIF_BTN:  //SPDIF button pressed 
      Buttons(spdifMode);
      Delay_ms(100);
      break;
      
    case ANALOG_BTN:   //Analog button pressed 
      Buttons(analogMode);
      Delay_ms(100);
      break;
      
    case MUTE_BTN:    //Mute button pressed
      Buttons(muteMode);
      Delay_ms(100);
      break;
      
    case SD_BTN:     //Shutdown button pressed
      Buttons(shutdownMode);
      Delay_ms(100);
      break;
    }
    P1IFG = 0x00;   //clear Port 1 interrupt flags
  }
}

#ifdef ENABLE_DISPLAY_TIMER

#pragma vector = TIMER0_A0_VECTOR
__interrupt void CCR0_ISR(void)
{
  if (sbState.nDisplayTimeCount++ >= 50)
  {
    TACCTL0 &= ~CCIE;
    sbState.nDisplayTimeCount = 0;
    sbState.bDisplayOn = false;
    UpdateDisplay();
  }
}

void StartTimer(unsigned int nPeriod)
{
  sbState.nDisplayTimeCount = 0;
  sbState.bDisplayOn = true;
  TACCR0 = 62500;
  TACCTL0 = CCIE;
  TACTL = TASSEL_2 | ID_3 | MC_1 | TACLR;
}
#endif

