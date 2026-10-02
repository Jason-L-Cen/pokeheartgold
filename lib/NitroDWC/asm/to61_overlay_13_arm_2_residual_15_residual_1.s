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
	.public ov13_0223D470

.text
	arm_func_start ov13_0223D470
ov13_0223D470: ; 0x0223D470
	ldr r2, _0223D4BC ; =ov13_0224F5BC
	ldr r1, _0223D4C0 ; =ov13_022468D8
	ldr ip, [r2]
	ldr r2, _0223D4C4 ; =ov13_022468D8
	add r3, ip, #2
	str r3, [r1]
	add r3, ip, #4
	str r3, [r1, #4]
	add r3, ip, #6
	str r3, [r1, #8]
	add r3, ip, #8
	str r3, [r1, #0xc]
	add r3, ip, #0xa
	str r3, [r1, #0x10]
	add r3, ip, #0xc
	str r3, [r1, #0x14]
	ldr r0, [r2, r0, lsl #2]
	ldrh r0, [r0]
	bx lr
	.balign 4, 0
_0223D4BC: .word ov13_0224F5BC
_0223D4C0: .word ov13_022468D8
_0223D4C4: .word ov13_022468D8
	arm_func_end ov13_0223D470
