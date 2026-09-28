	.include "asm/macros.inc"
	.include "overlay_45_arm.inc"
	.include "global.inc"
	.public ov45_02241048
	.extern ov45_02237B58
	.extern ov45_02237BC8
	.extern ov45_0223F1E4
	.extern ov45_0223F2D8
	.extern ov45_0223F32C
	.extern ov45_0223F5C8
	.extern ov45_0223F6F4
	.extern ov45_0223F768
	.extern ov45_0223F83C
	.extern ov45_0223F8F4
	.extern ov45_0223F910
	.extern ov45_02240234
	.extern ov45_02240620
	.extern ov45_0224206C
	.extern ov45_02242394
	.extern ov45_022428C8
	.extern ov45_02242ACC
	.extern ov45_02250688
	.extern ov45_02255238
	.extern ov45_02255280
	.extern ov45_022577DC
	.extern ov45_022577E0

.text
	arm_func_start ov45_02241048
ov45_02241048: ; 0x02241048
	stmdb sp!, {r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0x88
	ldr r4, _02241288 ; =ov45_022577DC
	mov r8, r0
	ldr r4, [r4]
	mov r7, r1
	cmp r4, #0
	mov r6, r2
	mov r5, r3
	addeq sp, sp, #0x88
	moveq r0, #4
	ldmeqia sp!, {r4, r5, r6, r7, r8, pc}
	beq _02241090
	ldr r0, [r4, #4]
	cmp r0, #5
	addeq sp, sp, #0x88
	moveq r0, #7
	ldmeqia sp!, {r4, r5, r6, r7, r8, pc}
_02241090:
	cmp r4, #0
	beq _022410B4
	ldr r0, _02241288 ; =ov45_022577DC
	ldr r0, [r0]
	ldr r0, [r0, #4]
	cmp r0, #3
	addne sp, sp, #0x88
	movne r0, #4
	ldmneia sp!, {r4, r5, r6, r7, r8, pc}
_022410B4:
	mov r0, r8
	bl strlen
	mov r0, #0x10
	bl ov45_02242394
	cmp r0, #0
	beq _02241108
	ldr r2, _0224128C ; =ov45_02255238
	ldr r1, _02241290 ; =ov45_022577E0
	str r2, [r0]
	mov ip, #0
	ldr r2, [r1]
	ldr r3, [sp, #0xa4]
	str ip, [r0, #4]
	str r3, [r0, #8]
	add ip, r2, #1
	ldr r3, _02241294 ; =ov45_02255280
	str r2, [r0, #4]
	ldr r2, [sp, #0xa0]
	str r3, [r0]
	str ip, [r1]
	str r2, [r0, #0xc]
_02241108:
	str r0, [sp, #0x10]
	add r0, sp, #0x14
	add r2, sp, #0x10
	add r1, r4, #0x90
	bl ov45_0224206C
	mov r1, #0
	ldr r0, [sp, #0x10]
	strb r1, [sp, #0xc]
	strb r1, [sp, #0xd]
	ldr r1, [r0]
	ldr r1, [r1, #8]
	blx r1
	mov r4, r0
	add r0, sp, #0x4c
	mov r1, r8
	bl ov45_02237B58
	ldr r1, _02241288 ; =ov45_022577DC
	add r0, sp, #0x40
	ldr r1, [r1]
	add r2, sp, #0x4c
	bl ov45_022428C8
	ldr r0, [sp, #0x40]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r1, sp, #0x41
	ldrne r1, [sp, #0x48]
	add r0, sp, #0x34
	bl ov45_02237B58
	ldr r0, _02241288 ; =ov45_022577DC
	add r1, sp, #0x34
	ldr r0, [r0]
	mov r2, r5
	mov r3, r6
	bl ov45_02242ACC
	add r0, sp, #0x34
	bl ov45_02237BC8
	add r0, sp, #0x40
	bl ov45_02237BC8
	add r0, sp, #0x4c
	bl ov45_02237BC8
	add r0, sp, #0x58
	mov r1, #0
	mov r2, #0x30
	bl MI_CpuFill8
	ldr r0, _02241298 ; =ov45_0223F8F4
	ldr r2, _0224129C ; =ov45_0223F1E4
	str r0, [sp, #0x70]
	ldr r0, _022412A0 ; =ov45_0223F83C
	str r2, [sp, #0x58]
	ldr r2, _022412A4 ; =ov45_0223F2D8
	str r0, [sp, #0x6c]
	ldr r0, _022412A8 ; =ov45_0223F32C
	str r2, [sp, #0x5c]
	ldr r2, _022412AC ; =ov45_0223F5C8
	str r0, [sp, #0x60]
	ldr r0, _022412B0 ; =ov45_0223F768
	str r2, [sp, #0x64]
	ldr r2, _022412B4 ; =ov45_0223F910
	str r0, [sp, #0x74]
	ldr r0, _022412B8 ; =ov45_02240620
	str r2, [sp, #0x78]
	ldr r2, _022412BC ; =ov45_0223F6F4
	str r0, [sp, #0x80]
	str r2, [sp, #0x68]
	ldr r2, [r5, #0x14]
	mov r1, r8
	add r0, sp, #0x28
	str r2, [sp, #0x84]
	bl ov45_02237B58
	ldr r1, _02241288 ; =ov45_022577DC
	add r0, sp, #0x1c
	ldr r1, [r1]
	add r2, sp, #0x28
	bl ov45_022428C8
	ldr r0, [sp, #0x1c]
	mov r2, #0
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r1, sp, #0x1d
	ldrne r1, [sp, #0x24]
	ldr r0, _022412C0 ; =ov45_02240234
	add r3, sp, #0x58
	stmia sp, {r0, r4}
	str r2, [sp, #8]
	ldr r0, _02241288 ; =ov45_022577DC
	mov r2, r7
	ldr r0, [r0]
	ldr r0, [r0]
	bl ov45_02250688
	add r0, sp, #0x1c
	bl ov45_02237BC8
	add r0, sp, #0x28
	bl ov45_02237BC8
	mov r0, #0
	add sp, sp, #0x88
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	.balign 4, 0
_02241288: .word ov45_022577DC
_0224128C: .word ov45_02255238
_02241290: .word ov45_022577E0
_02241294: .word ov45_02255280
_02241298: .word ov45_0223F8F4
_0224129C: .word ov45_0223F1E4
_022412A0: .word ov45_0223F83C
_022412A4: .word ov45_0223F2D8
_022412A8: .word ov45_0223F32C
_022412AC: .word ov45_0223F5C8
_022412B0: .word ov45_0223F768
_022412B4: .word ov45_0223F910
_022412B8: .word ov45_02240620
_022412BC: .word ov45_0223F6F4
_022412C0: .word ov45_02240234
	arm_func_end ov45_02241048
