/*
 * pwm.c
 *
 * Created: 29/7/2026 19:32:09
 *  Author: alber
 */ 

#include "pwm.h"
#include "timer0.h"
#include "timer1.h"

// Inicializa todos los canales PWM
void PWM_init(void)
{
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
void PWM_set_red(uint8_t duty)
{
	pwm_red_duty = duty;
}

// Actualiza el duty cycle del canal Verde
// Implementado mediante el canal OC1B
void PWM_set_green(uint8_t duty)
{
	TIMER1_set_duty_green(duty);
}

// Actualiza el duty cycle del canal Azul
// Implementado mediante el canal OC1A
void PWM_set_blue(uint8_t duty)
{
	TIMER1_set_duty_blue(duty);
}