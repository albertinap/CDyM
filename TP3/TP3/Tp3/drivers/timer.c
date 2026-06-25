/*
 * timer.c
 * Driver Timer1 en modo CTC.
 * Configuración:
 *   - Fcpu = 16MHz, prescaler = 256 ? Ftimer = 62.5kHz
 *   - OCR1A = 6249 ? interrupción cada 6250 ciclos = 100ms
 * tick_100ms se incrementa en cada ISR y es la base de tiempo del sistema para temporizar tareas de background
 * Author : Ignacio Mucci Bigliani y Albertina Pezzutti
 */
#include "timer.h"
#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint16_t tick_100ms=0;

void TIMER1_init(void){
	TCCR1A = 0;				// modo CTC: WGM13:10 = 0100, TCCR1A=0
	TCCR1B = (1<<WGM12);	// WGM12=1 --> CTC con tope en OCR1A

	OCR1A = 6249;			// tope: 6250 ciclos a 62.5kHz = 100ms

	TCCR1B |= (1<<CS12);	// prescaler 256 (CS12=1, CS11=0, CS10=0)

	TIMSK1 |= (1<<OCIE1A);	// habilitar interrupción por Compare Match A
}

//como tick_100ms es un registro de 16 bits, el avr lo lee y modifica en 2 pasos (registro high y low)
//para asegurar que la lectura sea consistente y no se realice a la mitad de una modificación, 
//se deshabilitan las interrupciones temporalmente
uint16_t TIMER_get_ticks(void){
	uint16_t ticks;
	cli();
	ticks = tick_100ms;
	sei();
	return ticks;
}

// Se dispara cada 100ms. Incrementa el contador global de ticks
ISR(TIMER1_COMPA_vect){
	tick_100ms++;
}