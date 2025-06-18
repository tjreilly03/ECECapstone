/*!
 * @file
 * 
 * @brief Processor configuration routines to support ECE 486 
 * 
 * @author Don Hummels (Config routines for nucleo taken from Yifeng Zhu template)
 * 
 * @date Jan  2025
 */

/*
 * Contains all of the subroutines to configure various peripherals of the stm32L476,
 * such as RCC, GPIO Ports, DMAs, ADCs, DACs, etc.  
 * 
 * Notes from Yifeng's demo code:
 */
// This demo code takes two analog inputs (PA0 and PA1), and simply streams them out
// as two analog outputs (PA4 and PB0). Specifically:
//  (1) copy the input from ADC1_IN5 (PA0) to DAC1_OUT1 (PA4)
//HUMMELS EDIT:
//  (2) copy the input from ADC1_IN6 (PA1) to DAC1_OUT2 to drive an op-amp buffer to (PB0)
//
// ADC1 and ADC2 work in the dual mode, and their results are saved in a 32-bit value.
// Therefore, there are no separated buffers for ADC1 and ADC2.
//
// ...
//
// The data are moved via DMA using 3 pairs of ping-pong buffers
//  (1) ADC_Buffer_Ping and ADC_Buffer_Pong   (pADC_Ready_to_Read_Buffer indicates which buffer is full)
//  (2) DAC1_Buffer_Ping and DAC1_Buffer_Pong (pDAC1_Ready_to_Write_Buffer indicates the empty buffer)
//  (3) DAC2_Buffer_Ping and DAC2_Buffer_Pong (pDAC2_Ready_to_Write_Buffer indicates the empty buffer)

#include "stm32l476xx.h"
#include "ADC.h"
#include "DAC.h"
#include "TIM.h"
#include "GPIO.h"
#include "EXTI.h"
#include "UART.h"
#include "SysTimer.h"
#include "SysClock.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "sample486.h"
#include "init486.h"

/* Private functions ---------------------------------------------------------*/

/*
 * Actual Sample Rate
 */
static float Actual_Sampling_Frequency;


/*
 * Basic wrapper function to initialize peripherals for ECE 486 labs
 */
void initialize_ece486(uint16_t fs_cnt, enum Clock_Reference clkref)
{
    // Configure the system clock
    if (clkref == HSI_INTERNAL_RC) System_Clock_Init_HSI();
	else if(clkref == HSE_EXTERNAL_8MHz) System_Clock_Init_HSE();
      
	SysTick_Init();	
	
	// TIM4_TRGO triggers ADC and DAC simutaneously.
	// GPIO PB6 (TIM4_CH1) is outputed for debugging
	// The sample rate is 10MHz/(fs_cnt+1)
	TIM4_Init(fs_cnt);
	
	// Two 12-bit Analog Outputs: 0 <=> 0V, 4095 <=> 3.0V 
	// PA4 (DAC1_OUT1), (PB0, throug an op-amp buffer) (DAC1_OUT2)
	DAC_Init();	
	
	// Two 12-bit Analog Inputs: 0V <=> 0, 3.0V <=> 4095
	// PA0 (ADC12_IN5), PA1 (ADC12_IN6)
	ADC_Init();
	
	// Enable GPIO i/o pins:
	// Pushbutton: PC13
	// Digital IO: PA5, PA6, PA7
	GPIO_Init();
	
	// Enable Interrupt on button Processor
	EXTI_Init();

	// Set up the serial port to hand strings back and forth through stm link
	UART2_Init();

}



