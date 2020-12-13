#include "i2c.h"
#include "spi.h"
#include <avr/io.h>
#include "Arduino.h"


// addresses in hexidecimal
#define SLA 0x68
#define PWR_MGMT 0x6B
#define WAKEUP 0x00
#define SL_X_HI 0x3B
#define SL_X_LO 0x3C
#define SL_Y_HI 0x3D
#define SL_Y_LO 0x3E
#define SL_Z_HI 0x3F
#define SL_Z_LO 0x40

int main() {

    signed int x, y, z;
    x = 0;
    y = 0;
    z = 0;

    Serial.begin(9600); // using serial port to print values from I2C bus
    sei(); 
    initI2C();  // initialize I2C and set bit rate
    initSPI();

    StartI2C_Trans(SLA); //need slave address here

    write(PWR_MGMT); // sla for pwr mgmt
    write(WAKEUP); // wakeup device from sleep mode

    StopI2C_Trans();

        
    while (1) {
        _delay_ms(1000);

        Read_from(SLA, SL_X_HI);
        x = Read_data();

        Read_from(SLA, SL_X_LO);
        x = (x << 8) | Read_data();

        Serial.print("X = ");
        Serial.println(x);

        Read_from(SLA, SL_Y_HI);
        y = Read_data();

        Read_from(SLA, SL_Y_LO);
        y = (y << 8) | Read_data();

        Serial.print("Y = ");
        Serial.println(y);

        Read_from(SLA, SL_Z_HI);
        z = Read_data();

        Read_from(SLA, SL_Z_LO);
        z = (z << 8) | Read_data();

        Serial.print("Z = ");
        Serial.println(z);

        StopI2C_Trans();
    }

    return 0;

}