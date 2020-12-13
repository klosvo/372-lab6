#include "i2c.h"
#include <avr/io.h>
#include "Arduino.h"

#define wait_for_completion while(!(TWCR & (1 << TWINT)));
#define I2C_WRITE 0
#define I2C_READ 1


void initI2C() {
    PRR0 &= ~(1 << PRTWI); // Wake up from power reduction

    // Set Prescalar value to 1
    TWSR |= (1<<TWPS0);
    TWSR &= ~(1<<TWPS1);

    TWBR = 0xC6; // TWBR = 18 based on SCL frequency of 1949 Hz

    TWCR |= (1<<TWINT) | (1<<TWEN); // Set 2 wire interface
}

 
void StartI2C_Trans(unsigned char SLA) {
    TWCR = (1<<TWEN) | (1<<TWINT) | (1<<TWSTA); // reset flag and initiate start condition
    wait_for_completion;
    TWDR = (SLA<<1); // Slave address + write bit 0
    TWCR = (1<<TWEN) | (1<<TWINT); // clear flag and enable TWI
    wait_for_completion;
}

void StopI2C_Trans() {
    // stop condition set
  TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
  // wait_for_completion;

}


// Kama - TODO
/*
- Write(unsigned char data)
o Set two wire data register equal to incoming data o Trigger action
o Wait for completion
*/
void write(unsigned char data){
  TWDR = data;
  TWCR = (1 << TWINT) | (1 << TWEN);  //  Trigger I2C Action
  wait_for_completion; // waits til this is sent
}


/* - Read_from(unsigned char SLA, unsigned Char MEMADDRESS)
o Start a transmission to the SLA
o Write to the MEMADDRESS
o Clear TWINT, initiate start condition, initiate enable 
o Wait for completion
o Set two wire data register to the SLA + read bit 
o Trigger action + master acknowledge bit
o Wait for completion
o Trigger action
o Wait for completion
o Trigger action + stop condition
*/
// need 2 resistors for i2c 1.8k, 4.7k, or 10k
void Read_from(unsigned char SLA, unsigned char MEMADDRESS){

  StartI2C_Trans(SLA);

  // send register address
  write(MEMADDRESS);

  // Switch Master to receiver/read mode and slave to transmitter
  // New start condition
  TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTA);
  wait_for_completion; // waits til this is sent

  // Send slave address and read signal
  TWDR = (SLA << 1) | I2C_READ;
  TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA); //  Trigger I2C Action w/ ACK set by TWEA
  wait_for_completion; // waits til this is sent

  // no acknowledge bit set, NOT ACK, ending read
  TWCR = (1 << TWINT) | (1 << TWEN); // NO ACK
  wait_for_completion;

  StopI2C_Trans();
}

 /*
 - Read_data()
o Return TWDR
*/ 
unsigned char Read_data() 
{
    return TWDR;
}
  