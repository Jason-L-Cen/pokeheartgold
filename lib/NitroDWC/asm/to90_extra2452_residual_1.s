	.extern _02254D04
	.extern _02254F58
	.extern _022577C4
	.extern ov45_02231C78
	.extern ov45_02232B24
	.extern ov45_02234814
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
	.extern ov45_02232AE8

.text
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
