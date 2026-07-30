/*
 * timer0.c
 *
 * Created: 29/7/2026 19:27:55
 *  Author: alber
 */ 

#include "timer0.h"
#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint32_t ticks_100us = 0;

// Duty cycle del canal Rojo (PWM por software)
static volatile uint8_t duty_red = 0;



void TIMER0_init(void){
	// Modo CTC
	TCCR0A = (1 << WGM01);

	// Prescaler = 64
	TCCR0B = (1 << CS01) | (1 << CS00);

	// 16 MHz / 64 = 250 kHz	frecuencia de nuestro clock
	// Tick del timer = 4 us	periodo
	// 25 ticks -> 100 us		periodo de interrupción
	OCR0A = 24;

	// Habilita interrupción por comparación A
	TIMSK0 |= (1 << OCIE0A);
}

ISR(TIMER0_COMPA_vect){
	ticks_100us++;
}

void TIMER0_set_duty_red(uint8_t duty){
	duty_red = duty;
}

uint32_t TIMER0_get_ticks_100us(void){
	return ticks_100us;
}
