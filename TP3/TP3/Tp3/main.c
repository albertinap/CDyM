/*
 * main.c
 *
 * Created: 22/6/2026 17:10:22
 * Authors : Ignacio Mucci Bigliani y Albertina Pezzutti
 */ 

#include <avr/sleep.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "drivers/I2C.h"
#include "drivers/uart.h"
#include "drivers/timer.h"
#include "invernadero.h"


int main(void)
{

	UART_init();
	I2C_init();
	//RTC_init();
	//DHT11_init();
	TIMER1_init();
	invernadero_init();
	sei(); // habilitar interrupciones

	while(1){
		invernadero_tarea_uart();
		invernadero_tarea();
		sleep_mode();
	}
}

