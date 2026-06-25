/*
 * utils.C
 *
 * Created: 23/6/2026 00:58:27
 *  Author: Ignacio Mucci Bigliani y Albertina Pezzutti
 */ 

#include "utils.h"
#include <stdio.h>

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


uint8_t is_diurno(uint8_t hours) {
	// Devuelve 1 si la hora esta entre 07:00 y 18:59, 0 si es nocturno
	return (hours >= 7 && hours <= 18);
}

RangoStatus check_rangos(uint8_t temp, uint8_t hum, uint8_t es_diurno) {
	if (es_diurno) {
		// Rangos diurnos segun tabla 1 del enunciado
		if (temp < 20 || temp > 30) return RANGO_TEMP_FUERA;
		if (hum  < 50 || hum  > 70) return RANGO_HUM_FUERA;
		} else {
		// Rangos nocturnos segun tabla 1 del enunciado
		if (temp < 15 || temp > 22) return RANGO_TEMP_FUERA;
		if (hum  < 60 || hum  > 80) return RANGO_HUM_FUERA;
	}
	return RANGO_OK; // Ambos valores dentro del rango
}

void build_telemetry_string(char *buf, RTC_Time *t, uint8_t temp, uint8_t hum, Estado estado) {
	// buf: buffer de 70 chars donde se escribe el string
	// %02u: entero sin signo con minimo 2 digitos, rellena con 0 a la izquierda (ej: 9 -> "09")
	// %%: imprime un % literal, porque % solo inicia un formato
	// operador ternario: si estado es NORMAL imprime "NORMAL", sino "ALERTA"
	snprintf(buf, 64, "[%02u:%02u:%02u]T:%u°C|H:%u%%|Estado:%s",
	t->hours, t->minutes, t->seconds,
	temp, hum,
	(estado == ESTADO_NORMAL) ? "NORMAL" : "ALERTA");
}

void build_alert_string(char *buf, RTC_Time *t, uint8_t valor, RangoStatus rango, uint8_t es_diurno) {
	// Selecciona el texto segun que variable esta fuera de rango
	const char *variable = (rango == RANGO_TEMP_FUERA) ? "Temperatura" : "Humedad";
	// Selecciona el periodo horario
	const char *periodo  = es_diurno ? "diurno" : "nocturno";
	// Selecciona la unidad segun la variable
	const char *unidad   = (rango == RANGO_TEMP_FUERA) ? "C" : "%";
	// Construye el string de alerta con todos los valores
	snprintf(buf, 64, "[ALERTA][%02u:%02u:%02u]%s fuera de rango %s!Valor:%u%s",
	t->hours, t->minutes, t->seconds,
	variable, periodo, valor, unidad);
}