/*
 * TP4.c
 *
 * Created: 29/7/2026 18:13:34
 * Author : alber
 */ 
#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include "drivers/adc/adc.h"
#include "drivers/Mef/MEF.h"
#include "drivers/pwm/pwm.h"
#include "drivers/rgb/rgb.h"
#include "drivers/uart/uart.h"
#include "drivers/uart/comandos.h"


int main(void)
{
	MEF_Init();
	ADC_Init();
	PWM_init();
	UART_init();
	RGB_init();
	comandos_init();
	sei();
    while(1){
		comandos_tarea();
		
		MEF_task();
		
		RGB_task();
				
		PWM_task();

		sleep_mode();
	}

}