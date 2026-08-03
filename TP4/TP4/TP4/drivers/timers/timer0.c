/*
 * timer0.c
 *
 * Driver del Timer0.
 * Genera un tick periódico de 100 us utilizado como base de tiempos del sistema.
 *
 * Además almacena el duty cycle correspondiente al canal rojo implementado mediante PWM por software.
 */

#include "timer0.h"
#include <avr/io.h>
#include <avr/interrupt.h>

// Contador global de ticks de 100 us
volatile uint32_t ticks_100us = 0;

void TIMER0_init(void){

	// Configura el Timer0 en modo CTC
	TCCR0A = (1 << WGM01);

	// Selecciona un prescaler de 64
	TCCR0B = (1 << CS01) | (1 << CS00);

	// Cálculo del período de interrupción:
	// fclk = 16 MHz
	// fTimer = 16 MHz / 64 = 250 kHz
	// Ttick = 4 us
	// OCR0A = 24 -> 25 cuentas -> 100 us
	
	/*Cálculo del período de interrupción:
	  fCPU = 16 MHz;  Prescaler = 64; fTimer = 16 MHz / 64 = 250 kHz;  Ttick = 4 us
	  OCR0A = 24 -> 25 cuentas -> 100 us*/
	OCR0A = 24;
	/*El PWM por software utiliza un contador de 8 bits (256 niveles),
	  por lo que su período es: 256 x 100 us = 25,6 ms
	  equivalente a una frecuencia de: fPWM ? 39 Hz
	  Cumple con el requisito del proyecto de generar una señal PWM con frecuencia superior a 30 Hz*/

	// Habilita la interrupción por comparación A
	TIMSK0 |= (1 << OCIE0A);
}

// Se ejecuta cada 100 us
ISR(TIMER0_COMPA_vect){
	ticks_100us++;
}

uint32_t TIMER0_get_ticks_100us(void){
	return ticks_100us;
}