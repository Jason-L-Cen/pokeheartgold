	.include "asm/macros.inc"
	.include "overlay_00_arm.inc"
	.include "global.inc"
	.public ov00_0220D9A4
	.extern ov00_0220D35C
	.extern ov00_0220ED98

.text
	arm_func_start ov00_0220D9A4
ov00_0220D9A4: ; 0x0220D9A4
	stmdb sp!, {r4, lr}
	mov r4, r0
	bl ov00_0220ED98
	cmp r0, #0
	moveq r0, #0
	ldmeqia sp!, {r4, pc}
	ldr r0, [r4, #0xc]
	mov r1, #2
	cmp r0, #6
	moveq r2, #0
	movne r2, #1
	mov r0, r4
	bl ov00_0220D35C
	cmp r0, #0
	moveq r0, #0
	movne r0, #1
	ldmia sp!, {r4, pc}
	arm_func_end ov00_0220D9A4
