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
#include "uart.h"
#include "../rgb/rgb.h"

#define CMD_SIZE 32				// tamaño de comando máximo

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
			comando[indice] = '\0';
			if(indice > 0){
				UART_send_string(comando);  // mostramos lo que el usuario escribió
				UART_send_string("\r\n");
				procesar_comando(comando);
			}
			indice = 0;
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
// - SET_COLOR=R,G,B/r/n --> cambia el color rgb del led
void procesar_comando(char *cmd)
{
	if(cmd[0] == '\0')
		return;

	if(strncmp(cmd, "SET_COLOR=", 10) == 0){
		uint16_t r, g, b;

		if(sscanf(&cmd[10], "%u,%u,%u", &r, &g, &b) == 3){
			if(r <= 255 && g <= 255 && b <= 255){
				RGB_set_color((uint8_t)r, (uint8_t)g, (uint8_t)b);
				UART_send_string("Color actualizado.\r\n");
			}
			else{
				UART_send_string("ERROR: valores entre 0 y 255\r\n");
			}
		}
		else{
			UART_send_string("ERROR: formato SET_COLOR=R,G,B\r\n");
		}
	}
	else{
		UART_send_string("Comando no reconocido.\r\n");
	}
}
