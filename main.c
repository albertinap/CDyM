/*
 * TP1
 * MCU: ATmega328P - 16MHz
 *
 */

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdbool.h>

// CONSTANTES GENERALES
#define CANT_LEDS      8
#define DELAY_LEDS_MS  100   // retardo secuencias LED en ms  
#define DELAY_LOOP_MS  10    // tiempo de delay que agregamos al super loop   

// CONTADOR DE MILISEGUNDOS (para 100ms y 150ms)
uint8_t cont_100 = 0;
uint8_t cont_150 = 0;

// VARIABLES DE ESTADO DE SECUENCIAS
/* LEDs Puerto D */
uint8_t  pos_leds	  = 0;
int8_t   dir_leds	  = -1;
uint8_t  secuencia_leds = 0;
uint8_t  secuencia_neo = 0;

void leer_botones(void) {
	static uint8_t btn0_presionado = 0;
	static uint8_t btn1_presionado = 0;

	if (!(PINC & (1 << PORTC0))) {
		if (!btn0_presionado) {
			secuencia_leds ^= 1;
			btn0_presionado = 1;
		}
		} else {
		btn0_presionado = 0;
	}

	if (!(PINC & (1 << PORTC1))) {
		if (!btn1_presionado) {
			secuencia_neo ^= 1;
			btn1_presionado = 1;
		}
	} else {
		btn1_presionado = 0;
	}
}

// SECUENCIA A
void secuencia_a(void)
{
    if (cont_100 < DELAY_LEDS_MS) return;
    cont_100=0;

    PORTD = (1 << pos_leds);

    pos_leds++;
    if (pos_leds >= CANT_LEDS) pos_leds = 0;
}

// SECUENCIA B
void secuencia_b(void)
{
    if (cont_100 < DELAY_LEDS_MS) return;
    cont_100 = 0;

    PORTD = (1 << pos_leds);
    pos_leds += dir_leds;

    if (pos_leds >= CANT_LEDS - 1) dir_leds = -1;
    if (pos_leds == 0)             dir_leds =  1;
}

// PROGRAMA PRINCIPAL
int main(void)
{
    // CONFIGURACION DE PINES
    DDRD  = 0xFF;   // Puerto D: todos salidas (LEDs)  
    PORTD = 0x00;   // LEDs apagados al inicio         

    DDRC &= ~(1 << PORTC0);   // PC0 entrada (boton 1)
	PORTC |= (1 << PORTC0);  // PC0 entrada (boton 1)
	
	secuencia_leds = 0;

    // SUPER LOOP 
    while (1)
    {                		
		leer_botones();

		if (secuencia_leds == 0) 
			secuencia_a();
		else					 
			secuencia_b();
		
		_delay_ms(DELAY_LOOP_MS);
		cont_100 += DELAY_LOOP_MS;		
    }

    return 0;
}