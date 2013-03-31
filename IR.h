#ifndef __IR_H
#define __IR_H

#define NEC_ADDRESS 0
#define NEC_CMD_VOL_UP 0x82
#define NEC_CMD_VOL_DN 0xA2
#define NEC_CMD_MUTE 0x12

#define RC5_BITS 14
#define RC5_ADDRESS 0
#define RC5_CMD_VOL_UP 0x1E
#define RC5_CMD_VOL_DN 0x1C
#define RC5_CMD_MUTE 0x04

void ProcessIR(void);

#endif
