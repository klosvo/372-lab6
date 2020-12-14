// Description: This file implements the initialization of an external
// switch.
//----------------------------------------------------------------------//

#include "switch.h"
#include <avr/io.h>



/*
 * Initializes pull-up resistor on PB5 and sets it into input mode
 */
void initSwitchPB5(){
    //set the data direction register to input
    // pin 11/24 (bit location PB5)

    // Initializes pull-up resistor on PB5 and sets it into input mode
    DDRB &= ~(1<<DDB5); // set direction for input
    PORTB |= (1<< PORTB5); // enable the pullup resistor for stable input

    // enable the group pin change interrupts PCINTs 0 through 7
    PCICR |= (1 << PCIE0); // ENABLE PCINT FOR PCINT5 (PCINT 0-7)
    // enable the local PCINT5 corresponding to development board header pin 11
    PCMSK0 |= (1 << PCINT5); // set up local pin change interrupt PCINT register (PMSK0)
}



//Setup pin D0 as input 
//Set pull up resistance 
//Setup external interupt with any edge change 

// void initSwitchPD0(){
//     DDRD &= ~(1<< DDD0);// turns port 21 into an input
//     PORTD |= (1<<PORTD0);// enables the pull up resistor

//     /////////////////////////////////////////////////////////////
//     // this code turns pin 21 into an external interupt.
//     EICRA |= (1<<ISC00);
//     EICRA &= ~(1<<ISC01);
//     EIMSK |= (1 << INT0);        
// }