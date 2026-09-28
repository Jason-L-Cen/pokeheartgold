	.include "asm/macros.inc"
	.include "overlay_13_arm_2.inc"
	.include "global.inc"
	.public ov13_02241B70
	.extern ov13_02242478

.text
	arm_func_start ov13_02241B70
ov13_02241B70: ; 0x02241B70
	stmdb sp!, {r3, lr}
	mov r0, #1
	bl ov13_02242478
	ldr r0, _02241B94 ; =0x027E0000
	add r0, r0, #0x3000
	ldr r1, [r0, #0xff8]
	orr r1, r1, #1
	str r1, [r0, #0xff8]
	ldmia sp!, {r3, pc}
	.balign 4, 0
_02241B94: .word 0x027E0000
	arm_func_end ov13_02241B70
