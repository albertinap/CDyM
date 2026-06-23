/*
 * Tp3.c
 *
 * Created: 22/6/2026 17:10:22
 * Author : Ignacio Mucci Bigliani
 */ 
#include <avr/interrupt.h>
#include "drivers/uart.h"
#include "drivers/I2C.h"
#include "drivers/rtc.h"

int main(void)
{

	UART_init();
	I2C_init();
	//RTC_init();	
	TIMER1_init();

	sei(); // habilitar interrupciones

	while(1){
		invernadero_tarea();
	}

}

