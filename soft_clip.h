/*!
 * @file soft_clip.h
 * 
 * @brief This will hold the function defs for the soft clipping portion of the pedal.
 * 
 * @author Tyler Reilly
 * 
 * @date April 2025
 * 
 * @defgroup Function Interface Requirements for Soft Clip Filters
 *   @{
 * 
 * 
      }@
 */


#ifndef SOFT_CLIP
#define SOFT_CLIP

/*
 * Parameter Structure Definitions
 */

/*!
 * @brief Soft Clip filter structure containing whatever is needed for the function
 * 
 * 
 */
typedef struct your_soft_clip_struct {
		float distortionLevel;
		float volumeLevel;
		//Some variables here
		//Some variables here

} SOFT_CLIP_T;
/*
 * Function Prototypes
 */

/*!
 * @brief FIR filter initialization 
 * 
 * Call init_fir() to create and initialize an FIR filter structure.  The
 * input array @a fir_coefs[i] should contain the ith sample of the impulse
 * response of the filter: h[i], i=0,1,...,n_coef-1.
 * 
 * @returns pointer to an initialized structure of type #FIR_T which may 
 * be used to calculate the requested filter outputs.
 */
SOFT_CLIP_T *init_soft_clip(
	float distortionLevel,
	float volumeLevel
	//Same variables as struct as input,
	//Same variables as struct as input,
	//Same variables as struct as input,
);

/*!
 * @brief Calculate a block of output samples of the soft clipping from an input sample block
 */ 
void calc_soft_clip(
  SOFT_CLIP_T *s,      	//!< [in,out] Pointer to previously initialized FIR filter structure,
                        //!< as provided by init_fir() (and possibly modified by 
		        //!< previous calls to calc_fir() )
  float *x,             //!< [in] pointer to the input sample array
  float *y              //!< [out] Pointer to an array for storage of output samples
);

void update_distortion_soft_clip(
	SOFT_CLIP_T *s,
	float new_distortion
);

void update_volume_soft_clip(
	SOFT_CLIP_T *s,
	float new_volume
);

/*!
 * @brief Release memory associated with an #FIR_T
 */
void destroy_soft_clip(
  SOFT_CLIP_T *s		//!< [in] Pointer to previously initialized Soft Clip filter
			//!< structure, provided by init_soft_clip()
);

#endif
