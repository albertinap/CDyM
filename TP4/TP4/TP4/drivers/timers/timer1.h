/*
 * timer1.h
 * Interfaz del driver Timer1
 * Author : Ignacio Mucci Bigliani y Albertina Pezzutti
 */
#ifndef TIMER1_H_
#define TIMER1_H_

#include <stdint.h>

void TIMER1_init(void);

void TIMER1_set_duty_blue(uint8_t duty);	//para modificar la intensidad del azul
void TIMER1_set_duty_green(uint8_t duty);	//para modificar la intensidad del verde

#endif /* TIMER1_H_ */
