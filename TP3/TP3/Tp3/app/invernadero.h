/*
 * invernadero.h
 *
 * Created: 23/6/2026 00:58:27
 *  Author: Ignacio Mucci Bigliani y Albertina Pezzutti
 */ 

#ifndef INVERNADERO_H_
#define INVERNADERO_H_

void invernadero_init(void);

void invernadero_procesar_comando(char *cmd);

void invernadero_tarea_uart(void);

void invernadero_tarea(void);

#endif /* INVERNADERO_H_ */
