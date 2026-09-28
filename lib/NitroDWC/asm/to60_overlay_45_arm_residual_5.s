	.include "asm/macros.inc"
	.include "overlay_45_arm.inc"
	.include "global.inc"
	.public ov45_0223DB8C
	.extern ov45_02237890
	.extern ov45_022389B0
	.extern ov45_022389CC

.text
	arm_func_start ov45_0223DB8C
ov45_0223DB8C: ; 0x0223DB8C
	stmdb sp!, {r3, r4, r5, r6, lr}
	sub sp, sp, #0x1c
	mov r4, #0
	mov r6, r0
	add r3, r6, #8
	str r4, [sp, #8]
	str r4, [sp, #0xc]
	str r4, [sp, #0x10]
	str r4, [sp, #0x18]
	str r3, [sp, #0x14]
	mov r5, r1
	ldr r3, [r6, #4]
	ldr r1, [r6, #8]
	add r3, r3, r5
	sub r1, r3, r1
	mov r4, r2
	bl ov45_02237890
	mov r3, #0
	mov r1, r0
	strb r3, [sp, #3]
	sub r0, sp, #4
	strb r3, [r0]
	ldr r2, [r0]
	add r0, sp, #8
	strb r3, [sp, #4]
	bl ov45_022389CC
	ldr r2, [r6, #4]
	mov r0, #0
	ldr r1, [sp, #8]
	strb r0, [sp, #2]
	str r2, [sp, #0x18]
	add r3, r1, r2
	ldr r0, [sp, #0xc]
	ldrb r1, [r4]
	mov r2, r5
	add r0, r3, r0
	bl memset
	ldr r0, [sp, #0xc]
	mov r1, #0
	add r0, r0, r5
	str r0, [sp, #0xc]
	ldr r5, [r6]
	ldr r2, [r6, #4]
	ldr r0, [sp, #0x18]
	strb r1, [sp, #1]
	add r1, r5, r2
	sub r4, r1, r5
	sub r2, r0, r4
	ldr r0, [sp, #8]
	str r2, [sp, #0x18]
	add r0, r0, r2
	mov r1, r5
	mov r2, r4
	bl memcpy
	mov r0, r5
	mov r1, #0
	mov r2, r4
	bl memset
	ldr r1, [sp, #0xc]
	mov r0, #0
	add r1, r1, r4
	str r1, [sp, #0xc]
	strb r0, [sp]
	str r0, [r6, #4]
	add r3, sp, #0x10
	ldr r2, [r6, #8]
	ldr r1, [r3]
	add r0, sp, #8
	str r1, [r6, #8]
	str r2, [r3]
	ldr r2, [r6]
	ldr r1, [sp, #8]
	str r1, [r6]
	str r2, [sp, #8]
	ldr r2, [r6, #4]
	ldr r1, [sp, #0xc]
	str r1, [r6, #4]
	str r2, [sp, #0xc]
	bl ov45_022389B0
	add sp, sp, #0x1c
	ldmia sp!, {r3, r4, r5, r6, pc}
	arm_func_end ov45_0223DB8C
