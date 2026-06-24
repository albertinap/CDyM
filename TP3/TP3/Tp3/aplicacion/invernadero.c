#include "invernadero.h"

#include "drivers/uart.h"
#include "drivers/timer.h"

static uint16_t periodo_reporte;
static uint16_t ultimo_reporte;

void invernadero_init(void){
	//5 segundos iniciales
	//5 segundos = 50 ticks de 100ms

	periodo_reporte = 50;

	ultimo_reporte = TIMER_get_ticks();
}

void invernadero_tarea(void){
	uint16_t ahora;

	ahora = TIMER_get_ticks();

	if((ahora - ultimo_reporte) >= periodo_reporte){

		ultimo_reporte = ahora;
		UART_send_string("Reporte de invernadero\r\n");
	}
}

void invernadero_set_periodo(uint16_t segundos){
	if(segundos >= 2 && segundos <= 60){
		periodo_reporte = segundos * 10;	//acá multiplicamos por 10 porque nuestro timer cuenta de a 100ms
	}
}