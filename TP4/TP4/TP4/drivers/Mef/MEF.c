/*
 * MEF.c
 *
 * Created: 29/7/2026 18:59:39
 *  Author: rocio
 */ 

#include "mef.h"
#include "../adc/adc.h"
#include "../timers/timer0.h"

#define TICK_MS        1 //Ticks de timer

#define FADE_TICKS     1000 //Cantidad de ticks de cambio de brillo dependiento de los ticks del timer
#define ON_TICKS       1000 //Cantidad de ticks de cambio de brillo dependiento de los ticks del timer

static MEF_State_t estado;

static uint16_t contadorEstado; // Variable que cuenta en cada estado cuanto tiempo se tiene que estar; con esta varaible tambien se va a hacer el cambio de brillo periodico
static uint16_t periodoTicks;   // Valor entre 300 ms y 600 ms el cual depende del LDR y el valor que devuelve el adc
static uint32_t ultimo_tick_mef = 0;

static uint8_t brillo;

void MEF_Init(void)
{
	estado = ST_OFF;

	contadorEstado = 0;

	brillo = 0;

	periodoTicks = 6000;
}


void MEF_Update(void)
{
	uint16_t adc;  //Valor que devuelve el adc

	contadorEstado++;

	adc = ADC_Read();

	periodoTicks = 6000 - ((uint32_t)adc * 3000) / 1023; // Calculo el periodo entre 300 ms y 600 ms

	switch(estado)
	{

		case ST_OFF:

		brillo = 0;

		if(contadorEstado >= (periodoTicks - 3000)) // Dado a que se tiene un tiempo determinado de fade in, fade out y on, los cuales sumando son 3 seg, el tiempo restante es la cantidad de tiempo que debe estar en off
		{
			estado = ST_FADE_IN; // Cambio al estado fade in
			contadorEstado = 0;
		}

		break;

		case ST_FADE_IN:

		brillo = (255UL * contadorEstado) / FADE_TICKS; // Cambio el valor del brillo dependiendo de que tan cerca estoy del top

		if(contadorEstado >= FADE_TICKS)
		{
			brillo = 255; // Pongo el brillo en max
			estado = ST_ON; // Cabmio al estado on
			contadorEstado = 0;
		}

		break;

		case ST_ON:

		brillo = 255;

		if(contadorEstado >= ON_TICKS) // Espero a que pase el segundo de on
		{
			estado = ST_FADE_OUT;
			contadorEstado = 0;
		}

		break;

		case ST_FADE_OUT:

		brillo = 255 - ((255UL * contadorEstado) / FADE_TICKS); // Voy bajando el brillo hasta que se ponga en 0.

		if(contadorEstado >= FADE_TICKS)
		{
			brillo = 0; // Bajo el brillo a 0
			estado = ST_OFF;
			contadorEstado = 0;
		}

		break;
	}
}

void MEF_task(void){
	uint32_t ahora = TIMER0_get_ticks_100us();

	// 10 ms = 100 ticks de 100 us
	if((ahora - ultimo_tick_mef) < 100)
		return;

	ultimo_tick_mef = ahora;

	MEF_Update();
}

uint8_t MEF_GetBrightness(void) // Funcion para devolver el brillo para los valores de pmw.
{
	return brillo;
}
