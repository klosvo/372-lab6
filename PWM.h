#ifndef PWM_H
#define PWM_H

#include <avr/io.h>

    void initPWM();
    void IncFrequency( unsigned int frequency);
    void BuzzerOn(void);
    void BuzzerOff(void);

#endif