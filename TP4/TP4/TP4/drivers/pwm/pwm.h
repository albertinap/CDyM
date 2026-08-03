/*
 * pwm.h
 * Interfaz del driver PWM.
 * Permite controlar los tres canales del LED RGB:
 * - Rojo mediante PWM por software.
 * - Verde y azul mediante PWM por hardware (Timer1).
 *
 * Author: Ignacio Mucci Bigliani y Albertina Pezzutti
 */

#ifndef PWM_H_
#define PWM_H_

#include <stdint.h>

// Inicializa el módulo PWM y los timers asociados
void PWM_init(void);

// Actualiza el estado del PWM por software
void PWM_task(void);

// Actualiza el duty cycle de cada canal
void PWM_set_red(uint8_t duty);
void PWM_set_green(uint8_t duty);
void PWM_set_blue(uint8_t duty);

// Fuerza el nivel lógico de cada salida, desconectando el PWM
void PWM_force_red(uint8_t nivel);
void PWM_force_green(uint8_t nivel);
void PWM_force_blue(uint8_t nivel);

// Restaura el control de la salida por PWM
void PWM_reconnect_red(void);
void PWM_reconnect_green(void);
void PWM_reconnect_blue(void);

#endif /* PWM_H_ */