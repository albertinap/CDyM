#include "timer.h"

#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint16_t tick_100ms=0;

void TIMER1_init(void){
	//Modo CTC
	TCCR1A = 0;

	TCCR1B = (1<<WGM12);

	//Periodo 100 ms
	OCR1A = 6249;

	//Prescaler 256
	TCCR1B |= (1<<CS12);

	//Habilitar interrupción Compare Match A
	TIMSK1 |= (1<<OCIE1A);
}

//como tick_100ms es un registro de 16 bits, tenemos que leerlo y modificarlo de a 2 pasos (registro high y low)
//para asegurar que la lectura sea consistente y no se realice a la mitad de una modificación, 
//se deshabilitan las interrupciones temporalmente
uint16_t TIMER_get_ticks(void){
	uint16_t ticks;
	cli();
	ticks = tick_100ms;
	sei();
	return ticks;
}

ISR(TIMER1_COMPA_vect){
	tick_100ms++;
}