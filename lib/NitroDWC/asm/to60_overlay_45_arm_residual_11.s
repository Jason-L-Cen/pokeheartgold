	.include "asm/macros.inc"
	.include "overlay_45_arm.inc"
	.include "global.inc"
	.public ov45_022417B4
	.public ov45_02241A20
	.public ov45_02241B84
	.extern ov45_02237B58
	.extern ov45_02237BC8
	.extern ov45_02237D48
	.extern ov45_02237FD8
	.extern ov45_0223DAD4
	.extern ov45_0223FC6C
	.extern ov45_0223FE20
	.extern ov45_0223FE28
	.extern ov45_02240494
	.extern ov45_0224206C
	.extern ov45_02242394
	.extern ov45_022428C8
	.extern ov45_0224367C
	.extern ov45_022505A0
	.extern ov45_02251024
	.extern ov45_02251208
	.extern ov45_022518F0
	.extern ov45_02255238
	.extern ov45_02255268
	.extern ov45_02255298
	.extern ov45_022552B0
	.extern ov45_02255304
	.extern ov45_022577DC
	.extern ov45_022577E0

.text
	arm_func_start ov45_022417B4
ov45_022417B4: ; 0x022417B4
	stmdb sp!, {r0, r1, r2, r3}
	stmdb sp!, {r4, r5, r6, lr}
	sub sp, sp, #0x50
	ldr r2, _02241A08 ; =ov45_022577DC
	mov r6, r0
	ldr r2, [r2]
	mov r4, r1
	cmp r2, #0
	mov r5, r3
	addeq sp, sp, #0x50
	moveq r0, #4
	ldmeqia sp!, {r4, r5, r6, lr}
	addeq sp, sp, #0x10
	bxeq lr
	beq _0224180C
	ldr r0, [r2, #4]
	cmp r0, #5
	addeq sp, sp, #0x50
	moveq r0, #7
	ldmeqia sp!, {r4, r5, r6, lr}
	addeq sp, sp, #0x10
	bxeq lr
_0224180C:
	cmp r2, #0
	beq _02241838
	ldr r0, _02241A08 ; =ov45_022577DC
	ldr r0, [r0]
	ldr r0, [r0, #4]
	cmp r0, #3
	addne sp, sp, #0x50
	movne r0, #4
	ldmneia sp!, {r4, r5, r6, lr}
	addne sp, sp, #0x10
	bxne lr
_02241838:
	ldr r0, [sp, #0x68]
	bl strlen
	add r0, r0, #1
	cmp r0, #0x14
	addhi sp, sp, #0x50
	movhi r0, #3
	ldmhiia sp!, {r4, r5, r6, lr}
	addhi sp, sp, #0x10
	bxhi lr
	ldr r1, _02241A0C ; =ov45_02255304
	add r0, sp, #0x44
	bl ov45_02237B58
	mvn r0, #0
	cmp r4, r0
	bne _02241884
	ldr r1, _02241A0C ; =ov45_02255304
	add r0, sp, #0x44
	bl ov45_0223DAD4
	b _022418E8
_02241884:
	ldr r1, _02241A08 ; =ov45_022577DC
	add r0, sp, #0x38
	ldr r1, [r1]
	mov r2, r4
	bl ov45_0224367C
	add r0, sp, #0x44
	add r1, sp, #0x38
	bl ov45_02237D48
	add r0, sp, #0x38
	bl ov45_02237BC8
	ldr r1, _02241A0C ; =ov45_02255304
	add r0, sp, #0x44
	bl ov45_02237FD8
	cmp r0, #0
	moveq r0, #1
	movne r0, #0
	cmp r0, #0
	beq _022418E8
	add r0, sp, #0x44
	bl ov45_02237BC8
	add sp, sp, #0x50
	mov r0, #3
	ldmia sp!, {r4, r5, r6, lr}
	add sp, sp, #0x10
	bx lr
_022418E8:
	ldr r1, _02241A08 ; =ov45_022577DC
	mov r0, #0x10
	ldr r4, [r1]
	bl ov45_02242394
	cmp r0, #0
	beq _02241934
	ldr r2, _02241A10 ; =ov45_02255238
	ldr r1, _02241A14 ; =ov45_022577E0
	str r2, [r0]
	mov r3, #0
	ldr r2, [sp, #0x70]
	str r3, [r0, #4]
	ldr ip, [r1]
	str r2, [r0, #8]
	ldr r2, _02241A18 ; =ov45_02255268
	add r3, ip, #1
	stmia r0, {r2, ip}
	str r3, [r1]
	str r5, [r0, #0xc]
_02241934:
	str r0, [sp, #0x14]
	add r0, sp, #0x18
	add r2, sp, #0x14
	add r1, r4, #0x90
	bl ov45_0224206C
	mov r1, #0
	ldr r0, [sp, #0x14]
	strb r1, [sp, #0x10]
	strb r1, [sp, #0x11]
	ldr r1, [r0]
	ldr r1, [r1, #8]
	blx r1
	mov r4, r0
	add r0, sp, #0x2c
	mov r1, r6
	bl ov45_02237B58
	ldr r1, _02241A08 ; =ov45_022577DC
	add r0, sp, #0x20
	ldr r1, [r1]
	add r2, sp, #0x2c
	bl ov45_022428C8
	ldr r0, [sp, #0x44]
	add r3, sp, #0x68
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	ldr r0, [sp, #0x20]
	addeq r2, sp, #0x45
	mov r0, r0, lsl #0x1f
	ldrne r2, [sp, #0x4c]
	movs r0, r0, lsr #0x1f
	addeq r1, sp, #0x21
	ldrne r1, [sp, #0x28]
	ldr r0, _02241A1C ; =ov45_02240494
	str r3, [sp]
	stmib sp, {r0, r4}
	mov r3, #0
	str r3, [sp, #0xc]
	ldr r0, _02241A08 ; =ov45_022577DC
	mov r3, #1
	ldr r0, [r0]
	ldr r0, [r0]
	bl ov45_022518F0
	add r0, sp, #0x20
	bl ov45_02237BC8
	add r0, sp, #0x2c
	bl ov45_02237BC8
	add r0, sp, #0x44
	bl ov45_02237BC8
	mov r0, #0
	add sp, sp, #0x50
	ldmia sp!, {r4, r5, r6, lr}
	add sp, sp, #0x10
	bx lr
	.balign 4, 0
_02241A08: .word ov45_022577DC
_02241A0C: .word ov45_02255304
_02241A10: .word ov45_02255238
_02241A14: .word ov45_022577E0
_02241A18: .word ov45_02255268
_02241A1C: .word ov45_02240494
	arm_func_end ov45_022417B4

	arm_func_start ov45_02241A20
ov45_02241A20: ; 0x02241A20
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	sub sp, sp, #0x30
	ldr r3, _02241B6C ; =ov45_022577DC
	mov r7, r0
	ldr r4, [r3]
	mov r6, r1
	cmp r4, #0
	mov r5, r2
	addeq sp, sp, #0x30
	moveq r0, #4
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	beq _02241A64
	ldr r0, [r4, #4]
	cmp r0, #5
	addeq sp, sp, #0x30
	moveq r0, #7
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
_02241A64:
	cmp r4, #0
	beq _02241A88
	ldr r0, _02241B6C ; =ov45_022577DC
	ldr r0, [r0]
	ldr r0, [r0, #4]
	cmp r0, #3
	addne sp, sp, #0x30
	movne r0, #4
	ldmneia sp!, {r3, r4, r5, r6, r7, pc}
_02241A88:
	mov r0, #0x10
	bl ov45_02242394
	cmp r0, #0
	beq _02241AC4
	ldr r2, _02241B70 ; =ov45_02255238
	ldr r1, _02241B74 ; =ov45_022577E0
	str r2, [r0]
	mov r2, #0
	ldr ip, [r1]
	stmib r0, {r2, r5}
	ldr r2, _02241B78 ; =ov45_022552B0
	add r3, ip, #1
	stmia r0, {r2, ip}
	str r3, [r1]
	str r6, [r0, #0xc]
_02241AC4:
	str r0, [sp, #0xc]
	add r0, sp, #0x10
	add r2, sp, #0xc
	add r1, r4, #0x90
	bl ov45_0224206C
	mov r1, #0
	ldr r0, [sp, #0xc]
	strb r1, [sp, #8]
	strb r1, [sp, #9]
	ldr r1, [r0]
	ldr r1, [r1, #8]
	blx r1
	mov r4, r0
	add r0, sp, #0x24
	mov r1, r7
	bl ov45_02237B58
	ldr r1, _02241B6C ; =ov45_022577DC
	add r0, sp, #0x18
	ldr r1, [r1]
	add r2, sp, #0x24
	bl ov45_022428C8
	ldr r0, [sp, #0x18]
	mov r2, #0
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r1, sp, #0x19
	ldrne r1, [sp, #0x20]
	ldr r0, _02241B6C ; =ov45_022577DC
	str r4, [sp]
	str r2, [sp, #4]
	ldr r0, [r0]
	ldr r2, _02241B7C ; =ov45_0223FE20
	ldr r0, [r0]
	ldr r3, _02241B80 ; =ov45_0223FC6C
	bl ov45_022505A0
	add r0, sp, #0x18
	bl ov45_02237BC8
	add r0, sp, #0x24
	bl ov45_02237BC8
	mov r0, #0
	add sp, sp, #0x30
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.balign 4, 0
_02241B6C: .word ov45_022577DC
_02241B70: .word ov45_02255238
_02241B74: .word ov45_022577E0
_02241B78: .word ov45_022552B0
_02241B7C: .word ov45_0223FE20
_02241B80: .word ov45_0223FC6C
	arm_func_end ov45_02241A20

	arm_func_start ov45_02241B84
ov45_02241B84: ; 0x02241B84
	stmdb sp!, {r4, r5, r6, r7, lr}
	sub sp, sp, #0x44
	ldr r3, _02241D44 ; =ov45_022577DC
	mov r7, r0
	ldr r3, [r3]
	mov r6, r1
	cmp r3, #0
	mov r5, r2
	addeq sp, sp, #0x44
	moveq r0, #4
	ldmeqia sp!, {r4, r5, r6, r7, pc}
	beq _02241BC8
	ldr r0, [r3, #4]
	cmp r0, #5
	addeq sp, sp, #0x44
	moveq r0, #7
	ldmeqia sp!, {r4, r5, r6, r7, pc}
_02241BC8:
	cmp r3, #0
	beq _02241BEC
	ldr r0, _02241D44 ; =ov45_022577DC
	ldr r0, [r0]
	ldr r0, [r0, #4]
	cmp r0, #3
	addne sp, sp, #0x44
	movne r0, #4
	ldmneia sp!, {r4, r5, r6, r7, pc}
_02241BEC:
	add r0, sp, #0x38
	mov r1, r7
	bl ov45_02237B58
	ldr r1, _02241D44 ; =ov45_022577DC
	add r0, sp, #0x2c
	ldr r1, [r1]
	add r2, sp, #0x38
	bl ov45_022428C8
	ldr r0, [sp, #0x2c]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	ldr r0, _02241D44 ; =ov45_022577DC
	addeq r1, sp, #0x2d
	ldr r0, [r0]
	ldrne r1, [sp, #0x34]
	ldr r0, [r0]
	bl ov45_02251024
	cmp r0, #0
	moveq r4, #1
	add r0, sp, #0x2c
	movne r4, #0
	bl ov45_02237BC8
	add r0, sp, #0x38
	bl ov45_02237BC8
	cmp r4, #0
	addne sp, sp, #0x44
	movne r0, #5
	ldmneia sp!, {r4, r5, r6, r7, pc}
	ldr r1, _02241D44 ; =ov45_022577DC
	mov r0, #0x10
	ldr r4, [r1]
	bl ov45_02242394
	cmp r0, #0
	beq _02241CA0
	ldr r2, _02241D48 ; =ov45_02255238
	ldr r1, _02241D4C ; =ov45_022577E0
	str r2, [r0]
	mov r2, #0
	ldr ip, [r1]
	stmib r0, {r2, r5}
	ldr r2, _02241D50 ; =ov45_02255298
	add r3, ip, #1
	stmia r0, {r2, ip}
	str r3, [r1]
	str r6, [r0, #0xc]
_02241CA0:
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
	ldr r1, _02241D44 ; =ov45_022577DC
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
	ldr r0, _02241D44 ; =ov45_022577DC
	str r2, [sp]
	ldr r0, [r0]
	ldr r2, _02241D54 ; =ov45_0223FE28
	ldr r0, [r0]
	mov r3, r4
	bl ov45_02251208
	add r0, sp, #0x14
	bl ov45_02237BC8
	add r0, sp, #0x20
	bl ov45_02237BC8
	mov r0, #0
	add sp, sp, #0x44
	ldmia sp!, {r4, r5, r6, r7, pc}
	.balign 4, 0
_02241D44: .word ov45_022577DC
_02241D48: .word ov45_02255238
_02241D4C: .word ov45_022577E0
_02241D50: .word ov45_02255298
_02241D54: .word ov45_0223FE28
	arm_func_end ov45_02241B84
