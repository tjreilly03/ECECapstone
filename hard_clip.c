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
    	//Same variables as struct as input,
	//Same variables as struct as input,
	//Same variables as struct as input,
)
{
  HARD_CLIP_T *s;
  //initialize the filter structure
  s = (HARD_CLIP_T *)malloc( sizeof(HARD_CLIP_T) );
  return(s);
}


void calc_hard_clip( HARD_CLIP_T *s, float *x_in, float *y_out)
{
  //Whatever calc is needed to create the hard clipping func
}


void destroy_hard_clip(HARD_CLIP_T *s)
{
  free(s);
}
