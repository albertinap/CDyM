/*
 * CFile1.c
 *
 * Created: 22/6/2026 17:11:07
 *  Author: Mucci
 */ 
void i2c_init(void)
{   // Configuramos la velocidad del bus a 100 kHz que es el estandar de la industria
	TWSR = 0x00;    // seteo el preescalador a 0
	TWBR = 72;     //  F_SCL = F_CPU / (16 + 2 * TWBR * Prescaler), para que el Fcsl sea 100khz tenemos que poner el TWBR en 72
	TWCR = 0x04;    // Habilitamos el uso de I2C
}

void i2c_start(void)
{
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN); // Generamos la condicion de Start, limpiamos el TWINT y mantenemos habilitado el pasaje de mensaje

	while ((TWCR & (1 << TWINT)) == 0); // Esperamos respuesta
}
void I2C_stop(void)
{
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN); // Generamos la condición STOP limpiamos el TWINT y mantenemos habilitado el pasaje de mensaje
}

void i2c_write(unsigned char data)
{
	TWDR = data;  // Cargamos el dato a transmitir en el registro de datos TWI

	TWCR = (1 << TWINT) | (1 << TWEN); // Limpiamos TWINT para iniciar la transmisión y mantenemos habilitado el periférico

	while ((TWCR & (1 << TWINT)) == 0); // Esperamos hasta que termine la transmicion
}

unsigned char i2c_read(unsigned char isLast)
{
	if (isLast == 0){      // Si no es el último byte a recibir
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA); // Iniciamos la recepción y enviamos ACK al terminar
	}else{                  // Si es el último byte a recibir
	TWCR = (1 << TWINT) | (1 << TWEN); // Iniciamos la recepción y enviamos NACK al terminar
	}
	while ((TWCR & (1 << TWINT)) == 0); // Esperamos hasta que termine

	return TWDR; // Devolvemos el dato recibido almacenado en el registro de datos TWI
}