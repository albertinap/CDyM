/*
 * display.h
 *
 * Created: 15/5/2026 20:22:34
 *  Author: Mucci
 */ 


#ifndef DISPLAY_H
#define DISPLAY_H


#include "<stdint.h>"
#include "lcd.h"

void lcd_display_init(void);
void lcd_display_reposo(void);// No entran valores ya es predefinido el valor del display como 00:00 en la linea 1 y linea dos "Ingrese tiempo"
void lcd_display_fin(void); // Valores predefinidos linea 1 = FIN, linea 2=Listo!
// Desde ahora  en adelante a las funciones entran dos valores uno para el valor de los minutos y otro para el valor de los segundos de la linea 1
void lcd_display_ingreso(uint8_t min, uint8_t seg); // linea 2 del diplay tiene A=OK B=BORRAR
void lcd_display_cocinando(uint8_t min, uint8_t seg ); // Linea 2 del display tiene Cocinando ..
void lcd_display_puerta(uint8_t min, uint8_t seg); // Linea 2 del display tiene PUERTA ABIERTA
void lcd_display_pausa(uint8_t min, uint8_t seg); // Linea 2 del display tiene PAUSA B=CANCELAR

#endif /* DISPLAY_H_ */