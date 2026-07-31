#include "timer1.h"
#include <avr/io.h>

void TIMER1_init(void){
	// Configura PB1 (OC1A) y PB2 (OC1B) como salidas PWM
	DDRB |= (1 << DDB1) | (1 << DDB2);
	// Modo Fast PWM de 8 bits
	// OC1A y OC1B en modo INVERSOR (ánodo común: mayor duty = mayor tiempo en bajo = más encendido)
	TCCR1A = (1 << COM1A1) | (1 << COM1A0) | (1 << COM1B1) | (1 << COM1B0) | (1 << WGM10);
	TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10);     // WGM12 = 1 -> Fast PWM 8 bits; Prescaler = 64
	
	// Inicializa ambos canales con duty cycle 0% (apagado)
	OCR1A = 0;
	OCR1B = 0;
}

void TIMER1_set_duty_blue(uint8_t duty){
	// Actualiza el duty cycle del canal Azul (PB1 - OC1A)
	OCR1A = duty;
}

void TIMER1_set_duty_green(uint8_t duty){
	// Actualiza el duty cycle del canal Verde (PB2 - OC1B)
	OCR1B = duty;
}