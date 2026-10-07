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
	.extern ov45_02233434
	.extern ov45_02233994
	.extern ov45_022354F4
	.extern ov45_02235C40
	.extern ov45_02235E4C
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

.text
	arm_func_start ov45_02233CA0
ov45_02233CA0: ; 0x02233CA0
	stmdb sp!, {r3, r4, r5, r6, lr}
	sub sp, sp, #0x54
	ldr r0, _02233DF4 ; =_022577C4
	ldr r5, _02233DF8 ; =ov45_02235C40
	ldr lr, _02233DFC ; =ov45_02235E4C
	ldr ip, _02233E00 ; =ov45_02236090
	ldr r4, _02233E04 ; =ov45_022362D0
	ldr r3, _02233E08 ; =ov45_02236A44
	ldr r1, [r0]
	mov r2, #0
	add r0, sp, #0x24
	str r5, [sp, #0x3c]
	str lr, [sp, #0x40]
	str ip, [sp, #0x44]
	str r4, [sp, #0x48]
	str r3, [sp, #0x4c]
	str r2, [sp, #0x50]
	bl ov45_02231EB0
	add r0, sp, #0x30
	add r2, sp, #0x24
	mov r1, #0
	bl ov45_02233994
	add r0, sp, #0x24
	bl ov45_02237BC8
	ldr r0, [sp, #0x30]
	ldr r1, _02233E0C ; =ov45_022354F4
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r0, sp, #0x31
	ldrne r0, [sp, #0x38]
	add r3, sp, #0x3c
	str r1, [sp]
	mov r1, #0
	mov r2, #0x14
	str r1, [sp, #4]
	bl ov45_02241048
	mov r4, r0
	ldr r1, _02233DF4 ; =_022577C4
	mov r0, #0
	strb r0, [sp, #8]
	cmp r4, #1
	ldr r1, [r1]
	beq _02233D58
	cmp r4, #2
	moveq r0, #3
	b _02233D5C
_02233D58:
	mov r0, #2
_02233D5C:
	cmp r0, #0
	strne r0, [r1, #4]
	movne r0, #0xa
	strne r0, [r1]
	cmp r4, #0
	beq _02233D88
	add r0, sp, #0x30
	bl ov45_02237BC8
	add sp, sp, #0x54
	mov r0, r4
	ldmia sp!, {r3, r4, r5, r6, pc}
_02233D88:
	mov r2, #0
	add r0, sp, #0x10
	add r1, sp, #0x30
	str r2, [sp, #0xc]
	add r5, sp, #0xc
	bl ov45_02237A24
	mov r1, #0
	ldr r0, _02233DF4 ; =_022577C4
	ldr r2, [sp, #0xc]
	ldr r6, [r0]
	str r1, [r5, #0x10]
	str r1, [r5, #0x14]
	add r1, sp, #0x10
	str r2, [r6, #0xb8]
	add r0, r6, #0xbc
	bl ov45_02237D48
	ldr r1, [sp, #0x1c]
	add r0, sp, #0x10
	str r1, [r6, #0xc8]
	ldr r1, [sp, #0x20]
	str r1, [r6, #0xcc]
	bl ov45_02237BC8
	add r0, sp, #0x30
	bl ov45_02237BC8
	mov r0, r4
	add sp, sp, #0x54
	ldmia sp!, {r3, r4, r5, r6, pc}
	.balign 4, 0
_02233DF4: .word _022577C4
_02233DF8: .word ov45_02235C40
_02233DFC: .word ov45_02235E4C
_02233E00: .word ov45_02236090
_02233E04: .word ov45_022362D0
_02233E08: .word ov45_02236A44
_02233E0C: .word ov45_022354F4
	arm_func_end ov45_02233CA0

	arm_func_start ov45_02233E10
ov45_02233E10: ; 0x02233E10
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, lr}
	sub sp, sp, #0x40
	mov r8, r0
	mov r7, r1
	add r0, sp, #0x28
	mov r1, r2
	mov r6, r3
	bl ov45_02237B58
	ldr r1, _02233FE0 ; =_022577C4
	add r0, sp, #0x10
	ldr r5, [r1]
	add sb, sp, #0x28
	add r1, r5, #0xb8
	bl ov45_022320B4
	add r0, sp, #0x10
	mov r1, sb
	bl ov45_022340F8
	mov r4, r0
	add r0, sp, #0x10
	bl ov45_02237BC8
	cmp r4, #0
	ldrne r5, [r5, #0xb8]
	bne _02233E9C
	add r0, sp, #0x1c
	add r1, r5, #0xd0
	bl ov45_022320B4
	add r0, sp, #0x1c
	mov r1, sb
	bl ov45_022340F8
	mov r4, r0
	add r0, sp, #0x1c
	bl ov45_02237BC8
	cmp r4, #0
	ldrne r5, [r5, #0xd0]
	moveq r5, #5
_02233E9C:
	add r0, sp, #0x28
	bl ov45_02237BC8
	cmp r5, #5
	addeq sp, sp, #0x40
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	ldr r0, [sp, #0x64]
	mov r2, #0
	add r1, sp, #0x34
	str r2, [sp, #0x34]
	str r2, [sp, #0x38]
	str r2, [sp, #0x3c]
	bl ov45_02233FE4
	cmp r0, #0
	add r0, sp, #0x34
	bne _02233EE4
	bl ov45_022379D4
	add sp, sp, #0x40
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
_02233EE4:
	ldr sb, [sp, #0x38]
	cmp sb, #0
	bne _02233EFC
	bl ov45_022379D4
	add sp, sp, #0x40
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
_02233EFC:
	bl ov45_02231C78
	ldr r1, [sp, #0x60]
	ldr r4, [sp, #0x68]
	str r1, [sp]
	stmib sp, {r0, sb}
	mov r0, r8
	mov r1, r7
	mov r2, r5
	mov r3, r6
	str r4, [sp, #0xc]
	bl ov45_0223415C
	cmp r0, #0
	beq _02233F40
	add r0, sp, #0x34
	bl ov45_022379D4
	add sp, sp, #0x40
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
_02233F40:
	mvn r0, #0
	cmp r6, r0
	beq _02233F8C
	ldr r0, _02233FE0 ; =_022577C4
	mov r1, r6
	ldr r0, [r0]
	add r0, r0, #0xe8
	bl ov45_0223DEC4
	cmp r0, #0
	ldrne r0, [r0, #0x38]
	cmpne r0, #0
	movne r0, #1
	moveq r0, #0
	cmp r0, #0
	bne _02233F8C
	add r0, sp, #0x34
	bl ov45_022379D4
	add sp, sp, #0x40
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
_02233F8C:
	ldr r1, _02233FE0 ; =_022577C4
	add r0, sp, #0x34
	ldr r4, [r1]
	bl ov45_02231C78
	ldr r2, [sp, #0x60]
	ldr r1, [sp, #0x68]
	str r2, [sp]
	str r0, [sp, #4]
	ldr r2, [sp, #0x38]
	mov r0, r8
	str r2, [sp, #8]
	str r1, [sp, #0xc]
	ldr r4, [r4, #0x1c]
	mov r1, r7
	mov r2, r5
	mov r3, r6
	blx r4
	add r0, sp, #0x34
	bl ov45_022379D4
	add sp, sp, #0x40
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	.balign 4, 0
_02233FE0: .word _022577C4
	arm_func_end ov45_02233E10

	arm_func_start ov45_02233FE4
ov45_02233FE4: ; 0x02233FE4
	stmdb sp!, {r4, r5, r6, lr}
	sub sp, sp, #8
	mov r2, #0
	mov r5, r1
	strb r2, [sp, #4]
	mov r6, r0
	bl strlen
	ldr r3, [r5, #4]
	mov r4, r0
	cmp r4, r3
	bls _02234034
	mov r1, #0
	strb r1, [sp, #2]
	sub r0, sp, #4
	strb r1, [r0]
	ldr r2, [r0]
	mov r0, r5
	sub r1, r4, r3
	bl ov45_0223795C
	b _02234050
_02234034:
	bhs _02234050
	mov r0, #0
	strb r0, [sp, #3]
	ldr r1, [r5, #4]
	sub r0, r3, r4
	sub r0, r1, r0
	str r0, [r5, #4]
_02234050:
	cmp r4, #0
	addeq sp, sp, #8
	moveq r0, #1
	ldmeqia sp!, {r4, r5, r6, pc}
	mov r0, r6
	bl strlen
	mov r4, r0
	mov r0, r5
	bl ov45_02231C78
	mov r2, r0
	ldr r3, [r5, #4]
	mov r0, r6
	mov r1, r4
	bl ov00_021EFF3C
	mov r4, r0
	mvn r0, #0
	cmp r4, r0
	addeq sp, sp, #8
	moveq r0, #0
	ldmeqia sp!, {r4, r5, r6, pc}
	ldr r3, [r5, #4]
	cmp r4, r3
	bls _022340D0
	mov r1, #0
	strb r1, [sp]
	sub r0, sp, #4
	strb r1, [r0]
	ldr r2, [r0]
	mov r0, r5
	sub r1, r4, r3
	bl ov45_0223795C
	b _022340EC
_022340D0:
	bhs _022340EC
	mov r0, #0
	strb r0, [sp, #1]
	ldr r1, [r5, #4]
	sub r0, r3, r4
	sub r0, r1, r0
	str r0, [r5, #4]
_022340EC:
	mov r0, #1
	add sp, sp, #8
	ldmia sp!, {r4, r5, r6, pc}
	arm_func_end ov45_02233FE4

	arm_func_start ov45_022340F8
ov45_022340F8: ; 0x022340F8
	stmdb sp!, {r4, lr}
	ldr r2, [r1]
	mov r4, #0
	mov r2, r2, lsl #0x1f
	movs r2, r2, lsr #0x1f
	ldrne r3, [r1, #4]
	bne _02234120
	ldrb r2, [r1]
	mov r2, r2, lsl #0x18
	mov r3, r2, lsr #0x19
_02234120:
	ldr r2, [r0]
	mov r2, r2, lsl #0x1f
	movs r2, r2, lsr #0x1f
	ldrne r2, [r0, #4]
	bne _02234140
	ldrb r2, [r0]
	mov r2, r2, lsl #0x18
	mov r2, r2, lsr #0x19
_02234140:
	cmp r2, r3
	bne _02234154
	bl ov45_02237F7C
	cmp r0, #0
	moveq r4, #1
_02234154:
	mov r0, r4
	ldmia sp!, {r4, pc}
	arm_func_end ov45_022340F8

	arm_func_start ov45_0223415C
ov45_0223415C: ; 0x0223415C
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x70
	ldr r4, [sp, #0x98]
	cmp r2, #0
	ldr sb, [sp, #0x9c]
	ldr r8, [sp, #0xa0]
	mov sl, r3
	addne sp, sp, #0x70
	movne r0, #0
	ldmneia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldr r1, _02234538 ; =ov45_022550C4
	mov r0, r4
	bl strcmp
	cmp r0, #0
	bne _02234334
	ldr r0, _0223453C ; =_022577C4
	ldr r2, [r0]
	ldr r0, [r2, #0x4c]
	cmp sl, r0
	addeq sp, sp, #0x70
	moveq r0, #1
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	mov r1, sl
	add r0, r2, #0xe8
	bl ov45_0223DEC4
	movs r4, r0
	addeq sp, sp, #0x70
	moveq r0, #1
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	mov r0, #0
	str r0, [sp, #0x48]
	add r2, sb, r8
	str r0, [sp, #0x4c]
	str r0, [sp, #0x50]
	strb r0, [sp, #9]
	strb r0, [sp, #0xa]
	strb r0, [sp, #8]
	strb r0, [sp, #4]
	sub r1, sp, #4
	ldr r0, [sp, #0x24]
	str r2, [r1]
	str r0, [r1, #4]
	ldr r3, [r1]
	add r0, sp, #0x48
	add r1, sp, #0x18
	str sb, [sp, #0x18]
	str r2, [sp, #0x20]
	ldmia r1, {r1, r2}
	bl ov45_022327B8
	mov r2, #0
	ldrb r0, [sp, #0xc]
	ldr r1, [sp, #0x48]
	ldr r6, [sp, #0x4c]
	str r2, [sp, #0x54]
	str r2, [sp, #0x58]
	str r2, [sp, #0x5c]
	sub r2, sp, #4
	strb r0, [r2]
	add r5, sp, #0x54
	ldr r3, [r2]
	mov r0, r5
	add r2, r1, r6
	bl ov45_022383E0
	mov r0, #1
	str r0, [r5, #0xc]
	ldr r7, [r4, #0x1c]
	ldr r0, [sp, #0x58]
	mov r6, #0
	cmp r7, r0
	bne _022342AC
	cmp r7, #0
	beq _022342A8
	add r0, r4, #0x18
	bl ov45_02231C78
	mov fp, r0
	mov r0, r5
	bl ov45_02231C78
	mov r2, r7
	mov r1, r0
	mov r0, fp
	bl memcmp
	cmp r0, #0
	bne _022342AC
_022342A8:
	mov r6, #1
_022342AC:
	cmp r6, #0
	bne _022342E8
	add r0, r4, #0x18
	cmp r0, r5
	beq _022342E0
	mov r3, #0
	ldmia r5, {r1, r5}
	strb r3, [sp, #0xb]
	sub r2, sp, #4
	strb r3, [r2]
	ldr r3, [r2]
	add r2, r1, r5
	bl ov45_02232760
_022342E0:
	mov r0, #1
	str r0, [r4, #0x24]
_022342E8:
	add r0, sp, #0x54
	bl ov45_022379D4
	mov r1, #1
	add r0, sp, #0x48
	str r1, [r4, #0x34]
	bl ov45_022379D4
	ldr r0, [r4, #0x38]
	cmp r0, #0
	beq _02234328
	ldr r1, _0223453C ; =_022577C4
	mov r0, sl
	ldr r2, [r1]
	mov r1, sb
	ldr r3, [r2, #0x24]
	mov r2, r8
	blx r3
_02234328:
	add sp, sp, #0x70
	mov r0, #1
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_02234334:
	ldr r1, _02234540 ; =ov45_022550D4
	mov r0, r4
	bl strcmp
	cmp r0, #0
	bne _02234478
	ldr r0, _0223453C ; =_022577C4
	ldr r2, [r0]
	ldr r0, [r2, #0x4c]
	cmp sl, r0
	addeq sp, sp, #0x70
	moveq r0, #1
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	cmp r8, #0x10
	addne sp, sp, #0x70
	movne r0, #1
	ldmneia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	mov r1, sl
	add r0, r2, #0xe8
	bl ov45_0223DEC4
	movs r4, r0
	addeq sp, sp, #0x70
	moveq r0, #1
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldr r1, [sb]
	add r0, sp, #0x38
	str r1, [sp, #0x34]
	add r1, sb, #4
	ldmia r1, {r2, r3}
	stmia r0, {r2, r3}
	add r5, sp, #0x34
	ldr r6, [sb, #0xc]
	mov r3, #1
	mov r1, r5
	add r0, r4, #4
	mov r2, #0x10
	str r6, [sp, #0x40]
	str r3, [sp, #0x44]
	bl memcmp
	cmp r0, #0
	moveq r0, #1
	movne r0, #0
	cmp r0, #0
	bne _02234408
	ldr r0, [r5]
	add r1, r5, #4
	str r0, [r4, #4]
	add r0, r4, #8
	ldmia r1, {r2, r3}
	stmia r0, {r2, r3}
	ldr r1, [r5, #0xc]
	mov r0, #1
	str r1, [r4, #0x10]
	str r0, [r4, #0x14]
_02234408:
	mov r0, #1
	str r0, [r4, #0x30]
	ldr r0, [r4, #0x38]
	cmp r0, #0
	ldrne r0, [r4, #0x14]
	cmpne r0, #0
	beq _0223446C
	mov r0, #0
	str r0, [r4, #0x14]
	ldr r5, [r4, #4]
	ldr r2, [r4, #8]
	ldr r1, [r4, #0xc]
	ldr r3, _0223453C ; =_022577C4
	add r0, sp, #0x28
	add r4, sp, #0x64
	str r5, [sp, #0x30]
	str r2, [sp, #0x28]
	str r1, [sp, #0x2c]
	ldmia r0, {r0, r1, r2}
	stmia r4, {r0, r1, r2}
	ldr r1, [r3]
	mov r0, sl
	ldr r2, [r1, #0x20]
	mov r1, r4
	blx r2
_0223446C:
	add sp, sp, #0x70
	mov r0, #1
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_02234478:
	ldr r1, _02234544 ; =ov45_022550E4
	mov r0, r4
	bl strcmp
	cmp r0, #0
	bne _022344D4
	cmp r8, #8
	addne sp, sp, #0x70
	movne r0, #1
	ldmneia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldr r0, _0223453C ; =_022577C4
	ldr r4, [sb]
	ldr r3, [sb, #4]
	ldr r1, [r0]
	mov r0, #1
	str r0, [r1, #0x148]
	add r2, sp, #0x10
	str r4, [sp, #0x10]
	str r3, [sp, #0x14]
	add r1, r1, #0x14c
	ldmia r2, {r2, r3}
	add sp, sp, #0x70
	stmia r1, {r2, r3}
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_022344D4:
	ldr r1, _02234548 ; =ov45_022550F4
	mov r0, r4
	bl strcmp
	cmp r0, #0
	bne _0223452C
	ldr r0, _0223453C ; =_022577C4
	mov r1, sb
	ldr r0, [r0]
	mov r2, r8
	add r0, r0, #0x144
	bl ov45_02238C74
	cmp r0, #0
	addeq sp, sp, #0x70
	moveq r0, #1
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldr r0, _0223453C ; =_022577C4
	mov r2, #4
	ldr r1, [r0]
	add sp, sp, #0x70
	str r2, [r1, #0x144]
	mov r0, #1
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_0223452C:
	mov r0, #0
	add sp, sp, #0x70
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.balign 4, 0
_02234538: .word ov45_022550C4
_0223453C: .word _022577C4
_02234540: .word ov45_022550D4
_02234544: .word ov45_022550E4
_02234548: .word ov45_022550F4
	arm_func_end ov45_0223415C
