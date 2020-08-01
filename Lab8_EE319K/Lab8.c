// Lab8.c
// Runs on LM4F120 or TM4C123
// Student names: change this to your names or look very silly
// Last modification date: change this to the last modification date or look very silly
// Last Modified: 11/6/2018 

// Specifications:
// Measure distance using slide pot, sample at 60 Hz
// maximum distance can be any value from 1.5 to 2cm
// minimum distance is 0 cm
// Calculate distance in fixed point, 0.001cm
// Analog Input connected to PD2=ADC5
// displays distance on Sitronox ST7735
// PF3, PF2, PF1 are heartbeats (use them in creative ways)
// 

#include <stdint.h>

#include "ST7735.h"
#include "TExaS.h"
#include "ADC.h"
#include "print.h"
#include "../inc/tm4c123gh6pm.h"

//*****the first three main programs are for debugging *****
// main1 tests just the ADC and slide pot, use debugger to see data
// main2 adds the LCD to the ADC and slide pot, ADC data is on ST7735
// main3 adds your convert function, position data is no ST7735

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))
	uint32_t ADCMail;
	uint8_t ADCMail_Flag = 0;
// Initialize Port F so PF1, PF2 and PF3 are heartbeats
void PortF_Init(void){
	SYSCTL_RCGCGPIO_R |= 0x20;				//turn on clock for Port F
	uint8_t k = 0;										//wait for clock to start
	k = 42;
	GPIO_PORTF_DIR_R |= 0x0E;					//PF1,2,3 are outputs
	GPIO_PORTF_DEN_R |= 0x0E;					//enable digital I/O for PF1,2,3
}

// Initialize SysTick with busy wait running at bus clock.
void SysTick_Init(void){
	NVIC_ST_CTRL_R = 0;
	NVIC_ST_RELOAD_R = 0xA2038;	//creates a periodic interrupt of 60 Hz
	NVIC_ST_CURRENT_R = 0;
	NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R & 0x00FFFFFF) | 0x40000000; //priority 2
	NVIC_ST_CTRL_R = 0x07;
}

void SysTick_Handler(void){
	PF1 ^= 0x02;					//toggle heatbeats
	PF2 ^= 0x04;
	ADCMail = ADC_In();		//put ADC data into the mailbox
	ADCMail_Flag = 1;			//set flag to signify mailbox is full
	//PF3 ^= 0x08;
}
uint32_t Data;        // 12-bit ADC
uint32_t Position;    // 32-bit fixed-point 0.001 cm
/*int main(void){       // single step this program and look at Data
  TExaS_Init();       // Bus clock is 80 MHz 
  ADC_Init();         // turn on ADC, set channel to 5
  while(1){                
    Data = ADC_In();  // sample 12-bit channel 5
  }
}*/

/*int main(void){
	uint32_t count = 0;
  TExaS_Init();       // Bus clock is 80 MHz 
  ADC_Init();         // turn on ADC, set channel to 5
  ST7735_InitR(INITR_REDTAB); 
  PortF_Init();
  while(1){           // use scope to measure execution time for ADC_In and LCD_OutDec           
    if (count >= 100) {
		count = 0;
		PF2 ^= 0x04;       // Profile ADC
		}
    Data = ADC_In();  // sample 12-bit channel 5
    //PF2 = 0x00;       // end of ADC Profile
    ST7735_SetCursor(0,0);
    PF1 = 0x02;       // Profile LCD
    LCD_OutDec(Data); 
    ST7735_OutString("    ");  // spaces cover up characters from last output
    PF1 = 0;          // end of LCD Profile
		count++;
  }
}*/

// your function to convert ADC sample to distance (0.001cm)
uint32_t Convert(uint32_t input){
	//input = ((input*5.479452055)-20438.35616);
	if (input == 4095) return 2000;						//for the dead zone at the top
	input = ((input*4.471787401)-16554.70338);//linear regression calculated
	if (input <= 230) return 0;								//for the dead zone at the bottom
  return input;
}
/*int main3(void){ 
  TExaS_Init();         // Bus clock is 80 MHz 
  ST7735_InitR(INITR_REDTAB); 
  PortF_Init();
  ADC_Init();         // turn on ADC, set channel to 5
  while(1){  
    PF2 ^= 0x04;      // Heartbeat
    Data = ADC_In();  // sample 12-bit channel 5
    PF3 = 0x08;       // Profile Convert
    Position = Convert(Data); 
    PF3 = 0;          // end of Convert Profile
    PF1 = 0x02;       // Profile LCD
    ST7735_SetCursor(0,0);
    LCD_OutDec(Data); ST7735_OutString("    "); 
    ST7735_SetCursor(6,0);
    LCD_OutFix(Position);
    PF1 = 0;          // end of LCD Profile
  }
}   */
int main(void){
  TExaS_Init();				//Bus clock is 80 MHz
	ST7735_InitR(INITR_REDTAB); 
	PortF_Init();
	ADC_Init();					//turn on ADC, set to channel 5
	SysTick_Init();			//initialize SysTick interrupts
  EnableInterrupts();
  while(1){
		PF2 ^= 0x04; 			//heartbeat
		//PF3 = 0x08; 
		
		if (ADCMail_Flag == 1){					//once flag is set output to LCD		
			ST7735_SetCursor(0,0);
			PF1 = 0x02;										//turn on LED
			LCD_OutFix(Convert(ADCMail));	//sample the channel, then convert it to centimeters
			//PF3 = 0; 										
			PF1 = 0x02;										//heartebeat 
			ST7735_OutString("    "); 
			ADCMail_Flag = 0;							//reset flag after data's written to LCD
			PF1 = 0;											//turn off LED
		}
		else {
			for(uint32_t k = 0; k < 1000000; k++){
			PF1 = 0;											//heartbeat off
			PF2 = 0;
			//PF3 = 0;
		}
	}
  }
}

