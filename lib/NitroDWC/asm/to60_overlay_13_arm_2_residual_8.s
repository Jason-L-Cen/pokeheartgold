	.include "asm/macros.inc"
	.include "overlay_13_arm_2.inc"
	.include "global.inc"
	.public ov13_02239B34
	.extern ov13_022412B0
	.extern ov13_0224F5A0

.text
	arm_func_start ov13_02239B34
ov13_02239B34: ; 0x02239B34
	stmdb sp!, {r3, lr}
	ldr r0, _02239B60 ; =ov13_0224F5A0
	ldr r0, [r0]
	cmp r0, #0
	ldmeqia sp!, {r3, pc}
_02239B48:
	bl WM_GetAllowedChannel
	cmp r0, #0x8000
	bne _02239B48
	ldr r0, _02239B64 ; =ov13_0224F5A0
	bl ov13_022412B0
	ldmia sp!, {r3, pc}
	.balign 4, 0
_02239B60: .word ov13_0224F5A0
_02239B64: .word ov13_0224F5A0
	arm_func_end ov13_02239B34
