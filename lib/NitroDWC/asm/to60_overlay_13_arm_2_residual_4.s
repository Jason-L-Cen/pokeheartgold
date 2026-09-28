	.include "asm/macros.inc"
	.include "overlay_13_arm_2.inc"
	.include "global.inc"
	.public ov13_0222FD10
	.extern ov13_022400F0
	.extern ov13_02240184
	.extern ov13_0224544C
	.extern ov13_02245450
	.extern ov13_0224F538

.text
	arm_func_start ov13_0222FD10
ov13_0222FD10: ; 0x0222FD10
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, lr}
	ldr r0, _0222FE74 ; =ov13_0224F538
	mov sb, #0
	ldr r0, [r0, #4]
	ldrb r8, [r0, #0x51]
	cmp r8, #5
	movgt r8, #5
	cmp r8, #0
	ble _0222FD84
	ldr sl, _0222FE74 ; =ov13_0224F538
	mov r7, sb
	mov r6, #0x2e
	mov r5, sb
	mov r4, #0x18
_0222FD48:
	mov r0, r7
	mov r1, r6
	bl ov13_02240184
	ldr r2, [sl, #4]
	mov r1, r4
	add r2, r2, sb, lsl #2
	str r0, [r2, #0x10]
	mov r0, r5
	bl ov13_02240184
	ldr r1, [sl, #4]
	add r1, r1, sb, lsl #2
	add sb, sb, #1
	str r0, [r1, #0x24]
	cmp sb, r8
	blt _0222FD48
_0222FD84:
	mov r6, #0
	ldr r5, _0222FE78 ; =ov13_0224544C
	ldr sb, _0222FE74 ; =ov13_0224F538
	ldr r7, _0222FE7C ; =0x000003FF
	mov r4, r6
_0222FD98:
	ldr r0, [sb, #4]
	ldrb r1, [r5], #1
	ldr r2, [r0, #0x10]
	mov r0, r4
	bl ov13_022400F0
	ldr r0, [sb, #4]
	ldr r1, [r0, #0x10]
	add r0, r0, r6, lsl #1
	ldrh r1, [r1, #4]
	add r6, r6, #1
	cmp r6, #3
	and r1, r1, r7
	strh r1, [r0, #0x42]
	blo _0222FD98
	mov r6, #0
	ldr r5, _0222FE80 ; =ov13_02245450
	ldr sb, _0222FE74 ; =ov13_0224F538
	ldr r7, _0222FE7C ; =0x000003FF
	mov r4, r6
_0222FDE4:
	ldr r0, [sb, #4]
	ldrb r1, [r5], #1
	ldr r2, [r0, #0x24]
	mov r0, r4
	bl ov13_022400F0
	ldr r0, [sb, #4]
	ldr r1, [r0, #0x24]
	add r0, r0, r6, lsl #1
	ldrh r1, [r1, #4]
	add r6, r6, #1
	cmp r6, #4
	and r1, r1, r7
	strh r1, [r0, #0x48]
	blo _0222FDE4
	cmp r8, #0
	mov r0, #0
	ldmleia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
	ldr r1, _0222FE74 ; =ov13_0224F538
_0222FE2C:
	ldr r2, [r1, #4]
	add r2, r2, r0, lsl #2
	ldr r3, [r2, #0x10]
	ldrh r2, [r3, #4]
	bic r2, r2, #0xc00
	orr r2, r2, #0xc00
	strh r2, [r3, #4]
	ldr r2, [r1, #4]
	add r2, r2, r0, lsl #2
	ldr r3, [r2, #0x24]
	add r0, r0, #1
	ldrh r2, [r3, #4]
	cmp r0, r8
	bic r2, r2, #0xc00
	orr r2, r2, #0xc00
	strh r2, [r3, #4]
	blt _0222FE2C
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
	.balign 4, 0
_0222FE74: .word ov13_0224F538
_0222FE78: .word ov13_0224544C
_0222FE7C: .word 0x000003FF
_0222FE80: .word ov13_02245450
	arm_func_end ov13_0222FD10
