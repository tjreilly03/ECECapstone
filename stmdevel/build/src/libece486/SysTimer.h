#ifndef __NUCLEO_L476RG_SYSTICK_H
#define __NUCLEO_L476RG_SYSTICK_H

#include "stm32l476xx.h"

void SysTick_Init(void);
void SysTick_Handler(void);
void delay (uint32_t T);
void disable_Heartbeat(void);
void Set_Heartbeat_Count (uint32_t);

#endif /* __NUCLEO_L476RG_SYSTICK_H */
