/*
 * dht11.h
 *
 * Created: 22/6/2026 18:02:07
 *  Author: Ignacio Mucci Bigliani y Albertina Pezzutti
 */ 


#ifndef DHT11_H_
#define DHT11_H_

#include <stdint.h>

typedef enum {
	DHT11_OK,
	DHT11_ERROR_NO_RESPONSE,
	DHT11_ERROR_CHECKSUM
} DHT11_Status;

typedef struct {
	uint8_t humidity;
	uint8_t temperature;
} DHT11_Data;

DHT11_Status DHT11_read(DHT11_Data *data);
void DHT11_init(void);

#endif /* DHT11_H_ */