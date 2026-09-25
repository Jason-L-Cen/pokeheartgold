	.include "asm/macros.inc"
#include <nitro/fs/overlay.h>
	.include "global.inc"

	.text

	arm_func_start strcmp
strcmp: ; 0x020E9734
	stmdb sp!, {r4, lr}
	ldrb r2, [r0]
	ldrb r3, [r1]
	subs r3, r2, r3
	movne r0, r3
	ldmneia sp!, {r4, pc}
	and r4, r0, #3
	and r3, r1, #3
	cmp r3, r4
	bne _020E9810
	cmp r4, #0
	beq _020E97A8
	cmp r2, #0
	moveq r0, #0
	ldmeqia sp!, {r4, pc}
	rsbs r4, r4, #3
	beq _020E97A0
_020E9778:
	ldrb r3, [r0, #1]!
	ldrb r2, [r1, #1]!
	subs r2, r3, r2
	movne r0, r2
	ldmneia sp!, {r4, pc}
	cmp r3, #0
	moveq r0, #0
	ldmeqia sp!, {r4, pc}
	subs r4, r4, #1
	bne _020E9778
_020E97A0:
	add r0, r0, #1
	add r1, r1, #1
_020E97A8:
	ldr r2, [r0]
	ldr r3, _020E9840 ; =0xFEFEFEFF
	mvn r4, r2
	add lr, r2, r3
	ldr ip, _020E9844 ; =0x80808080
	and r4, lr, r4
	tst r4, ip
	ldr r4, [r1]
	bne _020E97FC
	cmp r2, r4
	bne _020E97F0
_020E97D4:
	ldr r2, [r0, #4]!
	ldr r4, [r1, #4]!
	add lr, r2, r3
	tst lr, ip
	bne _020E97FC
	cmp r2, r4
	beq _020E97D4
_020E97F0:
	sub r0, r0, #1
	sub r1, r1, #1
	b _020E9810
_020E97FC:
	ldrb r2, [r0]
	ldrb r3, [r1]
	subs r3, r2, r3
	movne r0, r3
	ldmneia sp!, {r4, pc}
_020E9810:
	cmp r2, #0
	moveq r0, #0
	ldmeqia sp!, {r4, pc}
_020E981C:
	ldrb r3, [r0, #1]!
	ldrb r2, [r1, #1]!
	subs r2, r3, r2
	movne r0, r2
	ldmneia sp!, {r4, pc}
	cmp r3, #0
	bne _020E981C
	mov r0, #0
	ldmia sp!, {r4, pc}
	.align 2, 0
_020E9840: .word 0xFEFEFEFF
_020E9844: .word 0x80808080
	arm_func_end strcmp
