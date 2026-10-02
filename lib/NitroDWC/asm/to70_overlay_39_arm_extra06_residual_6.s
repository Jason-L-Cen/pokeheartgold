	.include "asm/macros.inc"
	.include "overlay_39_arm.inc"
	.include "global.inc"
	.public ov39_0222A4EC
	.public ov39_0222A544
	.extern ov39_0222AA7C

.text
	arm_func_start ov39_0222A4EC
ov39_0222A4EC: ; 0x0222A4EC
	stmdb sp!, {r3, r4, r5, lr}
	mov r5, #0
	cmp r1, #0
	ldmleia sp!, {r3, r4, r5, pc}
	ldr lr, _0222A540 ; =ov39_0222AA7C
	mov ip, r5
_0222A504:
	cmp r5, #0
	moveq r4, ip
	beq _0222A524
	ldrb r4, [r0]
	add r3, r5, r1
	add r3, r4, r3
	and r3, r3, #0xff
	ldrb r4, [lr, r3]
_0222A524:
	ldrb r3, [r0, r5]
	add r5, r5, #1
	cmp r5, r1
	eor r3, r4, r3
	strb r3, [r2], #1
	blt _0222A504
	ldmia sp!, {r3, r4, r5, pc}
	.balign 4, 0
_0222A540: .word ov39_0222AA7C
	arm_func_end ov39_0222A4EC

	arm_func_start ov39_0222A544
ov39_0222A544: ; 0x0222A544
	stmdb sp!, {r3, r4, r5, lr}
	mov r5, #0
	cmp r1, #0
	ldmleia sp!, {r3, r4, r5, pc}
	ldr lr, _0222A598 ; =ov39_0222AA7C
	mov ip, r5
_0222A55C:
	cmp r5, #0
	moveq r4, ip
	beq _0222A57C
	ldrb r4, [r0]
	add r3, r5, r1
	add r3, r4, r3
	and r3, r3, #0xff
	ldrb r4, [lr, r3]
_0222A57C:
	ldrb r3, [r2, r5]
	eor r3, r3, r4
	strb r3, [r2, r5]
	add r5, r5, #1
	cmp r5, r1
	blt _0222A55C
	ldmia sp!, {r3, r4, r5, pc}
	.balign 4, 0
_0222A598: .word ov39_0222AA7C
	arm_func_end ov39_0222A544
