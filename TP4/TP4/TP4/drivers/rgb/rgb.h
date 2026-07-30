/*
 * rgb.h
 *
 * Created: 29/7/2026 18:57:21
 *  Author: Ignacio Mucci Bigliani y Albertina Pezzutti
 */ 

#ifndef RGB_H_
#define RGB_H_

#include <stdint.h>

void RGB_init(void);

// Guarda el color elegido por el usuario
void RGB_set_color(uint8_t red, uint8_t green, uint8_t blue);

// Actualiza el brillo del LED según el brillo actual
void RGB_task(void);

#endif /* RGB_H_ */