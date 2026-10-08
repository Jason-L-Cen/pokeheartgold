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
	.extern ov45_0223FFE8

.text
	arm_func_start ov45_0223FE28
ov45_0223FE28: ; 0x0223FE28
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, lr}
	sub sp, sp, #0x58
	ldr r0, _0223FFE0 ; =ov45_022577DC
	mov r4, r1
	ldr r0, [r0]
	mov r6, r2
	mov r5, r3
	cmp r0, #0
	beq _0223FE5C
	ldr r0, [r0, #4]
	cmp r0, #5
	addeq sp, sp, #0x58
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
_0223FE5C:
	mov r0, #0
	str r0, [sp, #0x4c]
	str r0, [sp, #0x50]
	str r0, [sp, #0x54]
	cmp r5, #0
	beq _0223FEE4
	add r0, sp, #0x40
	mov r1, r6
	bl ov45_02237B58
	add r1, sp, #0x40
	ldr r0, _0223FFE0 ; =ov45_022577DC
	str r1, [sp]
	ldr r1, [r0]
	ldr r3, [sp, #0x78]
	add r0, sp, #0x34
	mov r2, r5
	bl ov45_02242EF4
	add r0, sp, #0x4c
	add r1, sp, #0x34
	cmp r0, r1
	beq _0223FED4
	mov r3, #0
	ldr r1, [sp, #0x34]
	ldr r5, [sp, #0x38]
	strb r3, [sp, #4]
	sub r2, sp, #4
	strb r3, [r2]
	ldr r3, [r2]
	add r2, r1, r5, lsl #2
	bl ov45_0223FFE8
_0223FED4:
	add r0, sp, #0x34
	bl ov45_022376BC
	add r0, sp, #0x40
	bl ov45_02237BC8
_0223FEE4:
	ldr r0, _0223FFE0 ; =ov45_022577DC
	ldr r8, [sp, #0x80]
	ldr r5, [r0]
	ldr sb, _0223FFE4 ; =ov45_02255238
	add r3, sp, #0x10
	mov r7, #0
	add r0, sp, #0xc
	add r2, sp, #8
	add r1, r5, #0x90
	str sb, [sp, #0x10]
	str r8, [sp, #0x14]
	str r7, [sp, #0x18]
	str r3, [sp, #8]
	bl ov45_0223FD8C
	ldr r1, [sp, #0xc]
	add r0, r5, #0x94
	cmp r1, r0
	moveq r5, r7
	ldrne r5, [r1, #0xc]
	add r0, sp, #0x28
	mov r1, r6
	bl ov45_02237B58
	ldr r1, _0223FFE0 ; =ov45_022577DC
	add r0, sp, #0x1c
	ldr r1, [r1]
	add r2, sp, #0x28
	bl ov45_022428D8
	ldr r0, [sp, #0x1c]
	ldr r6, [sp, #0x50]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq sb, sp, #0x1d
	ldrne sb, [sp, #0x24]
	cmp r4, #1
	moveq sl, #1
	add r0, sp, #0x4c
	ldr r7, [r5, #0xc]
	movne sl, #0
	bl ov45_0223259C
	mov r4, r0
	mov r0, r5
	ldr r1, [r0]
	ldr r1, [r1, #0xc]
	blx r1
	str r0, [sp]
	mov r1, sb
	mov r2, r6
	mov r3, r4
	mov r0, sl
	blx r7
	add r0, sp, #0x1c
	bl ov45_02237BC8
	add r0, sp, #0x28
	bl ov45_02237BC8
	ldr r0, _0223FFE0 ; =ov45_022577DC
	mov r1, r8
	ldr r0, [r0]
	add r0, r0, #0x90
	bl ov45_02242510
	add r0, sp, #0x4c
	bl ov45_022376BC
	add sp, sp, #0x58
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
	.balign 4, 0
_0223FFE0: .word ov45_022577DC
_0223FFE4: .word ov45_02255238
	arm_func_end ov45_0223FE28
