/*
 * TP2.c
 *
 * Created: 20/4/2026 15:24:53
 * Author : Ignacio Mucci y Albertina Pezzutti
 */ 

#include <avr/io.h>
#include "lcd.h"
#include "keypad.h"

// Estados posibles del sistema
typedef enum {REPOSO,INGRESO,COCINANDO,PAUSA,PUERTA_ABIERTA,FIN} estado;

// Tiempo de duración de la luz alarma parpadeante (en segundos)
#define ALARMA 5

// ------ variables privadas----------------------------------
static estado estado_actual; // Estado actual del sistema
static long tiempo_en_estado; // Tiempo en el estado


//LO PRIMERO QUE HAY QUE HACER DESP DE INICIALIZAR ES PONER EL CURSOR AL PRINICPIO DE LA MEMORIA
int main(void)
{
    LCDinit();
	LCDGotoXY(0,0);	//posicionamos el cursor al principio del display
	LCDstring("HOLIIS!!",8);
	/* Replace with your application code */
    while (1) 
    {
		
    }
}

/*void Actualizar_Estado(void)
{
	switch (estado)
	{
		case :
		salidas
		entrada
		Red_light = ON;
		Amber_light = OFF;
		Green_light = OFF;
		if(++Time_in_state == RED_DURATION)
		{
			Light_state = RED_AMBER;
			Time_in_state = 0;
		}
		break;
		case RED_AMBER:
		Red_light = ON;
		Amber_light = ON;
		Green_light = OFF;
		if (++Time_in_state == RED_AMBER_DURATION)
		{
			Light_state = GREEN;
			Time_in_state = 0;
		}
		break;
		(Moore)
		Cambio
		estado
		case GREEN:
		Red_light = OFF;
		Amber_light = OFF;
		Green_light = ON;
		if (++Time_in_state == GREEN_DURATION)
		{
			Light_state = AMBER;
			Time_in_state = 0;
		}
		break;
		case AMBER:
		Red_light = OFF;
		Amber_light = ON;
		Green_light = OFF;
		if (++Time_in_state == AMBER_DURATION)
		{
			Light_state = RED;
			Time_in_state = 0;
		}
		break;
	}
}

*/