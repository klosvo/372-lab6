// timer.cpp file for timer functions
// ECE 372A  10-12-20

#include <avr/io.h>
#include "timer.h"

// Set up Timer0 for a micro second delay with CTC mode of operation 

// WGM bits need to be specified
// for CTC mode WGM00 = 0, WGM01 = 1, WGM02 = 0
// void initTIMER0(){
//     TCCR0A &= ~( 1 << WGM00);
//     TCCR0A |=  ( 1 << WGM01);
//     TCCR0B &= ~( 1 << WGM02);
// }

// initialize mode of operation for Timer1 using CTC mode
// WGM bits are configured such that WGM10 = 0, WGM11 = 0, WGM12 = 1, WGM13
void initTIMER1() {
    TCCR1A &= ~( (1 << WGM10) | ( 1<< WGM11));
    TCCR1B |= ( 1 << WGM12);
    TCCR1B &= ~ ( 1 << WGM13); 
}

// function delayuS0 (value)
// this is for a microsecond based delay using CTC mode
// can create a delay up to 250 us
// void delayuS0(int value){

//     // set TCNT0 = 0
//     TCNT0 = 0;

//     // set outout compare value
//     // we used the formula OCR0A = [Td * fclk]/ PS
//     // using a PS = 8, Td = 1us fclk = 16MHz.
//     // OCR1A = 2  - 1 to acccount for starting at bit 0.
//     // if we want to pass a value called delay then we can set OCR0A = 1 * delay
//     // andthat should allow values in us to be delayed by the right time up to at least 250us.
//     OCR0A = 1 * value;

//     // set output compare flag  TIFR0 down by writing a logic 1
//     TIFR0 |= (1 << OCF0A);

//     // turn on clock with the CS bits and start counting
//     // Use Prescaler of 8 (2 counts is  1 us)
//     TCCR0B |= (1 << CS01);
//     TCCR0B &= ~((1 << CS02)| (1 << CS00));

//     // poll the flag OCF0A bit to see when it is raised
//     // while the flag bit OCF0A is down, do nothing
//     while (( TIFR0 & ( 1 << OCF0A)) == 0) ;

//     // turn off clock
//     TCCR0B &= ~( ( 1 << CS02) | ( 1 << CS01) | (1 << CS00));
//     return;
// }



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