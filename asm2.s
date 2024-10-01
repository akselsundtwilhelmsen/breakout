.section .data
	VGAaddress: .word 0xC8000000  // Base address of the video memory

.section .text

.global _start
_start:

bl ClearScreen
mov r0, #160
bl DrawBar
b .

ClearScreen:
	push {lr, r0, r1, r2, r3, r4, r5, r6}
	mov r0, #0
	mov r1, #0
	mov r2, #0x00000000
	mov r3, #320
	mov r4, #240
	bl DrawBlock
	pop {lr, r0, r1, r2, r3, r4, r5, r6}
	bx lr

DrawBlock:
	push {lr, r0, r1, r2, r3, r4, r5, r6}
	//mov r0, #100 // x (input)
	//mov r1, #40 // y (input)
	//mov r2, #0xFFFFFFFF // color (input)
	//mov r3, #120 // width (input)
	//mov r4, #140 // height (input)
	add r5, r0, r3 // final x
	add r6, r1, r4 // final y
DrawLoop:
	bl SetPixel
	add r0, r0, #1
	cmp r0, r5
	bne DrawLoop // don't loop if x value is final
	subs r0, r0, r3 // reset x
	add r1, r1, #1
	cmp r1, r6
	bne DrawLoop // don't loop if y value is final
	pop {lr, r0, r1, r2, r3, r4, r5, r6}
	bx lr

SetPixel:
	push {lr, r0, r1, r2, r3}
	//mov r0, #20 // (input)
	//mov r1, #30 // (input)
	//mov r2, #0xFFFFFFFF // (input)
	ldr r3, =VGAaddress
	ldr r3, [r3]
	lsl r1, r1, #10
	lsl r0, r0, #1
	add r1, r0
	strh r2, [r3, r1]
	pop {lr, r0, r1, r2, r3}
	bx lr

DrawBar:
	push {lr, r0, r1, r2, r3, r4}
	mov r1, r0 // y is first input
	mov r0, #0
	//mov r1, #120 // (input)
	mov r2, #0xFFFFFFFF
	mov r3, #7
	mov r4, #45
	bl DrawBlock
	pop {lr, r0, r1, r2, r3, r4}
	bx lr
