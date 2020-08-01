// IO.c
// This software configures the switch and LED
// You are allowed to use any switch and any LED, 
// although the Lab suggests the SW1 switch PF4 and Red LED PF1
// Runs on LM4F120 or TM4C123
// Program written by: put your names here
// Date Created: March 30, 2018
// Last Modified:  change this or look silly
// Lab number: 7

//#include "ST7735.c"
#include "../inc/tm4c123gh6pm.h"
#include <stdint.h>


//------------IO_Init------------
// Initialize GPIO Port for a switch and an LED
// Input: none
// Output: none
void IO_Init(void) {
	volatile int32_t delay;
	SYSCTL_RCGCGPIO_R |= 0x20; // --UUU-- Code to initialize PF4 and PF2
	delay = 0;
	delay = 0;
	delay = 0;
	GPIO_PORTF_DEN_R |= 0x14;
	GPIO_PORTF_DIR_R |= 0x04;
	GPIO_PORTF_DIR_R &= (~0x10);
	GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY;
	GPIO_PORTF_CR_R = 0xFF;
	GPIO_PORTF_PUR_R |= 0x10;
	}

//------------IO_HeartBeat------------
// Toggle the output state of the  LED.
// Input: none
// Output: none
void IO_HeartBeat(void) {
static uint32_t delay=1000000;
GPIO_PORTF_DATA_R ^= 0x04; // --UUU-- PF2 is heartbeat
while (delay>0){
	delay--;
	}
}


//------------IO_Touch------------
// wait for release and press of the switch
// Delay to debounce the switch
// Input: none
// Output: none
void IO_Touch(void) {
	uint8_t switch_st; // --UUU-- wait for release; delay for 20ms; and then wait for press
	uint32_t delay = 20;
	switch_st = GPIO_PORTF_DATA_R & 0x10;
	while (switch_st > 0) {
		switch_st = GPIO_PORTF_DATA_R & 0x10;
			}
	while (delay>0) {
	Delay1ms();
	delay--;
	}
	/*while (delay > 0) {
		delay--;
			}*/
	while (switch_st == 0) {
		switch_st = GPIO_PORTF_DATA_R & 0x10;
			}
}  

