/*
 * comandos.c
 * Se encarga de leer caracteres de la UART, armar comandos línea por línea,
 * y ejecutarlos validando formato y rango de los datos recibidos.
 * Author : Ignacio Mucci Bigliani y Albertina Pezzutti
 */ 
#include "comandos.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "../drivers/uart.h"
#include "../drivers/rtc.h"
#include "invernadero.h"

#define CMD_SIZE 32				// el comando más largo ocupa 17 bytes; está contenido en este tamaño

static char comando[CMD_SIZE];	// buffer donde se acumula el comando actual
static uint8_t indice;			// posición actual dentro del buffer

// inicializamos el índice al principio del vector
void comandos_init(void){
	indice = 0;
}

// Tarea de foreground: va consumiendo el buffer RX de la UART carácter por carácter
// Cuando detecta fin de línea (enter), despacha el comando acumulado
void comandos_tarea(void){
	char c;
	
	while(UART_read_char(&c)){
		if(c == '\n' || c == '\r'){
			comando[indice] = '\0';		// terminamos el string
			procesar_comando(comando);	
			indice = 0;					// reiniciamos para el próximo comando
		}
		else{
			if(indice < CMD_SIZE-1){	// guardamos solo si hay espacio
				comando[indice] = c;
				indice++;
			}
		}
	}
}

// Interpreta y ejecuta el comando recibido
// Comandos soportados actualmente:
// - SET_TM=SS --> cambia la tasa de muestreo (2 a 60 segundos)
// - SET_TIME=HH:MM:SS --> actualiza la hora del RTC
void procesar_comando(char *cmd){
	if(cmd[0] == '\0') return;				// ignorar líneas vacías (ej: \r\n seguidos)
	
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
		
		// atoi se detiene solo al encontrar ':' o '\0'
		t.hours   = atoi(&p[0]);
		t.minutes = atoi(&p[3]);
		t.seconds = atoi(&p[6]);
		 
		// Validar rangos de hora
		if(t.hours > 23 || t.minutes > 59 || t.seconds > 59){
			UART_send_string("ERROR EN LA HORA\r\n");
			return;
		}
		
		RTC_set_time(&t);
		UART_send_string("Hora modificada.\r\n");
	}
	else{
		UART_send_string("Comando no reconocido.\r\n");
	}
}
