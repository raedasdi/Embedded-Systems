// Lab9.c
// Runs on LM4F120 or TM4C123
// Student names: change this to your names or look very silly
// Last modification date: change this to the last modification date or look very silly
// Last Modified: 11/14/2018

// Analog Input connected to PE2=ADC1
// displays on Sitronox ST7735
// PF3, PF2, PF1 are heartbeats
// This U0Rx PC4 (in) is connected to other LaunchPad PC5 (out)
// This U0Tx PC5 (out) is connected to other LaunchPad PC4 (in)
// This ground is connected to other LaunchPad ground
// * Start with where you left off in Lab8. 
// * Get Lab8 code working in this project.
// * Understand what parts of your main have to move into the UART1_Handler ISR
// * Rewrite the SysTickHandler
// * Implement the s/w Fifo on the receiver end 
//    (we suggest implementing and testing this first)

#include <stdint.h>

#include "ST7735.h"
#include "TExaS.h"
#include "PLL.h"
#include "ADC.h"
#include "print.h"
#include "../inc/tm4c123gh6pm.h"
#include "Uart.h"
#include "FiFo.h"

//*****the first three main programs are for debugging *****
// main1 tests just the ADC and slide pot, use debugger to see data
// main2 adds the LCD to the ADC and slide pot, ADC data is on Nokia
// main3 adds your convert function, position data is no Nokia

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))
uint32_t Data;      // 12-bit ADC
uint32_t Position;  // 32-bit fixed-point 0.001 cm
char arr[4] = 0;		//array holds ASCII digit of slide measurement
uint8_t k;					//clock delay
uint32_t TxCounter = 0;

// Initialize SysTick with busy wait running at bus clock.
void SysTick_Init(void){
	NVIC_ST_CTRL_R = 0;					//disable SysTick during setup
	NVIC_ST_RELOAD_R = 0xA2038;	//creates a periodic interrupt of 60 Hz
	NVIC_ST_CURRENT_R = 0;			//any write to current clears it
	NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R & 0x00FFFFFF) | 0x40000000; //priority 2
	NVIC_ST_CTRL_R = 0x07;			//enables SysTick core clock
}

// Initialize Port F so PF1, PF2 and PF3 are heartbeats
void PortF_Init(void){
	SYSCTL_RCGCGPIO_R |= 0x20;				//turn on clock for Port F
	uint8_t k = 0;										//wait for clock to start
	k = 42;
	GPIO_PORTF_DIR_R |= 0x0E;					//PF1,2,3 are outputs
	GPIO_PORTF_DEN_R |= 0x0E;					//enable digital I/O for PF1,2,3
}




//uint32_t Status[20];             // entries 0,7,12,19 should be false, others true
//char GetData[10];  // entries 1 2 3 4 5 6 7 8 should be 1 2 3 4 5 6 7 8
/*int main(void){ // Make this main to test FiFo
  Fifo_Init();   // Assuming a buffer of size 6
  for(;;){
    Status[0]  = Fifo_Get(&GetData[0]);  // should fail,    empty
    Status[1]  = Fifo_Put(1);            // should succeed, 1 
    Status[2]  = Fifo_Put(2);            // should succeed, 1 2
    Status[3]  = Fifo_Put(3);            // should succeed, 1 2 3
    Status[4]  = Fifo_Put(4);            // should succeed, 1 2 3 4
    Status[5]  = Fifo_Put(5);            // should succeed, 1 2 3 4 5
    Status[6]  = Fifo_Put(6);            // should succeed, 1 2 3 4 5 6
    Status[7]  = Fifo_Put(7);            // should fail,    1 2 3 4 5 6 
    Status[8]  = Fifo_Get(&GetData[1]);  // should succeed, 2 3 4 5 6
    Status[9]  = Fifo_Get(&GetData[2]);  // should succeed, 3 4 5 6
    Status[10] = Fifo_Put(7);            // should succeed, 3 4 5 6 7
    Status[11] = Fifo_Put(8);            // should succeed, 3 4 5 6 7 8
    Status[12] = Fifo_Put(9);            // should fail,    3 4 5 6 7 8 
    Status[13] = Fifo_Get(&GetData[3]);  // should succeed, 4 5 6 7 8
    Status[14] = Fifo_Get(&GetData[4]);  // should succeed, 5 6 7 8
    Status[15] = Fifo_Get(&GetData[5]);  // should succeed, 6 7 8
    Status[16] = Fifo_Get(&GetData[6]);  // should succeed, 7 8
    Status[17] = Fifo_Get(&GetData[7]);  // should succeed, 8
    Status[18] = Fifo_Get(&GetData[8]);  // should succeed, empty
    Status[19] = Fifo_Get(&GetData[9]);  // should fail,    empty
  }
}*/

// Get fit from excel and code the convert routine with the constants
// from the curve-fit
uint32_t Convert(uint32_t input){
  input = input/2 + 113;
	if (input <= 125) return 0;
	if (input >= 2000) return 2000;
	return input;
	
}

// final main program for bidirectional communication
// Sender sends using SysTick Interrupt
// Receiver receives using RX
char data;
int main(void){ 
  
  PLL_Init(Bus80MHz);     // Bus clock is 80 MHz 
  ST7735_InitR(INITR_REDTAB);
  ADC_Init();    // initialize to sample ADC
  PortF_Init();
  Uart_Init();       // initialize UART
  ST7735_SetCursor(0,0);
  LCD_OutFix(0);
  ST7735_OutString(" cm");
	SysTick_Init();
  EnableInterrupts();
	while(1){
	while(Fifo_Get(&data)==0){PF1 = 0x00;
		PF2 = 0x00;}	//wait for character to be put into data
		
		ST7735_SetCursor(0,0);
		Fifo_Get(&data);							
		ST7735_OutChar(data);			//thousands digit to LCD  
		Fifo_Get(&data);
		ST7735_OutChar(data);			//'.' to LCD
		Fifo_Get(&data);
		ST7735_OutChar(data);			//hundreds digit to LCD
		Fifo_Get(&data);	
		ST7735_OutChar(data);			//tens digit to LCD
		Fifo_Get(&data);
		ST7735_OutChar(data);			//ones digit to LCD
		ST7735_OutString("  cm");
		Fifo_Get(&data);
		Fifo_Init();							//reset head and tail
}
    
}
/* SysTick ISR
*/
void SysTick_Handler(void){
	PF1 ^= 0x02;										//heartbeat
	Data = ADC_In();								//reads from the ADC
	PF1	^= 0x02;										//heartbeat
	Position = Convert(Data);
	arr[0] = Position/1000 +48;							//places thousands place digit into arr
	arr[1] = (Position % 1000) / 100+48;		//places hundres place digit into arr
	arr[2] = (Position % 1000 % 100) / 10+48; //places tens place digit into arr
  arr[3] = (Position % 1000 % 100 % 10)+48; //places ones digit into arr
	Uart_OutChar(0x02);					//Start Tx 
	Uart_OutChar(arr[0]);				//first digit put into UART1_DR_R
	Uart_OutChar(0x2E);					//period put into UART1_DR_R
	Uart_OutChar(arr[1]);				//hundreds digit put into UART1_DR_R
	Uart_OutChar(arr[2]);				//tens digit put into UART1_DR_R
	Uart_OutChar(arr[3]);				//ones digit put into UART1_DR_R
	Uart_OutChar(0x0D);					//carriage return
	Uart_OutChar(0x03);					//End Tx 
	PF1 ^= 0x02;								//heartbeat
	TxCounter++;
	
}

