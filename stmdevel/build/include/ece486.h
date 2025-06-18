/*!
  @file
  @brief NUCLEO-L476RG ECE 486 Interface
  @author Don Hummels
  @date Jan, 2025

    @mainpage NUCLEO-L476RG support library for ECE 486 Digital Signal Processing

    This library defines an interface that allows a user use the 
    NUCLEO-L476RG development boards
    to stream input sampled data
    in real time from two ADCs, process the data, 
    and stream the resulting output samples to two output DACs.
	
    The user interface is as follows:
	
      -# Required include: @code #include "ece486.h" @endcode
      -# Optionally, the user calls @code
                    setblocksize(nsamp);
        @endcode
        This call is used to change the data block size that will be delivered
        to the user in later function calls. (If setblocksize() is not called,
        a default value of #ADC_DEFAULT_SAMPLE_SIZE is used.)  Using a larger block
        size may result in more efficient code, while using a smaller block 
        size will reduce the latency of the system.
      -# The user calls @code
		    initialize_ece486(fs_count, clk_ref);
	@endcode 
	where:
	  - @a fs_count determines the sampling frequency.  Predefined integer
                  constants for @a fs are provided at @ref FS_Count_Values. 
	  - @a clk_ref should be #HSI_INTERNAL_RC or #HSE_EXTERNAL_8MHz
	      
	The same sample rate is used for the input ADC and output DAC data streams.  
	The initialize_ece486() call enables clocks and configures the 
	appropriate ADC(s), DAC(s), Timers, DMAs and interrupts.    
	    
	Two analog input signals are sampled via ADC1 ("channel 1") and ADC2 ("channel 2"). 
	ADC1 is accessed as an input on PA0, and ADC2 is accessed on PA1.  
	    
	Two analog outputs are generated using DAC1 ("channel 1") and DAC2 ("channel 2")
	DAC1 is accessed as an output on PA4.  DAC2 actually drives an internal op-amp
	buffer (OPAMP2), with output to PB0.
	   
	The green "user" LED is configured (PA5) and enabled.  The LED
	provides feedback during program execution.  
	
      -# The user calls @code
		nsamp = getblocksize();
	@endcode 
	@a nsamp gives the data block size which will be delivered to
	the user in later function calls.  Input samples (provided by
	the ADC(s) via DMA1)
	will be accessed in blocks of @a nsamp samples at a time. 
	Likewise, the user programs should generate "nsamp" output 
	samples for delivery to the DAC.  Typically, users call
	getblocksize() to learn the required number of samples so 
	that the correct amount of memory for processing the blocks 
	of data may be allocated.
	
      -# Users then repeatedly call getblockstereo() to obtain
	samples from the ADCs, and (after processing the samples) 
	putblockstereo() to write the results back 
	to the DAC.  
	@code
	  getblockstereo(input1, input2);	// stereo input
	@endcode
	will fill in the user's input array(s) (type float) with 
	normalized input (ADC) samples.  Callers are responsible to make sure 
	that the arrays @a input1 and @a input2 are allocated with enough 
	memory to hold @a nsamp float values.  After processing the 
	input waveforms to create results, the output samples are 
	placed in the DAC output buffers using
	@code
	  putblockstereo(result1, result2);	// Stereo Output
	@endcode
	Typically, after calling the putblockstereo() routine, the user 
	calls getblockstereo() to wait for the next available block of 
	input data, and the process repeats.  If the DMA 
	completes filling a new block of data before the user calls 
	getblockstereo(), a #SAMPLE_OVERRUN error is indicated, and 
	the green LED is flashed at twice the normal rate (2 flashes per second).  
	In this case, the data is not being 
	processed fast enough to keep up with the input data stream, 
	and input samples are being lost.
      
      Sample values in the "input" or "result" arrays are of 
      type float, and are normalized to range from -1.0 to +1.0 
      over the voltage range of the ADCs | DACs. (A sample of -1.0 is near 0V,
      +1.0 is near 3.3V, and 0.0 indicates the mid-scale voltage of 1.65V)

      By default, Pins PA5 (green LED), PA6 and PA7 on the NUCLEO board are configured
      as a digital output pins.  The pin status may be set using the macros like:
       @code
         PA6_SET();      // Set high
         PA6_RESET();    // Set low
         PA6_TOGGLE();   // Toggle state
       @endcode

	  Users can check the global variable #KeyPressed to 
      check whether the (blue) user button on the development board 
      has been pressed.   #KeyPressed==SET indicates that 
      a button press has occurred.  
      The user should reset the value using #KeyPressed=RESET;
      
      A UART is also configured to direct printf() output to the ST-LINK USB serial
      port.  Assuming that the host is connected to the ST-LINK connector, printf output
      may be examined on the host by connecting a terminal to the serial port.  The terminal
      should be configured at 9600 baud, 8 bits, no parity, one stop-bit.  printf() calls are 
      blocking.
      

	
*/

#ifndef __ECE486__
#define __ECE486__

#include "init486.h"
#include "stm32l4xx.h"

#include "sample486.h"
#include "SysTimer.h"

#endif
