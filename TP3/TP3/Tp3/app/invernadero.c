/*
 * invernadero.h
 *
 * Created: 23/6/2026 00:58:27
 *  Author: Ignacio Mucci Bigliani y Albertina Pezzutti
 */ 
#include "invernadero.h"
#include "../drivers/uart.h"
#include "../drivers/timer.h"
#include <string.h>
#include <stdlib.h>

static uint16_t periodo_reporte;			//cada cuánto mostramos las tramas de telemetría
static uint16_t ultimo_reporte;				//

void invernadero_init(void){
	//el periodo arranca inicializado en 10 segundos
	//10 segundos = 100 ticks de 100ms

	periodo_reporte = 100;

	ultimo_reporte = TIMER_get_ticks();		//si hubo un reporte antes
}

void invernadero_set_periodo(uint16_t segundos){
	if(segundos >= 2 && segundos <= 60){
		periodo_reporte = segundos * 10;	//lo multiplicamos por 10 porque nuestro timer cuenta de a 100ms
	}
}

void invernadero_tarea(void){
	uint16_t ahora;

	ahora = TIMER_get_ticks();

	if((ahora - ultimo_reporte) >= periodo_reporte){

		ultimo_reporte = ahora;
		UART_send_string("Reporte de invernadero\r\n");
	}
}