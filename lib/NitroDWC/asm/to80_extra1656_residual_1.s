	.extern _02254D04
	.extern _02254F58
	.extern _022577C4
	.extern ov45_02231C78
	.extern ov45_02233994
	.extern ov45_02233CA0
	.extern ov45_02234688
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
	.extern ov45_02231490

.text
	arm_func_start ov45_02231514
ov45_02231514: ; 0x02231514
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x6c
	ldr r0, _02231C20 ; =ov45_022577D4
	ldr r7, [r0]
	mov r0, #0
	cmp r7, #0
	beq _022317B0
	str r0, [sp, #0x10]
	bl ov45_022545F0
	cmp r0, #0
	beq _02231734
	cmp r0, #1
	beq _02231620
	cmp r0, #8
	bne _02231778
	add r0, sp, #0x10
	bl ov45_02254774
	mov r5, r0
	ldr r1, [sp, #0x10]
	mov r0, #0
	bl ov00_021EC2A8
	movs r4, r0
	beq _02231580
	ldr r2, [sp, #0x10]
	mov r0, r5
	mov r1, r4
	bl MI_CpuCopy8
_02231580:
	cmp r4, #0
	beq _022315C8
	ldr r6, [r7, #0x38]
	ldr r5, [sp, #0x10]
	bl ov45_022546F8
	ldr r2, _02231C24 ; =ov45_022577D8
	ldr r0, _02231C20 ; =ov45_022577D4
	ldr r8, [r2]
	ldr r3, [r0]
	mov r1, r4
	str r3, [r2]
	str r8, [r0]
	ldr r7, [r7, #0x34]
	mov r2, r5
	mov r3, r6
	mov r0, #1
	blx r7
	b _02231600
_022315C8:
	ldr r5, [r7, #0x38]
	bl ov45_022546F8
	ldr r3, _02231C24 ; =ov45_022577D8
	mov r0, #0
	ldr r2, _02231C20 ; =ov45_022577D4
	ldr r8, [r3]
	ldr r6, [r2]
	mov r1, r0
	str r6, [r3]
	str r8, [r2]
	ldr r6, [r7, #0x34]
	mov r2, r0
	mov r3, r5
	blx r6
_02231600:
	cmp r4, #0
	beq _02231618
	mov r0, #0
	mov r1, r4
	mov r2, r0
	bl ov00_021EC2EC
_02231618:
	mov r0, #1
	b _0223177C
_02231620:
	bl ov00_021EC210
	ldr r0, [r7, #4]
	subs r0, r0, #1
	str r0, [r7, #4]
	beq _022316F4
	bl ov45_022546F8
	ldr r0, [r7, #0xc]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r1, r7, #0xd
	ldrne r1, [r7, #0x14]
	ldr r0, [r7, #8]
	bl ov45_0225443C
	ldr r0, [r7, #0x3c]
	ldr r4, [r7, #0x2c]
	cmp r0, #0
	ldr r0, [r7, #0x18]
	beq _022316A0
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r5, r7, #0x19
	ldrne r5, [r7, #0x20]
	add r0, r7, #0x28
	bl ov45_02231C78
	mov r1, #0
	str r1, [sp]
	mov r2, r0
	ldr r1, [r7, #0x24]
	mov r0, r5
	mov r3, r4
	bl ov45_02254A04
	b _022316D4
_022316A0:
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r5, r7, #0x19
	ldrne r5, [r7, #0x20]
	add r0, r7, #0x28
	bl ov45_02231C78
	mov r1, #0
	str r1, [sp]
	mov r2, r0
	ldr r1, [r7, #0x24]
	mov r0, r5
	mov r3, r4
	bl ov45_022549E0
_022316D4:
	cmp r0, #0
	moveq r0, #1
	beq _022316E8
	bl ov45_022546F8
	mov r0, #0
_022316E8:
	cmp r0, #0
	movne r0, #0
	bne _0223177C
_022316F4:
	ldr r4, [r7, #0x38]
	bl ov45_022546F8
	ldr r3, _02231C24 ; =ov45_022577D8
	mov r0, #0
	ldr r2, _02231C20 ; =ov45_022577D4
	ldr r6, [r3]
	ldr r5, [r2]
	mov r1, r0
	str r5, [r3]
	str r6, [r2]
	ldr r5, [r7, #0x34]
	mov r2, r0
	mov r3, r4
	blx r5
	mov r0, #1
	b _0223177C
_02231734:
	bl ov00_021EC210
	ldr r5, [r7, #0x38]
	bl ov45_022546F8
	mov r0, #0
	ldr r4, _02231C24 ; =ov45_022577D8
	ldr r2, _02231C20 ; =ov45_022577D4
	ldr r6, [r4]
	mov r3, r5
	ldr r5, [r2]
	mov r1, r0
	str r5, [r4]
	str r6, [r2]
	ldr r4, [r7, #0x34]
	mov r2, r0
	blx r4
	mov r0, #1
	b _0223177C
_02231778:
	mov r0, #0
_0223177C:
	cmp r0, #0
	ldrne r0, _02231C24 ; =ov45_022577D8
	ldrne r0, [r0]
	cmpne r0, #0
	beq _022317AC
	beq _022317A0
	ldr r1, [r0]
	ldr r1, [r1, #4]
	blx r1
_022317A0:
	ldr r0, _02231C24 ; =ov45_022577D8
	mov r1, #0
	str r1, [r0]
_022317AC:
	mov r0, #1
_022317B0:
	cmp r0, #0
	beq _022317D4
	ldr r0, _02231C28 ; =_022577C4
	ldr r0, [r0]
	cmp r0, #0
	bne _022317EC
	add sp, sp, #0x6c
	mov r0, #9
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
_022317D4:
	ldr r0, _02231C28 ; =_022577C4
	ldr r0, [r0]
	cmp r0, #0
	addeq sp, sp, #0x6c
	moveq r0, #0
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
_022317EC:
	cmp r0, #0
	beq _02231810
	ldr r0, _02231C28 ; =_022577C4
	ldr r0, [r0]
	ldr r0, [r0]
	cmp r0, #0xa
	addeq sp, sp, #0x6c
	moveq r0, #0xa
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
_02231810:
	bl ov45_02240874
	cmp r0, #5
	addls pc, pc, r0, lsl #2
	b _02231918
_02231820: ; jump table
	b _02231838 ; case 0
	b _02231918 ; case 1
	b _02231918 ; case 2
	b _022318E0 ; case 3
	b _02231918 ; case 4
	b _022318FC ; case 5
_02231838:
	bl ov45_02231490
	ldr r0, _02231C28 ; =_022577C4
	ldr r4, [r0]
	cmp r4, #0
	beq _022318D4
	beq _022318C8
	add r0, r4, #0x194
	bl ov45_0223770C
	add r0, r4, #0x17c
	bl ov45_02236AE8
	add r0, r4, #0x170
	bl ov45_0223775C
	add r0, r4, #0x134
	bl ov45_02236CB0
	add r0, r4, #0x134
	bl ov45_02236E38
	ldr r1, _02231C2C ; =ov45_02254FA0
	add r0, r4, #0xe8
	str r1, [r4, #0xe8]
	bl ov45_0223DF4C
	add r0, r4, #0x110
	bl ov45_022379D4
	add r0, r4, #0xec
	bl ov45_02236B88
	add r0, r4, #0xd4
	bl ov45_02237BC8
	add r0, r4, #0xbc
	bl ov45_02237BC8
	add r0, r4, #0x58
	bl ov45_02237BC8
	cmp r4, #0
	beq _022318C8
	mov r0, #0
	mov r1, r4
	mov r2, r0
	bl ov00_021EC2EC
_022318C8:
	ldr r0, _02231C28 ; =_022577C4
	mov r1, #0
	str r1, [r0]
_022318D4:
	add sp, sp, #0x6c
	mov r0, #9
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
_022318E0:
	ldr r0, _02231C28 ; =_022577C4
	ldr r1, [r0]
	ldr r0, [r1]
	cmp r0, #1
	moveq r0, #2
	streq r0, [r1]
	b _02231918
_022318FC:
	bl ov45_02240A58
	ldr r1, _02231C28 ; =_022577C4
	cmp r0, #0
	ldr r1, [r1]
	strne r0, [r1, #4]
	movne r0, #0xa
	strne r0, [r1]
_02231918:
	ldr r2, _02231C28 ; =_022577C4
	add r0, sp, #0x28
	add r1, sp, #0x38
	ldr r5, [r2]
	bl ov00_021ECB94
	add r0, sp, #0x28
	add r1, sp, #0x38
	bl RTC_ConvertDateTimeToSecond
	ldr r2, [r5, #0x140]
	add r7, r5, #0x138
	mov r6, r0
	mov sb, r1
	str r2, [sp, #0xc]
	cmp r2, r7
	beq _022319E8
	mov fp, #1
	mov sl, #0
	add r4, sp, #0xc
_02231960:
	ldr r1, [r2, #0x10]
	ldr r0, [r2, #0x14]
	subs r1, r6, r1
	ldr r8, [r2, #0x18]
	sbc r0, sb, r0
	ldr r3, [r2, #0x1c]
	subs r1, r8, r1
	sbcs r1, r3, r0
	bge _022319A4
	str r6, [r2, #0x10]
	str sb, [r2, #0x14]
	ldr r0, [r2, #0x20]
	ldr r1, [r2, #0x24]
	blx r1
	cmp r0, #0
	moveq r0, sl
	beq _022319A8
_022319A4:
	mov r0, fp
_022319A8:
	cmp r0, #0
	bne _022319D4
	ldr r8, [sp, #0xc]
	mov r0, r4
	bl ov45_02236E58
	add r0, sp, #8
	mov r2, r8
	add r1, r5, #0x134
	str r8, [sp, #4]
	bl ov45_02236C40
	b _022319DC
_022319D4:
	mov r0, r4
	bl ov45_02236E58
_022319DC:
	ldr r2, [sp, #0xc]
	cmp r2, r7
	bne _02231960
_022319E8:
	add r0, r5, #0xe8
	bl ov45_0223E558
	add r0, r5, #0x144
	bl ov45_0223AF28
	ldr r0, _02231C28 ; =_022577C4
	ldr r1, [r0]
	ldr r0, [r1]
	cmp r0, #0xa
	addls pc, pc, r0, lsl #2
	b _02231C0C
_02231A10: ; jump table
	b _02231A3C ; case 0
	b _02231C0C ; case 1
	b _02231A48 ; case 2
	b _02231A54 ; case 3
	b _02231A74 ; case 4
	b _02231C0C ; case 5
	b _02231AD4 ; case 6
	b _02231B3C ; case 7
	b _02231C0C ; case 8
	b _02231C0C ; case 9
	b _02231C0C ; case 10
_02231A3C:
	add sp, sp, #0x6c
	mov r0, #9
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
_02231A48:
	mov r0, #3
	str r0, [r1]
	b _02231C0C
_02231A54:
	ldr r0, [r1, #0x144]
	cmp r0, #2
	movge r0, #1
	movlt r0, #0
	cmp r0, #0
	movne r0, #4
	strne r0, [r1]
	b _02231C0C
_02231A74:
	ldr r1, _02231C30 ; =ov45_022550B0
	add r0, sp, #0x50
	bl ov45_02237B58
	add r0, sp, #0x44
	add r2, sp, #0x50
	mov r1, #0
	bl ov45_02233994
	ldr r0, [sp, #0x44]
	ldr r1, _02231C34 ; =ov45_0223633C
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r0, sp, #0x45
	ldrne r0, [sp, #0x4c]
	mov r2, #0
	bl ov45_02241A20
	add r0, sp, #0x44
	bl ov45_02237BC8
	add r0, sp, #0x50
	bl ov45_02237BC8
	ldr r0, _02231C28 ; =_022577C4
	mov r1, #5
	ldr r0, [r0]
	str r1, [r0]
	b _02231C0C
_02231AD4:
	bl ov45_02233CA0
	cmp r0, #0
	beq _02231B28
	ldr r1, _02231C28 ; =_022577C4
	cmp r0, #1
	ldr r1, [r1]
	beq _02231AFC
	cmp r0, #2
	beq _02231B04
	b _02231B0C
_02231AFC:
	mov r0, #2
	b _02231B10
_02231B04:
	mov r0, #3
	b _02231B10
_02231B0C:
	mov r0, #0
_02231B10:
	cmp r0, #0
	beq _02231C0C
	str r0, [r1, #4]
	mov r0, #0xa
	str r0, [r1]
	b _02231C0C
_02231B28:
	ldr r0, _02231C28 ; =_022577C4
	mov r1, #7
	ldr r0, [r0]
	str r1, [r0]
	b _02231C0C
_02231B3C:
	bl ov45_02234688
	cmp r0, #0
	beq _02231C0C
	ldr r0, _02231C28 ; =_022577C4
	add r7, sp, #0x60
	ldr r4, [r0]
	mov r0, #0
	str r0, [r4, #0x10c]
	ldr r0, [r4, #0xfc]
	mov r6, #1
	str r0, [sp, #0x5c]
	add r0, r4, #0x100
	ldmia r0, {r2, r3}
	stmia r7, {r2, r3}
	ldr r1, [sp, #0x5c]
	str r6, [sp, #0x68]
	add r0, sp, #0x18
	str r1, [sp, #0x14]
	add r5, sp, #0x14
	ldmia r7, {r2, r3}
	stmia r0, {r2, r3}
	mov r1, r5
	add r0, r4, #0xfc
	mov r2, #0x10
	str r6, [sp, #0x20]
	str r6, [sp, #0x24]
	bl memcmp
	cmp r0, #0
	moveq r0, r6
	movne r0, #0
	cmp r0, #0
	bne _02231BE4
	ldr r0, [r5]
	add r1, r5, #4
	str r0, [r4, #0xfc]
	add r0, r4, #0x100
	ldmia r1, {r2, r3}
	stmia r0, {r2, r3}
	ldr r1, [r5, #0xc]
	mov r0, #1
	str r1, [r4, #0x108]
	str r0, [r4, #0x10c]
_02231BE4:
	mov r0, #1
	ldr r1, _02231C28 ; =_022577C4
	str r0, [r4, #0x128]
	ldr r2, [r1]
	mov r3, #8
	str r3, [r2]
	ldr r2, [r1]
	mov r1, #0
	ldr r2, [r2, #8]
	blx r2
_02231C0C:
	ldr r0, _02231C28 ; =_022577C4
	ldr r0, [r0]
	ldr r0, [r0]
	add sp, sp, #0x6c
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.balign 4, 0
_02231C20: .word ov45_022577D4
_02231C24: .word ov45_022577D8
_02231C28: .word _022577C4
_02231C2C: .word ov45_02254FA0
_02231C30: .word ov45_022550B0
_02231C34: .word ov45_0223633C
	arm_func_end ov45_02231514
