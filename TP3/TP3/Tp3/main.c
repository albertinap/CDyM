/*
 * main.c
 * Punto de entrada. Inicializa todos los periféricos y ejecuta el super loop
 * con arquitectura background/foreground
 * Authors : Ignacio Mucci Bigliani y Albertina Pezzutti
 */ 

#define F_CPU 16000000UL
#include <avr/sleep.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "drivers/I2C.h"
#include "drivers/uart.h"
#include "drivers/timer.h"
#include "drivers/dht11.h"
#include "app/invernadero.h"
#include "app/comandos.h"

#define F_CPU 16000000UL

int main(void){

	UART_init();		// UART0 a 9600bps 8N1, con interrupciones de RX y TX
	I2C_init();			// I2C para comunicación con el RTC DS3232
	TIMER1_init();		// Timer1 en modo CTC, tick cada 100ms
	invernadero_init();	// estado inicial del monitor (período de reporte inicializado en 10s)
	comandos_init();	// índice del buffer de comandos arranca en 0
	sei();				// habilitar interrupciones globales

	// Super loop: las ISRs (foreground) manejan UART y Timer;
	// las tareas (background) procesan los datos cuando el MCU está despierto

	while(1){
		comandos_tarea();		// procesa comandos recibidos por UART
		invernadero_tarea();	// envía telemetría si venció el período de reporte
		sleep_mode();			// duerme hasta la próxima interrupción (Timer1 o UART RX)
	}
}

