#ifndef MAIN_H
#define MAIN_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdlib.h>

#include "keypad.h"
#include "lcd.h"

//FRECUENCIA DE CPU
#ifndef F_CPU
#define F_CPU 16000000UL
#endif

//PINES DE LEDs
#define LED_MAG_DDR   DDRB
#define LED_MAG_PORT  PORTB
#define LED_MAG_PIN   PB5   // Rojo - Magnetrón

#define LED_LUZ_DDR   DDRC
#define LED_LUZ_PORT  PORTC
#define LED_LUZ_PIN   PC4   // Amarillo - Luz interior

#define LED_ALR_DDR   DDRC
#define LED_ALR_PORT  PORTC
#define LED_ALR_PIN   PC5   // Azul - Alarma fin (parpadeante)


// CONSTANTES DE TIEMPO
#define TIEMPO_MAX_SEG   5999u   // 99:59 es el máximo tiempo permitido para ingresar, equivale a 5999 segundos
#define TIEMPO_MAS30     30u     // segundos que suma la tecla "C" 
#define DURACION_BLINK   5u      // segundos que dura el parpadeo al finalizar la cocción

// ESTADOS DE LA MEF (Máquina de Moore)
typedef enum {REPOSO = 0, INGRESO, COCINANDO, PAUSA, PUERTA_ABIERTA, FIN} estado_t;

// ESTRUCTURA DE CONTEXTO GLOBAL
// En vez de trabajar con 10 variables globales, las agrupamos en una estructura
typedef struct {
    estado_t estado;            // Estado actual de la MEF              

    uint16_t tiempo_seg;        // Tiempo restante en segundos de la cocción        

    uint8_t  digitos[4];        // Buffer de digitos ingresados (MM:SS)
    uint8_t  n_digitos;         // Cantidad de dígitos ingresados (0-4) 

    uint8_t  puerta_abierta;    // 1 = puerta abierta, 0 = cerrada      

    uint8_t  tick_1seg;         // Flag: Timer1 disparo 1 segundo --> vamos mostrando en el display cuando va pasando el tiempo       
    uint16_t blink_contador;    // Contador de segundos en estado FIN (para el parpadeo del display y de la luz alarma)
    uint8_t  blink_display;     // Flag: toggle display en FIN          
    uint16_t blink_ms;          // Contador de ms para toggle display   

    uint8_t  lcd_actualizar;    // Flag: redibujar LCD                  
} contexto_t;


// VARIABLE GLOBAL con la que manejamos todo el contexto de la mef
extern volatile contexto_t ctx;

// PROTOTIPOS

// Inicialización 
void sistema_init(void);
void timer1_init(void);

// MEF 
void mef_ejecutar(uint8_t tecla);

// Acciones de entrada a cada estado (configuro las salidas Moore)
void accion_reposo(void);
void accion_ingreso(void);
void accion_cocinando(void);
void accion_pausa(void);
void accion_puerta_abierta(void);
void accion_fin(void);

// Helpers de tiempo
void tiempo_reset(void);			// se resetea el tiempo a 0, y vaciamos el buffer de dígitos
void tiempo_agregar(uint16_t seg);	// sumo "seg" al tiempo actual, la función tiene control de overflow para que no se pase de 99:59
uint8_t tiempo_valido(void);        // retorna 1 si tiempo_seg > 0 (es para que no se pase al estado COCINANDO si tiempo=0)
void digitos_a_tiempo(void);        // se convierten los digitos del buffer (ctx.digitos[4]) a tiempo en segundos 

// Helpers de LED
void leds_apagar_todos(void);

#endif /* MAIN_H */