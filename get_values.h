/*!
 * @file get_values.h
 * 
 * @brief This will hold the function defs for getting the switch and potnetiometer values
 * 
 * @author Tyler Reilly
 * 
 * @date April 2025

 */


#ifndef GET_VALUES
#define GET_VALUES

#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_adc.h"

float get_dial_value(
	//Needs some variable for what poteniometer pin to use
	int pin_number,
	ADC_HandleTypeDef *hadc
);

int getblcksize(void);


void setblcksize(
   uint32_t blksiz       //!< Number of samples per block of ADC/DAC data processed
 );

void getblock(
   float * chan1      //!< [in,out] pointer to an array of floats for the stereo ADC channel
 );


  void putblock(
   float * chan1      //!< [in] Array of output samples for the stereo output channel
 );

float get_switch_value(
	//Needs some variable for what switch pin to use
	int pin_number
);

void set_led_on_value(
	//Needs some variable for what led pin to use
	int pin_number
);


void set_led_off_value(
	//Needs some variable for what led pin to use
	int pin_number
);

#endif
