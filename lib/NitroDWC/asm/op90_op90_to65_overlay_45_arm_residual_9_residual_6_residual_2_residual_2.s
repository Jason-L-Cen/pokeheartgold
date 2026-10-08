	.extern ov45_0223259C
	.extern ov45_02232AE8
	.extern ov45_02232D8C
	.extern ov45_02233ABC
	.extern ov45_02236680
	.extern ov45_02236BA8
	.extern ov45_02236C40
	.extern ov45_02236E58
	.extern ov45_02237634
	.extern ov45_02237664
	.extern ov45_022376BC
	.extern ov45_02237A24
	.extern ov45_02237B58
	.extern ov45_02237BC8
	.extern ov45_02237D48
	.extern ov45_0223D644
	.extern ov45_0223DAD4
	.extern ov45_0223EA34
	.extern ov45_02240F8C
	.extern ov45_022412CC
	.extern ov45_022421E4
	.extern ov45_02242254
	.extern ov45_02242394
	.extern ov45_0224246C
	.extern ov45_022424DC
	.extern ov45_02242510
	.extern ov45_0224268C
	.extern ov45_02242858
	.extern ov45_022428C8
	.extern ov45_022428D8
	.extern ov45_02242910
	.extern ov45_022429E0
	.extern ov45_02242A2C
	.extern ov45_02242C3C
	.extern ov45_02242D30
	.extern ov45_02242EF4
	.extern ov45_02243280
	.extern ov45_0224335C
	.extern ov45_022435F8
	.extern ov45_0224367C
	.extern ov45_022436E0
	.extern ov45_02243794
	.extern ov45_022437CC
	.extern ov45_02243808
	.extern ov45_022502D0
	.extern ov45_0225035C
	.extern ov45_02250424
	.extern ov45_02250590
	.extern ov45_02250E6C
	.extern ov45_02250F5C
	.extern ov45_02251024
	.extern ov45_02251208
	.extern ov45_02255238
	.extern ov45_022552C0
	.extern ov45_022552C8
	.extern ov45_022552D8
	.extern ov45_022552DC
	.extern ov45_022552E4
	.extern ov45_022552EC
	.extern ov45_022552F0
	.extern ov45_022552FC
	.extern ov45_02255304
	.extern ov45_02255308
	.extern ov45_02255310
	.extern ov45_02255318
	.extern ov45_0225531C
	.extern ov45_022577DC
	.include "asm/macros.inc"
	.include "overlay_45_arm.inc"
	.include "global.inc"
	.extern ov45_0223FC5C
	.extern ov45_0223FD8C

.text
	arm_func_start ov45_02240494
ov45_02240494: ; 0x02240494
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x40
	ldr r0, _02240618 ; =ov45_022577DC
	mov r7, r1
	ldr r0, [r0]
	mov sb, r2
	cmp r0, #0
	beq _022404C4
	ldr r0, [r0, #4]
	cmp r0, #5
	addeq sp, sp, #0x40
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_022404C4:
	cmp sb, #0
	addeq sp, sp, #0x40
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	cmp r3, #0
	mvneq fp, #0
	beq _02240504
	add r0, sp, #0x34
	mov r1, r3
	bl ov45_02237B58
	ldr r0, _02240618 ; =ov45_022577DC
	add r1, sp, #0x34
	ldr r0, [r0]
	bl ov45_022435F8
	mov fp, r0
	add r0, sp, #0x34
	bl ov45_02237BC8
_02240504:
	ldr r0, _02240618 ; =ov45_022577DC
	ldr r8, [sp, #0x74]
	ldr r4, [r0]
	ldr r6, _0224061C ; =ov45_02255238
	add r3, sp, #0x10
	mov r5, #0
	add r0, sp, #0xc
	add r2, sp, #8
	add r1, r4, #0x90
	str r6, [sp, #0x10]
	str r8, [sp, #0x14]
	str r5, [sp, #0x18]
	str r3, [sp, #8]
	bl ov45_0223FD8C
	add r0, r4, #0x94
	ldr r1, [sp, #0xc]
	mov r4, #0
	cmp r1, r0
	moveq r6, r5
	ldrne r6, [r1, #0xc]
	mov r5, r4
	cmp r7, #1
	bne _0224057C
	ldr r0, [sp, #0x68]
	cmp r0, #0
	ble _0224057C
	ldr r1, [sp, #0x6c]
	ldr r0, [sp, #0x70]
	ldr r4, [r1]
	ldr r5, [r0]
_0224057C:
	add r0, sp, #0x28
	mov r1, sb
	bl ov45_02237B58
	ldr r1, _02240618 ; =ov45_022577DC
	add r0, sp, #0x1c
	ldr r1, [r1]
	add r2, sp, #0x28
	bl ov45_022428D8
	ldr r0, [sp, #0x1c]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	mov r0, r6
	ldr r1, [r0]
	addeq sb, sp, #0x1d
	ldrne sb, [sp, #0x24]
	cmp r7, #1
	moveq sl, #1
	ldr r7, [r6, #0xc]
	ldr r1, [r1, #0xc]
	movne sl, #0
	blx r1
	str r5, [sp]
	str r0, [sp, #4]
	mov r0, sl
	mov r1, sb
	mov r2, fp
	mov r3, r4
	blx r7
	add r0, sp, #0x1c
	bl ov45_02237BC8
	add r0, sp, #0x28
	bl ov45_02237BC8
	ldr r0, _02240618 ; =ov45_022577DC
	mov r1, r8
	ldr r0, [r0]
	add r0, r0, #0x90
	bl ov45_02242510
	add sp, sp, #0x40
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.balign 4, 0
_02240618: .word ov45_022577DC
_0224061C: .word ov45_02255238
	arm_func_end ov45_02240494
