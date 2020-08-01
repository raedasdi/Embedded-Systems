; Print.s
; Student Names: Ra'ed Asdi, Trevor Barrett
; Last modification date: 3/30/2019
; Runs on LM4F120 or TM4C123
; EE319K lab 7 device driver for any LCD
;
; As part of Lab 7, students need to implement these LCD_OutDec and LCD_OutFix
; This driver assumes two low-level LCD functions
; ST7735_OutChar   outputs a single 8-bit ASCII character
; ST7735_OutString outputs a null-terminated string 

			

		

    IMPORT   ST7735_OutChar
    IMPORT   ST7735_OutString
    EXPORT   LCD_OutDec
    EXPORT   LCD_OutFix

    AREA    |.text|, CODE, READONLY, ALIGN=2
    THUMB

divcount1 		EQU 0		;binding phase
num			RN 	4
onum		RN	5
divisor		RN	6
nnum		RN	7
next_num	RN	8
divcount	RN	9


;-----------------------LCD_OutDec-----------------------
; Output a 32-bit number in unsigned decimal format
; Input: R0 (call by value) 32-bit unsigned number
; Output: none
; Invariables: This function must not permanently modify registers R4 to R11
LCD_OutDec
	PUSH {R4,R5,R6,R7,R8,R9,R10,LR} 
	;SUB SP, #4		
	ADD R0, #0
	CMP R0, #0
	BEQ Print2
	MOV divisor, #1
	MOV num, R0
	MOV onum, R0
	MOV nnum, R0
	MOV next_num, R0
	MOV divcount, #0
	MOV R1, #10
	MOV R10, #0						;allocation phase		
	
divcheck
	
	;;ADD divcount, #0
	;;CMP divcount, #9
	;;LDR R3, [SP, #divcount1]
	;;BEQ check
	MOV R2, onum
	UDIV R2, divisor
	CMP R2, #0
	BEQ check
	LDR R10, [SP, #divcount1]	;access phase
	ADD R10, #1
	;STR R10, [SP, #divcount1]
	CMP R10, #10
	ADD divcount, #1
	CMP divcount, #10
	BEQ check
	MUL divisor, R1
	B divcheck
check

	MOV R0, next_num
	MOV R1, #10
	LDR R10, [SP, #divcount1]	;access phase
	ADD R10, #0
	CMP R10, #10
	ADD divcount, #0
	CMP divcount, #10
	BEQ	cont
	UDIV divisor, R1
cont
	MOV divcount, #0
	CMP divisor, #1
	BEQ Print2	
	UDIV num, next_num, divisor
	MOV nnum, num
	MUL num, divisor
	SUB R0, num
	CMP R0, #0
	BEQ Perfect_Division
	MOV next_num, R0
	B Print
		
Perfect_Division			;if the number is divisible by 10 branch here
	MOV next_num, R0
	MOV R0, nnum
	ADD R0, #0x30
	BL ST7735_OutChar
	B check

Print
	MOV R0, nnum
	ADD R0, #0x30
	BL ST7735_OutChar
	B check
Print2						;when the remainder becomes single digiits branch here
	ADD R0, #0x30
	BL ST7735_OutChar
Done 
	POP {R4,R5,R6,R7,R8,R9,R10,PC}
	  
;* * * * * * * * End of LCD_OutDec * * * * * * * *

; -----------------------LCD _OutFix----------------------
; Output characters to LCD display in fixed-point format
; unsigned decimal, resolution 0.001, range 0.000 to 9.999
; Inputs:  R0 is an unsigned 32-bit number
; Outputs: none
; E.g., R0=0,    then output "0.000 "
;       R0=3,    then output "0.003 "
;       R0=89,   then output "0.089 "
;       R0=123,  then output "0.123 "
;       R0=9999, then output "9.999 "
;       R0>9999, then output "*.*** "
; Invariables: This function must not permanently modify registers R4 to R11
LCD_OutFix

	PUSH {R4,R5,R6,R7,R8,R9,R10,LR} 
	;SUB SP, #4		;allocation phase
	MOV R3, #10000
	UDIV R3, R0, R3
	CMP R3, #0
	BHI Stars
	;ADD R0, #0
	;CMP R0, #0
	;BEQ Print2
	MOV divisor, #1
	MOV num, R0
	MOV onum, R0
	MOV nnum, R0
	MOV next_num, R0
	MOV divcount, #0
	MOV R1, #10
	B divcheck1
	
Stars					;when R0 is greater than 9999 branch here
	MOV R0, #42
	BL ST7735_OutChar
	MOV R0, #46
	BL ST7735_OutChar
	MOV R0, #42
	BL ST7735_OutChar
	MOV R0, #42
	BL ST7735_OutChar
	MOV R0, #42
	BL ST7735_OutChar
	B Fin
	
divcheck1
	;MOV R3, #9					
	;STR R3, [SP, #divcount1]		;divcheck = 0
	;LDR R3, [SP, #divcount1]
	MOV R2, onum
	UDIV R2, divisor
	CMP R2, #0
	BEQ Length
	MUL divisor, R1
	ADD divcount, #1
	CMP divcount, #4
	BEQ Four
	B divcheck1

Length							;this will branch to different spots based on the number of digits 
	CMP divcount, #3			;in R0
	BEQ Three
	CMP divcount, #2
	BEQ Two
	CMP divcount, #1
	BEQ One
	B Zero
	
Zero							;R0 is zero branch here
		MOV R0, #0x30
	BL ST7735_OutChar			;print 0
Zeros
	MOV R0, #46
	BL ST7735_OutChar			;print period
	MOV R0, #0x30
	BL ST7735_OutChar			;print 0
	MOV R0, #0x30
	BL ST7735_OutChar			;print 0
	MOV R0, #0x30
	BL ST7735_OutChar			;print 0
	B Fin
	
Three
	MOV R0, #0x30
	BL ST7735_OutChar			;print 0
	MOV R0, #46
	BL ST7735_OutChar			;print period
	MOV R0, onum
	BL LCD_OutDec			;print number
	B Fin
	
Two
	MOV R0, #0x30
	BL ST7735_OutChar			;print 0
	MOV R0, #46
	BL ST7735_OutChar			;print period
	MOV R0, #0x30
	BL ST7735_OutChar			;print 0
	MOV R0, onum
	BL LCD_OutDec			;print number
	B Fin
	
One
	MOV R0, #0x30
	BL ST7735_OutChar			;print 0
tens
	MOV R0, #46
	BL ST7735_OutChar			;print period

	MOV R0, #0x30
	BL ST7735_OutChar			;print 0
;huns
	MOV R0, #0x30
	BL ST7735_OutChar			;print 0
	MOV R0, num
	BL LCD_OutDec			;print number
	B Fin
	
huns
	MOV R0, #46
	BL ST7735_OutChar			;print period
	MOV R0, #0x30
	BL ST7735_OutChar			;print 0
	MOV R0, num
	BL LCD_OutDec			;print number
	B Fin

	
Four
	MOV R3, #10
	UDIV divisor, R3
	UDIV R0, divisor			;R0 holds thousands place
	MUL num, R0, divisor		
	SUB num, onum, num			;num is now remainder
	BL LCD_OutDec
	CMP num, #0					;if the remainder is zero, meaning it's 1000
	BEQ Zeros
	CMP num, #10				;if there's a zero in the tens place
	BLO tens
	CMP num, #100				;if there's a zero in the hundreds place
	BLO huns 
	MOV R0, #46
	BL ST7735_OutChar			;draw period
	MOV R0, num
	BL LCD_OutDec
	
	B Fin
	
Fin     POP {R4,R5,R6,R7,R8,R9,R10,PC}
 
     ALIGN
;* * * * * * * * End of LCD_OutFix * * * * * * * *

     ALIGN                           ; make sure the end of this section is aligned
     END                             ; end of file
