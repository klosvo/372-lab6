#ifndef PWM_H
#define PWM_H

#include <avr/io.h>

void initPWM();

int changeDutyCycle(int dutycycle);
void SetMOTORspeed(double result);
void IncFrequency( unsigned int frequency);


#endif