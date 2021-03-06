#include "i2c.h"
#include "spi.h"
#include "timer.h"
#include "switch.h"
#include "PWM.h"
#include <avr/io.h>
#include "Arduino.h"

//#define PI 3.14159265

// define statements for i2c/accelerometer
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

// SPI Register Address Map
#define DECODE_MODE 0x09
#define INTENSITY 0x0A // brightness control
#define SCAN_LIMIT 0x0B
#define SHUTDOWN 0x0C
#define DISPLAY_TEST 0x0F

#define ROW_1 0x01
#define ROW_2 0x02
#define ROW_3 0x03
#define ROW_4 0x04
#define ROW_5 0x05
#define ROW_6 0x06
#define ROW_7 0x07
#define ROW_8 0x08

// Create statemachine for button debouncing
// When system is on motor will rotate based on potentiometer location 
// Alarm turns off when button is pushed

// Four states for switch used in the state machine for debounce
typedef enum stateType_enum{
    wait_press, debounce_press, wait_release, debounce_release
} stateType;

// define global variables
// define state variable as volatile (to be able to use in ISR and in main)
// prevents compiler from doing any optimization with this variable
volatile stateType current_state = wait_press;  // sets to initila mode wait_press


// Four states for switch used in the state machine for angle threshold
typedef enum accState_enum{
    above_threshold, below_threshold
} accState;

// define global variables
// define state variable as volatile (to be able to use in ISR and in main)
// prevents compiler from doing any optimization with this variable
volatile accState acc_state = below_threshold;  // sets to initila mode wait_press

int main() {

    Serial.begin(9600); // using serial port to print values from I2C bus
    sei();

    // define local variables used for position
    signed int x, y, z;
    x = 0;
    y = 0;
    z = 0;
    bool buzzer;
    buzzer = 0;
    
    initTIMER1();
    initPWM();
    // initSwitchPD0();
    initSwitchPB5();
    initI2C();  // initialize I2C and set bit rate
    
    

    StartI2C_Trans(SLA); //need slave address here
    
    write(PWR_MGMT); // sla for pwr mgmt
    write(WAKEUP); // wakeup device from sleep mode
    StopI2C_Trans();
    
    
    SPI_MASTER_Init(); 
    // initialize 8 x 8 LED array
    write_execute(INTENSITY, 0x08);     // brightness control
    write_execute(SCAN_LIMIT, 0x07);    // display everything/all rows // scan all rows and columns
    write_execute(SHUTDOWN, 0x01);      // set shutdown register to normal operation
    write_execute(DISPLAY_TEST, 0x00);  // display test register set to normal operation

        
    while (1) {
    // Serial.println("Got to here 0");
        Read_from(SLA, SL_X_HI);
        x = Read_data();

        Read_from(SLA, SL_X_LO);
        x = (x << 8) | Read_data();

        //  Serial.print("X = ");
        //  Serial.println(x);

        Read_from(SLA, SL_Y_HI);
        y = Read_data();

        Read_from(SLA, SL_Y_LO);
        y = (y << 8) | Read_data();

            //  Serial.print("Y = ");
            //  Serial.println(y);
         

        Read_from(SLA, SL_Z_HI);
        z = Read_data();

        Read_from(SLA, SL_Z_LO);
        z = (z << 8) | Read_data();


            // Serial.print("Z = ");
            //  Serial.println(z);
         

        StopI2C_Trans();

        if( ((y > 500) /* && (y < 10000)*/) | (z > 10000)){    
            acc_state = above_threshold;
        }
        else{
            acc_state = below_threshold;
        }



        // Implement a state machine in the while loop which achieves the assignment
        switch(acc_state){
            case below_threshold:
            // Serial.println(":)");
                // LED SMILEY :)
                write_execute(ROW_1, 0b00111100);   // write row 1
                write_execute(ROW_2, 0b01000010);    // write row 2
                write_execute(ROW_3, 0b10100101);   // write row 3
                write_execute(ROW_4, 0b10000001);   // write row 4
                write_execute(ROW_5, 0b10100101);   // write row 5
                write_execute(ROW_6, 0b10011001);   // write row 6
                write_execute(ROW_7, 0b01000010);   // write row 7
                write_execute(ROW_8, 0b00111100);   // write row 8
            break;
            case above_threshold:
            // Serial.println(":(");
                buzzer = 1;
                BuzzerOn();
                
                // LED FROWN :(
                write_execute(ROW_1, 0b00111100);   // write row 1
                write_execute(ROW_2, 0b01000010);    // write row 2
                write_execute(ROW_3, 0b10100101);   // write row 3
                write_execute(ROW_4, 0b10000001);   // write row 4
                write_execute(ROW_5, 0b10011001);   // write row 5
                write_execute(ROW_6, 0b10100101);   // write row 6
                write_execute(ROW_7, 0b01000010);   // write row 7
                write_execute(ROW_8, 0b00111100);   // write row 8
            break;
        }

        if(buzzer) BuzzerOn();

        // Implement a state machine in the while loop which achieves the assignment
        switch(current_state){
            case wait_press:
                break;
            case debounce_press: 
                buzzer = 0;
                BuzzerOff();
                current_state = wait_release;
                break;
            case wait_release: 
                break;
            case debounce_release:
                current_state = wait_press;
                break;
        }
    }

    return 0;

}


/* Implement a Pin Change Interrupt which handles the switch being
* pressed and released. When the switch is pressed and released, the LEDs
* change at twice the original rate. If the LEDs are already changing at twice
* the original rate, it goes back to the original rate.
*/

// ISR for Pin Change Interrupt
// From table 14.1 vector table: PCINT0 corresponds to pins 0-7 (used for PCINT5 on B5)
ISR(PCINT0_vect){
    
    if(current_state == wait_press){        // if the PCINT was triggered by press 
        current_state = debounce_press;     // (was in wait_press state) change state
    }                                       // to debounce_press 
     
    // if the PCINT was triggered for release (was in the wait_release state)
    else if (current_state == wait_release){
        BuzzerOff();
        current_state = debounce_release;   // change current_state to debounce_release
    }
}