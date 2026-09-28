	.include "asm/macros.inc"
	.include "overlay_13_arm_2.inc"
	.include "global.inc"
	.public ov13_0223AB8C
	.extern ov13_022412B0
	.extern ov13_022425D0
	.extern ov13_0224F5A8

.text
	arm_func_start ov13_0223AB8C
ov13_0223AB8C: ; 0x0223AB8C
	stmdb sp!, {r3, lr}
	ldr r1, _0223ABB0 ; =ov13_0224F5A8
	mov r0, #0
	ldr r1, [r1]
	ldr r1, [r1, #0x9c]
	bl ov13_022425D0
	ldr r0, _0223ABB4 ; =ov13_0224F5A8
	bl ov13_022412B0
	ldmia sp!, {r3, pc}
	.balign 4, 0
_0223ABB0: .word ov13_0224F5A8
_0223ABB4: .word ov13_0224F5A8
	arm_func_end ov13_0223AB8C
