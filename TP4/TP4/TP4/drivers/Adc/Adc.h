/*
 * Adc.h
 *
 * Created: 29/7/2026 18:52:21
 *  Author: rocio
 */ 


#ifndef ADC_H_
#define ADC_H_
#include <stdint.h>

// Inicializa el ADC
void ADC_Init(void);

//Realiza una conversión ADC por polling. 
uint16_t ADC_Read(void);

#endif /* ADC_H_ */