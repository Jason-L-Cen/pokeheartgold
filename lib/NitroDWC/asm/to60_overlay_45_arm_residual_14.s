	.include "asm/macros.inc"
	.include "overlay_45_arm.inc"
	.include "global.inc"
	.public ov45_0224268C
	.extern ov45_022339F4
	.extern ov45_02233ABC
	.extern ov45_02237A24
	.extern ov45_02237B58
	.extern ov45_02237BC8
	.extern ov45_02237D48
	.extern ov45_02254DBC
	.extern ov45_02254DC4
	.extern ov45_02255398

.text
	arm_func_start ov45_0224268C
ov45_0224268C: ; 0x0224268C
	stmdb sp!, {r4, r5, r6, r7, lr}
	sub sp, sp, #0x84
	mov r5, r0
	mov r0, #0
	mov r7, r1
	str r0, [r5]
	str r0, [r5, #4]
	ldr ip, [sp, #0x98]
	ldr r1, [sp, #0x9c]
	add r0, r5, #0xc
	mov r6, r2
	mov r4, r3
	str ip, [r5, #8]
	bl ov45_02237A24
	ldr r1, [sp, #0xa0]
	add r0, r5, #0x18
	bl ov45_02237A24
	ldr r1, [sp, #0xa4]
	add r0, r5, #0x24
	bl ov45_02237A24
	mov r1, r7
	add r0, r5, #0x30
	bl ov45_02237A24
	mov r1, r6
	add r0, r5, #0x3c
	bl ov45_02237A24
	mov r1, #0
	add r0, r5, #0x48
	mov ip, r1
_02242700:
	str ip, [r0, r1, lsl #2]
	add r1, r1, #1
	cmp r1, #3
	blo _02242700
	str ip, [r5, #0x54]
	str ip, [r5, #0x58]
	str ip, [r5, #0x5c]
	ldr r0, [sp, #0xa8]
	str ip, [r5, #0x60]
	add r3, r5, #0x64
	ldmia r0, {r0, r1, r2}
	stmia r3, {r0, r1, r2}
	str ip, [r5, #0x70]
	str ip, [r5, #0x74]
	add r0, r5, #0x74
	str r0, [r5, #0x7c]
	str ip, [r5, #0x80]
	str ip, [r5, #0x84]
	add r0, r5, #0x84
	str r0, [r5, #0x8c]
	str ip, [r5, #0x90]
	str ip, [r5, #0x94]
	add r0, r5, #0x94
	str r0, [r5, #0x98]
	str ip, [r5, #0x9c]
	str ip, [r5, #0xa0]
	add r0, r5, #0xa0
	ldr lr, _0224284C ; =ov45_02254DC4
	str r0, [r5, #0xa8]
	add ip, sp, #0x54
	ldmia lr!, {r0, r1, r2, r3}
	stmia ip!, {r0, r1, r2, r3}
	ldmia lr!, {r0, r1, r2, r3}
	stmia ip!, {r0, r1, r2, r3}
	ldmia lr, {r0, r1, r2, r3}
	stmia ip, {r0, r1, r2, r3}
	ldr r1, _02242850 ; =ov45_02254DBC
	add r0, sp, #0x48
	bl ov45_02237B58
	ldr r2, _02242854 ; =ov45_02255398
	add r0, sp, #0x3c
	add r1, sp, #0x48
	bl ov45_02233ABC
	add r0, sp, #0x30
	add r1, sp, #0x3c
	add r2, r5, #0x30
	bl ov45_022339F4
	ldr r2, _02242854 ; =ov45_02255398
	add r0, sp, #0x24
	add r1, sp, #0x30
	bl ov45_02233ABC
	add r1, sp, #0x54
	ldr r1, [r1, r4, lsl #2]
	add r0, sp, #0x18
	bl ov45_02237B58
	add r0, sp, #0xc
	add r1, sp, #0x24
	add r2, sp, #0x18
	bl ov45_022339F4
	ldr r2, _02242854 ; =ov45_02255398
	add r0, sp, #0
	add r1, sp, #0xc
	bl ov45_02233ABC
	add r1, sp, #0
	add r0, r5, #0x48
	bl ov45_02237D48
	add r0, sp, #0
	bl ov45_02237BC8
	add r0, sp, #0xc
	bl ov45_02237BC8
	add r0, sp, #0x18
	bl ov45_02237BC8
	add r0, sp, #0x24
	bl ov45_02237BC8
	add r0, sp, #0x30
	bl ov45_02237BC8
	add r0, sp, #0x3c
	bl ov45_02237BC8
	add r0, sp, #0x48
	bl ov45_02237BC8
	mov r0, r5
	add sp, sp, #0x84
	ldmia sp!, {r4, r5, r6, r7, pc}
	.balign 4, 0
_0224284C: .word ov45_02254DC4
_02242850: .word ov45_02254DBC
_02242854: .word ov45_02255398
	arm_func_end ov45_0224268C
