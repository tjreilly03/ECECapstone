#include "ADC.h"
#include "DAC.h"
#include "DMA.h"
#include "OPAMP.h"
#include "SysTimer.h"

#include "stm32l476xx.h"
#include <stdint.h>
#include <stdlib.h>

// Samples per ADC Transfer
extern uint32_t ADC_Sample_Size;

uint16_t *DAC1_Buffer_Ping;
uint16_t *DAC1_Buffer_Pong;
uint16_t *DAC2_Buffer_Ping;
uint16_t *DAC2_Buffer_Pong;

uint16_t *pDAC1_Working_Buffer;
uint16_t *pDAC2_Working_Buffer;

uint16_t *pDAC1_Ready_to_Write_Buffer;
uint16_t *pDAC2_Ready_to_Write_Buffer;

//-------------------------------------------------------------------------------------------------------------------
// DAC Initialization
//-------------------------------------------------------------------------------------------------------------------
void DAC_Init(void){
	int i;
	
	// request memory for the ADC DMA transfer buffers
    // (These integer buffers store the raw ADC and DAC samples.  The values
    //  are converted to float's before being handed to the user.)
    DAC1_Buffer_Ping = (uint16_t *)malloc(sizeof(uint16_t)*ADC_Sample_Size);
    DAC1_Buffer_Pong = (uint16_t *)malloc(sizeof(uint16_t)*ADC_Sample_Size);
    DAC2_Buffer_Ping = (uint16_t *)malloc(sizeof(uint16_t)*ADC_Sample_Size);
    DAC2_Buffer_Pong = (uint16_t *)malloc(sizeof(uint16_t)*ADC_Sample_Size);
	
	for (i = 0; i < ADC_Sample_Size; i++){	
		DAC1_Buffer_Ping[i] = (uint16_t)(i * 4095.0/ADC_Sample_Size);
		DAC1_Buffer_Pong[i] = 4095.0 - i * 4095.0/ADC_Sample_Size;
		DAC2_Buffer_Ping[i] = i * 4095.0/ADC_Sample_Size;
		DAC2_Buffer_Pong[i] = 4095.0 - i * 4095.0/ADC_Sample_Size;		
	}
	
	pDAC1_Working_Buffer = &(DAC1_Buffer_Ping[0]);
	pDAC2_Working_Buffer = &(DAC2_Buffer_Ping[0]);
	
	DAC_Pin_Configuration();
	
	RCC->APB1ENR1 |= RCC_APB1ENR1_DAC1EN;  // Enable DAC Clock
	
	// DAC channel 1 12-bit right-aligned data holding register (DAC_DHR12R1)
	DAC_DMA_Configuration(DMA2_Channel4, pDAC1_Working_Buffer, (uint32_t *) &(DAC->DHR12R1));
	// DAC channel 2 12-bit right aligned data holding register (DAC_DHR12R2)
	DAC_DMA_Configuration(DMA2_Channel5, pDAC2_Working_Buffer, (uint32_t *) &(DAC->DHR12R2));
	
	DAC_Configuration();
	
	NVIC_EnableIRQ(TIM6_DAC_IRQn);
	NVIC_EnableIRQ(DMA2_Channel4_IRQn);
	NVIC_EnableIRQ(DMA2_Channel5_IRQn);
	
	OPAMP_Init(); // Op amp used to send DAC Output 2 to PB0
	
}
	
//-------------------------------------------------------------------------------------------------------------------
// DAC Pin Initialization
//-------------------------------------------------------------------------------------------------------------------
void DAC_Pin_Configuration(void){
	// Enable the clock of GPIO Port A
	RCC->AHB2ENR |=   RCC_AHB2ENR_GPIOAEN;
	
	// DMH...  Don't mess with PA5 here...  The DAC Oubput 2 will drive only an opamp peripherals
	// PA5 is tied to the green LED, and will get a separate configuration there.

	// GPIO Mode: Input(00, reset), Output(01), AlterFunc(10), Analog(11, reset)
	// GPIO Speed: Low speed (00), Medium speed (01), Fast speed (10), High speed (11)
	// GPIO Push-Pull: No pull-up, pull-down (00), Pull-up (01), Pull-down (10), Reserved (11)
	// GPIO Output Type: Output push-pull (0, reset), Output open drain (1)
	
	// Configure PA4 (DAC1_OUT1), PA5 (DAC1_OUT2) as Analog
//	GPIOA->MODER |=   3U<<(2*4) | 3U<<(2*5);  // Mode 11 = Analog
//	GPIOA->PUPDR &= ~(3U<<(2*4) | 3U<<(2*5)); // No pull-up, no pull-down	
	GPIOA->MODER |=   3U<<(2*4) ;  // Mode 11 = Analog
	GPIOA->PUPDR &= ~(3U<<(2*4)) ; // No pull-up, no pull-down	
	
	// GPIO port analog switch control register (ASCR)
	// 0: Disconnect analog switch to the ADC input (reset state)
	// 1: Connect analog switch to the ADC input
//	GPIOA->ASCR |= GPIO_ASCR_EN_4 | GPIO_ASCR_EN_5;
	GPIOA->ASCR |= GPIO_ASCR_EN_4 ;
}


//-------------------------------------------------------------------------------------------------------------------
// DAC Configuration
//-------------------------------------------------------------------------------------------------------------------
void DAC_Configuration(void){
	
	DAC_Calibration_Channel(1);  // Calibrate DAC Channel 1
	DAC_Calibration_Channel(2);  // Calibrate DAC Channel 2
	
	// Enable DMA
	DAC->CR |= DAC_CR_DMAEN1 | DAC_CR_DMAEN2; 
	
	// Enable DMA underrun interrupt  
	DAC->CR |= DAC_CR_DMAUDRIE1 | DAC_CR_DMAUDRIE2;
	
	// DAC mode control register (DAC_MCR)
//	DAC->MCR &= ~DAC_MCR_MODE1;   // 
//	DAC->MCR |=  DAC_MCR_MODE1_0; // 001: DAC Channel 2 is connected to external pin and to on chip peripherals with buffer enabled
//	DAC->MCR &= ~DAC_MCR_MODE2;   // 000: DAC Channel 2 is connected to external pin with Buffer enabled
	// DMH -- Changes here to us op-amp buffer for channel 2 (Moving output from PA5 to PB0)
	// (Green LED is already on PA5)
	DAC->MCR &= ~DAC_MCR_MODE1;   // 
	DAC->MCR |=  DAC_MCR_MODE1_0; // 001: DAC Channel 1 is connected to external pin and to on chip peripherals with buffer enabled
	DAC->MCR &= ~DAC_MCR_MODE2;   // 000: DAC Channel 2 is connected to external pin with Buffer enabled
	//DAC->MCR |=  DAC_MCR_MODE2_0; // 001: DAC Channel 2 is connected to external pin and to on chip peripherals with buffer enabled
	DAC->MCR |=  (3U<<16);	        // DMH Change to (3U<<16) for 011 connect to on-chip periferals with buffer disabled to free up PA5.
	
	// DAC Trigger selection
	// 000: Timer 6 TRGO event
	// 001: Timer 8 TRGO event
	// 010: Timer 7 TRGO event
	// 011: Timer 5 TRGO event
	// 100: Timer 2 TRGO event
	// 101: Timer 4 TRGO event
	// 110: External line9
	// 111: Software trigger	
	DAC->CR |= DAC_CR_TEN1 | DAC_CR_TEN2;       // Trigger enable 
	DAC->CR &= ~(DAC_CR_TSEL1 | DAC_CR_TSEL2);
	// Select 101 TIM4_TRGO as DAC triggers for Channel 1 and 2
	DAC->CR |= (DAC_CR_TSEL1_0 | DAC_CR_TSEL1_2 | DAC_CR_TSEL2_0 | DAC_CR_TSEL2_2); 
		
	DAC->CR |= DAC_CR_EN1 | DAC_CR_EN2;       // Enable DAC Channel 1 and 2
	
	delay(1);
}

//-------------------------------------------------------------------------------------------------------------------
// DAC Calibration
//-------------------------------------------------------------------------------------------------------------------
void DAC_Calibration_Channel(uint32_t channel){
	
	uint32_t trimmingvalue, delta, offset;
	uint32_t DAC_CR_CEN_Flag, DAC_CCR_OTRIM_Flag, DAC_SR_CAL_Flag;
	
	if (channel == 1) {
		DAC_CR_CEN_Flag = DAC_CR_CEN1;
		DAC_CCR_OTRIM_Flag  = DAC_CCR_OTRIM1;
		DAC_SR_CAL_Flag = DAC_SR_CAL_FLAG1;
		offset = 0;
	} else {
		DAC_CR_CEN_Flag = DAC_CR_CEN2;
		DAC_CCR_OTRIM_Flag  = DAC_CCR_OTRIM2;
		DAC_SR_CAL_Flag = DAC_SR_CAL_FLAG2;
		offset = 16;
	}
	
	if (channel == 1) {
		DAC->CR &= ~DAC_CR_EN1;  // Ensure DAC 1 is off
	} else {
		DAC->CR &= ~DAC_CR_EN2;  // Ensure DAC 2 is off
	}
	
	// Enable DAC Channel calibration 
	DAC->CR |=  DAC_CR_CEN_Flag;  
	
	/* Init trimming counter */    
  /* Medium value */
  trimmingvalue = 16; 
  delta = 8;
  while (delta != 0) {
		
    /* Set candidate trimming */
		// DAC calibration control register (DAC_CCR)
		DAC->CCR &= ~DAC_CCR_OTRIM_Flag;
		DAC->CCR |= ((trimmingvalue<<offset) & DAC_CCR_OTRIM_Flag);
  
    /* tOFFTRIMmax delay x ms as per datasheet (electrical characteristics */ 
    /* i.e. minimum time needed between two calibration steps */
    delay(1);
  
		if ((DAC->SR & DAC_SR_CAL_Flag) == 0) 
			/* DAC_SR_CAL_FLAGx is HIGH, try higher trimming */
			trimmingvalue += delta;
		else
			trimmingvalue -= delta;
		   
		delta >>= 1;
	}
	
	/* Still need to check if right calibration is current value or one step below */
	/* Indeed the first value that causes the DAC_SR_CAL_FLAGx bit to change from 0 to 1  */
	/* Set candidate trimming */
	DAC->CCR &= ~DAC_CCR_OTRIM_Flag;
	DAC->CCR |= ((trimmingvalue<<offset) & DAC_CCR_OTRIM_Flag);
  
	/* tOFFTRIMmax delay x ms as per datasheet (electrical characteristics */ 
	/* i.e. minimum time needed between two calibration steps */
  delay(1);
    
	if ((DAC->SR & DAC_SR_CAL_Flag) == 0) { 
		/* OPAMP_CSR_OUTCAL is actually one value more */
		trimmingvalue++;
		/* Set right trimming */
		DAC->CCR &= ~DAC_CCR_OTRIM_Flag;
		DAC->CCR |= ((trimmingvalue<<offset) & DAC_CCR_OTRIM_Flag);
	}
	
	DAC->CR &= ~DAC_CR_CEN_Flag; 
}



// DAC->DHR12R1
// DAC->DHR12R2

//-------------------------------------------------------------------------------------------------------------------
// DMA Interrupt Handler for DAC 1
//-------------------------------------------------------------------------------------------------------------------
void DMA2_Channel4_IRQHandler(void){	
	
	NVIC_ClearPendingIRQ(DMA2_Channel4_IRQn);
	
	if ( (DMA2->ISR & DMA_ISR_TCIF4) == DMA_ISR_TCIF4 ) {
		DMA2->IFCR |= DMA_IFCR_CTCIF4; // Write 1 to clear the corresponding TCIF flag in DMA_ISR 
		if (pDAC1_Working_Buffer == DAC1_Buffer_Ping){
			pDAC1_Working_Buffer = DAC1_Buffer_Pong;		
			pDAC1_Ready_to_Write_Buffer   = DAC1_Buffer_Ping;
		}else{
			pDAC1_Working_Buffer = DAC1_Buffer_Ping;
			pDAC1_Ready_to_Write_Buffer   = DAC1_Buffer_Pong;
		}
		DMA2_Channel4->CMAR = (uint32_t) pDAC1_Working_Buffer;
	}
	
	if ( (DMA2->ISR & DMA_ISR_HTIF4) == DMA_ISR_HTIF4 ) // half transfer
		DMA2->IFCR |= DMA_IFCR_CHTIF4; 
	
	if ( (DMA2->ISR & DMA_ISR_GIF4) == DMA_ISR_GIF4 ) // global interrupt
		DMA2->IFCR |= DMA_IFCR_CGIF4; 
	
	if ( (DMA2->ISR & DMA_ISR_TEIF4) == DMA_ISR_TEIF4 ) // transfer error
		DMA2->IFCR |= DMA_IFCR_CTEIF4; 
}

//-------------------------------------------------------------------------------------------------------------------
// DMA Interrupt Handler for DAC 2
//-------------------------------------------------------------------------------------------------------------------
void DMA2_Channel5_IRQHandler(void){	
	
	NVIC_ClearPendingIRQ(DMA2_Channel5_IRQn);	
	
	if ( (DMA2->ISR & DMA_ISR_TCIF5) == DMA_ISR_TCIF5 ) { // transfer complete
		DMA2->IFCR |= DMA_IFCR_CTCIF5; // Write 1 to clear the corresponding TCIF flag in DMA_ISR 
		if (pDAC2_Working_Buffer == DAC2_Buffer_Ping){
			pDAC2_Working_Buffer = DAC2_Buffer_Pong;
			pDAC2_Ready_to_Write_Buffer = DAC2_Buffer_Ping;
		}else{
			pDAC2_Working_Buffer = DAC2_Buffer_Ping;
			pDAC2_Ready_to_Write_Buffer = DAC2_Buffer_Pong;
		}
		DMA2_Channel5->CMAR = (uint32_t) pDAC2_Working_Buffer;
	}
	
	if ( (DMA2->ISR & DMA_ISR_HTIF5) == DMA_ISR_HTIF5 ) // half transfer
		DMA2->IFCR |= DMA_IFCR_CHTIF5; 
	
	if ( (DMA2->ISR & DMA_ISR_GIF5) == DMA_ISR_GIF5 ) // global interrupt
		DMA2->IFCR |= DMA_IFCR_CGIF5; 
	
	if ( (DMA2->ISR & DMA_ISR_TEIF5) == DMA_ISR_TEIF5 ) // transfer error
		DMA2->IFCR |= DMA_IFCR_CTEIF5; 
}

//-------------------------------------------------------------------------------------------------------------------
// DAC DMA Underrun IRQHandler
//-------------------------------------------------------------------------------------------------------------------
void TIM6_DAC_IRQHandler(void){
	
	if ((DAC->SR & DAC_SR_DMAUDR2) == DAC_SR_DMAUDR2){
		DAC->SR |= DAC_SR_DMAUDR2; // Write 1 to clear
	}
	
	if ((DAC->SR & DAC_SR_DMAUDR1) == DAC_SR_DMAUDR1){
		DAC->SR |= DAC_SR_DMAUDR1; // Write 1 to clear
	}
	
}
