/*!
 * @file
 * 
 * @brief ECE 486 Interface fuctions to manipulate blocks of sampled data
 * @author Don Hummels
 * @date Jan 2025
 * 
 * Interface routines that users call to handle data samples which are being 
 * streamed from the ADC and back to the DAC. 
 *
 * @defgroup ECE486_Sample ECE 486 user interface functions to handle streamed ADC/DAC data
 * @{
 */

#ifndef __SAMPLE_486__
#define __SAMPLE_486__

#include <stdint.h>


/*!
 * @defgroup ECE486_Sample_Functions User interface functions for sampled data
 * @{
 */

/*!
 * @brief Return the number of samples that the user should expect to process per block
 * 
 * Users should call getblocksize() once to determine the required number of samples which 
 * must be handled on each call to getblock() or getblockstereo().  Typically, the result is 
 * used to allocate required memory to handle the data
 * 
 * @return The number of ADC samples supplied per data request
 */
int getblocksize(void);

/*!
 * @brief Set the number of samples that the user should expect to process per block
 * 
 * setblocksize() may be called before calling initialize() to specify
 * the number of samples which should be delivered to the user on each
 * call to getblock() or getblockstereo().
 * 
 * If setblocksize() is not called, the default block size of #DEFAULT_BLOCKSIZE
 * is used for data transfers.  Using a smaller block size (down to a limit of 
 * one sample) can reduced the latency between the input and output stream, at 
 * the cost of some efficiency in handling the data.
 * 
 * @attention setblocksize() must be called before calling initialize().
 * 
 */
void setblocksize(
  uint32_t blksiz	//!< Number of samples per block of ADC/DAC data processed
);




/*!
 * @brief Request a block of data from the two ADC sample streams (Stereo input case)
 * 
 * For two input channel (STEREO_IN), getblockstero() is called by the user to request the
 * next input data blocks.  getblockstereo() will wait until the ADCs/DMA completes filling
 * the requested block of samples.  The binary data is then converted to float values
 * and returned to the user.
 *  
 * Output samples will range from -1.0 to +1.0 to cover
 * the range of the ADC input voltages.  The converter is
 * a 12-bit device, so the resolution of the output samples
 * is roughly 2/4096 = 0.000488.
 * 
 * Assuming the original voltage range of the ADC is 0 to 3.0 volts,
 * the conversion between the returned sample value and the input voltage is:
 * Vin = 1.5 + (sample*1.5).
 * (A zero sample value indicates the ADC is mid-scale, at 1.5 volts)
 * For a 0-3 V ADC range, the ADC resolution is 3/4096 = 732 uV.
 * 
 * @return On return, the memory blocks pointed to by @a chan1 and @a chan2 will be filled with the 
 * requested data blocks.  Samples are normalized to range from -1.0 to 1.0 to cover the 
 * entire range of the ADC.
 * 
 * @attention The user is responsible for allocating enough memory to hold the requested arrays
 * of float values.  The number of samples required for each array may be obtained by 
 * calling getblocksize()
 * 
 * @sa getblocksize(), getblockstereo()
 */
void getblockstereo(
  float * chan1, 	//!< [in,out] pointer to an array of floats for the main ADC channel
  float * chan2 	//!< [in,out] pointer to an array of floats for the stereo ADC channel
);



/*!
 * @brief Send two buffers of data to the DACs  for stereo output
 * 
 * For two output streams (STEREO_OUT), putblockstereo() is called by the user when they've finished
 * processing a block of data and are ready to stream the data to the DACs.  Samples passed
 * to putblock() are assumed to range from -1.0 to 1.0 to cover the entire output range of the 
 * DAC.
 * 
 * @attention The number of output samples required can be determined by calling getblocksize()
 */
void putblockstereo(
  float * chan1,	//!< [in] Array of output samples for the main output channel 
  float * chan2		//!< [in] Array of output samples for the stereo output channel
);

/*! @} End of ECE486_Sample_Functions 
 *  @} End of ECE486_Sample group
 */

#endif
