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

.text
	arm_func_start ov45_0223F768
ov45_0223F768: ; 0x0223F768
	stmdb sp!, {lr}
	sub sp, sp, #0xc
	ldr r0, _0223F824 ; =ov45_022577DC
	ldr r0, [r0]
	cmp r0, #0
	beq _0223F790
	ldr r0, [r0, #4]
	cmp r0, #5
	addeq sp, sp, #0xc
	ldmeqia sp!, {pc}
_0223F790:
	mov r2, #0
	add r1, sp, #0
	mov r0, r2
_0223F79C:
	str r0, [r1, r2, lsl #2]
	add r2, r2, #1
	cmp r2, #3
	blo _0223F79C
	cmp r3, #3
	addls pc, pc, r3, lsl #2
	b _0223F808
_0223F7B8: ; jump table
	b _0223F7C8 ; case 0
	b _0223F7D8 ; case 1
	b _0223F7E8 ; case 2
	b _0223F7F8 ; case 3
_0223F7C8:
	ldr r1, _0223F828 ; =ov45_022552DC
	add r0, sp, #0
	bl ov45_0223DAD4
	b _0223F814
_0223F7D8:
	ldr r1, _0223F82C ; =ov45_022552E4
	add r0, sp, #0
	bl ov45_0223DAD4
	b _0223F814
_0223F7E8:
	ldr r1, _0223F830 ; =ov45_022552EC
	add r0, sp, #0
	bl ov45_0223DAD4
	b _0223F814
_0223F7F8:
	ldr r1, _0223F834 ; =ov45_022552F0
	add r0, sp, #0
	bl ov45_0223DAD4
	b _0223F814
_0223F808:
	ldr r1, _0223F838 ; =ov45_022552FC
	add r0, sp, #0
	bl ov45_0223DAD4
_0223F814:
	add r0, sp, #0
	bl ov45_02237BC8
	add sp, sp, #0xc
	ldmia sp!, {pc}
	.balign 4, 0
_0223F824: .word ov45_022577DC
_0223F828: .word ov45_022552DC
_0223F82C: .word ov45_022552E4
_0223F830: .word ov45_022552EC
_0223F834: .word ov45_022552F0
_0223F838: .word ov45_022552FC
	arm_func_end ov45_0223F768
