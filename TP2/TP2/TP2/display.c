/*
 * display.c
 *
 * Created: 15/5/2026 22:08:18
 *  Author: Mucci y Pezzutti
 */ 
#include "display.h"

static void escribir_tiempo(uint8_t min, uint8_t seg){
	// Funcion general para cambiar el dispay del tiempo de la fila 1, estatica ya que solo funciona dentro de display
	LCDGotoXY(0,0); // reseteo la fila
	LCDsendChar('0' + min / 10); //decena de min
	LCDsendChar('0' + min % 10); // unidad de min
	LCDsendChar(':');
	LCDsendChar('0' + seg / 10); // decena de seg
	LCDsendChar('0' + seg % 10); // unidad de seg
}


void lcd_display_init(){
	LCDinit();
}


void lcd_display_reposo(){
	LCDclr(); // Limpio pantalla
	LCDGotoXY(0,0); // reinicio donde arranca a escribir
	LCDsendChar((uint8_t*)"00:00", 5); // escribo el valor del tiempo; casteo asi no me tira warning de diferente tipo ( puedo hacer un vector para el mensaje pero no quiero usar mas RAM)
	LCDGotoXY(0,1); // Voy a la segunda linea asi escribo el segundo mensaje
	LCDsendChar((uint8_t*) "Ingrese tiempo  ", 16); // escribo segunda linea, ocupo toda la linea con dos espacios mas al final
}

void lcd_display_fin(){
	LCDclr(); // Limpio pantalla
	LCDGotoXY(0,0); // reinicio donde arranca a escribir
	LCDsendChar((uint8_t*)"00:00", 5); // escribo el valor del tiempo; casteo asi no me tira warning de diferente tipo
	LCDGotoXY(0,1); // Voy a la segunda linea asi escribo el segundo mensaje
	LCDsendChar((uint8_t*) "LISTO!", 8); // escribo segunda linea
	
}



void lcd_display_ingreso(uint8_t min, uint8_t seg){
	LCDGotoXY(0,0); // reinicio donde arranca a escribir
	escribir_tiempo(min,seg);// escribo el valor del tiempo;
	LCDGotoXY(0,1); // Voy a la segunda linea asi escribo el segundo mensaje
	LCDsendChar((uint8_t*) "A=OK  B=BORRAR  ", 16); // escribo segunda linea, ocupo toda la linea con dos espacios mas al final
}


void lcd_display_puerta(uint8_t min, uint8_t seg){
	LCDGotoXY(0,0); // reinicio donde arranca a escribir
	escribir_tiempo(min,seg);// escribo el valor del tiempo;
	LCDGotoXY(0,1);// Voy a la segunda linea asi escribo el segundo mensaje
	LCDsendChar((uint8_t*) "PUERTA  ABIERTA ", 16); // escribo segunda linea, ocupo toda la linea 
}

void lcd_display_cocinando(uint8_t min, uint8_t seg ){
	LCDGotoXY(0,0); // reinicio donde arranca a escribir
	escribir_tiempo(min,seg);// escribo el valor del tiempo;
	LCDGotoXY(0,1);// Voy a la segunda linea asi escribo el segundo mensaje
	LCDsendChar((uint8_t*) "Cocinando...    ", 16); // escribo segunda linea, ocupo toda la linea
}

void lcd_display_pausa(uint8_t min, uint8_t seg){
	LCDGotoXY(0,0); // reinicio donde arranca a escribir
	escribir_tiempo(min,seg);// escribo el valor del tiempo;
	LCDGotoXY(0,1);// Voy a la segunda linea asi escribo el segundo mensaje
	LCDsendChar((uint8_t*) "PAUSA B=CANCELAR", 16); // escribo segunda linea, ocupo toda la linea
}

