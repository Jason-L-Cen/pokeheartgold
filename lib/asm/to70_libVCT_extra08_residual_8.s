	.include "asm/macros.inc"
	.include "libVCT.inc"
	.include "global.inc"
	.public vct_calc_jitter_and_skew
	.extern vct_init_sequence
	.extern vct_update_sequence

.text
	arm_func_start vct_calc_jitter_and_skew
vct_calc_jitter_and_skew: ; 0x020B2C9C
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	mov r7, r0
	ldr r2, [r7, #0x5c]
	mov r6, r1
	cmp r2, #0
	mov r5, #0
	beq _020B2CCC
	ldr r1, [r6, #0x18]
	mov r1, r1, lsl #0x10
	mov r1, r1, lsr #0x10
	bl vct_init_sequence
	b _020B2D64
_020B2CCC:
	ldr r2, [r6, #0x18]
	mov r2, r2, lsl #0x10
	mov r2, r2, lsr #0x10
	bl vct_update_sequence
	cmp r0, #0
	bne _020B2CF8
	ldr r1, [r7, #0x14]
	mov r0, r5
	add r1, r1, #1
	str r1, [r7, #0x14]
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_020B2CF8:
	ldr r0, [r6, #0x18]
	str r0, [r7, #0x10]
	ldr r1, [r6, #0x18]
	ldr r2, [r7, #0x54]
	cmp r2, r1
	bne _020B2D24
	ldr r1, [r7, #0x14]
	mov r0, r5
	add r1, r1, #1
	str r1, [r7, #0x14]
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_020B2D24:
	add r0, r2, #1
	cmp r0, r1
	beq _020B2D64
	ldr r0, [r6, #0x45c]
	cmp r0, #0
	bne _020B2D64
	subs r0, r1, r2
	rsbmi r0, r0, #0
	cmp r0, #0x64
	strgt r1, [r7, #0x54]
	movgt r0, #0
	ldmgtia sp!, {r3, r4, r5, r6, r7, pc}
	cmp r2, r1
	ldrhi r0, [r7, #0x1c]
	addhi r0, r0, #1
	strhi r0, [r7, #0x1c]
_020B2D64:
	ldr r0, [r6, #0x18]
	str r0, [r7, #0x54]
	ldr r0, [r7, #0x4c]
	cmp r0, #0
	bne _020B2D9C
	ldr r1, [r6, #0x460]
	add r0, r6, #0x64
	str r1, [r7, #0x4c]
	add r1, r0, #0x400
	add r0, r7, #0x44
	ldmia r1, {r2, r3}
	stmia r0, {r2, r3}
	mov r0, #0
	str r0, [r7, #0x6c]
_020B2D9C:
	ldr r2, [r6, #0x460]
	ldr r0, [r7, #0x4c]
	ldr r3, [r6, #0x464]
	ldr r1, [r7, #0x44]
	sub r0, r2, r0
	ldr r2, [r7, #0x80]
	mov r0, r0, lsl #6
	subs r1, r3, r1
	subs r4, r0, r1
	cmp r2, #0
	beq _020B2DD4
	ldr r1, [r6, #0x45c]
	cmp r1, #0
	beq _020B2DDC
_020B2DD4:
	str r4, [r7, #0x80]
	b _020B2E50
_020B2DDC:
	str r4, [r7, #0x80]
	subs r1, r4, r2
	ldr r2, [r7, #0x7c]
	rsbmi r1, r1, #0
	add r2, r2, #0x318
	add r2, r2, #0x8800
	mov ip, r2, lsl #1
	ldr r3, _020B302C ; =0x0000CC8D
	mov r2, #0
	adds r3, ip, r3
	mov lr, r1, asr #0x1f
	adc r2, r2, ip, asr #31
	cmp lr, r2
	cmpeq r1, r3
	bls _020B2E30
	ldr ip, _020B3030 ; =0x75CA82CB
	mov r2, r1, lsr #0x1f
	smull r3, lr, ip, r1
	add lr, r2, lr, asr #14
	add r2, lr, #2
	str r2, [r7, #0x84]
_020B2E30:
	ldr r2, [r7, #0x84]
	cmp r2, #0
	bgt _020B2E50
	ldr r2, [r7, #0x7c]
	sub r1, r1, r2
	add r1, r2, r1, asr #4
	str r1, [r7, #0x7c]
	str r1, [r7, #0x18]
_020B2E50:
	ldr r2, [r7, #0x44]
	ldr r3, [r7, #4]
	ldr r1, [r7, #0x48]
	adds r2, r0, r2
	adc r0, r1, #0
	adds r1, r3, r2
	str r1, [r6, #0x46c]
	adc r0, r0, #0
	str r0, [r6, #0x470]
	bl OS_GetTick
	ldr ip, [r6, #0x46c]
	ldr r2, _020B3034 ; =0x00008B18
	ldr lr, [r6, #0x470]
	adds r6, ip, r2
	adc r3, lr, #0
	cmp r3, r1
	mov r3, #0
	cmpeq r6, r0
	movlo r0, r3
	ldmloia sp!, {r3, r4, r5, r6, r7, pc}
	ldr r6, _020B3038 ; =0x0007FD88
	adds r6, r0, r6
	adc r0, r1, r3
	cmp lr, r0
	cmpeq ip, r6
	movhi r0, r3
	ldmhiia sp!, {r3, r4, r5, r6, r7, pc}
	ldr r0, [r7, #0x84]
	cmp r0, #0
	bgt _020B3018
	ldr r0, [r7, #0x7c]
	ldr r6, _020B3030 ; =0x75CA82CB
	add ip, r0, r0, lsl #1
	smull r1, r0, r6, ip
	ldr r6, [r7, #0x8c]
	mov r1, ip, lsr #0x1f
	cmp ip, r6
	add r0, r1, r0, asr #14
	bls _020B2F24
	add r1, r0, #1
	mul r6, r1, r2
	mul r1, r0, r2
	add r0, r6, #0xdf
	add r2, r0, #0x13400
	ldr r0, _020B303C ; =0xFFFF2F5C
	add r1, r1, #0xdf
	str r2, [r7, #0x8c]
	add r0, r2, r0
	str r0, [r7, #0x88]
	add r0, r1, #0x13400
	str r0, [r7, #4]
	str r3, [r7, #0x90]
	b _020B2F84
_020B2F24:
	ldr r1, [r7, #0x88]
	cmp ip, r1
	bge _020B2F84
	ldr r1, [r7, #0x90]
	add r1, r1, #1
	str r1, [r7, #0x90]
	cmp r1, #0x46
	bls _020B2F84
	add r1, r0, #1
	mul ip, r1, r2
	add r1, ip, #0xdf
	add r1, r1, #0x13400
	ldr r6, _020B3040 ; =0xFFFFBA74
	str r1, [r7, #0x8c]
	ldr r1, _020B3034 ; =0x00008B18
	mlas r2, r0, r2, r6
	str r2, [r7, #0x88]
	mul r1, r0, r1
	add r0, r1, #0xdf
	strmi r3, [r7, #0x88]
	add r0, r0, #0x13400
	mov r2, #0
	str r0, [r7, #4]
	str r2, [r7, #0x90]
_020B2F84:
	ldr r1, [r7, #4]
	ldr r0, _020B3044 ; =0x00068520
	cmp r1, r0
	strhi r0, [r7, #4]
	ldr r0, [r7, #0x6c]
	cmp r0, #0x10
	bhs _020B2FB4
	add r0, r0, #1
	str r0, [r7, #0x6c]
	str r4, [r7, #0x74]
	str r4, [r7, #0x78]
	b _020B2FD0
_020B2FB4:
	ldr r0, [r7, #0x74]
	rsb r0, r0, r0, lsl #5
	add r1, r4, r0
	mov r0, r1, asr #4
	add r0, r1, r0, lsr #27
	mov r0, r0, asr #5
	str r0, [r7, #0x74]
_020B2FD0:
	ldr r2, [r7, #0x78]
	ldr r1, [r7, #0x74]
	ldr r0, _020B3048 ; =0x00008701
	sub r1, r2, r1
	cmp r1, r0
	mvn r0, #0x8700
	movgt r5, #1
	str r1, [r7, #0xc]
	cmp r1, r0
	bge _020B3008
	ldr r1, [r7, #0x70]
	add r5, r0, #0x8700
	add r0, r1, #1
	str r0, [r7, #0x70]
_020B3008:
	cmp r5, #0
	movne r0, #0
	strne r0, [r7, #0x6c]
	strne r0, [r7, #0x4c]
_020B3018:
	ldr r1, [r7, #0x84]
	mov r0, #1
	sub r1, r1, #1
	str r1, [r7, #0x84]
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.align 2, 0
_020B302C: .word 0x0000CC8D
_020B3030: .word 0x75CA82CB
_020B3034: .word 0x00008B18
_020B3038: .word 0x0007FD88
_020B303C: .word 0xFFFF2F5C
_020B3040: .word 0xFFFFBA74
_020B3044: .word 0x00068520
_020B3048: .word 0x00008701
	arm_func_end vct_calc_jitter_and_skew
