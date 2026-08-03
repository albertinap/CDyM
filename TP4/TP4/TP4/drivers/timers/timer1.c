/* timer1.c
 * Driver del Timer1.
 * Genera dos señales PWM por hardware para los canales
 * verde y azul del LED RGB.
 */
#include "timer1.h"
#include <avr/io.h>

void TIMER1_init(void){

	// Configura los pines OC1A (PB1) y OC1B (PB2) como salidas
	DDRB |= (1 << DDB1) | (1 << DDB2);

	/*Completa el modo Fast PWM de 8 bits y selecciona un prescaler de 64 (Salidas OC1A y OC1B en modo inversor)
	  La frecuencia del PWM resulta:	  fPWM = fCPU / (Prescaler · 256) = 16 MHz / (64 · 256) ? 976 Hz
	  Esta frecuencia supera ampliamente el mínimo de 30 Hz requerido por el enunciado y evita el parpadeo perceptible */
	
	TCCR1A = (1 << COM1A1) | (1 << COM1A0)	| (1 << COM1B1) | (1 << COM1B0)	| (1 << WGM10);

	// Completa el modo Fast PWM de 8 bits
	// y selecciona un prescaler de 64.
	TCCR1B = (1 << WGM12)	| (1 << CS11)	| (1 << CS10);

	// Inicializa ambos canales apagados
	OCR1A = 0;
	OCR1B = 0;
}

void TIMER1_set_duty_blue(uint8_t duty){
	// Actualiza el duty cycle del canal OC1A (LED azul)
	OCR1A = duty;
}

void TIMER1_set_duty_green(uint8_t duty){
	// Actualiza el duty cycle del canal OC1B (LED verde)
	OCR1B = duty;
}