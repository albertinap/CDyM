#include "uart.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "buffer_circular.h"

#define BAUD 9600
#define UBRR_VALUE 103	//UBRR = FCPU/(16*Baud)-1

volatile CircularBuffer rx_buffer;		//buffer de 64 caracteres
volatile CircularBuffer tx_buffer;		//volatil porque estas variables pueden cambiar dentro de una ISR

void UART_init(void)
{
	// Baud rate
	UBRR0H = (uint8_t)(UBRR_VALUE>>8);	//si usamos 103 es innecesario, pero lo dejamos para hacerlo reutilizable
	UBRR0L = (uint8_t)UBRR_VALUE;

	// 8N1
	UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);	//8 bits: UCSZ02=0, UCSZ01=1, UCSZ00=1

	// Habilitar recepción y transmisión, e interrupciones para cuando llegan datos
	UCSR0B = (1<<RXEN0) | (1<<TXEN0) | (1<<RXCIE0);
	
	buffer_init(&rx_buffer);
	buffer_init(&tx_buffer);
}

uint8_t UART_rx_available(void){
	return !buffer_is_empty(&rx_buffer); //buffer_is_empty devuelve 0 si hay datos, y 1 si está vacío
}

uint8_t UART_read_char(char *dato){
	return buffer_pop(&rx_buffer, dato);
}

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

ISR(USART_RX_vect){
	char dato;
	dato = UDR0;	//leemos UDR0
	if(dato == '\b' || dato == 0x7F) return;	//si el dato es un del o backspace no lo recibimos, 
												//para evitar inconsistencias en la terminal
	buffer_push(&rx_buffer,	dato);				//lo pusheamos en el buffer circular de la recepción
												//esta función ya tiene en cuenta que el buffer no esté lleno
	UART_send_char(dato);						 // eco, lo dejamos? es necesario? iría dentro de la ISR?
}

//interrupción para cuando el registro UDR está vacío
ISR(USART_UDRE_vect){
	char dato;
	
	if(buffer_pop(&tx_buffer,&dato)){	//si hay datos en el buffer (esta función ya tiene en cuenta que el buffer no esté vacío)
		UDR0 = dato;					//entonces saco el próximo byte y lo escribo en el registro udr0
	}
	else{
		UCSR0B &= ~(1<<UDRIE0);			//si no hay datos para escribir deshabilito la interrupción
	}
}


