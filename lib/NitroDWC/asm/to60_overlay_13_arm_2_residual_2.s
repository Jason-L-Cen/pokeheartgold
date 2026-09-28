	.include "asm/macros.inc"
	.include "overlay_13_arm_2.inc"
	.include "global.inc"
	.public ov13_0222BAF4

.text
	arm_func_start ov13_0222BAF4
ov13_0222BAF4: ; 0x0222BAF4
	stmdb sp!, {r3, lr}
	mov r1, r1, lsl #0x10
	mov r1, r1, lsr #0x10
	ldmia r0, {ip, lr}
	ldr r0, [ip, r1, lsl #2]
	cmp r2, #0
	add r0, lr, r0
	addge r3, r3, #0x30
	movge r1, r2, lsl #1
	strgeh r3, [r0, r1]
	ldmia sp!, {r3, pc}
	arm_func_end ov13_0222BAF4
