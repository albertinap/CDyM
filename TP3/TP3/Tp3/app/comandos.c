/*
 * comandos.c
 *
 * Created: 24/6/2026 00:31:47
 * Author : Ignacio Mucci Bigliani y Albertina Pezzutti
 */ 
#include "comandos.h"

#include <string.h>
#include <stdlib.h>

#include "../drivers/uart.h"
#include "invernadero.h"

#define CMD_SIZE 32

static char comando[CMD_SIZE];
static uint8_t indice;

void comandos_init(void){
	indice = 0;
}


void comandos_tarea(void){
	char c;
	
	while(UART_read_char(&c)){
		if(c == '\n' || c == '\r'){
			comando[indice] = '\0';

			procesar_comando(comando);

			indice = 0;
		}
		else		{
			if(indice < CMD_SIZE-1){
				comando[indice] = c;
				indice++;
			}
		}
	}
}

void procesar_comando(char *cmd){
	/*UART_send_string("CMD:[");
    UART_send_string(cmd);
    UART_send_string("]\r\n");*/
	if(strncmp(cmd, "SET_TM=", 7) == 0){
		uint16_t segundos;

		segundos = atoi(&cmd[7]);

		invernadero_set_periodo(segundos);

		UART_send_string("Tasa de muestreo cambiada\r\n");
	}
}