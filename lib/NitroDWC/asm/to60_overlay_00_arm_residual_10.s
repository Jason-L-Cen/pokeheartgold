	.include "asm/macros.inc"
	.include "overlay_00_arm.inc"
	.include "global.inc"
	.public ov00_0220BCEC
	.public ov00_0220BE08
	.extern ov00_021FF368
	.extern ov00_022199D0
	.extern ov00_022199E4
	.extern ov00_02219A08
	.extern ov00_02219A10

.text
	arm_func_start ov00_0220BC50
ov00_0220BC50: ; 0x0220BC50
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, lr}
	ldrb r5, [r0]
	mov lr, #0
	mov r4, #1
	and r6, r5, #1
	mov r3, lr
	mov ip, r4
	mov r1, lr
	mov r2, r4
_0220BC74:
	add r7, r0, r4
	ldrb sl, [r7, #-1]
	cmp sl, r5
	eor sl, r4, sl
	movlo r8, ip
	and sl, sl, #1
	movhs r8, r3
	cmp r5, #0x4f
	movlo sb, r2
	eor sl, lr, sl
	movhs sb, r1
	eor sl, r6, sl
	eor sb, sl, sb
	eors lr, sb, r8
	beq _0220BCBC
	ldrb r8, [r7]
	tst r8, #1
	beq _0220BCD0
_0220BCBC:
	cmp lr, #0
	ldreqb r7, [r7]
	andeq r7, r7, #1
	cmpeq r7, #1
	bne _0220BCD8
_0220BCD0:
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
_0220BCD8:
	add r4, r4, #1
	cmp r4, #0x20
	blt _0220BC74
	mov r0, #1
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
	arm_func_end ov00_0220BC50

	arm_func_start ov00_0220BCEC
ov00_0220BCEC: ; 0x0220BCEC
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	movs sl, r0
	bne _0220BD0C
	ldr r0, _0220BDF8 ; =ov00_02219A08
	ldr r1, _0220BDFC ; =ov00_02219A10
	ldr r2, _0220BE00 ; =ov00_022199D0
	mov r3, #0x2b
	bl __msl_assertion_failed
_0220BD0C:
	bl ov00_021FF368
	bl srand
	bl rand
	ldr r5, _0220BE04 ; =0x2C0B02C1
	mov r1, r0, lsr #0x1f
	smull r2, r4, r5, r0
	mov r8, #0
	mov r7, #1
	add r4, r1, r4, asr #4
	mov r3, #0x5d
	smull r1, r2, r3, r4
	sub r4, r0, r1
	add r0, r4, #0x21
	strb r0, [sl]
	mov fp, r8
	mov r6, r7
	mov r4, r3
_0220BD50:
	add sb, sl, r7
	ldrb r3, [sb, #-1]
	ldrb r0, [sl]
	cmp r3, r0
	eor r3, r7, r3
	movlo r1, r6
	and r3, r3, #1
	movhs r1, fp
	cmp r0, #0x4f
	movlo r2, #1
	and r0, r0, #1
	eor r3, r8, r3
	movhs r2, #0
	eor r0, r0, r3
	eor r0, r0, r2
	eor r8, r0, r1
	bl rand
	smull r2, r3, r5, r0
	mov r1, r0, lsr #0x1f
	add r3, r1, r3, asr #4
	smull r1, r2, r4, r3
	sub r3, r0, r1
	add r0, r3, #0x21
	cmp r8, #0
	strb r0, [sb]
	beq _0220BDC4
	ldrb r0, [sb]
	tst r0, #1
	beq _0220BDD8
_0220BDC4:
	cmp r8, #0
	ldreqb r0, [sb]
	andeq r0, r0, #1
	cmpeq r0, #1
	bne _0220BDE4
_0220BDD8:
	ldrb r0, [sb]
	add r0, r0, #1
	strb r0, [sb]
_0220BDE4:
	add r7, r7, #1
	cmp r7, #0x20
	blt _0220BD50
	mov r0, sl
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.balign 4, 0
_0220BDF8: .word ov00_02219A08
_0220BDFC: .word ov00_02219A10
_0220BE00: .word ov00_022199D0
_0220BE04: .word 0x2C0B02C1
	arm_func_end ov00_0220BCEC

	arm_func_start ov00_0220BE08
ov00_0220BE08: ; 0x0220BE08
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	mov r5, r0
	ldr r0, _0220BF08 ; =ov00_022199E4
	mov r4, r1
	bl strlen
	mov r7, r0
	mov r0, r4
	bl ov00_0220BC50
	mov sl, #0
	mov fp, r0
	mov r8, sl
_0220BE34:
	cmp fp, #0
	cmpne sl, #0
	cmpne sl, #0xd
	bne _0220BE6C
	bl rand
	ldr r1, _0220BF0C ; =0x2C0B02C1
	mov r2, r0, lsr #0x1f
	smull r3, r6, r1, r0
	add r6, r2, r6, asr #4
	mov r1, #0x5d
	smull r2, r3, r1, r6
	sub r6, r0, r2
	add r0, r6, #0x21
	b _0220BEE8
_0220BE6C:
	cmp sl, #1
	cmpne sl, #0xe
	ldreqsb r6, [r4, sl]
	addne r0, r4, sl
	ldrb sb, [r4, sl]
	ldrnesb r6, [r0, #-1]
	mov r1, r7
	add r0, sl, sb
	bl _s32_div_f
	mul r0, r6, r8
	mov r6, r1
	mov r1, r7
	bl _s32_div_f
	ldr r0, _0220BF08 ; =ov00_022199E4
	ldrsb r3, [r0, r6]
	ldrsb r2, [r0, r1]
	mla r0, sl, sb, r3
	mov r1, r0, lsr #0x1f
	rsb r0, r1, r0, lsl #27
	add r0, r1, r0, ror #27
	ldrb r0, [r4, r0]
	eor r0, r0, r2
	bl abs
	ldr r1, _0220BF0C ; =0x2C0B02C1
	smull r2, r3, r1, r0
	mov r1, r0, lsr #0x1f
	add r3, r1, r3, asr #4
	mov r1, #0x5d
	smull r2, r3, r1, r3
	sub r3, r0, r2
	add r0, r3, #0x21
_0220BEE8:
	strb r0, [r5, sl]
	add r0, r8, #0x47
	add sl, sl, #1
	cmp sl, #0x20
	add r8, r0, #0x4600
	blt _0220BE34
	mov r0, r5
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.balign 4, 0
_0220BF08: .word ov00_022199E4
_0220BF0C: .word 0x2C0B02C1
	arm_func_end ov00_0220BE08
