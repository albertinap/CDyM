/*
 * MEF.h
 *
 * Created: 29/7/2026 18:59:52
 *  Author: rocio
 */ 

#ifndef MEF_H_
#define MEF_H_

#include <stdint.h>

typedef enum // Estados de la mef, para ver en que momento debo cambiar el brillo y si esta apagado o no
{
	ST_OFF,
	ST_FADE_IN,
	ST_ON,
	ST_FADE_OUT

}MEF_State_t;

void MEF_Init(void); // Inicializacion de lasvariables de la mef
void MEF_Update(void); // Cambio de estados de la mef

uint8_t MEF_GetBrightness(void); // Funcion para obtener el brillo dependiendo del LDR

#endif