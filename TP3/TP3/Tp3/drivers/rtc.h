/*
 * rtc.h
 *
 * Created: 23/6/2026 00:58:27
 *  Author: Ignacio Mucci Bigliani y Albertina Pezzutti
 */ 

#ifndef RTC_H
#define RTC_H

#include <stdint.h>

#define DS3232_ADDR 0x68 // Direccion fija del ds3232 para el bus I2C

typedef struct { //Estructura del tiempo  para devolver todos los valores directos 
	uint8_t hours;
	uint8_t minutes;
	uint8_t seconds;
} RTC_Time;

void RTC_set_time(RTC_Time *t); //Seteo el tiempo del rtc para que siga desde ahi
void RTC_get_time(RTC_Time *t); //Obtengo el tiempo del rtc

#endif /* RTC_H */
