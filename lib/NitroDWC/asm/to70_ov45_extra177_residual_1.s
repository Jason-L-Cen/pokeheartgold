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
	.extern ov45_0223FD74
	.extern ov45_0223FD8C
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

.text
	arm_func_start ov45_0223FA3C
ov45_0223FA3C: ; 0x0223FA3C
	stmdb sp!, {r4, lr}
	sub sp, sp, #0x60
	ldr r2, _0223FC54 ; =ov45_022577DC
	mov r4, r0
	ldr r0, [r2]
	cmp r0, #0
	beq _0223FA68
	ldr r0, [r0, #4]
	cmp r0, #5
	addeq sp, sp, #0x60
	ldmeqia sp!, {r4, pc}
_0223FA68:
	cmp r1, #1
	bne _0223FADC
	ldr r1, _0223FC54 ; =ov45_022577DC
	add r0, sp, #0x54
	ldr r1, [r1]
	ldr r1, [r1, #8]
	bl ov45_022429E0
	ldr r0, _0223FC54 ; =ov45_022577DC
	add r1, sp, #0x54
	ldr r0, [r0]
	add r0, r0, #0xc
	bl ov45_02237D48
	add r0, sp, #0x54
	bl ov45_02237BC8
	ldr r1, _0223FC54 ; =ov45_022577DC
	add r0, sp, #0x48
	ldr r1, [r1]
	bl ov45_0223FC5C
	ldr r0, [sp, #0x48]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r1, sp, #0x49
	ldrne r1, [sp, #0x50]
	mov r0, r4
	bl ov45_0225035C
	add r0, sp, #0x48
	bl ov45_02237BC8
	add sp, sp, #0x60
	ldmia sp!, {r4, pc}
_0223FADC:
	cmp r1, #2
	bne _0223FB50
	ldr r1, _0223FC54 ; =ov45_022577DC
	add r0, sp, #0x3c
	ldr r1, [r1]
	ldr r1, [r1, #8]
	bl ov45_022429E0
	ldr r0, _0223FC54 ; =ov45_022577DC
	add r1, sp, #0x3c
	ldr r0, [r0]
	add r0, r0, #0xc
	bl ov45_02237D48
	add r0, sp, #0x3c
	bl ov45_02237BC8
	ldr r1, _0223FC54 ; =ov45_022577DC
	add r0, sp, #0x30
	ldr r1, [r1]
	bl ov45_0223FC5C
	ldr r0, [sp, #0x30]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r1, sp, #0x31
	ldrne r1, [sp, #0x38]
	mov r0, r4
	bl ov45_0225035C
	add r0, sp, #0x30
	bl ov45_02237BC8
	add sp, sp, #0x60
	ldmia sp!, {r4, pc}
_0223FB50:
	sub r0, r1, #3
	cmp r0, #1
	bhi _0223FBD0
	ldr r1, _0223FC54 ; =ov45_022577DC
	add r0, sp, #0x24
	ldr r1, [r1]
	ldr r1, [r1, #8]
	bl ov45_022429E0
	ldr r0, _0223FC54 ; =ov45_022577DC
	add r1, sp, #0x24
	ldr r0, [r0]
	add r0, r0, #0xc
	bl ov45_02237D48
	add r0, sp, #0x24
	bl ov45_02237BC8
	ldr r1, _0223FC54 ; =ov45_022577DC
	add r0, sp, #0x18
	ldr r1, [r1]
	bl ov45_0223FC5C
	ldr r0, [sp, #0x18]
	ldr r3, _0223FC58 ; =ov45_02255304
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r2, sp, #0x19
	ldrne r2, [sp, #0x20]
	mov r0, r4
	mov r1, #2
	bl ov45_02250424
	add r0, sp, #0x18
	bl ov45_02237BC8
	add sp, sp, #0x60
	ldmia sp!, {r4, pc}
_0223FBD0:
	cmp r1, #5
	addeq sp, sp, #0x60
	ldmeqia sp!, {r4, pc}
	cmp r1, #6
	addne sp, sp, #0x60
	ldmneia sp!, {r4, pc}
	ldr r1, _0223FC54 ; =ov45_022577DC
	add r0, sp, #0xc
	ldr r1, [r1]
	ldr r1, [r1, #8]
	bl ov45_022429E0
	ldr r0, _0223FC54 ; =ov45_022577DC
	add r1, sp, #0xc
	ldr r0, [r0]
	add r0, r0, #0xc
	bl ov45_02237D48
	add r0, sp, #0xc
	bl ov45_02237BC8
	ldr r1, _0223FC54 ; =ov45_022577DC
	add r0, sp, #0
	ldr r1, [r1]
	bl ov45_0223FC5C
	ldr r0, [sp]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r1, sp, #1
	ldrne r1, [sp, #8]
	mov r0, r4
	bl ov45_0225035C
	add r0, sp, #0
	bl ov45_02237BC8
	add sp, sp, #0x60
	ldmia sp!, {r4, pc}
	.balign 4, 0
_0223FC54: .word ov45_022577DC
_0223FC58: .word ov45_02255304
	arm_func_end ov45_0223FA3C
