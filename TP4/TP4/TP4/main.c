/*
 * TP4.c
 *
 * Created: 29/7/2026 18:13:34
 * Author : alber
 */ 

#include <avr/io.h>


int main(void)
{
    while(1){
	    if(flag_tick){
		    flag_tick=0;

		    RGB_update();

		    ADC_task();

		    Fade_task();
	    }

	    comandos_tarea();
    }
}

