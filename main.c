#define F_CPU 16000000UL
#define CANT_LEDS      8

#include <avr/io.h>
#include <util/delay.h>

// Variables globales
uint8_t  pos_leds	    = 0;
int8_t   dir_leds	    = -1;
uint8_t  secuencia_leds = 0;
static uint8_t seq_neo = 0;
static uint8_t contador = 0;
static uint8_t cont_10 = 0;
static uint8_t neo_idx = 0;
static uint8_t fase_c = 0;
uint8_t neo_colores[8][3];

void leer_botones(void);
void tarea_neopixel(void);
void mostrar_neo(void);
static inline void mandar_byte(uint8_t byte);
void secuencia_a(void);
void secuencia_b(void);

int main(void)
{
	  DDRD  = 0xFF;   // Puerto D: todos salidas (LEDs)
	  PORTD = 0x00;   // LEDs apagados al inicio
    DDRB  |=  (1 << PORTB0);
    DDRC  &= ~((1 << PORTC0) | (1 << PORTC1));
    PORTC |=  ((1 << PORTC) | (1 << PORTC1));
    while (1)
    {
        leer_botones();
		if (secuencia_leds == 0)
		secuencia_a();
		else
		secuencia_b();

        if (contador >= 150)      
        {
            tarea_neopixel();
			contador = 0;
        }else{
			contador += 10;
		}
		cont_10 ++;
        _delay_ms(10);            
    }
}

void leer_botones(void)
{
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

    if (!(PINC & (1 << PORTC1)))
    {
        if (!btn1_presionado)
        {
            seq_neo ^= 1;
            btn1_presionado = 1;
        }
    }
    else
        btn1_presionado = 0;
}

void secuencia_a(void)
{
	if (cont_10 < 10) return;
	cont_10=0;

	PORTD = (1 << pos_leds);

	pos_leds++;
	if (pos_leds >= CANT_LEDS) pos_leds = 0;
}

// SECUENCIA B
void secuencia_b(void)
{
	if (cont_10 < 10) return;
	cont_10 = 0;

	PORTD = (1 << pos_leds);
	pos_leds += dir_leds;

	if (pos_leds >= CANT_LEDS - 1) dir_leds = -1;
	if (pos_leds == 0)             dir_leds =  1;
}




void tarea_neopixel(void)
{
	// Limpiar buffer
	for (uint8_t i = 0; i < 8; i++)
	neo_colores[i][0] = neo_colores[i][1] = neo_colores[i][2] = 0;

	if (seq_neo == 0)   // SECUENCIA C
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
		// Verde, un LED a la vez, derecha a izquierda (7→0)
		neo_colores[7 - neo_idx][1] = 255;
		neo_idx = (neo_idx + 1) % 8;
	}

	mostrar_neo();
}

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


void mostrar_neo(void)
{
	for (uint8_t i = 0; i < 8; i++)
	{
		mandar_byte(neo_colores[i][1]);  // G
		mandar_byte(neo_colores[i][0]);  // R
		mandar_byte(neo_colores[i][2]);  // B
	}
}