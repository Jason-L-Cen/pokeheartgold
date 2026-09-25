	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"
	.public DrawCellToSurface2D_
	.public DrawCellToSurface3D_
	.extern CalcSpriteParams_
	.extern NNS_G2dIsRndCoreFlipH
	.extern NNS_G2dIsRndCoreFlipV
	.extern _021092F8
	.extern _02109310
	.extern _021093CC
	.extern _021109FC
	.extern _021DA47C
	.extern _021DA498

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

	arm_func_start DrawCellToSurface2D_
DrawCellToSurface2D_: ; 0x020BB744
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x40
	ldr r2, _020BBB44 ; =_021DA498
	mov r6, r0
	ldr r4, [r2]
	ldr r0, _020BBB44 ; =_021DA498
	ldr r5, [r4, #0x2c]
	mov fp, r1
	cmp r5, #0
	ldreq r5, _020BBB48 ; =_021093CC
	ldr r2, [r0]
	ldr r3, [r5, #0x10]
	str r3, [sp, #0x18]
	ldr r1, [r5, #0x14]
	str r1, [sp, #0x1c]
	ldr r0, [r6]
	sub r0, r3, r0
	str r0, [sp, #0x18]
	ldr r0, [r6, #4]
	sub r0, r1, r0
	str r0, [sp, #0x1c]
	ldr r1, [r2, #0x24]
	ldr r0, [r2, #0x10]
	cmp r1, #0
	str r0, [sp, #0xc]
	bne _020BB7C0
	ldr r0, [r2, #0x28]
	cmp r0, #0
	movne r0, #1
	strne r0, [sp]
	bne _020BB7C8
_020BB7C0:
	mov r0, #0
	str r0, [sp]
_020BB7C8:
	ldrh r0, [fp]
	mov r8, #0
	cmp r0, #0
	addls sp, sp, #0x40
	ldmlsia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	mov r0, #0x400
	str r0, [sp, #0x10]
	ldr r0, _020BBB4C ; =0x0000FFFE
	ldr r7, _020BBB44 ; =_021DA498
	add r0, r0, #1
	str r0, [sp, #0x14]
	ldr r0, [sp, #0x10]
	rsb r0, r0, #0
	str r0, [sp, #0x10]
_020BB800:
	mov r0, #6
	mul r1, r8, r0
	ldr r2, [r7]
	mov r0, #1
	str r0, [r2, #0x30]
	ldr r0, [fp, #4]
	add r2, r0, r1
	ldrh r0, [r0, r1]
	strh r0, [r4, #0x38]
	ldrh r0, [r2, #2]
	strh r0, [r4, #0x3a]
	ldrh r0, [r2, #4]
	strh r0, [r4, #0x3c]
	ldr r3, [r6, #0x20]
	cmp r3, #0
	beq _020BB850
	ldr r0, [r7]
	mov r1, fp
	mov r2, r8
	blx r3
_020BB850:
	ldr r0, [r7]
	ldr r0, [r0, #0x30]
	cmp r0, #0
	beq _020BBB24
	ldr r0, [sp]
	ldr r1, [r4, #0x3c]
	cmp r0, #0
	ldr r0, [sp, #0x10]
	and r2, r1, r0
	ldr r0, [sp, #0xc]
	mov r1, r1, lsl #0x16
	add r1, r0, r1, lsr #22
	ldr r0, [sp, #0x10]
	and r0, r1, r0, lsr #22
	orr r0, r2, r0
	str r0, [r4, #0x3c]
	beq _020BB994
	ldr r0, [r4, #0x38]
	ldr lr, [r7]
	mov r1, r0, lsl #2
	mov r0, r0, lsl #3
	mov r1, r1, lsr #0x1f
	mov r0, r0, lsr #0x1f
	orr sl, r0, r1, lsl #1
	ldr sb, [lr, #0x28]
	ldr r1, [r6, #0x14]
	add r0, sb, sl, lsl #2
	add r0, r0, r1, lsl #1
	ldrh r1, [r0, #0xe]
	ldr r0, [sp, #0x14]
	str r1, [sp, #8]
	cmp r1, r0
	bne _020BB960
	cmp sl, #0
	bne _020BB8F0
	ldr r1, [lr, #0x20]
	mov r0, sb
	blx r1
	str r0, [sp, #8]
	b _020BB94C
_020BB8F0:
	add ip, sp, #0x20
	ldmia sb, {r0, r1, r2, r3}
	stmia ip, {r0, r1, r2, r3}
	tst sl, #1
	beq _020BB91C
	ldr r1, [sp, #0x20]
	ldr r0, [sp, #0x24]
	rsb r1, r1, #0
	str r1, [sp, #0x20]
	rsb r0, r0, #0
	str r0, [sp, #0x24]
_020BB91C:
	tst sl, #2
	beq _020BB93C
	ldr r1, [sp, #0x28]
	ldr r0, [sp, #0x2c]
	rsb r1, r1, #0
	str r1, [sp, #0x28]
	rsb r0, r0, #0
	str r0, [sp, #0x2c]
_020BB93C:
	ldr r1, [lr, #0x20]
	add r0, sp, #0x20
	blx r1
	str r0, [sp, #8]
_020BB94C:
	ldr r1, [r6, #0x14]
	add r0, sb, sl, lsl #2
	add r1, r0, r1, lsl #1
	ldr r0, [sp, #8]
	strh r0, [r1, #0xe]
_020BB960:
	ldr r1, [r5]
	add r0, sp, #0x30
	str r1, [sp, #0x30]
	ldr r2, [r5, #4]
	add r1, r4, #0x38
	str r2, [sp, #0x34]
	ldr r3, [r5, #8]
	add r2, sp, #0x18
	str r3, [sp, #0x38]
	ldr r3, [r5, #0xc]
	str r3, [sp, #0x3c]
	bl DoAffineTransforme_
	b _020BBAE4
_020BB994:
	ldr r0, _020BBB4C ; =0x0000FFFE
	str r0, [sp, #8]
	ldr r0, [r7]
	ldr r1, [r0, #0x24]
	cmp r1, #0
	beq _020BBA9C
	ldr r2, [r4, #0x38]
	ldr r1, _020BBB50 ; =0xC000C000
	mov sl, r2, lsl #3
	and sb, r2, r1
	bl NNS_G2dIsRndCoreFlipH
	eor r0, r0, sl, lsr #31
	ldr r1, [r4, #0x38]
	str r0, [sp, #4]
	ldr r0, [r7]
	mov sl, r1, lsl #2
	bl NNS_G2dIsRndCoreFlipV
	eor r2, r0, sl, lsr #31
	ldr r0, [r4, #0x38]
	bic r1, r0, #0x10000000
	ldr r0, [sp, #4]
	orr r0, r1, r0, lsl #28
	bic r0, r0, #0x20000000
	orr r0, r0, r2, lsl #29
	str r0, [r4, #0x38]
	ldr r0, [r7]
	bl NNS_G2dIsRndCoreFlipH
	cmp r0, #0
	beq _020BBA4C
	and r1, sb, #0xc000
	mov r2, r1, asr #0xe
	ldr r1, _020BBB54 ; =_02109310
	and r3, sb, #0xc0000000
	mov r3, r3, lsr #0x1e
	add r2, r1, r2, lsl #3
	mov r3, r3, lsl #1
	ldr r0, [r4, #0x38]
	ldr r1, _020BBB58 ; =0xFE00FFFF
	ldrh r2, [r3, r2]
	and r1, r0, r1
	mov r0, r0, lsl #7
	add r0, r2, r0, lsr #23
	rsb r0, r0, #0
	mov r0, r0, lsl #0x17
	orr r0, r1, r0, lsr #7
	str r0, [r4, #0x38]
_020BBA4C:
	ldr r0, [r7]
	bl NNS_G2dIsRndCoreFlipV
	cmp r0, #0
	beq _020BBA9C
	and r1, sb, #0xc000
	mov r3, r1, asr #0xe
	and r1, sb, #0xc0000000
	mov r1, r1, lsr #0x1e
	mov r2, r1, lsl #1
	ldr r1, _020BBB5C ; =_021092F8
	ldr r0, [r4, #0x38]
	add r1, r1, r3, lsl #3
	ldrh r1, [r2, r1]
	bic r2, r0, #0xff
	mov r0, r0, lsl #0x18
	add r0, r1, r0, lsr #24
	rsb r0, r0, #0
	and r0, r0, #0xff
	orr r0, r2, r0
	str r0, [r4, #0x38]
_020BBA9C:
	ldr r2, [r4, #0x38]
	ldr r0, _020BBB58 ; =0xFE00FFFF
	ldr r1, [sp, #0x18]
	and r3, r2, r0
	mov r2, r2, lsl #7
	mov r0, r1, asr #0xc
	add r0, r0, r2, lsr #23
	mov r0, r0, lsl #0x17
	orr r0, r3, r0, lsr #7
	str r0, [r4, #0x38]
	bic r2, r0, #0xff
	mov r1, r0, lsl #0x18
	ldr r0, [sp, #0x1c]
	mov r0, r0, asr #0xc
	add r0, r0, r1, lsr #24
	and r0, r0, #0xff
	orr r0, r2, r0
	str r0, [r4, #0x38]
_020BBAE4:
	ldr r3, [r7]
	ldr r1, [sp, #8]
	ldr r3, [r3, #0x1c]
	add r0, r4, #0x38
	mov r2, #0
	blx r3
	cmp r0, #1
	addne sp, sp, #0x40
	ldmneia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldr r3, [r6, #0x24]
	cmp r3, #0
	beq _020BBB24
	ldr r0, [r7]
	mov r1, fp
	mov r2, r8
	blx r3
_020BBB24:
	add r0, r8, #1
	mov r0, r0, lsl #0x10
	mov r8, r0, lsr #0x10
	ldrh r1, [fp]
	cmp r1, r0, lsr #16
	bhi _020BB800
	add sp, sp, #0x40
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_020BBB44: .word _021DA498
_020BBB48: .word _021093CC
_020BBB4C: .word 0x0000FFFE
_020BBB50: .word 0xC000C000
_020BBB54: .word _02109310
_020BBB58: .word 0xFE00FFFF
_020BBB5C: .word _021092F8
	arm_func_end DrawCellToSurface2D_

	arm_func_start DrawOamToSurface3D_
DrawOamToSurface3D_: ; 0x020BBB60
	stmdb sp!, {r3, r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0xc
	ldr r1, _020BBD94 ; =_021DA498
	mov r7, r0
	ldr r0, [r1]
	add r0, r0, #0x40
	bl G3_LoadMtx43
	ldr r0, _020BBD94 ; =_021DA498
	ldr r0, [r0]
	ldr r1, [r0, #0x24]
	cmp r1, #0
	beq _020BBD24
	ldr r2, [r7]
	ldr r1, _020BBD98 ; =0xC000C000
	and r4, r2, r1
	bl NNS_G2dIsRndCoreFlipH
	cmp r0, #0
	ldr r0, [r7]
	beq _020BBBF0
	mov r0, r0, lsl #7
	mov r3, r0, lsr #0x17
	and r2, r4, #0xc000
	and r1, r4, #0xc0000000
	mov r1, r1, lsr #0x1e
	ldr r0, _020BBD9C ; =_02109310
	mov r2, r2, asr #0xe
	mov r1, r1, lsl #1
	add r0, r0, r2, lsl #3
	cmp r3, #0xff
	ldrh r1, [r1, r0]
	orrgt r0, r3, #0xff00
	movgt r0, r0, lsl #0x10
	movgt r3, r0, asr #0x10
	add r0, r3, r1
	rsb r0, r0, #0
	b _020BBC08
_020BBBF0:
	mov r0, r0, lsl #7
	mov r0, r0, lsr #0x17
	cmp r0, #0xff
	orrgt r0, r0, #0xff00
	movgt r0, r0, lsl #0x10
	movgt r0, r0, asr #0x10
_020BBC08:
	mov r0, r0, lsl #0x10
	mov r1, r0, lsr #0x10
	ldr r0, _020BBD94 ; =_021DA498
	mov r1, r1, lsl #0x10
	ldr r0, [r0]
	mov r5, r1, asr #0x10
	bl NNS_G2dIsRndCoreFlipV
	cmp r0, #0
	ldr r0, [r7]
	beq _020BBC74
	mov r0, r0, lsl #0x18
	mov r3, r0, lsr #0x18
	and r2, r4, #0xc000
	and r1, r4, #0xc0000000
	mov r1, r1, lsr #0x1e
	ldr r0, _020BBDA0 ; =_021092F8
	mov r2, r2, asr #0xe
	mov r1, r1, lsl #1
	add r0, r0, r2, lsl #3
	cmp r3, #0x7f
	ldrh r1, [r1, r0]
	orrgt r0, r3, #0xff00
	movgt r0, r0, lsl #0x10
	movgt r3, r0, asr #0x10
	add r0, r3, r1
	rsb r0, r0, #0
	b _020BBC8C
_020BBC74:
	mov r0, r0, lsl #0x18
	mov r0, r0, lsr #0x18
	cmp r0, #0x7f
	orrgt r0, r0, #0xff00
	movgt r0, r0, lsl #0x10
	movgt r0, r0, asr #0x10
_020BBC8C:
	mov r0, r0, lsl #0x10
	mov r4, r0, lsr #0x10
	ldr r0, _020BBD94 ; =_021DA498
	ldr r1, [r7]
	ldr r0, [r0]
	mov r6, r1, lsl #3
	bl NNS_G2dIsRndCoreFlipH
	ldr r2, [r7]
	ldr r1, _020BBD94 ; =_021DA498
	eor r6, r0, r6, lsr #31
	ldr r0, [r1]
	mov r8, r2, lsl #2
	bl NNS_G2dIsRndCoreFlipV
	ldr r1, [r7]
	eor r3, r0, r8, lsr #31
	bic r2, r1, #0x10000000
	orr r2, r2, r6, lsl #28
	bic r2, r2, #0x20000000
	mov r1, r4, lsl #0x10
	orr r3, r2, r3, lsl #29
	str r2, [r7]
	str r3, [r7]
	ldr r2, _020BBD94 ; =_021DA498
	mov r0, r5
	ldr r5, [r2]
	mov r1, r1, asr #0x10
	ldr r4, [r5, #8]
	mov r3, r7
	add r4, r4, #0xc
	str r4, [sp]
	ldr r4, [r5, #0x14]
	mvn r2, #0
	str r4, [sp, #4]
	ldr r4, [r5, #0x18]
	str r4, [sp, #8]
	bl NNS_G2dDrawOneOam3DDirectWithPosFast
	add sp, sp, #0xc
	ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
_020BBD24:
	ldr r2, _020BBD94 ; =_021DA498
	ldr r0, [r7]
	ldr r5, [r2]
	mov r0, r0, lsl #7
	ldr r1, [r7]
	ldr r4, [r5, #8]
	mov r0, r0, lsr #0x17
	cmp r0, #0xff
	orrgt r0, r0, #0xff00
	mov r1, r1, lsl #0x18
	add r4, r4, #0xc
	movgt r0, r0, lsl #0x10
	str r4, [sp]
	ldr r4, [r5, #0x14]
	mov r1, r1, lsr #0x18
	movgt r0, r0, asr #0x10
	cmp r1, #0x7f
	orrgt r1, r1, #0xff00
	movgt r1, r1, lsl #0x10
	str r4, [sp, #4]
	ldr r4, [r5, #0x18]
	movgt r1, r1, asr #0x10
	mov r3, r7
	mvn r2, #0
	str r4, [sp, #8]
	bl NNS_G2dDrawOneOam3DDirectWithPosFast
	add sp, sp, #0xc
	ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
	.align 2, 0
_020BBD94: .word _021DA498
_020BBD98: .word 0xC000C000
_020BBD9C: .word _02109310
_020BBDA0: .word _021092F8
	arm_func_end DrawOamToSurface3D_

	arm_func_start DrawCellToSurface3D_
DrawCellToSurface3D_: ; 0x020BBDA4
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, lr}
	mov sb, r1
	ldr r5, _020BBE68 ; =_021DA498
	ldrh r1, [sb]
	ldr r7, [r5]
	mov sl, r0
	mov r8, #0
	cmp r1, #0
	ldmlsia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
	mov r6, #1
	mov r4, #6
_020BBDD0:
	mul r1, r8, r4
	ldr r0, [r5]
	str r6, [r0, #0x30]
	ldr r2, [sb, #4]
	ldrh r0, [r2, r1]
	add r1, r2, r1
	strh r0, [r7, #0x38]
	ldrh r0, [r1, #2]
	strh r0, [r7, #0x3a]
	ldrh r0, [r1, #4]
	strh r0, [r7, #0x3c]
	ldr r3, [sl, #0x20]
	cmp r3, #0
	beq _020BBE18
	ldr r0, [r5]
	mov r1, sb
	mov r2, r8
	blx r3
_020BBE18:
	ldr r0, [r5]
	ldr r0, [r0, #0x30]
	cmp r0, #0
	beq _020BBE30
	add r0, r7, #0x38
	bl DrawOamToSurface3D_
_020BBE30:
	ldr r3, [sl, #0x24]
	cmp r3, #0
	beq _020BBE4C
	ldr r0, [r5]
	mov r1, sb
	mov r2, r8
	blx r3
_020BBE4C:
	ldrh r1, [sb]
	add r0, r8, #1
	mov r0, r0, lsl #0x10
	cmp r1, r0, lsr #16
	mov r8, r0, lsr #0x10
	bhi _020BBDD0
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
	.align 2, 0
_020BBE68: .word _021DA498
	arm_func_end DrawCellToSurface3D_
