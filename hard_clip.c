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
#include "hard_clip.h"
#include <math.h>


HARD_CLIP_T *init_hard_clip(
	float distortionLevel,
	float volumeLevel
    	//Same variables as struct as input,
	//Same variables as struct as input,
	//Same variables as struct as input,
)
{
  HARD_CLIP_T *s;
  //initialize the filter structure
  s = (HARD_CLIP_T *)malloc( sizeof(HARD_CLIP_T) );
  s->distortionLevel = distortionLevel;
  s->volumeLevel = volumeLevel;
  return(s);
}


void calc_hard_clip( HARD_CLIP_T *s, float *x_in, float *y_out)
{
  //Whatever calc is needed to create the hard clipping func
  //Find max of the outputs, use that to find the maximum amplitude based off of the distortion level

  //Do some calculation based off of the s->distortionLevel and s->volumeLevel
    //Basically it will be like, get the value, find the difference from the limit set by the s->distortionLevel,
  //and if it x[n] is greater than that value, just set it to that value. This will create the hard clipped output.
}

void update_distortion_hard_clip( HARD_CLIP_T *s, float new_distortion)
{
  s->distortionLevel = new_distortion;
}

void update_volume_hard_clip( HARD_CLIP_T *s, float new_volume)
{
  s->volumeLevel = new_volume;
}

void destroy_hard_clip(HARD_CLIP_T *s)
{
  free(s);
}
