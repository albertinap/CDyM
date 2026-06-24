#include "invernadero.h"
#include "drivers/uart.h"
#include "drivers/timer.h"
#include <string.h>

static uint16_t periodo_reporte;
static uint16_t ultimo_reporte;

void invernadero_init(void){
	//5 segundos iniciales
	//5 segundos = 50 ticks de 100ms

	periodo_reporte = 50;

	ultimo_reporte = TIMER_get_ticks();
}

void invernadero_procesar_comando(char *cmd){
	if(strncmp(cmd,"SET_TM=",7)==0){
		uint16_t segundos;

		segundos = atoi(&cmd[7]);

		if(segundos >= 2 && segundos <= 60){
			periodo_reporte = segundos * 10;
			UART_send_string("OK\r\n");
		}
		else{
			UART_send_string("ERROR\r\n");
		}
	}
}

void invernadero_tarea_uart(void){
	static char comando[20];
	static uint8_t indice=0;

	char c;

	while(UART_read_char(&c)){
		if(c == '\n'){
			comando[indice]='\0';
			invernadero_procesar_comando(comando);
			indice=0;
		}
		else{
			comando[indice++]=c;
		}
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