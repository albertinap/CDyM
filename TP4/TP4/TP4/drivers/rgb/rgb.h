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

void RGB_set_color(uint8_t r, uint8_t g, uint8_t b);

void RGB_update(void); //hay que hacerla más adelante

uint8_t RGB_get_red(void);
uint8_t RGB_get_green(void);
uint8_t RGB_get_blue(void);

#endif /* RGB_H_ */