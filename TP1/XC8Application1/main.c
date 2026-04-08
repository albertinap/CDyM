/*
 * TP1
 * MCU: ATmega328P - 16MHz
 * Autores: Ignacio Mucci Bigliani y Albertina Pezzutti
 */

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

#define F_CPU 16000000UL
#define NOP asm volatile("nop")

// CONSTANTES GENERALES
#define CANT_LEDS      8
#define DELAY_LEDS_MS  100   // retardo secuencias LED en ms 
#define DELAY_NEO_MS   150   // retardo secuencias LED en ms  
#define DELAY_LOOP_MS  10    // tiempo de delay que agregamos al super loop   

// CONTADOR DE MILISEGUNDOS (para 100ms y 150ms)
uint8_t cont_100 = 0;
uint8_t cont_150 = 0;

// VARIABLES DE ESTADO DE SECUENCIAS
// LEDs Puerto D 
uint8_t  pos_leds	    = 0;
int8_t   dir_leds	    = -1;
uint8_t  secuencia_leds = 0;
// NEOPIXEL
static uint8_t secuencia_neo = 0;
static uint8_t neo_idx = 0;
static uint8_t fase_c = 0;
uint8_t neo_colores[8][3];


// PROTOTIPO DE FUNCIONES
void leer_botones(void);
void secuencia_a(void);
void secuencia_b(void);
void tarea_neopixel(void);
void mostrar_neo(void);
static inline void mandar_byte(uint8_t byte);


// PROGRAMA PRINCIPAL
int main(void)
{
    // CONFIGURACION DE PINES DE LED
    DDRD  = 0xFF;   // Puerto D: todos salidas (LEDs)  
    PORTD = 0x00;   // LEDs apagados al inicio         
	
	DDRB |= (1 << PORTB0);
	DDRC  &= ~((1 << PORTC0) | (1 << PORTC1));      // PC0 y PC1 = entradas (botones)
	PORTC |=  ((1 << PORTC0) | (1 << PORTC1));      // Pull-ups internos 	

    // SUPER LOOP 
    while (1)
    {                	
		leer_botones();

		if (secuencia_leds == 0) 
			secuencia_a();
		else					 
			secuencia_b();
			
		if (cont_150 >= DELAY_NEO_MS){
			 tarea_neopixel();
			 cont_150 = 0;
		}							
		
		_delay_ms(DELAY_LOOP_MS);
		cont_100 += DELAY_LOOP_MS;	
		cont_150 += DELAY_LOOP_MS;	
    }

    return 0;
}



// DECLARACIÓN DE FUNCIONES
void leer_botones(void) {
	static uint8_t btn0_presionado = 0;
	static uint8_t btn1_presionado = 0;

	if (!(PINC & (1 << PORTC0))) {
		if (!btn0_presionado) {
			secuencia_leds ^= 1;
			btn0_presionado = 1;
		}
		} else {
		btn0_presionado = 0;
	}

	if (!(PINC & (1 << PORTC1))) {
		if (!btn1_presionado) {
			secuencia_neo ^= 1;
			btn1_presionado = 1;
		}
		} else {
		btn1_presionado = 0;
	}
}

// SECUENCIA A
void secuencia_a(void)
{
	if (cont_100 < DELAY_LEDS_MS) return;
	cont_100=0;

	PORTD = (1 << pos_leds);

	pos_leds++;
	if (pos_leds >= CANT_LEDS) pos_leds = 0;
}

// SECUENCIA B
void secuencia_b(void)
{
	if (cont_100 < DELAY_LEDS_MS) return;
	cont_100 = 0;

	PORTD = (1 << pos_leds);
	pos_leds += dir_leds;

	if (pos_leds >= CANT_LEDS - 1) dir_leds = -1;
	if (pos_leds == 0)             dir_leds =  1;
}

// TODA LA LÓGICA DEL NEOPIXEL, LIMPIA BUFFER Y MANDA TODOS LOS COLORES
void tarea_neopixel(void)
{
	// Limpiar buffer
	for (uint8_t i = 0; i < 8; i++)
	neo_colores[i][0] = neo_colores[i][1] = neo_colores[i][2] = 0;

	if (secuencia_neo == 0)   // SECUENCIA C
	{
		if (fase_c == 0)
		{
			// LEDs pares (0,2,4,6) en rojo
			neo_colores[0][0] = 255;
			neo_colores[2][0] = 255;
			neo_colores[4][0] = 255;
			neo_colores[6][0] = 255;
			fase_c = 1;
		}
		else
		{
			// LEDs impares (1,3,5,7) en azul
			neo_colores[1][2] = 255;
			neo_colores[3][2] = 255;
			neo_colores[5][2] = 255;
			neo_colores[7][2] = 255;
			fase_c = 0;
		}
	}
	else                // SECUENCIA D
	{
		// Verde, un LED a la vez, derecha a izquierda (7?0)
		neo_colores[7 - neo_idx][1] = 255;
		neo_idx = (neo_idx + 1) % 8;
	}

	mostrar_neo();
}

// MANDA BYTE PARA EL NEOPIXEL
static inline void mandar_byte(uint8_t dato)
{
	for (int8_t i = 7; i >= 0; i--)
	{
		if ((dato >> i) & 1)
		{
			// BIT 1: HIGH largo, LOW corto
			PORTB |=  (1 << PB0);
			asm volatile("nop\n\t""nop\n\t""nop\n\t""nop\n\t"
			"nop\n\t""nop\n\t""nop\n\t""nop\n\t"
			"nop\n\t""nop\n\t""nop\n\t");
			PORTB &= ~(1 << PB0);
			asm volatile("nop\n\t""nop\n\t""nop\n\t""nop\n\t"
			"nop\n\t");
		}
		else
		{
			// BIT 0: HIGH corto, LOW largo
			PORTB |=  (1 << PB0);
			asm volatile("nop\n\t""nop\n\t""nop\n\t""nop\n\t");
			PORTB &= ~(1 << PB0);
			asm volatile("nop\n\t""nop\n\t""nop\n\t""nop\n\t"
			"nop\n\t""nop\n\t""nop\n\t""nop\n\t"
			"nop\n\t""nop\n\t""nop\n\t""nop\n\t");
		}
	}
}

// MANDA EL COLOR ENTERO AL PUERTO DEL NEOPIXEL
void mostrar_neo(void)
{
	for (uint8_t i = 0; i < 8; i++)
	{
		mandar_byte(neo_colores[i][1]);  // G
		mandar_byte(neo_colores[i][0]);  // R
		mandar_byte(neo_colores[i][2]);  // B
	}
}