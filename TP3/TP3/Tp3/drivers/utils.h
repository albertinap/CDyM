/*
 * utils.h
 *
 * Created: 23/6/2026 00:58:27
 *  Author: Ignacio Mucci Bigliani y Albertina Pezzutti
 */ 



#ifndef UTILS_H_
#define UTILS_H_


#include <stdint.h>


// Estas dos funciones son para pasar los valores traidos de la uart a los valores que reconoce el rtc y viceversa
uint8_t dec_to_bcd(uint8_t val);
uint8_t bcd_to_dec(uint8_t val);



#endif /* UTILS_H_ */