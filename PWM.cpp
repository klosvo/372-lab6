
#include "PWM.h"
#include "timer.h"
#include <avr/io.h>
#include "Arduino.h"

void initPWM(){

    //-----FOR TIMER 4-----//

    // Pin 6 on board to output (OC4A, PORTH3, PWM)
    // use timer 4 ICR4 for the TOP value to set the frequency and OCR4A as the output compare to set the duty cycle
    // PWM signal generated on the pin that corresponds to OCR4A/ Header Pin 6
    DDRH |= (1 << DDH3);

    // Set non-inverting mode - output starts high and then is low
    // COM4A0 = 0, COM4A1 = 1
    TCCR4A &= ~(1 << COM4A0);
    TCCR4A |= (1 << COM4A1);
    

    // PWM mode 14 bit, top value determined by ICR4 value
    // which determines the PWM freq
    // for mode 14: WGM40 = 0, WGM41 = WGM42 = WGM43 = 1 (table 17-2)
    TCCR4A &= ~(1 << WGM40);
    TCCR4A |= (1 << WGM41);
    TCCR4B |= (1 << WGM42) | (1 << WGM43);
}


// generate variable frequency
void IncFrequency( unsigned int frequency){
    // Using Timer 4
    // PWM frequency Calculation for FAST PWM mode (page 148 datasheet)
    // frequency of PWM = (F_clk) / (Prescaler * (1 + TOP))
    // F_clk = 16 MHz
    // Prescaler = 1
    // TOP Value = ICRN = variable passed - 1
    ICR4 = 16000000/(frequency) - 1;


    // Prescaler Bits for Prescaler of 1 (table 17-6 datasheet)
    // CS40 = 1, CS41 = 0, CS42 = 0
    TCCR4B |= (1 << CS40);
    TCCR4B &= ~((1 << CS41) | (1 << CS42));


    // Set Duty Cycle
    // duty cycle = OCR4A/(1 + TOP)
    // TOP = 1 + ICR4
    // OCR4A = output compare value = OCR4A = duty cycle * (1 + TOP)
    // We use an 80% duty cycle
    OCR4A = 0.8 * frequency;
    //Serial.println(frequency);
}

void BuzzerOff(void){
    ICR4 = 0;
    OCR4A = 0;
}

void BuzzerOn(void){
    for(int i = 800; i < 5000; i++){
        IncFrequency(i);
    }
}


