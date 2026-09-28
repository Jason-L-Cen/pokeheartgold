	.include "asm/macros.inc"
	.include "overlay_13_arm_2.inc"
	.include "global.inc"
	.public ov13_02240798
	.extern ov13_022402E0
	.extern ov13_02241EBC
	.extern ov13_0224F5E0

.text
	arm_func_start ov13_02240798
ov13_02240798: ; 0x02240798
	stmdb sp!, {r4, r5, r6, r7, r8, lr}
	sub sp, sp, #8
	ldr r4, _02240870 ; =ov13_0224F5E0
	mov r7, r0
	ldr r0, [r4]
	mov r6, r1
	ldr r0, [r0, #0x670]
	mov r5, r2
	mov r8, r3
	bl ov13_022402E0
	mov r4, r0
	strb r6, [r4, #0x2e]
	mul r1, r6, r5
	mov r2, r8
	strb r5, [r4, #0x2f]
	mov r0, r7
	add r3, sp, #4
	bl ov13_02241EBC
	str r0, [r4, #0x28]
	ldr r1, [sp, #4]
	mov r0, r6
	strh r1, [r4, #0x2c]
	mov r1, r5
	bl NNSi_G2dCalcRequiredOBJ
	ldr r1, [sp, #0x20]
	cmp r7, #1
	str r0, [r1]
	moveq ip, #0x6600000
	ldr r7, [sp, #4]
	mov r1, #4
	movne ip, #0x6400000
	str r1, [sp]
	mov r0, r4
	mov r2, r6
	mov r3, r5
	add r1, ip, r7, lsl #7
	bl NNS_G2dCharCanvasInitForOBJ1D
	ldr r1, [r4, #0x14]
	mov r0, r4
	ldr r2, [r1, #4]
	mov r1, #0
	blx r2
	ldr r0, _02240870 ; =ov13_0224F5E0
	ldr r2, [sp, #0x24]
	ldr r0, [r0]
	mov r1, #1
	str r4, [r4, #0x18]
	add r0, r0, r2, lsl #3
	str r0, [r4, #0x1c]
	str r1, [r4, #0x20]
	mov r0, r4
	str r1, [r4, #0x24]
	add sp, sp, #8
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	.balign 4, 0
_02240870: .word ov13_0224F5E0
	arm_func_end ov13_02240798
