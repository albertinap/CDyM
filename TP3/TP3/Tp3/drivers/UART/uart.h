#ifndef UART_H_
#define UART_H_

#include <stdint.h>

void UART_init(void);				//Inicializa el periférico, configurado a 9600bps con formato 8N1

void UART_send_char(char data);		//Manda un carácter

void UART_send_string(char *str);	//Manda un string, internamente hace muchos send_char

uint8_t UART_rx_available(void);	//Hay un byte disponible en RX y lo extrae

char UART_read_char(void);

#endif

