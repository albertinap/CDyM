/*
 * timer.h
 * Interfaz del driver Timer1 --> base de tiempo de 100ms, interrupción periódica por Compare Match
 * Author : Ignacio Mucci Bigliani y Albertina Pezzutti
 */
#ifndef TIMER_H_
#define TIMER_H_

#include <stdint.h>

void TIMER1_init(void);			// configura Timer1 en modo CTC con tick cada 100ms

uint16_t TIMER_get_ticks(void);	// devuelve el contador de ticks de forma atómica

#endif