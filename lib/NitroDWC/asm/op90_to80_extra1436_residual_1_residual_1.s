	.extern _02254D04
	.extern _02254F58
	.extern _022577C4
	.extern ov45_02231C78
	.extern ov45_02231EB0
	.extern ov45_022320B4
	.extern ov45_02232434
	.extern ov45_022325B0
	.extern ov45_02232760
	.extern ov45_022327B8
	.extern ov45_02232AE8
	.extern ov45_02232B9C
	.extern ov45_02234A24
	.extern ov45_02236070
	.extern ov45_02236090
	.extern ov45_022362B4
	.extern ov45_022362D0
	.extern ov45_02236308
	.extern ov45_0223633C
	.extern ov45_02236690
	.extern ov45_022367E0
	.extern ov45_0223683C
	.extern ov45_0223694C
	.extern ov45_02236A44
	.extern ov45_02236AE8
	.extern ov45_02236B88
	.extern ov45_02236BA8
	.extern ov45_02236C40
	.extern ov45_02236CB0
	.extern ov45_02236CE0
	.extern ov45_02236E38
	.extern ov45_02236E58
	.extern ov45_02237634
	.extern ov45_02237664
	.extern ov45_0223770C
	.extern ov45_0223775C
	.extern ov45_022377AC
	.extern ov45_02237820
	.extern ov45_02237890
	.extern ov45_0223795C
	.extern ov45_022379D4
	.extern ov45_02237A24
	.extern ov45_02237AA0
	.extern ov45_02237B58
	.extern ov45_02237BC8
	.extern ov45_02237BFC
	.extern ov45_02237D48
	.extern ov45_02237EB0
	.extern ov45_02237F7C
	.extern ov45_02237FD8
	.extern ov45_02238104
	.extern ov45_022383E0
	.extern ov45_02238450
	.extern ov45_02238508
	.extern ov45_02238558
	.extern ov45_02238B08
	.extern ov45_02238B70
	.extern ov45_02238C1C
	.extern ov45_02238C74
	.extern ov45_0223AC78
	.extern ov45_0223ACA4
	.extern ov45_0223AE84
	.extern ov45_0223AEDC
	.extern ov45_0223AF04
	.extern ov45_0223AF28
	.extern ov45_0223B1C8
	.extern ov45_0223CAB4
	.extern ov45_0223CB04
	.extern ov45_0223CB6C
	.extern ov45_0223CC78
	.extern ov45_0223CD78
	.extern ov45_0223CDBC
	.extern ov45_0223CE30
	.extern ov45_0223CE60
	.extern ov45_0223CE90
	.extern ov45_0223D27C
	.extern ov45_0223D7D4
	.extern ov45_0223DD3C
	.extern ov45_0223DE38
	.extern ov45_0223DEC4
	.extern ov45_0223DF4C
	.extern ov45_0223DFC8
	.extern ov45_0223E100
	.extern ov45_0223E4A8
	.extern ov45_0223E558
	.extern ov45_02240874
	.extern ov45_02240A58
	.extern ov45_02240A74
	.extern ov45_02240A80
	.extern ov45_02240B10
	.extern ov45_02240F9C
	.extern ov45_02241048
	.extern ov45_022412CC
	.extern ov45_022413D4
	.extern ov45_022414C0
	.extern ov45_022415CC
	.extern ov45_022417B4
	.extern ov45_02241A20
	.extern ov45_02241B84
	.extern ov45_02241D58
	.extern ov45_02241E24
	.extern ov45_02242394
	.extern ov45_0225443C
	.extern ov45_022545F0
	.extern ov45_022546F8
	.extern ov45_02254774
	.extern ov45_022549E0
	.extern ov45_02254A04
	.extern ov45_02254D1C
	.extern ov45_02254D3C
	.extern ov45_02254D5C
	.extern ov45_02254D7C
	.extern ov45_02254D9C
	.extern ov45_02254F88
	.extern ov45_02254FA0
	.extern ov45_02254FB0
	.extern ov45_02254FC0
	.extern ov45_022550B0
	.extern ov45_022550B4
	.extern ov45_022550B8
	.extern ov45_022550BC
	.extern ov45_022550C4
	.extern ov45_022550D4
	.extern ov45_022550E4
	.extern ov45_022550F4
	.extern ov45_02255104
	.extern ov45_02255120
	.extern ov45_02255144
	.extern ov45_02255160
	.extern ov45_0225517C
	.extern ov45_02255198
	.extern ov45_022551A4
	.extern ov45_022551A8
	.extern ov45_022551AC
	.extern ov45_022551C4
	.extern ov45_022577C8
	.extern ov45_022577D0
	.extern ov45_022577D4
	.extern ov45_022577D8
	.include "asm/macros.inc"
	.include "overlay_45_arm.inc"
	.include "global.inc"
	.extern ov45_022353F0
	.extern ov45_022350FC

.text
	arm_func_start ov45_02234A88
ov45_02234A88: ; 0x02234A88
	stmdb sp!, {r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0x38
	ldr r3, _02234D54 ; =_022577C4
	ldr r3, [r3]
	cmp r3, #0
	ldrne r3, [r3]
	cmpne r3, #9
	addeq sp, sp, #0x38
	ldmeqia sp!, {r4, r5, r6, r7, r8, pc}
	mov r5, #0
	add r4, sp, #0x1c
	add r3, sp, #0x20
	str r5, [sp, #0x20]
	str r5, [sp, #0x1c]
	str r4, [sp]
	bl ov45_02234A24
	movs r4, r0
	bne _02234ADC
	ldr r0, [sp, #0x1c]
	cmp r0, #0x94
	movne r4, #2
_02234ADC:
	cmp r4, #0
	bne _02234AF4
	ldr r0, [sp, #0x20]
	ldr r1, [sp, #0x1c]
	mov r2, #0
	bl ov45_022325B0
_02234AF4:
	ldr r0, _02234D54 ; =_022577C4
	ldr r1, [sp, #0x20]
	ldr r0, [r0]
	ldr r2, [sp, #0x1c]
	ldr r3, [r0, #0x34]
	mov r0, r4
	blx r3
	cmp r4, #0
	beq _02234B34
	ldr r0, _02234D54 ; =_022577C4
	mov r1, #6
	ldr r2, [r0]
	mov r0, #0xa
	stmia r2, {r0, r1}
	add sp, sp, #0x38
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_02234B34:
	ldr r0, _02234D54 ; =_022577C4
	ldr r0, [r0]
	add r0, r0, #0x110
	bl ov45_02231C78
	ldr r1, _02234D54 ; =_022577C4
	mov r2, #1
	ldr r1, [r1]
	ldr r1, [r1, #0x4c]
	strh r2, [sp, #0x28]
	str r1, [sp, #0x24]
	ldrb r1, [r0, #0x42]
	strb r1, [sp, #0x2a]
	ldrb r1, [r0, #0x39]
	add r0, sp, #0x2c
	strb r1, [sp, #0x2b]
	bl OS_GetMacAddress
	ldr r0, _02234D58 ; =_02254F58
	ldr r1, _02234D54 ; =_022577C4
	ldr r5, [r0, #0x10]
	ldr r1, [r1]
	ldr r4, [r0, #0xc]
	ldr r0, [r1, #0x50]
	ldr r7, [r1, #0x4c]
	cmp r0, #0
	ldr r0, _02234D5C ; =ov45_022577D4
	moveq r8, #0
	ldr r0, [r0]
	movne r8, #1
	cmp r0, #0
	addne sp, sp, #0x38
	ldmneia sp!, {r4, r5, r6, r7, r8, pc}
	mov r0, #0x40
	bl ov45_02242394
	movs r6, r0
	beq _02234C58
	ldr r1, _02234D60 ; =ov45_02254FC0
	mov r0, #3
	str r1, [r6]
	str r0, [r6, #4]
	mov r1, r4
	add r0, r6, #0xc
	str r8, [r6, #8]
	bl ov45_02237B58
	mov r1, r5
	add r0, r6, #0x18
	bl ov45_02237B58
	add r2, sp, #0x24
	add r4, r2, #0x14
	mov r1, #0
	str r7, [r6, #0x24]
	str r1, [r6, #0x28]
	str r1, [r6, #0x2c]
	ldr r0, _02234D64 ; =ov45_02234D68
	str r1, [r6, #0x30]
	str r0, [r6, #0x34]
	str r1, [r6, #0x38]
	mov r0, #1
	str r0, [r6, #0x3c]
	strb r1, [sp, #9]
	strb r1, [sp, #0xa]
	strb r1, [sp, #8]
	strb r1, [sp, #4]
	sub r1, sp, #4
	ldr r0, [sp, #0x18]
	str r4, [r1]
	str r0, [r1, #4]
	ldr r3, [r1]
	add r1, sp, #0xc
	str r2, [sp, #0xc]
	add r0, r6, #0x28
	str r4, [sp, #0x14]
	ldmia r1, {r1, r2}
	bl ov45_022327B8
_02234C58:
	ldr r0, _02234D5C ; =ov45_022577D4
	str r6, [r0]
	ldr r0, [r6, #0xc]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r1, r6, #0xd
	ldrne r1, [r6, #0x14]
	ldr r0, [r6, #8]
	bl ov45_0225443C
	ldr r0, [r6, #0x3c]
	ldr r4, [r6, #0x2c]
	cmp r0, #0
	ldr r0, [r6, #0x18]
	beq _02234CC8
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r5, r6, #0x19
	ldrne r5, [r6, #0x20]
	add r0, r6, #0x28
	bl ov45_02231C78
	mov r1, #0
	str r1, [sp]
	mov r2, r0
	ldr r1, [r6, #0x24]
	mov r0, r5
	mov r3, r4
	bl ov45_02254A04
	b _02234CFC
_02234CC8:
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r5, r6, #0x19
	ldrne r5, [r6, #0x20]
	add r0, r6, #0x28
	bl ov45_02231C78
	mov r1, #0
	str r1, [sp]
	mov r2, r0
	ldr r1, [r6, #0x24]
	mov r0, r5
	mov r3, r4
	bl ov45_022549E0
_02234CFC:
	cmp r0, #0
	moveq r0, #1
	beq _02234D10
	bl ov45_022546F8
	mov r0, #0
_02234D10:
	cmp r0, #0
	addne sp, sp, #0x38
	ldmneia sp!, {r4, r5, r6, r7, r8, pc}
	ldr r0, _02234D5C ; =ov45_022577D4
	ldr r0, [r0]
	cmp r0, #0
	addeq sp, sp, #0x38
	ldmeqia sp!, {r4, r5, r6, r7, r8, pc}
	beq _02234D40
	ldr r1, [r0]
	ldr r1, [r1, #4]
	blx r1
_02234D40:
	ldr r0, _02234D5C ; =ov45_022577D4
	mov r1, #0
	str r1, [r0]
	add sp, sp, #0x38
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	.balign 4, 0
_02234D54: .word _022577C4
_02234D58: .word _02254F58
_02234D5C: .word ov45_022577D4
_02234D60: .word ov45_02254FC0
_02234D64: .word ov45_02234D68
	arm_func_end ov45_02234A88

	arm_func_start ov45_02234D68
ov45_02234D68: ; 0x02234D68
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, lr}
	sub sp, sp, #0x50
	ldr r3, _022350E8 ; =_022577C4
	ldr r3, [r3]
	cmp r3, #0
	ldrne r3, [r3]
	cmpne r3, #9
	addeq sp, sp, #0x50
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
	mov r5, #0
	add r4, sp, #0x1c
	add r3, sp, #0x20
	str r5, [sp, #0x20]
	str r5, [sp, #0x1c]
	str r4, [sp]
	bl ov45_02234A24
	movs r4, r0
	bne _02234DD0
	ldr r0, _022350E8 ; =_022577C4
	ldr r1, [sp, #0x20]
	ldr r0, [r0]
	ldr r2, [sp, #0x1c]
	add r0, r0, #0x144
	bl ov45_0223AC78
	cmp r0, #0
	moveq r4, #2
_02234DD0:
	ldr r0, _022350E8 ; =_022577C4
	ldr r1, [sp, #0x20]
	ldr r2, [r0]
	mov r0, r4
	ldr r2, [r2, #0x38]
	blx r2
	cmp r4, #0
	beq _02234E0C
	ldr r0, _022350E8 ; =_022577C4
	mov r1, #4
	ldr r2, [r0]
	mov r0, #0xa
	stmia r2, {r0, r1}
	add sp, sp, #0x50
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
_02234E0C:
	cmp r0, #0
	beq _02234E2C
	ldr r0, _022350E8 ; =_022577C4
	ldr r1, [sp, #0x20]
	ldr r0, [r0]
	ldr r2, [sp, #0x1c]
	add r0, r0, #0x144
	bl ov45_02238C74
_02234E2C:
	ldr r0, _022350E8 ; =_022577C4
	ldr r0, [r0]
	add r0, r0, #0x110
	bl ov45_02231C78
	ldr r1, _022350E8 ; =_022577C4
	add r2, sp, #0x38
	mov r3, #0
	ldr r1, [r1]
	mov r4, r0
	add r0, r1, #0x110
	str r3, [r2]
	str r3, [r2, #4]
	str r3, [r2, #8]
	str r3, [r2, #0xc]
	str r3, [r2, #0x10]
	str r3, [r2, #0x14]
	bl ov45_02231C78
	ldr r1, _022350E8 ; =_022577C4
	mov r2, #1
	ldr r1, [r1]
	ldr r1, [r1, #0x4c]
	strh r2, [sp, #0x28]
	str r1, [sp, #0x24]
	ldrb r1, [r0, #0x42]
	strb r1, [sp, #0x2a]
	ldrb r1, [r0, #0x39]
	add r0, sp, #0x2c
	strb r1, [sp, #0x2b]
	bl OS_GetMacAddress
	ldrh r1, [sp, #0x28]
	ldrb r0, [sp, #0x2a]
	ldrb sb, [sp, #0x2b]
	ldrb lr, [sp, #0x2c]
	ldrb ip, [sp, #0x2d]
	ldrb r8, [sp, #0x2e]
	ldrb r7, [sp, #0x2f]
	ldrb r6, [sp, #0x30]
	ldrb r5, [sp, #0x31]
	ldrh r3, [sp, #0x32]
	ldr sl, [sp, #0x24]
	ldr r2, [sp, #0x34]
	strh r1, [sp, #0x3c]
	strb r0, [sp, #0x3e]
	str sl, [sp, #0x38]
	strb sb, [sp, #0x3f]
	strb lr, [sp, #0x40]
	strb ip, [sp, #0x41]
	strb r8, [sp, #0x42]
	strb r7, [sp, #0x43]
	strb r6, [sp, #0x44]
	strb r5, [sp, #0x45]
	ldr r0, _022350EC ; =_02254F58
	str r2, [sp, #0x48]
	ldr r1, _022350E8 ; =_022577C4
	strh r3, [sp, #0x46]
	ldr r3, [r4, #4]
	ldr r2, [r1]
	str r3, [sp, #0x4c]
	ldr r1, [r2, #0x50]
	ldr r5, [r0, #0x14]
	ldr r4, [r0, #0xc]
	ldr r0, _022350F0 ; =ov45_022577D4
	cmp r1, #0
	moveq r8, #0
	ldr r0, [r0]
	movne r8, #1
	cmp r0, #0
	ldr r7, [r2, #0x4c]
	addne sp, sp, #0x50
	ldmneia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
	mov r0, #0x40
	bl ov45_02242394
	movs r6, r0
	beq _02234FEC
	ldr r1, _022350F4 ; =ov45_02254FC0
	mov r0, #3
	str r1, [r6]
	str r0, [r6, #4]
	mov r1, r4
	add r0, r6, #0xc
	str r8, [r6, #8]
	bl ov45_02237B58
	mov r1, r5
	add r0, r6, #0x18
	bl ov45_02237B58
	add r2, sp, #0x38
	add r4, r2, #0x18
	mov r1, #0
	str r7, [r6, #0x24]
	str r1, [r6, #0x28]
	str r1, [r6, #0x2c]
	ldr r0, _022350F8 ; =ov45_022350FC
	str r1, [r6, #0x30]
	str r0, [r6, #0x34]
	str r1, [r6, #0x38]
	mov r0, #1
	str r0, [r6, #0x3c]
	strb r1, [sp, #9]
	strb r1, [sp, #0xa]
	strb r1, [sp, #8]
	strb r1, [sp, #4]
	sub r1, sp, #4
	ldr r0, [sp, #0x18]
	str r4, [r1]
	str r0, [r1, #4]
	ldr r3, [r1]
	add r1, sp, #0xc
	str r2, [sp, #0xc]
	add r0, r6, #0x28
	str r4, [sp, #0x14]
	ldmia r1, {r1, r2}
	bl ov45_022327B8
_02234FEC:
	ldr r0, _022350F0 ; =ov45_022577D4
	str r6, [r0]
	ldr r0, [r6, #0xc]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r1, r6, #0xd
	ldrne r1, [r6, #0x14]
	ldr r0, [r6, #8]
	bl ov45_0225443C
	ldr r0, [r6, #0x3c]
	ldr r4, [r6, #0x2c]
	cmp r0, #0
	ldr r0, [r6, #0x18]
	beq _0223505C
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r5, r6, #0x19
	ldrne r5, [r6, #0x20]
	add r0, r6, #0x28
	bl ov45_02231C78
	mov r1, #0
	str r1, [sp]
	mov r2, r0
	ldr r1, [r6, #0x24]
	mov r0, r5
	mov r3, r4
	bl ov45_02254A04
	b _02235090
_0223505C:
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r5, r6, #0x19
	ldrne r5, [r6, #0x20]
	add r0, r6, #0x28
	bl ov45_02231C78
	mov r1, #0
	str r1, [sp]
	mov r2, r0
	ldr r1, [r6, #0x24]
	mov r0, r5
	mov r3, r4
	bl ov45_022549E0
_02235090:
	cmp r0, #0
	moveq r0, #1
	beq _022350A4
	bl ov45_022546F8
	mov r0, #0
_022350A4:
	cmp r0, #0
	addne sp, sp, #0x50
	ldmneia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
	ldr r0, _022350F0 ; =ov45_022577D4
	ldr r0, [r0]
	cmp r0, #0
	addeq sp, sp, #0x50
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
	beq _022350D4
	ldr r1, [r0]
	ldr r1, [r1, #4]
	blx r1
_022350D4:
	ldr r0, _022350F0 ; =ov45_022577D4
	mov r1, #0
	str r1, [r0]
	add sp, sp, #0x50
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
	.balign 4, 0
_022350E8: .word _022577C4
_022350EC: .word _02254F58
_022350F0: .word ov45_022577D4
_022350F4: .word ov45_02254FC0
_022350F8: .word ov45_022350FC
	arm_func_end ov45_02234D68
