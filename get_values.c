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



float get_dial_value(
	//Needs some variable for what poteniometer pin to use
	int pin_number
)
{
	//Get voltage value at pin pin_number

  float value = 0.0f; //some function to get voltage from poteniometer
  return(value);
}


float get_switch_value(
	//Needs some variable for what switch pin to use
	int pin_number
)
{
	//Get voltage value for that switch at pin pin_number

  float value = 0.0f; //some function to get voltage from poteniometer
  return(value);
}

void set_led_on_value(
	//Needs some variable for what switch pin to use
	int pin_number
){

//Set the desired led on
}

void set_led_off_value(
	//Needs some variable for what switch pin to use
	int pin_number
){

//Set the desired led off
}

