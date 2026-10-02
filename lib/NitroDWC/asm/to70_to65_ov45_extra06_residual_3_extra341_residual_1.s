	.extern _02254D04
	.extern _02254F58
	.extern _022577C4
	.extern ov45_022320B4
	.extern ov45_02233B74
	.extern ov45_022340F8
	.extern ov45_02236308
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
	.extern ov45_02236680

.text
	arm_func_start ov45_0223633C
ov45_0223633C: ; 0x0223633C
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x3c
	ldr r8, [sp, #0x60]
	cmp r0, #0
	mov sl, r1
	mov sb, r2
	addeq sp, sp, #0x3c
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	cmp sl, #0
	mov r7, #0
	ble _0223644C
	ldr fp, _02236494 ; =ov45_022550B8
	add r6, sp, #0x24
	add r5, sp, #0x18
	add r4, sp, #0x30
_02236378:
	ldr r0, [r8, r7, lsl #2]
	cmp r0, #0x14
	bge _02236440
	ldr r1, [sb, r7, lsl #2]
	mov r0, r6
	bl ov45_02237B58
	mov r0, r5
	mov r1, fp
	bl ov45_02237B58
	mov r0, r4
	mov r1, r6
	mov r2, r5
	mov r3, #0
	bl ov45_0223649C
	mov r0, r5
	bl ov45_02237BC8
	mov r0, r6
	bl ov45_02237BC8
	ldr r0, [sp, #0x34]
	cmp r0, #2
	bhs _022363D8
	mov r0, r4
	bl ov45_02237664
	b _02236440
_022363D8:
	ldr r1, [sp, #0x30]
	ldr r0, [r1, #0xc]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r1, r1, #0xd
	ldrne r1, [r1, #0x14]
	add r0, sp, #0xc
	bl ov45_02237B58
	ldr r0, _02236498 ; =_022577C4
	add r1, sp, #0xc
	ldr r0, [r0]
	add r0, r0, #0x58
	bl ov45_02237D48
	add r0, sp, #0xc
	bl ov45_02237BC8
	ldr r1, _02236498 ; =_022577C4
	mov r3, #0
	ldr r2, [r1]
	add r0, sp, #0x30
	str r3, [r2, #0x64]
	ldr r1, [r1]
	mov r2, #6
	str r2, [r1]
	bl ov45_02237664
	add sp, sp, #0x3c
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
_02236440:
	add r7, r7, #1
	cmp r7, sl
	blt _02236378
_0223644C:
	add r0, sp, #0
	bl ov45_02233B74
	ldr r0, _02236498 ; =_022577C4
	add r1, sp, #0
	ldr r0, [r0]
	add r0, r0, #0x58
	bl ov45_02237D48
	add r0, sp, #0
	bl ov45_02237BC8
	ldr r0, _02236498 ; =_022577C4
	mov r3, #1
	ldr r2, [r0]
	mov r1, #6
	str r3, [r2, #0x64]
	ldr r0, [r0]
	str r1, [r0]
	add sp, sp, #0x3c
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.balign 4, 0
_02236494: .word ov45_022550B8
_02236498: .word _022577C4
	arm_func_end ov45_0223633C

	arm_func_start ov45_0223649C
ov45_0223649C: ; 0x0223649C
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x2c
	mov sl, r0
	mov r4, #0
	str r4, [sl]
	str r4, [sl, #4]
	add r0, sp, #0x20
	mov sb, r2
	str r3, [sp]
	str r4, [sl, #8]
	mov r7, #1
	bl ov45_02237A24
	sub r5, sp, #4
	add fp, sp, #0x20
	add r6, sp, #0x14
	mvn r4, #0
	b _022365BC
_022364E0:
	cmp r8, #0
	beq _02236574
	mov r0, r6
	mov r1, fp
	mov r2, #0
	mov r3, r8
	bl ov45_02236680
	mov r1, #0
	add r0, sp, #7
	strb r1, [r0]
	ldr r1, [sl, #4]
	ldr r0, [sl, #8]
	cmp r1, r0
	bhs _02236544
	mov r0, #0xc
	mul r0, r1, r0
	ldr r1, [sl]
	adds r0, r1, r0
	beq _02236534
	mov r1, r6
	bl ov45_02237A24
_02236534:
	ldr r0, [sl, #4]
	add r0, r0, #1
	str r0, [sl, #4]
	b _02236568
_02236544:
	mov r0, #0
	mov r2, #0
	strb r0, [sp, #6]
	mov r2, r2
	strb r2, [r5]
	ldr r2, [r5]
	mov r0, sl
	mov r1, r6
	bl ov45_02237634
_02236568:
	mov r0, r6
	bl ov45_02237BC8
	add r7, r7, #1
_02236574:
	ldr r0, [sb]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	ldrne r0, [sb, #4]
	bne _02236594
	ldrb r0, [sb]
	mov r0, r0, lsl #0x18
	mov r0, r0, lsr #0x19
_02236594:
	add r2, r8, r0
	add r0, sp, #8
	mov r1, fp
	mov r3, r4
	bl ov45_02236680
	mov r0, fp
	add r1, sp, #8
	bl ov45_02237D48
	add r0, sp, #8
	bl ov45_02237BC8
_022365BC:
	ldr r0, [sp]
	cmp r7, r0
	beq _022365E4
	mov r0, fp
	mov r1, sb
	mov r2, #0
	bl ov45_02237EB0
	mov r8, r0
	cmp r8, r4
	bne _022364E0
_022365E4:
	ldr r0, [sp, #0x20]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	ldrne r0, [sp, #0x24]
	bne _02236604
	ldrb r0, [sp, #0x20]
	mov r0, r0, lsl #0x18
	mov r0, r0, lsr #0x19
_02236604:
	cmp r0, #0
	beq _02236670
	mov r2, #0
	strb r2, [sp, #5]
	ldr r1, [sl, #4]
	ldr r0, [sl, #8]
	cmp r1, r0
	bhs _02236650
	mov r0, #0xc
	mul r0, r1, r0
	ldr r1, [sl]
	adds r0, r1, r0
	beq _02236640
	add r1, sp, #0x20
	bl ov45_02237A24
_02236640:
	ldr r0, [sl, #4]
	add r0, r0, #1
	str r0, [sl, #4]
	b _02236670
_02236650:
	strb r2, [sp, #4]
	sub r1, sp, #4
	and r0, r2, #0xff
	strb r0, [r1]
	ldr r2, [r1]
	add r1, sp, #0x20
	mov r0, sl
	bl ov45_02237634
_02236670:
	add r0, sp, #0x20
	bl ov45_02237BC8
	add sp, sp, #0x2c
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	arm_func_end ov45_0223649C
