/*
 * buffer_circular.h
 * Buffer circular genérico de caracteres para desacoplar velocidades (ISR y background)
 * Author : Ignacio Mucci Bigliani y Albertina Pezzutti
 */
#ifndef BUFFER_CIRCULAR_H_
#define BUFFER_CIRCULAR_H_

#include <stdint.h>

#define BUFFER_SIZE 140		//tenemos en cuenta el peor caso; tener que mandar 2 tramas, una de información y la otra de alerta


typedef struct{
	char data[BUFFER_SIZE];	//memoria donde guardamos los datos
	uint8_t head;			//índice donde corresponde escribir
	uint8_t tail;			//índice donde corresponde leer
} CircularBuffer;

void buffer_init(volatile CircularBuffer *buffer);					// inicializa head y tail en 0
	
uint8_t buffer_push(volatile CircularBuffer *buffer, char data);	// encola un byte; devuelve 0 si está lleno

uint8_t buffer_pop(volatile CircularBuffer *buffer, char *data);	// desencola un byte; devuelve 0 si está vacío

uint8_t buffer_is_empty(volatile CircularBuffer *buffer);			// devuelve 1 si head == tail (no hay elementos para leer)

uint8_t buffer_is_full(volatile CircularBuffer *buffer);			// devuelve 1 si (head+1) % SIZE == tail (no hay lugar para escribir algo nuevo)

#endif