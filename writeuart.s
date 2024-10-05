.data
	TEXT_STRING: .asciz "string"
	JTAG_UART_BASE: .word 0xFF201000
.text /* executable code follows */
.global _start
_start:
WriteUart:
	//MOV SP, #DDR_END - 3 // highest memory word address
	LDR R4, =TEXT_STRING
LOOP:
	LDRB R0, [R4]
	CMP R0, #0
	BEQ CONT // string is null-terminated
	BL PUT_JTAG // send the character in R0 to UART
	ADD R4, R4, #1
	B LOOP
/* read and echo characters */
CONT:
	BL GET_JTAG // read from the JTAG UART
	CMP R0, #0 // check if a character was read
	BEQ CONT
	BL PUT_JTAG
	B CONT

	#mov r0, #97
.global PUT_JTAG
PUT_JTAG:
	LDR R1, =JTAG_UART_BASE // JTAG UART base address
	LDR R2, [R1, #4] // read the JTAG UART control register
	LDR R3, =0xFFFF0000
	ANDS R2, R2, R3 // check for write space
	BEQ END_PUT // if no space, ignore the character
	STR R0, [R1] // send the character
END_PUT:
	BX LR

.global GET_JTAG
GET_JTAG:
	LDR R1, =JTAG_UART_BASE // JTAG UART base address
	LDR R0, [R1] // read the JTAG UART data register
	ANDS R2, R0, #0x8000 // check if there is new data
	BEQ RET_NULL // if no data, return 0
	AND R0, R0, #0x00FF // return the character
	B END_GET
RET_NULL:
	MOV R0, #0
END_GET:
	BX LR
