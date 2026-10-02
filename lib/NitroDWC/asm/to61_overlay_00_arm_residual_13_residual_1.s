	.extern ArrayLength
	.extern ov00_021EC3A8
	.extern ov00_021EC3D8
	.extern ov00_021FE5B4
	.extern ov00_021FE678
	.extern ov00_021FE6EC
	.extern ov00_021FE754
	.extern ov00_021FE8D8
	.extern ov00_021FE968
	.extern ov00_021FEA20
	.extern ov00_021FEC3C
	.extern ov00_021FED4C
	.extern ov00_021FEE8C
	.extern ov00_021FEF00
	.extern ov00_021FEF74
	.extern ov00_021FF01C
	.extern ov00_021FF0C0
	.extern ov00_021FF264
	.extern ov00_021FF368
	.extern ov00_021FF3C0
	.extern ov00_021FF3CC
	.extern ov00_021FF3D0
	.extern ov00_021FF3D4
	.extern ov00_021FF694
	.extern ov00_021FF6BC
	.extern ov00_021FF6E4
	.extern ov00_021FF770
	.extern ov00_021FF804
	.extern ov00_021FF818
	.extern ov00_021FF840
	.extern ov00_021FF8A0
	.extern ov00_021FF92C
	.extern ov00_021FF940
	.extern ov00_021FF970
	.extern ov00_021FF984
	.extern ov00_021FFA18
	.extern ov00_021FFA44
	.extern ov00_021FFA60
	.extern ov00_0220BF44
	.extern ov00_0220BF70
	.extern ov00_0220BF80
	.extern ov00_0220BFD0
	.extern ov00_0220C03C
	.extern ov00_0220C0C4
	.extern ov00_0220C164
	.extern ov00_0220C860
	.extern ov00_0220CC38
	.extern ov00_0220CE00
	.extern ov00_0220CF14
	.extern ov00_0220D238
	.extern ov00_0220D338
	.extern ov00_0220D404
	.extern ov00_0220E490
	.extern ov00_022163E8
	.extern ov00_022163F0
	.extern ov00_02219C94
	.extern ov00_02219CB4
	.extern ov00_02219CB8
	.extern ov00_02219CC0
	.extern ov00_02219CD0
	.extern ov00_02219CE4
	.extern ov00_02219CEC
	.extern ov00_02219CF0
	.extern ov00_02219CF4
	.extern ov00_02219D14
	.extern ov00_02219D24
	.extern ov00_02219D28
	.extern ov00_02219D30
	.extern ov00_02219D38
	.extern ov00_02219D54
	.extern ov00_02219D70
	.extern ov00_02219D74
	.extern ov00_02219E84
	.extern ov00_02219EA4
	.extern ov00_02219EDC
	.extern ov00_02219EE0
	.extern ov00_02219EF0
	.extern ov00_02219EFC
	.extern ov00_02219F04
	.extern ov00_02219F08
	.extern ov00_02219F10
	.extern ov00_02219F24
	.extern ov00_02219F30
	.extern ov00_02219F3C
	.extern ov00_02219F44
	.extern ov00_02219F48
	.extern ov00_02219F4C
	.extern ov00_02219F5C
	.extern ov00_02219F68
	.extern ov00_02219F74
	.extern ov00_0221A0B8
	.extern ov00_0221A4B0
	.extern ov00_0221A4C0
	.extern ov00_0221A4CC
	.extern ov00_0221A4E4
	.extern ov00_0221A4FC
	.extern ov00_0221A500
	.extern ov00_0221A508
	.extern ov00_0221A514
	.extern ov00_0221A51C
	.extern ov00_0221A520
	.extern ov00_0221A528
	.extern ov00_0221A52C
	.extern ov00_0221A530
	.extern ov00_0221A550
	.extern ov00_0221A560
	.extern ov00_0221A574
	.extern ov00_0221A588
	.extern ov00_0221A59C
	.extern ov00_0221A5B8
	.extern ov00_0221A5BC
	.extern ov00_0221A5C8
	.extern ov00_0221A5D8
	.extern ov00_0221A5E8
	.extern ov00_0221A5FC
	.extern ov00_0221A618
	.extern ov00_0221A638
	.extern ov00_0221A63C
	.extern ov00_0221A644
	.extern ov00_0221A650
	.extern ov00_0221B0D0
	.extern ov00_0221B16C
	.extern ov00_0221B634
	.extern ov00_0221B638
	.extern ov00_0221B664
	.extern ov00_0221B678
	.extern ov00_0221B878
	.extern ov00_0221B87C
	.extern ov00_0221B890
	.extern ov00_0221B8D0
	.extern ov00_0221B9D0
	.extern ov00_0221B9D4
	.extern ov00_0221B9DC
	.include "asm/macros.inc"
	.include "overlay_00_arm.inc"
	.include "global.inc"
	.extern ov00_02210E58
	.extern ov00_02210ECC

.text
	arm_func_start ov00_0220E6C0
ov00_0220E6C0: ; 0x0220E6C0
	stmdb sp!, {r3, r4, r5, lr}
	sub sp, sp, #0x10
	add r4, sp, #0
	mov r3, #0
	mov r5, r0
	str r3, [r4]
	str r3, [r4, #4]
	str r3, [r4, #8]
	str r3, [r4, #0xc]
	ldr r0, [r5, #0x58]
	str r0, [sp]
	str r2, [sp, #4]
	strh r1, [sp, #8]
	bl ov00_021FF368
	str r0, [sp, #0xc]
	ldr r0, [r5, #0x60]
	bl ArrayLength
	mov r4, r0
	ldr r0, [r5, #0x60]
	add r1, sp, #0
	bl ov00_021FE754
	ldr r0, [r5, #0x60]
	bl ArrayLength
	add r1, r4, #1
	cmp r1, r0
	moveq r0, #1
	movne r0, #0
	add sp, sp, #0x10
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end ov00_0220E6C0

	arm_func_start ov00_0220E734
ov00_0220E734: ; 0x0220E734
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	mov r7, r0
	mov r5, r2
	add r0, r7, #0x50
	mov r6, r1
	mov r4, r3
	bl ov00_0220BF70
	cmp r0, r5
	bge _0220E774
	mov r0, r7
	bl ov00_0220D404
	cmp r0, #0
	moveq r0, #0
	movne r0, #1
	strne r0, [r4]
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_0220E774:
	ldrh r1, [r7, #0x64]
	mov r0, r7
	mov r2, r5
	bl ov00_0220E6C0
	cmp r0, #0
	bne _0220E7A8
	mov r0, r7
	bl ov00_0220D404
	cmp r0, #0
	moveq r0, #0
	movne r0, #1
	strne r0, [r4]
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_0220E7A8:
	ldr r1, _0220E7F4 ; =ov00_02219CB4
	add r0, r7, #0x50
	mov r2, #2
	bl ov00_0220C03C
	add r0, r7, #0x50
	and r1, r6, #0xff
	bl ov00_0220BF80
	ldrh r1, [r7, #0x64]
	add r0, r7, #0x50
	add r2, r1, #1
	strh r2, [r7, #0x64]
	bl ov00_0220BFD0
	ldrh r1, [r7, #0x66]
	add r0, r7, #0x50
	bl ov00_0220BFD0
	mov r0, #0
	str r0, [r4]
	mov r0, #1
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.balign 4, 0
_0220E7F4: .word ov00_02219CB4
	arm_func_end ov00_0220E734

	arm_func_start ov00_0220E7F8
ov00_0220E7F8: ; 0x0220E7F8
	stmdb sp!, {r3, r4, r5, lr}
	mov r5, r0
	ldr r0, [r5, #0x60]
	bl ArrayLength
	mov r4, r0
	cmp r4, #0
	bgt _0220E828
	ldr r0, _0220E85C ; =ov00_02219CB8
	ldr r1, _0220E860 ; =ov00_02219CC0
	ldr r2, _0220E864 ; =ov00_02219C94
	ldr r3, _0220E868 ; =0x00000475
	bl __msl_assertion_failed
_0220E828:
	ldr r0, [r5, #0x60]
	sub r1, r4, #1
	bl ov00_021FE6EC
	ldmia r0, {r1, r2}
	ldr r3, [r5, #0x50]
	mov r0, r5
	add r1, r3, r1
	bl ov00_0220CC38
	cmp r0, #0
	mov r0, #0
	strne r0, [r5, #0x90]
	movne r0, #1
	ldmia sp!, {r3, r4, r5, pc}
	.balign 4, 0
_0220E85C: .word ov00_02219CB8
_0220E860: .word ov00_02219CC0
_0220E864: .word ov00_02219C94
_0220E868: .word 0x00000475
	arm_func_end ov00_0220E7F8

	arm_func_start ov00_0220E86C
ov00_0220E86C: ; 0x0220E86C
	stmdb sp!, {r3, r4, r5, r6, lr}
	sub sp, sp, #4
	mov r4, r2
	mov r5, r1
	add r3, sp, #0
	add r2, r4, #7
	mov r1, #0
	mov r6, r0
	bl ov00_0220E734
	cmp r0, #0
	addeq sp, sp, #4
	moveq r0, #0
	ldmeqia sp!, {r3, r4, r5, r6, pc}
	ldr r0, [sp]
	cmp r0, #0
	addne sp, sp, #4
	movne r0, #1
	ldmneia sp!, {r3, r4, r5, r6, pc}
	mov r1, r5
	mov r2, r4
	add r0, r6, #0x50
	bl ov00_0220C03C
	mov r0, r6
	bl ov00_0220E7F8
	cmp r0, #0
	movne r0, #1
	moveq r0, #0
	add sp, sp, #4
	ldmia sp!, {r3, r4, r5, r6, pc}
	arm_func_end ov00_0220E86C

	arm_func_start ov00_0220E8E0
ov00_0220E8E0: ; 0x0220E8E0
	stmdb sp!, {r3, r4, r5, lr}
	mov r4, r1
	add r3, sp, #0
	mov r1, #1
	mov r2, #0x27
	mov r5, r0
	bl ov00_0220E734
	cmp r0, #0
	moveq r0, #0
	ldmeqia sp!, {r3, r4, r5, pc}
	ldr r0, [sp]
	cmp r0, #0
	movne r0, #1
	ldmneia sp!, {r3, r4, r5, pc}
	mov r1, r4
	add r0, r5, #0x50
	mov r2, #0x20
	bl ov00_0220C03C
	mov r0, r5
	bl ov00_0220E7F8
	cmp r0, #0
	movne r0, #1
	moveq r0, #0
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end ov00_0220E8E0

	arm_func_start ov00_0220E940
ov00_0220E940: ; 0x0220E940
	stmdb sp!, {r3, r4, r5, r6, lr}
	sub sp, sp, #4
	mov r5, r1
	mov r4, r2
	add r3, sp, #0
	mov r1, #2
	mov r2, #0x47
	mov r6, r0
	bl ov00_0220E734
	cmp r0, #0
	addeq sp, sp, #4
	moveq r0, #0
	ldmeqia sp!, {r3, r4, r5, r6, pc}
	ldr r0, [sp]
	cmp r0, #0
	addne sp, sp, #4
	movne r0, #1
	ldmneia sp!, {r3, r4, r5, r6, pc}
	mov r1, r5
	add r0, r6, #0x50
	mov r2, #0x20
	bl ov00_0220C03C
	mov r1, r4
	add r0, r6, #0x50
	mov r2, #0x20
	bl ov00_0220C03C
	mov r0, r6
	bl ov00_0220E7F8
	cmp r0, #0
	ldrne r1, [r6, #0x88]
	moveq r0, #0
	movne r0, #1
	strne r1, [r6, #0x8c]
	add sp, sp, #4
	ldmia sp!, {r3, r4, r5, r6, pc}
	arm_func_end ov00_0220E940

	arm_func_start ov00_0220E9CC
ov00_0220E9CC: ; 0x0220E9CC
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	mov r4, r3
	mov r6, r1
	mov r5, r2
	add r3, sp, #0
	add r2, r4, #0x27
	mov r1, #3
	mov r7, r0
	bl ov00_0220E734
	cmp r0, #0
	moveq r0, #0
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	ldr r0, [sp]
	cmp r0, #0
	movne r0, #1
	ldmneia sp!, {r3, r4, r5, r6, r7, pc}
	mov r1, r6
	add r0, r7, #0x50
	mov r2, #0x20
	bl ov00_0220C03C
	mov r1, r5
	mov r2, r4
	add r0, r7, #0x50
	bl ov00_0220C03C
	mov r0, r7
	bl ov00_0220E7F8
	cmp r0, #0
	movne r0, #1
	moveq r0, #0
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	arm_func_end ov00_0220E9CC

	arm_func_start ov00_0220EA44
ov00_0220EA44: ; 0x0220EA44
	stmdb sp!, {r3, r4, lr}
	sub sp, sp, #4
	add r3, sp, #0
	mov r1, #4
	mov r2, #7
	mov r4, r0
	bl ov00_0220E734
	cmp r0, #0
	addeq sp, sp, #4
	moveq r0, #0
	ldmeqia sp!, {r3, r4, pc}
	ldr r0, [sp]
	cmp r0, #0
	addne sp, sp, #4
	movne r0, #1
	ldmneia sp!, {r3, r4, pc}
	mov r0, r4
	bl ov00_0220E7F8
	cmp r0, #0
	movne r0, #1
	moveq r0, #0
	add sp, sp, #4
	ldmia sp!, {r3, r4, pc}
	arm_func_end ov00_0220EA44

	arm_func_start ov00_0220EAA0
ov00_0220EAA0: ; 0x0220EAA0
	stmdb sp!, {r3, r4, r5, r6, lr}
	sub sp, sp, #4
	mov r4, r2
	mov r5, r1
	add r3, sp, #0
	add r2, r4, #7
	mov r1, #5
	mov r6, r0
	bl ov00_0220E734
	cmp r0, #0
	addeq sp, sp, #4
	moveq r0, #0
	ldmeqia sp!, {r3, r4, r5, r6, pc}
	ldr r0, [sp]
	cmp r0, #0
	addne sp, sp, #4
	movne r0, #1
	ldmneia sp!, {r3, r4, r5, r6, pc}
	mov r1, r5
	mov r2, r4
	add r0, r6, #0x50
	bl ov00_0220C03C
	mov r0, r6
	bl ov00_0220E7F8
	cmp r0, #0
	movne r0, #1
	moveq r0, #0
	add sp, sp, #4
	ldmia sp!, {r3, r4, r5, r6, pc}
	arm_func_end ov00_0220EAA0

	arm_func_start ov00_0220EB14
ov00_0220EB14: ; 0x0220EB14
	stmdb sp!, {r3, r4, lr}
	sub sp, sp, #4
	add r3, sp, #0
	mov r1, #6
	mov r2, #7
	mov r4, r0
	bl ov00_0220E734
	cmp r0, #0
	addeq sp, sp, #4
	moveq r0, #0
	ldmeqia sp!, {r3, r4, pc}
	ldr r0, [sp]
	cmp r0, #0
	addne sp, sp, #4
	movne r0, #1
	ldmneia sp!, {r3, r4, pc}
	mov r0, r4
	bl ov00_0220E7F8
	cmp r0, #0
	movne r0, #1
	moveq r0, #0
	add sp, sp, #4
	ldmia sp!, {r3, r4, pc}
	arm_func_end ov00_0220EB14

	arm_func_start ov00_0220EB70
ov00_0220EB70: ; 0x0220EB70
	stmdb sp!, {r3, r4, lr}
	sub sp, sp, #4
	mov r1, #7
	add r3, sp, #0
	mov r2, r1
	mov r4, r0
	bl ov00_0220E734
	cmp r0, #0
	addeq sp, sp, #4
	moveq r0, #0
	ldmeqia sp!, {r3, r4, pc}
	ldr r0, [sp]
	cmp r0, #0
	addne sp, sp, #4
	movne r0, #1
	ldmneia sp!, {r3, r4, pc}
	mov r0, r4
	bl ov00_0220E7F8
	cmp r0, #0
	movne r0, #1
	moveq r0, #0
	add sp, sp, #4
	ldmia sp!, {r3, r4, pc}
	arm_func_end ov00_0220EB70

	arm_func_start ov00_0220EBCC
ov00_0220EBCC: ; 0x0220EBCC
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, lr}
	mov r7, r2
	mov sb, r0
	mov r8, r1
	cmp r7, #2
	blt _0220EBFC
	ldr r1, _0220EC90 ; =ov00_02219CB4
	mov r0, r8
	mov r2, #2
	bl memcmp
	cmp r0, #0
	beq _0220EC1C
_0220EBFC:
	mov r0, sb
	mov r1, r8
	mov r2, r7
	bl ov00_0220CC38
	cmp r0, #0
	moveq r0, #0
	movne r0, #1
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
_0220EC1C:
	add r6, r7, #2
	add r0, sb, #0x50
	bl ov00_0220BF70
	cmp r0, r6
	movlt r0, #1
	ldmltia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	ldr r1, _0220EC90 ; =ov00_02219CB4
	add r0, sb, #0x50
	ldr r5, [sb, #0x50]
	ldr r4, [sb, #0x58]
	mov r2, #2
	bl ov00_0220C03C
	mov r1, r8
	mov r2, r7
	add r0, sb, #0x50
	bl ov00_0220C03C
	mov r0, sb
	mov r2, r6
	add r1, r5, r4
	bl ov00_0220CC38
	cmp r0, #0
	moveq r0, #0
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	mov r2, r6
	add r0, sb, #0x50
	mvn r1, #0
	bl ov00_0220C0C4
	mov r0, #1
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	.balign 4, 0
_0220EC90: .word ov00_02219CB4
	arm_func_end ov00_0220EBCC

	arm_func_start ov00_0220EC94
ov00_0220EC94: ; 0x0220EC94
	stmdb sp!, {r4, lr}
	sub sp, sp, #8
	ldr r1, _0220ECF4 ; =ov00_02219CB4
	mov r4, r0
	ldrb r3, [r1]
	ldrb r2, [r1, #1]
	add r0, sp, #0
	mov r1, #0x64
	strb r1, [sp, #2]
	strb r3, [r0]
	strb r2, [r0, #1]
	ldrh r2, [r4, #0x66]
	mov r1, #3
	bl ov00_0220D338
	add r1, sp, #0
	mov r0, r4
	mov r2, #5
	bl ov00_0220CC38
	cmp r0, #0
	mov r0, #0
	strne r0, [r4, #0x90]
	movne r0, #1
	add sp, sp, #8
	ldmia sp!, {r4, pc}
	.balign 4, 0
_0220ECF4: .word ov00_02219CB4
	arm_func_end ov00_0220EC94

	arm_func_start ov00_0220ECF8
ov00_0220ECF8: ; 0x0220ECF8
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	sub sp, sp, #8
	ldr r3, _0220ED80 ; =ov00_02219CB4
	mov r4, r1
	ldrb lr, [r3]
	ldrb ip, [r3, #1]
	mov r5, r0
	add r0, sp, #0
	mov r7, r2
	mov r3, #0x65
	mov r2, r4
	mov r6, #0
	mov r1, #3
	strb lr, [r0]
	strb ip, [r0, #1]
	strb r3, [sp, #2]
	bl ov00_0220D338
	cmp r4, r7
	add r6, r6, #5
	beq _0220ED5C
	add r0, sp, #0
	mov r1, r6
	mov r2, r7
	bl ov00_0220D338
	add r6, r6, #2
_0220ED5C:
	add r1, sp, #0
	mov r0, r5
	mov r2, r6
	bl ov00_0220CC38
	cmp r0, #0
	moveq r0, #0
	movne r0, #1
	add sp, sp, #8
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.balign 4, 0
_0220ED80: .word ov00_02219CB4
	arm_func_end ov00_0220ECF8

	arm_func_start ov00_0220ED84
ov00_0220ED84: ; 0x0220ED84
	ldr ip, _0220ED94 ; =ov00_0220CC38
	mov r3, #0x67
	strb r3, [r1, #2]
	bx ip
	.balign 4, 0
_0220ED94: .word ov00_0220CC38
	arm_func_end ov00_0220ED84

	arm_func_start ov00_0220ED98
ov00_0220ED98: ; 0x0220ED98
	ldr ip, _0220EDB0 ; =ov00_0220EDB4
	mov r1, r0
	ldrh r2, [r1, #4]
	ldr r0, [r1, #8]
	ldr r1, [r1]
	bx ip
	.balign 4, 0
_0220EDB0: .word ov00_0220EDB4
	arm_func_end ov00_0220ED98

	arm_func_start ov00_0220EDB4
ov00_0220EDB4: ; 0x0220EDB4
	stmdb sp!, {r4, lr}
	sub sp, sp, #8
	ldr lr, _0220EDFC ; =ov00_02219CB4
	mov ip, #0x68
	ldrb r4, [lr]
	ldrb lr, [lr, #1]
	add r3, sp, #4
	strb ip, [sp, #6]
	strb r4, [r3]
	strb lr, [r3, #1]
	mov ip, #3
	str ip, [sp]
	bl ov00_0220F474
	cmp r0, #0
	movne r0, #1
	moveq r0, #0
	add sp, sp, #8
	ldmia sp!, {r4, pc}
	.balign 4, 0
_0220EDFC: .word ov00_02219CB4
	arm_func_end ov00_0220EDB4

	arm_func_start ov00_0220EE00
ov00_0220EE00: ; 0x0220EE00
	stmdb sp!, {r3, r4, r5, lr}
	mov r4, r0
	mov r5, r1
	ldr r1, [r5]
	ldrh r2, [r4, #0x66]
	ldr r0, [r4, #0x50]
	add r1, r1, #5
	bl ov00_0220D338
	ldr r3, [r4, #0x50]
	ldmia r5, {r1, r2}
	mov r0, r4
	add r1, r3, r1
	bl ov00_0220CC38
	cmp r0, #0
	moveq r0, #0
	ldmeqia sp!, {r3, r4, r5, pc}
	ldr r0, [r4, #0x88]
	str r0, [r5, #0xc]
	ldr r0, [r5]
	ldr r1, [r4, #0x50]
	add r0, r0, #2
	ldrb r0, [r1, r0]
	cmp r0, #2
	ldreq r0, [r4, #0x88]
	streq r0, [r4, #0x8c]
	mov r0, #1
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end ov00_0220EE00

	arm_func_start ov00_0220EE6C
ov00_0220EE6C: ; 0x0220EE6C
	stmdb sp!, {r3, lr}
	cmp r3, #0
	beq _0220EE80
	bl ov00_0220E86C
	ldmia sp!, {r3, pc}
_0220EE80:
	bl ov00_0220EBCC
	ldmia sp!, {r3, pc}
	arm_func_end ov00_0220EE6C

	arm_func_start ov00_0220EE88
ov00_0220EE88: ; 0x0220EE88
	stmdb sp!, {r3, lr}
	ldr r2, [r0]
	ldrh r0, [r2, #4]
	ldr r2, [r2]
	mul r0, r2, r0
	bl _u32_div_f
	mov r0, r1
	ldmia sp!, {r3, pc}
	arm_func_end ov00_0220EE88

	arm_func_start ov00_0220EEA8
ov00_0220EEA8: ; 0x0220EEA8
	ldr r3, [r0]
	ldr r2, [r1]
	ldr r0, [r3]
	ldr r1, [r2]
	cmp r0, r1
	subne r0, r0, r1
	bxne lr
	ldrh r1, [r3, #4]
	ldrh r0, [r2, #4]
	sub r0, r1, r0
	mov r0, r0, lsl #0x10
	mov r0, r0, asr #0x10
	bx lr
	arm_func_end ov00_0220EEA8

	arm_func_start ov00_0220EEDC
ov00_0220EEDC: ; 0x0220EEDC
	ldr ip, _0220EEE8 ; =ov00_0220CF14
	ldr r0, [r0]
	bx ip
	.balign 4, 0
_0220EEE8: .word ov00_0220CF14
	arm_func_end ov00_0220EEDC

	arm_func_start ov00_0220EEEC
ov00_0220EEEC: ; 0x0220EEEC
	stmdb sp!, {lr}
	sub sp, sp, #0xa4
	add r3, sp, #4
	str r1, [sp, #4]
	strh r2, [sp, #8]
	str r3, [sp]
	ldr r0, [r0, #0xc]
	add r1, sp, #0
	bl ov00_021FF0C0
	cmp r0, #0
	ldrne r0, [r0]
	moveq r0, #0
	add sp, sp, #0xa4
	ldmia sp!, {pc}
	arm_func_end ov00_0220EEEC

	arm_func_start ov00_0220EF24
ov00_0220EF24: ; 0x0220EF24
	stmdb sp!, {r4, r5, r6, r7, lr}
	sub sp, sp, #0x1c
	mov r4, r1
	mov r6, r3
	mov r5, r0
	mov r7, r2
	bl ov00_021FF3CC
	cmp r6, #0
	moveq r6, #0x10000
	cmp r7, #0
	add r1, sp, #0x10
	add r2, sp, #8
	mov r0, r4
	moveq r7, #0x10000
	bl ov00_0220F7D4
	cmp r0, #0
	addeq sp, sp, #0x1c
	moveq r0, #4
	ldmeqia sp!, {r4, r5, r6, r7, pc}
	mov r0, #0x44
	bl ov00_021EC3A8
	movs r4, r0
	addeq sp, sp, #0x1c
	moveq r0, #1
	ldmeqia sp!, {r4, r5, r6, r7, pc}
	mov r1, #0
	mov r2, #0x44
	bl memset
	mvn r0, #0
	str r0, [r4]
	str r6, [r4, #0x3c]
	ldr r1, [sp, #0x30]
	str r7, [r4, #0x38]
	str r1, [r4, #0x24]
	ldr r0, _0220F138 ; =ov00_0220EEA8
	ldr r3, _0220F13C ; =ov00_0220EE88
	str r0, [sp]
	mov ip, #0
	mov r0, #4
	mov r1, #0x20
	mov r2, #2
	str ip, [sp, #4]
	bl ov00_021FED4C
	str r0, [r4, #0xc]
	cmp r0, #0
	bne _0220EFF0
	mov r0, r4
	bl ov00_021EC3D8
	add sp, sp, #0x1c
	mov r0, #1
	ldmia sp!, {r4, r5, r6, r7, pc}
_0220EFF0:
	mov r0, #4
	ldr r2, _0220F140 ; =ov00_0220EEDC
	mov r1, r0
	bl ov00_021FE5B4
	str r0, [r4, #0x10]
	cmp r0, #0
	bne _0220F028
	ldr r0, [r4, #0xc]
	bl ov00_021FEE8C
	mov r0, r4
	bl ov00_021EC3D8
	add sp, sp, #0x1c
	mov r0, #1
	ldmia sp!, {r4, r5, r6, r7, pc}
_0220F028:
	mov r0, #2
	mov r1, r0
	mov r2, #0
	bl ov00_021FF804
	mvn r1, #0
	str r0, [r4]
	cmp r0, r1
	bne _0220F06C
	ldr r0, [r4, #0xc]
	bl ov00_021FEE8C
	ldr r0, [r4, #0x10]
	bl ov00_021FE678
	mov r0, r4
	bl ov00_021EC3D8
	add sp, sp, #0x1c
	mov r0, #3
	ldmia sp!, {r4, r5, r6, r7, pc}
_0220F06C:
	ldrh r0, [sp, #8]
	add r1, sp, #0x14
	mov ip, #0
	mov r2, r0, asr #8
	mov r0, r0, lsl #8
	and r2, r2, #0xff
	and r0, r0, #0xff00
	orr r0, r2, r0
	str ip, [r1]
	mov r2, #2
	strb r2, [sp, #0x15]
	ldr r3, [sp, #0x10]
	str ip, [r1, #4]
	str r3, [sp, #0x18]
	strh r0, [sp, #0x16]
	ldr r0, [r4]
	mov r2, #8
	bl ov00_021FF840
	mvn r1, #0
	cmp r0, r1
	bne _0220F0EC
	ldr r0, [r4]
	bl ov00_021FF818
	ldr r0, [r4, #0xc]
	bl ov00_021FEE8C
	ldr r0, [r4, #0x10]
	bl ov00_021FE678
	mov r0, r4
	bl ov00_021EC3D8
	add sp, sp, #0x1c
	mov r0, #3
	ldmia sp!, {r4, r5, r6, r7, pc}
_0220F0EC:
	mov r0, #8
	str r0, [sp, #0xc]
	ldr r0, [r4]
	add r1, sp, #0x14
	add r2, sp, #0xc
	bl ov00_021FFA18
	ldr r1, [sp, #0x18]
	mov r0, #0
	str r1, [r4, #4]
	ldrh r1, [sp, #0x16]
	mov r2, r1, asr #8
	mov r1, r1, lsl #8
	and r2, r2, #0xff
	and r1, r1, #0xff00
	orr r1, r2, r1
	strh r1, [r4, #8]
	str r4, [r5]
	add sp, sp, #0x1c
	ldmia sp!, {r4, r5, r6, r7, pc}
	.balign 4, 0
_0220F138: .word ov00_0220EEA8
_0220F13C: .word ov00_0220EE88
_0220F140: .word ov00_0220EEDC
	arm_func_end ov00_0220EF24

	arm_func_start ov00_0220F144
ov00_0220F144: ; 0x0220F144
	stmdb sp!, {r4, lr}
	mov r4, r0
	ldr r0, [r4, #0x1c]
	cmp r0, #0
	movne r0, #1
	strne r0, [r4, #0x14]
	ldmneia sp!, {r4, pc}
	ldr r0, [r4]
	bl ov00_021FF818
	ldr r0, [r4, #0xc]
	bl ov00_021FEE8C
	ldr r0, [r4, #0x10]
	bl ov00_021FE678
	mov r0, r4
	bl ov00_021EC3D8
	bl ov00_021FF3D0
	ldmia sp!, {r4, pc}
	arm_func_end ov00_0220F144

	arm_func_start ov00_0220F188
ov00_0220F188: ; 0x0220F188
	str r1, [r0, #0x20]
	bx lr
	arm_func_end ov00_0220F188

	arm_func_start ov00_0220F190
ov00_0220F190: ; 0x0220F190
	ldr ip, _0220F19C ; =ov00_021EC3A8
	mov r0, #0xa0
	bx ip
	.balign 4, 0
_0220F19C: .word ov00_021EC3A8
	arm_func_end ov00_0220F190

	arm_func_start ov00_0220F1A0
ov00_0220F1A0: ; 0x0220F1A0
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	mov r4, r3
	mov r5, r2
	mov r3, #0
	mov r6, r1
	mov r1, r5
	mov r2, r4
	mov r7, r0
	str r3, [sp]
	bl ov00_0220EEEC
	cmp r0, #0
	movne r0, #5
	ldmneia sp!, {r3, r4, r5, r6, r7, pc}
	bl ov00_0220F190
	str r0, [sp]
	cmp r0, #0
	beq _0220F330
	mov r1, #0
	mov r2, #0xa0
	bl memset
	ldr r0, [sp]
	str r5, [r0]
	ldr r0, [sp]
	strh r4, [r0, #4]
	ldr r0, [sp]
	str r7, [r0, #8]
	bl ov00_021FF368
	ldr r2, [sp]
	mov r1, #0
	str r0, [r2, #0x1c]
	ldr r2, [sp]
	ldr r0, [r2, #0x1c]
	str r0, [r2, #0x88]
	ldr r0, [sp]
	strh r1, [r0, #0x64]
	ldr r0, [sp]
	strh r1, [r0, #0x66]
	ldr r0, [sp]
	ldr r1, [r7, #0x3c]
	add r0, r0, #0x44
	bl ov00_0220BF44
	cmp r0, #0
	beq _0220F330
	ldr r0, [sp]
	ldr r1, [r7, #0x38]
	add r0, r0, #0x50
	bl ov00_0220BF44
	cmp r0, #0
	beq _0220F330
	mov r0, #0x10
	mov r1, #0x40
	mov r2, #0
	bl ov00_021FE5B4
	ldr r1, [sp]
	str r0, [r1, #0x5c]
	ldr r0, [sp]
	ldr r0, [r0, #0x5c]
	cmp r0, #0
	beq _0220F330
	mov r0, #0x10
	mov r1, #0x40
	mov r2, #0
	bl ov00_021FE5B4
	ldr r1, [sp]
	str r0, [r1, #0x60]
	ldr r0, [sp]
	ldr r0, [r0, #0x60]
	cmp r0, #0
	beq _0220F330
	mov r0, #4
	mov r1, #2
	mov r2, #0
	bl ov00_021FE5B4
	ldr r1, [sp]
	str r0, [r1, #0x98]
	ldr r0, [sp]
	ldr r0, [r0, #0x98]
	cmp r0, #0
	beq _0220F330
	mov r0, #4
	mov r1, #2
	mov r2, #0
	bl ov00_021FE5B4
	ldr r1, [sp]
	str r0, [r1, #0x9c]
	ldr r0, [sp]
	ldr r0, [r0, #0x9c]
	cmp r0, #0
	beq _0220F330
	ldr r0, [r7, #0xc]
	add r1, sp, #0
	bl ov00_021FEF74
	mov r0, r7
	mov r1, r5
	mov r2, r4
	bl ov00_0220EEEC
	str r0, [r6]
	cmp r0, #0
	movne r0, #0
	ldmneia sp!, {r3, r4, r5, r6, r7, pc}
_0220F330:
	ldr r0, [sp]
	cmp r0, #0
	beq _0220F3A8
	ldr r0, [r0, #0x44]
	bl ov00_021EC3D8
	ldr r0, [sp]
	ldr r0, [r0, #0x50]
	bl ov00_021EC3D8
	ldr r0, [sp]
	ldr r0, [r0, #0x5c]
	cmp r0, #0
	beq _0220F364
	bl ov00_021FE678
_0220F364:
	ldr r0, [sp]
	ldr r0, [r0, #0x60]
	cmp r0, #0
	beq _0220F378
	bl ov00_021FE678
_0220F378:
	ldr r0, [sp]
	ldr r0, [r0, #0x98]
	cmp r0, #0
	beq _0220F38C
	bl ov00_021FE678
_0220F38C:
	ldr r0, [sp]
	ldr r0, [r0, #0x9c]
	cmp r0, #0
	beq _0220F3A0
	bl ov00_021FE678
_0220F3A0:
	ldr r0, [sp]
	bl ov00_021EC3D8
_0220F3A8:
	mov r0, #1
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	arm_func_end ov00_0220F1A0

	arm_func_start ov00_0220F3B0
ov00_0220F3B0: ; 0x0220F3B0
	stmdb sp!, {r0, r1, r2, r3}
	stmdb sp!, {r4, r5, r6, lr}
	ldr r1, [sp, #0x10]
	ldr r0, [r1, #0x14]
	cmp r0, #0
	ldreq r0, [r1, #0x24]
	cmpeq r0, #0
	ldmneia sp!, {r4, r5, r6, lr}
	addne sp, sp, #0x10
	bxne lr
	ldr r0, [r1, #0xc]
	cmp r0, #7
	ldr r0, [r1, #8]
	bne _0220F45C
	ldr r0, [r0, #0x10]
	bl ArrayLength
	mov r4, r0
	mov r5, #0
	cmp r4, #0
	ldmleia sp!, {r4, r5, r6, lr}
	addle sp, sp, #0x10
	bxle lr
_0220F408:
	ldr r6, [sp, #0x10]
	mov r1, r5
	ldr r0, [r6, #8]
	ldr r0, [r0, #0x10]
	bl ov00_021FE6EC
	ldr r0, [r0]
	cmp r6, r0
	bne _0220F444
	ldr r0, [r6, #8]
	mov r1, r5
	ldr r0, [r0, #0x10]
	bl ov00_021FE968
	ldmia sp!, {r4, r5, r6, lr}
	add sp, sp, #0x10
	bx lr
_0220F444:
	add r5, r5, #1
	cmp r5, r4
	blt _0220F408
	ldmia sp!, {r4, r5, r6, lr}
	add sp, sp, #0x10
	bx lr
_0220F45C:
	ldr r0, [r0, #0xc]
	add r1, sp, #0x10
	bl ov00_021FF01C
	ldmia sp!, {r4, r5, r6, lr}
	add sp, sp, #0x10
	bx lr
	arm_func_end ov00_0220F3B0

	arm_func_start ov00_0220F474
ov00_0220F474: ; 0x0220F474
	stmdb sp!, {r0, r1, r2, r3}
	stmdb sp!, {r4, r5, r6, lr}
	sub sp, sp, #0x18
	mov r6, r0
	mov r5, r1
	add r0, sp, #0x34
	add r1, sp, #0x38
	mov r4, r2
	bl ov00_0220F958
	ldr r0, [r6]
	bl ov00_021FF6BC
	cmp r0, #0
	addeq sp, sp, #0x18
	moveq r0, #1
	ldmeqia sp!, {r4, r5, r6, lr}
	addeq sp, sp, #0x10
	bxeq lr
	mov r1, r4, asr #8
	mov r0, r4, lsl #8
	add r2, sp, #0x10
	mov r3, #0
	str r3, [r2, #4]
	and r1, r1, #0xff
	and r0, r0, #0xff00
	orr r0, r1, r0
	str r3, [r2]
	mov r1, #2
	strh r0, [sp, #0x12]
	strb r1, [sp, #0x11]
	str r5, [sp, #0x14]
	str r2, [sp]
	mov r0, #8
	str r0, [sp, #4]
	ldr r0, [r6]
	ldr r1, [sp, #0x34]
	ldr r2, [sp, #0x38]
	bl ov00_021FF984
	mvn r1, #0
	cmp r0, r1
	bne _0220F5A0
	ldr r0, [r6]
	bl ov00_021FFA60
	mvn r2, #0xe
	cmp r0, r2
	bne _0220F554
	mov r0, r6
	mov r1, r5
	mov r2, r4
	bl ov00_0220E490
	cmp r0, #0
	bne _0220F608
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, lr}
	add sp, sp, #0x10
	bx lr
_0220F554:
	sub r1, r2, #0x1b
	cmp r0, r1
	addne r1, r2, #9
	cmpne r0, r1
	addeq sp, sp, #0x18
	moveq r0, #1
	ldmeqia sp!, {r4, r5, r6, lr}
	addeq sp, sp, #0x10
	bxeq lr
	sub r1, r2, #0x14
	cmp r0, r1
	beq _0220F608
	mov r0, r6
	bl ov00_0220F6E8
	add sp, sp, #0x18
	mov r0, #0
	ldmia sp!, {r4, r5, r6, lr}
	add sp, sp, #0x10
	bx lr
_0220F5A0:
	ldr r0, [r6, #0x28]
	cmp r0, #0
	beq _0220F608
	mov r0, r6
	mov r1, r5
	mov r2, r4
	bl ov00_0220EEEC
	mov r1, #0
	str r1, [sp]
	ldr r2, [sp, #0x34]
	mov r1, r0
	str r2, [sp, #4]
	ldr ip, [sp, #0x38]
	mov r3, r4
	mov r0, r6
	mov r2, r5
	str ip, [sp, #8]
	mov r4, #1
	str r4, [sp, #0xc]
	bl ov00_0220C860
	cmp r0, #0
	addeq sp, sp, #0x18
	moveq r0, #0
	ldmeqia sp!, {r4, r5, r6, lr}
	addeq sp, sp, #0x10
	bxeq lr
_0220F608:
	mov r0, #1
	add sp, sp, #0x18
	ldmia sp!, {r4, r5, r6, lr}
	add sp, sp, #0x10
	bx lr
	arm_func_end ov00_0220F474

	arm_func_start ov00_0220F61C
ov00_0220F61C: ; 0x0220F61C
	stmdb sp!, {r4, lr}
	ldr r4, [r0]
	ldr r1, [r1]
	ldr r0, [r4, #0xc]
	cmp r0, #7
	beq _0220F648
	mov r0, r4
	bl ov00_0220CE00
	cmp r0, #0
	moveq r0, #0
	ldmeqia sp!, {r4, pc}
_0220F648:
	ldr r0, [r4, #0xc]
	cmp r0, #7
	ldreq r0, [r4, #0x14]
	cmpeq r0, #0
	ldreq r0, [r4, #0x24]
	cmpeq r0, #0
	bne _0220F66C
	mov r0, r4
	bl ov00_0220F3B0
_0220F66C:
	mov r0, #1
	ldmia sp!, {r4, pc}
	arm_func_end ov00_0220F61C

	arm_func_start ov00_0220F674
ov00_0220F674: ; 0x0220F674
	stmdb sp!, {r3, r4, lr}
	sub sp, sp, #4
	mov r4, r0
	bl ov00_021FF368
	str r0, [sp]
	ldr r0, [r4, #0xc]
	ldr r1, _0220F6AC ; =ov00_0220F61C
	add r2, sp, #0
	bl ov00_021FF264
	cmp r0, #0
	moveq r0, #1
	movne r0, #0
	add sp, sp, #4
	ldmia sp!, {r3, r4, pc}
	.balign 4, 0
_0220F6AC: .word ov00_0220F61C
	arm_func_end ov00_0220F674

	arm_func_start ov00_0220F6B0
ov00_0220F6B0: ; 0x0220F6B0
	stmdb sp!, {r3, r4, r5, lr}
	mov r5, r0
	ldr r0, [r5, #0x10]
	bl ArrayLength
	subs r4, r0, #1
	ldmmiia sp!, {r3, r4, r5, pc}
_0220F6C8:
	ldr r0, [r5, #0x10]
	mov r1, r4
	bl ov00_021FE6EC
	ldr r0, [r0]
	bl ov00_0220F3B0
	subs r4, r4, #1
	bpl _0220F6C8
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end ov00_0220F6B0

	arm_func_start ov00_0220F6E8
ov00_0220F6E8: ; 0x0220F6E8
	stmdb sp!, {r4, lr}
	mov r4, r0
	ldr r1, [r4, #0x18]
	cmp r1, #0
	ldmneia sp!, {r4, pc}
	mov r1, #1
	str r1, [r4, #0x18]
	bl ov00_0220D238
	mov r0, r4
	bl ov00_0220C164
	cmp r0, #0
	ldmeqia sp!, {r4, pc}
	mov r0, r4
	bl ov00_0220F144
	ldmia sp!, {r4, pc}
	arm_func_end ov00_0220F6E8

	arm_func_start ov00_0220F724
ov00_0220F724: ; 0x0220F724
	stmdb sp!, {r3, r4, r5, lr}
	movs r4, r2
	mov r5, r1
	bne _0220F750
	ldr r2, _0220F7C0 ; =ov00_0221B634
	ldr r3, _0220F7C4 ; =ov00_0221B638
	ldr ip, [r2]
	mov r1, #0x16
	eor ip, ip, #1
	mla r4, ip, r1, r3
	str ip, [r2]
_0220F750:
	cmp r0, #0
	beq _0220F798
	str r0, [sp]
	cmp r5, #0
	beq _0220F780
	bl SOC_InetNtoA
	mov r2, r0
	ldr r1, _0220F7C8 ; =ov00_02219CE4
	mov r0, r4
	mov r3, r5
	bl sprintf
	b _0220F7B8
_0220F780:
	bl SOC_InetNtoA
	mov r2, r0
	ldr r1, _0220F7CC ; =ov00_02219CEC
	mov r0, r4
	bl sprintf
	b _0220F7B8
_0220F798:
	cmp r5, #0
	moveq r0, #0
	streqb r0, [r4]
	beq _0220F7B8
	ldr r1, _0220F7D0 ; =ov00_02219CF0
	mov r0, r4
	mov r2, r5
	bl sprintf
_0220F7B8:
	mov r0, r4
	ldmia sp!, {r3, r4, r5, pc}
	.balign 4, 0
_0220F7C0: .word ov00_0221B634
_0220F7C4: .word ov00_0221B638
_0220F7C8: .word ov00_02219CE4
_0220F7CC: .word ov00_02219CEC
_0220F7D0: .word ov00_02219CF0
	arm_func_end ov00_0220F724

	arm_func_start ov00_0220F7D4
ov00_0220F7D4: ; 0x0220F7D4
	stmdb sp!, {r4, r5, r6, r7, r8, sb, lr}
	sub sp, sp, #0x104
	movs r8, r0
	mov r7, r1
	ldrnesb r1, [r8]
	mov r6, r2
	cmpne r1, #0
	moveq r4, #0
	moveq r5, r4
	beq _0220F928
	mov r1, #0x3a
	bl strchr
	movs r5, r0
	moveq r5, #0
	beq _0220F8E4
	cmp r5, r8
	moveq r8, #0
	moveq r4, r8
	beq _0220F85C
	sub sb, r5, r8
	cmp sb, #0x100
	blt _0220F840
	ldr r0, _0220F944 ; =ov00_02219CF4
	ldr r1, _0220F948 ; =ov00_02219D14
	ldr r2, _0220F94C ; =ov00_02219CD0
	mov r3, #0x81
	bl __msl_assertion_failed
_0220F840:
	add r0, sp, #0
	mov r1, r8
	mov r2, sb
	bl memcpy
	add r8, sp, #0
	mov r0, #0
	strb r0, [r8, sb]
_0220F85C:
	ldrsb r0, [r5, #1]
	add r3, r5, #1
	cmp r0, #0
	beq _0220F8B4
	ldr r1, _0220F950 ; =__msl_digit
	mov r2, #0
_0220F874:
	cmp r0, #0
	blt _0220F884
	cmp r0, #0x80
	blt _0220F88C
_0220F884:
	mov r0, r2
	b _0220F898
_0220F88C:
	mov r0, r0, lsl #1
	ldrh r0, [r1, r0]
	and r0, r0, #8
_0220F898:
	cmp r0, #0
	addeq sp, sp, #0x104
	moveq r0, #0
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, pc}
	ldrsb r0, [r3, #1]!
	cmp r0, #0
	bne _0220F874
_0220F8B4:
	add r0, r5, #1
	bl atol
	cmp r0, #0
	blt _0220F8D0
	ldr r1, _0220F954 ; =0x0000FFFF
	cmp r0, r1
	ble _0220F8DC
_0220F8D0:
	add sp, sp, #0x104
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, sb, pc}
_0220F8DC:
	mov r0, r0, lsl #0x10
	mov r5, r0, lsr #0x10
_0220F8E4:
	cmp r8, #0
	beq _0220F928
	mov r0, r8
	bl ov00_021FFA44
	mov r4, r0
	mvn r0, #0
	cmp r4, r0
	bne _0220F928
	mov r0, r8
	bl SOC_GetHostByName
	cmp r0, #0
	addeq sp, sp, #0x104
	moveq r0, #0
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, pc}
	ldr r0, [r0, #0xc]
	ldr r0, [r0]
	ldr r4, [r0]
_0220F928:
	cmp r7, #0
	strne r4, [r7]
	cmp r6, #0
	strneh r5, [r6]
	mov r0, #1
	add sp, sp, #0x104
	ldmia sp!, {r4, r5, r6, r7, r8, sb, pc}
	.balign 4, 0
_0220F944: .word ov00_02219CF4
_0220F948: .word ov00_02219D14
_0220F94C: .word ov00_02219CD0
_0220F950: .word __msl_digit
_0220F954: .word 0x0000FFFF
	arm_func_end ov00_0220F7D4

	arm_func_start ov00_0220F958
ov00_0220F958: ; 0x0220F958
	stmdb sp!, {r4, lr}
	ldr r2, [r0]
	mov r4, r1
	cmp r2, #0
	bne _0220F980
	ldr r2, _0220F9A4 ; =ov00_02219D24
	mov r1, #0
	str r2, [r0]
	str r1, [r4]
	ldmia sp!, {r4, pc}
_0220F980:
	ldr r1, [r4]
	mvn r0, #0
	cmp r1, r0
	ldmneia sp!, {r4, pc}
	mov r0, r2
	bl strlen
	add r0, r0, #1
	str r0, [r4]
	ldmia sp!, {r4, pc}
	.balign 4, 0
_0220F9A4: .word ov00_02219D24
	arm_func_end ov00_0220F958

	arm_func_start ov00_0220F9A8
ov00_0220F9A8: ; 0x0220F9A8
	stmdb sp!, {r4, r5, r6, lr}
	ldr r1, _0220FA0C ; =ov00_0221B664
	mov r6, r0
	ldr r0, [r1, #0x10]
	cmp r0, #0
	moveq r0, #0
	ldmeqia sp!, {r4, r5, r6, pc}
	mov r5, #0
	bl ArrayLength
	cmp r0, #0
	ble _0220FA04
	ldr r4, _0220FA0C ; =ov00_0221B664
_0220F9D8:
	ldr r0, [r4, #0x10]
	mov r1, r5
	bl ov00_021FE6EC
	ldr r1, [r0, #8]
	cmp r1, r6
	ldmeqia sp!, {r4, r5, r6, pc}
	ldr r0, [r4, #0x10]
	add r5, r5, #1
	bl ArrayLength
	cmp r5, r0
	blt _0220F9D8
_0220FA04:
	mov r0, #0
	ldmia sp!, {r4, r5, r6, pc}
	.balign 4, 0
_0220FA0C: .word ov00_0221B664
	arm_func_end ov00_0220F9A8

	arm_func_start ov00_0220FA10
ov00_0220FA10: ; 0x0220FA10
	stmdb sp!, {r4, lr}
	mov r4, r0
	ldr r0, [r4]
	mvn r1, #0
	cmp r0, r1
	beq _0220FA2C
	bl ov00_021FF818
_0220FA2C:
	mvn r0, #0
	str r0, [r4]
	mov r0, #4
	str r0, [r4, #0x10]
	ldmia sp!, {r4, pc}
	arm_func_end ov00_0220FA10

	arm_func_start ov00_0220FA40
ov00_0220FA40: ; 0x0220FA40
	stmdb sp!, {r3, lr}
	sub sp, sp, #0x40
	mov r0, #0
	add r3, sp, #0
	mov r1, r0
	stmia r3!, {r0, r1}
	stmia r3!, {r0, r1}
	stmia r3!, {r0, r1}
	stmia r3!, {r0, r1}
	stmia r3!, {r0, r1}
	stmia r3!, {r0, r1}
	ldr r2, _0220FAD0 ; =ov00_0221B664
	stmia r3!, {r0, r1}
	ldr r2, [r2, #0x10]
	stmia r3, {r0, r1}
	cmp r2, #0
	bne _0220FA9C
	ldr r2, _0220FAD4 ; =ov00_0220FA10
	mov r0, #0x40
	mov r1, #4
	bl ov00_021FE5B4
	ldr r1, _0220FAD0 ; =ov00_0221B664
	str r0, [r1, #0x10]
_0220FA9C:
	ldr r0, _0220FAD0 ; =ov00_0221B664
	add r1, sp, #0
	ldr r0, [r0, #0x10]
	bl ov00_021FE754
	ldr r0, _0220FAD0 ; =ov00_0221B664
	ldr r0, [r0, #0x10]
	bl ArrayLength
	ldr r2, _0220FAD0 ; =ov00_0221B664
	sub r1, r0, #1
	ldr r0, [r2, #0x10]
	bl ov00_021FE6EC
	add sp, sp, #0x40
	ldmia sp!, {r3, pc}
	.balign 4, 0
_0220FAD0: .word ov00_0221B664
_0220FAD4: .word ov00_0220FA10
	arm_func_end ov00_0220FA40

	arm_func_start ov00_0220FAD8
ov00_0220FAD8: ; 0x0220FAD8
	stmdb sp!, {r4, r5, r6, lr}
	ldr r1, _0220FB3C ; =ov00_0221B664
	mov r6, r0
	ldr r0, [r1, #0x10]
	mov r5, #0
	bl ArrayLength
	cmp r0, #0
	ldmleia sp!, {r4, r5, r6, pc}
	ldr r4, _0220FB3C ; =ov00_0221B664
_0220FAFC:
	ldr r0, [r4, #0x10]
	mov r1, r5
	bl ov00_021FE6EC
	cmp r6, r0
	bne _0220FB24
	ldr r0, _0220FB3C ; =ov00_0221B664
	mov r1, r5
	ldr r0, [r0, #0x10]
	bl ov00_021FE8D8
	ldmia sp!, {r4, r5, r6, pc}
_0220FB24:
	ldr r0, [r4, #0x10]
	add r5, r5, #1
	bl ArrayLength
	cmp r5, r0
	blt _0220FAFC
	ldmia sp!, {r4, r5, r6, pc}
	.balign 4, 0
_0220FB3C: .word ov00_0221B664
	arm_func_end ov00_0220FAD8

	arm_func_start ov00_0220FB40
ov00_0220FB40: ; 0x0220FB40
	stmdb sp!, {r3, lr}
	ldr r0, _0220FB68 ; =ov00_0221B664
	ldr r0, [r0, #0x10]
	cmp r0, #0
	ldmeqia sp!, {r3, pc}
	bl ov00_021FE678
	ldr r0, _0220FB68 ; =ov00_0221B664
	mov r1, #0
	str r1, [r0, #0x10]
	ldmia sp!, {r3, pc}
	.balign 4, 0
_0220FB68: .word ov00_0221B664
	arm_func_end ov00_0220FB40

	arm_func_start ov00_0220FB6C
ov00_0220FB6C: ; 0x0220FB6C
	stmdb sp!, {r3, lr}
	ldr r1, _0220FB8C ; =ov00_02219D28
	mov r2, #6
	bl memcmp
	cmp r0, #0
	moveq r0, #1
	movne r0, #0
	ldmia sp!, {r3, pc}
	.balign 4, 0
_0220FB8C: .word ov00_02219D28
	arm_func_end ov00_0220FB6C

	arm_func_start ov00_0220FB90
ov00_0220FB90: ; 0x0220FB90
	stmdb sp!, {r3, lr}
	sub sp, sp, #0x10
	mov ip, r2, asr #8
	mov r2, r2, lsl #8
	str r1, [sp, #0xc]
	mov r1, r3
	and r3, ip, #0xff
	and r2, r2, #0xff00
	orr r2, r3, r2
	mov lr, #2
	strh r2, [sp, #0xa]
	add r3, sp, #8
	strb lr, [sp, #9]
	str r3, [sp]
	mov ip, #8
	ldr r2, [sp, #0x18]
	mov r3, #0
	str ip, [sp, #4]
	bl ov00_021FF984
	add sp, sp, #0x10
	ldmia sp!, {r3, pc}
	arm_func_end ov00_0220FB90

	arm_func_start ov00_0220FBE4
ov00_0220FBE4: ; 0x0220FBE4
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	mov r7, #0
	bl ov00_021FF6E4
	movs r6, r0
	moveq r0, r7
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	ldr r4, _0220FC44 ; =0x0100007F
	mov r5, r7
_0220FC04:
	ldr r0, [r6, #0xc]
	ldr r0, [r0, r5, lsl #2]
	cmp r0, #0
	beq _0220FC3C
	ldr r1, [r0]
	cmp r1, r4
	beq _0220FC34
	mov r7, r1
	bl ov00_021FF770
	cmp r0, #0
	movne r0, r7
	ldmneia sp!, {r3, r4, r5, r6, r7, pc}
_0220FC34:
	add r5, r5, #1
	b _0220FC04
_0220FC3C:
	mov r0, r7
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.balign 4, 0
_0220FC44: .word 0x0100007F
	arm_func_end ov00_0220FBE4

	arm_func_start ov00_0220FC48
ov00_0220FC48: ; 0x0220FC48
	stmdb sp!, {lr}
	sub sp, sp, #0xc
	mov r3, #8
	add r1, sp, #4
	add r2, sp, #0
	str r3, [sp]
	bl ov00_021FFA18
	mvn r1, #0
	cmp r0, r1
	moveq r0, #0
	ldrneh r0, [sp, #6]
	add sp, sp, #0xc
	ldmia sp!, {pc}
	arm_func_end ov00_0220FC48

	arm_func_start ov00_0220FC7C
ov00_0220FC7C: ; 0x0220FC7C
	stmdb sp!, {r3, r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0x5c
	ldr r2, _0220FEB4 ; =ov00_02219D28
	mov r1, #0
	ldrb lr, [r2]
	ldrb ip, [r2, #1]
	add r5, sp, #4
	ldrb r8, [r2, #2]
	ldrb r7, [r2, #3]
	ldrb r4, [r2, #4]
	ldrb r3, [r2, #5]
	mov r2, #2
	mov r6, r0
	strb lr, [r5]
	strb ip, [r5, #1]
	strb r8, [r5, #2]
	strb r7, [r5, #3]
	strb r4, [r5, #4]
	strb r3, [r5, #5]
	strb r1, [r5, #7]
	strb r2, [r5, #6]
	ldr r2, [r6, #0xc]
	sub r0, r1, #1
	strb r2, [r5, #0xd]
	ldr r7, [r6, #8]
	mov r3, r7, lsr #0x18
	mov r2, r7, lsr #8
	mov r4, r7, lsl #8
	and r3, r3, #0xff
	and r2, r2, #0xff00
	mov r7, r7, lsl #0x18
	orr r2, r3, r2
	and r4, r4, #0xff0000
	and r3, r7, #0xff000000
	orr r2, r4, r2
	orr r2, r3, r2
	str r2, [r5, #8]
	ldr r2, [r6, #4]
	cmp r2, r0
	movne r1, #1
	strb r1, [r5, #0xe]
	bl ov00_0220FBE4
	mov r7, r0
	bl ov00_0220FBE4
	mov r8, r0
	bl ov00_0220FBE4
	mov r4, r0
	bl ov00_0220FBE4
	mov r2, #0
	mov r1, r7, lsl #0x18
	and r7, r1, #0xff000000
	mov r1, r8, lsl #8
	mov r0, r0, lsr #8
	and r3, r1, #0xff0000
	mov r1, r4, lsr #0x18
	and r1, r1, #0xff
	and r0, r0, #0xff00
	orr r0, r1, r0
	orr r0, r3, r0
	orr r3, r7, r0
	mov r1, r3, lsr #0x18
	strb r1, [sp, #0x13]
	mov r1, r3, lsr #8
	mov r0, r3, lsr #0x10
	strb r1, [sp, #0x15]
	strb r0, [sp, #0x14]
	ldr r1, _0220FEB8 ; =ov00_0221B16C
	add r0, sp, #0x19
	strb r3, [sp, #0x16]
	strb r2, [sp, #0x17]
	strb r2, [sp, #0x18]
	bl strcpy
	ldr r0, _0220FEB8 ; =ov00_0221B16C
	bl strlen
	ldrb r1, [r5, #0xe]
	add r4, r0, #0x16
	cmp r1, #0
	beq _0220FDE4
	ldr r0, [r6, #0x14]
	cmp r0, #0
	bne _0220FDE4
	mov r0, #0
	strb r0, [r5, #0xc]
	str r4, [sp]
	ldr r1, _0220FEBC ; =ov00_0221B664
	ldr r0, [r6, #4]
	ldr r1, [r1, #0xc]
	ldr r2, _0220FEC0 ; =0x00006CFD
	mov r3, r5
	bl ov00_0220FB90
_0220FDE4:
	ldr r0, [r6, #0x18]
	cmp r0, #0
	bne _0220FE14
	mov r0, #1
	strb r0, [r5, #0xc]
	str r4, [sp]
	ldr r1, _0220FEBC ; =ov00_0221B664
	ldr r0, [r6]
	ldr r1, [r1, #0xc]
	ldr r2, _0220FEC0 ; =0x00006CFD
	mov r3, r5
	bl ov00_0220FB90
_0220FE14:
	ldrb r0, [r5, #0xe]
	cmp r0, #0
	ldrne r8, [r6, #4]
	ldreq r8, [r6]
	cmp r0, #0
	ldrne r0, [r6, #4]
	ldreq r0, [r6]
	bl ov00_0220FC48
	mov r7, r0
	mov r0, r8
	bl ov00_0220FC48
	mov r1, r7, asr #8
	mov r0, r0, lsl #8
	and r1, r1, #0xff
	and r0, r0, #0xff00
	orr r0, r1, r0
	mov r0, r0, lsl #0x10
	mov r1, r0, lsr #0x10
	mov r0, r1, asr #8
	strb r1, [sp, #0x18]
	strb r0, [sp, #0x17]
	ldr r0, [r6, #0x1c]
	cmp r0, #0
	bne _0220FE98
	mov r0, #2
	strb r0, [r5, #0xc]
	str r4, [sp]
	ldr r1, _0220FEBC ; =ov00_0221B664
	ldr r0, [r6]
	ldr r1, [r1, #8]
	ldr r2, _0220FEC0 ; =0x00006CFD
	mov r3, r5
	bl ov00_0220FB90
_0220FE98:
	bl ov00_021FF368
	add r0, r0, #0x1f4
	str r0, [r6, #0x28]
	mov r0, #0x1e
	str r0, [r6, #0x24]
	add sp, sp, #0x5c
	ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
	.balign 4, 0
_0220FEB4: .word ov00_02219D28
_0220FEB8: .word ov00_0221B16C
_0220FEBC: .word ov00_0221B664
_0220FEC0: .word 0x00006CFD
	arm_func_end ov00_0220FC7C

	arm_func_start ov00_0220FEC4
ov00_0220FEC4: ; 0x0220FEC4
	stmdb sp!, {r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0x18
	ldr r1, _0220FFE0 ; =ov00_02219D28
	mov r2, #2
	ldrb r7, [r1]
	ldrb r6, [r1, #1]
	add r8, sp, #4
	ldrb r5, [r1, #2]
	ldrb lr, [r1, #3]
	ldrb ip, [r1, #4]
	ldrb r3, [r1, #5]
	mov r1, #7
	mov r4, r0
	strb r7, [r8]
	strb r6, [r8, #1]
	strb r5, [r8, #2]
	strb lr, [r8, #3]
	strb ip, [r8, #4]
	strb r2, [sp, #0xa]
	strb r1, [sp, #0xb]
	strb r3, [r8, #5]
	ldr r3, [r4, #8]
	mov r1, r3, lsr #0x18
	mov r0, r3, lsr #8
	mov r2, r3, lsl #8
	and r1, r1, #0xff
	and r0, r0, #0xff00
	mov r3, r3, lsl #0x18
	orr r0, r1, r0
	and r2, r2, #0xff0000
	and r1, r3, #0xff000000
	orr r0, r2, r0
	orr r0, r1, r0
	str r0, [sp, #0xc]
	ldr r0, [r4, #0x2c]
	add r3, sp, #4
	str r0, [sp, #0x10]
	ldrh r0, [r4, #0x30]
	mov r1, r0, asr #8
	mov r0, r0, lsl #8
	and r1, r1, #0xff
	and r0, r0, #0xff00
	orr r0, r1, r0
	strh r0, [sp, #0x14]
	ldrb r0, [r4, #0x32]
	mvn r1, #0
	strb r0, [sp, #0x16]
	ldr r0, [r4, #0x10]
	cmp r0, #2
	movne r0, #1
	moveq r0, #0
	strb r0, [sp, #0x17]
	ldr r0, [r4, #4]
	cmp r0, r1
	ldreq r0, [r4]
	mov r1, #0x14
	str r1, [sp]
	ldrh r2, [r4, #0x30]
	ldr r1, [r4, #0x2c]
	bl ov00_0220FB90
	bl ov00_021FF368
	add r0, r0, #0x2bc
	str r0, [r4, #0x28]
	mov r0, #0xc
	str r0, [r4, #0x24]
	ldrb r0, [r4, #0x32]
	cmp r0, #0
	movne r0, #1
	strneb r0, [r4, #0x33]
	add sp, sp, #0x18
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	.balign 4, 0
_0220FFE0: .word ov00_02219D28
	arm_func_end ov00_0220FEC4

	arm_func_start ov00_0220FFE4
ov00_0220FFE4: ; 0x0220FFE4
	stmdb sp!, {r4, lr}
	mov r4, r0
	bl ov00_021FFA44
	mvn r1, #0
	cmp r0, r1
	ldmneia sp!, {r4, pc}
	mov r0, r4
	bl SOC_GetHostByName
	cmp r0, #0
	moveq r0, #0
	ldrne r0, [r0, #0xc]
	ldrne r0, [r0]
	ldrne r0, [r0]
	ldmia sp!, {r4, pc}
	arm_func_end ov00_0220FFE4

	arm_func_start ov00_0221001C
ov00_0221001C: ; 0x0221001C
	stmdb sp!, {lr}
	sub sp, sp, #0x84
	cmp r0, #0
	bne _02210048
	ldr r2, _02210054 ; =ov00_02219D30
	str r1, [sp]
	ldr r3, _02210058 ; =ov00_0221B16C
	add r0, sp, #4
	mov r1, #0x80
	bl snprintf
	add r0, sp, #4
_02210048:
	bl ov00_0220FFE4
	add sp, sp, #0x84
	ldmia sp!, {pc}
	.balign 4, 0
_02210054: .word ov00_02219D30
_02210058: .word ov00_0221B16C
	arm_func_end ov00_0221001C

	arm_func_start ov00_0221005C
ov00_0221005C: ; 0x0221005C
	stmdb sp!, {r3, lr}
	ldr r0, _022100C8 ; =ov00_0221B664
	ldr r1, [r0, #0xc]
	cmp r1, #0
	bne _02210084
	ldr r0, [r0, #4]
	ldr r1, _022100CC ; =ov00_02219D38
	bl ov00_0221001C
	ldr r1, _022100C8 ; =ov00_0221B664
	str r0, [r1, #0xc]
_02210084:
	ldr r0, _022100C8 ; =ov00_0221B664
	ldr r1, [r0, #8]
	cmp r1, #0
	bne _022100A8
	ldr r0, [r0]
	ldr r1, _022100D0 ; =ov00_02219D54
	bl ov00_0221001C
	ldr r1, _022100C8 ; =ov00_0221B664
	str r0, [r1, #8]
_022100A8:
	ldr r0, _022100C8 ; =ov00_0221B664
	ldr r1, [r0, #0xc]
	cmp r1, #0
	ldrne r0, [r0, #8]
	cmpne r0, #0
	moveq r0, #0
	movne r0, #1
	ldmia sp!, {r3, pc}
	.balign 4, 0
_022100C8: .word ov00_0221B664
_022100CC: .word ov00_02219D38
_022100D0: .word ov00_02219D54
	arm_func_end ov00_0221005C

	arm_func_start ov00_022100D4
ov00_022100D4: ; 0x022100D4
	stmdb sp!, {r4, r5, r6, r7, r8, lr}
	ldr ip, _02210198 ; =ov00_0221B0D0
	mov r8, r0
	ldr r0, [ip]
	mov r7, r1
	cmp r0, #1
	mov r6, r2
	mov r5, r3
	movne r0, #2
	ldmneia sp!, {r4, r5, r6, r7, r8, pc}
	bl ov00_0221005C
	cmp r0, #0
	moveq r0, #3
	ldmeqia sp!, {r4, r5, r6, r7, r8, pc}
	bl ov00_0220FA40
	movs r4, r0
	moveq r0, #1
	ldmeqia sp!, {r4, r5, r6, r7, r8, pc}
	str r8, [r4, #4]
	str r6, [r4, #0xc]
	str r7, [r4, #8]
	ldr r1, [sp, #0x18]
	str r5, [r4, #0x34]
	ldr r2, [sp, #0x1c]
	str r1, [r4, #0x38]
	mov r0, #2
	str r2, [r4, #0x3c]
	mov r1, r0
	mov r2, #0
	bl ov00_021FF804
	mov r2, #0
	str r0, [r4]
	str r2, [r4, #0x20]
	strb r2, [r4, #0x32]
	strb r2, [r4, #0x33]
	str r2, [r4, #0x2c]
	strh r2, [r4, #0x30]
	str r2, [r4, #0x24]
	ldr r1, [r4]
	sub r0, r2, #1
	cmp r1, r0
	mov r0, r4
	bne _0221018C
	bl ov00_0220FAD8
	mov r0, #2
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_0221018C:
	bl ov00_0220FC7C
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	.balign 4, 0
_02210198: .word ov00_0221B0D0
	arm_func_end ov00_022100D4

	arm_func_start ov00_0221019C
ov00_0221019C: ; 0x0221019C
	stmdb sp!, {r4, lr}
	bl ov00_0220F9A8
	movs r4, r0
	ldmeqia sp!, {r4, pc}
	ldr r0, [r4]
	mvn r1, #0
	cmp r0, r1
	beq _022101C0
	bl ov00_021FF818
_022101C0:
	mvn r0, #0
	str r0, [r4]
	mov r0, #4
	str r0, [r4, #0x10]
	ldmia sp!, {r4, pc}
	arm_func_end ov00_0221019C

	arm_func_start ov00_022101D4
ov00_022101D4: ; 0x022101D4
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, lr}
	sub sp, sp, #0x1c
	mov r2, #8
	mov sb, r0
	str r2, [sp, #0x10]
	ldr r1, [sb, #0x10]
	cmp r1, #4
	bne _02210200
	bl ov00_0220FAD8
	add sp, sp, #0x1c
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, pc}
_02210200:
	ldr r0, [sb]
	sub r1, r2, #9
	cmp r0, r1
	beq _0221027C
	ldr r6, _022103C0 ; =ov00_0221B678
	add r8, sp, #0x14
	add r7, sp, #0x10
	mov r5, #0x200
	mov r4, #0
	mvn sl, #0
_02210228:
	bl ov00_021FF694
	cmp r0, #0
	beq _0221027C
	str r8, [sp]
	str r7, [sp, #4]
	ldr r0, [sb]
	mov r1, r6
	mov r2, r5
	mov r3, r4
	bl ov00_021FF940
	mov r1, r0
	cmp r1, sl
	beq _0221027C
	mov r0, r6
	mov r2, r8
	bl ov00_0221075C
	ldr r0, [sb, #0x10]
	cmp r0, #4
	ldrne r0, [sb]
	cmpne r0, sl
	bne _02210228
_0221027C:
	ldr r0, [sb, #0x10]
	cmp r0, #0
	cmpne r0, #2
	bne _022102F4
	bl ov00_021FF368
	ldr r1, [sb, #0x28]
	cmp r0, r1
	bls _022102F4
	ldr r1, [sb, #0x20]
	ldr r0, [sb, #0x24]
	cmp r1, r0
	ble _022102D0
	mov r0, #2
	ldr r3, [sb, #0x3c]
	ldr r4, [sb, #0x38]
	sub r1, r0, #3
	mov r2, #0
	blx r4
	ldr r0, [sb, #8]
	bl ov00_0221019C
	b _022102F4
_022102D0:
	add r0, r1, #1
	str r0, [sb, #0x20]
	ldr r0, [sb, #0x10]
	cmp r0, #0
	mov r0, sb
	bne _022102F0
	bl ov00_0220FC7C
	b _022102F4
_022102F0:
	bl ov00_0220FEC4
_022102F4:
	ldr r0, [sb, #0x10]
	cmp r0, #3
	bne _02210374
	bl ov00_021FF368
	ldr r1, [sb, #0x28]
	cmp r0, r1
	bls _02210374
	ldr r1, [sb, #4]
	mvn r0, #0
	cmp r1, r0
	bne _0221036C
	mov r0, #2
	strb r0, [sp, #9]
	ldrh r1, [sb, #0x30]
	add r2, sp, #8
	mov r0, #0
	mov r3, r1, asr #8
	mov r1, r1, lsl #8
	and r3, r3, #0xff
	and r1, r1, #0xff00
	orr r1, r3, r1
	strh r1, [sp, #0xa]
	ldr r1, [sb, #0x2c]
	str r1, [sp, #0xc]
	ldr r1, [sb]
	ldr r3, [sb, #0x3c]
	ldr r4, [sb, #0x38]
	blx r4
	mvn r0, #0
	str r0, [sb]
_0221036C:
	ldr r0, [sb, #8]
	bl ov00_0221019C
_02210374:
	ldr r0, [sb, #0x10]
	cmp r0, #1
	addne sp, sp, #0x1c
	ldmneia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, pc}
	bl ov00_021FF368
	ldr r1, [sb, #0x28]
	cmp r0, r1
	addls sp, sp, #0x1c
	ldmlsia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, pc}
	mov r0, #1
	ldr r3, [sb, #0x3c]
	ldr r4, [sb, #0x38]
	sub r1, r0, #2
	mov r2, #0
	blx r4
	ldr r0, [sb, #8]
	bl ov00_0221019C
	add sp, sp, #0x1c
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, pc}
	.balign 4, 0
_022103C0: .word ov00_0221B678
	arm_func_end ov00_022101D4

	arm_func_start ov00_022103C4
ov00_022103C4: ; 0x022103C4
	stmdb sp!, {r3, r4, r5, lr}
	ldr r0, _02210404 ; =ov00_0221B664
	ldr r0, [r0, #0x10]
	cmp r0, #0
	ldmeqia sp!, {r3, r4, r5, pc}
	bl ArrayLength
	subs r5, r0, #1
	ldmmiia sp!, {r3, r4, r5, pc}
	ldr r4, _02210404 ; =ov00_0221B664
_022103E8:
	ldr r0, [r4, #0x10]
	mov r1, r5
	bl ov00_021FE6EC
	bl ov00_022101D4
	subs r5, r5, #1
	bpl _022103E8
	ldmia sp!, {r3, r4, r5, pc}
	.balign 4, 0
_02210404: .word ov00_0221B664
	arm_func_end ov00_022103C4

	arm_func_start ov00_02210408
ov00_02210408: ; 0x02210408
	stmdb sp!, {r3, r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0x1c
	ldr r2, _022104D0 ; =ov00_02219D28
	mov r4, #2
	ldrb r8, [r2]
	ldrb r7, [r2, #1]
	add r3, sp, #4
	ldrb r6, [r2, #2]
	ldrb lr, [r2, #3]
	ldrb ip, [r2, #4]
	ldrb r5, [r2, #5]
	mov r2, #6
	strb r2, [sp, #0xb]
	strb r8, [r3]
	strb r7, [r3, #1]
	strb r6, [r3, #2]
	strb lr, [r3, #3]
	strb ip, [r3, #4]
	strb r5, [r3, #5]
	strb r4, [sp, #0xa]
	ldr r4, [r0, #0xc]
	mov r2, #0x15
	strb r4, [sp, #0x11]
	ldr r5, [r0, #8]
	mov lr, r5, lsr #0x18
	mov ip, r5, lsr #8
	mov r4, r5, lsl #8
	mov r5, r5, lsl #0x18
	and lr, lr, #0xff
	and ip, ip, #0xff00
	and r4, r4, #0xff0000
	orr ip, lr, ip
	and r5, r5, #0xff000000
	orr r4, r4, ip
	orr r4, r5, r4
	str r4, [sp, #0xc]
	ldrh lr, [r1, #2]
	str r2, [sp]
	mov ip, lr, asr #8
	mov r2, lr, lsl #8
	and ip, ip, #0xff
	and r2, r2, #0xff00
	orr r2, ip, r2
	mov r2, r2, lsl #0x10
	ldr r0, [r0]
	ldr r1, [r1, #4]
	mov r2, r2, lsr #0x10
	bl ov00_0220FB90
	add sp, sp, #0x1c
	ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
	.balign 4, 0
_022104D0: .word ov00_02219D28
	arm_func_end ov00_02210408

	arm_func_start ov00_022104D4
ov00_022104D4: ; 0x022104D4
	stmdb sp!, {r3, r4, r5, lr}
	mov r5, r1
	ldrb r1, [r5, #0x13]
	mov r4, r0
	cmp r1, #0
	bne _022104F4
	mov r1, r2
	bl ov00_02210408
_022104F4:
	ldr r0, [r4, #0x10]
	cmp r0, #2
	ldmgeia sp!, {r3, r4, r5, pc}
	ldrb r1, [r5, #0x13]
	cmp r1, #0
	beq _02210544
	mov r0, #3
	cmp r1, #1
	moveq r0, #1
	beq _02210524
	cmp r1, #2
	moveq r0, #2
_02210524:
	ldr r3, [r4, #0x3c]
	ldr ip, [r4, #0x38]
	mvn r1, #0
	mov r2, #0
	blx ip
	ldr r0, [r4, #8]
	bl ov00_0221019C
	ldmia sp!, {r3, r4, r5, pc}
_02210544:
	ldr r1, [r5, #0xc]
	mov r0, #2
	str r1, [r4, #0x2c]
	ldrh r2, [r5, #0x10]
	mov r1, #0
	mov r3, r2, asr #8
	mov r2, r2, lsl #8
	and r3, r3, #0xff
	and r2, r2, #0xff00
	orr r2, r3, r2
	strh r2, [r4, #0x30]
	str r1, [r4, #0x20]
	str r0, [r4, #0x10]
	ldr r1, [r4, #0x3c]
	ldr r2, [r4, #0x34]
	blx r2
	mov r0, r4
	bl ov00_0220FEC4
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end ov00_022104D4

	arm_func_start ov00_02210590
ov00_02210590: ; 0x02210590
	stmdb sp!, {r3, r4, r5, lr}
	mov r5, r0
	ldr r3, [r5, #0x10]
	mov r4, r2
	cmp r3, #2
	ldmltia sp!, {r3, r4, r5, pc}
	ldr r3, [r4, #4]
	mov r2, #1
	str r3, [r5, #0x2c]
	ldrh r3, [r4, #2]
	mov ip, r3, asr #8
	mov r3, r3, lsl #8
	and ip, ip, #0xff
	and r3, r3, #0xff00
	orr r3, ip, r3
	strh r3, [r5, #0x30]
	strb r2, [r5, #0x32]
	ldrb r2, [r1, #0x12]
	cmp r2, #0
	bne _022105E8
	bl ov00_0220FEC4
	ldmia sp!, {r3, r4, r5, pc}
_022105E8:
	ldr r2, [r5, #0x10]
	cmp r2, #2
	bne _02210644
	ldrb r1, [r5, #0x33]
	cmp r1, #0
	bne _02210604
	bl ov00_0220FEC4
_02210604:
	mov r0, #3
	str r0, [r5, #0x10]
	bl ov00_021FF368
	add r0, r0, #0x388
	add r0, r0, #0x1000
	str r0, [r5, #0x28]
	ldr r1, [r5, #4]
	mvn r0, #0
	cmp r1, r0
	ldmeqia sp!, {r3, r4, r5, pc}
	ldr r3, [r5, #0x3c]
	ldr r5, [r5, #0x38]
	mov r2, r4
	mov r0, #0
	blx r5
	ldmia sp!, {r3, r4, r5, pc}
_02210644:
	ldrb r1, [r1, #0x13]
	cmp r1, #0
	ldmneia sp!, {r3, r4, r5, pc}
	bl ov00_0220FEC4
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end ov00_02210590

	arm_func_start ov00_02210658
ov00_02210658: ; 0x02210658
	stmdb sp!, {r3, r4, lr}
	sub sp, sp, #4
	mov r3, r1
	ldrb r1, [r3, #7]
	mov r4, r0
	cmp r1, #1
	beq _02210684
	cmp r1, #2
	beq _02210718
	add sp, sp, #4
	ldmia sp!, {r3, r4, pc}
_02210684:
	ldrb r0, [r3, #0xc]
	cmp r0, #2
	addhi sp, sp, #4
	ldmhiia sp!, {r3, r4, pc}
	add r0, r4, r0, lsl #2
	mov r2, #1
	str r2, [r0, #0x14]
	ldr r0, [r4, #0x10]
	cmp r0, #0
	addne sp, sp, #4
	ldmneia sp!, {r3, r4, pc}
	ldr r0, [r4, #0x18]
	cmp r0, #0
	ldrne r0, [r4, #0x1c]
	cmpne r0, #0
	addeq sp, sp, #4
	ldmeqia sp!, {r3, r4, pc}
	ldr r1, [r4, #4]
	sub r0, r2, #2
	cmp r1, r0
	beq _022106E8
	ldr r0, [r4, #0x14]
	cmp r0, #0
	addeq sp, sp, #4
	ldmeqia sp!, {r3, r4, pc}
_022106E8:
	mov r0, #1
	str r0, [r4, #0x10]
	bl ov00_021FF368
	add r0, r0, #0x710
	add r0, r0, #0x2000
	str r0, [r4, #0x28]
	ldr r0, [r4, #0x10]
	ldr r1, [r4, #0x3c]
	ldr r2, [r4, #0x34]
	blx r2
	add sp, sp, #4
	ldmia sp!, {r3, r4, pc}
_02210718:
	mov r0, #3
	strb r0, [r3, #7]
	ldrh r0, [r2, #2]
	mov ip, #0x15
	mov r1, r0, asr #8
	mov r0, r0, lsl #8
	str ip, [sp]
	and r1, r1, #0xff
	and r0, r0, #0xff00
	orr r0, r1, r0
	mov ip, r0, lsl #0x10
	ldr r1, [r2, #4]
	ldr r0, [r4]
	mov r2, ip, lsr #0x10
	bl ov00_0220FB90
	add sp, sp, #4
	ldmia sp!, {r3, r4, pc}
	arm_func_end ov00_02210658

	arm_func_start ov00_0221075C
ov00_0221075C: ; 0x0221075C
	stmdb sp!, {r4, r5, r6, r7, lr}
	sub sp, sp, #0x2c
	mov r6, r0
	mov r7, r1
	mov r5, r2
	bl ov00_0220FB6C
	cmp r0, #0
	addeq sp, sp, #0x2c
	ldmeqia sp!, {r4, r5, r6, r7, pc}
	ldrb r4, [r6, #7]
	cmp r4, #5
	cmpne r4, #7
	bne _0221082C
	cmp r7, #0x14
	addlt sp, sp, #0x2c
	ldmltia sp!, {r4, r5, r6, r7, pc}
	add r3, sp, #0x18
	mov r2, #0xa
_022107A4:
	ldrb r1, [r6]
	ldrb r0, [r6, #1]
	add r6, r6, #2
	subs r2, r2, #1
	strb r1, [r3]
	strb r0, [r3, #1]
	add r3, r3, #2
	bne _022107A4
	ldr r3, [sp, #0x20]
	mov r1, r3, lsr #0x18
	mov r0, r3, lsr #8
	mov r2, r3, lsl #8
	and r1, r1, #0xff
	and r0, r0, #0xff00
	mov r3, r3, lsl #0x18
	orr r0, r1, r0
	and r2, r2, #0xff0000
	and r1, r3, #0xff000000
	orr r0, r2, r0
	orr r0, r1, r0
	bl ov00_0220F9A8
	cmp r0, #0
	addeq sp, sp, #0x2c
	ldmeqia sp!, {r4, r5, r6, r7, pc}
	cmp r4, #5
	add r1, sp, #0x18
	mov r2, r5
	bne _02210820
	bl ov00_022104D4
	add sp, sp, #0x2c
	ldmia sp!, {r4, r5, r6, r7, pc}
_02210820:
	bl ov00_02210590
	add sp, sp, #0x2c
	ldmia sp!, {r4, r5, r6, r7, pc}
_0221082C:
	cmp r7, #0x15
	addlt sp, sp, #0x2c
	ldmltia sp!, {r4, r5, r6, r7, pc}
	add r3, sp, #0
	mov r2, #0xa
_02210840:
	ldrb r1, [r6]
	ldrb r0, [r6, #1]
	add r6, r6, #2
	subs r2, r2, #1
	strb r1, [r3]
	strb r0, [r3, #1]
	add r3, r3, #2
	bne _02210840
	ldrb r0, [r6]
	strb r0, [r3]
	ldr r3, [sp, #8]
	mov r1, r3, lsr #0x18
	mov r0, r3, lsr #8
	mov r2, r3, lsl #8
	and r1, r1, #0xff
	and r0, r0, #0xff00
	mov r3, r3, lsl #0x18
	orr r0, r1, r0
	and r2, r2, #0xff0000
	and r1, r3, #0xff000000
	orr r0, r2, r0
	orr r0, r1, r0
	bl ov00_0220F9A8
	cmp r0, #0
	addeq sp, sp, #0x2c
	ldmeqia sp!, {r4, r5, r6, r7, pc}
	add r1, sp, #0
	mov r2, r5
	bl ov00_02210658
	add sp, sp, #0x2c
	ldmia sp!, {r4, r5, r6, r7, pc}
	arm_func_end ov00_0221075C

	arm_func_start ov00_022108BC
ov00_022108BC: ; 0x022108BC
	stmdb sp!, {r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0x40
	movs r5, r0
	mov r8, r1
	mov r4, r2
	mov r7, r3
	ldreq r6, _02210A68 ; =ov00_02219D74
	beq _022108EC
	mov r0, #0x110
	bl ov00_021EC3A8
	mov r6, r0
	str r0, [r5]
_022108EC:
	bl ov00_021FF368
	bl srand
	mov r1, r7
	add r0, r6, #4
	bl strcpy
	ldr r1, [sp, #0x58]
	add r0, r6, #0x44
	bl strcpy
	str r4, [r6, #0xc0]
	mov r5, #0
	str r5, [r6, #0xac]
	str r5, [r6, #0xb0]
	str r8, [r6]
	mov r1, #1
	str r1, [r6, #0xb8]
	ldr r0, [sp, #0x7c]
	ldr r1, [sp, #0x64]
	str r0, [r6, #0x10c]
	str r1, [r6, #0x88]
	ldr r0, [sp, #0x68]
	ldr r1, [sp, #0x6c]
	str r0, [r6, #0x8c]
	str r1, [r6, #0x90]
	ldr r0, [sp, #0x70]
	ldr r1, [sp, #0x74]
	str r0, [r6, #0x94]
	ldr r0, [sp, #0x78]
	str r1, [r6, #0x98]
	str r0, [r6, #0x9c]
	str r5, [r6, #0xa0]
	str r5, [r6, #0xa4]
	ldr r0, [sp, #0x5c]
	str r5, [r6, #0xd4]
	str r0, [r6, #0xbc]
	ldr r0, [sp, #0x60]
	str r5, [r6, #0xc4]
	str r0, [r6, #0xc8]
	str r5, [r6, #0x104]
	add r0, r6, #0x100
	strh r5, [r0, #8]
	str r5, [r6, #0xa8]
	ldr r4, _02210A6C ; =0x80808081
	str r5, [r6, #0xb4]
	mov r8, #0xff
_0221099C:
	bl rand
	smull r1, r2, r4, r0
	add r3, r6, r5
	add r5, r5, #1
	add r2, r0, r2
	mov r1, r0, lsr #0x1f
	add r2, r1, r2, asr #7
	smull r1, r2, r8, r2
	sub r2, r0, r1
	strb r2, [r3, #0x84]
	cmp r5, #4
	blt _0221099C
	mov r2, #0
	mvn r1, #0
_022109D4:
	add r0, r6, r2, lsl #2
	add r2, r2, #1
	str r1, [r0, #0xd8]
	cmp r2, #0xa
	blt _022109D4
	mov r1, #0
	ldr r0, _02210A70 ; =ov00_0221B878
	str r1, [r6, #0x100]
	ldr r0, [r0]
	cmp r0, #0
	bne _02210A04
	bl ov00_02210E58
_02210A04:
	ldr r0, [sp, #0x5c]
	cmp r0, #0
	beq _02210A50
	ldr r0, _02210A74 ; =ov00_0221B890
	ldrsb r4, [r0]
	cmp r4, #0
	bne _02210A30
	ldr r1, _02210A78 ; =ov00_02219E84
	add r0, sp, #0
	mov r2, r7
	bl sprintf
_02210A30:
	cmp r4, #0
	ldrne r0, _02210A74 ; =ov00_0221B890
	ldr r1, _02210A7C ; =0x00006CFC
	addeq r0, sp, #0
	add r2, r6, #0xcc
	mov r3, #0
	bl ov00_02210ECC
	b _02210A54
_02210A50:
	mov r0, #1
_02210A54:
	cmp r0, #0
	moveq r0, #3
	movne r0, #0
	add sp, sp, #0x40
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	.balign 4, 0
_02210A68: .word ov00_02219D74
_02210A6C: .word 0x80808081
_02210A70: .word ov00_0221B878
_02210A74: .word ov00_0221B890
_02210A78: .word ov00_02219E84
_02210A7C: .word 0x00006CFC
	arm_func_end ov00_022108BC
