	.include "asm/macros.inc"
	.include "overlay_13_arm_2.inc"
	.include "global.inc"
	.public ov13_0223FDF4

.text
	arm_func_start ov13_0223FDF4
ov13_0223FDF4: ; 0x0223FDF4
	stmdb sp!, {r4, lr}
	cmp r1, #0
	ldr ip, [r0, #8]
	blt _0223FE40
	cmp r2, #0x100
	cmpne r2, #0x300
	ldr r0, _0223FE94 ; =0xC1FFFCFF
	beq _0223FE28
	ldr r3, [ip, r1, lsl #3]
	and r0, r3, r0
	orr r0, r0, r2
	str r0, [ip, r1, lsl #3]
	ldmia sp!, {r4, pc}
_0223FE28:
	ldr r4, [ip, r1, lsl #3]
	and r0, r4, r0
	orr r0, r0, r2
	orr r0, r0, r3, lsl #25
	str r0, [ip, r1, lsl #3]
	ldmia sp!, {r4, pc}
_0223FE40:
	ldrb r4, [r0, #0xc]
	mov r1, #0
	cmp r4, #0
	ldmleia sp!, {r4, pc}
	ldr lr, _0223FE94 ; =0xC1FFFCFF
_0223FE54:
	cmp r2, #0x100
	cmpne r2, #0x300
	ldr r4, [ip, r1, lsl #3]
	beq _0223FE70
	and r4, r4, lr
	orr r4, r4, r2
	b _0223FE7C
_0223FE70:
	and r4, r4, lr
	orr r4, r4, r2
	orr r4, r4, r3, lsl #25
_0223FE7C:
	str r4, [ip, r1, lsl #3]
	ldrb r4, [r0, #0xc]
	add r1, r1, #1
	cmp r1, r4
	blt _0223FE54
	ldmia sp!, {r4, pc}
	.balign 4, 0
_0223FE94: .word 0xC1FFFCFF
	arm_func_end ov13_0223FDF4
