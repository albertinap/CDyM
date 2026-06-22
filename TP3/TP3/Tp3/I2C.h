#ifndef I2C_H
#define I2C_H


#include <stdint.h>


void I2C_init(void); // Configuro el preescaler, tambien la frecuencia del scl y por ultimo habilito el I2C con TWEN en TWCR
void I2C_start(void); // Borro el valor anterior de la interupcion, inicio otra pasaje de mensaje con TWSTA y habilito el I2c con TWEN
void I2C_stop(void); // Borro el valor anterior de TWINT y luego mando por TWSTO que quiero que pare el periferico y habilito el I2c con TWEN
void I2C_write(unsigned char data);
unsigned char I2C_read(unsigned char IsLast); 

#endif /* I2C_H_ */
