/*
 * comandos.c
 *
 * Created: 24/6/2026 00:31:47
 * Author : Ignacio Mucci Bigliani y Albertina Pezzutti
 */ 
#include "comandos.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "../drivers/uart.h"
#include "../drivers/rtc.h"
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
		else{
			if(indice < CMD_SIZE-1){
				comando[indice] = c;
				indice++;
			}
		}
	}
}

void procesar_comando(char *cmd){
	if(cmd[0] == '\0') return;
	
	if(strncmp(cmd, "SET_TM=", 7) == 0){
		uint16_t segundos = atoi(&cmd[7]);
		
		if(segundos >= 2 && segundos <= 60){
			invernadero_set_periodo(segundos);
			char msg[32];
			sprintf(msg, "Tasa de muestreo: %d s\r\n", segundos);
			UART_send_string(msg);
		} else {
			UART_send_string("ERROR EN LA TASA DE MUESTREO\r\n");
		}
	}
	else if(strncmp(cmd, "SET_TIME=", 9) == 0){
		RTC_Time t;
		char *p = &cmd[9];
		 
		// Validar formato HH:MM:SS (largo fijo de 8 caracteres)
		if(strlen(p) != 8 || p[2] != ':' || p[5] != ':'){
			UART_send_string("ERROR EN EL FORMATO DE HORA\r\n");
			return;
		}
		 
		t.hours   = atoi(&p[0]);
		t.minutes = atoi(&p[3]);
		t.seconds = atoi(&p[6]);
		 
		// Validar rangos
		if(t.hours > 23 || t.minutes > 59 || t.seconds > 59){
			UART_send_string("ERROR EN LA HORA\r\n");
			return;
		}
		RTC_set_time(&t);
		
		UART_send_string("Hora modificada.\r\n");
	}
}