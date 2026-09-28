	.include "asm/macros.inc"
	.include "overlay_13_arm_2.inc"
	.include "global.inc"
	.public ov13_0223BC24
	.extern ov13_02245828
	.extern ov13_0224F5B4

.text
	arm_func_start ov13_0223BC24
ov13_0223BC24: ; 0x0223BC24
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	ldr r1, _0223BD28 ; =ov13_0224F5B4
	mov sl, r0
	ldr r2, [r1]
	add r1, r2, #0x1000
	ldrb r0, [r1, #0xb95]
	cmp r0, #0
	beq _0223BC58
	ldr r1, [r1, #0x370]
	cmp r1, #0
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	blx r1
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_0223BC58:
	add r0, r2, #0xf00
	mov r1, #0x400
	bl DC_InvalidateRange
	ldrh fp, [sl, #0xe]
	mov r6, #0
	cmp fp, #0
	ldmleia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldr r0, _0223BD28 ; =ov13_0224F5B4
	mov r4, #6
	ldr sb, [r0]
_0223BC80:
	add r0, sl, r6, lsl #2
	ldr r5, [r0, #0x10]
	ldr r1, _0223BD2C ; =ov13_02245828
	mov r2, #8
	add r0, r5, #0xc
	bl memcmp
	cmp r0, #0
	bne _0223BD18
	ldrb r0, [r5, #0x15]
	tst r0, #1
	beq _0223BD18
	mov r7, #0
	add r8, sb, #0x1300
_0223BCB4:
	mov r1, r8
	mov r2, r4
	add r0, r5, #4
	bl memcmp
	cmp r0, #0
	bne _0223BD08
	rsb r0, r7, r7, lsl #3
	add r0, sb, r0
	add r0, r0, #0x1000
	ldrb r0, [r0, #0x306]
	cmp r0, #0
	bne _0223BD18
	add r0, sb, #0x1000
	ldr r1, [r0, #0x370]
	cmp r1, #0
	moveq r1, #1
	streqb r1, [r0, #0xb95]
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	mov r0, #1
	blx r1
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_0223BD08:
	add r7, r7, #1
	cmp r7, #0x10
	add r8, r8, #7
	blt _0223BCB4
_0223BD18:
	add r6, r6, #1
	cmp r6, fp
	blt _0223BC80
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.balign 4, 0
_0223BD28: .word ov13_0224F5B4
_0223BD2C: .word ov13_02245828
	arm_func_end ov13_0223BC24
