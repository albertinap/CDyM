/*
 * invernadero.h
 * Interfaz del módulo de lógica del invernadero
 * Gestiona el período de reporte y la tarea de background que genera telemetría
 * Author : Ignacio Mucci Bigliani y Albertina Pezzutti
 */

#ifndef INVERNADERO_H_
#define INVERNADERO_H_

#include <stdint.h>

void invernadero_init(void);						// inicializa período de muestreo

void invernadero_set_periodo(uint16_t segundos);	// cambia el período de reporte (2 a 60 segundos)

void invernadero_tarea(void);						// tarea de background, envía telemetría si pasó el período T

#endif /* INVERNADERO_H_ */
