// Fifo.c
// Runs on LM4F120/TM4C123
// Provide functions that implement the Software FiFo Buffer
// Last Modified: November 14, 2018
// Trevor, Raed
// Last modification date: 4/22/19

#include <stdint.h>
// --UUU-- Declare state variables for Fifo
//        buffer, put and get indexes
	int32_t tail; //index of oldest
	int32_t head; //index to put new
	int32_t Fif[8];

// *********** Fifo_Init**********
// Initializes a software FIFO of a
// fixed size and sets up indexes for
// put and get operations
void Fifo_Init(){
	tail = 0;
	head = 0;
}

// *********** Fifo_Put**********
// Adds an element to the FIFO
// Input: Character to be inserted
// Output: 1 for success and 0 for failure
//         failure is when the buffer is full
uint32_t Fifo_Put(char data){
	if((head+1)%8 == tail){return 0;}
	Fif[head] = data;
	head = (head+1)%8;
	return 1;
}

// *********** FiFo_Get**********
// Gets an element from the FIFO
// Input: Pointer to a character that will get the character read from the buffer
// Output: 1 for success and 0 for failure
//         failure is when the buffer is empty
uint32_t Fifo_Get(char *datapt){ 
	if(tail == head ){return 0;}
	*datapt = Fif[tail];
	tail = (tail+1)%8;
  return 1; 
}



