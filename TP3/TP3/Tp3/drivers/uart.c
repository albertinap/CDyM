/*
 * uart.c
 * Driver UART0 con arquitectura foreground/background:
 *   - Foreground (ISRs): mueven bytes entre UDR0 y los buffers circulares
 *   - Background (UART_read_char / UART_send_string): accedidos desde el super loop
 * Author : Ignacio Mucci Bigliani y Albertina Pezzutti
 */

#include "uart.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "buffer_circular.h"

#define BAUD 9600
#define UBRR_VALUE 103	// UBRR = Fcpu / (16 * Baud) - 1  -->  16MHz / (16 * 9600) - 1 = 103

volatile CircularBuffer rx_buffer;		// buffer de recepción, escrito en ISR y leído en background
volatile CircularBuffer tx_buffer;		// buffer de transmisión, escrito en background y leído en ISR
										//variables volatiles porque pueden cambiar dentro de una ISR
void UART_init(void){
	// Baud rate: cargamos high y low por separado para que sea reutilizable con otros UBRR_VALUE
	UBRR0H = (uint8_t)(UBRR_VALUE>>8);
	UBRR0L = (uint8_t)UBRR_VALUE;

	// Formato 8N1: 8 bits de datos, sin paridad, 1 bit de stop
	// UCSZ02=0 (en UCSR0B), UCSZ01=1, UCSZ00=1 --> 8 bits
	UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);

	// Habilitar TX, RX y la interrupción de recepción completa
	// La interrupción de UDR vacío (UDRIE0) se habilita solo cuando hay datos para transmitir
	UCSR0B = (1<<RXEN0) | (1<<TXEN0) | (1<<RXCIE0);
	
	buffer_init(&rx_buffer);
	buffer_init(&tx_buffer);
}

// devuelve 1 si hay datos en el buffer de recepción, 0 c.c.
uint8_t UART_rx_available(void){
	return !buffer_is_empty(&rx_buffer); //buffer_is_empty devuelve 0 si hay datos, y 1 si está vacío
}

// Extrae un byte del buffer RX. Devuelve 1 si había dato, 0 si el buffer estaba vacío.
uint8_t UART_read_char(char *dato){
	return buffer_pop(&rx_buffer, dato);
}

// Encola un byte en el buffer TX y habilita la interrupción UDRIE para disparar la transmisión
void UART_send_char(char data){
	if(buffer_push(&tx_buffer,data)){
		UCSR0B |= (1<<UDRIE0);	//habilitamos la interrupción para cuando UDR0 quede vacío
	}
}

void UART_send_string(char *str){
	while(*str)	{
		UART_send_char(*str);
		str++;
	}
}

// ISR de recepción: se dispara cuando llega un byte completo a UDR0
// Filtra caracteres de backspace/DEL para que el usuario no pueda borrar caracteres desde la terminal
// Hace eco del carácter recibido para que el usuario vea lo que está escribendo
ISR(USART_RX_vect){
	char dato = UDR0;							//leemos UDR0
	
	if(dato == '\b' || dato == 0x7F) return;	//si el dato es un del o backspace no lo recibimos, para evitar inconsistencias en la terminal
	buffer_push(&rx_buffer,	dato);				// encolar para que background lo procese
	UART_send_char(dato);						// eco inmediato hacia la terminal (opcional, decisión de diseño)
}

// ISR de transmisión: se dispara cuando UDR0 queda vacío y UDRIE0 está habilitado
// Saca el próximo byte del buffer TX y lo carga en UDR0
// Si el buffer queda vacío, deshabilita UDRIE0 para no seguir disparando la ISR innecesariamente
ISR(USART_UDRE_vect){
	char dato;
	
	if(buffer_pop(&tx_buffer,&dato)){	//si hay datos en el buffer (esta función ya tiene en cuenta que el buffer no esté vacío)
		UDR0 = dato;					//entonces saco el próximo byte y lo escribo en el registro udr0
	}
	else{
		UCSR0B &= ~(1<<UDRIE0);			//si no hay datos para escribir deshabilito la interrupción
	}
}


