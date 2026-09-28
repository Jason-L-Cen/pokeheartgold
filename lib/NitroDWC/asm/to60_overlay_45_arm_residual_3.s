	.include "asm/macros.inc"
	.include "overlay_45_arm.inc"
	.include "global.inc"
	.public ov45_0223CF30
	.extern ov45_0223D1C4
	.extern ov45_0223D208

.text
	arm_func_start ov45_0223CF30
ov45_0223CF30: ; 0x0223CF30
	stmdb sp!, {r3, r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0x5c
	mov r7, r0
	ldr r4, [r7]
	mov r6, r1
	cmp r4, #0
	mov r5, r2
	addeq sp, sp, #0x5c
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, pc}
	ldr r2, [r7, #4]
	ldr r0, [r7, #8]
	ldr r1, [r7, #0xc]
	add r0, r2, r0
	bl _u32_div_f
	add r4, r4, r1, lsl #2
	mov r0, r5
	mov r1, r6
	bl ov45_0223D208
	str r0, [sp, #0x18]
	ldr r2, [r7]
	ldr r1, [r7, #0xc]
	add r1, r2, r1, lsl #2
	sub r2, r1, r4
	mov r1, r2, asr #1
	add r1, r2, r1, lsr #30
	mov r2, r1, asr #2
	cmp r0, r1, asr #2
	addlt r1, sp, #0x18
	str r2, [sp, #0x14]
	addge r1, sp, #0x14
	ldr r1, [r1]
	str r1, [sp, #0x14]
	ldr r1, [r7, #8]
	add r0, r1, r0
	str r0, [r7, #8]
	ldr r0, [r6]
	ldr r1, [sp, #0x14]
	str r0, [sp, #0x2c]
	ldr r2, [r6, #4]
	add r0, sp, #0x2c
	str r2, [sp, #0x30]
	ldr r2, [r6, #8]
	str r2, [sp, #0x34]
	ldr r2, [r6, #0xc]
	str r2, [sp, #0x38]
	bl ov45_0223D1C4
	ldr r8, [r0]
	ldr r3, [r0, #4]
	ldr r2, [r0, #8]
	ldr r1, [r0, #0xc]
	str r8, [sp, #0x4c]
	add r0, sp, #0x4c
	mov r8, sp
	str r3, [sp, #0x50]
	str r2, [sp, #0x54]
	str r1, [sp, #0x58]
	ldmia r0, {r0, r1, r2, r3}
	stmia r8, {r0, r1, r2, r3}
	str r4, [sp, #0x10]
	ldmia r6, {r0, r1, r2, r3}
	bl ov45_0223D0A8
	ldr r1, [sp, #0x14]
	ldr r0, [sp, #0x18]
	subs r0, r0, r1
	str r0, [sp, #0x18]
	addeq sp, sp, #0x5c
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, pc}
	ldr r2, [r6]
	add r0, sp, #0x1c
	str r2, [sp, #0x1c]
	ldr r2, [r6, #4]
	str r2, [sp, #0x20]
	ldr r2, [r6, #8]
	str r2, [sp, #0x24]
	ldr r2, [r6, #0xc]
	str r2, [sp, #0x28]
	bl ov45_0223D1C4
	ldmia r0, {r8, lr}
	ldr ip, [r0, #8]
	ldr r6, [r0, #0xc]
	mov r4, sp
	ldmia r5, {r0, r1, r2, r3}
	stmia r4, {r0, r1, r2, r3}
	ldr r4, [r7]
	str r8, [sp, #0x3c]
	add r0, sp, #0x3c
	str lr, [sp, #0x40]
	str ip, [sp, #0x44]
	str r6, [sp, #0x48]
	ldmia r0, {r0, r1, r2, r3}
	str r4, [sp, #0x10]
	bl ov45_0223D0A8
	add sp, sp, #0x5c
	ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
	arm_func_end ov45_0223CF30

	arm_func_start ov45_0223D0A8
ov45_0223D0A8: ; 0x0223D0A8
	stmdb sp!, {r0, r1, r2, r3}
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	ldr r0, [sp, #0x2c]
	ldr r6, [sp, #0x28]
	ldr r5, [sp, #0x38]
	ldr sl, [sp, #0x48]
	cmp r6, r5
	str r0, [sp]
	ldr r8, [sp, #0x34]
	ldr r7, [sp, #0x30]
	ldr fp, [sp, #0x40]
	beq _0223D14C
_0223D0D8:
	cmp r6, r7
	movhs r0, #1
	movlo r0, #0
	cmp r5, fp
	movhs r1, #1
	movlo r1, #0
	cmp r0, r1
	subeq r1, r5, r6
	subne r1, r8, r6
	mov r0, r1, asr #1
	add r0, r1, r0, lsr #30
	mov sb, r0, asr #2
	add r0, r6, sb, lsl #2
	sub r1, r0, r6
	mov r0, r1, asr #1
	add r0, r1, r0, lsr #30
	mov r4, r0, asr #2
	mov r2, r4, lsl #2
	mov r0, sl
	mov r1, r6
	bl memmove
	sub r0, sb, #1
	add r0, r6, r0, lsl #2
	add r6, r0, #4
	cmp r6, r8
	ldreq r6, [sp]
	add sl, sl, r4, lsl #2
	cmp r6, r5
	bne _0223D0D8
_0223D14C:
	mov r0, sl
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	add sp, sp, #0x10
	bx lr
	arm_func_end ov45_0223D0A8
