/*
 * TP2.c
 *
 * Created: 20/4/2026 15:24:53d
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
    ctx.estado			= REPOSO;	// La MEF arranca en el estado inicial
    ctx.tiempo_seg		= 0;		// El temporizador parte desde cero (display muestra 00:00)
	ctx.digitos[0]		= 0;		// Inicializamos el buffer con 0
	ctx.digitos[1]		= 0;
	ctx.digitos[2]		= 0;
	ctx.digitos[3]		= 0;				
    ctx.n_digitos       = 0;		// No hay ningún dígito ingresado por el usuario todavía
    ctx.puerta_abierta  = 0;		// La puerta arranca cerrada (0 = cerrada, 1 = abierta)
    ctx.tick_timer1     = 0;		// El flag del Timer1 arranca en 0, se pone en 1 cada vez
									// que el timer dispara una interrupción de 1 segundo
    ctx.blink_display   = 0;		// Indica si el display está visible u oculto durante el parpadeo
								    // 0 = visible, 1 = oculto --> se alterna cada 1 segundo en FIN  
	ctx.blink_contador	= 0;		// Cuenta ticks en estado FIN para el parpadeo			
	ctx.cont_1seg		= 0;		// cuenta ticks de 100ms hasta llegar a 10 = 1 segundo						  
    ctx.lcd_actualizar  = 1;		// Arranca en 1 para forzar el primer dibujado del LCD apenas entre al loop principal

    timer1_init();					// Configura el Timer1 para generar interrupciones cada 100ms
									// A partir de acá el sistema empieza a contar el tiempo
									
    sei();		// Habilita las interrupciones globales del AVR	
				// (sin esta línea el Timer1 nunca dispararía su ISR)
}

/*
 * Timer1 en modo CTC: interrupción cada 100ms. (Clear Timer on Compare Match)
 * F_CPU = 16 MHz, prescaler = 256, 1 tick cada 1/16000000*256 = 16 us
 * OCR1A = (16000000 / 256 / 10) - 1 = 6249 (contamos hasta 6249=100ms y ahí disparamos la interrupción)
 */
void timer1_init(void)
{
    TCCR1A = 0;
    TCCR1B = (1 << WGM12) | (1 << CS12);	// modo CTC, prescaler 256
    OCR1A  = OCR1A_100MS;					// cuando TCNT1 coincide con este valor en OCR1A se dispara la interrupción
    TIMSK1 = (1 << OCIE1A);					// habilitamos la interrupción
}

// ISR (Interrupt Service Routine) Timer1: rutina que se dispara cada 100ms
ISR(TIMER1_COMPA_vect)
{
    ctx.tick_timer1 = 1;	// levantamos el flag de que pasaron 100ms
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

uint8_t tiempo_formato_valido(void)					// Retorna 1 si el formato es válido (ss <= 59 y tiempo <= 99:59), 
{													// sólo se chequea al presionar START
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
	lcd_display_reposo();	// Mostramos el dibujo de la etapa de reposo
}

void accion_ingreso(void) {
	leds_apagar_todos();				// Apagamos todos los leds
	lcd_display_ingreso(ctx.digitos[0],	// Actualizamos el display con el tiempo actual
		ctx.digitos[1],ctx.digitos[2],ctx.digitos[3]);		
}

void accion_cocinando(void) {
	ctx.cont_1seg = 0;					// Inicializo esta variable por si es la segunda vez que alguien quiere cocinar
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
	leds_apagar_todos();		// Apagamos todos los leds
	ctx.blink_display  = 0;		// asociadas a los parpadeos del display y de la luz alarma
	lcd_display_fin();			// Actualizamos display con mensaje de fin
	ctx.blink_contador = 0;		// Reseteo este valor para la próxima vez que quiera cocinar
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
 * Maneja cuándo hay que transicionar de un estado de la mef a otro,
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
			} else if (tecla == 'A') {					 				 					 
				lcd_display_error();		    // tiempo es 00:00
				 _delay_ms(2000);
				 ctx.lcd_actualizar = 1;		// Actualizar display para mostrar mensaje de error						
            } else if (tecla == 'C') {
				if (ctx.puerta_abierta) {
					 ctx.lcd_actualizar = 1;
				} else {
					// MAS30: inicio rápido con 30 seg
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
					 _delay_ms(2000);
					 ctx.lcd_actualizar = 1;		// Actualizar display para mostrar mensaje de error
				 } 
				 else if (!tiempo_formato_valido()) {					 
					 lcd_display_maximo();			// Tiempo fuera de rango, los segundos son mayores a 59
					 _delay_ms(2000);
					 ctx.lcd_actualizar = 1;		// Actualizar display para mostrar mensaje de error					 
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
			else if (tecla == 'D') {
				ctx.puerta_abierta = !ctx.puerta_abierta;	// Toggle puerta
				ctx.lcd_actualizar = 1;						// Se informa en el display
			}
            break;

        // COCINANDO
        case COCINANDO:
            if (tecla == 'B') {
                cambiar_estado(PAUSA);			// si estoy cocinando y me apretan B paso al estado de pausa
            } else if (tecla == 'C') {
                tiempo_agregar(TIEMPO_MAS30);	// si me apretan la C agrego 30 segundos al tiempo actual
                ctx.lcd_actualizar = 1;
            } else if (tecla == 'D') {			// si me apretan la D paso al estado de puerta abierta
				ctx.puerta_abierta = 1;
                cambiar_estado(PUERTA_ABIERTA);
            }
			           
			// Tick de 100ms, me fijo si completo 1 segundo para decrementar tiempo en el temporizador
			if (ctx.tick_timer1) {
				ctx.tick_timer1 = 0;			// apago la flag, ya fue atendida
				ctx.cont_1seg++;				// sumo 1 al contador
				if (ctx.cont_1seg >= 10) {		// 10 ticks x 100ms = 1 segundo
					ctx.cont_1seg = 0;			// pongo en cero el contador para la próxima iteración
					if (ctx.tiempo_seg > 0) {	// si queda tiempo de coccion...
						ctx.tiempo_seg--;		// decremento 1 segundo
						ctx.lcd_actualizar = 1;	// actualizo el tiempo en el display
					}
					if (ctx.tiempo_seg == 0) {	// si terminé de cocinar paso al estado de fin
						cambiar_estado(FIN);
					}
				}
			}
            break;

        // PAUSA 
        case PAUSA:
            if (tecla == 'A') {					// si estoy en pausa y me apretan la A vuelvo a cocinar
                cambiar_estado(COCINANDO);
            } else if (tecla == 'B') {			// si me apretan la tecla B vuelvo al reposo
                cambiar_estado(REPOSO);
            } else if (tecla == 'C') {			// si me apretan la tecla C agrego 30 segundos al tiempo actual
                tiempo_agregar(TIEMPO_MAS30);	// pero me quedo en el estado de pausa
				ctx.lcd_actualizar = 1;         // y actualizo el tiempo en el display       
            } else if (tecla == 'D') {			// si me apretan la D paso al estado de puerta abierta
				ctx.puerta_abierta = 1;
                cambiar_estado(PUERTA_ABIERTA);
            }
            break;

        // PUERTA_ABIERTA
        case PUERTA_ABIERTA:
            if (tecla == 'D') {				// si me apretan la D cierro la puerta y vuelvo a cocinar
				ctx.puerta_abierta = 0;		// cerrar puerta                
                cambiar_estado(COCINANDO);	// reanudar coccion 
            } else if (tecla == 'B') {		// si me apretan la B con puerta abierta paso al estado de reposo
				ctx.puerta_abierta = 1;		// pero recordando que tengo la puerta abierta
                cambiar_estado(REPOSO);
            }
            break;

        // FIN
        case FIN:         
			// El Timer1 está configurado a 100ms, entonces este bloque se ejecuta 10 veces por segundo   
            if (ctx.tick_timer1) {
	            ctx.tick_timer1 = 0;		// apagamos el flag para indicar que ya fue atendido
	            ctx.blink_contador++;		// contamos cuántos ticks de 100ms pasaron

				// Alternamos el LED de alarma en cada tick (cada 100ms)
	            LED_ALR_PORT ^= (1 << LED_ALR_PIN); 
				// Alternamos el display en cada tick (cada 100ms)
	            if (ctx.blink_display) {
		            LCDblank();				// display apagado
		            ctx.blink_display = 0;
		        } else {
		            LCDvisible();			// display prendido
		            ctx.blink_display = 1;
	            }

	            // Verificamos si ya pasaron los 5 segundos completos, y para eso ctx.blink_contador = DURACION_BLINK * 10  
	            // DURACION_BLINK = 5u, y lo multiplico por 10 porque el tick es cada 100ms 
				// 5 * 10 = 50 ticks = 5 segundos
				if (ctx.blink_contador >= DURACION_BLINK * 10) {
					LED_ALR_PORT &= ~(1 << LED_ALR_PIN);		// apagamos el LED de alarma
					LCDvisible();								// dejamos el display visible
					cambiar_estado(REPOSO);						// volvemos al estado inicial
				}
            }
            break;
    }
}

// MAIN
int main(void)
{		
    sistema_init();				// Inicializamos el sistema: LEDs, LCD, keypad, contexto y timers

    // Mostrar pantalla inicial de reposo
    lcd_display_reposo();

    uint8_t tecla      = KEY_NONE;	// tecla procesada en este ciclo
    uint8_t tecla_ant  = KEY_NONE;	// tecla del ciclo anterior (para detectar flancos)
	uint8_t cont_display = 0;		// variable local para contar 500ms (es decir cuando cont_display=5)
									// chequeo si hay que actualizar el display (más lento que el keypad)
	
	while (1)
	{
		if (ctx.tick_timer1) {

			// Lectura de teclado cada 100ms
			uint8_t raw = KEY_NONE;				// escaneo no bloqueante: retorna la tecla presionada o KEY_NONE
			KEYPAD_Scan(&raw);
			// Detección de flanco: solo procesamos la tecla en el ciclo
			// en que se presiona por primera vez, ignorando mientras se mantiene apretada
			// Si raw es distinto de KEY_NONE (hay tecla) Y distinto de tecla_ant (es nueva)
			if (raw != KEY_NONE && raw != tecla_ant) {
				tecla = raw;		// tecla nueva: la procesamos
			} else {
				tecla = KEY_NONE;	// misma tecla que antes o ninguna: ignorar
			}
			tecla_ant = raw;		// guardamos para comparar en el próximo ciclo

			// MEF periódica: se ejecuta cada 100ms disparada por el timer1
			// La MEF decide si cambiar de estado o no según la tecla actual (que puede ser KEY_NONE) y el estado actual
			mef_ejecutar(tecla);

			// Consumimos el tick después de que la MEF lo procesó
			ctx.tick_timer1 = 0;

			// Display se actualiza cada 500ms (cada 5 ticks de 100ms) (cuando cont_display=5)
			cont_display++;
			if (cont_display >= 5) {
				cont_display = 0;
				
				// Solo redibujamos el LCD cuando algo cambió (flag seteado por la MEF)
				// Esto evita escribir en el LCD en cada ciclo del loop innecesariamente
				if (ctx.lcd_actualizar) {
					ctx.lcd_actualizar = 0;				// apagamos el flag para indicar que ha sido atendido
					
					// Precalculamos mm y ss para los estados que usan tiempo_seg
					uint8_t mm = ctx.tiempo_seg / 60;
					uint8_t ss = ctx.tiempo_seg % 60;
					// Llamamos la función de display correspondiente al estado actual
					switch (ctx.estado) {
						case REPOSO:
							if (ctx.puerta_abierta) lcd_display_puerta(0, 0);		// avisa que la puerta está abierta
							else                    lcd_display_reposo();			// pantalla inicial: "00:00 / Ingrese tiempo"
							break;
						case INGRESO:
							if (ctx.puerta_abierta) {
								// Puerta abierta durante el ingreso: mostramos aviso
								// Convertimos el buffer de dígitos a mm/ss para mostrarlo
								uint8_t mm = ctx.digitos[0] * 10 + ctx.digitos[1];
								uint8_t ss = ctx.digitos[2] * 10 + ctx.digitos[3];
								lcd_display_puerta(mm, ss);
							} else {
								// Mostramos los dígitos crudos del buffer sin convertir a segundos
								// Así el usuario ve exactamente lo que ingresó (ej: 9,9 --> "00:99")
								lcd_display_ingreso(ctx.digitos[0], ctx.digitos[1],
								ctx.digitos[2], ctx.digitos[3]);
							}
							break;
						case COCINANDO:      lcd_display_cocinando(mm, ss); break;	// Mostramos el tiempo restante y "Cocinando..."
						case PAUSA:          lcd_display_pausa(mm, ss);     break;	// Mostramos el tiempo restante y "PAUSA B=CANCELAR"
						case PUERTA_ABIERTA: lcd_display_puerta(mm, ss);    break;	// Mostramos el tiempo restante y "PUERTA ABIERTA"
						default: break;
					}
				}
			}
		}
	}
		

    return 0;
}