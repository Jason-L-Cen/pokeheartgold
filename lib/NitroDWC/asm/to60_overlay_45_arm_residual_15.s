	.include "asm/macros.inc"
	.include "overlay_45_arm.inc"
	.include "global.inc"
	.public ov45_02242ACC
	.extern ov45_02237A24
	.extern ov45_02237BC8
	.extern ov45_02242BE0
	.extern ov45_02243840
	.extern ov45_02243BC0
	.extern ov45_02243F1C

.text
	arm_func_start ov45_02242ACC
ov45_02242ACC: ; 0x02242ACC
	stmdb sp!, {r3, r4, r5, r6, lr}
	sub sp, sp, #0xb4
	mov r4, r0
	add r0, sp, #0xa8
	mov r6, r2
	mov r5, r3
	bl ov45_02237A24
	add r1, sp, #0x80
	mov r2, #0
	add r0, r1, #4
	add ip, sp, #0x90
	str r0, [r1, #8]
	str r2, [sp, #0x7c]
	str r2, [r1]
	str r2, [r1, #4]
	str r5, [sp, #0x8c]
	ldmia r6!, {r0, r1, r2, r3}
	stmia ip!, {r0, r1, r2, r3}
	ldmia r6, {r0, r1}
	stmia ip, {r0, r1}
	add r0, sp, #0x44
	add r1, sp, #0xa8
	add r2, sp, #0x7c
	bl ov45_02242BE0
	mov r1, #0
	strb r1, [sp]
	add r0, sp, #0xc
	add r1, sp, #0x44
	bl ov45_02237A24
	add r6, sp, #0x50
	mov r1, #0
	ldr r0, [r6]
	add r5, sp, #0x18
	stmia r5, {r0, r1}
	add r0, r5, #8
	str r0, [r5, #0xc]
	strb r1, [sp, #1]
	str r1, [r5, #8]
	add r0, r5, #4
	add r1, r6, #4
	bl ov45_02243BC0
	ldr r0, [r6, #0x10]
	add lr, r6, #0x14
	str r0, [r5, #0x10]
	add ip, r5, #0x14
	ldmia lr!, {r0, r1, r2, r3}
	stmia ip!, {r0, r1, r2, r3}
	ldmia lr, {r0, r1}
	stmia ip, {r0, r1}
	add r1, r4, #0x80
	add r0, sp, #4
	add r2, sp, #0xc
	bl ov45_02243840
	add r0, sp, #0x1c
	ldrb r4, [sp, #8]
	bl ov45_02243F1C
	add r0, sp, #0xc
	bl ov45_02237BC8
	add r0, sp, #0x54
	bl ov45_02243F1C
	add r0, sp, #0x44
	bl ov45_02237BC8
	add r0, sp, #0x80
	bl ov45_02243F1C
	add r0, sp, #0xa8
	bl ov45_02237BC8
	mov r0, r4
	add sp, sp, #0xb4
	ldmia sp!, {r3, r4, r5, r6, pc}
	arm_func_end ov45_02242ACC
