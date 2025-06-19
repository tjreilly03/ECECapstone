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
	float volumeLevel,
	int blocksize
    	//Same variables as struct as input,
	//Same variables as struct as input,
	//Same variables as struct as input,
)
{
  SOFT_CLIP_T *s;
  //initialize the filter structure
  s = (SOFT_CLIP_T *)malloc( sizeof(SOFT_CLIP_T) );
  s->distortionLevel = -distortionLevel;
  s->volumeLevel = volumeLevel;
    s->blocksize = blocksize;

  return(s);
}


void calc_soft_clip(SOFT_CLIP_T* s, float* x_in, float* y_out)
{
	//Whatever calc is needed to create the hard clipping func
	//Find max of the outputs, use that to find the maximum amplitude based off of the distortion level

	//Do some calculation based off of the s->distortionLevel and s->volumeLevel
	  //Basically it will be like, get the value, find the difference from the limit set by the s->distortionLevel,
	//and if it x[n] is greater than that value, just set it to that value. This will create the hard clipped output.
	float limit = s->distortionLevel;
	float difference = 0.0f;
	for (int i = 0; i < s->blocksize; i++) {
		//printf("x_in[%d] = %f.\n Distortion level is %f.\n",i, x_in[i], limit);
		if (x_in[i] > limit) {
			//find the difference
			difference = x_in[i] - limit;

			y_out[i] = limit + (difference/8.0f);
		}
		else if (x_in[i] < -limit) {
			difference = x_in[i] + limit;
			y_out[i] = -limit + (difference / 8.0f);
		}
		else {
			y_out[i] = x_in[i];
		}
	}
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
