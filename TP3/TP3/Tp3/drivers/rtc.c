/*
 * rtc.c
 *
 * Created: 23/6/2026 00:58:27
 *  Author: Ignacio Mucci Bigliani y Albertina Pezzutti
 */ 

#include "rtc.h"
#include "I2C.h"
#include "utils.h"

void RTC_set_time(RTC_Time *t) {
	I2C_start(); //Inicio pasaje de mensaje
	I2C_write((DS3232_ADDR << 1) | 0);  // Direccion + Write
	I2C_write(0x00);                     // Apunto al registro seconds dentro del rtc
	I2C_write(dec_to_bcd(t->seconds)); //mando los segundos al rtc
	I2C_write(dec_to_bcd(t->minutes)); // mando los minutos al rtc
	I2C_write(dec_to_bcd(t->hours)); //Mando las horas al rtc
	I2C_stop(); // termino el pasaje de mensaje
}

void RTC_get_time(RTC_Time *t) {
	I2C_start(); // Inicio mensaje
	I2C_write((DS3232_ADDR << 1) | 0);  // Direccion + Write, apunto al registro
	I2C_write(0x00);                     // Registro origen: seconds
	I2C_stop(); // termino el mensaje

	I2C_start(); //Inicio transaccion
	I2C_write((DS3232_ADDR << 1) | 1);       // Direccion + Read
	t->seconds = bcd_to_dec(I2C_read(0));    // Segundos y ACK, hay mas bytes
	t->minutes = bcd_to_dec(I2C_read(0));    // Minutos y ACK, hay mas bytes
	t->hours   = bcd_to_dec(I2C_read(1));    // Horas y NACK, ultimo byte
	I2C_stop(); // termino el mensaje
}