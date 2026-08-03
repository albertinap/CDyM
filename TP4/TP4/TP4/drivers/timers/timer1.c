#include "timer1.h"
#include <avr/io.h>

void TIMER1_init(void){

	// Configura los pines OC1A (PB1) y OC1B (PB2) como salidas
	DDRB |= (1 << DDB1) | (1 << DDB2);

	// Configuración del Timer1:
	// - Fast PWM de 8 bits.
	// - Salidas OC1A y OC1B en modo inversor
	//   (LED RGB de ánodo común).
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