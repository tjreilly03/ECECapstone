#ifndef __NUCLEO_L476RG_DAC_H
#define __NUCLEO_L476RG_DAC_H

#include "stm32l476xx.h"

void DAC_Init(void);
void DAC_Pin_Configuration(void);
void DAC_Configuration(void);
void DAC_Calibration_Channel(uint32_t channel);

#endif /* __NUCLEO_L476RG_DAC_H */

