#include "ADC.h"
#include "DMA.h"
#include "SysTimer.h"
#include "stm32l476xx.h"
#include <stdint.h>
#include <stdlib.h>

/*
 * Data Block sizes for streamed ADC/DAC data
 */
uint32_t ADC_Sample_Size = ADC_DEFAULT_SAMPLE_SIZE;	//!< Number of samples user accesses per data block

uint32_t *ADC_Buffer_Ping = NULL;
uint32_t *ADC_Buffer_Pong = NULL;

uint32_t * pADC_Working_Buffer = NULL;
uint32_t * pADC_Ready_to_Read_Buffer = NULL;

volatile uint32_t ADC_DMA_Done = 0;

// Analog Inputs: 
//    PA0 (ADC12_IN5), PA1 (ADC12_IN6)
// Analog Outputs: 
//    PA4 (DAC1_OUT1), PA5 (DAC1_OUT2)

//-------------------------------------------------------------------------------------------------------------------
// STM32L4x6xx Errata sheet
// When the delay between two consecutive ADC conversions is higher than 1 ms the result of 
// the second conversion might be incorrect. The same issue occurs when the delay between the 
// calibration and the first conversion is higher than 1 ms.
// Workaround
// When the delay between two ADC conversions is higher than the above limit, perform two ADC 
// consecutive conversions in single, scan or continuous mode: the first is a dummy conversion 
// of any ADC channel. This conversion should not be taken into account by the application.

//-------------------------------------------------------------------------------------------------------------------
// Initialize ADC	
//-------------------------------------------------------------------------------------------------------------------	
void ADC_Init(void){
	int i;

  // request memory for the ADC DMA transfer buffers
  // (These integer buffers store the raw ADC and DAC samples.  The values
  //  are converted to float's before being handed to the user.)
  ADC_Buffer_Ping = (uint32_t *)malloc(sizeof(uint32_t)*ADC_Sample_Size);
  ADC_Buffer_Pong = (uint32_t *)malloc(sizeof(uint32_t)*ADC_Sample_Size);
  
	for (i = 0; i < ADC_Sample_Size; i++) {
		ADC_Buffer_Ping[i] = 0;
		ADC_Buffer_Pong[i] = 0;
	}
	
	pADC_Working_Buffer = ADC_Buffer_Ping;
	pADC_Ready_to_Read_Buffer = ADC_Buffer_Pong;
	
	ADC_Pin_Init();
	
	// Enable the clock to ADC peripheral
	RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;
	
	ADC_Common_Configuration();
	
	// ADC is in deep-power-down mode by default.
	ADC_Wakeup(ADC1);
	ADC_Wakeup(ADC2);
		
	// System was in deep power down mode, calibration must be relaunched.
	// Calibration can only be initiated when the ADC is disabled (when ADEN=0).
	ADC_Calibration(ADC1);
	ADC_Calibration(ADC2);	
	
	ADC_Configure(ADC1);
	ADC_Configure(ADC2);
	
	ADC_Regular_Channel_Configuration();
	
	// Only need to configure DMA for ADC1 (master).
	// ADC2 is slave of ADC1.
	ADC_DMA_Configuration (DMA1_Channel1, pADC_Working_Buffer);
	NVIC_EnableIRQ(DMA1_Channel1_IRQn);
	// NVIC_SetPriority(DMA1_Channel1_IRQn, 0);
	
	// NVIC_EnableIRQ(ADC1_2_IRQn);
	
	//ADC1->CR |= ADC_CR_JADSTART;  // Start of injected conversion
	ADC1->CR |= ADC_CR_ADSTART;     // Start of regular conversion
}


//-------------------------------------------------------------------------------------------------------------------
// 	ADC Common Configuration
//-------------------------------------------------------------------------------------------------------------------	
void ADC_Common_Configuration(){
	
	// I/O analog switches voltage booster
	// The I/O analog switches resistance increases when the VDDA voltage is too low. This
	// requires to have the sampling time adapted accordingly (cf datasheet for electrical
	// characteristics). This resistance can be minimized at low VDDA by enabling an internal
	// voltage booster with BOOSTEN bit in the SYSCFG_CFGR1 register.
	SYSCFG->CFGR1 |= SYSCFG_CFGR1_BOOSTEN;
	
	// V_REFINT enable
	ADC123_COMMON->CCR |= ADC_CCR_VREFEN;  
	
	// ADC Clock Source: System Clock, PLLSAI1, PLLSAI2
	// Maximum ADC Clock: 80 MHz
	
	// ADC prescaler to select the frequency of the clock to the ADC
	ADC123_COMMON->CCR &= ~ADC_CCR_PRESC;   // 0000: input ADC clock not divided
	
	// ADC clock mode
	//   00: CK_ADCx (x=123) (Asynchronous clock mode),
	//   01: HCLK/1 (Synchronous clock mode).
	//   10: HCLK/2 (Synchronous clock mode)
	//   11: HCLK/4 (Synchronous clock mode)	 
	ADC123_COMMON->CCR &= ~ADC_CCR_CKMODE;  // HCLK = 80MHz
	ADC123_COMMON->CCR |=  ADC_CCR_CKMODE_0;

	//////////////////////////////////////////////////////////////////////////////////////////////
	// Dual ADC mode	
	ADC123_COMMON->CCR &= ~ADC_CCR_DUAL;
	ADC123_COMMON->CCR |= 6U;  // 00110: Regular simultaneous mode only
	
	//////////////////////////////////////////////////////////////////////////////////////////////
	// DMA requests in dual ADC mode
	// MDMA=0b10: A single DMA request is generated each time both master and slave
	// EOC events have occurred. At that time, two data items are available and the 32-bit
	// register ADCx_CDR contains the two half-words representing two ADC-converted data.
	ADC123_COMMON->CCR &= ~ADC_CCR_MDMA;
	ADC123_COMMON->CCR |=  ADC_CCR_MDMA_1;  // MDMA = 10: MDMA mode enabled for 12 and 10-bit resolution
	
	ADC123_COMMON->CCR |=  ADC_CCR_DMACFG;  // Select DMA Circular Mode
}

//-------------------------------------------------------------------------------------------------------------------
// ADC Wakeup
// By default, the ADC is in deep-power-down mode where its supply is internally switched off
// to reduce the leakage currents.
//-------------------------------------------------------------------------------------------------------------------
void ADC_Wakeup (ADC_TypeDef * ADCx) {
	
	int wait_time;
	
	// To start ADC operations, the following sequence should be applied
	// DEEPPWD = 0: ADC not in deep-power down
	// DEEPPWD = 1: ADC in deep-power-down (default reset state)
	if ((ADCx->CR & ADC_CR_DEEPPWD) == ADC_CR_DEEPPWD)
		ADCx->CR &= ~ADC_CR_DEEPPWD; // Exit deep power down mode if still in that state
	
	// Enable the ADC internal voltage regulator
	// Before performing any operation such as launching a calibration or enabling the ADC, the ADC
	// voltage regulator must first be enabled and the software must wait for the regulator start-up time.
	ADCx->CR |= ADC_CR_ADVREGEN;	
	
	// Wait for ADC voltage regulator start-up time
	// The software must wait for the startup time of the ADC voltage regulator (T_ADCVREG_STUP) 
	// before launching a calibration or enabling the ADC.
	// T_ADCVREG_STUP = 20 us
	wait_time = 20 * (80000000 / 1000000);
	while(wait_time != 0) {
		wait_time--;
	}   
}

//-------------------------------------------------------------------------------------------------------------------
// 	ADC Calibration
//-------------------------------------------------------------------------------------------------------------------
void ADC_Calibration(ADC_TypeDef * ADCx){
	// Ensure ADC is off
	ADCx->CR &= ~ADC_CR_ADEN;         	
	// Wait until ADRDY is reset by hardware
	while((ADCx->ISR & ADC_ISR_ADRDY) == ADC_ISR_ADRDY);  	
	// Calibration for single ended input conversions
	ADCx->CR &= ~ADC_CR_ADCALDIF;  // ADCALDIF: 0 = Single Ended, 1 = Differential Input
	// Each ADC provides an automatic calibration procedure which drives all the calibration 
	// sequence including the power-on/off sequence of the ADC.	
	// Calibration can only be initiated when the ADC is disabled (when ADEN=0).
	ADCx->CR |= ADC_CR_ADCAL; // The calibration is then initiated by software by setting bit ADCAL = 1.	
	// ADCAL bit stays at 1 during all the calibration sequence. 
	// It is then cleared by hardware as soon the calibration completes.
	while ( (ADCx->CR & ADC_CR_ADCAL) == ADC_CR_ADCAL ); // Wait for calibration to complete 
}

//-------------------------------------------------------------------------------------------------------------------
// 	ADC Regular Channel Configuration
//-------------------------------------------------------------------------------------------------------------------
// ADC Triggers
// Regular Channels              |   Injected Channels
// EXT0  TIM1_CC1 event   0000   |   JEXT0  TIM1_TRGO event  0000
// EXT1  TIM1_CC2 event   0001   |   JEXT1  TIM1_CC4 event   0001
// EXT2  TIM1_CC3 event   0010   |   JEXT2  TIM2_TRGO event  0010
// EXT3  TIM2_CC2 event   0011   |   JEXT3  TIM2_CC1 event   0011
// EXT4  TIM3_TRGO event  0100   |   JEXT4  TIM3_CC4 event   0100
// EXT5  TIM4_CC4 event   0101   |   JEXT5  TIM4_TRGO event  0101
// EXT6  EXTI line 11     0110   |   JEXT6  EXTI line 15     0110
// EXT7  TIM8_TRGO event  0111   |   JEXT7  TIM8_CC4 event   0111
// EXT8  TIM8_TRGO2 event 1000   |   JEXT8  TIM1_TRGO2 event 1000
// EXT9  TIM1_TRGO event  1001   |   JEXT9  TIM8_TRGO event  1001
// EXT10 TIM1_TRGO2 event 1010   |   JEXT10 TIM8_TRGO2 event 1010
// EXT11 TIM2_TRGO event  1011   |   JEXT11 TIM3_CC3 event   1011
// EXT12 TIM4_TRGO event  1100   |   JEXT12 TIM3_TRGO event  1100
// EXT13 TIM6_TRGO event  1101   |   JEXT13 TIM3_CC1 event   1101
// EXT14 TIM15_TRGO event 1110   |   JEXT14 TIM6_TRGO event  1110
// EXT15 TIM3_CC4 event   1111   |   JEXT15 TIM15_TRGO event 1111	
//
// ADC Sample Time
// This sampling time must be enough for the input voltage source to charge the embedded
// capacitor to the input voltage level.
// Software is allowed to write these bits only when ADSTART=0 and JADSTART=0
//   000: 2.5 ADC clock cycles      001: 6.5 ADC clock cycles
//   010: 12.5 ADC clock cycles     011: 24.5 ADC clock cycles
//   100: 47.5 ADC clock cycles     101: 92.5 ADC clock cycles
//   110: 247.5 ADC clock cycles    111: 640.5 ADC clock cycles	
	
void ADC_Regular_Channel_Configuration(void){
	
	uint32_t SampleTime = 3;
	
	//////////////////////////////////////////////////////////////////////////////////////////////
	// ADC 1 regular sequence
	// ADC1: PA0: ADC12_IN5 
	ADC1->SQR1 &= ~ADC_SQR1_L;            // ADC regular channel sequence lenght, 0000: 1 conversion 
	ADC1->SQR1 &= ~ADC_SQR1_SQ1;
	ADC1->SQR1 |=  ( 5U << 6 );           // PA0: ADC12_IN5
	ADC1->DIFSEL &= ~ADC_DIFSEL_DIFSEL_5; // Single-ended for PA0: ADC12_IN5 
	ADC1->SMPR1  &= ~ADC_SMPR1_SMP5;      // ADC Sample Time
	ADC1->SMPR1  |= SampleTime << 15;     // 3: 24.5 ADC clock cycles @80MHz = 0.3 us
	
	//////////////////////////////////////////////////////////////////////////////////////////////
	// ADC 2 regular sequence
	// ADC2: PA1: ADC12_IN6
	ADC2->SQR1 &= ~ADC_SQR1_L;   					// ADC regular channel sequence lenght, 0000: 1 conversion 
	ADC2->SQR1 &= ~ADC_SQR1_SQ1;
	ADC2->SQR1 |=  ( 6U << 6 );           // PA1: ADC12_IN6
	ADC2->DIFSEL &= ~ADC_DIFSEL_DIFSEL_6; // Single-ended for PA1: ADC12_IN6
	ADC2->SMPR1  &= ~ADC_SMPR1_SMP6;      // ADC Sample Time
	ADC2->SMPR1  |= SampleTime << 18;     // 3: 24.5 ADC clock cycles @80MHz = 0.3 us
}

//-------------------------------------------------------------------------------------------------------------------
// 	ADC Configuration
//  ADC Dual Mode: bits CONT, AUTDLY, DISCEN, DISCNUM[2:0], JDISCEN, JQM, JAUTO of the 
//  ADCx_CFGR register of the slave ADC2 are determined by those bits of the master ADC1
//-------------------------------------------------------------------------------------------------------------------
void ADC_Configure(ADC_TypeDef * ADCx){
	
	ADCx->CFGR &= ~ADC_CFGR_RES;     // Resolution, (00 = 12-bit, 01 = 10-bit, 10 = 8-bit, 11 = 6-bit)
	ADCx->CFGR &= ~ADC_CFGR_ALIGN;   // Data Alignment (0 = Right alignment, 1 = Left alignment)
	ADCx->CFGR &= ~ADC_CFGR_OVRMOD;  // Overrun (0 = old data, 1 = last conversion result)
	
	if (ADCx == ADC1 ) { // Only need to configure the Master ADC
		// Discontinuous mode for regular channels
		// It is forbidden to set both DISCEN=1 and CONT=1.
		// When dual mode is enabled (DUAL bits in ADCx_CCR register are not equal to zero), the bit
		// DISCEN of the slave ADC is no more writable and its content is equal to the bit DISCEN of the
		// master ADC.
		ADCx->CFGR &= ~ADC_CFGR_DISCEN; // 0 = disabled, 1 = enabled
		
		// When dual mode is enabled (DUAL bits in ADCx_CCR register are not equal to zero), the bits
		// DISCNUM[2:0] of the slave ADC are no more writable and their content is equal to the bits
		// DISCNUM[2:0] of the master ADC.
		ADCx->CFGR &= ~ADC_CFGR_DISCNUM;
		
		// Delayed conversion mode
		ADCx->CFGR |= ADC_CFGR_DISCEN;  // Auto-delayed conversion mode on
		
		// External trigger for regular channels
		ADCx->CFGR &= ~ADC_CFGR_EXTSEL;   
		ADCx->CFGR |=  ADC_CFGR_EXTSEL_3 | ADC_CFGR_EXTSEL_2;	// Select 1100 TIM4_TRGO event  
		//  00: Hardware trigger detection disabled (conversions can be launched by software)
		//  01: Hardware trigger detection on the rising edge
		//  10: Hardware trigger detection on the falling edge
		//  11: Hardware trigger detection on both the rising and falling edges	
		ADCx->CFGR &=~ ADC_CFGR_EXTEN;
		ADCx->CFGR |=  ADC_CFGR_EXTEN_0;  // Choose rising edge because DAC works on rising edge
		
		// ADC continuous mode	
		ADCx->CFGR &= ~ADC_CFGR_CONT;       // ADC Single/continuous conversion mode for regular conversion	
	}
	
	// 0: ADCx_DR register is preserved with the old data when an overrun is detected.
	ADCx->CFGR &= ~ADC_CFGR_OVRMOD;
	
	//////////////////////////////////////////////////////////////////////////////////////////////																 
	// ADC DMA Configuration 
	// There are two DMACFG bits. One in ADCx_CFGR for single ADC mode and the other in ADCx_CCR
	// for dual ADC mode.	
	// In dual-ADC modes, DMAEN and DMACFG bits are not relevant and replaced by control bits MDMA[1:0] of the ADCx_CCR register
	// ADCx->CFGR |= ADC_CFGR_DMACFG;      // 0 = DMA One Shot Mode, 1 = DMA Circular Mode	
	// ADCx->CFGR |= ADC_CFGR_DMAEN;       // 0 = DMA disabled,      1 = DMA enabled				
	
	//////////////////////////////////////////////////////////////////////////////////////////////
	// Configuration of Oversampler
	// Under certain conditions, the oversampled output can have a 16-bit result.
	ADCx->CFGR2 |= ADC_CFGR2_ROVSE;     // ADC Regular group oversampler enable (0 = disabled, 1 = enabled)
	// TROVS: Triggered Regular Oversampling
	// 0: All oversampled conversions for a channel are done consecutively following a trigger
	// 1: Each oversampled conversion for a channel needs a new trigger
	ADCx->CFGR2 &= ~ADC_CFGR2_TROVS;
	// 2x oversampling
	ADCx->CFGR2 &= ~ADC_CFGR2_OVSR;     // ADC Regular Oversampling ratio (000: 2x, 001: 4x, 010: 8x, 011: 16x, 100: 32x, 101: 64x, 110: 128x, 111: 256x)
	// ADC Regular Oversampling shift 
	ADCx->CFGR2 &= ~ADC_CFGR2_OVSS;     
	ADCx->CFGR2 |=  ADC_CFGR2_OVSS_0;   // Shift 1 bit
																	   // 0000: No shift,     0001: Shift 1-bit,  0010: Shift 2-bits, 0011: Shift 3-bits
																	   // 0100: Shift 4-bits, 0101: Shift 5-bits, 0110: Shift 6-bits, 0111: Shift 7-bits, 1000: Shift 8-bits
	
	//////////////////////////////////////////////////////////////////////////////////////////////																 
	// ADC Interrupt Enable
	// ADCx->IER |= ADC_IER_EOS;           // Enable ADC End of Regular Sequence of Conversions Interrupt
		
	//////////////////////////////////////////////////////////////////////////////////////////////
	// Enable ADC
	ADCx->CR |= ADC_CR_ADEN;         // ADC enable control
	/* DMH Edit 1/8/2025... (Compiler warning... always false since ADC_ISR_ADRDY is non-zero)
	 * // Wait until ADRDY is set by hardware
	 * while((ADCx->ISR | ADC_ISR_ADRDY) == 0);  // ADRDY is set after the ADC startup time
	 */
	// Wait until ADRDY is reset by hardware
	while((ADCx->ISR & ADC_ISR_ADRDY) == ADC_ISR_ADRDY);
}

//-------------------------------------------------------------------------------------------------------------------
// 	ADC Pin Initialization
//-------------------------------------------------------------------------------------------------------------------
void ADC_Pin_Init(void){	
	// Enable the clock of GPIO Port A
	RCC->AHB2ENR |=   RCC_AHB2ENR_GPIOAEN;
	
	// GPIO Pin Initialization
	// GPIO Speed: Low speed (00), Medium speed (01), Fast speed (10), High speed (11)
	// GPIO Output Type: Output push-pull (0, reset), Output open drain (1)
	
	// GPIO Mode: Input(00, reset), Output(01), AlterFunc(10), Analog(11, reset)
	// Configure  PA0: ADC12_IN5, PA1: ADC12_IN6, as Analog
	GPIOA->MODER |=  3U<<(2*0) | 3U<<(2*1);  // Mode 11 = Analog
	
	// GPIO Push-Pull: No pull-up, pull-down (00), Pull-up (01), Pull-down (10), Reserved (11)
	GPIOA->PUPDR &= ~( 3U<<(2*0) | 3U<<(2*1)); // No pull-up, no pull-down
	
	// GPIO port analog switch control register (ASCR)
	// 0: Disconnect analog switch to the ADC input (reset state)
	// 1: Connect analog switch to the ADC input
	GPIOA->ASCR |= GPIO_ASCR_EN_1 | GPIO_ASCR_EN_0;
}

//-------------------------------------------------------------------------------------------------------------------
// 	DMA Channel 1 Interrupt Handler
//-------------------------------------------------------------------------------------------------------------------
void DMA1_Channel1_IRQHandler(void){
	
	NVIC_ClearPendingIRQ(DMA1_Channel1_IRQn);	
	
	if ( (DMA1->ISR & DMA_ISR_TCIF1) == DMA_ISR_TCIF1 ) {
		
		DMA1->IFCR |= DMA_IFCR_CTCIF1; // Write 1 to clear the corresponding TCIF flag in DMA_ISR 
		// Both EOC flags are cleared when the DMA reads the ADCx_CDR register
		if (pADC_Working_Buffer == ADC_Buffer_Ping){
			pADC_Working_Buffer = ADC_Buffer_Pong;
			pADC_Ready_to_Read_Buffer = ADC_Buffer_Ping;
		}else{
			pADC_Working_Buffer = ADC_Buffer_Ping;
			pADC_Ready_to_Read_Buffer = ADC_Buffer_Pong;
		}
		
		DMA1_Channel1->CMAR  = (uint32_t) pADC_Working_Buffer;  // Memory address register
		
		ADC_DMA_Done = 1;
	}
	// Clear interrupt flags: half transfer, global interrupt, transfer error
	DMA1->IFCR |= (DMA_IFCR_CHTIF1 | DMA_IFCR_CGIF1 | DMA_IFCR_CTEIF1);
	
//	if ( (DMA1->ISR & DMA_ISR_HTIF1) == DMA_ISR_HTIF1 ) // half transfer
//		DMA1->IFCR |= DMA_IFCR_CHTIF1; 
//	
//	if ( (DMA1->ISR & DMA_ISR_GIF1) == DMA_ISR_GIF1 ) // global interrupt
//		DMA1->IFCR |= DMA_IFCR_CGIF1; 
//	
//	if ( (DMA1->ISR & DMA_ISR_TEIF1) == DMA_ISR_TEIF1 ) // transfer error
//		DMA1->IFCR |= DMA_IFCR_CTEIF1; 
	
}

//-------------------------------------------------------------------------------------------------------------------
// 	ADC 1/2 Interrupt Handler
//-------------------------------------------------------------------------------------------------------------------
void ADC1_2_IRQHandler(void){
	NVIC_ClearPendingIRQ(ADC1_2_IRQn);
	
	// ADC End of Conversion (EOC)
	if ((ADC1->ISR & ADC_ISR_EOC) == ADC_ISR_EOC) {
		// It is cleared by software writing 1 to it or by reading the corresponding ADCx_JDRy register
		ADC1->ISR |= ADC_ISR_EOC;
	}
	
	// ADC End of Injected Sequence of Conversions  (JEOS)
	if ((ADC1->ISR & ADC_ISR_EOS) == ADC_ISR_EOS) {
		// It is cleared by software writing 1 to it.
		ADC1->ISR |= ADC_ISR_EOS;		
	}
}





