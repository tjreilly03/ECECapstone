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

//switch pins
#define DISTORTION_TYPE_PIN 2
#define BYPASS_TYPE_PIN 3


//whatever pins I end up using, just update here!
#define VOLUME_LOW_PIN 4
#define VOLUME_MID_PIN 5
#define VOLUME_HI_PIN 6

#define VOLUME_LOW_LED_PIN 7
#define VOLUME_MID_LED_PIN 8
#define VOLUME_HI_LED_PIN 9


#define PEDAL_ON_LED_PIN 10
#define PEDAL_OFF_LED_PIN 12

#define PEDAL_HARD_DIST_LED_PIN 0
#define PEDAL_SOFT_DIST_LED_PIN 11



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
  float amplificationLevel = 0.8f;
  int firstRun = 1;
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
  initialize_ece486(FS_50K, HSI_INTERNAL_RC);
  //Initialize sampling rate of 50k to meet spec. 
  //initialize_ece486(FS_50K, HSE_EXTERNAL_8MHz);

  //initialize_ece486(FS_200K, HSE_EXTERNAL_8MHz);
  
  // Allocate Required Memory:
  //  "float" arrays are required to store the sample stream.
  //  other arrays are typically allocated to support the DSP algorithms
  nblocksize = getblocksize(); // Query the sample block size being used
	
  //These are 32 bit floats, single precision, meaning they have 24 bit resolution, meeting spec.
  input1 = (float *)malloc(sizeof(float)*nblocksize);
  distortionDial = (float *)malloc(sizeof(float)*nblocksize);

  output1 = (float *)malloc(sizeof(float)*nblocksize);

  softFilt = init_soft_clip(distortionLevel, volumeLevel, AUDIO_BLOCKSIZE);
  hardFilt = init_hard_clip(distortionLevel, volumeLevel, AUDIO_BLOCKSIZE);
  


  // Infinite Loop to process the sample stream, "nsamp" samples at a time
  while (1) {
    getblockstereo(input1, distortionDial);

    PA6_SET(); //Start of processing time

    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN | RCC_AHB2ENR_GPIOCEN;
	
	GPIOC->MODER &= ~(3<<(2*BYPASS_TYPE_PIN)); // Input (00)

	GPIOC->PUPDR &= ~(3<<(2*BYPASS_TYPE_PIN)); //No Pull up no pull down

    //hi med low volume button pins
    GPIOC->MODER &= ~(3 << (2 * VOLUME_HI_PIN)); // Input (00)

    GPIOC->PUPDR &= ~(3 << (2 * VOLUME_HI_PIN)); //No Pull up no pull down

    GPIOC->MODER &= ~(3 << (2 * VOLUME_MID_PIN)); // Input (00)

    GPIOC->PUPDR &= ~(3 << (2 * VOLUME_MID_PIN)); //No Pull up no pull down

    GPIOC->MODER &= ~(3 << (2 * VOLUME_LOW_PIN)); // Input (00)

    GPIOC->PUPDR &= ~(3 << (2 * VOLUME_LOW_PIN)); //No Pull up no pull down

    // Clear mode bits
    GPIOC->MODER &= ~(0b11 << (PEDAL_OFF_LED_PIN * 2));
    GPIOC->MODER &= ~(0b11 << (PEDAL_ON_LED_PIN * 2));

    GPIOC->MODER &= ~(0b11 << (PEDAL_HARD_DIST_LED_PIN * 2));
    GPIOC->MODER &= ~(0b11 << (PEDAL_SOFT_DIST_LED_PIN * 2));

    // Set both to output mode
    GPIOC->MODER |= (0b01 << (PEDAL_OFF_LED_PIN * 2));
    GPIOC->MODER |= (0b01 << (PEDAL_ON_LED_PIN * 2));
    GPIOC->MODER |= (0b01 << (PEDAL_HARD_DIST_LED_PIN * 2));
    GPIOC->MODER |= (0b01 << (PEDAL_SOFT_DIST_LED_PIN * 2));

    //Determine whether the footswitch is on or off
    //This will determine whether or not to just pass the input to the output or to do the crunching
    bypassSwitchValue = get_switch_value(BYPASS_TYPE_PIN);
    
    //read the values of the high med low buttons to see which one is high. prioritize high, then med, then low


    printf("PRE BUTTON LOGIC!!!!! \n");
    float high = get_switch_value(VOLUME_HI_PIN);
    float mid = get_switch_value(VOLUME_MID_PIN);
    float low = get_switch_value(VOLUME_LOW_PIN);
    printf("high value %f!!!! \n", high);
    printf("mid value %f!!!! \n", mid);
    printf("low value %f!!!! \n", low);


        
    
    if (get_switch_value(VOLUME_HI_PIN) < 1.00f) {
        amplificationLevel = 1.2f;
        firstRun = 0;
        //turn on the assocated led

        printf("HIGH!!!! \n");

        GPIOC->BSRR = (1 << VOLUME_HI_LED_PIN);
        GPIOC->BSRR = (1 << (VOLUME_MID_LED_PIN + 16));
        GPIOC->BSRR = (1 << (VOLUME_LOW_LED_PIN + 16));

    }
    else if (get_switch_value(VOLUME_MID_PIN) < 1.00f || firstRun == 1) {
        amplificationLevel = 0.8f;
        firstRun = 0;
        //turn on the assocated led
        printf("MID!!!! \n");

        GPIOC->BSRR = (1 << VOLUME_MID_LED_PIN);
        GPIOC->BSRR = (1 << (VOLUME_HI_LED_PIN + 16));
        GPIOC->BSRR = (1 << (VOLUME_LOW_LED_PIN + 16));

    }
    else if (get_switch_value(VOLUME_LOW_PIN) < 1.00f) {
        amplificationLevel = 0.4f;
        firstRun = 0;
        //turn on the assocated led
        printf("LOW!!!! \n");

        GPIOC->BSRR = (1 << VOLUME_LOW_LED_PIN);
        GPIOC->BSRR = (1 << (VOLUME_MID_LED_PIN + 16));
        GPIOC->BSRR = (1 << (VOLUME_HI_LED_PIN + 16));
    }

    if(bypassSwitchValue > 1.65f){
        //turn on the PA5 led and turn off pa4
        GPIOC->BSRR = (1 << PEDAL_ON_LED_PIN);
        GPIOC->BSRR = (1 << (PEDAL_OFF_LED_PIN + 16));


        //Pedal is in the on position, because the higher the value, the lower the max amplitude is.

        // Map to output range
        inversedDistortionLevel = calc_mean(distortionDial,AUDIO_BLOCKSIZE);
          //Set this to the maximum amplitude for the compression amount
        //This will have to be some sort of inverse relation sh

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

            GPIOC->BSRR = (1 << PEDAL_SOFT_DIST_LED_PIN);
            GPIOC->BSRR = (1 << (PEDAL_HARD_DIST_LED_PIN + 16));
            //If soft, run soft clipping code
            //Also logic to light up led at pin whatever to indicate soft clipping
            update_distortion_soft_clip(softFilt, inversedDistortionLevel);
            update_volume_soft_clip(softFilt, amplificationLevel);
            calc_soft_clip( softFilt, input1, output1);
            //I guess if it's stereo,
            //calc_soft_clip( softFilt, input2, output2);
        }
        else{
            GPIOC->BSRR = (1 << PEDAL_HARD_DIST_LED_PIN);
            GPIOC->BSRR = (1 << (PEDAL_SOFT_DIST_LED_PIN + 16));
            //If Hard, run hard clipping code
            //Also logic to light up led at pin whatever to indicate hard clipping
            update_distortion_hard_clip(hardFilt,inversedDistortionLevel);
            update_volume_hard_clip(hardFilt, amplificationLevel);
            calc_hard_clip( hardFilt, input1, output1);
            //I guess if it's stereo,
            //calc_hard_clip( hardFilt, input2, output2);
        }
        //then have to do some sort of overall amplitude based on s->volumeLevel to the outputs, either way. This saves some lines of code in each calc function.
        //Why define the same thing twice when you can just define it once.

        PA6_RESET();  // (falling edge on PA6: Done processing data )
    
        //Stage the output arrays back to the DAC to streem the output
        putblockstereo(output1, distortionDial);
    }
    else{
        GPIOC->BSRR = (1 <<  PEDAL_OFF_LED_PIN);
        GPIOC->BSRR = (1 << (PEDAL_ON_LED_PIN + 16));
        GPIOC->BSRR = (1 << (PEDAL_SOFT_DIST_LED_PIN + 16));
        GPIOC->BSRR = (1 << (PEDAL_HARD_DIST_LED_PIN + 16));
        putblockstereo(input1, distortionDial);
    }
  }  /// end of while(1)... time to go wait for the next block of input samples...
}
