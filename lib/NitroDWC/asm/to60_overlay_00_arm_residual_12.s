	.include "asm/macros.inc"
	.include "overlay_00_arm.inc"
	.include "global.inc"
	.public ov00_0220E0F0
	.extern ov00_0220D35C

.text
	arm_func_start ov00_0220E0F0
ov00_0220E0F0: ; 0x0220E0F0
	stmdb sp!, {r3, lr}
	ldr r1, [r0, #0xc]
	cmp r1, #7
	moveq r0, #1
	ldmeqia sp!, {r3, pc}
	cmp r1, #6
	moveq r2, #0
	movne r2, #1
	mov r1, #2
	bl ov00_0220D35C
	cmp r0, #0
	moveq r0, #0
	movne r0, #1
	ldmia sp!, {r3, pc}
	arm_func_end ov00_0220E0F0
