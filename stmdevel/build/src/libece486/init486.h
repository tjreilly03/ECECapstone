/**!
 * @file
 * 
 * @brief STM32L476-Nucleo configuration routines to support ECE 486
 * 
 * @author Don Hummels
 * 
 * @date Jan 2025
 * 
 * Contains all of the subroutines to configure various peripherals of the stm32L476
 * 	 RCC, GPIO ports, DMAs, ADCs, DACs, etc.
 * 
 * @defgroup ECE486_Init ECE 486 Processor Initialization 
 * @{
 */

#ifndef __INIT486__
#define __INIT486__

/*
 * Includes..........................................................................
 */
#include <stdint.h>
#include "stm32l476xx.h"


/*
 * Defines..........................................................................
 */

// Valid Clock divisors to sample rate constants for initialize()
// (Integers used to divide an 80 MHz clock to obtain a desired sample rate)
/*! 
 * 
 * @defgroup FS_Count_Values Timer Count constants for determining ADC/DAC sample rates
 * @{
 * 
 * Timers are each driven by 80 MHz clock signals. This clock is divided by 8 (PSC+1)
 * to obtain a 10 MHz reference. This reference is divided by "ARR+1" to obtain the final
 * sampling frequency. Values of ARR are provided in the table below for common sampling 
 * rates. For example, using ARR=000 gives a sampling clock rate of (10 MHz)/(999+1) = 10 kHz
 */
#define FS_10K 999 	//!< 10 ksamples/sec
#define FS_20K 499 	//!< 20 ksamples/sec
#define FS_25K 399 	//!< 25 ksamples/sec
#define FS_40K 249 	//!< 40 ksamples/sec
#define FS_50K 199 	//!< 50 ksamples/sec
#define FS_100K 99 	//!< 100 ksamples/sec
#define FS_200K 49 	//!< 200 ksamples/sec
#define FS_500K 19 	//!< 500 ksamples/sec
/*! @} End of FS_Count_Values group */

/*
 * Digital GPIO output actions
 */
#define PA5_SET() GPIOA->ODR |= GPIO_ODR_ODR_5
#define PA5_RESET() GPIOA->ODR &= ~GPIO_ODR_ODR_5
#define PA5_TOGGLE() GPIOA->ODR ^= GPIO_ODR_ODR_5
#define PA6_SET() GPIOA->ODR |= GPIO_ODR_ODR_6
#define PA6_RESET() GPIOA->ODR &= ~GPIO_ODR_ODR_6
#define PA6_TOGGLE() GPIOA->ODR ^= GPIO_ODR_ODR_6
#define PA7_SET() GPIOA->ODR |= GPIO_ODR_ODR_7
#define PA7_RESET() GPIOA->ODR &= ~GPIO_ODR_ODR_7
#define PA7_TOGGLE() GPIOA->ODR ^= GPIO_ODR_ODR_7

/*
 * structs, typedefs, enums .............................................................
 */
/*!
 * @brief Clock Reference Source
 * 
 * By default, the STM32L476G-Nucleo board uses the HSI internal RC Oscillator as
 * its clock source.  Specify MSI_INTERNAL_RC for an unmodified board (SB54, SB55 closed,
 * SB16 and SB50 open, R35 and R37 removed. .
 * 
 * By modifying jumpers on the board, an external clock reference driving the 
 * high speed external (HSE) OSC_IN pin can be used.  The external clock can be 
 * provided by the ST-LINK MCU, or driven externally through a header pin.  
 * Specify HSE_EXTERNAL_8MHz to use this improved clock 
 * source if your board has been modified.
 * 
 * - 8 MHz XTAL Ref from ST-LINK MCU: SB55 open, SB54 closed, SB16 and SB50 closed, R35 & R37 removed.
 * 
 */
enum Clock_Reference {
  HSI_INTERNAL_RC,	//!< Internal HSI RC Oscillator 
  HSE_EXTERNAL_8MHz	//!< External 8MHz reference 
};



/*
 * Global variables.....................................................................
 */

/*
 * Function Prototypes.....................................................................
 */

/*!
 * @defgroup Functions Configuration Functions
 * @{
 */

/*!
 * @brief Wrapper function to perform all processor initialization for ECE 486
 * 
 * This wrapper function is called by users at the start of ECE 486 routines to
 * perform initialization of I/O pins, ADCs, DACs, Timers, DMAs, and Interrupts.
 * 
 * Input parameters determine the ADC/DAC sample rate, and the source of the clock reference signal.  
 * Sample rates are determined by specifying a 
 * @a timer_count_value, which determines the ultimate sample frequency.  
 * Various \#define constants are provided to obtain common sample rates: 
 * @ref FS_Count_Values.
 * 
 * For example:
 * @code
 *   initialize_ece486( FS_50K, MSI_INTERNAL_RC);
 * @endcode
 * is used to configure the STM32-Nucleo board for sampling at a 50 ksamples/second
 * input and output rate.
 * The internal RC MSI oscillator is used to provide the clock generation reference.
 * 
 * It is not possible to have different ADC and DAC sample rates using this routine.
 * 
 * 
 */
void initialize_ece486(
  uint16_t timer_count_value, 	//!< [in] ARR Value for clock generation
  enum Clock_Reference clkref   //!< [in] Clock ref source: HSI_INTERNAL_RC or HSE_EXTERNAL_8MHz
);



/*! @} End of Functions group */

#endif

/*!
 * @} End of ECE486_Init group
 */
