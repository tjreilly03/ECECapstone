#ifndef __NUCLEO_L476RG_ADC_H
#define __NUCLEO_L476RG_ADC_H

#include "stm32l476xx.h"

#define  ADC_DEFAULT_SAMPLE_SIZE 100

void ADC_Init(void);

void ADC_Wakeup (ADC_TypeDef * ADCx);
void ADC_Calibration(ADC_TypeDef * ADCx);
void ADC_Configure(ADC_TypeDef * ADCx);

void ADC_Pin_Init(void);
void ADC_Regular_Channel_Configuration(void);
void ADC_Common_Configuration(void);

#endif /* __NUCLEO_L476RG_ADC_H */
