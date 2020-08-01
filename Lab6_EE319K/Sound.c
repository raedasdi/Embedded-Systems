// Sound.c
// This module contains the SysTick ISR that plays sound
// Runs on LM4F120 or TM4C123
// Program written by: put your names here
// Date Created: 3/6/17 
// Last Modified: 3/5/18 
// Lab number: 6
// Hardware connections
// TO STUDENTS "REMOVE THIS LINE AND SPECIFY YOUR HARDWARE********

// Code files contain the actual implemenation for public functions
// this file also contains an private functions and private data
#include <stdint.h>
#include "dac.h"
#include "../inc/tm4c123gh6pm.h"
#include "Piano.h"
uint8_t i = 0;  									//index of sin array
const unsigned short wave[32]	= {8,9,11,12,13,14,14,15,15,15,14,14,13,
	12,11,9,8,7,5,4,3,2,2,1,1,1,2,2,3,4,5,7};

	// **************Sound_Play*********************
// Start sound output, and set Systick interrupt period 
// Sound continues until Sound_Play called again
// This function returns right away and sound is produced using a periodic interrupt
// Input: interrupt period
//           Units of period to be determined by YOU
//           Maximum period to be determined by YOU
//           Minimum period to be determined by YOU
//         if period equals zero, disable sound output
// Output: none
void Sound_Play(uint32_t period){
	NVIC_ST_RELOAD_R = period;
}


void SysTick_Init(void){
	//SYSCTL_RCGCGPIO_R |= 0x20;			//initialize portF
	NVIC_ST_CTRL_R = 0;							//disable SysTick during setup
	NVIC_ST_RELOAD_R = 0;		//reload value
	//GPIO_PORTF_DIR_R |= 0x04;       //PF2 is output (built-in LED)
	//GPIO_PORTF_AFSEL_R &= ~(0x04);  //disable alt function of PF2
	//GPIO_PORTF_DEN_R |= 0x04;       //enable digital I/O on PF2
	NVIC_ST_CURRENT_R = 0;					//any write to current clears it
	NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R & 0x00FFFFFF) | 0x40000000; //priority 2
	NVIC_ST_CTRL_R = 0x07;					//enable SysTick core clock 
	//EnableInterrupts();	
}

void SysTick_Handler(void){
	//GPIO_PORTF_DATA_R ^= 0x04;			//toggle PF2
	
	DAC_Out(wave[i]);									//write sin wave to DAC
	i = (i+1) % 32; 									//for i to cycle once it's greater than 32
	//GPIO_PORTF_DATA_R ^= 0x04;
	
}

// **************Sound_Init*********************
// Initialize digital outputs and SysTick timer
// Called once, with sound/interrupts initially off
// Input: none
// Output: none
void Sound_Init(void){
	DAC_Init();
	SysTick_Init();	
}


