//La función de este buffer es resolver la diferencia de velocidades entre la UART (hardware + ISR) y el programa principal.
#include "buffer_circular.h"

void buffer_init(volatile CircularBuffer *buffer){
	buffer->head = 0;
	buffer->tail = 0;
}

uint8_t buffer_push(volatile CircularBuffer *buffer, char data){
	if(buffer_is_full(buffer))
		return 0;

	buffer->data[buffer->head] = data;
	buffer->head++;

	if(buffer->head >= BUFFER_SIZE)	//vuelvo al principio, buffer circular
		buffer->head = 0;

	return 1;
}

uint8_t buffer_pop(volatile CircularBuffer *buffer, char *data){
	if(buffer_is_empty(buffer))
		return 0;

	*data = buffer->data[buffer->tail];

	buffer->tail++;

	if(buffer->tail >= BUFFER_SIZE)
		buffer->tail=0;

	return 1;
}

uint8_t buffer_is_empty(volatile CircularBuffer *buffer){
	return (buffer->head == buffer->tail);	//si head==tail entonces no hay datos para leer
}

uint8_t buffer_is_full(volatile CircularBuffer *buffer){
	uint8_t next;

	next = buffer->head + 1;

	if(next >= BUFFER_SIZE)
		next=0;

	return (next == buffer->tail);
}