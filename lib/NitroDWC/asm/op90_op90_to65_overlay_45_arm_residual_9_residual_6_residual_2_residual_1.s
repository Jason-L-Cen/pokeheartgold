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
	.public ov45_0224005C

.text
	arm_func_start ov45_0224005C
ov45_0224005C: ; 0x0224005C
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, lr}
	sub sp, sp, #0x6c
	ldr r0, _0224022C ; =ov45_022577DC
	mov r5, r1
	ldr r0, [r0]
	mov r4, r2
	mov r6, r3
	cmp r0, #0
	beq _02240090
	ldr r0, [r0, #4]
	cmp r0, #5
	addeq sp, sp, #0x6c
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, pc}
_02240090:
	cmp r5, #1
	bne _022400D4
	add r0, sp, #0x60
	mov r1, r4
	bl ov45_02237B58
	add r1, sp, #0x60
	ldr r0, _0224022C ; =ov45_022577DC
	str r1, [sp]
	ldr r1, [r0]
	ldr r3, [sp, #0x90]
	add r0, sp, #0x54
	mov r2, r6
	bl ov45_02242EF4
	add r0, sp, #0x54
	bl ov45_022376BC
	add r0, sp, #0x60
	bl ov45_02237BC8
_022400D4:
	add r0, sp, #0x48
	mov r1, r4
	bl ov45_02237B58
	ldr r0, _0224022C ; =ov45_022577DC
	add r1, sp, #0x48
	ldr r0, [r0]
	bl ov45_022437CC
	add r0, sp, #0x48
	bl ov45_02237BC8
	ldr r0, _0224022C ; =ov45_022577DC
	ldr r8, [sp, #0x98]
	ldr r6, [r0]
	ldr r0, _02240230 ; =ov45_02255238
	mov r1, #0
	str r0, [sp, #0xc]
	add r0, sp, #0xc
	str r1, [sp, #0x14]
	str r0, [sp, #4]
	add r0, sp, #8
	add r1, r6, #0x90
	add r2, sp, #4
	str r8, [sp, #0x10]
	bl ov45_0223FD8C
	ldr r1, [sp, #8]
	add r0, r6, #0x94
	cmp r1, r0
	moveq r6, #0
	ldrne r6, [r1, #0xc]
	add r0, sp, #0x3c
	mov r1, r4
	bl ov45_02237B58
	ldr r1, _0224022C ; =ov45_022577DC
	add r0, sp, #0x30
	ldr r1, [r1]
	add r2, sp, #0x3c
	bl ov45_022428D8
	ldr r0, [sp, #0x30]
	ldr r7, [r6, #0xc]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	mov r0, r6
	ldr r1, [r0]
	addeq sb, sp, #0x31
	ldrne sb, [sp, #0x38]
	cmp r5, #1
	moveq sl, #1
	ldr r1, [r1, #0xc]
	movne sl, #0
	blx r1
	mov r3, r0
	mov r0, sl
	mov r2, sb
	mov r1, #0
	blx r7
	add r0, sp, #0x30
	bl ov45_02237BC8
	add r0, sp, #0x3c
	bl ov45_02237BC8
	ldr r0, _0224022C ; =ov45_022577DC
	mov r1, r8
	ldr r0, [r0]
	add r0, r0, #0x90
	bl ov45_02242510
	cmp r5, #0
	addne sp, sp, #0x6c
	ldmneia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, pc}
	add r0, sp, #0x24
	mov r1, r4
	bl ov45_02237B58
	ldr r1, _0224022C ; =ov45_022577DC
	add r0, sp, #0x18
	ldr r1, [r1]
	add r2, sp, #0x24
	bl ov45_022428D8
	ldr r0, [sp, #0x18]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r0, sp, #0x19
	ldrne r0, [sp, #0x20]
	bl ov45_022412CC
	add r0, sp, #0x18
	bl ov45_02237BC8
	add r0, sp, #0x24
	bl ov45_02237BC8
	add sp, sp, #0x6c
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, pc}
	.balign 4, 0
_0224022C: .word ov45_022577DC
_02240230: .word ov45_02255238
	arm_func_end ov45_0224005C
