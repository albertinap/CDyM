/*
 * rgb.c
 *
 * Created: 29/7/2026 18:56:54
 *  Author: Ignacio Mucci Bigliani y Albertina Pezzutti
 */ 

#include "rgb.h"
#include "../mef/MEF.h"
#include "../pwm/pwm.h"

// Color seleccionado por el usuario
static uint8_t red_base = 0;
static uint8_t green_base = 0;
static uint8_t blue_base = 0;

void RGB_init(void){
	RGB_set_color(0, 0, 0);
}

void RGB_set_color(uint8_t red, uint8_t green, uint8_t blue){
	// Guarda el color seleccionado por el usuario
	red_base = red;
	green_base = green;
	blue_base = blue;
}

void RGB_task(void){
	uint8_t brightness;

	// Obtiene el nivel de brillo actual (0 a 255)
	brightness = MEF_GetBrightness();

	// Escala cada componente manteniendo la proporción del color
	PWM_set_red((red_base * brightness) / 255);
	PWM_set_green((green_base * brightness) / 255);
	PWM_set_blue((blue_base * brightness) / 255);
}