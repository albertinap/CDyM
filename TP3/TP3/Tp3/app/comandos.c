/*
 * comandos.c
 *
 * Created: 24/6/2026 00:31:47
 * Author : Ignacio Mucci Bigliani y Albertina Pezzutti
 */ 
#include "comandos.h"

#include <string.h>
#include <stdlib.h>

#include "drivers/uart.h"
#include "invernadero.h"

#define CMD_SIZE 32

static char comando[CMD_SIZE];
static uint8_t indice;

void comandos_init(void){
	indice = 0;
}


void comandos_tarea(void){
	char c;
	while(UART_rx_available()){
		c = UART_read_char();

		if(c == '\n'){
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


static void procesar_comando(char *cmd){
	if(strncmp(cmd, "SET_TM=", 7) == 0){
		uint16_t segundos;

		segundos = atoi(&cmd[7]);

		invernadero_set_periodo(segundos);

		UART_send_string("OK\r\n");
	}
}