/*
 * rgb.c
 *
 * Implementación del driver RGB. Combina el color seleccionado por el usuario con el
 * nivel de brillo generado por la MEF y actualiza los canales PWM correspondientes.
 *
 * Author: Ignacio Mucci Bigliani y Albertina Pezzutti
 */

#include "rgb.h"
#include "../mef/MEF.h"
#include "../pwm/pwm.h"

// Componentes del color seleccionado por el usuario
static uint8_t red_base = 0;
static uint8_t green_base = 0;
static uint8_t blue_base = 0;

void RGB_init(void){
	// Inicializa el color en negro (LED apagado)
	RGB_set_color(0, 0, 0);
}

// Guarda el color seleccionado por el usuario
void RGB_set_color(uint8_t red, uint8_t green, uint8_t blue){	
	red_base = red;
	green_base = green;
	blue_base = blue;
}

void RGB_task(void){
	uint8_t brightness;
	uint8_t duty_r, duty_g, duty_b;

	// Obtiene el nivel de brillo actual (0 a 255)
	brightness = MEF_GetBrightness();
	
	// Escala cada componente del color manteniendo
	// la proporción definida por el usuario
	duty_r = (red_base   * brightness) / 255;
	duty_g = (green_base * brightness) / 255;
	duty_b = (blue_base  * brightness) / 255;
	
	// Canal Rojo (PB5, software PWM)
	if(duty_r == 0){
		PWM_force_red(1);		// forzamos HIGH -> apagado (ánodo común)
	} else if(duty_r == 255){
		PWM_force_red(0);		// forzamos LOW -> prendido 100%
	} else {
		// Utiliza PWM para obtener un brillo intermedio
		PWM_reconnect_red();
		PWM_set_red(duty_r);
	}

	// Canal Verde (PB2, hardware)
	if(duty_g == 0){
		PWM_force_green(1);		// Apaga completamente el canal
	} else if(duty_g == 255){
		PWM_force_green(0);		// Enciende el canal al 100%
	} else {
		// Utiliza PWM para obtener un brillo intermedio
		PWM_reconnect_green();
		PWM_set_green(duty_g);
	}

	// Canal Azul (PB1, hardware)
	if(duty_b == 0){
		PWM_force_blue(1);		// Apaga completamente el canal
	} else if(duty_b == 255){
		PWM_force_blue(0);		// Enciende el canal al 100%
	} else {
		// Utiliza PWM para obtener un brillo intermedio
		PWM_reconnect_blue();
		PWM_set_blue(duty_b);
	}
		
		
/*	// Escala cada componente manteniendo la proporción del color
	PWM_set_red((red_base * brightness) / 255);
	PWM_set_green((green_base * brightness) / 255);
	PWM_set_blue((blue_base * brightness) / 255);*/
}