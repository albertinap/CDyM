/*
 * utils.C
 *
 * Created: 23/6/2026 00:58:27
 *  Author: Ignacio Mucci Bigliani y Albertina Pezzutti
 */ 

#include "utils.h"



uint8_t dec_to_bcd(uint8_t val) {
	// val/10 saca la decena (ej: 34/10 = 3), la shiftea 4 bits a la izquierda para ponerla en el nibble alto
	// val%10 saca la unidad (ej: 34%10 = 4), va en el nibble bajo
	// OR combina ambos nibbles: 0x34
	return ((val / 10) << 4) | (val % 10);
}

uint8_t bcd_to_dec(uint8_t val) {
	// val>>4 extrae el nibble alto (decena), multiplica por 10 para convertirlo a decimal
	// val&0x0F enmascara y extrae solo el nibble bajo (unidad)
	// suma ambos: (3*10) + 4 = 34
	return ((val >> 4) * 10) + (val & 0x0F);
}