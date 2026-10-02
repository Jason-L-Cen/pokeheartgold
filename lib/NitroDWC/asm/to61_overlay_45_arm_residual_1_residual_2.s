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
	.public ov45_02236CE0
	.extern ov45_02236EC4

.text
	arm_func_start ov45_02236CE0
ov45_02236CE0: ; 0x02236CE0
	stmdb sp!, {r4, r5, r6, r7, r8, sb, lr}
	sub sp, sp, #0x14
	mov sb, r0
	ldr r4, [sb]
	mvn r0, #0
	mov r8, r1
	mov r7, r2
	mov r6, r3
	cmp r4, r0
	ldr r5, [sp, #0x30]
	bne _02236D18
	ldr r0, _02236DE8 ; =ov45_02255104
	bl printf
	bl abort
_02236D18:
	mov r1, #0
	mov r0, #0x28
	strb r1, [sp]
	bl ov45_02242394
	mov r4, r0
	add r0, sb, #4
	str r4, [sp, #4]
	str r4, [sp, #8]
	str r0, [sp, #0xc]
	str r4, [sp, #0x10]
	adds r2, r4, #0xc
	beq _02236D80
	ldr r1, [r5]
	add r0, r2, #4
	str r1, [r2]
	add r1, r5, #4
	ldmia r1, {r2, r3}
	stmia r0, {r2, r3}
	add r2, r5, #0xc
	add r1, r0, #8
	ldmia r2, {r2, r3}
	stmia r1, {r2, r3}
	ldr r1, [r5, #0x14]
	str r1, [r0, #0x10]
	ldr r1, [r5, #0x18]
	str r1, [r0, #0x14]
_02236D80:
	mov r0, #0
	str r0, [r4, #4]
	str r0, [sp, #0x10]
	str r0, [r4]
	adds r0, r4, #8
	strne r8, [r0]
	cmp r7, #0
	strne r4, [r8]
	streq r4, [r8, #4]
	ldr r1, [sb]
	mov r0, r4
	add r1, r1, #1
	str r1, [sb]
	ldr r1, [sb, #4]
	bl ov45_02236EC4
	cmp r6, #0
	ldr r1, [sp, #0x10]
	strne r4, [sb, #0xc]
	cmp r1, #0
	beq _02236DDC
	mov r0, #0
	mov r2, r0
	bl ov00_021EC2EC
_02236DDC:
	mov r0, r4
	add sp, sp, #0x14
	ldmia sp!, {r4, r5, r6, r7, r8, sb, pc}
	.balign 4, 0
_02236DE8: .word ov45_02255104
	arm_func_end ov45_02236CE0
