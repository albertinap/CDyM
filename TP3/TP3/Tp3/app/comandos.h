/*
 * comandos.h
 *
 * Created: 24/6/2026 00:32:00
 * Author : Ignacio Mucci Bigliani y Albertina Pezzutti
 */ 

#ifndef COMANDOS_H_
#define COMANDOS_H_

#include <stdint.h>

void comandos_init(void);
void comandos_tarea(void);
void procesar_comando(char *cmd);

#endif /* COMANDOS_H_ */