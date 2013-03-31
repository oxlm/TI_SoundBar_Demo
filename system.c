#include "device.h"
#include "system.h"

void InitOsc(void)
{
  WDTCTL = WDTPW + WDTHOLD;   //stop WDT
  BCSCTL1 = CALBC1_8MHZ;
  DCOCTL = CALDCO_8MHZ;    //Set MCU main clock to 8MHz
}

void InitPeriph(void)
{
  /*  Select pin function     0 = I/O function                          */
  /*                          1 = peripheral function                   */
    
  /*  Select pin direction    0 = input                                 */
  /*                          1 = output                                */  
    
  /************configure Port 1***************/
  P1SEL  = 0x00; //set all pins of Port 1 to be I/O function
  P1DIR  = 0x00; //set all pins as Input
  P1REN  = 0xFF; //enable the pull-up resistors
  P1IE   = 0xFF; //enable interrupt
  P1IES  = 0xFF;       
  P1IFG  = 0x00; //clear the interrupt flags
  P1OUT  = 0xFF; //configure as Pull-up mode 
  P1SEL2 = 0x00; //configure as I/O function along with P1SEL
  
  /*******configure Port 2**************/
  P2SEL_bit.P3 = 0;    //set as I/O function
  P2DIR_bit.P3 = 0;     // set as INPUT
  P2REN_bit.P3 = 1;    //enable pull-up resister
  P2IE_bit.P3 =   1;      //enable interrupt
  P2IES_bit.P3 = 1;     //interrupt edge select
  P2IFG_bit.P3 = 0;    // clear interrupt flag
  P2SEL2_bit.P3 = 0; 
  P2OUT_bit.P3 = 1;
  
  P2SEL_bit.P4 = 0;    //set as I/O function
  P2DIR_bit.P4 = 0;     // set as INPUT
  P2REN_bit.P4 = 1;    //enable pull-up resister
  P2IE_bit.P4 =   1;      //enable interrupt
  P2IES_bit.P4 = 1;     //interrupt edge select
  P2IFG_bit.P4 = 0;    // clear interrupt flag
  P2SEL2_bit.P4 = 0;
  P2OUT_bit.P4 = 1;
  
  /*********** pin2.2 is CKSEL *************/  
  P2SEL_bit.P2 = 0;      //I/O function
  P2SEL2_bit.P2 = 0;
  P2DIR_bit.P2 = 1;      //Output
  P2OUT_bit.P2 = 1;      // start with HIGH
    
  /*********** pin3.7 is Switch_EN **********/
  P3SEL_bit.P7 = 0;      //I/O function  
  P3SEL2_bit.P7 = 0;  //I/O function
  P3DIR_bit.P7 = 1;      //Output
  P3OUT_bit.P7 = 1;      //start with HIGH, switch disabled 
    
    
  /************* pin3.6 is Switch_Logic ********/ 
  P3SEL_bit.P6 = 0;        //I/O function
  P3SEL2_bit.P6 = 0;
  P3DIR_bit.P6 = 1;        //Output
  P3OUT_bit.P6 = 0;        //start with LOW
    
  /********** pin 3.0 is SDpin ****************/
  P3SEL_bit.P0 = 0;        //I/O function
  P3SEL2_bit.P0 = 0; 
  P3DIR_bit.P0 = 1;        //Output
  P3OUT_bit.P0 = 0;        //start with LOW
}


void Delay_ms(int time)                     // Delay function in ms
{
  for (volatile int i = 0; i < time; i++)
  {
    for (volatile int h = 0; h < 100; h++);
  }
}

