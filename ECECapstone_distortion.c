/*
 Lab 3 Code for Nucleo Board
 */

#include "ece486.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "stm32l476xx.h"
#include "arm_math.h"
#include "soft_clip.h"
#include "hard_clip.h"

#define BLOCKSIZE 100

int main(void)
{
    //Initialize all variables being used
  int nblocksize;
  float *input1, *input2, *output1, *output2;

  float distortionLevel = 0.0f;
 
  HARD_CLIP_T *hardFilt;
  SOFT_CLIP_T *softFilt;

  // Use setblocksize() to control the sized of the sample arrays
  setblocksize(BLOCKSIZE);

  // Set up ADCs, DACs, GPIO, Clocks, DMAs, and Timer
  //  Two call versions are shown here depending upon to 
  //  jumpers on the nucleo board. The internal RC operator
  //  should always work, but is lower quality. Use the 
  //  external 8MHz option if your board is jumpered to support it.
  //initialize_ece486(FS_50K, HSI_INTERNAL_RC);
  initialize_ece486(FS_50K, HSE_EXTERNAL_8MHz);

  //initialize_ece486(FS_200K, HSE_EXTERNAL_8MHz);
  
  // Allocate Required Memory:
  //  "float" arrays are required to store the sample stream.
  //  other arrays are typically allocated to support the DSP algorithms
  nblocksize = getblocksize(); // Query the sample block size being used
	
  input1 = (float *)malloc(sizeof(float)*nblocksize);
  input2 = (float *)malloc(sizeof(float)*nblocksize);
  output1 = (float *)malloc(sizeof(float)*nblocksize);
  output2 = (float *)malloc(sizeof(float)*nblocksize);

  softFilt = init_soft_clip(distortionLevel);
  hardFilt = init_hard_clip(distortionLevel);
  

  // Infinite Loop to process the sample stream, "nsamp" samples at a time
  while(1){
    
    // Ask for a block of ADC samples to be put into the sample buffer:
    //   getblockstereo() will wait here until the input buffer is filled.
    //   On return, we work on the new data buffer... then come back here 
    //   to wait for the next block
    getblockstereo(input1,input2);	

    //Sudo Code:
    //Get value of distortion level potentiometer
    //Set this to the maximum amplitude for the compression amount
    //This will have to be some sort of inverse relation ship, because the higher the value, the lower the max amplitude is.

    //distortionLevel = getDistortionValue();

    update_distortion_soft_clip(softFilt, distortionLevel);
    update_distortion_hard_clip(hardFilt,distortionLevel);


    //Get value of distortion type switch.
    //If soft, run soft clipping code
    //If Hard, run hard clipping code

    //Then get value of output amplitude potentiometer
    //Amplify the final output by this amount.
    //Send out the final output
    
    

    PA6_SET();

    PA6_RESET();  // (falling edge on PA6: Done processing data )
    
    //Stage the output arrays back to the DAC to streem the output
    putblockstereo(output1, output2);
    
  }  /// end of while(1)... time to go wait for the next block of input samples...
}
