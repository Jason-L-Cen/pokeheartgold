	.include "asm/macros.inc"
	.include "overlay_45_arm.inc"
	.include "global.inc"
	.public ov45_0223E4A8
	.public ov45_0223E558
	.extern ov45_022310B0
	.extern ov45_02231C78
	.extern ov45_02236E58
	.extern ov45_0223DCD0
	.extern ov45_02255200

.text
	arm_func_start ov45_0223E4A8
ov45_0223E4A8: ; 0x0223E4A8
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	sub sp, sp, #0x20
	ldr r1, _0223E554 ; =ov45_02255200
	mov r7, r0
	ldr r2, [r1, #8]
	ldr r1, [r1, #4]
	str r2, [sp, #4]
	str r1, [sp]
	ldr r0, [r7, #0xc]
	add r5, r7, #8
	str r1, [sp, #0x10]
	str r2, [sp, #0x14]
	str r5, [sp, #0x18]
	str r5, [sp, #8]
	str r0, [sp, #0x1c]
	str r0, [sp, #0xc]
	add r6, sp, #0
	add r4, sp, #0xc
	b _0223E4FC
_0223E4F4:
	mov r0, r4
	bl ov45_02236E58
_0223E4FC:
	ldr r0, [sp, #0xc]
	cmp r0, r5
	beq _0223E538
	ldr r1, [sp, #4]
	ldr r0, [r0, #0xc]
	tst r1, #1
	add r0, r0, r1, asr #1
	ldreq r1, [r6]
	beq _0223E52C
	ldr r2, [r0]
	ldr r1, [r6]
	ldr r1, [r2, r1]
_0223E52C:
	blx r1
	cmp r0, #0
	bne _0223E4F4
_0223E538:
	ldr r1, [sp, #0xc]
	add r0, r7, #8
	cmp r1, r0
	moveq r0, #1
	movne r0, #0
	add sp, sp, #0x20
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.balign 4, 0
_0223E554: .word ov45_02255200
	arm_func_end ov45_0223E4A8

	arm_func_start ov45_0223E558
ov45_0223E558: ; 0x0223E558
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x1c
	ldr r1, [r0, #0xc]
	add sb, r0, #8
	str r1, [sp]
	cmp r1, sb
	addeq sp, sp, #0x1c
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	mov r6, #0
	add fp, sp, #4
	add sl, sp, #0x10
_0223E584:
	ldr r0, [r1, #0xc]
	bl ov45_0223DCD0
	ldr r0, [sp]
	ldr r3, [r0, #0xc]
	ldr r0, [r3, #0x28]
	cmp r0, #3
	addls pc, pc, r0, lsl #2
	b _0223E62C
_0223E5A4: ; jump table
	b _0223E62C ; case 0
	b _0223E62C ; case 1
	b _0223E5B4 ; case 2
	b _0223E62C ; case 3
_0223E5B4:
	str r6, [r3, #0x14]
	ldr r0, [sp]
	ldr r5, [r0, #0xc]
	str r6, [r5, #0x24]
	ldr r2, [r3, #4]
	ldr r1, [r3, #8]
	ldr r0, [r3, #0xc]
	str r2, [sp, #0xc]
	str r1, [sp, #4]
	str r0, [sp, #8]
	ldmia fp, {r0, r1, r2}
	stmia sl, {r0, r1, r2}
	ldr r0, [sp]
	ldr r7, [r5, #0x1c]
	ldr r0, [r0, #0xc]
	ldr r8, [r0, #0x2c]
	bl ov45_022310B0
	mov r4, r0
	add r0, r5, #0x18
	bl ov45_02231C78
	mov r2, r0
	ldr r4, [r4, #0xc]
	mov r3, r7
	mov r0, r8
	mov r1, sl
	blx r4
	ldr r0, [sp]
	ldr r1, [r0, #0xc]
	mov r0, #1
	str r0, [r1, #0x38]
_0223E62C:
	add r0, sp, #0
	bl ov45_02236E58
	ldr r1, [sp]
	cmp r1, sb
	bne _0223E584
	add sp, sp, #0x1c
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	arm_func_end ov45_0223E558
