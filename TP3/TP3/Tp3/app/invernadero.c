/*
 * invernadero.c
 * Lógica principal del monitor de invernadero
 * Tarea de background: cada 'periodo_reporte' ticks lee el RTC y el DHT11,
 * evalúa rangos y envía la trama de telemetría (y alerta si corresponde) por UART
 * Author : Ignacio Mucci Bigliani y Albertina Pezzutti
 */
#include "invernadero.h"
#include "../drivers/uart.h"
#include "../drivers/timer.h"
#include "../drivers/rtc.h"
#include "../drivers/I2C.h"
#include "../drivers/utils.h"
#include "../drivers/dht11.h"
#include <string.h>
#include <stdlib.h>

static uint16_t periodo_reporte;			// intervalo entre reportes, en ticks de 100ms
static uint16_t ultimo_reporte;				// tick en el que se envió el último reporte

void invernadero_init(void){
	//el periodo arranca inicializado en 10 segundos
	//10 segundos = 100 ticks de 100ms
	periodo_reporte = 100;
	ultimo_reporte = TIMER_get_ticks();		//si hubo un reporte antes
}

// Actualiza el período de reporte. 
// La validación de rango se hace acá para que el módulo sea autónomo independientemente de quién llame a esta función
void invernadero_set_periodo(uint16_t segundos){
	if(segundos >= 2 && segundos <= 60){
		periodo_reporte = segundos * 10;	//lo multiplicamos por 10 porque nuestro timer cuenta de a 100ms
	}
}

// Tarea de background, se llama en cada iteración del super loop
// Si pasó al menos 'periodo_reporte' ticks desde el último reporte:
// - Lee hora del RTC y datos del DHT11
// - Evalúa si temperatura y humedad están en rango según la ventana horaria
// - Envía trama de telemetría
// - Si hay alerta, envía trama de emergencia adicional
void invernadero_tarea(void){
	uint16_t ahora;
	ahora = TIMER_get_ticks();
	if((ahora - ultimo_reporte) >= periodo_reporte){
		ultimo_reporte = ahora;
		UART_send_string("Reporte de invernadero\r\n");
		char buf[64];
		// Seteamos una hora de prueba en el RTC
		RTC_Time t_set = {.hours = 20, .minutes = 30, .seconds = 0};
		RTC_set_time(&t_set);

		// Leemos el tiempo del RTC
		RTC_Time t;
		RTC_get_time(&t);

		// Valores de prueba del DHT11
		DHT11_Status DS;
		DHT11_Data DHD;

		DS= DHT11_read(&DHD);
			
		if (DS != DHT11_OK) {
			build_dht11_error_string(buf,&t,DS);
			UART_send_string(buf);
			UART_send_string("\r\n");
			}else {
				// Chequeamos rangos
				uint8_t diurno       = is_diurno(t.hours);
				RangoStatus rango    = check_rangos(DHD.temperature, DHD.humidity, diurno);
				Estado estado        = (rango == RANGO_OK) ? ESTADO_NORMAL : ESTADO_ALERTA;

				// Construimos y enviamos string de telemetria
		
				build_telemetry_string(buf, &t, DHD.temperature, DHD.humidity, estado);
				UART_send_string(buf);
				UART_send_string("\r\n");
				// Si hay alerta construimos y enviamos string de emergencia
				if (rango != RANGO_OK) {
					uint8_t valor = (rango == RANGO_TEMP_FUERA) ? DHD.temperature : DHD.humidity;
					build_alert_string(buf, &t, valor, rango, diurno);
					UART_send_string(buf);
					UART_send_string("\r\n");
				}
			}
		}
}