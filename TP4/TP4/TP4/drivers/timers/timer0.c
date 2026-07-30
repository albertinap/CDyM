/*
 * timer0.c
 *
 * Created: 29/7/2026 19:27:55
 *  Author: alber
 */ 

#include "timer0.h"

#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint32_t timer0_ticks = 0;

// Duty cycle del canal Rojo (PWM por software)
volatile uint8_t pwm_red_duty = 0;

// Contador del PWM por software
static volatile uint8_t pwm_counter = 0;

void TIMER0_init(void){
	// Configura PB5 como salida (canal Rojo)
	DDRB |= (1 << DDB5);

	// Estado inicial: LED apagado
	PORTB |= (1 << PORTB5);

	// Modo CTC
	TCCR0A = (1 << WGM01);

	// Prescaler = 64
	TCCR0B = (1 << CS01) | (1 << CS00);

	// Interrupción cada 64 us
	// F = 16 MHz
	// Tick timer = 4 us
	// OCR0A = 15 -> (15+1)*4 us = 64 us
	OCR0A = 15;

	// Habilita interrupción por comparación A
	TIMSK0 |= (1 << OCIE0A);
}

ISR(TIMER0_COMPA_vect){
	// Base de tiempo del sistema
	timer0_ticks++;

	// Incrementa el contador del PWM software
	pwm_counter++;

	// PWM software sobre PB5
	if (pwm_counter < pwm_red_duty){
		// Ánodo común:
		// Nivel bajo -> LED encendido
		PORTB &= ~(1 << PORTB5);
	}
	else{
		// Nivel alto -> LED apagado
		PORTB |= (1 << PORTB5);
	}
}

uint32_t TIMER0_get_ticks(void){
	// Devuelve la cantidad de ticks transcurridos
	return timer0_ticks;
}