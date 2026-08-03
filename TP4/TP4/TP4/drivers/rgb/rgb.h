/*
 * rgb.h
 * Interfaz del driver RGB.
 * Administra el color del LED RGB combinando el color
 * seleccionado por el usuario con el nivel de brillo
 * calculado por la máquina de estados.
 *
 * Author: Ignacio Mucci Bigliani y Albertina Pezzutti
 */

#ifndef RGB_H_
#define RGB_H_

#include <stdint.h>

// Inicializa el módulo RGB
void RGB_init(void);

// Guarda el color base seleccionado por el usuario
void RGB_set_color(uint8_t red, uint8_t green, uint8_t blue);

// Actualiza el brillo efectivo del LED RGB
void RGB_task(void);

#endif /* RGB_H_ */