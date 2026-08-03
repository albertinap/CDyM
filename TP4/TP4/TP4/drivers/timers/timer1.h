/*
 * timer1.h
 * Interfaz del driver Timer1.
 * Utiliza los canales PWM por hardware OC1A y OC1B para controlar los LEDs azul y verde.
 *
 * Author: Ignacio Mucci Bigliani y Albertina Pezzutti
 */

#ifndef TIMER1_H_
#define TIMER1_H_

#include <stdint.h>

// Inicializa el Timer1 en modo Fast PWM de 8 bits
void TIMER1_init(void);

// Actualiza el duty cycle del canal PWM asociado al LED azul (OC1A)
void TIMER1_set_duty_blue(uint8_t duty);

// Actualiza el duty cycle del canal PWM asociado al LED verde (OC1B)
void TIMER1_set_duty_green(uint8_t duty);

#endif /* TIMER1_H_ */