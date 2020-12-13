
#include "PWM.h"

void initPWM(){

    //-----FOR TIMER 4-----//

    // Pin 8 on board to output (OC4C, PORTH5, PWM)
    // use timer 4 ICR4 for the TOP value to set the frequency and OCR4A as the output compare to set the duty cycle
    // PWM signal generated onthe pin that corresponds to OCR4A/ Header Pin 6
    DDRH |= (1 << DDH3);

    // Set non-inverting mode - output starts high and then is low
    // COM4A0 = 0, COM4A1 = 1
    TCCR4A |= (1 << COM4A1);
    TCCR4A &= ~(1 << COM4A0);

    // PWM mode 14 bit, top value determined by ICR1 value
    // which determines the PWM freq
    // for mode 14: WGM40 - 0, WGM41 = WGM42 = WGM43 = 1 (table 17-2)
    TCCR4A &= ~(1 << WGM40);
    TCCR4A |= (1 << WGM41);
    TCCR4B |= (1 << WGM42) | (1 << WGM43);

}


// generate variable frequency
void IncFrequency( unsigned int frequency){
    
    ICR4 = 16000000/(frequency) - 1;

    TCCR4B |= (1 << CS40);
    TCCR4B &= ~((1 << CS41) | (1 << CS42));

    OCR4A = 0.8 * frequency;



    // OCR4A = 16000000 / frequency;
    // OCR4AH = OCR4A >> 8;
    // OCR4AL = OCR4A;
    // OCR4CH = OCR4AH >> 1;
    // OCR4CL = OCR4AL >> 1;
}

//Set motor speed and direction with count registers using information obtained by ADC 
void SetMOTORspeed(double result) {
    // the last thing is to set the duty cycle.     
    // duty cycle is set by dividing output compare OCR4A value by 1 + TOP value
    // the top value is (1 + 0x3FF) = 1024
    // calculate OCR4A value => OCR4A = duty cycle(fractional number) * (1 + TOP) 
    // we want a duty cycle varies based on voltage variable result
    // OCR1A = 0.60 * 1024


 //convert result into a voltage for this to not change


    if(result < 2.3){
        // set duty cycle
        // Tpulse/Tperiod = OCRnx/TOP
        // duty cycle varies based on voltage variable result
        // 0x3FF/4

        OCR3A = result * 0x400;
        OCR4A = 0;

    }else if (result > 2.7){
        // set duty cycle
        // Tpulse/Tperiod = OCRnx/TOP
        // duty cycle of 25%
        // 0x3FF/4
        OCR4A = result * 0x400;
        OCR3A = 0;
    } else{
         OCR3A = 0;
         OCR4A = 0;
    }

}
