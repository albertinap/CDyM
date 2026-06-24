/*
 * invernadero.h
 *
 * Created: 23/6/2026 00:58:27
 *  Author: Ignacio Mucci Bigliani y Albertina Pezzutti
 */ 

#ifndef INVERNADERO_H_
#define INVERNADERO_H_

#include <stdint.h>

void invernadero_init(void);

void invernadero_set_periodo(uint16_t segundos);

void invernadero_procesar_comando(char *cmd);

void invernadero_tarea(void);

#endif /* INVERNADERO_H_ */
