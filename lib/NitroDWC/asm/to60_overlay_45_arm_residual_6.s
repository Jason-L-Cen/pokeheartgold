	.include "asm/macros.inc"
	.include "overlay_45_arm.inc"
	.include "global.inc"
	.public ov45_0223DF4C
	.extern ov45_02236E58
	.extern ov45_0223E6E8

.text
	arm_func_start ov45_0223DF4C
ov45_0223DF4C: ; 0x0223DF4C
	stmdb sp!, {r3, r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0xc
	mov r5, r0
	ldr r0, [r5, #0xc]
	add r4, r5, #8
	str r0, [sp, #8]
	cmp r0, r4
	addeq sp, sp, #0xc
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, pc}
	add r8, sp, #8
	add r7, sp, #0
	add r6, sp, #4
_0223DF7C:
	ldr r0, [r0, #0xc]
	cmp r0, #0
	beq _0223DF94
	ldr r1, [r0]
	ldr r1, [r1, #4]
	blx r1
_0223DF94:
	ldr r1, [sp, #8]
	mov r0, r8
	str r1, [sp, #4]
	bl ov45_02236E58
	ldr r2, [r6]
	mov r0, r7
	add r1, r5, #4
	bl ov45_0223E6E8
	ldr r0, [sp, #8]
	cmp r0, r4
	bne _0223DF7C
	add sp, sp, #0xc
	ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
	arm_func_end ov45_0223DF4C
