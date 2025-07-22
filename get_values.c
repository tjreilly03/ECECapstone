/*!
 * @file
 *
 * @brief Getting and Setting functions, for leds and switches
 *
 * @author Tyler Reilly
 *
 * @date May 2025
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "get_values.h"
#include <math.h>
#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_adc.h"

extern volatile uint32_t ADC_DMA_Done;


 extern uint16_t *pDAC1_Ready_to_Write_Buffer;
 extern uint16_t *pDAC2_Ready_to_Write_Buffer;
 extern uint32_t *pADC_Ready_to_Read_Buffer;
  



float get_switch_value(
	//Needs some variable for what switch pin to use
	int pin_number
)
{
	//Get voltage value for that switch at pin pin_number
	float switch_state = 0.0f;
	switch_state = (GPIOC->IDR & 1<< pin_number) == 0 ? 3.3f : 0.0f;
  return(switch_state);
}

void set_led_on_value(
	//Needs some variable for what switch pin to use
	int pin_number
){

	GPIOC->BSRR = (1 << pin_number);
}

void set_led_off_value(
	//Needs some variable for what switch pin to use
	int pin_number
){
	//not sure if this will work or if I will use? whats happening now is working, don't want to make it mad
	GPIOC->BSRR = (1 << (pin_number + 16));
}

