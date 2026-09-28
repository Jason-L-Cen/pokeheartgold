	.include "asm/macros.inc"
	.include "overlay_45_arm.inc"
	.include "global.inc"
	.public ov45_02242EF4
	.public ov45_0224307C
	.extern ov45_02236E58
	.extern ov45_02237B58
	.extern ov45_02237BC8
	.extern ov45_02237FD8
	.extern ov45_0223E830
	.extern ov45_0223E910
	.extern ov45_0223FC5C
	.extern ov45_02242A2C
	.extern ov45_02242D30
	.extern ov45_02243128
	.extern ov45_022431AC

.text
	arm_func_start ov45_02242EF4
ov45_02242EF4: ; 0x02242EF4
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x34
	mov sb, r1
	movs r1, r2
	ldr r1, [sp, #0x58]
	mov r4, #0
	mov sl, r0
	str r4, [sl]
	str r4, [sl, #4]
	str r2, [sp]
	mov r8, r3
	str r4, [sl, #8]
	str r1, [sp, #0x58]
	beq _02242F34
	mov r1, r2
	bl ov45_0223E830
_02242F34:
	ldr r0, [sp]
	mov r7, #0
	cmp r0, #0
	addle sp, sp, #0x34
	ldmleia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	mvn r0, #0
	sub r4, sp, #4
	str r0, [sp, #4]
_02242F54:
	ldr r1, [r8, r7, lsl #2]
	add r0, sp, #0x28
	bl ov45_02237B58
	mov fp, #0
	ldr r6, [sb, #8]
	add r0, sp, #0x28
	mov r5, fp
	bl ov45_02242A2C
	cmp r0, r6
	bne _02242FB0
	ldr r6, [r8, r7, lsl #2]
	add r0, sp, #0x1c
	mov r1, sb
	bl ov45_0223FC5C
	mov r1, r6
	mov fp, #1
	add r0, sp, #0x1c
	bl ov45_02237FD8
	cmp r0, #0
	movne r0, fp
	moveq r0, r5
	cmp r0, #0
	movne r5, #1
_02242FB0:
	cmp fp, #0
	beq _02242FC0
	add r0, sp, #0x1c
	bl ov45_02237BC8
_02242FC0:
	add r0, sp, #0x28
	bl ov45_02237BC8
	cmp r5, #0
	bne _02243064
	ldr r1, [r8, r7, lsl #2]
	add r0, sp, #0x10
	bl ov45_02237B58
	ldr r2, [sp, #0x58]
	mov r0, sb
	add r1, sp, #0x10
	bl ov45_02242D30
	str r0, [sp, #0xc]
	add r0, sp, #0x10
	bl ov45_02237BC8
	ldr r1, [sp, #0xc]
	ldr r0, [sp, #4]
	cmp r1, r0
	beq _02243064
	mov r0, #0
	strb r0, [sp, #8]
	ldr r1, [sl, #4]
	ldr r0, [sl, #8]
	cmp r1, r0
	bhs _02243040
	mov r0, r1
	add r2, r0, #1
	str r2, [sl, #4]
	ldr r0, [sl]
	ldr r1, [sp, #0xc]
	add r0, r0, r2, lsl #2
	str r1, [r0, #-4]
	b _02243064
_02243040:
	mov r0, #0
	mov r2, #0
	strb r0, [sp, #9]
	mov r2, r2
	strb r2, [r4]
	ldr r2, [r4]
	mov r0, sl
	add r1, sp, #0xc
	bl ov45_0223E910
_02243064:
	ldr r0, [sp]
	add r7, r7, #1
	cmp r7, r0
	blt _02242F54
	add sp, sp, #0x34
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	arm_func_end ov45_02242EF4

	arm_func_start ov45_0224307C
ov45_0224307C: ; 0x0224307C
	stmdb sp!, {r0, r1, r2, r3}
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, lr}
	sub sp, sp, #0x10
	ldr r1, [sp, #0x34]
	mov r7, r0
	str r1, [sp, #4]
	ldr r0, [r7, #0x8c]
	str r1, [sp, #8]
	str r0, [sp, #0xc]
	add r6, r7, #0x84
	add r5, sp, #4
	add r8, sp, #0
	add r4, sp, #0xc
	b _022430BC
_022430B4:
	mov r0, r4
	bl ov45_02236E58
_022430BC:
	ldr sb, [sp, #0xc]
	cmp sb, r6
	beq _022430E8
	mov r0, r8
	mov r2, r5
	add r1, sb, #0x1c
	bl ov45_02243128
	ldr r1, [sp]
	add r0, sb, #0x20
	cmp r1, r0
	beq _022430B4
_022430E8:
	ldr r1, [sp, #0xc]
	add r0, r7, #0x84
	cmp r1, r0
	addne sp, sp, #0x10
	movne r0, #0
	ldmneia sp!, {r3, r4, r5, r6, r7, r8, sb, lr}
	addne sp, sp, #0x10
	bxne lr
	add r1, sp, #0x34
	add r0, r7, #0x70
	bl ov45_022431AC
	mov r0, #1
	add sp, sp, #0x10
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, lr}
	add sp, sp, #0x10
	bx lr
	arm_func_end ov45_0224307C
