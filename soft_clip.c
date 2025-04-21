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
#include "soft_clip.h"
#include <math.h>


SOFT_CLIP_T *init_soft_clip(
	float distortionLevel,
	float volumeLevel
    	//Same variables as struct as input,
	//Same variables as struct as input,
	//Same variables as struct as input,
)
{
  SOFT_CLIP_T *s;
  //initialize the filter structure
  s = (SOFT_CLIP_T *)malloc( sizeof(SOFT_CLIP_T) );
  s->distortionLevel = distortionLevel;
  s->volumeLevel = volumeLevel;
  return(s);
}


void calc_soft_clip( SOFT_CLIP_T *s, float *x_in, float *y_out)
{
  //Whatever calc is needed to create the soft clipping func
  //Do some sort of calculation here.
  //Basically it will be like, get the value, find the difference from the limit set by the s->distortionLevel,
  //and then multiply by some fraction, like 1/8 or 1/4 to make the peak lower, but still a nice soft wave, not hard clipped.
}

void update_distortion_soft_clip( SOFT_CLIP_T *s, float new_distortion)
{
  s->distortionLevel = new_distortion;
}

void update_volume_soft_clip( SOFT_CLIP_T *s, float new_volume)
{
  s->volumeLevel = new_volume;
}

void destroy_soft_clip(SOFT_CLIP_T *s)
{
  free(s);
}
