/*!
 * @file
 * 
 * @brief Running Mean Filter Implementation
 * 
 * @author ECE486 Lab Group 12
 * @author Tyler Reilly, Joshua Deveau, Basel Alsarraf
 * 
 * @date Feb 2024
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
	//Use gpio pin
	float switch_state = 0.0f;
	if(pin_number == 3){
		switch_state = (GPIOC->IDR & 1<<3) == 0 ? 3.3f : 0.0f;
	}
	else if(pin_number == 2){
		switch_state = (GPIOC->IDR & 1<<2) == 0 ? 3.3f : 0.0f;
	}
  return(switch_state);
}

void set_led_on_value(
	//Needs some variable for what switch pin to use
	int pin_number
){

//Set the desired led on
//Use gpio pin to turn voltage high to 3.3v
}

void set_led_off_value(
	//Needs some variable for what switch pin to use
	int pin_number
){

//Set the desired led off
//Use gpio pin to turn voltage low to gnd
}

