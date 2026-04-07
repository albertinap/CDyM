/*
 * TP1 - Sistema de visualizacion multitarea cooperativa
 * MCU: ATmega328P @ 16MHz
 *
 * Puerto D  -> 8 LEDs (secuencias a y b)
 * Puerto B0 -> NeoPixel WS2812x8 (secuencias c y d)
 * PORTC0    -> Boton cambia secuencia LEDs
 * PORTC1    -> Boton cambia secuencia NeoPixel
 *
 * Los botones tienen pull-down: reposo = 0, presionado = 1
 *
 * RESISTENCIAS PARA PROTEUS (VCC=5V, If=10mA):
 *   Rojo     Vf=1.8V  ->  R = (5 - 1.8) / 0.010 = 320 ohm  -> usar 330 ohm
 *   Verde    Vf=2.2V  ->  R = (5 - 2.2) / 0.010 = 280 ohm  -> usar 270 ohm
 *   Amarillo Vf=2.0V  ->  R = (5 - 2.0) / 0.010 = 300 ohm  -> usar 300 ohm
 *   Azul     Vf=3.0V  ->  R = (5 - 3.0) / 0.010 = 200 ohm  -> usar 200 ohm
 */

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

/* =========================================================
 * CONSTANTES GENERALES
 * ========================================================= */
#define CANT_LEDS       8
#define DELAY_LEDS_MS   100   /* retardo secuencias LED en ms  */
#define DELAY_NEO_MS    150   /* retardo secuencias Neo en ms  */

/* =========================================================
 * CONTADOR DE MILISEGUNDOS (sin timer de hardware)
 *
 * Cada vuelta del super loop dura exactamente 1ms porque
 * llamamos _delay_ms(1) al principio. Entonces solo
 * necesitamos contar cuantas vueltas pasaron.
 *
 * Usamos uint32_t para que no desborde rapido:
 * un uint32_t llega hasta ~49 dias de cuenta continua.
 * ========================================================= */
uint32_t ms = 0;   /* contador global de milisegundos */

/* =========================================================
 * DRIVER NEOPIXEL WS2812
 *
 * El WS2812 recibe datos por un unico cable en serie.
 * Cada LED necesita 3 bytes: Verde, Rojo, Azul (orden GRB).
 * Los bits se envian con pulsos de duracion muy precisa
 * (cientos de nanosegundos), por eso usamos codigo assembler
 * inline para controlar los ciclos exactos del procesador.
 *
 * A 16MHz, 1 ciclo = 62.5ns
 *   Bit 1: linea HIGH ~800ns, luego LOW ~450ns
 *   Bit 0: linea HIGH ~400ns, luego LOW ~850ns
 * ========================================================= */
#define NEO_PORT  PORTB
#define NEO_DDR   DDRB
#define NEO_PIN   PB0

/* Buffer interno: un byte G, R, B por cada LED */
uint8_t neo_g[CANT_LEDS];
uint8_t neo_r[CANT_LEDS];
uint8_t neo_b[CANT_LEDS];

/* Envia un byte al WS2812 bit a bit con timing preciso */
void neo_enviar_byte(uint8_t dato)
{
    for (uint8_t bit = 0; bit < 8; bit++) {
        if (dato & 0x80) {
            /* Bit 1 */
            __asm__ volatile(
                "sbi %[port], %[pin] \n\t"
                "nop \n\t nop \n\t nop \n\t nop \n\t"
                "nop \n\t nop \n\t nop \n\t nop \n\t"
                "nop \n\t nop \n\t nop \n\t"
                "cbi %[port], %[pin] \n\t"
                "nop \n\t nop \n\t nop \n\t nop \n\t"
                "nop \n\t nop \n\t"
                :: [port] "I"(_SFR_IO_ADDR(NEO_PORT)), [pin] "I"(NEO_PIN)
            );
        } else {
            /* Bit 0 */
            __asm__ volatile(
                "sbi %[port], %[pin] \n\t"
                "nop \n\t nop \n\t nop \n\t"
                "cbi %[port], %[pin] \n\t"
                "nop \n\t nop \n\t nop \n\t nop \n\t"
                "nop \n\t nop \n\t nop \n\t nop \n\t"
                "nop \n\t nop \n\t"
                :: [port] "I"(_SFR_IO_ADDR(NEO_PORT)), [pin] "I"(NEO_PIN)
            );
        }
        dato <<= 1;
    }
}

/* Manda el buffer completo a la tira NeoPixel */
void neo_mostrar(void)
{
    uint8_t sreg = SREG;
    __asm__ volatile("cli");

    for (uint8_t i = 0; i < CANT_LEDS; i++) {
        neo_enviar_byte(neo_g[i]);
        neo_enviar_byte(neo_r[i]);
        neo_enviar_byte(neo_b[i]);
    }

    SREG = sreg;

    NEO_PORT &= ~(1 << NEO_PIN);
    _delay_us(60);
}

/* Apaga todos los NeoPixels */
void neo_apagar_todos(void)
{
    for (uint8_t i = 0; i < CANT_LEDS; i++) {
        neo_g[i] = neo_r[i] = neo_b[i] = 0;
    }
    neo_mostrar();
}

/* Enciende un NeoPixel con un color */
void neo_set_color(uint8_t idx, uint8_t r, uint8_t g, uint8_t b)
{
    neo_r[idx] = r;
    neo_g[idx] = g;
    neo_b[idx] = b;
}

/* =========================================================
 * VARIABLES DE ESTADO DE SECUENCIAS
 * ========================================================= */

/* LEDs Puerto D */
uint8_t  seq_leds       = 0;
uint8_t  pos_leds       = 0;
int8_t   dir_leds       = 1;
uint32_t ultimo_leds_ms = 0;

/* NeoPixels */
uint8_t  seq_neo       = 0;
uint8_t  paso_neo      = 0;
uint32_t ultimo_neo_ms = 0;

/* Botones: guardamos el estado anterior para detectar flancos */
uint8_t btn0_anterior = 0;
uint8_t btn1_anterior = 0;

/* =========================================================
 * FUNCION: DETECTAR PRESION DE BOTON
 *
 * Detecta el momento exacto en que se presiona el boton
 * (flanco ascendente: pasa de 0 a 1).
 * Retorna 1 si se acaba de presionar, 0 si no.
 *
 * Por que no simplemente "if (PINC & (1<<PC0))"?
 * Porque eso retornaria 1 durante TODO el tiempo que
 * esta apretado, no solo cuando se presiona.
 * ========================================================= */
uint8_t boton_presionado(uint8_t pin, uint8_t *anterior)
{
    uint8_t actual = (PINC >> pin) & 1;
    uint8_t presionado = 0;

    if (actual == 1 && *anterior == 0) {
        presionado = 1;
    }

    *anterior = actual;
    return presionado;
}

/* =========================================================
 * TAREA: SECUENCIA A
 * Un LED a la vez, de LED0 a LED7, circular.
 * ========================================================= */
void tarea_seq_a(void)
{
    if ((ms - ultimo_leds_ms) < DELAY_LEDS_MS) return;
    ultimo_leds_ms = ms;

    PORTD = (1 << pos_leds);

    pos_leds++;
    if (pos_leds >= 8) pos_leds = 0;
}

/* =========================================================
 * TAREA: SECUENCIA B
 * Un LED a la vez, rebota entre LED7 y LED0.
 *   LED7->LED6->...->LED0->LED1->...->LED7->...
 * ========================================================= */
void tarea_seq_b(void)
{
    if ((ms - ultimo_leds_ms) < DELAY_LEDS_MS) return;
    ultimo_leds_ms = ms;

    PORTD = (1 << pos_leds);

    pos_leds += dir_leds;

    if (pos_leds >= 7) dir_leds = -1;
    if (pos_leds == 0) dir_leds =  1;
}

/* =========================================================
 * TAREA: SECUENCIA C
 * Alterna: LEDs pares en rojo / LEDs impares en azul.
 * ========================================================= */
void tarea_seq_c(void)
{
    if ((ms - ultimo_neo_ms) < DELAY_NEO_MS) return;
    ultimo_neo_ms = ms;

    neo_apagar_todos();

    if (paso_neo == 0) {
        for (uint8_t i = 0; i < CANT_LEDS; i += 2)
            neo_set_color(i, 150, 0, 0);   /* rojo */
    } else {
        for (uint8_t i = 1; i < CANT_LEDS; i += 2)
            neo_set_color(i, 0, 0, 150);   /* azul */
    }

    neo_mostrar();
    paso_neo ^= 1;
}

/* =========================================================
 * TAREA: SECUENCIA D
 * Un LED verde a la vez, de LED7 a LED0, circular.
 * ========================================================= */
void tarea_seq_d(void)
{
    if ((ms - ultimo_neo_ms) < DELAY_NEO_MS) return;
    ultimo_neo_ms = ms;

    neo_apagar_todos();

    uint8_t idx = 7 - paso_neo;   /* 7,6,5,4,3,2,1,0 */
    neo_set_color(idx, 0, 150, 0);   /* verde */
    neo_mostrar();

    paso_neo++;
    if (paso_neo >= CANT_LEDS) paso_neo = 0;
}

/* =========================================================
 * PROGRAMA PRINCIPAL
 * ========================================================= */
int main(void)
{
    /* CONFIGURACION DE PINES */
    DDRD  = 0xFF;            /* Puerto D: todos salidas (LEDs)   */
    PORTD = 0x00;            /* LEDs apagados al inicio          */

    DDRB = (1 << NEO_PIN);  /* PB0 salida (NeoPixel data)       */
    NEO_PORT &= ~(1 << NEO_PIN);

    DDRC &= ~((1 << PC0) | (1 << PC1));  /* PC0 y PC1 entradas  */
    /* NO activamos pull-up porque los botones tienen pull-down   */

    neo_apagar_todos();

    /* Secuencia b arranca desde LED7 bajando */
    pos_leds = 7;
    dir_leds = -1;

    /* SUPER LOOP */
    while (1)
    {
        /* 1. Cada vuelta dura 1ms -> ms cuenta milisegundos reales */
        _delay_ms(1);
        ms++;

        /* 2. Leer botones */
        if (boton_presionado(PC0, &btn0_anterior)) {
            seq_leds ^= 1;
            if (seq_leds == 0) { pos_leds = 0; }
            else               { pos_leds = 7; dir_leds = -1; }
            ultimo_leds_ms = ms;
        }

        if (boton_presionado(PC1, &btn1_anterior)) {
            seq_neo ^= 1;
            paso_neo = 0;
            ultimo_neo_ms = ms;
        }

        /* 3. Ejecutar tarea de LEDs */
        if (seq_leds == 0) tarea_seq_a();
        else               tarea_seq_b();

        /* 4. Ejecutar tarea de NeoPixels */
        if (seq_neo == 0) tarea_seq_c();
        else              tarea_seq_d();
    }

    return 0;
}