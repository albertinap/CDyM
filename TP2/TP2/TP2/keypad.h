#ifndef KEYPAD_H
#define KEYPAD_H

#include <avr/io.h>
#ifndef F_CPU
#define F_CPU 16000000UL //Frecuencia de la CPU
#endif
#include <util/delay.h>
#include <stdint.h>

// Filas - salidas, para cada uno de los pines defino que puerto es, que data tengo que cambiar y cual pin es, asi es mas facil cambiarlo en el futuro
// Con solo cambiar una variable aca no es necesario cambiar el .c 
#define ROW0_PORT PORTB
#define ROW0_DDR  DDRB
#define ROW0_PIN  PB4

#define ROW1_PORT PORTB
#define ROW1_DDR  DDRB
#define ROW1_PIN  PB3

#define ROW2_PORT PORTB
#define ROW2_DDR  DDRB
#define ROW2_PIN  PB0

#define ROW3_PORT PORTD
#define ROW3_DDR  DDRD
#define ROW3_PIN  PD7

// Columnas - entradas con pull-up
//Vuelvo a hacer lo mismo con las columnas asi es mas facil para poder cambiarlo y que funcione directamente
#define COL0_PIN_REG PIND
#define COL0_DDR     DDRD
#define COL0_PORT    PORTD
#define COL0_PIN     PD3

#define COL1_PIN_REG PIND
#define COL1_DDR     DDRD
#define COL1_PORT    PORTD
#define COL1_PIN     PD5

#define COL2_PIN_REG PIND
#define COL2_DDR     DDRD
#define COL2_PORT    PORTD
#define COL2_PIN     PD4

#define COL3_PIN_REG PIND
#define COL3_DDR     DDRD
#define COL3_PORT    PORTD
#define COL3_PIN     PD2

#define KEY_NONE 0xFF

//Defino las funciones del keypad
void keypad_init(void);
uint8_t KEYPAD_Scan(uint8_t *pkey);

#endif