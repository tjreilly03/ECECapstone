#include "OPAMP.h"
#include "SysTimer.h"

/*
  Table 1.  OPAMPs inverting/non-inverting inputs for the STM32L4 devices:
     
       (+) +------------------------------------------------------------------------|     
       (+) |                 |         | OPAMP1               | OPAMP2              |
       (+) |-----------------|---------|----------------------|---------------------|
       (+) | Inverting Input | VM_SEL  |                      |                     |
       (+) |                 |         |  IO0-> PA1           | IO0-> PA7           |
       (+) |                 |         |  LOW LEAKAGE IO (2)  | LOW LEAKAGE IO (2)  |
       (+) |                 |         |  Not connected       | Not connected       |
       (+) | (1)             |         |    PGA mode only     |   PGA mode only     |
       (+) |-----------------|---------|----------------------|---------------------|
       (+) |  Non Inverting  | VP_SEL  |                      |                     |
       (+) |                 |         |  IO0-> PA0 (GPIO)    | IO0-> PA6  (GPIO)   |
       (+) |    Input        |         |  DAC1_OUT1 internal  | DAC1_OUT2 internal  |
       (+) +------------------------------------------------------------------------|
       [..] (1): NA in follower mode.
       [..] (2): Available on some package only (ex. BGA132).
           
   Table 2.  OPAMPs outputs for the STM32L4 devices:
       (+) +-------------------------------------------------------------------------     
       (+) |                 |        | OPAMP1                | OPAMP2              |
       (+) |-----------------|--------|-----------------------|---------------------|
       (+) | Output          |  VOUT  |  PA3                  |  PB0                |
       (+) |                 |        |  & (2) ADC12_IN if    | & (2) ADC12_IN if   |
       (+) |                 |        |  connected internally | connected internally|
       (+) |-----------------|--------|-----------------------|---------------------|
    [..] (2): ADC1 or ADC2 shall select IN15
*/

// DMH: Set up OPAMP2 to act as a unity gain buffer with input from DAC Channel 2
// to send the output to PB0.

void OPAMP_Init(void){
	// OPAMP2_VOUT: Pin PB0
	// OPAMP1_VINP: Internal Input, DAC1_OUT2 (Controlled by VP_SEL)
	// OPAMP1_VINM: Internal Input, Unity gain buffer (controlled by bits OPAMODE and VM_SEL)
	
	RCC->APB1ENR1 |= RCC_APB1ENR1_OPAMPEN;  // Enable Operational Amplifier

	// Enable the clock of GPIO Port B
	RCC->AHB2ENR |=   RCC_AHB2ENR_GPIOBEN;
	// GPIO Mode: Input(00, reset), Output(01), AlterFunc(10), Analog(11, reset)
	GPIOB->MODER |=   3U<<(2*0);      // Configure PA3 as Analog
	GPIOB->PUPDR &= ~(3U<<(2*0));     // No pull-up, pull-down (00), Pull-up (01), Pull-down (10), Reserved (11)	
	//GPIOB->ASCR  &= ~GPIO_ASCR_EN_3;  // 0: Disconnect analog switch to the ADC input	
	
	OPAMP2_Calibration();
	
	OPAMP2->CSR  |=  OPAMP2_CSR_OPALPM; 
	OPAMP2->CSR  &= ~OPAMP2_CSR_OPALPM;  // 0: operational amplifier in normal mode
	
	// Operational amplifier PGA mode
	//OPAMP2->CSR  |= OPAMP1_CSR_OPAMODE; 
	// 10: internal PGA enable, gain programmed in PGA_GAIN
	// OPAMP1->CSR  |=  OPAMP1_CSR_OPAMODE_1; 
	OPAMP2->CSR |= (3U<<2);		// 11: Internal follower
	
	// Operational amplifier Programmable amplifier gain value
	// OPAMP1->CSR  &= ~OPAMP1_CSR_PGAGAIN; 

	// Non inverted input selection
	// 0: GPIO connected to VINP
  // 1: DAC connected to VINP	
	OPAMP2->CSR  |=  OPAMP2_CSR_VPSEL;   

	// Inverting input selection
	// 00: GPIO connected to VINM (valid also in PGA mode for filtering)
	// 01: Dedicated low leakage input, (available only on BGA132) connected to VINM (valid also in PGA mode for filtering)
	// 1x: Inverting input not externally connected. These configurations are valid only when OPAMODE = 10 (PGA mode)
	//OPAMP1->CSR  &= ~OPAMP1_CSR_VMSEL; 
	//OPAMP1->CSR  |=  OPAMP1_CSR_VMSEL_1; 
	
	OPAMP2->CSR  |= OPAMP2_CSR_OPAEN;    // OPAMP enable
}

void OPAMP2_Calibration(void){
  
  uint32_t trimmingvaluen = 0;
  uint32_t trimmingvaluep = 0;
  uint32_t delta;
	
	OPAMP2->CSR |= OPAMP2_CSR_OPALPM;   // High range, VDDA > 2.4V
	OPAMP2->CSR |= OPAMP2_CSR_OPAEN;    // OPAMP enable
	OPAMP2->CSR |= OPAMP2_CSR_USERTRIM; // Enable User Trim
	OPAMP2->CSR |= OPAMP2_CSR_CALON;    // Turn on calibration
		 
	/* 1st calibration - N */
  OPAMP2->CSR &= ~OPAMP2_CSR_CALSEL;
      
	/* Enable the selected opamp */
	OPAMP2->CSR |= OPAMP2_CSR_OPALPM;
      
	/* Init trimming counter */    
	/* Medium value */
	trimmingvaluen = 16; 
	delta = 8;
           
	while (delta != 0){
		/* Set candidate trimming */
		/* OPAMP_POWERMODE_NORMAL */
		OPAMP2->OTR &= ~OPAMP_OTR_TRIMOFFSETN;
		OPAMP2->OTR |= (OPAMP_OTR_TRIMOFFSETN & trimmingvaluen);
        
		/* OFFTRIMmax delay 1 ms as per datasheet (electrical characteristics */ 
		/* Offset trim time: during calibration, minimum time needed between */
		/* two steps to have 1 mV accuracy */
		delay(1);

		if ((OPAMP2->CSR & OPAMP_CSR_CALOUT) != 0) { 
			/* OPAMP_CSR_CALOUT is HIGH try higher trimming */
			trimmingvaluen -= delta;
		} else {
			/* OPAMP_CSR_CALOUT is LOW try lower trimming */
			trimmingvaluen += delta;
		}
    /* Divide range by 2 to continue dichotomy sweep */       
    delta >>= 1;
	}

	/* Still need to check if right calibration is current value or one step below */
  /* Indeed the first value that causes the OUTCAL bit to change from 0 to 1  */
	/* Set candidate trimming */
	OPAMP2->OTR &= ~OPAMP_OTR_TRIMOFFSETN;
	OPAMP2->OTR |= (OPAMP_OTR_TRIMOFFSETN & trimmingvaluen);
           
	/* OFFTRIMmax delay 1 ms as per datasheet (electrical characteristics */ 
	/* Offset trim time: during calibration, minimum time needed between */
	/* two steps to have 1 mV accuracy */
	delay(1);
      
	if ((OPAMP2->CSR & OPAMP_CSR_CALOUT) == 0){ 
		/* Trimming value is actually one value more */
		trimmingvaluen++;
		/* Set right trimming */
		OPAMP2->OTR &= ~OPAMP_OTR_TRIMOFFSETN;
		OPAMP2->OTR |= (OPAMP_OTR_TRIMOFFSETN & trimmingvaluen);
	}

	/* 2nd calibration - P */
	OPAMP2->CSR |= OPAMP_CSR_CALSEL;
      
	/* Init trimming counter */    
	/* Medium value */
	trimmingvaluep = 16; 
	delta = 8;
      
	while (delta != 0) {
		/* Set candidate trimming */
		/* OPAMP_POWERMODE_NORMAL */
		OPAMP2->OTR &= ~OPAMP_OTR_TRIMOFFSETP;
		OPAMP2->OTR |= (OPAMP_OTR_TRIMOFFSETP & (trimmingvaluep << 8));

		/* OFFTRIMmax delay 1 ms as per datasheet (electrical characteristics */ 
		/* Offset trim time: during calibration, minimum time needed between */
		/* two steps to have 1 mV accuracy */
    delay(1);

		if ( (OPAMP2->CSR & OPAMP_CSR_CALOUT) != 0){ 
			/* OPAMP_CSR_CALOUT is HIGH try higher trimming */
			trimmingvaluep += delta;
		} else {
			/* OPAMP_CSR_CALOUT  is LOW try lower trimming */
			trimmingvaluep -= delta;
		}
        
		/* Divide range by 2 to continue dichotomy sweep */
		delta >>= 1;
	}
      
	/* Still need to check if right calibration is current value or one step below */
	/* Indeed the first value that causes the OUTCAL bit to change from 1 to 0  */
	/* Set candidate trimming */
	OPAMP2->OTR &= ~OPAMP_OTR_TRIMOFFSETP;
	OPAMP2->OTR |= (OPAMP_OTR_TRIMOFFSETP & (trimmingvaluep << 8));
	
	/* OFFTRIMmax delay 1 ms as per datasheet (electrical characteristics */ 
	/* Offset trim time: during calibration, minimum time needed between */
	/* two steps to have 1 mV accuracy */
	delay(1);
      
	if ((OPAMP2->CSR & OPAMP_CSR_CALOUT) != 0){
		/* Trimming value is actually one value more */
		trimmingvaluep++;
		OPAMP2->OTR &= ~OPAMP_OTR_TRIMOFFSETP;
		OPAMP2->OTR |= (OPAMP_OTR_TRIMOFFSETP & (trimmingvaluep << 8));
	}
      
	/* Disable the OPAMP */
	OPAMP2->CSR &= ~OPAMP1_CSR_OPALPM;
      
	/* Disable calibration & set normal mode (operating mode) */
	OPAMP2->CSR &= ~OPAMP_CSR_CALON;	
}

