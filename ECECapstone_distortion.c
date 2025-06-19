/*
Tyler Reilly
ECE Capstone
Distortion Pedal
 */

#include "ece486.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "stm32l476xx.h"
#include "stm32l4xx_hal.h"
#include "arm_math.h"
#include "soft_clip.h"
#include "hard_clip.h"
#include "get_values.h"


#define AUDIO_BLOCKSIZE 100
#define DISTORTION_TYPE_PIN 2
#define BYPASS_TYPE_PIN 3

float calc_mean(float* x, int blocksize) {
    float sum = 0.0f;
    for (int i = 0; i < blocksize; i++) {
        sum += x[i];
    }
    return sum / (float)blocksize;
}

int main(void)
{
    //Initialize all variables being used
  int nblocksize;
  float *input1, *output1, *distortionDial;

  float distortionLevel = 0.0f;
  float volumeLevel = 1.0f;

  float inversedDistortionLevel = 0.0f;

  float bypassSwitchValue , distortionTypeValue;

  int bypassSwitchPin, distortionTypeSwitchPin, distortionLevelPotentiometerPin, volumeLevelPotentiometerPin;
 
  HARD_CLIP_T *hardFilt;
  SOFT_CLIP_T *softFilt;

  // Use setblocksize() to control the sized of the sample arrays
  setblocksize(AUDIO_BLOCKSIZE);

  // Set up ADCs, DACs, GPIO, Clocks, DMAs, and Timer
  //  Two call versions are shown here depending upon to 
  //  jumpers on the nucleo board. The internal RC operator
  //  should always work, but is lower quality. Use the 
  //  external 8MHz option if your board is jumpered to support it.
  //initialize_ece486(FS_50K, HSI_INTERNAL_RC);
  //Initialize sampling rate of 50k to meet spec. 
  initialize_ece486(FS_50K, HSE_EXTERNAL_8MHz);

  //initialize_ece486(FS_200K, HSE_EXTERNAL_8MHz);
  
  // Allocate Required Memory:
  //  "float" arrays are required to store the sample stream.
  //  other arrays are typically allocated to support the DSP algorithms
  nblocksize = getblocksize(); // Query the sample block size being used
	
  //These are 32 bit floats, single precision, meaning they have 24 bit resolution, meeting spec.
  input1 = (float *)malloc(sizeof(float)*nblocksize);
  distortionDial = (float *)malloc(sizeof(float)*nblocksize);
  //dial2 = (float *)malloc(sizeof(float)*nblocksize);

  output1 = (float *)malloc(sizeof(float)*nblocksize);

  softFilt = init_soft_clip(distortionLevel, volumeLevel, AUDIO_BLOCKSIZE);
  hardFilt = init_hard_clip(distortionLevel, volumeLevel, AUDIO_BLOCKSIZE);
  


  // Infinite Loop to process the sample stream, "nsamp" samples at a time
  while (1) {
    getblockstereo(input1, distortionDial);

    PA6_SET(); //Start of processing time

    RCC->AHB2ENR |=  RCC_AHB2ENR_GPIOCEN; // Set bits of interests to target value
	
	GPIOC->MODER &= ~(3<<(2*BYPASS_TYPE_PIN)); // Input (00)

	GPIOC->PUPDR &= ~(3<<(2*BYPASS_TYPE_PIN)); //No Pull up no pull down


    //Determine whether the footswitch is on or off
    //This will determine whether or not to just pass the input to the output or to do the crunching
    bypassSwitchValue = get_switch_value(BYPASS_TYPE_PIN);

    if(bypassSwitchValue > 1.65f){
        //Pedal is in the on position
        
        //Get value of distortion level potentiometer
        //distortionLevel = get_dial_value(distortionLevelPotentiometerPin, hadc1);

            //Set this to the maximum amplitude for the compression amount
        //This will have to be some sort of inverse relation ship, because the higher the value, the lower the max amplitude is.

        // Map to output range
        inversedDistortionLevel = calc_mean(distortionDial,AUDIO_BLOCKSIZE);
        if(inversedDistortionLevel < -0.8f){
            inversedDistortionLevel = -0.8f;
        }
        if(inversedDistortionLevel > -.05f){
            inversedDistortionLevel = -.05f;
        }
        inversedDistortionLevel = -inversedDistortionLevel;

        //printf("The value of the inversedDistortionLevel is %3.4f \n\n\n", inversedDistortionLevel);

        GPIOC->MODER &= ~(3<<(2*DISTORTION_TYPE_PIN)); // Input (00)

	    GPIOC->PUPDR &= ~(3<<(2*DISTORTION_TYPE_PIN)); //No Pull up no pull down

        //Get value of distortion type switch.
        distortionTypeValue = get_switch_value(DISTORTION_TYPE_PIN);
        
        //Then get value of output amplitude potentiometer
        //Amplify the final output by this amount.
        //volumeLevel = get_dial_value(volumeLevelPotentiometerPin, hadc1);

        if(distortionTypeValue > 1.65f){
            //If soft, run soft clipping code
            //Also logic to light up led at pin whatever to indicate soft clipping
            update_distortion_soft_clip(softFilt, inversedDistortionLevel);
            //update_volume_soft_clip(softFilt, 2.5f);
            calc_soft_clip( softFilt, input1, output1);
            //I guess if it's stereo,
            //calc_soft_clip( softFilt, input2, output2);
        }
        else{
            //If Hard, run hard clipping code
            //Also logic to light up led at pin whatever to indicate hard clipping
            update_distortion_hard_clip(hardFilt,inversedDistortionLevel);
            //update_volume_hard_clip(hardFilt, 2.5f);
            calc_hard_clip( hardFilt, input1, output1);
            //I guess if it's stereo,
            //calc_hard_clip( hardFilt, input2, output2);
        }
        //then have to do some sort of overall amplitude based on s->volumeLevel to the outputs, either way. This saves some lines of code in each calc function.
        //Why define the same thing twice when you can just define it once.
        
        //Send out the final output

        PA6_RESET();  // (falling edge on PA6: Done processing data )
    
        //Stage the output arrays back to the DAC to streem the output
        putblockstereo(output1, distortionDial);
    }
    else{
        putblockstereo(input1, distortionDial);
    }
  }  /// end of while(1)... time to go wait for the next block of input samples...
}
