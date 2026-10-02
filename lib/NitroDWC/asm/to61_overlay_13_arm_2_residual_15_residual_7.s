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
	.public ov13_0223E308
	.extern ov13_0223E3A0

.text
	arm_func_start ov13_0223E308
ov13_0223E308: ; 0x0223E308
	stmdb sp!, {r3, lr}
	ldr r2, _0223E37C ; =ov13_0224F5C0
	mov r0, #0
	ldr r1, [r2, #4]
	mov r3, #1
	add r1, r1, #0x1000
	str r0, [r1, #0x2a4]
	ldr r1, [r2, #4]
	add r1, r1, #0x1000
	str r0, [r1, #0x2a0]
	ldr r1, [r2, #4]
	str r0, [r1, #0x48]
	ldr r1, [r2, #4]
	strh r0, [r1, #0x50]
	ldr r1, [r2, #4]
	strh r3, [r1, #0x52]
	ldr r1, [r2, #4]
	str r0, [r1, #0x54]
	ldr r1, [r2, #4]
	str r0, [r1]
	ldr r1, [r2, #4]
	strh r0, [r1, #4]
	ldr r1, [r2, #4]
	str r0, [r1, #0x4c]
	bl ov13_0223E3A0
	cmp r0, #0
	moveq r0, #0
	movne r0, #1
	ldmia sp!, {r3, pc}
	.balign 4, 0
_0223E37C: .word ov13_0224F5C0
	arm_func_end ov13_0223E308
