#include "GPIO.h"
#include "stm32l476xx.h"
#include <stdint.h>

//-------------------------------------------------------------------------------------------------------------------
// GPIO PA5, PA6, PA7 as digital outputs.
//
// PA5 is connected to the "User LED2" on the Nucleo boards
// PA6, PA7 is open for any user functions
//-------------------------------------------------------------------------------------------------------------------
void GPIO_Init(void){
	RCC->APB2ENR |= RCC_AHB2ENR_GPIOAEN ; // Enable GPIOA Clocks
	
    /*
     * PA5, PA6, PA7: Digital output pins
     */
    GPIOA->MODER &= ~(3U<<(2*5) | 3U<<(2*6) | 3U<<(2*7));    // Clear functions
    GPIOA->MODER |=  (1U<<(2*5) | 1U<<(2*6) | 1U<<(2*7));    // Set as output
    GPIOA->OTYPER &= ~(1U<<5 | 1U<<6 | 1U<<7 );              //  push/pull
    GPIOA->OSPEEDR &= ~(3U<<(2*5) | 3U<<(2*6) | 3U<<(2*7));
    GPIOA->OSPEEDR |=  (3U<<(2*5) | 3U<<(2*6) | 3U<<(2*7));  // Set to "very high speed" 
    GPIOA->PUPDR   &= ~(3U<<(2*5) | 3U<<(2*6) | 3U<<(2*7));  // No pull-up, no pull-down
	
}

