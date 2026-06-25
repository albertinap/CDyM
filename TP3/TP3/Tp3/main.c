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
#include "app/invernadero.h"
#include "app/comandos.h"


int main(void)
{

	UART_init();
	I2C_init();
	TIMER1_init();
	invernadero_init();
	comandos_init();
	sei(); // habilitar interrupciones

	while(1){
		comandos_tarea();
		invernadero_tarea();
		sleep_mode();
	}
}

