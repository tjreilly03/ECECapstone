/*
 * Example program to illustrate the use of the ECE 486 interface.
 * 
 * Channel 1 and channel 2 ADC inputs are copied to the channel 1 and 
 * channel 2 DAC outputs respectively. Each
 * USER button press inverts the signal on the channel 2 DAC.
 * 
 */

#include "ece486.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "stm32l476xx.h"

extern FlagStatus KeyPressed;   // Use to detect button presses

int main(void)
{
  int nsamp, i;
  float *input1, *input2, *output1, *output2;
  static float sign=1.0;
  
  // Use setblocksize() to control the sized of the sample arrays
  setblocksize(100);

  // Set up ADCs, DACs, GPIO, Clocks, DMAs, and Timer
  //  Two call versions are shown here depending upon to 
  //  jumpers on the nucleo board. The internal RC operator
  //  should always work, but is lower quality. Use the 
  //  external 8MHz option if your board is jumpered to support it.
  //initialize_ece486(FS_50K, HSI_INTERNAL_RC);
  initialize_ece486(FS_50K, HSE_EXTERNAL_8MHz);
  
  // Allocate Required Memory:
  //  "float" arrays are required to store the sample stream.
  //  other arrays are typically allocated to support the DSP algorithms
  
  nsamp = getblocksize(); // Query the sample block size being used
	
  input1 = (float *)malloc(sizeof(float)*nsamp);
  input2 = (float *)malloc(sizeof(float)*nsamp);
  output1 = (float *)malloc(sizeof(float)*nsamp);
  output2 = (float *)malloc(sizeof(float)*nsamp);
  
  // (Checking for NULL to indicate malloc failure not supported)
  
  
  // Infinite Loop to process the sample stream, "nsamp" samples at a time
  while(1){
    
    // Ask for a block of ADC samples to be put into the sample buffer:
    //   getblockstereo() will wait here until the input buffer is filled.
    //   On return, we work on the new data buffer... then come back here 
    //   to wait for the next block
    getblockstereo(input1,input2);	
    
    PA6_SET();  // Use a scope on PA6 to measure execution time
                // (Rising edge: Begin processing data block)
                // (PA6 high -> DSP Code at work
                //  PA6 low -> idle time waiting for the next buffer)

    // Signal processing code to calculate the required output buffers:
    //  ECE 486 Students modify this section to process the input 
    //  signals and create the output arrays.
    for (i=0; i<nsamp; i++) {
      output1[i] = input1[i];
      output2[i] = input2[i] * sign;
    }  
    
    PA6_RESET();  // (falling edge on PA6: Done processing data )
    
    //Stage the output arrays back to the DAC to streem the output
    putblockstereo(output1, output2);
    
    if( KeyPressed ) {      // Act on any button press (PC13 "User" button)
      sign *= -1;
      KeyPressed = RESET;
      // printf illustration: 
      // printf output is sent to the STlink serial port: 
      //    9600 Baud Rate, 8 data bits, no parity, 1 start bit and 1 stop bit
      //    "9600 8N1"
      // (Generally, printf calls take too long to handle streaming samples
      //  Expect a buffer overrun error.)
      printf("Button Press! Current sign = %f\n",sign);
    }
    
  }  /// end of while(1)... time to go wait for the next block of input samples...
}
