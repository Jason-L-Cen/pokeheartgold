	.include "asm/macros.inc"
	.include "overlay_13_arm_2.inc"
	.include "global.inc"
	.public ov13_02239514
	.extern ov13_0223FD60
	.extern ov13_022412B0
	.extern ov13_022425F0
	.extern ov13_0224F59C

.text
	arm_func_start ov13_02239514
ov13_02239514: ; 0x02239514
	stmdb sp!, {r3, lr}
	ldr r1, _02239548 ; =ov13_0224F59C
	mov r0, #0
	ldr r1, [r1]
	ldr r1, [r1, #0xc]
	bl ov13_022425F0
	ldr r0, _02239548 ; =ov13_0224F59C
	ldr r0, [r0]
	ldr r0, [r0]
	bl ov13_0223FD60
	ldr r0, _0223954C ; =ov13_0224F59C
	bl ov13_022412B0
	ldmia sp!, {r3, pc}
	.balign 4, 0
_02239548: .word ov13_0224F59C
_0223954C: .word ov13_0224F59C
	arm_func_end ov13_02239514
