.section .data
	VGAaddress: .word 0xC8000000

.section .text

.global _start
_start:

ClearScreen:
    PUSH {LR}
    PUSH {R4, R5}
	LDR R0, =VGAaddress
	LDR R1, [R0]
	MOV R2, #0x00000000
	MOV R3, #307200
ClearLoop:
	STR R2, [R1], #4
	SUBS R3, R3, #1
	BNE ClearLoop
    POP {R4,R5}
    POP {LR}
    BX LR

DrawBlock:
	mov r0, #100 // x
	mov r1, #40 // y
	mov r2, #0xFFFFFFFF // color
	mov r3, #120 // width
	mov r4, #140 // height
	add r5, r0, r3 // final x
	add r6, r1, r4 // final y
DrawLoop:
	push {r0, r1, r2, r3}
	bl SetPixel
	pop {r0, r1, r2, r3}
	add r0, r0, #1
	cmp r0, r5
	bne DrawLoop // loop if x value is final
	subs r0, r0, r3 // reset x
	add r1, r1, #1
	cmp r1, r6
	bne DrawLoop // don't loop if y value is final
	//bx lr 
	b .

SetPixel:
	//mov r0, #20
	//mov r1, #30
	//mov r2, #0xFFFFFFFF
	ldr r3, =VGAaddress
	ldr r3, [r3]
	lsl r1, r1, #10
	lsl r0, r0, #1
	add r1, r0
	strh r2, [r3, r1]
	bx lr

