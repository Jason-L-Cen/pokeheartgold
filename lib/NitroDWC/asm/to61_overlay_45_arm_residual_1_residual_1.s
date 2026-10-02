	.extern _02254D04
	.extern _02254F58
	.extern _022577C4
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
	.public ov45_022310B0
	.public ov45_02231C78
	.public ov45_022320B4
	.public ov45_0223325C
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

.text
	arm_func_start ov45_022310B0
ov45_022310B0: ; 0x022310B0
	ldr r0, _022310BC ; =_022577C4
	ldr r0, [r0]
	bx lr
	.balign 4, 0
_022310BC: .word _022577C4
	arm_func_end ov45_022310B0

	arm_func_start ov45_022310C0
ov45_022310C0: ; 0x022310C0
	stmdb sp!, {r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0x10
	ldr r8, [sp, #0x28]
	mov r7, r0
	mov r0, r8
	mov r6, r1
	mov r5, r2
	mov r4, r3
	bl DWC_CheckUserData
	cmp r0, #0
	beq _022310FC
	mov r0, r8
	bl DWC_CheckHasProfile
	cmp r0, #0
	bne _02231108
_022310FC:
	add sp, sp, #0x10
	mov r0, #3
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_02231108:
	ldr r1, [r8, #0x1c]
	ldr r0, [sp, #0x2c]
	str r1, [sp]
	ldr ip, [sp, #0x30]
	str r0, [sp, #4]
	mov r0, r7
	mov r1, r6
	mov r2, r5
	mov r3, r4
	str ip, [sp, #8]
	mov r4, #1
	str r4, [sp, #0xc]
	bl ov45_02231144
	add sp, sp, #0x10
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	arm_func_end ov45_022310C0

	arm_func_start ov45_02231144
ov45_02231144: ; 0x02231144
	stmdb sp!, {r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0x30
	ldr r4, _02231430 ; =_022577C4
	mov r8, r0
	ldr r0, [r4]
	mov r7, r1
	cmp r0, #0
	mov r6, r2
	mov r5, r3
	addne sp, sp, #0x30
	movne r0, #4
	ldmneia sp!, {r4, r5, r6, r7, r8, pc}
	ldr r3, _02231434 ; =ov45_02236070
	ldr r2, _02231438 ; =ov45_022362B4
	mov r1, #5
	ldr r0, _0223143C ; =0x0000047C
	str r3, [sp, #0x24]
	str r2, [sp, #0x28]
	str r1, [sp, #0x2c]
	bl ov45_02242394
	movs r4, r0
	beq _022312D8
	mov r0, #0
	str r0, [r4]
	str r0, [r4, #4]
	add lr, r4, #8
	mov ip, #4
_022311B0:
	ldmia r5!, {r0, r1, r2, r3}
	stmia lr!, {r0, r1, r2, r3}
	subs ip, ip, #1
	bne _022311B0
	ldr r1, [r5]
	ldr r0, [sp, #0x48]
	str r1, [lr]
	str r0, [r4, #0x4c]
	ldr r0, [sp, #0x54]
	str r6, [r4, #0x50]
	str r0, [r4, #0x54]
	mov r0, #0
	add r2, r4, #0x58
	mov r1, r0
_022311E8:
	str r1, [r2, r0, lsl #2]
	add r0, r0, #1
	cmp r0, #3
	blo _022311E8
	str r1, [r4, #0x64]
	str r1, [r4, #0x68]
	str r1, [r4, #0x6c]
	str r1, [r4, #0x70]
	mov r0, #5
	str r0, [r4, #0xb8]
	add r0, r4, #0xbc
	mov r3, #0
_02231218:
	str r3, [r0, r1, lsl #2]
	add r1, r1, #1
	cmp r1, #3
	blo _02231218
	mov r0, #5
	str r0, [r4, #0xd0]
	add r0, r4, #0xd4
	mov r2, #0
_02231238:
	str r2, [r0, r3, lsl #2]
	add r3, r3, #1
	cmp r3, #3
	blo _02231238
	ldr r0, _02231440 ; =ov45_02254FA0
	add r1, r4, #0xf0
	str r0, [r4, #0xe8]
	str r2, [r4, #0xec]
	str r2, [r4, #0xf0]
	str r1, [r4, #0xf4]
	ldr r0, _02231444 ; =ov45_02254FB0
	add r1, sp, #8
	str r0, [r4, #0xf8]
	mov r0, #5
	str r0, [r4, #0xfc]
	str r2, [r4, #0x100]
	str r2, [r4, #0x104]
	add r0, sp, #0x14
	str r2, [r4, #0x108]
	bl ov00_021ECB94
	add r0, sp, #0x14
	add r1, sp, #8
	bl RTC_ConvertDateTimeToSecond
	str r0, [r4, #0x100]
	str r1, [r4, #0x104]
	mov r0, #1
	mov r2, #0
	str r0, [r4, #0x10c]
	str r2, [r4, #0x110]
	str r2, [r4, #0x114]
	str r2, [r4, #0x118]
	str r0, [r4, #0x11c]
	sub r0, r2, #1
	str r0, [r4, #0x124]
	str r2, [r4, #0x134]
	str r2, [r4, #0x138]
	add r1, r4, #0x138
	add r0, r4, #0x144
	str r1, [r4, #0x140]
	bl ov45_02238B08
_022312D8:
	ldr r0, _02231430 ; =_022577C4
	cmp r4, #0
	str r4, [r0]
	addeq sp, sp, #0x30
	moveq r0, #1
	ldmeqia sp!, {r4, r5, r6, r7, r8, pc}
	ldr r4, [sp, #0x48]
	add r3, sp, #0x24
	mov r0, r8
	mov r1, r7
	mov r2, r6
	str r4, [sp]
	bl ov45_02240B10
	mov r4, r0
	ldr r1, _02231430 ; =_022577C4
	mov r0, #0
	strb r0, [sp, #4]
	cmp r4, #1
	ldr r1, [r1]
	beq _02231334
	cmp r4, #2
	moveq r0, #3
	b _02231338
_02231334:
	mov r0, #2
_02231338:
	cmp r0, #0
	strne r0, [r1, #4]
	movne r0, #0xa
	strne r0, [r1]
	cmp r4, #0
	beq _022313F4
	ldr r0, _02231430 ; =_022577C4
	ldr r5, [r0]
	cmp r5, #0
	beq _022313E8
	beq _022313DC
	add r0, r5, #0x194
	bl ov45_0223770C
	add r0, r5, #0x17c
	bl ov45_02236AE8
	add r0, r5, #0x170
	bl ov45_0223775C
	add r0, r5, #0x134
	bl ov45_02236CB0
	add r0, r5, #0x134
	bl ov45_02236E38
	ldr r1, _02231440 ; =ov45_02254FA0
	add r0, r5, #0xe8
	str r1, [r5, #0xe8]
	bl ov45_0223DF4C
	add r0, r5, #0x110
	bl ov45_022379D4
	add r0, r5, #0xec
	bl ov45_02236B88
	add r0, r5, #0xd4
	bl ov45_02237BC8
	add r0, r5, #0xbc
	bl ov45_02237BC8
	add r0, r5, #0x58
	bl ov45_02237BC8
	cmp r5, #0
	beq _022313DC
	mov r0, #0
	mov r1, r5
	mov r2, r0
	bl ov00_021EC2EC
_022313DC:
	ldr r0, _02231430 ; =_022577C4
	mov r1, #0
	str r1, [r0]
_022313E8:
	add sp, sp, #0x30
	mov r0, r4
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_022313F4:
	ldr r0, _02231430 ; =_022577C4
	ldr r1, [sp, #0x48]
	ldr r0, [r0]
	ldr r2, [sp, #0x4c]
	ldr r3, [sp, #0x50]
	add r0, r0, #0xe8
	bl ov45_0223E100
	bl ov45_022335A0
	ldr r0, _02231430 ; =_022577C4
	mov r2, #1
	ldr r1, [r0]
	mov r0, #0
	str r2, [r1]
	add sp, sp, #0x30
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	.balign 4, 0
_02231430: .word _022577C4
_02231434: .word ov45_02236070
_02231438: .word ov45_022362B4
_0223143C: .word 0x0000047C
_02231440: .word ov45_02254FA0
_02231444: .word ov45_02254FB0
	arm_func_end ov45_02231144

	arm_func_start ov45_02231448
ov45_02231448: ; 0x02231448
	stmdb sp!, {r4, lr}
	mov r4, r0
	add r0, r4, #0x18
	bl ov45_022379D4
	mov r0, r4
	ldmia sp!, {r4, pc}
	arm_func_end ov45_02231448

	arm_func_start ov45_02231460
ov45_02231460: ; 0x02231460
	stmdb sp!, {r4, lr}
	ldr r1, _0223148C ; =ov45_02254FA0
	mov r4, r0
	str r1, [r4]
	bl ov45_0223DF4C
	add r0, r4, #0x28
	bl ov45_022379D4
	add r0, r4, #4
	bl ov45_02236B88
	mov r0, r4
	ldmia sp!, {r4, pc}
	.balign 4, 0
_0223148C: .word ov45_02254FA0
	arm_func_end ov45_02231460

	arm_func_start ov45_02231490
ov45_02231490: ; 0x02231490
	stmdb sp!, {r3, lr}
	ldr r0, _02231510 ; =_022577C4
	ldr r1, [r0]
	cmp r1, #0
	moveq r0, #0
	ldmeqia sp!, {r3, pc}
	ldr r0, [r1]
	cmp r0, #9
	moveq r0, #0
	ldmeqia sp!, {r3, pc}
	add r0, r1, #0x134
	bl ov45_02236CB0
	ldr r0, _02231510 ; =_022577C4
	mov r1, #9
	ldr r0, [r0]
	str r1, [r0]
	bl ov45_02240F9C
	ldr r1, _02231510 ; =_022577C4
	mov r2, #0
	strb r2, [sp]
	cmp r0, #1
	ldr r3, [r1]
	beq _022314F8
	cmp r0, #2
	moveq r2, #3
	b _022314FC
_022314F8:
	mov r2, #2
_022314FC:
	cmp r2, #0
	strne r2, [r3, #4]
	movne r1, #0xa
	strne r1, [r3]
	ldmia sp!, {r3, pc}
	.balign 4, 0
_02231510: .word _022577C4
	arm_func_end ov45_02231490

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

	arm_func_start ov45_02231C38
ov45_02231C38: ; 0x02231C38
	stmdb sp!, {r4, lr}
	mov r4, r0
	add r0, r4, #0x28
	bl ov45_022379D4
	add r0, r4, #0x18
	bl ov45_02237BC8
	add r0, r4, #0xc
	bl ov45_02237BC8
	cmp r4, #0
	beq _02231C70
	mov r0, #0
	mov r1, r4
	mov r2, r0
	bl ov00_021EC2EC
_02231C70:
	mov r0, r4
	ldmia sp!, {r4, pc}
	arm_func_end ov45_02231C38

	arm_func_start ov45_02231C78
ov45_02231C78: ; 0x02231C78
	ldr r1, [r0, #4]
	cmp r1, #0
	moveq r0, #0
	ldrne r0, [r0]
	bx lr
	arm_func_end ov45_02231C78

	arm_func_start ov45_02231C8C
ov45_02231C8C: ; 0x02231C8C
	stmdb sp!, {r4, r5, lr}
	sub sp, sp, #0x54
	ldr r1, _02231E88 ; =_022577C4
	mov r4, r0
	ldr r1, [r1]
	cmp r1, #0
	addeq sp, sp, #0x54
	moveq r0, #4
	ldmeqia sp!, {r4, r5, pc}
	beq _02231CC8
	ldr r0, [r1]
	cmp r0, #0xa
	addeq sp, sp, #0x54
	moveq r0, #7
	ldmeqia sp!, {r4, r5, pc}
_02231CC8:
	cmp r1, #0
	beq _02231CEC
	ldr r0, _02231E88 ; =_022577C4
	ldr r0, [r0]
	ldr r0, [r0]
	cmp r0, #8
	addne sp, sp, #0x54
	movne r0, #4
	ldmneia sp!, {r4, r5, pc}
_02231CEC:
	cmp r1, #0
	beq _02231D14
	ldr r0, [r1, #0xd0]
	cmp r0, #5
	movne r0, #1
	moveq r0, #0
	cmp r0, #0
	addne sp, sp, #0x54
	movne r0, #4
	ldmneia sp!, {r4, r5, pc}
_02231D14:
	ldr r5, _02231E8C ; =ov45_02235C40
	ldr lr, _02231E90 ; =ov45_02235E4C
	ldr ip, _02231E94 ; =ov45_02236090
	ldr r3, _02231E98 ; =ov45_022362D0
	ldr r2, _02231E9C ; =ov45_02236A44
	add r0, sp, #0x24
	str r5, [sp, #0x3c]
	str lr, [sp, #0x40]
	str ip, [sp, #0x44]
	str r3, [sp, #0x48]
	str r2, [sp, #0x4c]
	str r4, [sp, #0x50]
	bl ov45_02231EB0
	add r0, sp, #0x30
	add r2, sp, #0x24
	mov r1, r4
	bl ov45_02233994
	add r0, sp, #0x24
	bl ov45_02237BC8
	cmp r4, #4
	addls pc, pc, r4, lsl #2
	b _02231D98
_02231D6C: ; jump table
	b _02231D98 ; case 0
	b _02231D80 ; case 1
	b _02231D80 ; case 2
	b _02231D88 ; case 3
	b _02231D90 ; case 4
_02231D80:
	ldr r2, _02231EA0 ; =ov45_02254D5C
	b _02231D9C
_02231D88:
	ldr r2, _02231EA4 ; =ov45_02254D1C
	b _02231D9C
_02231D90:
	ldr r2, _02231EA8 ; =ov45_02254D3C
	b _02231D9C
_02231D98:
	mov r2, #0
_02231D9C:
	ldr r0, [sp, #0x30]
	ldr r1, _02231EAC ; =ov45_02235A94
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r0, sp, #0x31
	ldrne r0, [sp, #0x38]
	add r3, sp, #0x3c
	stmia sp, {r1, r4}
	ldr r2, [r2, #0x1c]
	mov r1, #0
	bl ov45_02241048
	mov r5, r0
	ldr r1, _02231E88 ; =_022577C4
	mov r0, #0
	strb r0, [sp, #8]
	cmp r5, #1
	ldr r1, [r1]
	beq _02231DF0
	cmp r5, #2
	moveq r0, #3
	b _02231DF4
_02231DF0:
	mov r0, #2
_02231DF4:
	cmp r0, #0
	strne r0, [r1, #4]
	movne r0, #0xa
	strne r0, [r1]
	cmp r5, #0
	beq _02231E20
	add r0, sp, #0x30
	bl ov45_02237BC8
	add sp, sp, #0x54
	mov r0, r5
	ldmia sp!, {r4, r5, pc}
_02231E20:
	add r0, sp, #0x10
	str r4, [sp, #0xc]
	add r1, sp, #0x30
	add r4, sp, #0xc
	bl ov45_02237A24
	mov r1, #0
	ldr r0, _02231E88 ; =_022577C4
	ldr r2, [sp, #0xc]
	ldr r5, [r0]
	str r1, [r4, #0x10]
	str r1, [r4, #0x14]
	add r1, sp, #0x10
	str r2, [r5, #0xd0]
	add r0, r5, #0xd4
	bl ov45_02237D48
	ldr r1, [sp, #0x1c]
	add r0, sp, #0x10
	str r1, [r5, #0xe0]
	ldr r1, [sp, #0x20]
	str r1, [r5, #0xe4]
	bl ov45_02237BC8
	add r0, sp, #0x30
	bl ov45_02237BC8
	mov r0, #0
	add sp, sp, #0x54
	ldmia sp!, {r4, r5, pc}
	.balign 4, 0
_02231E88: .word _022577C4
_02231E8C: .word ov45_02235C40
_02231E90: .word ov45_02235E4C
_02231E94: .word ov45_02236090
_02231E98: .word ov45_022362D0
_02231E9C: .word ov45_02236A44
_02231EA0: .word ov45_02254D5C
_02231EA4: .word ov45_02254D1C
_02231EA8: .word ov45_02254D3C
_02231EAC: .word ov45_02235A94
	arm_func_end ov45_02231C8C

	arm_func_start ov45_02231EB0
ov45_02231EB0: ; 0x02231EB0
	ldr ip, _02231EBC ; =ov45_02237A24
	add r1, r1, #0x58
	bx ip
	.balign 4, 0
_02231EBC: .word ov45_02237A24
	arm_func_end ov45_02231EB0

	arm_func_start ov45_02231EC0
ov45_02231EC0: ; 0x02231EC0
	stmdb sp!, {r3, r4, r5, lr}
	sub sp, sp, #0x40
	ldr r0, _022320B0 ; =_022577C4
	ldr r1, [r0]
	cmp r1, #0
	addeq sp, sp, #0x40
	moveq r0, #4
	ldmeqia sp!, {r3, r4, r5, pc}
	beq _02231EF8
	ldr r0, [r1]
	cmp r0, #0xa
	addeq sp, sp, #0x40
	moveq r0, #7
	ldmeqia sp!, {r3, r4, r5, pc}
_02231EF8:
	cmp r1, #0
	beq _02231F1C
	ldr r0, _022320B0 ; =_022577C4
	ldr r0, [r0]
	ldr r0, [r0]
	cmp r0, #8
	addne sp, sp, #0x40
	movne r0, #4
	ldmneia sp!, {r3, r4, r5, pc}
_02231F1C:
	cmp r1, #0
	beq _02231F3C
	ldr r0, [r1, #0xd0]
	cmp r0, #5
	movne r0, #1
	moveq r0, #0
	cmp r0, #0
	bne _02231F48
_02231F3C:
	add sp, sp, #0x40
	mov r0, #4
	ldmia sp!, {r3, r4, r5, pc}
_02231F48:
	add r0, sp, #0x24
	add r1, r1, #0xd0
	bl ov45_022320B4
	ldr r0, [sp, #0x24]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r0, sp, #0x25
	ldrne r0, [sp, #0x2c]
	bl ov45_022412CC
	ldr r1, _022320B0 ; =_022577C4
	mov r2, #0
	strb r2, [sp]
	cmp r0, #1
	ldr r1, [r1]
	beq _02231F90
	cmp r0, #2
	moveq r2, #3
	b _02231F94
_02231F90:
	mov r2, #2
_02231F94:
	cmp r2, #0
	strne r2, [r1, #4]
	movne r0, #0xa
	strne r0, [r1]
	add r0, sp, #0x24
	bl ov45_02237BC8
	ldr r0, _022320B0 ; =_022577C4
	mov r1, #0
	ldr r4, [r0]
	add r2, sp, #0x18
	mov r0, r1
_02231FC0:
	str r0, [r2, r1, lsl #2]
	add r1, r1, #1
	cmp r1, #3
	blo _02231FC0
	mov r2, #5
	add r1, sp, #0x18
	add r0, r4, #0xd4
	str r2, [r4, #0xd0]
	bl ov45_02237D48
	mov r1, #0
	str r1, [r4, #0xe0]
	add r0, sp, #0x18
	str r1, [r4, #0xe4]
	bl ov45_02237BC8
	ldr r0, _022320B0 ; =_022577C4
	mov r1, #0
	ldr r4, [r0]
	add r5, sp, #4
	str r1, [r4, #0x10c]
	ldr r0, [r4, #0xfc]
	add r1, sp, #0x34
	str r0, [sp, #0x30]
	add r0, r4, #0x100
	ldmia r0, {r2, r3}
	stmia r1, {r2, r3}
	ldr ip, [r4, #0x108]
	mov r0, #5
	str ip, [sp, #0x3c]
	str r0, [sp, #0x30]
	str r0, [sp, #4]
	ldmia r1, {r2, r3}
	add r1, sp, #8
	stmia r1, {r2, r3}
	mov r0, #1
	str r0, [sp, #0x14]
	mov r1, r5
	add r0, r4, #0xfc
	mov r2, #0x10
	str ip, [sp, #0x10]
	bl memcmp
	cmp r0, #0
	moveq r0, #1
	movne r0, #0
	cmp r0, #0
	bne _0223209C
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
_0223209C:
	mov r0, #1
	str r0, [r4, #0x128]
	mov r0, #0
	add sp, sp, #0x40
	ldmia sp!, {r3, r4, r5, pc}
	.balign 4, 0
_022320B0: .word _022577C4
	arm_func_end ov45_02231EC0

	arm_func_start ov45_022320B4
ov45_022320B4: ; 0x022320B4
	ldr ip, _022320C0 ; =ov45_02237A24
	add r1, r1, #4
	bx ip
	.balign 4, 0
_022320C0: .word ov45_02237A24
	arm_func_end ov45_022320B4

	arm_func_start ov45_022320C4
ov45_022320C4: ; 0x022320C4
	stmdb sp!, {r4, r5, lr}
	sub sp, sp, #0x1c
	ldr r1, _0223216C ; =_022577C4
	mov r5, r0
	ldr r4, [r1]
	cmp r4, #0
	addeq sp, sp, #0x1c
	moveq r0, #4
	ldmeqia sp!, {r4, r5, pc}
	beq _02232100
	ldr r0, [r4]
	cmp r0, #0xa
	addeq sp, sp, #0x1c
	moveq r0, #7
	ldmeqia sp!, {r4, r5, pc}
_02232100:
	cmp r4, #0
	beq _02232124
	ldr r0, _0223216C ; =_022577C4
	ldr r0, [r0]
	ldr r0, [r0]
	cmp r0, #8
	addne sp, sp, #0x1c
	movne r0, #4
	ldmneia sp!, {r4, r5, pc}
_02232124:
	add r0, sp, #0xc
	add r1, sp, #0
	bl ov00_021ECB94
	add r0, sp, #0xc
	add r1, sp, #0
	bl RTC_ConvertDateTimeToSecond
	stmia r5, {r0, r1}
	ldr r2, [r4, #0x14c]
	ldr r1, [r4, #0x150]
	mov r0, #0
	str r2, [r5, #8]
	str r1, [r5, #0xc]
	ldr r2, [r4, #0x154]
	ldr r1, [r4, #0x158]
	str r2, [r5, #0x10]
	str r1, [r5, #0x14]
	add sp, sp, #0x1c
	ldmia sp!, {r4, r5, pc}
	.balign 4, 0
_0223216C: .word _022577C4
	arm_func_end ov45_022320C4

	arm_func_start ov45_02232170
ov45_02232170: ; 0x02232170
	stmdb sp!, {r3, lr}
	ldr r2, _022321DC ; =_022577C4
	ldr r3, [r2]
	mov r2, r1
	cmp r3, #0
	moveq r0, #4
	ldmeqia sp!, {r3, pc}
	beq _022321A0
	ldr r1, [r3]
	cmp r1, #0xa
	moveq r0, #7
	ldmeqia sp!, {r3, pc}
_022321A0:
	cmp r3, #0
	beq _022321C0
	ldr r1, _022321DC ; =_022577C4
	ldr r1, [r1]
	ldr r1, [r1]
	cmp r1, #8
	movne r0, #4
	ldmneia sp!, {r3, pc}
_022321C0:
	mov r1, r0
	add r0, r3, #0x144
	bl ov45_02238B70
	cmp r0, #0
	movne r0, #0
	moveq r0, #3
	ldmia sp!, {r3, pc}
	.balign 4, 0
_022321DC: .word _022577C4
	arm_func_end ov45_02232170

	arm_func_start ov45_022321E0
ov45_022321E0: ; 0x022321E0
	stmdb sp!, {r3, r4, lr}
	sub sp, sp, #0xc
	ldr r0, _022322A0 ; =_022577C4
	ldr r1, [r0]
	cmp r1, #0
	addeq sp, sp, #0xc
	moveq r0, #3
	ldmeqia sp!, {r3, r4, pc}
	beq _02232218
	ldr r0, [r1]
	cmp r0, #0xa
	addeq sp, sp, #0xc
	moveq r0, #3
	ldmeqia sp!, {r3, r4, pc}
_02232218:
	cmp r1, #0
	beq _0223223C
	ldr r0, _022322A0 ; =_022577C4
	ldr r0, [r0]
	ldr r0, [r0]
	cmp r0, #8
	addne sp, sp, #0xc
	movne r0, #3
	ldmneia sp!, {r3, r4, pc}
_0223223C:
	ldr r0, [r1, #0xd0]
	cmp r0, #5
	movne r0, #1
	moveq r0, #0
	cmp r0, #0
	addeq sp, sp, #0xc
	moveq r0, #0
	ldmeqia sp!, {r3, r4, pc}
	add r0, sp, #0
	add r1, r1, #0xd0
	bl ov45_022320B4
	ldr r0, [sp]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r0, sp, #1
	ldrne r0, [sp, #8]
	bl ov45_02240A80
	mov r4, r0
	add r0, sp, #0
	bl ov45_02237BC8
	cmp r4, #0
	movne r0, #2
	moveq r0, #1
	add sp, sp, #0xc
	ldmia sp!, {r3, r4, pc}
	.balign 4, 0
_022322A0: .word _022577C4
	arm_func_end ov45_022321E0

	arm_func_start ov45_022322A4
ov45_022322A4: ; 0x022322A4
	stmdb sp!, {r3, lr}
	ldr ip, _02232300 ; =_022577C4
	ldr lr, [ip]
	cmp lr, #0
	moveq r0, #4
	ldmeqia sp!, {r3, pc}
	beq _022322D0
	ldr ip, [lr]
	cmp ip, #0xa
	moveq r0, #7
	ldmeqia sp!, {r3, pc}
_022322D0:
	cmp lr, #0
	beq _022322F0
	ldr ip, _02232300 ; =_022577C4
	ldr ip, [ip]
	ldr ip, [ip]
	cmp ip, #8
	movne r0, #4
	ldmneia sp!, {r3, pc}
_022322F0:
	ldr ip, [sp, #8]
	str ip, [sp]
	bl ov45_02233434
	ldmia sp!, {r3, pc}
	.balign 4, 0
_02232300: .word _022577C4
	arm_func_end ov45_022322A4

	arm_func_start ov45_02232304
ov45_02232304: ; 0x02232304
	stmdb sp!, {r3, r4, r5, r6, lr}
	sub sp, sp, #0x14
	ldr r4, _02232428 ; =_022577C4
	mov r6, r0
	ldr ip, [r4]
	mov r5, r2
	cmp ip, #0
	mov r4, r3
	addeq sp, sp, #0x14
	moveq r0, #4
	ldmeqia sp!, {r3, r4, r5, r6, pc}
	beq _02232348
	ldr r0, [ip]
	cmp r0, #0xa
	addeq sp, sp, #0x14
	moveq r0, #7
	ldmeqia sp!, {r3, r4, r5, r6, pc}
_02232348:
	cmp ip, #0
	beq _0223236C
	ldr r0, _02232428 ; =_022577C4
	ldr r0, [r0]
	ldr r0, [r0]
	cmp r0, #8
	addne sp, sp, #0x14
	movne r0, #4
	ldmneia sp!, {r3, r4, r5, r6, pc}
_0223236C:
	mov r2, r1
	add r0, sp, #8
	add r1, ip, #0xb8
	bl ov45_02232434
	ldr r1, _0223242C ; =ov45_022550B4
	add r0, sp, #8
	bl ov45_02237FD8
	cmp r0, #0
	moveq r0, #1
	movne r0, #0
	cmp r0, #0
	beq _022323B0
	add r0, sp, #8
	bl ov45_02237BC8
	add sp, sp, #0x14
	mov r0, #5
	ldmia sp!, {r3, r4, r5, r6, pc}
_022323B0:
	ldr r0, [sp, #8]
	ldr r3, _02232430 ; =ov45_02236308
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r0, sp, #9
	ldrne r0, [sp, #0x10]
	mov r1, r6
	mov r2, r5
	str r4, [sp]
	bl ov45_022417B4
	mov r4, r0
	ldr r1, _02232428 ; =_022577C4
	mov r0, #0
	strb r0, [sp, #4]
	cmp r4, #1
	ldr r1, [r1]
	beq _02232400
	cmp r4, #2
	moveq r0, #3
	b _02232404
_02232400:
	mov r0, #2
_02232404:
	cmp r0, #0
	strne r0, [r1, #4]
	movne r0, #0xa
	strne r0, [r1]
	add r0, sp, #8
	bl ov45_02237BC8
	mov r0, r4
	add sp, sp, #0x14
	ldmia sp!, {r3, r4, r5, r6, pc}
	.balign 4, 0
_02232428: .word _022577C4
_0223242C: .word ov45_022550B4
_02232430: .word ov45_02236308
	arm_func_end ov45_02232304

	arm_func_start ov45_02232434
ov45_02232434: ; 0x02232434
	stmdb sp!, {r3, r4, lr}
	sub sp, sp, #0xc
	mov r4, r0
	cmp r2, #0
	bne _02232460
	bl ov45_022320B4
	add r3, sp, #0
	ldmia r4, {r0, r1, r2}
	add sp, sp, #0xc
	stmia r3, {r0, r1, r2}
	ldmia sp!, {r3, r4, pc}
_02232460:
	add r1, r1, #0x18
	bl ov45_022320B4
	add r3, sp, #0
	ldmia r4, {r0, r1, r2}
	stmia r3, {r0, r1, r2}
	add sp, sp, #0xc
	ldmia sp!, {r3, r4, pc}
	arm_func_end ov45_02232434

	arm_func_start ov45_0223247C
ov45_0223247C: ; 0x0223247C
	stmdb sp!, {lr}
	sub sp, sp, #0xc
	ldr ip, _022324FC ; =_022577C4
	ldr lr, [ip]
	cmp lr, #0
	addeq sp, sp, #0xc
	moveq r0, #4
	ldmeqia sp!, {pc}
	beq _022324B4
	ldr ip, [lr]
	cmp ip, #0xa
	addeq sp, sp, #0xc
	moveq r0, #7
	ldmeqia sp!, {pc}
_022324B4:
	cmp lr, #0
	beq _022324D8
	ldr ip, _022324FC ; =_022577C4
	ldr ip, [ip]
	ldr ip, [ip]
	cmp ip, #8
	addne sp, sp, #0xc
	movne r0, #4
	ldmneia sp!, {pc}
_022324D8:
	stmia sp, {r1, r2}
	mov r1, r0
	mov r2, #1
	str r3, [sp, #8]
	mov r3, r2
	mvn r0, #0
	bl ov45_0223325C
	add sp, sp, #0xc
	ldmia sp!, {pc}
	.balign 4, 0
_022324FC: .word _022577C4
	arm_func_end ov45_0223247C

	arm_func_start ov45_02232500
ov45_02232500: ; 0x02232500
	stmdb sp!, {lr}
	sub sp, sp, #0xc
	ldr ip, _0223257C ; =_022577C4
	ldr lr, [ip]
	cmp lr, #0
	addeq sp, sp, #0xc
	moveq r0, #4
	ldmeqia sp!, {pc}
	beq _02232538
	ldr ip, [lr]
	cmp ip, #0xa
	addeq sp, sp, #0xc
	moveq r0, #7
	ldmeqia sp!, {pc}
_02232538:
	cmp lr, #0
	beq _0223255C
	ldr ip, _0223257C ; =_022577C4
	ldr ip, [ip]
	ldr ip, [ip]
	cmp ip, #8
	addne sp, sp, #0xc
	movne r0, #4
	ldmneia sp!, {pc}
_0223255C:
	stmia sp, {r1, r2}
	mov r2, #1
	str r3, [sp, #8]
	mov r3, r2
	mov r1, #5
	bl ov45_0223325C
	add sp, sp, #0xc
	ldmia sp!, {pc}
	.balign 4, 0
_0223257C: .word _022577C4
	arm_func_end ov45_02232500

	arm_func_start ov45_02232580
ov45_02232580: ; 0x02232580
	ldr r0, _02232598 ; =_022577C4
	ldr r0, [r0]
	cmp r0, #0
	ldrne r0, [r0, #0x4c]
	mvneq r0, #0
	bx lr
	.balign 4, 0
_02232598: .word _022577C4
	arm_func_end ov45_02232580

	arm_func_start ov45_0223259C
ov45_0223259C: ; 0x0223259C
	ldr r1, [r0, #4]
	cmp r1, #0
	moveq r0, #0
	ldrne r0, [r0]
	bx lr
	arm_func_end ov45_0223259C

	arm_func_start ov45_022325B0
ov45_022325B0: ; 0x022325B0
	stmdb sp!, {r3, r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0x3c
	ldr r3, _0223275C ; =_022577C4
	mov r6, r2
	ldr r5, [r3]
	cmp r5, #0
	addeq sp, sp, #0x3c
	moveq r0, #4
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, pc}
	beq _022325EC
	ldr r2, [r5]
	cmp r2, #0xa
	addeq sp, sp, #0x3c
	moveq r0, #7
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, pc}
_022325EC:
	cmp r5, #0
	beq _02232610
	ldr r2, _0223275C ; =_022577C4
	ldr r2, [r2]
	ldr r2, [r2]
	cmp r2, #8
	addne sp, sp, #0x3c
	movne r0, #4
	ldmneia sp!, {r3, r4, r5, r6, r7, r8, pc}
_02232610:
	mov r2, #0
	str r2, [sp, #0x20]
	add r4, r0, r1
	str r2, [sp, #0x24]
	str r2, [sp, #0x28]
	strb r2, [sp, #9]
	strb r2, [sp, #0xa]
	strb r2, [sp, #8]
	strb r2, [sp, #4]
	sub r2, sp, #4
	ldr r1, [sp, #0x1c]
	str r4, [r2]
	str r1, [r2, #4]
	add ip, sp, #0x20
	str r0, [sp, #0x10]
	ldr r3, [r2]
	add r1, sp, #0x10
	mov r0, ip
	str r4, [sp, #0x18]
	ldmia r1, {r1, r2}
	bl ov45_022327B8
	mov r2, #0
	ldrb r0, [sp, #0xc]
	ldr r1, [sp, #0x20]
	ldr ip, [sp, #0x24]
	str r2, [sp, #0x2c]
	str r2, [sp, #0x30]
	str r2, [sp, #0x34]
	sub r2, sp, #4
	strb r0, [r2]
	add r4, sp, #0x2c
	ldr r3, [r2]
	mov r0, r4
	add r2, r1, ip
	bl ov45_022383E0
	mov r0, #1
	cmp r6, #0
	str r0, [r4, #0xc]
	strne r0, [r5, #0x11c]
	mov r6, r4
	ldr r8, [r5, #0x114]
	ldr r0, [r6, #4]
	mov r7, #0
	cmp r8, r0
	bne _022326FC
	cmp r8, #0
	beq _022326F8
	add r0, r5, #0x110
	bl ov45_02231C78
	mov r4, r0
	mov r0, r6
	bl ov45_02231C78
	mov r1, r0
	mov r0, r4
	mov r2, r8
	bl memcmp
	cmp r0, #0
	bne _022326FC
_022326F8:
	mov r7, #1
_022326FC:
	cmp r7, #0
	bne _02232738
	add r0, r5, #0x110
	cmp r0, r6
	beq _02232730
	mov r3, #0
	ldmia r6, {r1, r4}
	strb r3, [sp, #0xb]
	sub r2, sp, #4
	strb r3, [r2]
	ldr r3, [r2]
	add r2, r1, r4
	bl ov45_02232760
_02232730:
	mov r0, #1
	str r0, [r5, #0x11c]
_02232738:
	add r0, sp, #0x2c
	bl ov45_022379D4
	mov r1, #1
	add r0, sp, #0x20
	str r1, [r5, #0x12c]
	bl ov45_022379D4
	mov r0, #0
	add sp, sp, #0x3c
	ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
	.balign 4, 0
_0223275C: .word _022577C4
	arm_func_end ov45_022325B0

	arm_func_start ov45_02232760
ov45_02232760: ; 0x02232760
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	mov r7, r0
	mov r6, r1
	mov r5, r2
	ldr r1, [r7, #8]
	sub r4, r5, r6
	cmp r4, r1
	bls _022327A0
	mov r2, #0
	sub r1, r4, r1
	strb r2, [sp]
	bl ov45_02237890
	mov r2, r0
	mov r0, r7
	mov r1, r4
	bl ov45_02237820
_022327A0:
	ldr r0, [r7]
	mov r1, r6
	sub r2, r5, r6
	bl memmove
	str r4, [r7, #4]
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	arm_func_end ov45_02232760

	arm_func_start ov45_022327B8
ov45_022327B8: ; 0x022327B8
	stmdb sp!, {r0, r1, r2, r3}
	stmdb sp!, {r4, r5, lr}
	sub sp, sp, #0xc
	mov r5, r0
	ldr r3, [sp, #0x24]
	ldr r2, [sp, #0x1c]
	ldr r1, [r5, #8]
	sub r4, r3, r2
	cmp r4, r1
	bls _02232800
	mov r2, #0
	sub r1, r4, r1
	strb r2, [sp, #2]
	bl ov45_02237890
	mov r2, r0
	mov r0, r5
	mov r1, r4
	bl ov45_02237820
_02232800:
	ldr r1, [sp, #0x1c]
	ldr r0, [sp, #0x20]
	ldr r3, [sp, #0x24]
	str r1, [sp, #4]
	str r0, [sp, #8]
	cmp r1, r3
	ldr ip, [r5]
	beq _02232848
	mov r2, #0
_02232824:
	ldr r0, [sp, #4]
	strb r2, [sp]
	ldrb r1, [r0], #1
	str r0, [sp, #4]
	cmp r0, r3
	strb r1, [sp, #1]
	ldrsb r1, [sp, #1]
	strb r1, [ip], #1
	bne _02232824
_02232848:
	str r4, [r5, #4]
	add sp, sp, #0xc
	ldmia sp!, {r4, r5, lr}
	add sp, sp, #0x10
	bx lr
	arm_func_end ov45_022327B8

	arm_func_start ov45_0223285C
ov45_0223285C: ; 0x0223285C
	stmdb sp!, {r4, r5, lr}
	sub sp, sp, #0x94
	ldr r1, _02232AD8 ; =_022577C4
	mov r5, r0
	ldr r1, [r1]
	cmp r1, #0
	addeq sp, sp, #0x94
	moveq r0, #4
	ldmeqia sp!, {r4, r5, pc}
	beq _02232898
	ldr r0, [r1]
	cmp r0, #0xa
	addeq sp, sp, #0x94
	moveq r0, #7
	ldmeqia sp!, {r4, r5, pc}
_02232898:
	cmp r1, #0
	beq _022328BC
	ldr r0, _02232AD8 ; =_022577C4
	ldr r0, [r0]
	ldr r0, [r0]
	cmp r0, #8
	addne sp, sp, #0x94
	movne r0, #4
	ldmneia sp!, {r4, r5, pc}
_022328BC:
	mov r1, #0
	add r0, sp, #0x88
	mov r2, r1
_022328C8:
	str r2, [r0, r1, lsl #2]
	add r1, r1, #1
	cmp r1, #3
	blo _022328C8
	ldr r0, _02232AD8 ; =_022577C4
	ldr r4, [r0]
	ldr r0, [r4, #0x68]
	cmp r0, #0
	bne _022329D8
	mov r0, #1
	str r0, [r4, #0x68]
	ldr r0, [r5]
	add ip, r5, #0x10
	str r0, [r4, #0x74]
	ldr r0, [r5, #4]
	add r3, r4, #0x84
	str r0, [r4, #0x78]
	ldr r0, [r5, #8]
	mov r2, #0x10
	str r0, [r4, #0x7c]
	ldr r0, [r5, #0xc]
	str r0, [r4, #0x80]
_02232920:
	ldrb r1, [ip]
	ldrb r0, [ip, #1]
	add ip, ip, #2
	subs r2, r2, #1
	strb r1, [r3]
	strb r0, [r3, #1]
	add r3, r3, #2
	bne _02232920
	add ip, r5, #0x30
	add r3, r4, #0xa4
	mov r2, #0xa
_0223294C:
	ldrb r1, [ip]
	ldrb r0, [ip, #1]
	add ip, ip, #2
	subs r2, r2, #1
	strb r1, [r3]
	strb r0, [r3, #1]
	add r3, r3, #2
	bne _0223294C
	bl ov00_021FF368
	ldr r1, _02232ADC ; =0x10624DD3
	mov r3, #0x3e8
	umull r1, r2, r0, r1
	mov r2, r2, lsr #6
	umull r1, r2, r3, r2
	sub r2, r0, r1
	str r2, [sp]
	ldr r3, [r4, #0x4c]
	ldr r2, _02232AE0 ; =ov45_02254F88
	add r0, sp, #0x7c
	mov r1, #0x14
	bl ov45_02232B24
	add r0, sp, #0x88
	add r1, sp, #0x7c
	bl ov45_02237D48
	add r0, sp, #0x7c
	bl ov45_02237BC8
	ldr r0, [sp, #0x88]
	mov r2, #0x13
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r1, sp, #0x89
	ldrne r1, [sp, #0x90]
	add r0, r4, #0xa4
	bl strncpy
	mov r2, #1
_022329D8:
	cmp r2, #0
	bne _022329F4
	add r0, sp, #0x88
	bl ov45_02237BC8
	add sp, sp, #0x94
	mov r0, #4
	ldmia sp!, {r4, r5, pc}
_022329F4:
	ldr r0, [sp, #0x88]
	mov r2, #0x13
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r1, sp, #0x89
	ldrne r1, [sp, #0x90]
	add r0, r5, #0x30
	bl strncpy
	ldr r2, _02232AD8 ; =_022577C4
	add r0, sp, #0x6c
	add r1, sp, #0x60
	ldr r4, [r2]
	bl ov00_021ECB94
	add r0, sp, #0x6c
	add r1, sp, #0x60
	bl RTC_ConvertDateTimeToSecond
	mov r2, #0
	subs r0, r0, #0x15
	str r0, [sp, #0x10]
	sbc r0, r1, #0
	str r0, [sp, #0x14]
	ldr r0, _02232AE4 ; =ov45_02234814
	mov r1, #0x15
	str r1, [sp, #0x18]
	str r0, [sp, #0x24]
	str r2, [sp, #0x1c]
	str r2, [sp, #0x20]
	add r0, sp, #0x28
	mov r1, #3
	add r2, sp, #0x10
	bl ov45_02232AE8
	mov r0, #0
	add r2, sp, #0x2c
	add lr, sp, #0x48
	add r1, r4, #0x134
	strb r0, [sp, #5]
	strb r0, [sp, #4]
	ldr r0, [sp, #0x28]
	add ip, r2, #8
	str r0, [sp, #0x44]
	ldr r4, [r2, #0x10]
	ldr r0, [r2, #0x14]
	ldmia r2, {r2, r3}
	stmia lr, {r2, r3}
	add r5, lr, #8
	ldmia ip, {r2, r3}
	stmia r5, {r2, r3}
	str r0, [lr, #0x14]
	add r0, sp, #8
	add r2, sp, #0x44
	str r4, [lr, #0x10]
	bl ov45_02236BA8
	add r0, sp, #0x88
	bl ov45_02237BC8
	mov r0, #0
	add sp, sp, #0x94
	ldmia sp!, {r4, r5, pc}
	.balign 4, 0
_02232AD8: .word _022577C4
_02232ADC: .word 0x10624DD3
_02232AE0: .word ov45_02254F88
_02232AE4: .word ov45_02234814
	arm_func_end ov45_0223285C

	arm_func_start ov45_02232AE8
ov45_02232AE8: ; 0x02232AE8
	stmdb sp!, {r3, lr}
	mov lr, r2
	str r1, [r0]
	add ip, r0, #4
	ldmia lr, {r2, r3}
	stmia ip, {r2, r3}
	add r1, lr, #8
	add r0, ip, #8
	ldmia r1, {r2, r3}
	stmia r0, {r2, r3}
	ldr r0, [lr, #0x10]
	str r0, [ip, #0x10]
	ldr r0, [lr, #0x14]
	str r0, [ip, #0x14]
	ldmia sp!, {r3, pc}
	arm_func_end ov45_02232AE8

	arm_func_start ov45_02232B24
ov45_02232B24: ; 0x02232B24
	stmdb sp!, {r0, r1, r2, r3}
	stmdb sp!, {r4, r5, lr}
	sub sp, sp, #0xc
	mov r2, #0
	mov r5, r0
	add r0, sp, #0
	mov r4, r1
	str r2, [sp]
	str r2, [sp, #4]
	str r2, [sp, #8]
	bl ov45_022377AC
	add r0, sp, #0
	bl ov45_02232B9C
	add r1, sp, #0x20
	bic r3, r1, #3
	ldr r2, [sp, #0x20]
	mov r1, r4
	add r3, r3, #4
	bl OS_VSNPrintf
	add r0, sp, #0
	bl ov45_02232B9C
	mov r1, r0
	mov r0, r5
	bl ov45_02237B58
	add r0, sp, #0
	bl ov45_022379D4
	add sp, sp, #0xc
	ldmia sp!, {r4, r5, lr}
	add sp, sp, #0x10
	bx lr
	arm_func_end ov45_02232B24

	arm_func_start ov45_02232B9C
ov45_02232B9C: ; 0x02232B9C
	ldr r1, [r0, #4]
	cmp r1, #0
	moveq r0, #0
	ldrne r0, [r0]
	bx lr
	arm_func_end ov45_02232B9C

	arm_func_start ov45_02232BB0
ov45_02232BB0: ; 0x02232BB0
	stmdb sp!, {r4, lr}
	ldr r1, _02232CA0 ; =_022577C4
	ldr r2, [r1]
	cmp r2, #0
	moveq r0, #4
	ldmeqia sp!, {r4, pc}
	beq _02232BDC
	ldr r1, [r2]
	cmp r1, #0xa
	moveq r0, #7
	ldmeqia sp!, {r4, pc}
_02232BDC:
	cmp r2, #0
	beq _02232BFC
	ldr r1, _02232CA0 ; =_022577C4
	ldr r1, [r1]
	ldr r1, [r1]
	cmp r1, #8
	movne r0, #4
	ldmneia sp!, {r4, pc}
_02232BFC:
	ldr r1, _02232CA0 ; =_022577C4
	ldr r4, [r1]
	ldr r1, [r4, #0x68]
	cmp r1, #0
	moveq r0, #0
	beq _02232C90
	ldr r1, [r0]
	add lr, r0, #0x10
	str r1, [r4, #0x74]
	ldr r1, [r0, #4]
	add ip, r4, #0x84
	str r1, [r4, #0x78]
	ldr r1, [r0, #8]
	mov r3, #0x10
	str r1, [r4, #0x7c]
	ldr r1, [r0, #0xc]
	str r1, [r4, #0x80]
_02232C40:
	ldrb r2, [lr]
	ldrb r1, [lr, #1]
	add lr, lr, #2
	subs r3, r3, #1
	strb r2, [ip]
	strb r1, [ip, #1]
	add ip, ip, #2
	bne _02232C40
	add ip, r0, #0x30
	add r3, r4, #0xa4
	mov r2, #0xa
_02232C6C:
	ldrb r1, [ip]
	ldrb r0, [ip, #1]
	add ip, ip, #2
	subs r2, r2, #1
	strb r1, [r3]
	strb r0, [r3, #1]
	add r3, r3, #2
	bne _02232C6C
	mov r0, #1
_02232C90:
	cmp r0, #0
	moveq r0, #4
	movne r0, #0
	ldmia sp!, {r4, pc}
	.balign 4, 0
_02232CA0: .word _022577C4
	arm_func_end ov45_02232BB0

	arm_func_start ov45_02232CA4
ov45_02232CA4: ; 0x02232CA4
	stmdb sp!, {r3, lr}
	sub sp, sp, #0x10
	ldr r0, _02232D88 ; =_022577C4
	ldr r1, [r0]
	cmp r1, #0
	addeq sp, sp, #0x10
	moveq r0, #4
	ldmeqia sp!, {r3, pc}
	beq _02232CDC
	ldr r0, [r1]
	cmp r0, #0xa
	addeq sp, sp, #0x10
	moveq r0, #7
	ldmeqia sp!, {r3, pc}
_02232CDC:
	cmp r1, #0
	beq _02232D00
	ldr r0, _02232D88 ; =_022577C4
	ldr r0, [r0]
	ldr r0, [r0]
	cmp r0, #8
	addne sp, sp, #0x10
	movne r0, #4
	ldmneia sp!, {r3, pc}
_02232D00:
	ldr r0, _02232D88 ; =_022577C4
	ldr r1, [r0]
	ldr r0, [r1, #0x68]
	cmp r0, #0
	mov r0, #0
	strne r0, [r1, #0x68]
	movne r0, #1
	cmp r0, #0
	addeq sp, sp, #0x10
	moveq r0, #4
	ldmeqia sp!, {r3, pc}
	ldr r0, _02232D88 ; =_022577C4
	mov r1, #0
	ldr r2, [r0]
	mov r0, #2
	str r0, [sp]
	add r0, r2, #0x74
	str r0, [sp, #4]
	mov r0, #0x44
	str r0, [sp, #8]
	ldr r0, [r2, #0x4c]
	mov r3, r1
	mov r2, #1
	bl ov45_0223325C
	ldr r0, _02232D88 ; =_022577C4
	mov r2, #3
	ldr r0, [r0]
	add r1, sp, #0xc
	add r0, r0, #0x134
	str r2, [sp, #0xc]
	bl ov45_02232D8C
	mov r0, #0
	add sp, sp, #0x10
	ldmia sp!, {r3, pc}
	.balign 4, 0
_02232D88: .word _022577C4
	arm_func_end ov45_02232CA4

	arm_func_start ov45_02232D8C
ov45_02232D8C: ; 0x02232D8C
	stmdb sp!, {r3, r4, lr}
	sub sp, sp, #0xc
	mov r4, r0
	mov r2, r1
	add r0, sp, #8
	mov r1, r4
	bl ov45_02232DDC
	ldr r2, [sp, #8]
	add r0, r4, #4
	cmp r2, r0
	addeq sp, sp, #0xc
	moveq r0, #0
	ldmeqia sp!, {r3, r4, pc}
	add r0, sp, #4
	mov r1, r4
	str r2, [sp]
	bl ov45_02236C40
	mov r0, #1
	add sp, sp, #0xc
	ldmia sp!, {r3, r4, pc}
	arm_func_end ov45_02232D8C

	arm_func_start ov45_02232DDC
ov45_02232DDC: ; 0x02232DDC
	stmdb sp!, {r4, r5, r6, lr}
	mov r5, r1
	mov r4, r2
	mov r6, r0
	ldr r2, [r5, #4]
	mov r0, r5
	mov r1, r4
	add r3, r5, #4
	bl ov45_02232E30
	add r1, r5, #4
	cmp r0, r1
	beq _02232E1C
	ldr r2, [r4]
	ldr r1, [r0, #0xc]
	cmp r2, r1
	bhs _02232E28
_02232E1C:
	add r0, r5, #4
	str r0, [r6]
	ldmia sp!, {r4, r5, r6, pc}
_02232E28:
	str r0, [r6]
	ldmia sp!, {r4, r5, r6, pc}
	arm_func_end ov45_02232DDC

	arm_func_start ov45_02232E30
ov45_02232E30: ; 0x02232E30
	cmp r2, #0
	beq _02232E58
	ldr r1, [r1]
_02232E3C:
	ldr r0, [r2, #0xc]
	cmp r0, r1
	movhs r3, r2
	ldrhs r2, [r2]
	ldrlo r2, [r2, #4]
	cmp r2, #0
	bne _02232E3C
_02232E58:
	mov r0, r3
	bx lr
	arm_func_end ov45_02232E30

	arm_func_start ov45_02232E60
ov45_02232E60: ; 0x02232E60
	stmdb sp!, {r3, r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0x334
	ldr r1, _022331D4 ; =_022577C4
	mov r5, r0
	ldr r2, [r1]
	cmp r2, #0
	addeq sp, sp, #0x334
	moveq r0, #4
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, pc}
	beq _02232E9C
	ldr r0, [r2]
	cmp r0, #0xa
	addeq sp, sp, #0x334
	moveq r0, #7
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, pc}
_02232E9C:
	cmp r5, #0
	blt _02232EAC
	cmp r5, #2
	ble _02232EB8
_02232EAC:
	add sp, sp, #0x334
	mov r0, #3
	ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
_02232EB8:
	add r1, sp, #0x5c
	add r0, r2, #0x144
	bl ov45_0223AEDC
	cmp r0, #0
	beq _02232EE0
	ldr r0, _022331D4 ; =_022577C4
	ldr r1, [r0]
	ldr r0, [r1, #0x6c]
	cmp r0, #0
	beq _02232EEC
_02232EE0:
	add sp, sp, #0x334
	mov r0, #4
	ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
_02232EEC:
	add r0, r1, #0x110
	bl ov45_02231C78
	mov r4, r0
	mov r0, #0
	add r3, sp, #0x30
	mov r1, r0
	stmia r3!, {r0, r1}
	stmia r3!, {r0, r1}
	stmia r3!, {r0, r1}
	stmia r3!, {r0, r1}
	stmia r3!, {r0, r1}
	ldr r2, _022331D4 ; =_022577C4
	str r0, [r3]
	ldr r1, [r2]
	add r0, r1, #0x110
	bl ov45_02231C78
	ldr r1, _022331D4 ; =_022577C4
	mov r2, #1
	ldr r1, [r1]
	ldr r1, [r1, #0x4c]
	strh r2, [sp, #0x20]
	str r1, [sp, #0x1c]
	ldrb r1, [r0, #0x42]
	strb r1, [sp, #0x22]
	ldrb r1, [r0, #0x39]
	add r0, sp, #0x24
	strb r1, [sp, #0x23]
	bl OS_GetMacAddress
	ldrh r8, [sp, #0x20]
	ldrb r7, [sp, #0x22]
	ldrb r6, [sp, #0x23]
	ldrb r2, [sp, #0x25]
	ldrb r3, [sp, #0x24]
	ldrb r1, [sp, #0x26]
	ldrb r0, [sp, #0x27]
	ldrb ip, [sp, #0x28]
	strh r8, [sp, #0x34]
	ldrb r8, [sp, #0x29]
	strb r7, [sp, #0x36]
	ldrh r7, [sp, #0x2a]
	strb r6, [sp, #0x37]
	ldr lr, [sp, #0x1c]
	ldr r6, [sp, #0x2c]
	strb r2, [sp, #0x39]
	ldr r2, [sp, #0x60]
	strb r3, [sp, #0x38]
	ldr r3, [sp, #0x5c]
	strb r1, [sp, #0x3a]
	strb r0, [sp, #0x3b]
	str lr, [sp, #0x30]
	strb ip, [sp, #0x3c]
	strb r8, [sp, #0x3d]
	strh r7, [sp, #0x3e]
	str r6, [sp, #0x40]
	str r2, [sp, #0x48]
	ldr r1, _022331D4 ; =_022577C4
	str r5, [sp, #0x4c]
	str r3, [sp, #0x44]
	ldr r3, [r4, #4]
	ldr r0, _022331D8 ; =_02254F58
	str r3, [sp, #0x50]
	ldr r2, [r1]
	ldrb r1, [r4, #0x38]
	ldr r5, [r0, #4]
	str r1, [sp, #0x54]
	ldrh r1, [r4, #0x3c]
	strh r1, [sp, #0x58]
	ldrb r1, [r4, #0x3e]
	strh r1, [sp, #0x5a]
	ldr r4, [r0, #0xc]
	ldr r0, _022331DC ; =ov45_022577D4
	ldr r1, [r2, #0x50]
	ldr r0, [r0]
	cmp r1, #0
	moveq r8, #0
	movne r8, #1
	ldr r7, [r2, #0x4c]
	cmp r0, #0
	bne _022331B8
	mov r0, #0x40
	bl ov45_02242394
	movs r6, r0
	beq _022330CC
	ldr r1, _022331E0 ; =ov45_02254FC0
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
	add r2, sp, #0x30
	add r4, r2, #0x2c
	mov r1, #0
	str r7, [r6, #0x24]
	str r1, [r6, #0x28]
	str r1, [r6, #0x2c]
	ldr r0, _022331E4 ; =ov45_022354AC
	str r1, [r6, #0x30]
	str r0, [r6, #0x34]
	str r1, [r6, #0x38]
	str r1, [r6, #0x3c]
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
_022330CC:
	ldr r0, _022331DC ; =ov45_022577D4
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
	beq _0223313C
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
	b _02233170
_0223313C:
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
_02233170:
	cmp r0, #0
	moveq r0, #1
	beq _02233184
	bl ov45_022546F8
	mov r0, #0
_02233184:
	cmp r0, #0
	bne _022331B8
	ldr r0, _022331DC ; =ov45_022577D4
	ldr r0, [r0]
	cmp r0, #0
	beq _022331B8
	beq _022331AC
	ldr r1, [r0]
	ldr r1, [r1, #4]
	blx r1
_022331AC:
	ldr r0, _022331DC ; =ov45_022577D4
	mov r1, #0
	str r1, [r0]
_022331B8:
	ldr r0, _022331D4 ; =_022577C4
	mov r2, #1
	ldr r1, [r0]
	mov r0, #0
	str r2, [r1, #0x6c]
	add sp, sp, #0x334
	ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
	.balign 4, 0
_022331D4: .word _022577C4
_022331D8: .word _02254F58
_022331DC: .word ov45_022577D4
_022331E0: .word ov45_02254FC0
_022331E4: .word ov45_022354AC
	arm_func_end ov45_02232E60

	arm_func_start ov45_022331E8
ov45_022331E8: ; 0x022331E8
	ldr r0, _02233200 ; =_022577C4
	ldr r0, [r0]
	cmp r0, #0
	ldrne r0, [r0, #4]
	moveq r0, #0
	bx lr
	.balign 4, 0
_02233200: .word _022577C4
	arm_func_end ov45_022331E8

	arm_func_start ov45_02233204
ov45_02233204: ; 0x02233204
	stmdb sp!, {r3, lr}
	cmp r0, #4
	addge r0, r0, #0x17c
	addge r0, r0, #0x2800
	ldmgeia sp!, {r3, pc}
	bl ov45_02240A74
	ldmia sp!, {r3, pc}
	arm_func_end ov45_02233204

	arm_func_start ov45_02233220
ov45_02233220: ; 0x02233220
	cmp r0, #0
	beq _0223323C
	cmp r0, #1
	beq _02233244
	cmp r0, #2
	beq _0223324C
	b _02233254
_0223323C:
	mov r0, #0
	bx lr
_02233244:
	mov r0, #2
	bx lr
_0223324C:
	mov r0, #3
	bx lr
_02233254:
	mov r0, #1
	bx lr
	arm_func_end ov45_02233220

	arm_func_start ov45_0223325C
ov45_0223325C: ; 0x0223325C
	stmdb sp!, {r4, r5, lr}
	sub sp, sp, #0x24
	mov r5, r0
	mvn r0, #0
	mov r4, r1
	cmp r5, r0
	cmpeq r4, #5
	addeq sp, sp, #0x24
	moveq r0, #3
	ldmeqia sp!, {r4, r5, pc}
	mov ip, #0
	add r1, sp, #0x18
	mov r0, ip
_02233290:
	str r0, [r1, ip, lsl #2]
	add ip, ip, #1
	cmp ip, #3
	blo _02233290
	ldr lr, [sp, #0x38]
	mov r0, r2
	mov r1, r3
	ldr r2, [sp, #0x30]
	ldr r3, [sp, #0x34]
	add ip, sp, #0x18
	str lr, [sp]
	str ip, [sp, #4]
	bl ov45_0223D7D4
	cmp r0, #0
	bne _022332E0
	add r0, sp, #0x18
	bl ov45_02237BC8
	add sp, sp, #0x24
	mov r0, #3
	ldmia sp!, {r4, r5, pc}
_022332E0:
	cmp r4, #5
	bne _02233354
	ldr r0, [sp, #0x18]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r1, sp, #0x19
	ldrne r1, [sp, #0x20]
	mov r0, r5
	bl ov45_022414C0
	mov r4, r0
	ldr r1, _0223342C ; =_022577C4
	mov r0, #0
	strb r0, [sp, #9]
	cmp r4, #1
	ldr r1, [r1]
	beq _0223332C
	cmp r4, #2
	moveq r0, #3
	b _02233330
_0223332C:
	mov r0, #2
_02233330:
	cmp r0, #0
	strne r0, [r1, #4]
	movne r0, #0xa
	strne r0, [r1]
	add r0, sp, #0x18
	bl ov45_02237BC8
	add sp, sp, #0x24
	mov r0, r4
	ldmia sp!, {r4, r5, pc}
_02233354:
	ldr r1, _0223342C ; =_022577C4
	add r0, sp, #0xc
	ldr r1, [r1]
	mov r2, r4
	add r1, r1, #0xb8
	bl ov45_02232434
	ldr r1, _02233430 ; =ov45_022550B4
	add r0, sp, #0xc
	bl ov45_02237FD8
	cmp r0, #0
	moveq r0, #1
	movne r0, #0
	cmp r0, #0
	beq _022333A8
	add r0, sp, #0xc
	bl ov45_02237BC8
	add r0, sp, #0x18
	bl ov45_02237BC8
	add sp, sp, #0x24
	mov r0, #5
	ldmia sp!, {r4, r5, pc}
_022333A8:
	ldr r0, [sp, #0x18]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	ldr r0, [sp, #0xc]
	addeq r1, sp, #0x19
	mov r0, r0, lsl #0x1f
	ldrne r1, [sp, #0x20]
	movs r0, r0, lsr #0x1f
	addeq r0, sp, #0xd
	ldrne r0, [sp, #0x14]
	bl ov45_022413D4
	mov r4, r0
	ldr r1, _0223342C ; =_022577C4
	mov r0, #0
	strb r0, [sp, #8]
	cmp r4, #1
	ldr r1, [r1]
	beq _022333FC
	cmp r4, #2
	moveq r0, #3
	b _02233400
_022333FC:
	mov r0, #2
_02233400:
	cmp r0, #0
	strne r0, [r1, #4]
	movne r0, #0xa
	strne r0, [r1]
	add r0, sp, #0xc
	bl ov45_02237BC8
	add r0, sp, #0x18
	bl ov45_02237BC8
	mov r0, r4
	add sp, sp, #0x24
	ldmia sp!, {r4, r5, pc}
	.balign 4, 0
_0223342C: .word _022577C4
_02233430: .word ov45_022550B4
	arm_func_end ov45_0223325C

	arm_func_start ov45_02233434
ov45_02233434: ; 0x02233434
	stmdb sp!, {r3, r4, r5, r6, lr}
	sub sp, sp, #0x1c
	ldr r4, _02233594 ; =_022577C4
	mov r6, r0
	ldr r4, [r4]
	mov r5, r2
	mov r2, r1
	add r1, r4, #0xb8
	add r0, sp, #0x10
	mov r4, r3
	bl ov45_02232434
	ldr r1, _02233598 ; =ov45_022550B4
	add r0, sp, #0x10
	bl ov45_02237FD8
	cmp r0, #0
	moveq r0, #1
	movne r0, #0
	cmp r0, #0
	beq _02233494
	add r0, sp, #0x10
	bl ov45_02237BC8
	add sp, sp, #0x1c
	mov r0, #5
	ldmia sp!, {r3, r4, r5, r6, pc}
_02233494:
	ldr r1, [sp, #0x30]
	cmp r1, #0x12c
	bls _022334B4
	add r0, sp, #0x10
	bl ov45_02237BC8
	add sp, sp, #0x1c
	mov r0, #3
	ldmia sp!, {r3, r4, r5, r6, pc}
_022334B4:
	ldr r0, _0223359C ; =0xAAAAAAAB
	add r1, r1, #2
	umull r0, r2, r1, r0
	mov r3, #0
	mov r2, r2, lsr #1
	mov r1, r2, lsl #2
	add r0, sp, #4
	add r1, r1, #1
	str r3, [sp, #4]
	str r3, [sp, #8]
	str r3, [sp, #0xc]
	bl ov45_022377AC
	add r0, sp, #4
	bl ov45_02232B9C
	mov r2, r0
	ldr r1, [sp, #0x30]
	ldr r3, [sp, #8]
	mov r0, r4
	bl ov00_021EFF30
	ldr r1, [sp, #4]
	mov r2, #0
	strb r2, [r1, r0]
	ldr r0, [sp, #0x10]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r4, sp, #0x11
	ldrne r4, [sp, #0x18]
	add r0, sp, #4
	bl ov45_02232B9C
	mov r3, r0
	mov r0, r4
	mov r1, r6
	mov r2, r5
	bl ov45_022415CC
	mov r4, r0
	ldr r1, _02233594 ; =_022577C4
	mov r0, #0
	strb r0, [sp]
	cmp r4, #1
	ldr r1, [r1]
	beq _02233564
	cmp r4, #2
	moveq r0, #3
	b _02233568
_02233564:
	mov r0, #2
_02233568:
	cmp r0, #0
	strne r0, [r1, #4]
	movne r0, #0xa
	strne r0, [r1]
	add r0, sp, #4
	bl ov45_022379D4
	add r0, sp, #0x10
	bl ov45_02237BC8
	mov r0, r4
	add sp, sp, #0x1c
	ldmia sp!, {r3, r4, r5, r6, pc}
	.balign 4, 0
_02233594: .word _022577C4
_02233598: .word ov45_022550B4
_0223359C: .word 0xAAAAAAAB
	arm_func_end ov45_02233434

	arm_func_start ov45_022335A0
ov45_022335A0: ; 0x022335A0
	stmdb sp!, {r3, r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0xc4
	ldr r0, _02233980 ; =_022577C4
	ldr r0, [r0]
	add r0, r0, #0x110
	bl ov45_02231C78
	ldr r1, _02233980 ; =_022577C4
	mov r4, r0
	ldr r0, [r1]
	add r0, r0, #0x110
	bl ov45_02231C78
	ldr r1, _02233980 ; =_022577C4
	mov r2, #1
	ldr r1, [r1]
	ldr r1, [r1, #0x4c]
	strh r2, [sp, #0x20]
	str r1, [sp, #0x1c]
	ldrb r1, [r0, #0x42]
	strb r1, [sp, #0x22]
	ldrb r1, [r0, #0x39]
	add r0, sp, #0x24
	strb r1, [sp, #0x23]
	bl OS_GetMacAddress
	ldr r0, [r4]
	add r3, sp, #0x38
	str r0, [sp, #0x30]
	ldr r0, [r4, #4]
	add r5, r4, #8
	str r0, [sp, #0x34]
	mov r2, #4
_02233618:
	ldrh r1, [r5]
	ldrh r0, [r5, #2]
	add r5, r5, #4
	subs r2, r2, #1
	strh r1, [r3]
	strh r0, [r3, #2]
	add r3, r3, #4
	bne _02233618
	ldr r1, [r4, #0x18]
	add r0, sp, #0x74
	str r1, [sp, #0x48]
	ldr r2, [r4, #0x1c]
	add r1, r4, #0x44
	str r2, [sp, #0x4c]
	ldrh r3, [r4, #0x20]
	ldrh r2, [r4, #0x22]
	add r6, sp, #0x7c
	add r7, r4, #0x4c
	strh r3, [sp, #0x50]
	strh r2, [sp, #0x52]
	ldrh r3, [r4, #0x24]
	ldrh r2, [r4, #0x26]
	mov r5, #6
	strh r3, [sp, #0x54]
	strh r2, [sp, #0x56]
	ldrh r3, [r4, #0x28]
	ldrh r2, [r4, #0x2a]
	strh r3, [sp, #0x58]
	strh r2, [sp, #0x5a]
	ldrb r3, [r4, #0x2c]
	ldrb r2, [r4, #0x2d]
	strb r3, [sp, #0x5c]
	strb r2, [sp, #0x5d]
	ldrb r3, [r4, #0x2e]
	ldrb r2, [r4, #0x2f]
	strb r3, [sp, #0x5e]
	strb r2, [sp, #0x5f]
	ldrb r3, [r4, #0x30]
	ldrb r2, [r4, #0x31]
	strb r3, [sp, #0x60]
	strb r2, [sp, #0x61]
	ldrb r3, [r4, #0x32]
	ldrb r2, [r4, #0x33]
	strb r3, [sp, #0x62]
	strb r2, [sp, #0x63]
	ldrb r3, [r4, #0x34]
	ldrb r2, [r4, #0x35]
	strb r3, [sp, #0x64]
	strb r2, [sp, #0x65]
	ldrb r3, [r4, #0x36]
	ldrb r2, [r4, #0x37]
	strb r3, [sp, #0x66]
	strb r2, [sp, #0x67]
	ldrb r2, [r4, #0x38]
	strb r2, [sp, #0x68]
	ldrb r2, [r4, #0x39]
	strb r2, [sp, #0x69]
	ldrh r2, [r4, #0x3a]
	strh r2, [sp, #0x6a]
	ldrh r2, [r4, #0x3c]
	strh r2, [sp, #0x6c]
	ldrb r2, [r4, #0x3e]
	strb r2, [sp, #0x6e]
	ldrb r2, [r4, #0x3f]
	strb r2, [sp, #0x6f]
	ldrb r2, [r4, #0x40]
	strb r2, [sp, #0x70]
	ldrb r2, [r4, #0x41]
	strb r2, [sp, #0x71]
	ldrb r2, [r4, #0x42]
	strb r2, [sp, #0x72]
	ldrb r2, [r4, #0x43]
	strb r2, [sp, #0x73]
	ldmia r1, {r2, r3}
	stmia r0, {r2, r3}
_02233744:
	ldrb r1, [r7]
	ldrb r0, [r7, #1]
	add r7, r7, #2
	subs r5, r5, #1
	strb r1, [r6]
	strb r0, [r6, #1]
	add r6, r6, #2
	bne _02233744
	add r6, r4, #0x58
	add r5, sp, #0x88
	ldmia r6!, {r0, r1, r2, r3}
	stmia r5!, {r0, r1, r2, r3}
	ldmia r6!, {r0, r1, r2, r3}
	stmia r5!, {r0, r1, r2, r3}
	ldmia r6, {r0, r1, r2, r3}
	stmia r5, {r0, r1, r2, r3}
	ldr r1, _02233980 ; =_022577C4
	ldr r0, _02233984 ; =_02254F58
	ldrh r6, [r4, #0x88]
	ldrh r3, [r4, #0x8a]
	ldr r2, [r1]
	ldr r5, [r0, #0x18]
	strh r6, [sp, #0xb8]
	strh r3, [sp, #0xba]
	ldr r3, [r4, #0x8c]
	ldr r1, [r4, #0x90]
	str r3, [sp, #0xbc]
	str r1, [sp, #0xc0]
	ldr r4, [r0, #0xc]
	ldr r0, _02233988 ; =ov45_022577D4
	ldr r1, [r2, #0x50]
	ldr r0, [r0]
	cmp r1, #0
	moveq r8, #0
	movne r8, #1
	cmp r0, #0
	ldr r7, [r2, #0x4c]
	addne sp, sp, #0xc4
	ldmneia sp!, {r3, r4, r5, r6, r7, r8, pc}
	mov r0, #0x40
	bl ov45_02242394
	movs r6, r0
	beq _02233884
	ldr r1, _0223398C ; =ov45_02254FC0
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
	add r2, sp, #0x1c
	add r4, r2, #0xa8
	mov r1, #0
	str r7, [r6, #0x24]
	str r1, [r6, #0x28]
	str r1, [r6, #0x2c]
	ldr r0, _02233990 ; =ov45_02234A88
	str r1, [r6, #0x30]
	str r0, [r6, #0x34]
	str r1, [r6, #0x38]
	str r1, [r6, #0x3c]
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
_02233884:
	ldr r0, _02233988 ; =ov45_022577D4
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
	beq _022338F4
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
	b _02233928
_022338F4:
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
_02233928:
	cmp r0, #0
	moveq r0, #1
	beq _0223393C
	bl ov45_022546F8
	mov r0, #0
_0223393C:
	cmp r0, #0
	addne sp, sp, #0xc4
	ldmneia sp!, {r3, r4, r5, r6, r7, r8, pc}
	ldr r0, _02233988 ; =ov45_022577D4
	ldr r0, [r0]
	cmp r0, #0
	addeq sp, sp, #0xc4
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, pc}
	beq _0223396C
	ldr r1, [r0]
	ldr r1, [r1, #4]
	blx r1
_0223396C:
	ldr r0, _02233988 ; =ov45_022577D4
	mov r1, #0
	str r1, [r0]
	add sp, sp, #0xc4
	ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
	.balign 4, 0
_02233980: .word _022577C4
_02233984: .word _02254F58
_02233988: .word ov45_022577D4
_0223398C: .word ov45_02254FC0
_02233990: .word ov45_02234A88
	arm_func_end ov45_022335A0

	arm_func_start ov45_02233994
ov45_02233994: ; 0x02233994
	stmdb sp!, {r3, r4, r5, lr}
	sub sp, sp, #0x18
	ldr r3, _022339EC ; =_02254D04
	mov r5, r0
	ldr r1, [r3, r1, lsl #2]
	add r0, sp, #0xc
	mov r4, r2
	bl ov45_02237B58
	ldr r2, _022339F0 ; =ov45_022550B8
	add r0, sp, #0
	add r1, sp, #0xc
	bl ov45_02233ABC
	add r1, sp, #0
	mov r0, r5
	mov r2, r4
	bl ov45_022339F4
	add r0, sp, #0
	bl ov45_02237BC8
	add r0, sp, #0xc
	bl ov45_02237BC8
	add sp, sp, #0x18
	ldmia sp!, {r3, r4, r5, pc}
	.balign 4, 0
_022339EC: .word _02254D04
_022339F0: .word ov45_022550B8
	arm_func_end ov45_02233994

	arm_func_start ov45_022339F4
ov45_022339F4: ; 0x022339F4
	stmdb sp!, {r3, r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0xc
	mov r3, #0
	mov r7, r0
	mov r0, r3
_02233A08:
	str r0, [r7, r3, lsl #2]
	add r3, r3, #1
	cmp r3, #3
	blo _02233A08
	ldr r0, [r1]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	ldmneib r1, {r4, r5}
	bne _02233A3C
	ldrb r0, [r1]
	add r5, r1, #1
	mov r0, r0, lsl #0x18
	mov r4, r0, lsr #0x19
_02233A3C:
	ldr r0, [r2]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	ldrne r6, [r2, #8]
	ldrne r8, [r2, #4]
	bne _02233A64
	ldrb r0, [r2]
	add r6, r2, #1
	mov r0, r0, lsl #0x18
	mov r8, r0, lsr #0x19
_02233A64:
	mov r0, r7
	add r1, r4, r8
	bl ov45_02237BFC
	add r0, r5, r4
	mov r1, #0
	ldrb ip, [sp, #9]
	str r0, [sp]
	mov r0, r7
	mov r2, r1
	mov r3, r5
	strb ip, [sp, #4]
	bl ov45_02238104
	add r1, r6, r8
	ldrb r5, [sp, #8]
	str r1, [sp]
	mov r1, r4
	mov r3, r6
	mov r2, #0
	strb r5, [sp, #4]
	bl ov45_02238104
	add sp, sp, #0xc
	ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
	arm_func_end ov45_022339F4

	arm_func_start ov45_02233ABC
ov45_02233ABC: ; 0x02233ABC
	stmdb sp!, {r3, r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0xc
	mov r4, r1
	mov r1, #0
	mov r8, r0
	mov r7, r2
	mov r0, r1
_02233AD8:
	str r0, [r8, r1, lsl #2]
	add r1, r1, #1
	cmp r1, #3
	blo _02233AD8
	mov r0, r7
	bl strlen
	ldr r1, [r4]
	mov r6, r0
	mov r0, r1, lsl #0x1f
	movs r0, r0, lsr #0x1f
	ldrne r5, [r4, #8]
	ldrne r4, [r4, #4]
	bne _02233B1C
	ldrb r0, [r4]
	add r5, r4, #1
	mov r0, r0, lsl #0x18
	mov r4, r0, lsr #0x19
_02233B1C:
	mov r0, r8
	add r1, r4, r6
	bl ov45_02237BFC
	add r0, r5, r4
	mov r1, #0
	ldrb ip, [sp, #9]
	str r0, [sp]
	mov r0, r8
	mov r2, r1
	mov r3, r5
	strb ip, [sp, #4]
	bl ov45_02238104
	add r1, r7, r6
	ldrb r5, [sp, #8]
	str r1, [sp]
	mov r1, r4
	mov r3, r7
	mov r2, #0
	strb r5, [sp, #4]
	bl ov45_02238104
	add sp, sp, #0xc
	ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
	arm_func_end ov45_02233ABC

	arm_func_start ov45_02233B74
ov45_02233B74: ; 0x02233B74
	stmdb sp!, {r3, r4, r5, lr}
	ldr r1, _02233BBC ; =_022577C4
	mov r5, r0
	ldr r0, [r1]
	ldr r4, [r0, #0x4c]
	bl ov00_021FF368
	mov ip, r0
	ldr r0, _02233BC0 ; =0x10624DD3
	mov r1, #0x3e8
	umull r0, r3, ip, r0
	mov r3, r3, lsr #6
	umull r3, r0, r1, r3
	ldr r1, _02233BC4 ; =ov45_022550BC
	mov r0, r5
	mov r2, r4
	sub r3, ip, r3
	bl ov45_02233BC8
	ldmia sp!, {r3, r4, r5, pc}
	.balign 4, 0
_02233BBC: .word _022577C4
_02233BC0: .word 0x10624DD3
_02233BC4: .word ov45_022550BC
	arm_func_end ov45_02233B74

	arm_func_start ov45_02233BC8
ov45_02233BC8: ; 0x02233BC8
	stmdb sp!, {r0, r1, r2, r3}
	stmdb sp!, {r4, r5, r6, lr}
	sub sp, sp, #0x10
	add r1, sp, #0x24
	mov r6, r0
	mov r0, #0
	bic r4, r1, #3
	ldr r2, [sp, #0x24]
	mov r1, r0
	str r0, [sp, #8]
	add r3, r4, #4
	str r0, [sp, #4]
	str r0, [sp, #0xc]
	bl OS_VSNPrintf
	ldr r3, [sp, #8]
	add r5, r0, #1
	cmp r5, r3
	bls _02233C34
	mov r1, #0
	strb r1, [sp]
	sub r0, sp, #4
	strb r1, [r0]
	ldr r2, [r0]
	add r0, sp, #4
	sub r1, r5, r3
	bl ov45_0223795C
	b _02233C4C
_02233C34:
	bhs _02233C4C
	sub r0, r3, r5
	sub r0, r3, r0
	mov r1, #0
	strb r1, [sp, #1]
	str r0, [sp, #8]
_02233C4C:
	add r0, sp, #4
	bl ov45_02232B9C
	ldr r2, [sp, #0x24]
	mov r1, r5
	add r3, r4, #4
	bl OS_VSNPrintf
	ldr r1, [sp, #4]
	add r0, sp, #4
	add r1, r1, r5
	mov r2, #0
	strb r2, [r1, #-1]
	bl ov45_02232B9C
	mov r1, r0
	mov r0, r6
	bl ov45_02237B58
	add r0, sp, #4
	bl ov45_022379D4
	add sp, sp, #0x10
	ldmia sp!, {r4, r5, r6, lr}
	add sp, sp, #0x10
	bx lr
	arm_func_end ov45_02233BC8

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

	arm_func_start ov45_0223454C
ov45_0223454C: ; 0x0223454C
	stmdb sp!, {r4, r5, r6, lr}
	mov r6, r0
	cmp r2, #0
	mov r5, r1
	mov r4, r3
	movne r0, #1
	ldmneia sp!, {r4, r5, r6, pc}
	ldr r0, [r6, #0x10]
	cmp r0, #0
	beq _02234658
	cmp r0, #1
	beq _02234588
	cmp r0, #2
	beq _022345F0
	b _0223467C
_02234588:
	ldr r0, _02234684 ; =_022577C4
	ldr r2, [r0]
	ldr r0, [r2, #0x4c]
	cmp r5, r0
	beq _0223467C
	add r0, r2, #0xe8
	bl ov45_0223DEC4
	cmp r0, #0
	ldrne r0, [r0, #0x38]
	cmpne r0, #0
	movne r0, #1
	moveq r0, #0
	cmp r0, #0
	beq _0223467C
	ldr r0, [r6, #8]
	cmp r0, #1
	ldreq r0, [sp, #0x10]
	cmpeq r0, #0x44
	bne _0223467C
	ldr r1, _02234684 ; =_022577C4
	mov r0, r5
	ldr r2, [r1]
	mov r1, r4
	ldr r2, [r2, #0x28]
	blx r2
	b _0223467C
_022345F0:
	ldr r0, _02234684 ; =_022577C4
	ldr r2, [r0]
	ldr r0, [r2, #0x4c]
	cmp r5, r0
	beq _0223467C
	add r0, r2, #0xe8
	bl ov45_0223DEC4
	cmp r0, #0
	ldrne r0, [r0, #0x38]
	cmpne r0, #0
	movne r0, #1
	moveq r0, #0
	cmp r0, #0
	beq _0223467C
	ldr r0, [r6, #8]
	cmp r0, #1
	ldreq r0, [sp, #0x10]
	cmpeq r0, #0x44
	bne _0223467C
	ldr r1, _02234684 ; =_022577C4
	mov r0, r5
	ldr r2, [r1]
	mov r1, r4
	ldr r2, [r2, #0x2c]
	blx r2
	b _0223467C
_02234658:
	ldr r0, [r6, #8]
	cmp r0, #1
	ldreq r0, [sp, #0x10]
	cmpeq r0, #4
	bne _0223467C
	ldr r0, _02234684 ; =_022577C4
	ldr r0, [r0]
	add r0, r0, #0x144
	bl ov45_0223B1C8
_0223467C:
	mov r0, #1
	ldmia sp!, {r4, r5, r6, pc}
	.balign 4, 0
_02234684: .word _022577C4
	arm_func_end ov45_0223454C

	arm_func_start ov45_02234688
ov45_02234688: ; 0x02234688
	stmdb sp!, {r3, r4, lr}
	sub sp, sp, #0xc
	ldr r1, _02234760 ; =_022577C4
	add r0, sp, #0
	ldr r1, [r1]
	add r1, r1, #0xb8
	bl ov45_022320B4
	ldr r0, [sp]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r0, sp, #1
	ldrne r0, [sp, #8]
	bl ov45_02240A80
	cmp r0, #0
	moveq r4, #1
	movne r4, #0
	add r0, sp, #0
	bl ov45_02237BC8
	cmp r4, #0
	addne sp, sp, #0xc
	movne r0, #0
	ldmneia sp!, {r3, r4, pc}
	ldr r0, _02234760 ; =_022577C4
	ldr r1, [r0]
	ldr r0, [r1, #0xc8]
	cmp r0, #0
	addeq sp, sp, #0xc
	moveq r0, #0
	ldmeqia sp!, {r3, r4, pc}
	ldr r0, [r1, #0xcc]
	cmp r0, #0
	addeq sp, sp, #0xc
	moveq r0, #0
	ldmeqia sp!, {r3, r4, pc}
	ldr r0, [r1, #0x148]
	cmp r0, #0
	addeq sp, sp, #0xc
	moveq r0, #0
	ldmeqia sp!, {r3, r4, pc}
	ldr r0, [r1, #0x144]
	cmp r0, #4
	movge r0, #1
	movlt r0, #0
	cmp r0, #0
	addeq sp, sp, #0xc
	moveq r0, #0
	ldmeqia sp!, {r3, r4, pc}
	add r0, r1, #0xe8
	bl ov45_0223E4A8
	cmp r0, #0
	movne r0, #1
	moveq r0, #0
	add sp, sp, #0xc
	ldmia sp!, {r3, r4, pc}
	.balign 4, 0
_02234760: .word _022577C4
	arm_func_end ov45_02234688

	arm_func_start ov45_02234764
ov45_02234764: ; 0x02234764
	stmdb sp!, {r3, r4, r5, lr}
	ldr r0, _022347BC ; =_022577C4
	ldr r4, [r0]
	ldr r1, [r4, #0x11c]
	cmp r1, #0
	moveq r0, #1
	ldmeqia sp!, {r3, r4, r5, pc}
	mov r1, #0
	str r1, [r4, #0x11c]
	ldr r1, [r0]
	add r0, r4, #0x110
	ldr r5, [r1, #0x4c]
	bl ov45_02231C78
	ldr ip, [r4, #0x114]
	mov r3, r0
	ldr r2, _022347C0 ; =ov45_022550C4
	mov r0, r5
	mov r1, #0
	str ip, [sp]
	bl ov45_02233434
	mov r0, #1
	ldmia sp!, {r3, r4, r5, pc}
	.balign 4, 0
_022347BC: .word _022577C4
_022347C0: .word ov45_022550C4
	arm_func_end ov45_02234764

	arm_func_start ov45_022347C4
ov45_022347C4: ; 0x022347C4
	stmdb sp!, {r3, lr}
	ldr r0, _0223480C ; =_022577C4
	ldr r3, [r0]
	ldr r1, [r3, #0x10c]
	cmp r1, #0
	moveq r0, #1
	ldmeqia sp!, {r3, pc}
	mov r1, #0
	str r1, [r3, #0x10c]
	mov r2, #0x10
	str r2, [sp]
	ldr r0, [r0]
	ldr r2, _02234810 ; =ov45_022550D4
	ldr r0, [r0, #0x4c]
	add r3, r3, #0xfc
	bl ov45_02233434
	mov r0, #1
	ldmia sp!, {r3, pc}
	.balign 4, 0
_0223480C: .word _022577C4
_02234810: .word ov45_022550D4
	arm_func_end ov45_022347C4

	arm_func_start ov45_02234814
ov45_02234814: ; 0x02234814
	stmdb sp!, {lr}
	sub sp, sp, #0xc
	ldr r0, _0223486C ; =_022577C4
	ldr r3, [r0]
	ldr r0, [r3, #0x68]
	cmp r0, #0
	addeq sp, sp, #0xc
	moveq r0, #0
	ldmeqia sp!, {pc}
	mov r2, #1
	str r2, [sp]
	add r0, r3, #0x74
	str r0, [sp, #4]
	mov r0, #0x44
	str r0, [sp, #8]
	mov r1, #0
	ldr r0, [r3, #0x4c]
	mov r3, r1
	bl ov45_0223325C
	mov r0, #1
	add sp, sp, #0xc
	ldmia sp!, {pc}
	.balign 4, 0
_0223486C: .word _022577C4
	arm_func_end ov45_02234814

	arm_func_start ov45_02234870
ov45_02234870: ; 0x02234870
	stmdb sp!, {lr}
	sub sp, sp, #0xc
	ldr r1, _022348C0 ; =_022577C4
	add r0, sp, #0
	ldr r1, [r1]
	add r1, r1, #0xb8
	bl ov45_022320B4
	ldr r0, [sp]
	ldr r1, _022348C4 ; =ov45_022367E0
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r0, sp, #1
	ldrne r0, [sp, #8]
	mov r2, #0
	bl ov45_02241E24
	add r0, sp, #0
	bl ov45_02237BC8
	mov r0, #1
	add sp, sp, #0xc
	ldmia sp!, {pc}
	.balign 4, 0
_022348C0: .word _022577C4
_022348C4: .word ov45_022367E0
	arm_func_end ov45_02234870

	arm_func_start ov45_022348C8
ov45_022348C8: ; 0x022348C8
	stmdb sp!, {r3, lr}
	sub sp, sp, #0x10
	ldr r0, _02234938 ; =_022577C4
	ldr r1, [r0]
	ldr r0, [r1, #0x148]
	cmp r0, #0
	addne sp, sp, #0x10
	movne r0, #0
	ldmneia sp!, {r3, pc}
	add r0, sp, #4
	add r1, r1, #0xb8
	bl ov45_022320B4
	ldr r0, [sp, #4]
	mov ip, #0
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r0, sp, #5
	ldrne r0, [sp, #0xc]
	ldr r2, _0223493C ; =ov45_022550E4
	ldr r3, _02234940 ; =ov45_0223683C
	sub r1, ip, #1
	str ip, [sp]
	bl ov45_022417B4
	add r0, sp, #4
	bl ov45_02237BC8
	mov r0, #1
	add sp, sp, #0x10
	ldmia sp!, {r3, pc}
	.balign 4, 0
_02234938: .word _022577C4
_0223493C: .word ov45_022550E4
_02234940: .word ov45_0223683C
	arm_func_end ov45_022348C8

	arm_func_start ov45_02234944
ov45_02234944: ; 0x02234944
	stmdb sp!, {r3, lr}
	sub sp, sp, #0x10
	ldr r0, _022349C0 ; =_022577C4
	ldr r1, [r0]
	ldr r0, [r1, #0x144]
	cmp r0, #4
	movge r0, #1
	movlt r0, #0
	cmp r0, #0
	addne sp, sp, #0x10
	movne r0, #0
	ldmneia sp!, {r3, pc}
	add r0, sp, #4
	add r1, r1, #0xb8
	bl ov45_022320B4
	ldr r0, [sp, #4]
	mov ip, #0
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r0, sp, #5
	ldrne r0, [sp, #0xc]
	ldr r2, _022349C4 ; =ov45_022550F4
	ldr r3, _022349C8 ; =ov45_0223694C
	sub r1, ip, #1
	str ip, [sp]
	bl ov45_022417B4
	add r0, sp, #4
	bl ov45_02237BC8
	mov r0, #1
	add sp, sp, #0x10
	ldmia sp!, {r3, pc}
	.balign 4, 0
_022349C0: .word _022577C4
_022349C4: .word ov45_022550F4
_022349C8: .word ov45_0223694C
	arm_func_end ov45_02234944

	arm_func_start ov45_022349CC
ov45_022349CC: ; 0x022349CC
	cmp r0, #0xa
	addls pc, pc, r0, lsl #2
	b _02234A1C
_022349D8: ; jump table
	b _02234A04 ; case 0
	b _02234A1C ; case 1
	b _02234A1C ; case 2
	b _02234A1C ; case 3
	b _02234A1C ; case 4
	b _02234A1C ; case 5
	b _02234A0C ; case 6
	b _02234A14 ; case 7
	b _02234A14 ; case 8
	b _02234A14 ; case 9
	b _02234A14 ; case 10
_02234A04:
	mov r0, #0
	bx lr
_02234A0C:
	mov r0, #1
	bx lr
_02234A14:
	mov r0, #2
	bx lr
_02234A1C:
	mov r0, #2
	bx lr
	arm_func_end ov45_022349CC

	arm_func_start ov45_02234A24
ov45_02234A24: ; 0x02234A24
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	mov r5, r3
	mov r3, #0
	cmp r0, #0
	ldr r4, [sp, #0x18]
	str r3, [r5]
	mov r7, r1
	mov r6, r2
	str r3, [r4]
	moveq r0, #2
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	cmp r6, #4
	movlo r0, #2
	ldmloia sp!, {r3, r4, r5, r6, r7, pc}
	ldrb r0, [r7]
	bl ov45_022349CC
	cmp r0, #2
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	cmp r0, #1
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	add r1, r7, #4
	str r1, [r5]
	sub r1, r6, #4
	str r1, [r4]
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	arm_func_end ov45_02234A24

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

	arm_func_start ov45_022350FC
ov45_022350FC: ; 0x022350FC
	stmdb sp!, {r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0x38
	ldr r3, _022353DC ; =_022577C4
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
	ldr r2, _022353DC ; =_022577C4
	ldr r1, [sp, #0x20]
	ldr r3, [r2]
	mov r4, r0
	ldr r2, [sp, #0x1c]
	add r0, r3, #0x144
	bl ov45_0223AE84
	cmp r0, #0
	moveq r4, #2
	cmp r4, #0
	beq _02235184
	ldr r0, _022353DC ; =_022577C4
	mov r1, #5
	ldr r2, [r0]
	mov r0, #0xa
	stmia r2, {r0, r1}
	add sp, sp, #0x38
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_02235184:
	ldr r0, _022353DC ; =_022577C4
	ldr r1, [sp, #0x20]
	ldr r0, [r0]
	ldr r2, [sp, #0x1c]
	add r0, r0, #0x144
	bl ov45_0223ACA4
	ldr r0, _022353DC ; =_022577C4
	ldr r2, [sp, #0x1c]
	ldr r0, [r0]
	ldr r1, [sp, #0x20]
	ldr r3, [r0, #0x3c]
	mov r0, r4
	mov r2, r2, lsr #3
	blx r3
	ldr r0, _022353DC ; =_022577C4
	ldr r0, [r0]
	add r0, r0, #0x110
	bl ov45_02231C78
	ldr r1, _022353DC ; =_022577C4
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
	ldr r0, _022353E0 ; =_02254F58
	ldr r1, _022353DC ; =_022577C4
	ldr r5, [r0, #8]
	ldr r1, [r1]
	ldr r4, [r0, #0xc]
	ldr r0, [r1, #0x50]
	ldr r7, [r1, #0x4c]
	cmp r0, #0
	ldr r0, _022353E4 ; =ov45_022577D4
	moveq r8, #0
	ldr r0, [r0]
	movne r8, #1
	cmp r0, #0
	addne sp, sp, #0x38
	ldmneia sp!, {r4, r5, r6, r7, r8, pc}
	mov r0, #0x40
	bl ov45_02242394
	movs r6, r0
	beq _022352E0
	ldr r1, _022353E8 ; =ov45_02254FC0
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
	ldr r0, _022353EC ; =ov45_022353F0
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
_022352E0:
	ldr r0, _022353E4 ; =ov45_022577D4
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
	beq _02235350
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
	b _02235384
_02235350:
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
_02235384:
	cmp r0, #0
	moveq r0, #1
	beq _02235398
	bl ov45_022546F8
	mov r0, #0
_02235398:
	cmp r0, #0
	addne sp, sp, #0x38
	ldmneia sp!, {r4, r5, r6, r7, r8, pc}
	ldr r0, _022353E4 ; =ov45_022577D4
	ldr r0, [r0]
	cmp r0, #0
	addeq sp, sp, #0x38
	ldmeqia sp!, {r4, r5, r6, r7, r8, pc}
	beq _022353C8
	ldr r1, [r0]
	ldr r1, [r1, #4]
	blx r1
_022353C8:
	ldr r0, _022353E4 ; =ov45_022577D4
	mov r1, #0
	str r1, [r0]
	add sp, sp, #0x38
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	.balign 4, 0
_022353DC: .word _022577C4
_022353E0: .word _02254F58
_022353E4: .word ov45_022577D4
_022353E8: .word ov45_02254FC0
_022353EC: .word ov45_022353F0
	arm_func_end ov45_022350FC

	arm_func_start ov45_022353F0
ov45_022353F0: ; 0x022353F0
	stmdb sp!, {r3, r4, lr}
	sub sp, sp, #0xc
	ldr r3, _022354A8 ; =_022577C4
	ldr r3, [r3]
	cmp r3, #0
	ldrne r3, [r3]
	cmpne r3, #9
	addeq sp, sp, #0xc
	ldmeqia sp!, {r3, r4, pc}
	mov r3, #0
	add ip, sp, #4
	str r3, [sp, #8]
	str r3, [sp, #4]
	add r3, sp, #8
	str ip, [sp]
	bl ov45_02234A24
	ldr r1, [sp, #4]
	mov r4, r0
	cmp r1, #0x2d8
	movne r4, #2
	cmp r4, #0
	beq _02235464
	ldr r0, _022354A8 ; =_022577C4
	mov r1, #7
	ldr r2, [r0]
	mov r0, #0xa
	stmia r2, {r0, r1}
	add sp, sp, #0xc
	ldmia sp!, {r3, r4, pc}
_02235464:
	ldr r0, _022354A8 ; =_022577C4
	ldr r1, [sp, #8]
	ldr r0, [r0]
	add r0, r0, #0x144
	bl ov45_0223AF04
	ldr r0, _022354A8 ; =_022577C4
	ldr r1, [sp, #8]
	ldr r2, [r0]
	mov r0, r4
	ldr r2, [r2, #0x40]
	blx r2
	ldr r0, _022354A8 ; =_022577C4
	mov r1, #2
	ldr r0, [r0]
	str r1, [r0, #0x144]
	add sp, sp, #0xc
	ldmia sp!, {r3, r4, pc}
	.balign 4, 0
_022354A8: .word _022577C4
	arm_func_end ov45_022353F0

	arm_func_start ov45_022354AC
ov45_022354AC: ; 0x022354AC
	stmdb sp!, {r3, lr}
	ldr r1, _022354F0 ; =_022577C4
	ldr r2, [r1]
	cmp r2, #0
	ldrne r1, [r2]
	cmpne r1, #9
	ldmeqia sp!, {r3, pc}
	mov r3, #0
	ldr r1, _022354F0 ; =_022577C4
	str r3, [r2, #0x6c]
	ldr r1, [r1]
	cmp r0, #0
	moveq r3, #2
	ldr r1, [r1, #0x44]
	mov r0, r3
	blx r1
	ldmia sp!, {r3, pc}
	.balign 4, 0
_022354F0: .word _022577C4
	arm_func_end ov45_022354AC

	arm_func_start ov45_022354F4
ov45_022354F4: ; 0x022354F4
	stmdb sp!, {r4, r5, r6, r7, lr}
	sub sp, sp, #0x2b4
	mov r6, r3
	cmp r6, #5
	mov r7, r2
	addeq sp, sp, #0x2b4
	ldmeqia sp!, {r4, r5, r6, r7, pc}
	cmp r0, #0
	beq _022359D0
	ldr r0, _02235A78 ; =_022577C4
	mov r4, #0
	ldr r3, [r0]
	mov r2, #5
	add r0, sp, #0x288
	add r1, sp, #0x27c
	str r4, [r3, #0x70]
	str r2, [sp, #0x298]
	str r4, [sp, #0x29c]
	str r4, [sp, #0x2a0]
	str r4, [sp, #0x2a4]
	bl ov00_021ECB94
	add r0, sp, #0x288
	add r1, sp, #0x27c
	bl RTC_ConvertDateTimeToSecond
	ldr r4, [sp, #0x298]
	str r0, [sp, #0x29c]
	str r1, [sp, #0x2a0]
	ldr r2, _02235A78 ; =_022577C4
	str r4, [sp, #0x268]
	add r3, sp, #0x29c
	ldr r4, [r2]
	add ip, sp, #0x26c
	ldmia r3, {r2, r3}
	stmia ip, {r2, r3}
	ldr r1, [sp, #0x2a4]
	mov r0, #1
	str r0, [sp, #0x278]
	add r5, sp, #0x268
	str r1, [sp, #0x274]
	mov r1, r5
	add r0, r4, #0xfc
	mov r2, #0x10
	bl memcmp
	cmp r0, #0
	moveq r0, #1
	movne r0, #0
	cmp r0, #0
	bne _022355DC
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
_022355DC:
	ldr r1, _02235A7C ; =ov45_02236690
	mov r3, #1
	mov r0, r7
	mov r2, r6
	str r3, [r4, #0x128]
	bl ov45_02241B84
	ldr r2, _02235A78 ; =_022577C4
	add r0, sp, #0x258
	add r1, sp, #0x24c
	ldr r4, [r2]
	bl ov00_021ECB94
	add r0, sp, #0x258
	add r1, sp, #0x24c
	bl RTC_ConvertDateTimeToSecond
	mov r2, #0
	subs r0, r0, #3
	str r0, [sp, #0x1fc]
	sbc r0, r1, #0
	str r0, [sp, #0x200]
	ldr r0, _02235A80 ; =ov45_022347C4
	mov r1, #3
	str r1, [sp, #0x204]
	str r0, [sp, #0x210]
	str r2, [sp, #0x208]
	str r2, [sp, #0x20c]
	add r0, sp, #0x214
	mov r1, #1
	add r2, sp, #0x1fc
	bl ov45_02232AE8
	mov r0, #0
	add r2, sp, #0x218
	add r7, sp, #0x234
	add r1, r4, #0x134
	strb r0, [sp, #9]
	strb r0, [sp, #8]
	ldr r0, [sp, #0x214]
	add r6, r2, #8
	str r0, [sp, #0x230]
	ldr r4, [r2, #0x10]
	ldr r0, [r2, #0x14]
	ldmia r2, {r2, r3}
	stmia r7, {r2, r3}
	add r5, r7, #8
	ldmia r6, {r2, r3}
	stmia r5, {r2, r3}
	str r0, [r7, #0x14]
	add r0, sp, #0x2c
	add r2, sp, #0x230
	str r4, [r7, #0x10]
	bl ov45_02236BA8
	ldr r1, _02235A78 ; =_022577C4
	add r0, sp, #0x1ec
	ldr r4, [r1]
	add r1, sp, #0x1e0
	bl ov00_021ECB94
	add r0, sp, #0x1ec
	add r1, sp, #0x1e0
	bl RTC_ConvertDateTimeToSecond
	ldr r6, _02235A84 ; =ov45_02234764
	mov r7, #0
	subs r5, r0, #4
	str r0, [sp, #0x190]
	sbc r3, r1, #0
	mov ip, #4
	add r0, sp, #0x1a8
	add r2, sp, #0x190
	mov r1, #2
	str ip, [sp, #0x198]
	str r7, [sp, #0x19c]
	str r7, [sp, #0x1a0]
	str r6, [sp, #0x1a4]
	str r5, [sp, #0x190]
	str r3, [sp, #0x194]
	bl ov45_02232AE8
	ldr r0, [sp, #0x1a8]
	add r6, sp, #0x1ac
	add r5, sp, #0x1c8
	str r0, [sp, #0x1c4]
	ldmia r6, {r2, r3}
	stmia r5, {r2, r3}
	add r2, r6, #8
	add r0, r5, #8
	ldmia r2, {r2, r3}
	stmia r0, {r2, r3}
	ldr r2, [r6, #0x10]
	ldr r3, [r6, #0x14]
	str r2, [r5, #0x10]
	add r1, r4, #0x134
	add r0, sp, #0x24
	add r2, sp, #0x1c4
	strb r7, [sp, #7]
	strb r7, [sp, #6]
	str r3, [r5, #0x14]
	bl ov45_02236BA8
	ldr r1, _02235A78 ; =_022577C4
	add r0, sp, #0x180
	ldr r4, [r1]
	add r1, sp, #0x174
	bl ov00_021ECB94
	add r0, sp, #0x180
	add r1, sp, #0x174
	bl RTC_ConvertDateTimeToSecond
	mov r2, #4
	str r2, [sp, #0x12c]
	mov r2, r7
	str r0, [sp, #0x124]
	subs r0, r0, #4
	str r0, [sp, #0x124]
	str r2, [sp, #0x130]
	str r2, [sp, #0x134]
	ldr r2, _02235A88 ; =ov45_022348C8
	str r1, [sp, #0x128]
	sbc r0, r1, #0
	str r2, [sp, #0x138]
	str r0, [sp, #0x128]
	add r0, sp, #0x13c
	mov r1, #5
	add r2, sp, #0x124
	bl ov45_02232AE8
	ldr r0, [sp, #0x13c]
	add r7, sp, #0x140
	add r6, sp, #0x15c
	str r0, [sp, #0x158]
	ldmia r7, {r2, r3}
	mov ip, #0
	stmia r6, {r2, r3}
	add r1, r7, #8
	ldmia r1, {r2, r3}
	add r0, r6, #8
	stmia r0, {r2, r3}
	ldr r5, [r7, #0x10]
	ldr r3, [r7, #0x14]
	add r0, sp, #0x1c
	add r2, sp, #0x158
	add r1, r4, #0x134
	strb ip, [sp, #5]
	strb ip, [sp, #4]
	str r5, [r6, #0x10]
	str r3, [r6, #0x14]
	bl ov45_02236BA8
	ldr r2, _02235A78 ; =_022577C4
	add r0, sp, #0x114
	add r1, sp, #0x108
	ldr r4, [r2]
	bl ov00_021ECB94
	add r0, sp, #0x114
	add r1, sp, #0x108
	bl RTC_ConvertDateTimeToSecond
	mov r2, #3
	str r2, [sp, #0xc0]
	mov r2, #0
	str r0, [sp, #0xb8]
	subs r0, r0, #3
	str r0, [sp, #0xb8]
	str r2, [sp, #0xc4]
	str r2, [sp, #0xc8]
	ldr r2, _02235A8C ; =ov45_02234870
	str r1, [sp, #0xbc]
	sbc r0, r1, #0
	str r2, [sp, #0xcc]
	str r0, [sp, #0xbc]
	add r0, sp, #0xd0
	mov r1, #4
	add r2, sp, #0xb8
	bl ov45_02232AE8
	mov r0, #0
	add r2, sp, #0xd4
	add r7, sp, #0xf0
	strb r0, [sp, #3]
	strb r0, [sp, #2]
	ldr r0, [sp, #0xd0]
	add r6, r2, #8
	str r0, [sp, #0xec]
	ldr r1, [r2, #0x10]
	ldr r0, [r2, #0x14]
	ldmia r2, {r2, r3}
	stmia r7, {r2, r3}
	add r5, r7, #8
	ldmia r6, {r2, r3}
	stmia r5, {r2, r3}
	str r1, [r7, #0x10]
	str r0, [r7, #0x14]
	add r0, sp, #0x14
	add r2, sp, #0xec
	add r1, r4, #0x134
	bl ov45_02236BA8
	ldr r2, _02235A78 ; =_022577C4
	add r0, sp, #0xa8
	add r1, sp, #0x9c
	ldr r4, [r2]
	bl ov00_021ECB94
	add r0, sp, #0xa8
	add r1, sp, #0x9c
	bl RTC_ConvertDateTimeToSecond
	mov r2, #0
	subs r0, r0, #4
	str r0, [sp, #0x4c]
	sbc r0, r1, #0
	str r0, [sp, #0x50]
	ldr r0, _02235A90 ; =ov45_02234944
	mov r1, #4
	str r1, [sp, #0x54]
	str r0, [sp, #0x60]
	str r2, [sp, #0x58]
	str r2, [sp, #0x5c]
	add r0, sp, #0x64
	mov r1, #6
	add r2, sp, #0x4c
	bl ov45_02232AE8
	mov r0, #0
	add r2, sp, #0x68
	add r7, sp, #0x84
	add r1, r4, #0x134
	strb r0, [sp, #1]
	strb r0, [sp]
	ldr r0, [sp, #0x64]
	add r6, r2, #8
	str r0, [sp, #0x80]
	ldr r4, [r2, #0x10]
	ldr r0, [r2, #0x14]
	ldmia r2, {r2, r3}
	stmia r7, {r2, r3}
	add r5, r7, #8
	ldmia r6, {r2, r3}
	stmia r5, {r2, r3}
	str r0, [r7, #0x14]
	add r0, sp, #0xc
	add r2, sp, #0x80
	str r4, [r7, #0x10]
	bl ov45_02236BA8
	ldr r0, _02235A78 ; =_022577C4
	add r1, sp, #0x40
	ldr r4, [r0]
	add r3, sp, #0x2a8
	ldr r0, [r4, #0x100]
	ldr r2, [r4, #0xfc]
	str r0, [sp, #0x40]
	ldr r0, [r4, #0x104]
	str r2, [sp, #0x48]
	str r0, [sp, #0x44]
	ldmia r1, {r0, r1, r2}
	stmia r3, {r0, r1, r2}
	ldr r5, [r4, #0x124]
	add r0, r4, #0x110
	bl ov45_02231C78
	ldr r3, [r4, #0x114]
	mov r2, r0
	ldr r4, [r4, #0xc]
	add r1, sp, #0x2a8
	mov r0, r5
	blx r4
	add sp, sp, #0x2b4
	ldmia sp!, {r4, r5, r6, r7, pc}
_022359D0:
	ldr r0, _02235A78 ; =_022577C4
	mov r1, #0
	ldr r4, [r0]
	add r2, sp, #0x34
	mov r0, r1
_022359E4:
	str r0, [r2, r1, lsl #2]
	add r1, r1, #1
	cmp r1, #3
	blo _022359E4
	mov r2, #5
	add r1, sp, #0x34
	add r0, r4, #0xbc
	str r2, [r4, #0xb8]
	bl ov45_02237D48
	mov r1, #0
	str r1, [r4, #0xc8]
	add r0, sp, #0x34
	str r1, [r4, #0xcc]
	bl ov45_02237BC8
	ldr r0, _02235A78 ; =_022577C4
	ldr r1, [r0]
	ldr r0, [r1, #0x70]
	cmp r0, #3
	movge r0, #0
	addlt r0, r0, #1
	strlt r0, [r1, #0x70]
	movlt r0, #1
	cmp r0, #0
	beq _02235A5C
	ldr r0, _02235A78 ; =_022577C4
	mov r1, #4
	ldr r0, [r0]
	add sp, sp, #0x2b4
	str r1, [r0]
	ldmia sp!, {r4, r5, r6, r7, pc}
_02235A5C:
	ldr r0, _02235A78 ; =_022577C4
	mov r1, #3
	ldr r2, [r0]
	mov r0, #0xa
	stmia r2, {r0, r1}
	add sp, sp, #0x2b4
	ldmia sp!, {r4, r5, r6, r7, pc}
	.balign 4, 0
_02235A78: .word _022577C4
_02235A7C: .word ov45_02236690
_02235A80: .word ov45_022347C4
_02235A84: .word ov45_02234764
_02235A88: .word ov45_022348C8
_02235A8C: .word ov45_02234870
_02235A90: .word ov45_02234944
	arm_func_end ov45_022354F4

	arm_func_start ov45_02235A94
ov45_02235A94: ; 0x02235A94
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	sub sp, sp, #0x30
	mov r6, r3
	cmp r6, #5
	mov r7, r0
	addeq sp, sp, #0x30
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	cmp r7, #0
	beq _02235BC4
	cmp r6, #4
	addls pc, pc, r6, lsl #2
	b _02235AF0
_02235AC4: ; jump table
	b _02235AF0 ; case 0
	b _02235AD8 ; case 1
	b _02235AD8 ; case 2
	b _02235AE0 ; case 3
	b _02235AE8 ; case 4
_02235AD8:
	ldr r1, _02235C30 ; =ov45_02254D5C
	b _02235AF4
_02235AE0:
	ldr r1, _02235C34 ; =ov45_02254D1C
	b _02235AF4
_02235AE8:
	ldr r1, _02235C38 ; =ov45_02254D3C
	b _02235AF4
_02235AF0:
	mov r1, #0
_02235AF4:
	cmp r1, #0
	bne _02235B10
	ldr r0, _02235C3C ; =_022577C4
	mov r3, #1
	ldr r4, [r0]
	mov r0, #0xa
	stmia r4, {r0, r3}
_02235B10:
	mov r0, r2
	bl ov45_02241D58
	ldr r0, _02235C3C ; =_022577C4
	mov r1, #0
	ldr r4, [r0]
	add ip, sp, #0x24
	str r1, [r4, #0x10c]
	ldr r1, [r4, #0xfc]
	add r0, r4, #0x100
	str r1, [sp, #0x20]
	ldmia r0, {r2, r3}
	stmia ip, {r2, r3}
	ldr r1, [r4, #0x108]
	add r0, sp, #0x10
	str r1, [sp, #0x2c]
	str r6, [sp, #0x20]
	str r6, [sp, #0xc]
	ldmia ip, {r2, r3}
	stmia r0, {r2, r3}
	mov r0, #1
	str r0, [sp, #0x1c]
	add r5, sp, #0xc
	str r1, [sp, #0x18]
	mov r1, r5
	add r0, r4, #0xfc
	mov r2, #0x10
	bl memcmp
	cmp r0, #0
	moveq r0, #1
	movne r0, #0
	cmp r0, #0
	bne _02235BB8
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
_02235BB8:
	mov r0, #1
	str r0, [r4, #0x128]
	b _02235C10
_02235BC4:
	ldr r0, _02235C3C ; =_022577C4
	mov r1, #0
	ldr r4, [r0]
	add r2, sp, #0
	mov r0, r1
_02235BD8:
	str r0, [r2, r1, lsl #2]
	add r1, r1, #1
	cmp r1, #3
	blo _02235BD8
	mov r2, #5
	add r1, sp, #0
	add r0, r4, #0xd4
	str r2, [r4, #0xd0]
	bl ov45_02237D48
	mov r1, #0
	str r1, [r4, #0xe0]
	add r0, sp, #0
	str r1, [r4, #0xe4]
	bl ov45_02237BC8
_02235C10:
	ldr r1, _02235C3C ; =_022577C4
	mov r0, r7
	ldr r2, [r1]
	mov r1, r6
	ldr r2, [r2, #8]
	blx r2
	add sp, sp, #0x30
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.balign 4, 0
_02235C30: .word ov45_02254D5C
_02235C34: .word ov45_02254D1C
_02235C38: .word ov45_02254D3C
_02235C3C: .word _022577C4
	arm_func_end ov45_02235A94

	arm_func_start ov45_02235C40
ov45_02235C40: ; 0x02235C40
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	sub sp, sp, #0x60
	mov r2, r0
	mov r7, r1
	add r0, sp, #0x38
	mov r1, r2
	bl ov45_02237B58
	ldr r1, _02235E48 ; =_022577C4
	add r0, sp, #0x20
	ldr r5, [r1]
	add r6, sp, #0x38
	add r1, r5, #0xb8
	bl ov45_022320B4
	add r0, sp, #0x20
	mov r1, r6
	bl ov45_022340F8
	mov r4, r0
	add r0, sp, #0x20
	bl ov45_02237BC8
	cmp r4, #0
	ldrne r5, [r5, #0xb8]
	bne _02235CC8
	add r0, sp, #0x2c
	add r1, r5, #0xd0
	bl ov45_022320B4
	add r0, sp, #0x2c
	mov r1, r6
	bl ov45_022340F8
	mov r4, r0
	add r0, sp, #0x2c
	bl ov45_02237BC8
	cmp r4, #0
	ldrne r5, [r5, #0xd0]
	moveq r5, #5
_02235CC8:
	add r0, sp, #0x38
	bl ov45_02237BC8
	cmp r5, #5
	addeq sp, sp, #0x60
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	cmp r5, #0
	bne _02235D00
	ldr r0, _02235E48 ; =_022577C4
	mov r1, r7
	ldr r0, [r0]
	add r0, r0, #0xe8
	bl ov45_0223DD3C
	add sp, sp, #0x60
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_02235D00:
	ldr r0, _02235E48 ; =_022577C4
	mov r1, r7
	ldr r0, [r0]
	add r0, r0, #0xe8
	bl ov45_0223DEC4
	movs r4, r0
	bne _02235D48
	ldr r0, _02235E48 ; =_022577C4
	mov r1, r7
	ldr r0, [r0]
	add r0, r0, #0xe8
	bl ov45_0223DD3C
	ldr r0, _02235E48 ; =_022577C4
	mov r1, r7
	ldr r0, [r0]
	add r0, r0, #0xe8
	bl ov45_0223DEC4
	mov r4, r0
_02235D48:
	mov r0, #0
	str r0, [r4, #0x14]
	ldr r0, [r4, #4]
	add r1, sp, #0x54
	str r0, [sp, #0x50]
	add r0, r4, #8
	ldmia r0, {r2, r3}
	stmia r1, {r2, r3}
	ldr ip, [r4, #0x10]
	add r0, sp, #0x10
	str ip, [sp, #0x5c]
	str r5, [sp, #0x50]
	str r5, [sp, #0xc]
	ldmia r1, {r2, r3}
	stmia r0, {r2, r3}
	add r6, sp, #0xc
	mov r3, #1
	mov r1, r6
	add r0, r4, #4
	mov r2, #0x10
	str ip, [sp, #0x18]
	str r3, [sp, #0x1c]
	bl memcmp
	cmp r0, #0
	moveq r0, #1
	movne r0, #0
	cmp r0, #0
	bne _02235DE0
	ldr r0, [r6]
	add r1, r6, #4
	str r0, [r4, #4]
	add r0, r4, #8
	ldmia r1, {r2, r3}
	stmia r0, {r2, r3}
	ldr r1, [r6, #0xc]
	mov r0, #1
	str r1, [r4, #0x10]
	str r0, [r4, #0x14]
_02235DE0:
	mov r0, #1
	str r0, [r4, #0x30]
	mov r0, #0
	str r0, [r4, #0x14]
	ldr r5, [r4, #4]
	ldr r2, [r4, #8]
	ldr r1, [r4, #0xc]
	add r0, sp, #0
	add r3, sp, #0x44
	str r5, [sp, #8]
	str r2, [sp]
	str r1, [sp, #4]
	ldmia r0, {r0, r1, r2}
	stmia r3, {r0, r1, r2}
	ldr r0, [r4, #0x38]
	cmp r0, #0
	addeq sp, sp, #0x60
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	ldr r1, _02235E48 ; =_022577C4
	mov r0, r7
	ldr r2, [r1]
	mov r1, r3
	ldr r2, [r2, #0x20]
	blx r2
	add sp, sp, #0x60
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.balign 4, 0
_02235E48: .word _022577C4
	arm_func_end ov45_02235C40

	arm_func_start ov45_02235E4C
ov45_02235E4C: ; 0x02235E4C
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	sub sp, sp, #0x60
	mov r2, r0
	mov r6, r1
	add r0, sp, #0x38
	mov r1, r2
	bl ov45_02237B58
	ldr r1, _0223606C ; =_022577C4
	add r0, sp, #0x20
	ldr r5, [r1]
	add r7, sp, #0x38
	add r1, r5, #0xb8
	bl ov45_022320B4
	add r0, sp, #0x20
	mov r1, r7
	bl ov45_022340F8
	mov r4, r0
	add r0, sp, #0x20
	bl ov45_02237BC8
	cmp r4, #0
	ldrne r4, [r5, #0xb8]
	bne _02235ED4
	add r0, sp, #0x2c
	add r1, r5, #0xd0
	bl ov45_022320B4
	add r0, sp, #0x2c
	mov r1, r7
	bl ov45_022340F8
	mov r4, r0
	add r0, sp, #0x2c
	bl ov45_02237BC8
	cmp r4, #0
	ldrne r4, [r5, #0xd0]
	moveq r4, #5
_02235ED4:
	add r0, sp, #0x38
	bl ov45_02237BC8
	cmp r4, #5
	addeq sp, sp, #0x60
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	cmp r4, #0
	bne _02235F48
	ldr r0, _0223606C ; =_022577C4
	mov r1, r6
	ldr r0, [r0]
	add r0, r0, #0xe8
	bl ov45_0223DEC4
	cmp r0, #0
	ldrne r0, [r0, #0x38]
	cmpne r0, #0
	addeq sp, sp, #0x60
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	ldr r1, _0223606C ; =_022577C4
	mov r0, r6
	ldr r1, [r1]
	ldr r1, [r1, #0x10]
	blx r1
	ldr r0, _0223606C ; =_022577C4
	mov r1, r6
	ldr r0, [r0]
	add r0, r0, #0xe8
	bl ov45_0223DE38
	add sp, sp, #0x60
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_02235F48:
	ldr r0, _0223606C ; =_022577C4
	mov r1, r6
	ldr r0, [r0]
	add r0, r0, #0xe8
	bl ov45_0223DEC4
	movs r4, r0
	addeq sp, sp, #0x60
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	mov r0, #0
	str r0, [r4, #0x14]
	ldr r0, [r4, #4]
	add lr, sp, #0x54
	str r0, [sp, #0x50]
	add r0, r4, #8
	ldmia r0, {r2, r3}
	stmia lr, {r2, r3}
	ldr ip, [r4, #0x10]
	mov r1, #5
	add r0, sp, #0x10
	str ip, [sp, #0x5c]
	str r1, [sp, #0x50]
	str r1, [sp, #0xc]
	add r5, sp, #0xc
	ldmia lr, {r2, r3}
	stmia r0, {r2, r3}
	mov r3, #1
	mov r1, r5
	add r0, r4, #4
	mov r2, #0x10
	str ip, [sp, #0x18]
	str r3, [sp, #0x1c]
	bl memcmp
	cmp r0, #0
	moveq r0, #1
	movne r0, #0
	cmp r0, #0
	bne _02236004
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
_02236004:
	mov r0, #1
	str r0, [r4, #0x30]
	mov r0, #0
	str r0, [r4, #0x14]
	ldr r5, [r4, #4]
	ldr r2, [r4, #8]
	ldr r1, [r4, #0xc]
	add r0, sp, #0
	add r3, sp, #0x44
	str r5, [sp, #8]
	str r2, [sp]
	str r1, [sp, #4]
	ldmia r0, {r0, r1, r2}
	stmia r3, {r0, r1, r2}
	ldr r0, [r4, #0x38]
	cmp r0, #0
	addeq sp, sp, #0x60
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	ldr r1, _0223606C ; =_022577C4
	mov r0, r6
	ldr r2, [r1]
	mov r1, r3
	ldr r2, [r2, #0x20]
	blx r2
	add sp, sp, #0x60
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.balign 4, 0
_0223606C: .word _022577C4
	arm_func_end ov45_02235E4C

	arm_func_start ov45_02236070
ov45_02236070: ; 0x02236070
	stmdb sp!, {r3, lr}
	mov ip, r1
	mov r3, r2
	mov r1, r0
	mov r2, ip
	mov r0, #0
	bl ov45_02236090
	ldmia sp!, {r3, pc}
	arm_func_end ov45_02236070

	arm_func_start ov45_02236090
ov45_02236090: ; 0x02236090
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, lr}
	sub sp, sp, #0x58
	movs r7, r0
	mov r6, r1
	mov r5, r2
	mov r4, #5
	beq _0223612C
	add r0, sp, #0x28
	mov r1, r7
	bl ov45_02237B58
	ldr r1, _022362B0 ; =_022577C4
	add r0, sp, #4
	ldr r8, [r1]
	add sb, sp, #0x28
	add r1, r8, #0xb8
	bl ov45_022320B4
	add r0, sp, #4
	mov r1, sb
	bl ov45_022340F8
	mov r4, r0
	add r0, sp, #4
	bl ov45_02237BC8
	cmp r4, #0
	ldrne r4, [r8, #0xb8]
	bne _02236124
	add r0, sp, #0x10
	add r1, r8, #0xd0
	bl ov45_022320B4
	add r0, sp, #0x10
	mov r1, sb
	bl ov45_022340F8
	mov r4, r0
	add r0, sp, #0x10
	bl ov45_02237BC8
	cmp r4, #0
	ldrne r4, [r8, #0xd0]
	moveq r4, #5
_02236124:
	add r0, sp, #0x28
	bl ov45_02237BC8
_0223612C:
	cmp r7, #0
	beq _02236140
	cmp r4, #5
	addeq sp, sp, #0x58
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
_02236140:
	mov r7, #0
	add r2, sp, #0x4c
	mov r3, #6
	add r0, sp, #0x1c
	mov r1, r5
	str r7, [sp, #0x34]
	str r3, [sp, #0x38]
	str r7, [sp, #0x3c]
	str r7, [sp, #0x40]
	str r7, [sp, #0x44]
	str r7, [sp, #0x48]
	str r7, [r2]
	str r7, [r2, #4]
	str r7, [r2, #8]
	bl ov45_02237B58
	add r0, sp, #0x1c
	add r1, sp, #0x34
	bl ov45_0223D27C
	cmp r0, #0
	moveq r5, #1
	movne r5, r7
	add r0, sp, #0x1c
	bl ov45_02237BC8
	cmp r5, #0
	beq _022361B4
	add r0, sp, #0x4c
	bl ov45_022379D4
	add sp, sp, #0x58
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
_022361B4:
	ldr r0, [sp, #0x40]
	cmp r0, #0
	beq _0223627C
	cmp r0, #1
	bne _022362A0
	ldr r0, _022362B0 ; =_022577C4
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
	bne _02236208
	add r0, sp, #0x4c
	bl ov45_022379D4
	add sp, sp, #0x58
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
_02236208:
	ldr r0, [sp, #0x3c]
	cmp r0, #0
	beq _02236250
	cmp r0, #1
	bne _022362A0
	ldr r1, _022362B0 ; =_022577C4
	add r0, sp, #0x4c
	ldr r5, [r1]
	bl ov45_02231C78
	ldr r1, [sp, #0x50]
	mov r3, r0
	str r1, [sp]
	ldr r2, [sp, #0x44]
	ldr r5, [r5, #0x18]
	mov r0, r6
	mov r1, r4
	blx r5
	b _022362A0
_02236250:
	ldr r1, _022362B0 ; =_022577C4
	add r0, sp, #0x4c
	ldr r5, [r1]
	bl ov45_02231C78
	mov r3, r0
	ldr r2, [sp, #0x44]
	ldr r5, [r5, #0x14]
	mov r0, r6
	mov r1, r4
	blx r5
	b _022362A0
_0223627C:
	add r0, sp, #0x4c
	bl ov45_02231C78
	ldr r5, [sp, #0x50]
	mov r3, r0
	add r0, sp, #0x34
	mov r1, r6
	mov r2, r4
	str r5, [sp]
	bl ov45_0223454C
_022362A0:
	add r0, sp, #0x4c
	bl ov45_022379D4
	add sp, sp, #0x58
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	.balign 4, 0
_022362B0: .word _022577C4
	arm_func_end ov45_02236090

	arm_func_start ov45_022362B4
ov45_022362B4: ; 0x022362B4
	stmdb sp!, {r3, lr}
	ldr r1, _022362CC ; =_022577C4
	ldr r1, [r1]
	ldr r1, [r1, #0x48]
	blx r1
	ldmia sp!, {r3, pc}
	.balign 4, 0
_022362CC: .word _022577C4
	arm_func_end ov45_022362B4

	arm_func_start ov45_022362D0
ov45_022362D0: ; 0x022362D0
	stmdb sp!, {r3, r4, lr}
	sub sp, sp, #0xc
	mov r4, r0
	stmia sp, {r2, r3}
	mov lr, r1
	mov r0, #1
	mov ip, #0
	mov r1, r0
	mov r2, r4
	mov r3, lr
	str ip, [sp, #8]
	bl ov45_02233E10
	add sp, sp, #0xc
	ldmia sp!, {r3, r4, pc}
	arm_func_end ov45_022362D0

	arm_func_start ov45_02236308
ov45_02236308: ; 0x02236308
	stmdb sp!, {r3, r4, lr}
	sub sp, sp, #0xc
	ldr lr, [sp, #0x18]
	mov r4, r2
	stmia sp, {r3, lr}
	ldr ip, [sp, #0x1c]
	mov r2, r1
	mov r3, r4
	mov r1, #0
	str ip, [sp, #8]
	bl ov45_02233E10
	add sp, sp, #0xc
	ldmia sp!, {r3, r4, pc}
	arm_func_end ov45_02236308

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

	arm_func_start ov45_02236680
ov45_02236680: ; 0x02236680
	stmdb sp!, {r3, lr}
	str r1, [sp]
	bl ov45_02237AA0
	ldmia sp!, {r3, pc}
	arm_func_end ov45_02236680

	arm_func_start ov45_02236690
ov45_02236690: ; 0x02236690
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x28
	cmp r0, #0
	mov sl, r1
	mov sb, r2
	mov r8, r3
	addeq sp, sp, #0x28
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	add r0, sp, #0x1c
	bl ov45_02237B58
	ldr r1, _022367D0 ; =_022577C4
	add r0, sp, #4
	ldr r5, [r1]
	add r6, sp, #0x1c
	add r1, r5, #0xb8
	bl ov45_022320B4
	add r0, sp, #4
	mov r1, r6
	bl ov45_022340F8
	mov r4, r0
	add r0, sp, #4
	bl ov45_02237BC8
	cmp r4, #0
	ldrne r4, [r5, #0xb8]
	bne _02236724
	add r0, sp, #0x10
	add r1, r5, #0xd0
	bl ov45_022320B4
	add r0, sp, #0x10
	mov r1, r6
	bl ov45_022340F8
	mov r4, r0
	add r0, sp, #0x10
	bl ov45_02237BC8
	cmp r4, #0
	ldrne r4, [r5, #0xd0]
	moveq r4, #5
_02236724:
	add r0, sp, #0x1c
	bl ov45_02237BC8
	cmp r4, #5
	addeq sp, sp, #0x28
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	cmp r4, #0
	addne sp, sp, #0x28
	ldmneia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	cmp sb, #0
	mov r7, #0
	ble _022367B8
	ldr r5, _022367D4 ; =ov45_02236308
	ldr fp, _022367D0 ; =_022577C4
	mov r6, r7
	mov r4, r7
_02236760:
	ldr r2, [fp]
	ldr r1, [r8, r7, lsl #2]
	ldr r0, [r2, #0x4c]
	cmp r1, r0
	beq _022367AC
	add r0, r2, #0xe8
	bl ov45_0223DD3C
	str r6, [sp]
	ldr r1, [r8, r7, lsl #2]
	ldr r2, _022367D8 ; =ov45_022550C4
	mov r0, sl
	mov r3, r5
	bl ov45_022417B4
	str r4, [sp]
	ldr r1, [r8, r7, lsl #2]
	ldr r2, _022367DC ; =ov45_022550D4
	mov r0, sl
	mov r3, r5
	bl ov45_022417B4
_022367AC:
	add r7, r7, #1
	cmp r7, sb
	blt _02236760
_022367B8:
	ldr r0, _022367D0 ; =_022577C4
	mov r1, #1
	ldr r0, [r0]
	str r1, [r0, #0xc8]
	add sp, sp, #0x28
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.balign 4, 0
_022367D0: .word _022577C4
_022367D4: .word ov45_02236308
_022367D8: .word ov45_022550C4
_022367DC: .word ov45_022550D4
	arm_func_end ov45_02236690

	arm_func_start ov45_022367E0
ov45_022367E0: ; 0x022367E0
	stmdb sp!, {r3, lr}
	cmp r0, #0
	ldmeqia sp!, {r3, pc}
	ldr r0, [r2, #0x1c]
	cmp r0, #0x14
	bne _02236824
	ldr r0, _02236834 ; =_022577C4
	mov r3, #1
	ldr r2, [r0]
	add r1, sp, #0
	str r3, [r2, #0xcc]
	ldr r0, [r0]
	mov r2, #4
	add r0, r0, #0x134
	str r2, [sp]
	bl ov45_02232D8C
	ldmia sp!, {r3, pc}
_02236824:
	mov r0, r1
	ldr r1, _02236838 ; =ov45_02254D7C
	bl ov45_02241D58
	ldmia sp!, {r3, pc}
	.balign 4, 0
_02236834: .word _022577C4
_02236838: .word ov45_02254D7C
	arm_func_end ov45_022367E0

	arm_func_start ov45_0223683C
ov45_0223683C: ; 0x0223683C
	stmdb sp!, {r4, lr}
	sub sp, sp, #0x40
	cmp r0, #0
	addeq sp, sp, #0x40
	ldmeqia sp!, {r4, pc}
	ldr r0, [sp, #0x48]
	mov r2, #0
	add r1, sp, #0x34
	str r2, [sp, #0x34]
	str r2, [sp, #0x38]
	str r2, [sp, #0x3c]
	bl ov45_02233FE4
	cmp r0, #0
	beq _02236880
	ldr r0, [sp, #0x38]
	cmp r0, #8
	beq _022368E4
_02236880:
	ldr r2, _02236944 ; =_022577C4
	add r0, sp, #0x18
	add r1, sp, #0x28
	ldr r4, [r2]
	bl ov00_021ECB94
	add r0, sp, #0x18
	add r1, sp, #0x28
	bl RTC_ConvertDateTimeToSecond
	str r0, [r4, #0x14c]
	str r1, [r4, #0x150]
	add r2, r4, #0x14c
	add r0, sp, #0x10
	ldmia r2, {r2, r3}
	stmia r0, {r2, r3}
	mov r1, #8
	mov r3, r0
	ldr r2, _02236948 ; =ov45_022550E4
	sub r0, r1, #9
	str r1, [sp]
	mov r1, #0
	bl ov45_02233434
	add r0, sp, #0x34
	bl ov45_022379D4
	add sp, sp, #0x40
	ldmia sp!, {r4, pc}
_022368E4:
	add r0, sp, #0x34
	bl ov45_02231C78
	ldr r1, _02236944 ; =_022577C4
	ldr lr, [r0]
	ldr ip, [r0, #4]
	ldr r2, [r1]
	mov r0, #1
	str r0, [r2, #0x148]
	add r3, sp, #8
	str lr, [sp, #8]
	str ip, [sp, #0xc]
	add r0, r2, #0x14c
	ldmia r3, {r2, r3}
	stmia r0, {r2, r3}
	ldr r0, [r1]
	mov r2, #5
	add r1, sp, #4
	add r0, r0, #0x134
	str r2, [sp, #4]
	bl ov45_02232D8C
	add r0, sp, #0x34
	bl ov45_022379D4
	add sp, sp, #0x40
	ldmia sp!, {r4, pc}
	.balign 4, 0
_02236944: .word _022577C4
_02236948: .word ov45_022550E4
	arm_func_end ov45_0223683C

	arm_func_start ov45_0223694C
ov45_0223694C: ; 0x0223694C
	stmdb sp!, {r3, r4, r5, lr}
	sub sp, sp, #0x20
	cmp r0, #0
	addeq sp, sp, #0x20
	ldmeqia sp!, {r3, r4, r5, pc}
	mov r2, #0
	ldr r0, [sp, #0x30]
	add r1, sp, #0x14
	str r2, [sp, #0x18]
	str r2, [sp, #0x14]
	str r2, [sp, #0x1c]
	bl ov45_02233FE4
	cmp r0, #0
	ldrne r5, [sp, #0x18]
	cmpne r5, #0
	beq _022369B4
	ldr r1, _02236A3C ; =_022577C4
	add r0, sp, #0x14
	ldr r4, [r1]
	bl ov45_02231C78
	mov r1, r0
	mov r2, r5
	add r0, r4, #0x144
	bl ov45_02238C74
	cmp r0, #0
	bne _02236A04
_022369B4:
	ldr r1, _02236A3C ; =_022577C4
	add r0, sp, #8
	ldr r1, [r1]
	add r1, r1, #0x144
	bl ov45_02238C1C
	add r0, sp, #8
	bl ov45_02231C78
	ldr ip, [sp, #0xc]
	mov r3, r0
	ldr r2, _02236A40 ; =ov45_022550F4
	mvn r0, #0
	mov r1, #0
	str ip, [sp]
	bl ov45_02233434
	add r0, sp, #8
	bl ov45_022379D4
	add r0, sp, #0x14
	bl ov45_022379D4
	add sp, sp, #0x20
	ldmia sp!, {r3, r4, r5, pc}
_02236A04:
	ldr r0, _02236A3C ; =_022577C4
	mov r3, #4
	ldr r2, [r0]
	add r1, sp, #4
	str r3, [r2, #0x144]
	ldr r0, [r0]
	mov r2, #6
	add r0, r0, #0x134
	str r2, [sp, #4]
	bl ov45_02232D8C
	add r0, sp, #0x14
	bl ov45_022379D4
	add sp, sp, #0x20
	ldmia sp!, {r3, r4, r5, pc}
	.balign 4, 0
_02236A3C: .word _022577C4
_02236A40: .word ov45_022550F4
	arm_func_end ov45_0223694C

	arm_func_start ov45_02236A44
ov45_02236A44: ; 0x02236A44
	bx lr
	arm_func_end ov45_02236A44

	arm_func_start ov45_02236A48
ov45_02236A48: ; 0x02236A48
	stmdb sp!, {r4, lr}
	mov r4, r0
	add r0, r4, #0x28
	bl ov45_022379D4
	add r0, r4, #0x18
	bl ov45_02237BC8
	add r0, r4, #0xc
	bl ov45_02237BC8
	mov r0, r4
	ldmia sp!, {r4, pc}
	arm_func_end ov45_02236A48

	arm_func_start ov45_02236A70
ov45_02236A70: ; 0x02236A70
	stmdb sp!, {r4, lr}
	ldr r1, _02236AB4 ; =ov45_02254FA0
	mov r4, r0
	str r1, [r4]
	bl ov45_0223DF4C
	add r0, r4, #0x28
	bl ov45_022379D4
	add r0, r4, #4
	bl ov45_02236B88
	cmp r4, #0
	beq _02236AAC
	mov r0, #0
	mov r1, r4
	mov r2, r0
	bl ov00_021EC2EC
_02236AAC:
	mov r0, r4
	ldmia sp!, {r4, pc}
	.balign 4, 0
_02236AB4: .word ov45_02254FA0
	arm_func_end ov45_02236A70

	arm_func_start ov45_02236AB8
ov45_02236AB8: ; 0x02236AB8
	stmdb sp!, {r4, lr}
	mov r4, r0
	add r0, r4, #0x18
	bl ov45_022379D4
	cmp r4, #0
	beq _02236AE0
	mov r0, #0
	mov r1, r4
	mov r2, r0
	bl ov00_021EC2EC
_02236AE0:
	mov r0, r4
	ldmia sp!, {r4, pc}
	arm_func_end ov45_02236AB8

	arm_func_start ov45_02236AE8
ov45_02236AE8: ; 0x02236AE8
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, lr}
	mov r2, #0
	mov r4, r0
	strb r2, [sp]
	str r2, [r4, #4]
	ldr r0, [r4, #0x10]
	cmp r0, #0
	beq _02236B78
	ldr r1, [r4, #0x14]
	cmp r1, #0
	beq _02236B28
	ldr r2, [r4, #0xc]
	ldr r0, [r4, #0x10]
	add r0, r2, r0
	bl _u32_div_f
	mov r2, r1
_02236B28:
	ldr r8, [r4, #8]
	ldr r0, [r4, #0xc]
	ldr r1, [r4, #0x14]
	add r6, r8, r2, lsl #2
	add r7, r8, r0, lsl #2
	cmp r7, r6
	add sb, r8, r1, lsl #2
	beq _02236B78
	mov r5, #0
_02236B4C:
	ldr r1, [r7]
	cmp r1, #0
	beq _02236B64
	mov r0, r5
	mov r2, r5
	bl ov00_021EC2EC
_02236B64:
	add r7, r7, #4
	cmp r7, sb
	moveq r7, r8
	cmp r7, r6
	bne _02236B4C
_02236B78:
	add r0, r4, #8
	bl ov45_02238508
	mov r0, r4
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	arm_func_end ov45_02236AE8

	arm_func_start ov45_02236B88
ov45_02236B88: ; 0x02236B88
	stmdb sp!, {r4, lr}
	mov r4, r0
	ldr r1, [r4, #4]
	cmp r1, #0
	beq _02236BA0
	bl ov45_02238558
_02236BA0:
	mov r0, r4
	ldmia sp!, {r4, pc}
	arm_func_end ov45_02236B88

	arm_func_start ov45_02236BA8
ov45_02236BA8: ; 0x02236BA8
	stmdb sp!, {r3, r4, r5, r6, lr}
	sub sp, sp, #0xc
	add ip, sp, #4
	mov r5, r2
	mov r6, r1
	mov r4, r0
	add r2, sp, #8
	add r3, sp, #5
	mov r0, r6
	mov r1, r5
	str ip, [sp]
	bl ov45_02238450
	ldr r3, [sp, #8]
	mov r1, r0
	cmp r3, #0
	beq _02236BF8
	ldr r2, [r3, #0xc]
	ldr r0, [r5]
	cmp r2, r0
	bhs _02236C24
_02236BF8:
	str r5, [sp]
	ldrb r2, [sp, #5]
	ldrb r3, [sp, #4]
	mov r0, r6
	bl ov45_02236CE0
	ldr r1, _02236C3C ; =_02254F58
	str r0, [r4]
	ldrb r0, [r1, #2]
	add sp, sp, #0xc
	strb r0, [r4, #4]
	ldmia sp!, {r3, r4, r5, r6, pc}
_02236C24:
	ldr r0, _02236C3C ; =_02254F58
	str r3, [r4]
	ldrb r0, [r0]
	strb r0, [r4, #4]
	add sp, sp, #0xc
	ldmia sp!, {r3, r4, r5, r6, pc}
	.balign 4, 0
_02236C3C: .word _02254F58
	arm_func_end ov45_02236BA8
