// SpaceInvaders.c
// Runs on LM4F120/TM4C123
// Jonathan Valvano and Daniel Valvano
// This is a starter project for the EE319K Lab 10

// Last Modified: 11/20/2018 
// http://www.spaceinvaders.de/
// sounds at http://www.classicgaming.cc/classics/spaceinvaders/sounds.php
// http://www.classicgaming.cc/classics/spaceinvaders/playguide.php
/* This example accompanies the books
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2018

   "Embedded Systems: Introduction to Arm Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2018

 Copyright 2018 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */
// ******* Possible Hardware I/O connections*******************
// Slide pot pin 1 connected to ground
// Slide pot pin 2 connected to PD2/AIN5
// Slide pot pin 3 connected to +3.3V 
// fire button connected to PE0
// special weapon fire button connected to PE1
// 8*R resistor DAC bit 0 on PB0 (least significant bit)
// 4*R resistor DAC bit 1 on PB1
// 2*R resistor DAC bit 2 on PB2
// 1*R resistor DAC bit 3 on PB3 (most significant bit)
// LED on PB4
// LED on PB5

// Backlight (pin 10) connected to +3.3 V
// MISO (pin 9) unconnected
// SCK (pin 8) connected to PA2 (SSI0Clk)
// MOSI (pin 7) connected to PA5 (SSI0Tx)
// TFT_CS (pin 6) connected to PA3 (SSI0Fss)
// CARD_CS (pin 5) unconnected
// Data/Command (pin 4) connected to PA6 (GPIO), high for data, low for command
// RESET (pin 3) connected to PA7 (GPIO)
// VCC (pin 2) connected to +3.3 V
// Gnd (pin 1) connected to ground

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "ST7735.h"
#include "Random.h"
#include "PLL.h"
#include "ADC.h"
#include "Images.h"
#include "Sound.h"
#include "Timer0.h"
#include "Timer1.h"
#define dead 2
#define dying 1
#define alive 0


void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void Delay100ms(uint32_t count); // time delay in 0.1 seconds
//uint8_t xPos = 0;
//uint8_t yPos = 9;
int16_t shipPos;
int16_t pastPos;
int8_t k;
int32_t laserypos=159,delay,laserxPos;
uint8_t deadCount = 0;
uint8_t lives = 3;

struct Alien {
		int8_t xPos;		//x-coordinate of alien
		int8_t yPos;		//y-coordinate of alien
		int8_t ab;			//mouth and arms open or closed
		int8_t state;	//0 for alive, 1 for dying, 2 for dead
};
	typedef struct Alien Alien_t;

Alien_t enemies[12] = {
	{0, 9, 0, alive},
	{20, 9, 1, alive},
	{40, 9, 0, alive},
	{60, 9, 1, alive},
	{80, 9, 0, alive},
	{100, 9, 1, alive},
	{0, 22, 0, alive},
	{20, 22, alive},
	{40, 22, 0, alive},
	{60, 22, 1, alive},
	{80, 22, 0, alive},
	{100, 22, 1, alive}};

	void AlienReset(void){
	ST7735_FillScreen(0x0000);            // set screen to black
  ST7735_SetCursor(1, 1);
	ST7735_OutString("Lives Remaining: ");
	ST7735_SetCursor(2, 1);
  LCD_OutDec(lives);
	delay = 50000;
			while (delay>0) {
				delay--;
			}
	ST7735_FillScreen(0x0000);		
	for (k = 0; k < 6; k++){
		enemies[k].yPos = 9;
		enemies[k].ab = k%2;
	}
	for (k = 6; k < 12; k++){
		enemies[k].yPos = 22;
		enemies[k].ab = k%2;
	}
	}

	void NextLevel(void){
	ST7735_FillScreen(0x0000);            // set screen to black
  ST7735_SetCursor(20, 20);
  ST7735_OutString("Next Level");
  ST7735_SetCursor(1, 2);
	for (k = 0; k < 6; k++){
		enemies[k].yPos = 9;
		enemies[k].state = alive;
		enemies[k].ab = k%2;
	}
	for (k = 6; k < 12; k++){
		enemies[k].yPos = 22;
		enemies[k].state = alive;
		enemies[k].ab = k%2;
	}
	
	deadCount = 0;
  Delay100ms(30);
	ST7735_FillScreen(0x0000); 
	NVIC_ST_RELOAD_R = NVIC_ST_RELOAD_R / 1.2;
}

uint8_t PlayerShoot(void) {
	/*shipPos = ADC_In();
	if (shipPos >= 3495) {shipPos = 3495;}
		shipPos = shipPos/32;
		ST7735_DrawBitmap(shipPos, 159, PlayerShip0, 18,8);
		pastPos = shipPos;
		ST7735_FillRect (shipPos + 9, 159, 2, 10, ST7735_Color565(0,255,0));*/
		
		laserxPos = ADC_In() / 32 + 9;
		laserypos=151;
		while (laserypos > 0) {
		shipPos = ADC_In();
		if (shipPos >= 3495) {shipPos = 3495;}
		pastPos = shipPos;
		shipPos = shipPos/32;
		ST7735_DrawBitmap(shipPos, 159, PlayerShip0, 18,8);	
			laserypos--;
			ST7735_FillRect (laserxPos, laserypos, 2, 5, ST7735_Color565(0,255,0));
			ST7735_FillRect (laserxPos, laserypos+5, 2, 5, ST7735_Color565(0,0,0));
			for (k=0; k<12; k++){
				if (enemies[k].state != dead){
				if (((enemies[k].xPos + 16) - (laserxPos)) <= 17  && ((enemies[k].xPos + 16) - (laserxPos)) >= -1 ){
					if ((enemies[k].yPos) - (laserypos) == 0){
						enemies[k].state = dead;
						ST7735_FillRect(enemies[k].xPos, enemies[k].yPos - 10,21,15,ST7735_Color565(0,0,0));
						//ST7735_FillRect (laserxPos, laserypos, 2, 5, ST7735_Color565(255,0,0));
						return 1;
					}
					}
				}
			}
			delay = 5000;
			while (delay>0) {
				delay--;
			}
			
		}
		ST7735_FillRect (0, 0, 128, 5, ST7735_Color565(0,0,0));
}

// Initialize SysTick with busy wait running at bus clock.
void SysTick_Init(void){
	NVIC_ST_CTRL_R = 0;					//disable SysTick during setup
	NVIC_ST_RELOAD_R = 0xFFFFFF;	//creates the length of the periodic interrupt
	NVIC_ST_CURRENT_R = 0;			//any write to current clears it
	NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R & 0x00FFFFFF) | 0x10000000; //priority level
	NVIC_ST_CTRL_R = 0x07;			//enables SysTick core clock
}
/* SysTick ISR
*/
void SysTick_Handler(void){
	for (k = 0; k < 12; k++){
		if (enemies[k].state != dead){
		if (enemies[k].ab == 0) {
			ST7735_DrawBitmap(enemies[k].xPos + 5, enemies[k].yPos, SmallEnemy20pointA, 16,10);
			enemies[k].ab = 1;
		}
		else {
		ST7735_DrawBitmap(enemies[k].xPos + 5, enemies[k].yPos, SmallEnemy20pointB, 16,10);
		enemies[k].ab = 0;
		}
		enemies[k].yPos++;
	}
		else{
			deadCount++;
			if (deadCount == 12)
				NextLevel();
		}
		
	}
	deadCount = 0;
	/*ST7735_FillRect(xPos-9,yPos-9,16,10,0); 
	ST7735_DrawBitmap(xPos, yPos, SmallEnemy20pointA, 16,10);
	xPos++;
	if(xPos == 128)
		yPos += 9;*/
}

void PortE_Init(void){
	SYSCTL_RCGCGPIO_R |= 0x10;				//turn on clock for Port E
	uint8_t k = 0;										//wait for clock to start
	k = 42;
	GPIO_PORTE_DIR_R &= ~0x03;					//PE0, PE1 are inputs
	GPIO_PORTE_DEN_R |= 0x03;					//enable digital I/O for PE0, PE1
}



int main(void){
  PLL_Init(Bus80MHz);       // Bus clock is 80 MHz 
	PortE_Init();
  Random_Init(1);
  Output_Init();
	ADC_Init();
	SysTick_Init();
	Timer0_Init(&PlayerShoot, 80000000/2);
  ST7735_FillScreen(0x0000);            // set screen to black
  
 // ST7735_DrawBitmap(52, 159, ns, 18,8); // player ship middle bottom
  /*ST7735_DrawBitmap(53, 151, Bunker0, 18,5);

  ST7735_DrawBitmap(0, 9, SmallEnemy10pointA, 16,10);
  ST7735_DrawBitmap(20,9, SmallEnemy10pointB, 16,10);
  ST7735_DrawBitmap(40, 9, SmallEnemy20pointA, 16,10);
  ST7735_DrawBitmap(60, 9, SmallEnemy20pointB, 16,10);
  ST7735_DrawBitmap(80, 9, SmallEnemy30pointA, 16,10);
  ST7735_DrawBitmap(100, 9, SmallEnemy30pointB, 16,10);*/
	while(1){
		//ST7735_FillRect (pastPos, 159, 18, 8, ST7735_Color565(0,0,0));
		//if (
		shipPos = ADC_In();
		if (shipPos >= 3495) {shipPos = 3495;}
		pastPos = shipPos;
		shipPos = shipPos/32;
		ST7735_DrawBitmap(shipPos, 159, PlayerShip0, 18,8);	
		}
			
  Delay100ms(50);              // delay 5 sec at 80 MHz

  ST7735_FillScreen(0x0000);            // set screen to black
  ST7735_SetCursor(1, 1);
  ST7735_OutString("GAME OVER");
  ST7735_SetCursor(1, 2);
  ST7735_OutString("Nice try,");
  ST7735_SetCursor(1, 3);
  ST7735_OutString("Earthling!");
  ST7735_SetCursor(2, 4);
  LCD_OutDec(1234);
	
	EnableInterrupts();
  

}


// You can't use this timer, it is here for starter code only 
// you must use interrupts to perform delays
void Delay100ms(uint32_t count){uint32_t volatile time;
  while(count>0){
    time = 727240;  // 0.1sec at 80 MHz
    while(time){
	  	time--;
    }
    count--;
  }
}
