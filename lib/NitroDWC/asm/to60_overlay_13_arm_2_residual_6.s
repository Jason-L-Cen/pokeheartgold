	.include "asm/macros.inc"
	.include "overlay_13_arm_2.inc"
	.include "global.inc"
	.public ov13_02239288
	.extern ov13_022412B0
	.extern ov13_022425F0
	.extern ov13_0224F590

.text
	arm_func_start ov13_02239288
ov13_02239288: ; 0x02239288
	stmdb sp!, {r3, lr}
	ldr r1, _022392AC ; =ov13_0224F590
	mov r0, #1
	ldr r1, [r1]
	ldr r1, [r1]
	bl ov13_022425F0
	ldr r0, _022392B0 ; =ov13_0224F590
	bl ov13_022412B0
	ldmia sp!, {r3, pc}
	.balign 4, 0
_022392AC: .word ov13_0224F590
_022392B0: .word ov13_0224F590
	arm_func_end ov13_02239288
