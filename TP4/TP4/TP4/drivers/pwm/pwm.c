/*
 * pwm.c
 *
 * Created: 29/7/2026 19:32:09
 *  Author: alber
 */ 
#include <avr/io.h>

#include "pwm.h"
#include "../timers/timer0.h"
#include "../timers/timer1.h"

static uint8_t duty_red = 0;

static uint8_t pwm_counter = 0;

// Inicializa todos los canales PWM
void PWM_init(void){
	
	DDRB |= (1<<DDB5);
	
	// Inicializa el Timer1 encargado de los canales
	// Verde (OC1B) y Azul (OC1A)
	TIMER1_init();

	// Inicializa el Timer0 encargado del
	// PWM por software del canal Rojo
	TIMER0_init();

	// Todos los canales comienzan apagados
	PWM_set_red(0);
	PWM_set_green(0);
	PWM_set_blue(0);
}

// Actualiza el duty cycle del canal Rojo
// Implementado mediante PWM por software
void PWM_set_red(uint8_t duty){
	duty_red = duty;
}

// Actualiza el duty cycle del canal Verde
// Implementado mediante el canal OC1B
void PWM_set_green(uint8_t duty){
	TIMER1_set_duty_green(duty);
}

// Actualiza el duty cycle del canal Azul
// Implementado mediante el canal OC1A
void PWM_set_blue(uint8_t duty){
	TIMER1_set_duty_blue(duty);
}

void PWM_task(void){
	static uint32_t ultimo_tick_pwm = 0;
	
	uint32_t ahora = TIMER0_get_ticks_100us();

	// Espera al siguiente tick de 100 us
	if(ahora == ultimo_tick_pwm)
		return;

	ultimo_tick_pwm = ahora;

	// Incrementa el contador del PWM
	pwm_counter++;

	// PWM por software sobre PB5
	if(pwm_counter < duty_red){
		// Ánodo común:
		// nivel bajo -> LED encendido
		PORTB &= ~(1 << PORTB5);
	}
	else{
		// Nivel alto -> LED apagado
		PORTB |= (1 << PORTB5);
	}
}