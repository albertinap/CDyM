#ifndef BUFFER_CIRCULAR_H_
#define BUFFER_CIRCULAR_H_

#include <stdint.h>

#define BUFFER_SIZE 64

typedef struct{
	char data[BUFFER_SIZE];	//memoria donde guardamos los datos
	uint8_t head;			//índice donde corresponde escribir
	uint8_t tail;			//índice donde corresponde leer
} CircularBuffer;			//nombre de la estructura

void buffer_init(volatile CircularBuffer *buffer);				//inicializar buffer (índices head y tail)
	
uint8_t buffer_push(volatile CircularBuffer *buffer, char data);//agregar un elemento (escribir)

uint8_t buffer_pop(volatile CircularBuffer *buffer, char *data);//quitar un elemento (leer)

uint8_t buffer_is_empty(volatile CircularBuffer *buffer);		//

uint8_t buffer_is_full(volatile CircularBuffer *buffer);

#endif