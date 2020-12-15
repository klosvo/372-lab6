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

#define DDR_SPI DDRB // Data Direction Register on ATMEGA2560 for SPI is DDRB
#define DD_SS DDB0  // SS Chip Select data direction bit B0 of ATMEGA2560 is DDB0
#define DD_SCK DDB1  // Clock pin connection data direction bit B1 on ATMEGA2560 is DDB1
#define DD_MOSI DDB2 // MOSI pin datadirection on ATMEGA2560 is DDB0
#define SPI_PORT PORTB // PortB for SPI on ATMEGA2560 is PORTB
#define SPI_SS_BIT PORTB0 // Port B register Bit B0 of Chip Select on ATMEGA2560 is PORTB0


#define wait_for_completion while(!(SPSR & (1 << SPIF))); // use flag register to determine whether all data is completed correctly


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


    // set data direction (DDRB = direction register)
    // set the SS, MOSI, and SCLK pin as output
    // DDB0 = SS, DDB1 = SCLK, DDB2 = MOSI
    DDR_SPI = (1 << DD_MOSI) | (1 << DD_SCK) | (1 << DD_SS);

    // we're only writing so we do not use MISO line
    // set MISO pin as input 
    // DDRB &= ~(1 << DDB3);

    // set SS high at first (disabling it)
    // PORTB0 = PORT B register bit of chip select
    SPI_PORT |= (1 << SPI_SS_BIT);

    // enable the interupt, SPI, mastermode, CPOL, CPHA, default clock, and fosc/128
    // datasheet says sample on rising edge so ADXL345 CPOL = 1, CPHA = 1
    // SPCR |= (1 << SPIE) | (1 << SPE) | (1 << MSTR) | (1 << CPOL) | (1 << CPHA) | (1 << SPR1) | (1 << SPR0);
    SPCR |= (1 << SPE) | (1 << MSTR) | (1 << CPOL) | (1 << CPHA) | (1 << SPR1) | (1 << SPR0);

    // SPIE enables the interrupt for the SPI Module
    // SPE must be enabled to use the SPI module
    // DORD determines whether MSB (if writing/ microcontroller is master) or LSB (microcontroller is slave) is sent first
    // MSTR determines whether SPI module is in slave or master mode
    // CPOL - Clock polarity
}

void write_execute(unsigned char CMD, unsigned char data) {
    // SS low to begin SPI frame (enable chip select bit)
    SPI_PORT &= ~(1 << SPI_SS_BIT);
    // load CMD address into register
    SPDR = CMD;
    wait_for_completion; // wait for flag

    SPDR = data;    // load data into register
    wait_for_completion;    // wait for flag

    // set SS high to end SPI frame (disable chip select)
    SPI_PORT |= (1 << SPI_SS_BIT);
}