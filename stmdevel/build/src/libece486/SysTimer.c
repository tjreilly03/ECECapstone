
#include "SysTimer.h"
#include "init486.h"

static volatile uint32_t msTicks;
static volatile uint32_t Enable_Heartbeat=1;
static volatile uint32_t Heartbeat_Count = 500;


//-------------------------------------------------------------------------------------------------------------------
// Initialize SysTick	
//-------------------------------------------------------------------------------------------------------------------	
void SysTick_Init(void){
	// The RCC feeds the Cortex System Timer (SysTick) external clock with the AHB clock
	// (HCLK) divided by 8. The SysTick can work either with this clock or with the Cortex clock
	// (HCLK), configurable in the SysTick Control and Status Register.
	
	//  SysTick Control and Status Register
	SysTick->CTRL = 0;										// Disable SysTick IRQ and SysTick Counter
	
	// SysTick Reload Value Register
	SysTick->LOAD = 80000000/1000 - 1;    // 1ms, Default clock
	
	// SysTick Current Value Register
	SysTick->VAL = 0;

	NVIC_SetPriority(SysTick_IRQn, 1);		// Set Priority to 1
	NVIC_EnableIRQ(SysTick_IRQn);					// Enable EXTI0_1 interrupt in NVIC

	// Enables SysTick exception request
	// 1 = counting down to zero asserts the SysTick exception request
	// 0 = counting down to zero does not assert the SysTick exception request
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
	
	// Select processor clock
	// 1 = processor clock;  0 = external clock
	SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;		
	
	// Enable SysTick IRQ and SysTick Timer
	SysTick->CTRL |=  SysTick_CTRL_ENABLE_Msk;  
}

//-------------------------------------------------------------------------------------------------------------------
// SysTick Interrupt Handler
//-------------------------------------------------------------------------------------------------------------------
void SysTick_Handler(void){
	msTicks++;
	if (msTicks % Heartbeat_Count == 0) {
		if (Enable_Heartbeat==1) {
			PA5_TOGGLE();			// Flash LED if clock is running
		}
	}
		
}
	
//-------------------------------------------------------------------------------------------------------------------
// Delay in ms
//-------------------------------------------------------------------------------------------------------------------
void delay (uint32_t T){
  uint32_t curTicks;

  curTicks = msTicks;
  while ((msTicks - curTicks) < T);
	
	msTicks = 0;
}

//-------------------------------------------------------------------------------------------------------------------
// Disable the SysTick "heartbeat" if LED2 is used for other purposes.
//-------------------------------------------------------------------------------------------------------------------
void disable_Heartbeat (void){
  Enable_Heartbeat = 0;
}

//-------------------------------------------------------------------------------------------------------------------
// Set the "heartbeat" period
//   Heartbeat_Count is the SysTick count between togles of the heartbeat
//-------------------------------------------------------------------------------------------------------------------
void Set_Heartbeat_Count (uint32_t count){
  Heartbeat_Count = count;
}
