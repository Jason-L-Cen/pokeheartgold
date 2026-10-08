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
	.extern NNS_G2dDrawOneOam3DDirectWithPosFast

.text
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
