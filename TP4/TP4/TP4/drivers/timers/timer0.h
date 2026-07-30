#ifndef TIMER0_H_
#define TIMER0_H_

#include <stdint.h>

void TIMER0_init(void);

uint32_t TIMER0_get_ticks(void);		//devuelve la cantidad de ticks actuales

extern volatile uint8_t pwm_red_duty;

#endif /* TIMER0_H_ */