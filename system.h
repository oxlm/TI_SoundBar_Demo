#ifndef __SYSTEM_H
#define __SYSTEM_H

#define VOLUP_BTN  0x02                 //pin 1.1
#define VOLDOWN_BTN 0x01                //pin 1.0
#define USB_BTN  0x80                   //pin 1.7
#define SPDIF_BTN 0x40                  //pin 1.6
#define ANALOG_BTN 0x20                 //pin 1.5
#define MUTE_BTN 0x08                   //pin 1.3
#define SD_BTN 0x10                     //pin 1.4
#define SRRD0_BTN 0x08                  //pin 2.3
#define SRRD1_BTN 0x10                  //pin 2.4

__no_init volatile union
{
  unsigned char P3SEL2;  /* Port 1 Selection 2 */ 
  
  struct
  {
    unsigned char P3SEL2_0       : 1; 
    unsigned char P3SEL2_1       : 1; 
    unsigned char P3SEL2_2       : 1; 
    unsigned char P3SEL2_3       : 1; 
    unsigned char P3SEL2_4       : 1; 
    unsigned char P3SEL2_5       : 1; 
    unsigned char P3SEL2_6       : 1; 
    unsigned char P3SEL2_7       : 1; 
  } P3SEL2_bit;  
} @ 0x0043;

enum {
  P3SEL2_0            = 0x0001,
  P3SEL2_1            = 0x0002,
  P3SEL2_2            = 0x0004,
  P3SEL2_3            = 0x0008,
  P3SEL2_4            = 0x0010,
  P3SEL2_5            = 0x0020,
  P3SEL2_6            = 0x0040,
  P3SEL2_7            = 0x0080,
};

void InitOsc(void);
void InitPeriph(void);
void Delay_ms(int time);

#endif
