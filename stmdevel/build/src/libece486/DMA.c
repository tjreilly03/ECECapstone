#include "DMA.h"
#include "ADC.h"
#include "DAC.h"

// Samples per ADC Transfer
extern uint32_t ADC_Sample_Size;

// DMA Setting:
//   DMA 1 Channel 1  <--->  ADC1 (Master). We do not need DMA for ADC2 (slave).
//   DMA 2 Channel 4  <--->  DAC 1
//   DMA 2 Channel 5  <--->  DAC 2Ish


//-------------------------------------------------------------------------------------------------------------------
// 	Configuration DMA for ADC
//-------------------------------------------------------------------------------------------------------------------
void ADC_DMA_Configuration (DMA_Channel_TypeDef * DMA_Channelx, uint32_t * pBuffer) {
	
	// ADC1: DMA1 Channel 1 or DMA2 Channel 3
	// ADC2: DMA1 Channel 2 or DMA2 Channel 4
	// ADC3: DMA1 Channel 3 or DMA2 Channel 5
	
	int wait_time;
	
	if (( RCC->AHB1ENR & RCC_AHB1ENR_DMA1EN) == 0) {
		RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
		//RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;
	
		wait_time = 20 * (80000000 / 1000000);
		while(wait_time != 0) {
			wait_time--;
		} 
		//RCC->AHB1RSTR |= RCC_AHB1RSTR_DMA1RST;
	}
	
	// DMA1 channel 1 configuration for ADC1
	DMA_Channelx->CCR &= ~DMA_CCR_MEM2MEM; // Disable memory to memory mode
	
	DMA_Channelx->CCR &= ~DMA_CCR_PL;      // Channel priority level, 
	DMA_Channelx->CCR |=  DMA_CCR_PL_1;		 // 00 = Low, 01 = Medium, 10 = High, 11 = Very high

	DMA_Channelx->CCR &= ~DMA_CCR_PSIZE;   // Peripheral size: 00 = 8-bits, 01 = 16-bits, 10 = 32-bits, 11 = Reserved
	DMA_Channelx->CCR |=  DMA_CCR_PSIZE_1; // Size = 32 bits	
		
	DMA_Channelx->CCR &= ~DMA_CCR_MSIZE;   // Memory size: 00 = 8-bits, 01 = 16-bits, 10 = 32-bits, 11 = Reserved
	DMA_Channelx->CCR |=  DMA_CCR_MSIZE_1; // Size = 32 bits
		
	DMA_Channelx->CCR &= ~DMA_CCR_PINC;    // Peripheral increment mode (0 = disabled, 1 = enabled)
	
	DMA_Channelx->CCR |= DMA_CCR_MINC;     // Memory increment mode (0 = disabled, 1 = enabled)
	
	DMA_Channelx->CCR |= DMA_CCR_CIRC;     // Circular mode (0 = disabled, 1 = enabled)
	
	DMA_Channelx->CCR &= ~DMA_CCR_DIR;     // Data transfer direction (0: Read from peripheral, 1: Read from memory)
	
	DMA_Channelx->CNDTR = ADC_Sample_Size; // Number of data to transfer
	
	DMA_Channelx->CPAR  = (uint32_t) &(ADC123_COMMON->CDR); // Peripheral address register
	DMA_Channelx->CMAR  = (uint32_t) pBuffer; 	            // Memory address register
	
	// DMA Channel Selection
	DMA1_CSELR->CSELR &= ~DMA_CSELR_C1S;    // 0000: Channel 1 mapped on ADC1	
	                                        // Dual-mode ADC, one DMA request from the master. 
																					// The slave does not generate any DMA request (see MDMA).
	
	DMA_Channelx->CCR |= DMA_CCR_TCIE;     // Transfer complete interrupt enable
	DMA_Channelx->CCR &= ~DMA_CCR_HTIE;    // Disable Half transfer interrupt
	
	DMA_Channelx->CCR |= DMA_CCR_EN;       // Enable DMA channel
	
}


// DAC1: DMA1 Channel 3, or DMA2 Channel 4
// DAC2: DMA1 Channel 4, or DMA2 Channel 4
//-------------------------------------------------------------------------------------------------------------------
// 	Configure DMA for DAC
//-------------------------------------------------------------------------------------------------------------------
void DAC_DMA_Configuration(DMA_Channel_TypeDef * DMA_Channelx, uint16_t * pBuffer, uint32_t * pPeripheralAddr){
	int wait_time;
	
	if (( RCC->AHB1ENR & RCC_AHB1ENR_DMA1EN) == 0) {
		RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;	
		wait_time = 20 * (80000000 / 1000000);
		while(wait_time != 0) {
			wait_time--;
		} 
		//RCC->AHB1RSTR |= RCC_AHB1RSTR_DMA1RST;
	}
	
	if (( RCC->AHB1ENR & RCC_AHB1ENR_DMA2EN) == 0) {
		RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;	
		wait_time = 20 * (80000000 / 1000000);
		while(wait_time != 0) {
			wait_time--;
		} 
		//RCC->AHB1RSTR |= RCC_AHB1RSTR_DMA2RST;
	}
	
	DMA_Channelx->CCR &= ~DMA_CCR_EN;        // Disable DMA channel
	
	// DMA1 channel 1 configuration for ADC1
	DMA_Channelx->CCR &= ~DMA_CCR_MEM2MEM; // Disable memory to memory mode
	
	DMA_Channelx->CCR &= ~DMA_CCR_PL;      // Channel priority level, 
	DMA_Channelx->CCR |=  DMA_CCR_PL_1;		 // 00 = Low, 01 = Medium, 10 = High, 11 = Very high
	
	DMA_Channelx->CCR &= ~DMA_CCR_PSIZE;   // Peripheral size: 00 = 8-bits, 01 = 16-bits, 10 = 32-bits, 11 = Reserved
	DMA_Channelx->CCR |=  DMA_CCR_PSIZE_0; // Size = 16 bits	
		
	DMA_Channelx->CCR &= ~DMA_CCR_MSIZE;   // Memory size: 00 = 8-bits, 01 = 16-bits, 10 = 32-bits, 11 = Reserved
	DMA_Channelx->CCR |=  DMA_CCR_MSIZE_0; // Size = 16 bits
		
	DMA_Channelx->CCR &= ~DMA_CCR_PINC;    // Peripheral increment mode (0 = disabled, 1 = enabled)
	DMA_Channelx->CCR |=  DMA_CCR_MINC;    // Memory increment mode (0 = disabled, 1 = enabled)
	
	DMA_Channelx->CCR |= DMA_CCR_CIRC;     // Circular mode (0 = disabled, 1 = enabled)
	DMA_Channelx->CCR |= DMA_CCR_DIR;      // Data transfer direction (0: Read from peripheral, 1: Read from memory)
	
	DMA_Channelx->CNDTR = ADC_Sample_Size;  					// Number of data to transfer
	
	DMA_Channelx->CPAR  = (uint32_t) pPeripheralAddr; // Peripheral address register
	DMA_Channelx->CMAR  = (uint32_t) pBuffer; 				// Memory address register
	
	// DMA Channel Selection
	
	if (DMA_Channelx == DMA1_Channel3) {
		DMA1_CSELR->CSELR &= ~DMA_CSELR_C3S;
		// DMA1: 0110: Channel 3 mapped on TIM6_UP/DAC1
		DMA1_CSELR->CSELR |= 6U << 8;  // 
	} else if (DMA_Channelx == DMA1_Channel4) { // DMA1_Channel5
		DMA1_CSELR->CSELR &= ~DMA_CSELR_C4S;   
		// DMA1: 0101: Channel 4 mapped on TIM7_UP/DAC2
		DMA1_CSELR->CSELR |= 5U << 12;  
	} else	if (DMA_Channelx == DMA2_Channel4) {
		DMA2_CSELR->CSELR &= ~DMA_CSELR_C4S;
		// DMA2: 0011: Channel 3 mapped to DAC1
		DMA2_CSELR->CSELR |= 3U << 12;  // 
	} else if (DMA_Channelx == DMA2_Channel5) { // DMA1_Channel5
		DMA2_CSELR->CSELR &= ~DMA_CSELR_C5S;   
		// DMA2: 0011: Channel 3 mapped to DAC2
		DMA2_CSELR->CSELR |= 3U << 16;  
	}
	
	DMA_Channelx->CCR |= DMA_CCR_TCIE;      // Transfer complete interrupt enable
	DMA_Channelx->CCR &= ~DMA_CCR_HTIE;    	// Disable Half transfer interrupt
	
	DMA_Channelx->CCR |= DMA_CCR_EN;        // Enable DMA channel
}

