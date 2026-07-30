#ifndef TIMER0_H_
#define TIMER0_H_

#include <stdint.h>

void TIMER0_init(void);	

// Devuelve el contador de 100us del sistema	
uint32_t TIMER0_get_ticks_100us(void);

// Actualiza el duty del PWM software (canal rojo)
void TIMER0_set_duty_red(uint8_t duty);

#endif /* TIMER0_H_ */