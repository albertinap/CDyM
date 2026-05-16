/*
 * TP2.c
 *
 * Created: 20/4/2026 15:24:53
 * Author : Ignacio Mucci y Albertina Pezzutti
 */ 

#include "main.h"
#include "display.h"

// VARIABLE GLOBAL DE CONTEXTO 
// Agrupamos todas las variables que maneja la mef en una sola estructura de contexto
volatile contexto_t ctx;

// INICIALIZACIÓN
void sistema_init(void)
{
    // configurar LEDs como salidas, apagados inicialmente
    LED_MAG_DDR  |=  (1 << LED_MAG_PIN);
    LED_LUZ_DDR  |=  (1 << LED_LUZ_PIN);
    LED_ALR_DDR  |=  (1 << LED_ALR_PIN);
    leds_apagar_todos();	// apaga los 3 leds

    // Inicializamos LCD y limpiamos display
    LCDinit();
    LCDclr();

    // Inicializamos Keypad
    keypad_init();

    // Inicializamos contexto inicial
    ctx.estado         = REPOSO;	// La MEF arranca en el estado inicial
    ctx.tiempo_seg     = 0;			// El temporizador parte desde cero (display muestra 00:00)
    ctx.n_digitos      = 0;			// No hay ningún dígito ingresado por el usuario todavía
    ctx.puerta_abierta = 0;			// La puerta arranca cerrada (0 = cerrada, 1 = abierta)
    ctx.tick_1seg      = 0;			// El flag del Timer1 arranca en 0, se pone en 1 cada vez
									// que el timer dispara una interrupción de 1 segundo
    ctx.blink_contador = 0;			// Cuenta cuántos segundos llevamos parpadeando en estado FIN
									// Cuando llega a DURACION_BLINK (5seg), volvemos a REPOSO
    ctx.blink_display  = 0;			// Indica si el display está visible u oculto durante el parpadeo
								    // 0 = visible, 1 = oculto --> se alterna cada 1 segundo en FIN    
    ctx.lcd_actualizar = 1;			// Arranca en 1 para forzar el primer dibujado del LCD apenas entre al loop principal

    timer1_init();					// Configura el Timer1 para generar interrupciones cada 1 segundo
									// A partir de acá el sistema empieza a contar el tiempo
									
    sei();		// Habilita las interrupciones globales del AVR	
				// (sin esta línea el Timer1 nunca dispararía su ISR)
}

/*
 * Timer1 en modo CTC: interrupción cada 1 segundo. (Clear Timer on Compare Match)
 * F_CPU = 16 MHz, prescaler = 1024 ? tick cada 1/16000000*1024 = 64 us
 * OCR1A = (16000000 / 1024) - 1 = 15624 (contamos hasta 15624 y ahí disparamos la interrupción)
 * Tenemos 15624 pulsos por segundo con el prescaler, en vez de tener 16000000
 */
void timer1_init(void)
{
    TCCR1A = 0;
    TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10); // modo CTC, prescaler 1024
    OCR1A  = 15624;			// cuando TCNT1 coincide con este valor en OCR1A se dispara la interrupción
    TIMSK1 = (1 << OCIE1A);	// habilitamos la interrupción
}

// ISR (Interrupt Service Routine) Timer1: rutina que se dispara cada 1 segundo
ISR(TIMER1_COMPA_vect)
{
    ctx.tick_1seg = 1;	// levantamos el flag de que pasó 1seg, para que se descuente un segundo
						// del temporizador de la etapa de cocción
}

// HELPERS - LEDs
void leds_apagar_todos(void)
{
    LED_MAG_PORT &= ~(1 << LED_MAG_PIN);	// apagamos los led que ya estaban configurados como salidas
    LED_LUZ_PORT &= ~(1 << LED_LUZ_PIN);
    LED_ALR_PORT &= ~(1 << LED_ALR_PIN);
}

// HELPERS - TIEMPO
void tiempo_reset(void)
{
    ctx.tiempo_seg = 0;	// reiniciamos el temporizador
    ctx.n_digitos  = 0;	// limpiamos el buffer de dígitos ingresados por el usuario
    ctx.digitos[0] = ctx.digitos[1] = ctx.digitos[2] = ctx.digitos[3] = 0;
}

void tiempo_agregar(uint16_t seg)
{
    if ((ctx.tiempo_seg + seg) > TIEMPO_MAX_SEG)	// si el usuario agrega un tiempo mayor al permitido lo acotamos
        ctx.tiempo_seg = TIEMPO_MAX_SEG;			
    else
        ctx.tiempo_seg += seg;						// si es un tiempo válido, lo agregamos
}

uint8_t tiempo_valido(void)							// retorna 1 si es un tiempo mayor a cero, para saber si hay que cocinar o no
{
    return (ctx.tiempo_seg > 0) ? 1 : 0;
}

uint8_t tiempo_formato_valido(void)					// Retorna 1 si el formato es válido (ss <= 59 y tiempo <= 99:59), sólo se chequea al presionar START
{
	uint8_t ss = ctx.digitos[2] * 10 + ctx.digitos[3];
	if (ss > 59)                          return 0;
	if (ctx.tiempo_seg > TIEMPO_MAX_SEG) return 0;
	return 1;
}

/*
 * Convierte el buffer de 4 digitos a tiempo_seg.
 * Formato: digitos[0..1] = MM, digitos[2..3] = SS
 * Ejemplo: [0,1,3,0] -> 01:30 -> 90 seg
 */
void digitos_a_tiempo(void)
{
    uint8_t mm = ctx.digitos[0] * 10 + ctx.digitos[1];
    uint8_t ss = ctx.digitos[2] * 10 + ctx.digitos[3];
    ctx.tiempo_seg = (uint16_t)mm * 60 + ss;		
}

// ACCIONES DE ESTADO (salidas Moore) - Se ejecutan UNA VEZ al entrar al estado.
void accion_reposo(void) {
	leds_apagar_todos();	// Apagamos todos los leds, estamos en reposo
	tiempo_reset();			// Reiniciamos el temporizador
	lcd_display_reposo();
}

void accion_ingreso(void) {
	leds_apagar_todos();				// Apagamos todos los leds
	lcd_display_ingreso(ctx.digitos[0],	// Actualizamos el display con el tiempo actual
		ctx.digitos[1],ctx.digitos[2],ctx.digitos[3]);		
}

void accion_cocinando(void) {
	LED_MAG_PORT |= (1 << LED_MAG_PIN);	// Prendemos el led magnetron
	LED_LUZ_PORT |= (1 << LED_LUZ_PIN);	// Prendemos el led de luz interior
	uint8_t mm = ctx.tiempo_seg / 60;	// Convertimos tiempo del contexto (en segundos) a tiempo en minutos y segundos 
	uint8_t ss = ctx.tiempo_seg % 60;
	lcd_display_cocinando(mm, ss);		// Actualizamos el display con tiempo actual y mensaje de cocinando
}

void accion_pausa(void) {
	LED_MAG_PORT &= ~(1 << LED_MAG_PIN);// Apagamos el led magnetron
	LED_LUZ_PORT |=  (1 << LED_LUZ_PIN);// Se mantiene encendida la luz interior
	uint8_t mm = ctx.tiempo_seg / 60;	
	uint8_t ss = ctx.tiempo_seg % 60;
	lcd_display_pausa(mm, ss);			// Actualizamos display con el mensaje de pausa
}

void accion_puerta_abierta(void) {
	LED_MAG_PORT &= ~(1 << LED_MAG_PIN);// Apagamos led magnetron
	LED_LUZ_PORT |=  (1 << LED_LUZ_PIN);// Luz interior se mantiene encendida
	uint8_t mm = ctx.tiempo_seg / 60;	
	uint8_t ss = ctx.tiempo_seg % 60;
	lcd_display_puerta(mm, ss);			// Actualizamos display con el mensaje de puerta abierta
}

void accion_fin(void) {
	leds_apagar_todos();				// Apagamos todos los leds
	ctx.blink_contador = 0;				// Inicializamos el contador de parpadeo en 0
	lcd_display_fin();					// Actualizamos display con mensaje de fin
}


// MEF PRINCIPAL
/*
 * Cambia de estado y ejecuta la accion de entrada.
 * Centralizar la transicion acá evita llamar
 * las acciones manualmente en cada caso.
 */
static void cambiar_estado(estado_t nuevo)
{
    ctx.estado = nuevo;
    switch (nuevo) {
        case REPOSO:         accion_reposo();         break;
        case INGRESO:        accion_ingreso();        break;
        case COCINANDO:      accion_cocinando();      break;
        case PAUSA:          accion_pausa();          break;
        case PUERTA_ABIERTA: accion_puerta_abierta(); break;
        case FIN:            accion_fin();            break;
    }
}

/*
 * Maneja cuándo hay que transicionar de un estado de la mef a otro
 * y cuándo actualizar variables del estado actual
 * Recibe la tecla actual (KEY_NONE si no hay tecla).
 */
void mef_ejecutar(uint8_t tecla)
{
    switch (ctx.estado)
    {
        // REPOSO 
        case REPOSO:
            if (tecla >= '0' && tecla <= '9') {
                if (ctx.puerta_abierta) {
	                // No permitir ingreso con puerta abierta
	                ctx.lcd_actualizar = 1;  // refresca mostrando aviso
	            } else {
					// Primer dígito: cargar buffer y pasar a INGRESO
					tiempo_reset();
					ctx.digitos[0] = ctx.digitos[1] = ctx.digitos[2] = 0;
					ctx.digitos[3] = tecla - '0';
					ctx.n_digitos  = 1;
					digitos_a_tiempo();
					cambiar_estado(INGRESO);
				}
            } else if (tecla == 'C') {
				if (ctx.puerta_abierta) {
					 ctx.lcd_actualizar = 1;
				} else {
					// MAS30: inicio rapido con 30 seg
					tiempo_reset();
					ctx.tiempo_seg = TIEMPO_MAS30;
					cambiar_estado(COCINANDO);
				}
			} else if (tecla == 'D') {
				// Toggle puerta
				ctx.puerta_abierta = !ctx.puerta_abierta;
				ctx.lcd_actualizar = 1;
			}
            break;

        // INGRESO
        case INGRESO:
            if (tecla >= '0' && tecla <= '9') {
                uint8_t nuevo_dig = tecla - '0';               

                // shift circular del buffer, se van ingresando dígitos y los desplazo a la izquierda
                ctx.digitos[0] = ctx.digitos[1];
                ctx.digitos[1] = ctx.digitos[2];
                ctx.digitos[2] = ctx.digitos[3];
                ctx.digitos[3] = nuevo_dig;
                if (ctx.n_digitos < 4) ctx.n_digitos++;
                                
                ctx.lcd_actualizar = 1;	// actualizamos el display

            } else if (tecla == 'A') {	
				 digitos_a_tiempo();				// Recién cuando se quiere cocinar convertimos los dígitos del display a tiempo en segundos			 
				 if (!tiempo_valido()) {					 
					 lcd_display_error();		    // tiempo es 00:00
					 ctx.lcd_actualizar = 1;		// Actualizar display para mostrar mensaje de error
				 } 
				 else if (!tiempo_formato_valido()) {					 
					 lcd_display_maximo();			// Tiempo fuera de rango, los segundos son mayores a 59
					 ctx.lcd_actualizar = 1;		// Actualizar display para mostrar mensaje de error
				 } 
				 else if (tecla == 'D') {
					 ctx.puerta_abierta = !ctx.puerta_abierta;	// Toggle puerta					 
					 ctx.lcd_actualizar = 1;					// Se informa en el display
				 }
				 else {					 
					 cambiar_estado(COCINANDO);		// El tiempo está bien, arrancar a cocinar
				 }
				                 
            } else if (tecla == 'B') {
                // CLEAR: volver a REPOSO
                cambiar_estado(REPOSO);
            } else if (tecla == 'C') {
                // MAS30 desde INGRESO: sumar y arrancar
                digitos_a_tiempo();
                tiempo_agregar(TIEMPO_MAS30);
                cambiar_estado(COCINANDO);
            }
            break;

        // COCINANDO
        case COCINANDO:
            if (tecla == 'B') {
                cambiar_estado(PAUSA);
            } else if (tecla == 'C') {
                tiempo_agregar(TIEMPO_MAS30);
                ctx.lcd_actualizar = 1;
            } else if (tecla == 'D') {
				ctx.puerta_abierta = 1;
                cambiar_estado(PUERTA_ABIERTA);
            }

            // Tick de 1 segundo: decrementar tiempo
            if (ctx.tick_1seg) {
                ctx.tick_1seg = 0;
                if (ctx.tiempo_seg > 0) {
                    ctx.tiempo_seg--;
                    ctx.lcd_actualizar = 1;
                }
                if (ctx.tiempo_seg == 0) {
                    cambiar_estado(FIN);
                }
            }
            break;

        // PAUSA 
        case PAUSA:
            if (tecla == 'A') {
                cambiar_estado(COCINANDO);
            } else if (tecla == 'B') {
                cambiar_estado(REPOSO);
            } else if (tecla == 'C') {
                tiempo_agregar(TIEMPO_MAS30);
                cambiar_estado(COCINANDO);
            } else if (tecla == 'D') {
				ctx.puerta_abierta = 1;
                cambiar_estado(PUERTA_ABIERTA);
            }
            break;

        // PUERTA_ABIERTA
        case PUERTA_ABIERTA:
            if (tecla == 'D') {
				ctx.puerta_abierta = 0;		// cerrar puerta                
                cambiar_estado(COCINANDO);	// reanudar coccion 
            } else if (tecla == 'B') {
				// Cancelar con puerta abierta 
				ctx.puerta_abierta = 1;  // sigue abierta al cancelar                
                cambiar_estado(REPOSO);
            }
            break;

        // FIN
        case FIN:
            // Parpadeo del display durante DURACION_BLINK segundos
            if (ctx.tick_1seg) {
                ctx.tick_1seg = 0;
                ctx.blink_contador++;

                // Toggle LED alarma
                LED_ALR_PORT ^= (1 << LED_ALR_PIN);

                if (ctx.blink_contador >= DURACION_BLINK) {
                    // BLINK_END: volver a REPOSO 
                    LED_ALR_PORT &= ~(1 << LED_ALR_PIN);
                    cambiar_estado(REPOSO);
                } else {
                    // Toggle display
                    if (ctx.blink_display) {
                        LCDblank();
                        ctx.blink_display = 0;
                    } else {
                        LCDvisible();
                        ctx.blink_display = 1;
                    }
                }
            }
            break;
    }
}

// MAIN
int main(void)
{
    sistema_init();

    // Mostrar pantalla inicial
    lcd_display_reposo();

    uint8_t tecla      = KEY_NONE;
    uint8_t tecla_ant  = KEY_NONE;

    while (1)
    {
        // Lectura de teclado (sin bloqueo)
        uint8_t raw = KEY_NONE;
        KEYPAD_Scan(&raw);

        // Detectar flanco de bajada (nueva presion)
        if (raw != KEY_NONE && raw != tecla_ant) {
            tecla = raw;
        } else {
            tecla = KEY_NONE;
        }
        tecla_ant = raw;

        // Ejecutar MEF
        mef_ejecutar(tecla);
        
        if (ctx.lcd_actualizar) {
			ctx.lcd_actualizar = 0;
			uint8_t mm = ctx.tiempo_seg / 60;
			uint8_t ss = ctx.tiempo_seg % 60;
			switch (ctx.estado) {
				case REPOSO:
					if (ctx.puerta_abierta) {
						lcd_display_puerta(0, 0);  // avisa que la puerta está abierta
						} else {
						lcd_display_reposo();
					}
					break;
				case INGRESO:            
					if (ctx.puerta_abierta) {
						uint8_t mm = ctx.digitos[0] * 10 + ctx.digitos[1];
						uint8_t ss = ctx.digitos[2] * 10 + ctx.digitos[3];
						lcd_display_puerta(mm,ss);
					} else {
						lcd_display_ingreso(ctx.digitos[0], ctx.digitos[1], ctx.digitos[2], ctx.digitos[3]);
					}
					break;
				case COCINANDO:
					lcd_display_cocinando(mm, ss);
					break;
				case PAUSA:
					lcd_display_pausa(mm, ss);
					break;
				case PUERTA_ABIERTA:
					lcd_display_puerta(mm, ss);
					break;
				default: break;
			}
		}

        _delay_ms(20); // Anti-rebote y cadencia del loop
    }

    return 0;
}