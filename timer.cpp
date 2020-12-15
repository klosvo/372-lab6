// timer.cpp file for timer functions
// ECE 372A  10-12-20

#include <avr/io.h>
#include "timer.h"


// initialize mode of operation for Timer1 using CTC mode
// WGM bits are configured such that WGM10 = 0, WGM11 = 0, WGM12 = 1, WGM13
void initTIMER1() {
    TCCR1A &= ~( (1 << WGM10) | ( 1<< WGM11));
    TCCR1B |= ( 1 << WGM12);
    TCCR1B &= ~ ( 1 << WGM13); 
}


// function delayMs (delay)
// function takes an int value called delay to delay the total time in milliseconds
// this function is limited to delay = 1000 as the upper limit
void delayMs (int delay) {

    TCNT1 = 0;

    // set outout compare value
    // we used the formula OCR1A = [Td * fclk]/ PS
    // using a PS = 256, Td = 1ms fclk = 16MHz.
    // OCR1A = 62
    // if we want to pass a value called delay then we can set OCR1A = 1 * delay
    // and that should allow values in ms to be delayed by the right time up to 1000ms.

    OCR1A = 62 * delay;

    // set output compare flag down by writing a logic 1
    TIFR1 |= (1 << OCF1A);

    // turn on clock with the CS bits and start counting
    // Use Prescaler of 256 (62 counts is approximately 1 ms)
    TCCR1B |= (1 << CS12);
    TCCR1B &= ~((1 << CS11)| (1 << CS10));

    // poll the flag OCF1A bit to see when it is raised
    // while the flag bit OCF1A is down , do nothing
    while (( TIFR1 & ( 1 << OCF1A)) == 0) ;

    // turn off clock
    TCCR1B &= ~( ( 1 << CS12) | ( 1 << CS11) | (1 << CS10));
    return;
}