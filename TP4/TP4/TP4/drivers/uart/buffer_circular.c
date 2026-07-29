/*
* buffer_circular.c
* Implementación del buffer circular. Se usa para desacoplar la ISR de UART
* (que escribe/lee a alta velocidad) del background (que procesa cuando puede)
*
* Política de llenado: se reserva un slot para distinguir lleno de vacío,
* por eso la capacidad efectiva es BUFFER_SIZE-1 bytes
*
* head apunta al próximo lugar donde escribir
* tail apunta al próximo lugar donde leer
* Si head == tail --> vacío. Si (head+1) % SIZE == tail --> lleno.
* Author : Ignacio Mucci Bigliani y Albertina Pezzutti
*/

#include "buffer_circular.h"

void buffer_init(volatile CircularBuffer *buffer){
	buffer->head = 0;
	buffer->tail = 0;
}

// Encola 'data'. Devuelve 1 si tuvo éxito, 0 si el buffer estaba lleno
uint8_t buffer_push(volatile CircularBuffer *buffer, char data){
	if(buffer_is_full(buffer))
		return 0;

	buffer->data[buffer->head] = data;
	buffer->head++;

	if(buffer->head >= BUFFER_SIZE)	//vuelvo al principio, buffer circular
		buffer->head = 0;

	return 1;
}

// Desencola en *data. Devuelve 1 si tuvo éxito, 0 si el buffer estaba vacío
uint8_t buffer_pop(volatile CircularBuffer *buffer, char *data){
	if(buffer_is_empty(buffer))return 0;
	
	*data = buffer->data[buffer->tail];
	buffer->tail++;
	
	if(buffer->tail >= BUFFER_SIZE)
		buffer->tail=0;
	return 1;
}

// head == tail significa que no hay datos pendientes de leer
uint8_t buffer_is_empty(volatile CircularBuffer *buffer){
	return (buffer->head == buffer->tail);
}

// El slot siguiente a head está ocupado por tail --> buffer lleno
uint8_t buffer_is_full(volatile CircularBuffer *buffer){
	uint8_t next;

	next = buffer->head + 1;

	if(next >= BUFFER_SIZE)
		next=0;

	return (next == buffer->tail);
}