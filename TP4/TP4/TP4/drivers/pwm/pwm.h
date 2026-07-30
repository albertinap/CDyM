/*
 * pwm.h
 *
 * Created: 29/7/2026 19:32:00
 *  Author: alber
 */ 


#ifndef PWM_H_
#define PWM_H_

#include <stdint.h>

void PWM_init(void);

void PWM_set_red(uint8_t duty);
void PWM_set_green(uint8_t duty);
void PWM_set_blue(uint8_t duty);

#endif /* PWM_H_ */