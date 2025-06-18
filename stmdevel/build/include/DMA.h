#ifndef __NUCLEO_L476RG_DMA_H
#define __NUCLEO_L476RG_DMA_H

#include "stm32l476xx.h"

void ADC_DMA_Configuration(DMA_Channel_TypeDef * DMA_Channelx, uint32_t * pBuffer);

void DAC_DMA_Configuration(DMA_Channel_TypeDef * DMA_Channelx, uint16_t * pBuffer, uint32_t * pPeripheralAddr);

void DFSDM_DMA_Configuration (DMA_Channel_TypeDef * DMA_Channelx, uint32_t * pBuffer);

void CODEC_DMA_Configuration (DMA_Channel_TypeDef * DMA_Channelx, uint16_t * pBuffer);

#endif /* __NUCLEO_L476RG_DMA_H */

