/*
 * CFile1.c
 *
 * Created: 22/6/2026 18:00:39
 *  Author: alber
 */ 
#include "dht11.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

 /*#define DHT11_OUTPUT()  (DDRC |=  (1 << PORTC0))
 #define DHT11_INPUT()   (DDRC &= ~(1 << PORTC0))
 #define DHT11_HIGH()    (PORTC |=  (1 << PORTC0))
 #define DHT11_LOW()     (PORTC &= ~(1 << PORTC0))
 #define DHT11_READ()    (PINC  &   (1 << PORTC0))
 */

DHT11_Status DHT11_read(DHT11_Data *data){
	uint8_t bytes[5] = {0}; // Son los 5 bytes que el DHT11 me va a pasar seguidos Bytes[0] es la parte de la humedad entera, luego Bytes[1] es la parte de la humedad decimal
		// los siguiente es lo mismo con la temperatura y por ultimo el valor del checksum para asegurar el pasaje de mensaje
		
		// START: MCU tira LOW 18ms, suelta y espera respuesta
		DDRC |=  (1 << PORTC0); // Configuro el pin PC0 como salida
		PORTC &= ~(1 << PORTC0); // Pongo la linea a 0V
		_delay_ms(18); //Valor estandar que el mcu debe dejar la linea en 0 para poder comunicar
		PORTC |=  (1 << PORTC0); // Dejo la linea a los 5V
		DDRC &= ~(1 << PORTC0); // Configuro PC0 para entrada
		_delay_us(30); // El dht11 tarda alrededor de entre 20 a 40 us para poder responder, tomo tiempo medio para esperar la respuesta
		
		
		 // Verifico respuesta del sensor: LOW 80us + HIGH 80us
		 if (!(PINC  &   (1 << PORTC0))) { // La linea esta en LOW, el sensor respondio
			 _delay_us(80);	// Espero los 80us que dura el LOW del sensor
			 if ((PINC  &   (1 << PORTC0)) { // La linea subio a HIGH, respuesta correcta
				 _delay_us(80);// Espero los 80us que dura el HIGH del sensor
				 } else {//En los dos casos donde esto no pase devuelvo no response del dht 11
				 return DHT11_ERROR_NO_RESPONSE;
			 }
			 } else {
			 return DHT11_ERROR_NO_RESPONSE;
		 }
		 
		  // Leo 40 bits: 5 bytes
		  cli(); // Deshabilito interrupciones para no corromper el timing
		  for (uint8_t i = 0; i < 5; i++) { // Itero para cada byte
			  for (uint8_t bit = 0; bit < 8; bit++) { // itero para cada bit
				  // Espero el LOW de 50us que inicia cada bit
				  while (!(PINC  &   (1 << PORTC0)));// Espero flanco ascendente, fin del LOW de 50us
				  _delay_us(40); // Sampleo a los 40us
				  bytes[i] <<= 1; // Corro los bits 1 lugar para el nuevo bit entrante
				  if ((PINC  &   (1 << PORTC0))) { // Si a los 40us sigue HIGH, es un 1
					  bytes[i] |= 1; // Si sigue HIGH a los 40us es un 1
				  }
				  while ((PINC  &   (1 << PORTC0))); // Espero que baje para el siguiente bit
			  }
		  }
		  sei(); // Vuelvo a habilitar las interrupciones
		  
		  if (bytes[4] != ((bytes[0] + bytes[1] + bytes[2] + bytes[3]) & 0xFF)) {
			  return DHT11_ERROR_CHECKSUM; // Hubo un error en el pasaje del mensaje
		  }

		  data->humidity= bytes[0]; // Paso el valor entero de humedad
		  data->temperature= bytes[2]; // Paso el valor entero de temperatura
		  return DHT11_OK; //Pasaje de mensaje exitoso
}
