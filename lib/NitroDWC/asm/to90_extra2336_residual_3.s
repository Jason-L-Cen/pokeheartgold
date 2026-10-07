	.extern _02254D04
	.extern _02254F58
	.extern _022577C4
	.extern ov45_022310B0
	.extern ov45_02231C78
	.extern ov45_022320B4
	.extern ov45_0223325C
	.extern ov45_022377AC
	.extern ov45_02237800
	.extern ov45_02237890
	.extern ov45_02237A24
	.extern ov45_02237BC8
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
	.public ov45_02238104
	.extern ov45_022384BC

.text
	arm_func_start ov45_02238104
ov45_02238104: ; 0x02238104
	stmdb sp!, {r0, r1, r2, r3}
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x28
	mov r6, r0
	ldr r0, [r6]
	mov r5, r1
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	movne r0, #1
	strne r0, [sp]
	moveq r0, #0
	streq r0, [sp]
	ldr r0, [sp]
	mov fp, r3
	ldr r4, [sp, #0x60]
	cmp r0, #0
	bne _02238164
	ldrb r1, [r6]
	mov r0, #0xb
	str r0, [sp, #0x14]
	mov r0, r1, lsl #0x18
	add r7, r6, #1
	mov r8, r0, lsr #0x19
	b _02238174
_02238164:
	ldmia r6, {r0, r8}
	mov r0, r0, lsr #1
	ldr r7, [r6, #8]
	str r0, [sp, #0x14]
_02238174:
	cmp r5, r8
	bls _02238188
	ldr r0, _022383D4 ; =ov45_02255144
	bl printf
	bl abort
_02238188:
	ldr r0, [sp, #0x58]
	sub r1, r8, r5
	cmp r1, r0
	addlo r0, sp, #0x18
	str r1, [sp, #0x18]
	addhs r0, sp, #0x58
	ldr r0, [r0]
	sub sb, r4, fp
	mvn r1, #0x80000001
	cmp sb, r1
	str r0, [sp, #0x10]
	subls sl, r8, r0
	subls r0, r1, sb
	cmpls sl, r0
	bls _022381D0
	ldr r0, _022383D8 ; =ov45_0225517C
	bl printf
	bl abort
_022381D0:
	ldr r0, [sp, #0x10]
	add sl, sb, sl
	add r1, r5, r0
	ldr r0, [sp, #0x14]
	cmp sl, r0
	sub r0, r8, r1
	str r0, [sp, #0xc]
	blo _022382E4
	ldr r0, [sp, #0x14]
	add r1, sl, #1
	add r0, r0, #0xf
	bic r8, r0, #0xf
	cmp r8, r1
	bhs _0223821C
_02238208:
	mov r0, r8, lsl #1
	add r0, r0, #0xf
	bic r8, r0, #0xf
	cmp r8, r1
	blo _02238208
_0223821C:
	mov r0, r8
	bl ov45_02242394
	str r0, [sp, #8]
	cmp r5, #0
	beq _0223823C
	mov r1, r7
	mov r2, r5
	bl memcpy
_0223823C:
	ldr r0, [sp, #8]
	sub r2, r4, fp
	add r0, r0, r5
	mov r1, fp
	str r0, [sp, #4]
	bl memmove
	ldr r0, [sp, #0xc]
	cmp r0, #0
	beq _0223827C
	mov r2, r0
	ldr r0, [sp, #4]
	ldr r1, [sp, #0x10]
	add r3, r7, r5
	add r0, r0, sb
	add r1, r3, r1
	bl memcpy
_0223827C:
	ldr r1, _022383DC ; =_02254F58
	ldr r0, [sp]
	ldrsb r1, [r1, #3]
	cmp r0, #0
	ldr r0, [sp, #8]
	strb r1, [r0, sl]
	beq _022382B4
	cmp r7, #0
	beq _022382C4
	mov r0, #0
	mov r1, r7
	mov r2, r0
	bl ov00_021EC2EC
	b _022382C4
_022382B4:
	ldr r0, [r6]
	bic r0, r0, #1
	orr r0, r0, #1
	str r0, [r6]
_022382C4:
	ldr r0, [sp, #8]
	str r0, [r6, #8]
	str sl, [r6, #4]
	ldr r0, [r6]
	and r0, r0, #1
	orr r0, r0, r8, lsl #1
	str r0, [r6]
	b _022383C0
_022382E4:
	mov r1, #0
	add r0, sp, #0x1c
	mov r3, r1
_022382F0:
	str r3, [r0, r1, lsl #2]
	add r1, r1, #1
	cmp r1, #3
	blo _022382F0
	ldr r0, [sp, #0xc]
	cmp r0, #0
	beq _02238350
	add r0, r7, r5
	add r0, r0, sb
	cmp r0, r4
	bhs _02238350
	add r0, r7, r8
	cmp r4, r0
	bhi _02238350
	add r0, sp, #0x1c
	mov r1, fp
	mov r2, r4
	bl ov45_022384BC
	ldr r0, [sp, #0x1c]
	add r1, sp, #0x1c
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq fp, r1, #1
	ldrne fp, [r1, #8]
_02238350:
	ldr r0, [sp, #0xc]
	cmp r0, #0
	beq _02238374
	ldr r1, [sp, #0x10]
	add r3, r7, r5
	mov r2, r0
	add r0, r3, sb
	add r1, r3, r1
	bl memmove
_02238374:
	mov r1, fp
	mov r2, sb
	add r0, r7, r5
	bl memmove
	ldr r0, [sp]
	ldr r1, _022383DC ; =_02254F58
	cmp r0, #0
	ldrsb r0, [r1, #1]
	strb r0, [r7, sl]
	strne sl, [r6, #4]
	bne _022383B8
	ldrb r1, [r6]
	and r0, sl, #0xff
	mov r0, r0, lsl #0x19
	bic r1, r1, #0xfe
	orr r0, r1, r0, lsr #24
	strb r0, [r6]
_022383B8:
	add r0, sp, #0x1c
	bl ov45_02237BC8
_022383C0:
	mov r0, r6
	add sp, sp, #0x28
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	add sp, sp, #0x10
	bx lr
	.balign 4, 0
_022383D4: .word ov45_02255144
_022383D8: .word ov45_0225517C
_022383DC: .word _02254F58
	arm_func_end ov45_02238104

	arm_func_start ov45_022383E0
ov45_022383E0: ; 0x022383E0
	stmdb sp!, {r3, r4, r5, r6, lr}
	sub sp, sp, #4
	mov r5, r1
	mov r4, r2
	subs r1, r4, r5
	mov r6, r0
	addeq sp, sp, #4
	ldmeqia sp!, {r3, r4, r5, r6, pc}
	mov r3, #0
	strb r3, [sp, #1]
	sub r2, sp, #4
	strb r3, [r2]
	ldr r2, [r2]
	bl ov45_02237800
	mov r0, #0
	strb r0, [sp]
	ldr r2, [r6]
	ldr r0, [r6, #4]
	mov r1, r5
	add r0, r2, r0
	sub r2, r4, r5
	bl memmove
	ldr r1, [r6, #4]
	sub r0, r4, r5
	add r0, r1, r0
	str r0, [r6, #4]
	add sp, sp, #4
	ldmia sp!, {r3, r4, r5, r6, pc}
	arm_func_end ov45_022383E0
