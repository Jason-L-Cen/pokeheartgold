	.extern ov13_0223A7F4
	.extern ov13_0223C9D4
	.extern ov13_0223CA14
	.extern ov13_0223CB34
	.extern ov13_0223CEA0
	.extern ov13_0223CF80
	.extern ov13_0223CFE8
	.extern ov13_0223D0E0
	.extern ov13_0223F8A0
	.extern ov13_0223F904
	.extern ov13_0223F954
	.extern ov13_0224022C
	.extern ov13_0224128C
	.extern ov13_0224593C
	.extern ov13_022468D8
	.extern ov13_02247120
	.extern ov13_02247200
	.extern ov13_02247208
	.extern ov13_0224720C
	.extern ov13_02247238
	.extern ov13_02247264
	.extern ov13_02247284
	.extern ov13_022472B4
	.extern ov13_022472DC
	.extern ov13_022472F4
	.extern ov13_0224730C
	.extern ov13_02247324
	.extern ov13_0224733C
	.extern ov13_02247358
	.extern ov13_02247388
	.extern ov13_022473BC
	.extern ov13_022473E0
	.extern ov13_02247404
	.extern ov13_0224741C
	.extern ov13_0224F5BC
	.extern ov13_0224F5C0
	.extern ov13_0224F5C8
	.extern ov13_0224F5CC
	.include "asm/macros.inc"
	.include "overlay_13_arm_2.inc"
	.include "global.inc"
	.public ov13_0223D7F4
	.extern ov13_0223D6A8
	.extern ov13_0223D724
	.extern ov13_0223D858

.text
	arm_func_start ov13_0223D7F4
ov13_0223D7F4: ; 0x0223D7F4
	stmdb sp!, {r3, lr}
	mov r0, #3
	bl ov13_0223D6A8
	ldr r0, _0223D850 ; =ov13_0224F5C0
	ldr r1, [r0, #4]
	add r0, r1, #0x1000
	ldr r2, [r0, #0x3ac]
	add r0, r1, #0x13c0
	blx r2
	ldr r2, _0223D850 ; =ov13_0224F5C0
	mov r1, r0
	ldr r2, [r2, #4]
	ldr r0, _0223D854 ; =ov13_0223D858
	add r2, r2, #0x13c0
	bl WM_SetWEPKey
	cmp r0, #2
	moveq r0, #1
	ldmeqia sp!, {r3, pc}
	bl ov13_0223D724
	mov r0, #9
	bl ov13_0223D6A8
	mov r0, #0
	ldmia sp!, {r3, pc}
	.balign 4, 0
_0223D850: .word ov13_0224F5C0
_0223D854: .word ov13_0223D858
	arm_func_end ov13_0223D7F4
