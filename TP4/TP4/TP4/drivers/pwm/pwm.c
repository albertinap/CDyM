/*
 * pwm.c
 *
 * Implementación del driver PWM.
 *
 * - Canal rojo: PWM por software utilizando la base de tiempos
 *   generada por Timer0.
 * - Canales verde y azul: PWM por hardware mediante Timer1.
 *
 * Author: Ignacio Mucci Bigliani y Albertina Pezzutti
 */

#include <avr/io.h>
#include <stdint.h>
#include "pwm.h"
#include "../timers/timer0.h"
#include "../timers/timer1.h"


// Duty cycle del canal rojo (PWM por software)
static uint8_t duty_red = 0;

// Contador utilizado para generar el PWM por software
static uint8_t pwm_counter = 0;

void PWM_init(void){
	// Configura PB5 (LED rojo) como salida
	DDRB |= (1<<DDB5);

	// Inicializa el PWM por hardware (verde y azul)
	TIMER1_init();

	// Inicializa la base de tiempos del PWM por software
	TIMER0_init();

	// Todos los canales comienzan apagados
	PWM_set_red(0);
	PWM_set_green(0);
	PWM_set_blue(0);
}

// Actualiza el duty cycle del canal rojo.
// El valor será utilizado por PWM_task().
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


void PWM_force_green(uint8_t nivel){
	// Desconecta el control PWM del pin OC1B
	TCCR1A &= ~((1<<COM1B1) | (1<<COM1B0));

	// Mantiene el pin configurado como salida
	DDRB |= (1 << PORTB2);

	// Fuerza el nivel lógico solicitado
	if(nivel)
		PORTB |= (1 << PORTB2);
	else
		PORTB &= ~(1 << PORTB2);
}
void PWM_reconnect_green(void){
	TCCR1A |= (1<<COM1B1) | (1<<COM1B0);		// reconecta verde modo PWM no invertido
}

void PWM_force_blue(uint8_t nivel){
	// Desconecta el control PWM del pin OC1A
	TCCR1A &= ~((1<<COM1A1) | (1<<COM1A0));
	// Mantiene el pin configurado como salida
	DDRB |= (1 << PORTB1);
	// Fuerza el nivel lógico solicitado
	if(nivel) PORTB |= (1 << PORTB1);
	else      PORTB &= ~(1 << PORTB1);
}
void PWM_reconnect_blue(void){
	TCCR1A |= (1 << COM1A1) | (1 << COM1A0);    // reconecta azul modo PWM no invertido
}

// Flag que indica si el canal rojo está siendo controlado manualmente
// en lugar de por el PWM por software
static volatile uint8_t red_forzado = 0; 

void PWM_force_red(uint8_t nivel){
	red_forzado = 1;
	if(nivel) PORTB |= (1 << PORTB5);
	else      PORTB &= ~(1 << PORTB5);
}
void PWM_reconnect_red(void){
	red_forzado = 0; // Volvemos a controlar el canal rojo con pwm x soft
}


void PWM_task(void){
	static uint32_t ultimo_tick_pwm = 0;
	
	// Obtiene el tick actual de la base de tiempos
	uint32_t ahora = TIMER0_get_ticks_100us();

	// Espera al siguiente tick de 100 us
	if(ahora == ultimo_tick_pwm)
		return;

	// Avanza un paso del PWM por software
	ultimo_tick_pwm = ahora;
	
	// Si el canal rojo está forzado (negro o blanco puro),
	// no tocar el pin: ya lo dejó en el nivel correcto PWM_force_red()
	if(red_forzado)
		return;

	// Incrementa el contador del PWM
	pwm_counter++;

	// Genera la señal PWM del canal rojo
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