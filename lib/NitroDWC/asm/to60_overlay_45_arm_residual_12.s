	.include "asm/macros.inc"
	.include "overlay_45_arm.inc"
	.include "global.inc"
	.public ov45_02241E24
	.extern ov45_02237B58
	.extern ov45_02237BC8
	.extern ov45_02240718
	.extern ov45_0224206C
	.extern ov45_02242394
	.extern ov45_022428C8
	.extern ov45_02250C80
	.extern ov45_02255238
	.extern ov45_02255250
	.extern ov45_022577DC
	.extern ov45_022577E0

.text
	arm_func_start ov45_02241E24
ov45_02241E24: ; 0x02241E24
	stmdb sp!, {r4, r5, r6, r7, lr}
	sub sp, sp, #0x2c
	ldr r3, _02241F6C ; =ov45_022577DC
	mov r7, r0
	ldr r4, [r3]
	mov r6, r1
	cmp r4, #0
	mov r5, r2
	addeq sp, sp, #0x2c
	moveq r0, #4
	ldmeqia sp!, {r4, r5, r6, r7, pc}
	beq _02241E68
	ldr r0, [r4, #4]
	cmp r0, #5
	addeq sp, sp, #0x2c
	moveq r0, #7
	ldmeqia sp!, {r4, r5, r6, r7, pc}
_02241E68:
	cmp r4, #0
	beq _02241E8C
	ldr r0, _02241F6C ; =ov45_022577DC
	ldr r0, [r0]
	ldr r0, [r0, #4]
	cmp r0, #3
	addne sp, sp, #0x2c
	movne r0, #4
	ldmneia sp!, {r4, r5, r6, r7, pc}
_02241E8C:
	mov r0, #0x10
	bl ov45_02242394
	cmp r0, #0
	beq _02241EC8
	ldr r2, _02241F70 ; =ov45_02255238
	ldr r1, _02241F74 ; =ov45_022577E0
	str r2, [r0]
	mov r2, #0
	ldr ip, [r1]
	stmib r0, {r2, r5}
	ldr r2, _02241F78 ; =ov45_02255250
	add r3, ip, #1
	stmia r0, {r2, ip}
	str r3, [r1]
	str r6, [r0, #0xc]
_02241EC8:
	str r0, [sp, #8]
	add r0, sp, #0xc
	add r2, sp, #8
	add r1, r4, #0x90
	bl ov45_0224206C
	mov r1, #0
	ldr r0, [sp, #8]
	strb r1, [sp, #4]
	strb r1, [sp, #5]
	ldr r1, [r0]
	ldr r1, [r1, #8]
	blx r1
	mov r4, r0
	add r0, sp, #0x20
	mov r1, r7
	bl ov45_02237B58
	ldr r1, _02241F6C ; =ov45_022577DC
	add r0, sp, #0x14
	ldr r1, [r1]
	add r2, sp, #0x20
	bl ov45_022428C8
	ldr r0, [sp, #0x14]
	mov r2, #0
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r1, sp, #0x15
	ldrne r1, [sp, #0x1c]
	ldr r0, _02241F6C ; =ov45_022577DC
	str r2, [sp]
	ldr r0, [r0]
	ldr r2, _02241F7C ; =ov45_02240718
	ldr r0, [r0]
	mov r3, r4
	bl ov45_02250C80
	add r0, sp, #0x14
	bl ov45_02237BC8
	add r0, sp, #0x20
	bl ov45_02237BC8
	mov r0, #0
	add sp, sp, #0x2c
	ldmia sp!, {r4, r5, r6, r7, pc}
	.balign 4, 0
_02241F6C: .word ov45_022577DC
_02241F70: .word ov45_02255238
_02241F74: .word ov45_022577E0
_02241F78: .word ov45_02255250
_02241F7C: .word ov45_02240718
	arm_func_end ov45_02241E24
