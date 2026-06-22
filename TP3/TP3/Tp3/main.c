/*
 * Tp3.c
 *
 * Created: 22/6/2026 17:10:22
 * Author : Ignacio Mucci Bigliani
 */ 

int main(void)
{

	UART_init();
	I2C_init();
	RTC_init();
	DHT11_init();
	TIMER_init();


	sei(); // habilitar interrupciones


	while(1)
	{
		invernadero_tarea();
	}

}

