/*
 * rgb.c
 *
 * Created: 29/7/2026 18:56:54
 *  Author: Ignacio Mucci Bigliani y Albertina Pezzutti
 */ 

#include "rgb.h"
//#include "pwm.h"

static uint8_t red = 0;
static uint8_t green = 0;
static uint8_t blue = 0;

void RGB_init(void){
	//PWM_init();

	RGB_set_color(0, 0, 0);
}

void RGB_set_color(uint8_t r, uint8_t g, uint8_t b){
	red   = r;
	green = g;
	blue  = b;

	//PWM_set_red(red);
	//PWM_set_green(green);
	//PWM_set_blue(blue);
}

uint8_t RGB_get_red(void){
	return red;
}

uint8_t RGB_get_green(void){
	return green;
}

uint8_t RGB_get_blue(void){
	return blue;
}

void RGB_update(void)
{
} /*va a ser la encargada de:
calcular el brillo según el estado (st_off, st_fadein, st_on, st_fadeout);
escalar los tres colores;
actualizar los PWM

Así no tenemos que modificar la interfaz pública del módulo cuando implementemos el efecto de respiración;*/