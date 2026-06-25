/*
 * comandos.h
 * Interfaz del módulo de comandos serie
 * Author : Ignacio Mucci Bigliani y Albertina Pezzutti
 */ 

#ifndef COMANDOS_H_
#define COMANDOS_H_

#include <stdint.h>

// Inicializa el índice del buffer de comandos
void comandos_init(void);

// Tarea de foreground, se llama en el super loop
// Lee la UART y procesa comandos completos al detectar que el usuario ingresa un enter
void comandos_tarea(void);

// Parsea y ejecuta un comando terminado en '\0' (si tiene algún error tira un aviso)
// Lo llama la función comandos_tareas cuando detecta que ya se ingresó un comando
void procesar_comando(char *cmd);

#endif /* COMANDOS_H_ */