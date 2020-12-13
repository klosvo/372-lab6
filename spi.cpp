#include <Arduino.h>
#include "spi.h"
#include <avr/interrupt.h>
#include <avr/io.h>

// SPI: Serial Peripheral Interface Communication Protocol
// Full Duplex and synchronous (needs clock)
// 2 pins for data transfer
//      -> SDI (Din) / MOSI (Master Output/Slave Input)
//      -> SDO (Dout) / MISO (Master Input/Slave Output)
// Max of 1 Master, Unlimited Slaves (theoretically)
// Works as Distributed Shift Registers - Serial-In/Serial-Out
//      -> 2 shift registers
//      -> Like a 16 bit register distributed across two devices
// Simultaneously transmits and receives, starts at LSB and works way through to MSB

#define wait_for_completion while(!(SPSR & (1 << SPIF))); // use flag register to determine whether all data is completed correctly

// Register Address Map
#define NO_OP 0x0

#define DIGIT_0 0x01
#define DIGIT_1 0x02
#define DIGIT_2 0x03
#define DIGIT_3 0x04
#define DIGIT_4 0x05
#define DIGIT_5 0x06
#define DIGIT_6 0x07
#define DIGIT_7 0x08

#define DECODE_MODE 0x09
#define INTENSITY 0x0A // brightness control
#define SCAN_LIMIT 0x0B
#define SHUTDOWN 0x0C
#define DISPLAY_TEST 0x0F


// About SPI

// About MAX7219
/*
For the MAX7219, serial data at DIN, sent in 16-bit packets, 
is shifted into the internal 16-bit shift register with each 
rising edge of CLK regardless of the state of LOAD. 

*/
// Section 21 of ATmega2560 Datasheet

    // SPI uses PB0 - PB3 pins
    // must set data direction for the MOSI, SCK, and Chip select (SS) lines as part of init

void SPI_MASTER_Init() {

    // SPI Control Register
    // SPIE enables the interrupt for the SPI Module
    // SPE must be enabled to use the SPI module
    // DORD determines whether MSB (if writing/ microcontroller is master) or LSB (microcontroller is slave) is sent first
    // MSTR determines whether SPI module is in slave or master mode
    // CPOL - Clock polarity

    // set data direction
    // set the SS, MOSI, and SCLK pin as output
    DDRB |= (1 << DDB0) | (1 << DDB1) | (1 << DDB2);

    // set MISO pin as input 
    DDRB &= ~(1 << DDB3);

    // set SS high at first (disabling it)
    PORTB |= (1 << PORTB0);

    // enable the interupt, SPI, mastermode, CPOL, CPHA, default clock, and fosc/128
    // ADXL345 CPOL = 1, CPHA = 1
    SPCR |= (1 << SPIE) | (1 << SPE) | (1 << MSTR) | (1 << CPOL) | (1 << CPHA) | (1 << SPR1) | (1 << SPR0);

    // need spie here?? ^


}

void write_execute(unsigned char CMD, unsigned char data) {
    // SS low to begin SPI frame (enable chip select bit)
    PORTB &= ~(1 << PORTB0);

    // load CMD address into register
    SPDR = CMD;
    wait_for_completion; // wait for flag

    SPDR = data;    // load data into register
    wait_for_completion;    // wait for flag

    // set SS high to end SPI frame (disable chip select)
    PORTB |= (1 << PORTB0);

}