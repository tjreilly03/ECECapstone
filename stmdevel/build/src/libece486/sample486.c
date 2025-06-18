/*!
 * @file
 * 
 * @brief Implementation of user sampler interface functions
 * @author Don Hummels
 * @date Jan 2025
 */

#include <stdlib.h>
#include "sample486.h"
#include "ece486.h"
#include "arm_math.h"

// This demo code takes two analog inputs (PA0 and PA1), and simply streams them out
// as two analog outputs (PA4 and PB0). Specifically:
//  (1) copy the input from ADC1_IN5 (PA0) to DAC1_OUT1 (PA4)
//  (2) copy the input from ADC1_IN6 (PA1) to DAC1_OUT2 
// The DAC1_OUT2 output is buffered through a unity gain buffer to PB0
//
// ADC1 and ADC2 work in the dual mode, and their results are saved in a 32-bit value.
// Therefore, there are no separated buffers for ADC1 and ADC2.
//
// Both ADCs and DACs are triggered synchronously by TIM4_CH1 (PB6). ADCs and DACs
// run at 10K samples/second.
//
// The data are moved via DMA using 3 pairs of ping-pong buffers
//  (1) ADC_Buffer_Ping and ADC_Buffer_Pong   (pADC_Ready_to_Read_Buffer indicates which buffer is full)
//  (2) DAC1_Buffer_Ping and DAC1_Buffer_Pong (pDAC1_Ready_to_Write_Buffer indicates the empty buffer)
//  (3) DAC2_Buffer_Ping and DAC2_Buffer_Pong (pDAC2_Ready_to_Write_Buffer indicates the empty buffer)

extern volatile uint32_t ADC_DMA_Done;

extern uint16_t *pDAC1_Ready_to_Write_Buffer;
extern uint16_t *pDAC2_Ready_to_Write_Buffer;
extern uint32_t *pADC_Ready_to_Read_Buffer;


/*
 * Data Block sizes for streamed ADC/DAC data
 */
extern uint32_t ADC_Sample_Size;	//!< Number of samples user accesses per data block


/*
	Simple function to return block size.
	Needed for creating a working buffer.
*/
int getblocksize()
{
  return ADC_Sample_Size;
}

/*
 * Set the number of samples that the user should expect to process per block
 * 
 */
void setblocksize( uint32_t blksiz )
{
  /*
   * setblocksize() should only be called before calling initialize_ece486().
   * If the ADC & DAC buffers have already been allocated, then initialize()
   * must have already been called, and we're too late to change the buffer
   * sizes.  Flag an error and return without changing anything.
   */
//   if (pADC_Ready_to_Read_Buffer != NULL) {
//     flagerror(SETBLOCKSIZE_ERROR);
//     return;
//   }
  
  ADC_Sample_Size = blksiz;
}



void putblockstereo(float * chan1, float * chan2)
{
	uint32_t i;
  
  // the "outbuf" pointer is set by getblock() to indicate the 
  // appropriate destination of any output samples.
  //
  // floating point values between -1 and +1 are mapped 
  // into the range of the DAC
  //
  // chan1 goes into the most-significant 16 bits (DAC1), 
  // chan2 in the least significant (DAC1)
  for (i=0; i<ADC_Sample_Size; i++) {
    pDAC1_Ready_to_Write_Buffer[i] = (((uint16_t)((chan1[i]+1.0)*2048.0f)) & 0x0000ffff);
    pDAC2_Ready_to_Write_Buffer[i] = (((uint16_t)((chan2[i]+1.0)*2048.0f)) & 0x0000ffff);
  }
}

void getblockstereo(float *chan1, float *chan2)
{
  uint32_t i;
  int samp1,samp2;  // signed integers used in coversion to floats.

  //Error Check Needed Here...
  // IF ADC_DMA_Done = 1 when we get here, there has been a buffer overrun. The DMA ISR sets
  // this to 1 when a transfer is complete...  Ideally, the value is 0 when we reach this part 
  // of the code, implying that we've processed a complete transfer of data before we stat to 
  // wait on the next transfer.
  //
  // If we see this, indicate the "Buffer Overrun" error by doubling the LED Flash Rate
  // to 2 beats/second
  if (ADC_DMA_Done == 1) Set_Heartbeat_Count(250);
  
  while(ADC_DMA_Done == 0);  // Wait until ADC_DMA_Done is set by DMA1_Channel1_IRQHandler
  
  ADC_DMA_Done = 0; // Now reset the ADC_DMA_Done flag so that we can process the data and will 
                    // appropriately wait for the next data transfer on the next call to this funciton.
                    

  // Now convert the valid ADC data into the caller's arrays of floats.
  // Samples are normalized to range from -1.0 to 1.0
  // Channel 1 is in the least significant 16 bits of the DMA transfer data,
  // Channel 2 is in the most significant 16 bits.
  // (values are actually 12 bits, right justified in the 16-bit values, I hope)
  for (i=0; i< ADC_Sample_Size; i++) {
    samp1 = (int)(pADC_Ready_to_Read_Buffer[i]&0x0000ffff);
    samp2 = (int)((pADC_Ready_to_Read_Buffer[i]&0xffff0000)>>16);
    
    // 1/2048 =  4.8828125e-04   (Avoiding division)
    chan1[i] = ((float)(samp1-2048)) * 4.8828125e-04f;
    chan2[i] = ((float)(samp2-2048)) * 4.8828125e-04f;
  }
}


