/*
 * adc.c
 *
 * Created: 29/7/2026 18:52:08
 *  Author: rocio
 */ 
#include "adc.h"
#include <avr/io.h>
#include "../timers/timer0.h"

void ADC_Init(void)
{ 
    DDRC &= ~(1 << DDC3);		// Configuro PC3 como entrada para el ADC
    PORTC &= ~(1 << PORTC3);	// Deshabilito el pull-up interno
    ADMUX |= (1 << REFS0) | (1 << MUX1) | (1 << MUX0); // Pongo la tension de referencia en 5v y ademas  selecciono el canal 3 el cual es el pin 3 del portc
    ADCSRA |= (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1)| (1 << ADPS0); //  Con Prescaler = 128, entonces queda ADCLK = 16MHz / 128 = 125kHz valor que esta dentro de las medidas posibles
}

uint16_t ADC_Read(void)
{
    ADCSRA |= (1 << ADSC); //Inicio la conversion
    while (!(ADCSRA & (1 << ADIF)));
    ADCSRA |= (1 << ADIF);   // Borra el flag
    return ADC;
}
