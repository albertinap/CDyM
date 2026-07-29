/*
 * uart.h
 * Interfaz del driver UART0
 * Comunicación serie asincrónica a 9600bps 8N1
 * Recepción y transmisión manejadas por interrupciones y con buffers circulares 
 * Author : Ignacio Mucci Bigliani y Albertina Pezzutti
 */

#ifndef UART_H_
#define UART_H_
#include <stdint.h>

void UART_init(void);				// Inicializa UART0 a 9600bps 8N1 con interrupciones

void UART_send_char(char data);		// Encola un carácter para transmitir

void UART_send_string(char *str);	// Encola un string completo para transmitir (internamente hace muchos send_char)

uint8_t UART_rx_available(void);	// Devuelve 1 si hay datos en el buffer RX

uint8_t UART_read_char(char *dato);	// Extrae un carácter del buffer RX en el parámetro dato. Devuelve 1 si había dato, 0 si no

#endif

