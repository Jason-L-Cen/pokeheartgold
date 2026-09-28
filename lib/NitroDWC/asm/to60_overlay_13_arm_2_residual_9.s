	.include "asm/macros.inc"
	.include "overlay_13_arm_2.inc"
	.include "global.inc"
	.public ov13_0223A8F0
	.extern ov13_0224128C
	.extern ov13_022412B0
	.extern ov13_0224F5A4

.text
	arm_func_start ov13_0223A8F0
ov13_0223A8F0: ; 0x0223A8F0
	stmdb sp!, {r3, r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0x14
	ldr r1, _0223AA54 ; =ov13_0224F5A4
	mov r7, r0
	ldr r0, [r1]
	mov r6, #0
	add r2, r0, r7, lsl #8
	ldrb r5, [r2, #0xe7]
	add r0, sp, #4
	mov r1, r6
	mov r2, #0x10
	mov r4, #1
	bl MI_CpuFill8
	add r0, sp, #4
	mov r2, r4
	str r2, [r0, r7, lsl #2]
	cmp r7, #2
	bgt _0223A9CC
	ldr r0, _0223AA54 ; =ov13_0224F5A4
	ldr r0, [r0]
	ldrb r1, [r0, #0xef]
	tst r1, r4, lsl r7
	movne r6, r2
	cmp r5, #0xff
	bne _0223A990
	cmp r6, #0
	beq _0223A990
	ldrb r1, [r0, #0xef]
	mvn r3, r4, lsl r7
	and r2, r1, r3
	ldr r1, _0223AA54 ; =ov13_0224F5A4
	strb r2, [r0, #0xef]
	ldr r2, [r1]
	mov r0, #1
	ldrb r1, [r2, #0x1ef]
	and r1, r1, r3
	strb r1, [r2, #0x1ef]
	str r0, [sp, #8]
	str r0, [sp, #4]
	b _0223A9CC
_0223A990:
	cmp r5, #0xff
	beq _0223A9CC
	cmp r6, #0
	bne _0223A9CC
	ldrb r3, [r0, #0xef]
	ldr r1, _0223AA54 ; =ov13_0224F5A4
	mov r2, #1
	orr r3, r3, r4, lsl r7
	strb r3, [r0, #0xef]
	ldr r1, [r1]
	ldrb r0, [r1, #0x1ef]
	orr r0, r0, r4, lsl r7
	strb r0, [r1, #0x1ef]
	str r2, [sp, #8]
	str r2, [sp, #4]
_0223A9CC:
	mov r6, #0
	ldr r8, _0223AA54 ; =ov13_0224F5A4
	mov r7, r6
	mov r4, #0xfe
	add r5, sp, #4
_0223A9E0:
	ldr r0, [r5, r6, lsl #2]
	cmp r0, #0
	beq _0223AA10
	ldr r1, [r8]
	mov r2, r4
	add r0, r1, #0xf8
	add r0, r0, #0x400
	add r1, r1, r7
	bl MATH_CalcCRC16
	ldr r1, [r8]
	add r1, r1, r6, lsl #8
	strh r0, [r1, #0xfe]
_0223AA10:
	add r6, r6, #1
	cmp r6, #4
	add r7, r7, #0x100
	blt _0223A9E0
	mov r0, #0x100
	mov r1, #0x20
	bl ov13_0224128C
	mov r2, r0
	ldr r0, _0223AA54 ; =ov13_0224F5A4
	add r1, sp, #4
	ldr r0, [r0]
	str r2, [sp]
	bl DWCi_BACKUPlWritePage
	add r0, sp, #0
	bl ov13_022412B0
	add sp, sp, #0x14
	ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
	.balign 4, 0
_0223AA54: .word ov13_0224F5A4
	arm_func_end ov13_0223A8F0
