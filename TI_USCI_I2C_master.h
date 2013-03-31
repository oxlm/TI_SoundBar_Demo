#ifndef USCI_LIB_H
#define USCI_LIB_H

#include  "io430x21x2.h"
#define SDA_PIN 0x02                                  // msp430x261x UCB0SDA pin, pin 3 = 0000 0010
#define SCL_PIN 0x04                                  // msp430x261x UCB0SCL pin, pin 3 = 0000 0100

void I2C_Tx_Init(unsigned char I2C_master_speed);
void I2C_singleWrite(unsigned char deviceAddr, unsigned char registerAddr, unsigned char W_data);
void I2C_multiWrite(unsigned char deviceAddr, unsigned char registerAddr, unsigned char wordLength, unsigned char *W_bytes);
void I2C_writeEEPROM(unsigned char deviceAddr, unsigned int nAddress, unsigned char wordLength, unsigned char const *W_bytes);
void I2C_readEEPROM(unsigned char deviceAddr, unsigned int nAddress, unsigned char wordLength, unsigned char *R_bytes);

#endif
