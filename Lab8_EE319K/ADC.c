// ADC.c
// Runs on LM4F120/TM4C123
// Provide functions that initialize ADC0
// Last Modified: 11/6/2018
// Student names: change this to your names or look very silly
// Last modification date: change this to the last modification date or look very silly

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"

// ADC initialization function 
// Input: none
// Output: none
// measures from PD2, analog channel 5
void ADC_Init(void){ 
	SYSCTL_RCGCGPIO_R |= 0x08;		//turn on clock for Port D
	uint8_t k;										//wait for clock to start
	k = 42;
	GPIO_PORTD_DIR_R &= ~0x04;		//make PD2 input
	GPIO_PORTD_AFSEL_R |= 0x04;   //enable alternate function on PD2
  GPIO_PORTD_DEN_R &= ~0x04;    //disable digital I/O on PD2
  GPIO_PORTD_AMSEL_R |= 0x04;   //enable analog functionality on PD2
  SYSCTL_RCGCADC_R |= 0x0001;   //activate ADC0 
	k = 42;												//waiting for the clock so stabilize
	k = 42;
	k = 42;
	k = 42;
	k = 42;
	k = 42;
	k = 42;
	k = 42;
	k = 42;
	k = 42;
	k = 42;
	ADC0_PC_R = 0x01;							//configure for 125K
	ADC0_SSPRI_R = 0x0123;				//Seq 3 is highest priority
	ADC0_ACTSS_R &= ~0x0008; 			//disable sample sequencer 3
	ADC0_EMUX_R &= ~0xF000;			//seq3 is software trigger
	ADC0_SSMUX3_R = (ADC0_SSMUX3_R&0xFFFFFFF0)+5;  //Ain5 (PD2)
	ADC0_SSCTL3_R = 0x0006;				//no TS0 D0, yes IE0 END0
	ADC0_IM_R &= ~0x0008;					//disable SS3 interrupts
	ADC0_ACTSS_R |= 0x0008;				//enable sample sequencer 3
	ADC0_SAC_R = 0x08;  

}

//------------ADC_In------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
// measures from PD2, analog channel 5
uint32_t ADC_In(void){  
	uint32_t data;									//will hold the 12 bit result
  ADC0_PSSI_R = 0x0008;       		//start ADC     
  while((ADC0_RIS_R&0x08)==0){}; 	//checking status  
  data = ADC0_SSFIFO3_R&0xFFF;  	//read data
  ADC0_ISC_R = 0x0008; 						//clear flag
  return data;
}


