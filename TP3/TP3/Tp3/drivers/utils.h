/*
 * utils.h
 *
 * Created: 23/6/2026 00:58:27
 *  Author: Ignacio Mucci Bigliani y Albertina Pezzutti
 */ 



#ifndef UTILS_H_
#define UTILS_H_


#include <stdint.h>
#include "rtc.h"
#include "dht11.h"
// Estados de alerta
typedef enum {
	ESTADO_NORMAL,
	ESTADO_ALERTA
} Estado;

// Resultados del chequeo de rangos
typedef enum {
	RANGO_OK,
	RANGO_TEMP_FUERA,
	RANGO_HUM_FUERA
} RangoStatus;



// Estas dos funciones son para pasar los valores traidos de la uart a los valores que reconoce el rtc y viceversa
uint8_t dec_to_bcd(uint8_t val);
uint8_t bcd_to_dec(uint8_t val);

// Checkeo de si el horario del rtc es diurno o nocturno
uint8_t is_diurno(uint8_t hours);

// Constructures de string para mandarle por telemetria al uart, tanto del estado como el strign de alerta
void build_telemetry_string(char *buf, RTC_Time *t, uint8_t temp, uint8_t hum, Estado estado);
void build_alert_string(char *buf, RTC_Time *t, uint8_t valor, RangoStatus rango, uint8_t es_diurno);

// Checkeo de si la temperatura o la humedad estan en los rangos correctos o si estan fuera de sus rangos a la hora que corresponde
RangoStatus check_rangos(uint8_t temp, uint8_t hum, uint8_t es_diurno);

void build_dht11_error_string(char *buf, RTC_Time *t, DHT11_Status status);

#endif /* UTILS_H_ */