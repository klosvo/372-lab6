// Author: 
// Net ID: 
// Date: 
// Assignment: Lab 6
//----------------------------------------------------------------------//

#include "timer.h"
//You many use any timer you wish for the microsecond delay and the millisecond delay


/* Initialize timer 1, you should not turn the timer on here. Use CTC mode  .*/
void initTimer1(){
	// Set timer1 to CTC Mode
    TCCR1A &= ~(1<<WGM10);
    TCCR1A &= ~(1<<WGM11);
    TCCR1B |= (1<<WGM12);
    TCCR1B &= ~(1<<WGM13);
    // OCR1A value per ms delay is 250, based on no prescalar value of 64
    

    // Set prescalar value to 64 and OCR1A to 250
    TCCR1B &= ~(1<<CS10);
    TCCR1B |= (1<<CS11);
    TCCR1B |= (1<<CS12);
    OCR1A = 250;
}

/* This delays the program an amount of microseconds specified by unsigned int delay.
*/
void delayMs(unsigned int delay){
    // Set flag down
    TIFR1 |= (1<<OCF1A);
    // Reset counter to 0
    TCNT1 = 0;

    unsigned int count = 0;
    while (count< delay) {
        // If flag is down, increment count and restart timer
        if (TIFR1 & (1<<OCF1A)) {
            count++;
            TIFR1 |= (1<<OCF1A);
        }
    }
}
