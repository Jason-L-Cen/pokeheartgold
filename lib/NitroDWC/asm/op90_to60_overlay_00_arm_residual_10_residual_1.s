	.include "asm/macros.inc"
	.include "overlay_00_arm.inc"
	.include "global.inc"
	.extern ov00_021FF368
	.extern ov00_022199D0
	.extern ov00_022199E4
	.extern ov00_02219A08
	.extern ov00_02219A10
	.extern ov00_0220BC50

.text
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
