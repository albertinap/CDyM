/*
 * timer0.h
 * Interfaz del driver Timer0.
 * Genera la base de tiempos del sistema (tick de 100 us)
 * y controla el canal rojo mediante PWM por software.
 */

#ifndef TIMER0_H_
#define TIMER0_H_

#include <stdint.h>

// Inicializa el Timer0 en modo CTC
void TIMER0_init(void);

// Devuelve la cantidad de ticks de 100 us transcurridos desde el arranque
uint32_t TIMER0_get_ticks_100us(void);

#endif /* TIMER0_H_ */