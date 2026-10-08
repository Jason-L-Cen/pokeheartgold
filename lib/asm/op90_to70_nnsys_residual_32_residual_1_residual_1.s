	.extern CalcSpriteParams_
	.extern NNS_G2dIsRndCoreFlipH
	.extern NNS_G2dIsRndCoreFlipV
	.extern _021092F8
	.extern _02109310
	.extern _021093CC
	.extern _021109FC
	.extern _021DA47C
	.extern _021DA498
	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"
	.public DoAffineTransforme_
	.public NNS_G2dDrawOneOam3DDirectWithPosFast

.text
	arm_func_start NNS_G2dDrawOneOam3DDirectWithPosFast
NNS_G2dDrawOneOam3DDirectWithPosFast: ; 0x020BB234
	stmdb sp!, {r4, r5, r6, r7, lr}
	sub sp, sp, #0x1c
	add ip, sp, #4
	mov r5, r1
	mov r4, r2
	mov r7, r3
	mov r6, r0
	ldr r1, [sp, #0x30]
	ldr r2, [sp, #0x34]
	ldr r3, [sp, #0x38]
	mov r0, r7
	str ip, [sp]
	bl CalcSpriteParams_
	ldr r2, [r7]
	and r1, r2, #0x300
	cmp r1, #0x100
	cmpne r1, #0x300
	andne r0, r2, #0x30000000
	orrne r1, r1, r0
	cmp r1, #0x300
	bne _020BB2F8
	ldr r0, _020BB468 ; =0xC000C000
	ldr r3, _020BB46C ; =_02109310
	and r2, r2, r0
	and r0, r2, #0xc0000000
	and r2, r2, #0xc000
	mov ip, r2, asr #0xe
	mov lr, r0, lsr #0x1e
	ldr r1, _020BB470 ; =_021DA47C
	ldr r2, _020BB474 ; =_021092F8
	ldr r0, [r1, #8]
	mov lr, lr, lsl #1
	cmp r0, #0
	ldrne r0, [r1, #4]
	add r3, r3, ip, lsl #3
	add r2, r2, ip, lsl #3
	ldrh ip, [lr, r3]
	ldrh r3, [lr, r2]
	addne r4, r0, r4, lsl #12
	add r2, r6, ip, asr #1
	add r0, r5, r3, asr #1
	ldr r1, _020BB478 ; =0x04000470
	mov r2, r2, lsl #0xc
	str r2, [r1]
	mov r0, r0, lsl #0xc
	moveq r4, r4, lsl #0xc
	str r0, [r1]
	str r4, [r1]
	b _020BB328
_020BB2F8:
	ldr r0, _020BB470 ; =_021DA47C
	ldr r1, [r0, #8]
	cmp r1, #0
	ldrne r0, [r0, #4]
	ldr r1, _020BB478 ; =0x04000470
	addne r2, r0, r4, lsl #12
	mov r0, r6, lsl #0xc
	str r0, [r1]
	mov r0, r5, lsl #0xc
	moveq r2, r4, lsl #0xc
	str r0, [r1]
	str r2, [r1]
_020BB328:
	ldr r0, [sp, #0x14]
	ldr r2, [sp, #0x18]
	ldr r1, _020BB47C ; =0x0400046C
	mov r0, r0, lsl #0xc
	str r0, [r1]
	mov r0, r2, lsl #0xc
	str r0, [r1]
	mov r0, #0x1000
	str r0, [r1]
	mov r0, #1
	str r0, [r1, #0x94]
	ldr r0, [sp, #0x10]
	ldr r3, [sp, #4]
	mov r0, r0, lsl #8
	mov r2, r0, asr #0x10
	mov r0, r3, lsl #8
	mov r2, r2, lsl #0x10
	mov r4, r0, asr #0x10
	mov r3, r2, lsr #0x10
	mov r2, r4, lsl #0x10
	mov r3, r3, lsl #0x10
	orr r2, r3, r2, lsr #16
	mov r0, #0x10000
	str r2, [r1, #0x1c]
	str r0, [r1, #0x24]
	ldr r2, [sp, #0x10]
	ldr r4, [sp, #0xc]
	mov r2, r2, lsl #8
	mov r3, r2, asr #0x10
	mov r2, r4, lsl #8
	mov r3, r3, lsl #0x10
	mov r2, r2, asr #0x10
	mov r3, r3, lsr #0x10
	mov r2, r2, lsl #0x10
	mov r3, r3, lsl #0x10
	orr r2, r3, r2, lsr #16
	str r2, [r1, #0x1c]
	add r0, r0, #0x40
	str r0, [r1, #0x24]
	ldr r0, [sp, #8]
	ldr r3, [sp, #0xc]
	mov r0, r0, lsl #8
	mov r2, r0, asr #0x10
	mov r0, r3, lsl #8
	mov r2, r2, lsl #0x10
	mov r0, r0, asr #0x10
	mov r2, r2, lsr #0x10
	mov r0, r0, lsl #0x10
	mov r2, r2, lsl #0x10
	orr r0, r2, r0, lsr #16
	str r0, [r1, #0x1c]
	mov r0, #0x40
	str r0, [r1, #0x24]
	ldr r0, [sp, #8]
	ldr r3, [sp, #4]
	mov r0, r0, lsl #8
	mov r2, r0, asr #0x10
	mov r0, r3, lsl #8
	mov r2, r2, lsl #0x10
	mov r0, r0, asr #0x10
	mov r2, r2, lsr #0x10
	mov r0, r0, lsl #0x10
	mov r2, r2, lsl #0x10
	orr r0, r2, r0, lsr #16
	str r0, [r1, #0x1c]
	mov r0, #0
	str r0, [r1, #0x24]
	ldr r2, _020BB470 ; =_021DA47C
	str r0, [r1, #0x98]
	ldr r0, [r2, #8]
	cmp r0, #0
	addeq sp, sp, #0x1c
	ldmeqia sp!, {r4, r5, r6, r7, pc}
	ldr r0, _020BB480 ; =_021109FC
	ldr r1, [r2, #4]
	ldr r0, [r0]
	add r0, r1, r0
	str r0, [r2, #4]
	add sp, sp, #0x1c
	ldmia sp!, {r4, r5, r6, r7, pc}
	.align 2, 0
_020BB468: .word 0xC000C000
_020BB46C: .word _02109310
_020BB470: .word _021DA47C
_020BB474: .word _021092F8
_020BB478: .word 0x04000470
_020BB47C: .word 0x0400046C
_020BB480: .word _021109FC
	arm_func_end NNS_G2dDrawOneOam3DDirectWithPosFast

	arm_func_start DoAffineTransforme_
DoAffineTransforme_: ; 0x020BB484
	stmdb sp!, {r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0x10
	ldr r4, [r1]
	ldr r3, _020BB728 ; =0x01FF0000
	and r3, r4, r3
	mov r3, r3, lsr #0x10
	mov r3, r3, lsl #0x10
	mov r3, r3, asr #0x10
	cmp r3, #0xff
	orrgt r3, r3, #0xff00
	movgt r3, r3, lsl #0x10
	movgt r3, r3, asr #0x10
	mov r3, r3, lsl #0xc
	str r3, [sp, #8]
	ldr r3, [r1]
	and r3, r3, #0xff
	cmp r3, #0x7f
	orrgt r3, r3, #0xff00
	movgt r3, r3, lsl #0x10
	movgt r3, r3, asr #0x10
	mov r3, r3, lsl #0xc
	str r3, [sp, #0xc]
	ldr r5, [r1]
	and r4, r5, #0x300
	cmp r4, #0x100
	cmpne r4, #0x300
	andne r3, r5, #0x30000000
	orrne r4, r4, r3
	cmp r4, #0x300
	bne _020BB548
	ldr r3, _020BB72C ; =0xC000C000
	ldr r4, _020BB730 ; =_02109310
	and r3, r5, r3
	and r5, r3, #0xc0000000
	and r3, r3, #0xc000
	mov r6, r5, lsr #0x1e
	mov r5, r3, asr #0xe
	ldr r3, _020BB734 ; =_021092F8
	mov r6, r6, lsl #1
	add r4, r4, r5, lsl #3
	add r3, r3, r5, lsl #3
	ldrh r5, [r6, r4]
	ldr r7, [sp, #8]
	ldrh r3, [r6, r3]
	ldr r4, [sp, #0xc]
	add r5, r7, r5, lsl #11
	add r3, r4, r3, lsl #11
	str r5, [sp, #8]
	str r3, [sp, #0xc]
_020BB548:
	ldr r6, [r0, #8]
	ldr r4, [sp, #0xc]
	add r3, sp, #8
	smull r4, r7, r6, r4
	cmp r3, r3
	addeq r3, sp, #0
	adds r8, r4, #0x1000
	ldr r5, [r0]
	ldr r4, [sp, #8]
	adc r6, r7, #0
	smlal r8, r6, r5, r4
	mov r4, r8, lsr #0xc
	orr r4, r4, r6, lsl #20
	str r4, [r3]
	ldr r6, [r0, #0xc]
	ldr r4, [sp, #0xc]
	ldr r5, [r0, #4]
	smull r4, r7, r6, r4
	adds r8, r4, #0x1000
	ldr r4, [sp, #8]
	adc r6, r7, #0
	smlal r8, r6, r5, r4
	mov r5, r8, lsr #0xc
	add r4, sp, #0
	orr r5, r5, r6, lsl #20
	str r5, [r3, #4]
	cmp r3, r4
	bne _020BB5C8
	ldr r4, [sp]
	ldr r3, [sp, #4]
	str r4, [sp, #8]
	str r3, [sp, #0xc]
_020BB5C8:
	ldr r3, _020BB738 ; =_021DA498
	ldr r3, [r3]
	ldr r3, [r3, #4]
	cmp r3, #0
	beq _020BB610
	cmp r3, #2
	moveq r5, #0x300
	movne r5, #0x100
	ldr r4, [r1]
	ldr r3, _020BB73C ; =0xC1FFFCFF
	cmp r5, #0x100
	andeq r3, r4, r3
	orreq r3, r3, r5
	streq r3, [r1]
	beq _020BB610
	and r3, r4, r3
	orr r3, r3, r5
	str r3, [r1]
_020BB610:
	ldr r6, [r1]
	ldr r5, _020BB730 ; =_02109310
	and r4, r6, #0x300
	cmp r4, #0x100
	cmpne r4, #0x300
	andne r3, r6, #0x30000000
	orrne r4, r4, r3
	ldr r3, _020BB72C ; =0xC000C000
	ldr r8, _020BB734 ; =_021092F8
	and r3, r6, r3
	and r6, r3, #0xc0000000
	and r3, r3, #0xc000
	mov lr, r3, asr #0xe
	mov r6, r6, lsr #0x1e
	mov r3, r6, lsl #1
	add r5, r5, lr, lsl #3
	ldrh r5, [r3, r5]
	add lr, r8, lr, lsl #3
	ldrh r3, [r3, lr]
	mov ip, r5, asr #1
	mov r5, ip, lsl #0xc
	ldr r6, [r0]
	rsb r7, r5, #0
	mla r7, r6, ip, r7
	ldr r6, [r0, #8]
	mov r3, r3, asr #1
	mla r7, r6, r3, r7
	ldr lr, [sp, #8]
	cmp r4, #0x300
	add r6, lr, r7
	str r6, [sp, #8]
	mov lr, r3, lsl #0xc
	ldr r6, [r0, #4]
	rsb r7, lr, #0
	mla r7, r6, ip, r7
	ldr r0, [r0, #0xc]
	ldr r6, [sp, #0xc]
	mla r3, r0, r3, r7
	add r0, r6, r3
	str r0, [sp, #0xc]
	moveq r0, #1
	movne r0, #0
	cmp r0, #0
	beq _020BB6D8
	ldr r3, [sp, #8]
	ldr r0, [sp, #0xc]
	sub r3, r3, r5
	sub r0, r0, lr
	str r3, [sp, #8]
	str r0, [sp, #0xc]
_020BB6D8:
	ldr r4, [sp, #8]
	ldr r0, [r2]
	ldr r3, [sp, #0xc]
	add r4, r4, r0
	str r4, [sp, #8]
	ldr r0, [r2, #4]
	mov r4, r4, asr #0xc
	add r0, r3, r0
	str r0, [sp, #0xc]
	mov r2, r0, asr #0xc
	ldr r3, [r1]
	ldr r0, _020BB740 ; =0xFE00FF00
	and r2, r2, #0xff
	and r0, r3, r0
	mov r3, r4, lsl #0x17
	orr r0, r0, r2
	orr r0, r0, r3, lsr #7
	str r0, [r1]
	add sp, sp, #0x10
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	.align 2, 0
_020BB728: .word 0x01FF0000
_020BB72C: .word 0xC000C000
_020BB730: .word _02109310
_020BB734: .word _021092F8
_020BB738: .word _021DA498
_020BB73C: .word 0xC1FFFCFF
_020BB740: .word 0xFE00FF00
	arm_func_end DoAffineTransforme_
