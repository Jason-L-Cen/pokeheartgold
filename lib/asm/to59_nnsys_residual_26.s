	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"
	.public NNS_G2dDrawMultiCell
	.extern DrawCellAnimationImpl_
	.extern DrawNode_
	.extern NNS_G2dIsRndCoreFlipH
	.extern NNS_G2dIsRndCoreFlipV
	.extern NNS_G2dPopMtx
	.extern NNS_G2dPushMtx
	.extern NNS_G2dTranslate
	.extern NNSi_G2dGetOamSoftEmuAutoZOffsetStep
	.extern NNSi_G2dResetOamSoftEmuAutoZOffset
	.extern NNSi_G2dSetOamSoftEmuAutoZOffsetFlag
	.extern NNSi_G2dSetOamSoftEmuAutoZOffsetStep
	.extern _021D94D8
	.extern _021D9FD8
	.extern _021DA068

.text
	arm_func_start NNS_G2dDrawMultiCell
NNS_G2dDrawMultiCell: ; 0x020BA1B0
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	mov r5, r0
	ldr r0, [r5, #8]
	cmp r0, #1
	bne _020BA3AC
	ldr r0, [r5]
	mov r2, #0
	ldrh r0, [r0, #2]
	cmp r0, #0
	bls _020BA204
	ldr r3, _020BA478 ; =_021DA068
	mov r4, r2
_020BA1E0:
	add r0, r3, r2, lsl #2
	str r4, [r0, #4]
	ldr r0, [r5]
	add r2, r2, #1
	ldrh r1, [r0, #2]
	mov r0, r2, lsl #0x10
	mov r2, r0, lsr #0x10
	cmp r1, r0, lsr #16
	bhi _020BA1E0
_020BA204:
	ldr r6, _020BA47C ; =_021D94D8
	mov r0, #1
	str r0, [r6, #0xf94]
	ldr r0, [r6, #8]
	mov sl, #0
	ldr r0, [r0, #0x80]
	cmp r0, #0
	beq _020BA2FC
	bl NNSi_G2dGetOamSoftEmuAutoZOffsetStep
	str r0, [sp]
	mov r0, #1
	bl NNSi_G2dSetOamSoftEmuAutoZOffsetFlag
	mov r0, r6
	ldr r0, [r0, #8]
	ldr r0, [r0, #0x80]
	bl NNSi_G2dSetOamSoftEmuAutoZOffsetStep
	ldr r2, [r5]
	ldrh r0, [r2]
	cmp r0, #0
	bls _020BA2E4
	ldr fp, _020BA480 ; =_021D9FD8
_020BA258:
	ldr r0, [r2, #4]
	ldr r8, [r5, #0xc]
	add r7, r0, sl, lsl #3
	ldrh r0, [r7, #6]
	and r0, r0, #0xff00
	mov r0, r0, lsl #8
	mov sb, r0, lsr #0x10
	strh sb, [fp, #0x90]
	bl NNS_G2dPushMtx
	ldrsh r4, [r7, #4]
	ldr r0, [r6, #8]
	ldrsh r7, [r7, #2]
	bl NNS_G2dIsRndCoreFlipH
	cmp r0, #0
	ldr r0, [r6, #8]
	rsbne r7, r7, #0
	bl NNS_G2dIsRndCoreFlipV
	cmp r0, #0
	rsbne r4, r4, #0
	mov r2, #0
	mov r0, r7, lsl #0xc
	mov r1, r4, lsl #0xc
	bl NNS_G2dTranslate
	mov r0, #0x5c
	mla r0, sb, r0, r8
	bl DrawCellAnimationImpl_
	mov r0, #1
	bl NNS_G2dPopMtx
	ldr r2, [r5]
	add r0, sl, #1
	mov r0, r0, lsl #0x10
	ldrh r1, [r2]
	mov sl, r0, lsr #0x10
	cmp r1, r0, lsr #16
	bhi _020BA258
_020BA2E4:
	mov r0, #0
	bl NNSi_G2dSetOamSoftEmuAutoZOffsetFlag
	ldr r0, [sp]
	bl NNSi_G2dSetOamSoftEmuAutoZOffsetStep
	bl NNSi_G2dResetOamSoftEmuAutoZOffset
	b _020BA39C
_020BA2FC:
	ldr r2, [r5]
	ldrh r0, [r2]
	cmp r0, #0
	bls _020BA39C
	ldr fp, _020BA480 ; =_021D9FD8
_020BA310:
	ldr r0, [r2, #4]
	ldr r8, [r5, #0xc]
	add r7, r0, sl, lsl #3
	ldrh r0, [r7, #6]
	and r0, r0, #0xff00
	mov r0, r0, lsl #8
	mov sb, r0, lsr #0x10
	strh sb, [fp, #0x90]
	bl NNS_G2dPushMtx
	ldrsh r4, [r7, #4]
	ldr r0, [r6, #8]
	ldrsh r7, [r7, #2]
	bl NNS_G2dIsRndCoreFlipH
	cmp r0, #0
	ldr r0, [r6, #8]
	rsbne r7, r7, #0
	bl NNS_G2dIsRndCoreFlipV
	cmp r0, #0
	rsbne r4, r4, #0
	mov r2, #0
	mov r0, r7, lsl #0xc
	mov r1, r4, lsl #0xc
	bl NNS_G2dTranslate
	mov r0, #0x5c
	mla r0, sb, r0, r8
	bl DrawCellAnimationImpl_
	mov r0, #1
	bl NNS_G2dPopMtx
	ldr r2, [r5]
	add r0, sl, #1
	mov r0, r0, lsl #0x10
	ldrh r1, [r2]
	mov sl, r0, lsr #0x10
	cmp r1, r0, lsr #16
	bhi _020BA310
_020BA39C:
	ldr r0, _020BA47C ; =_021D94D8
	mov r1, #0
	str r1, [r0, #0xf94]
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020BA3AC:
	ldr r0, _020BA47C ; =_021D94D8
	ldr r4, [r5, #0xc]
	ldr r0, [r0, #8]
	ldr r0, [r0, #0x80]
	cmp r0, #0
	beq _020BA438
	bl NNSi_G2dGetOamSoftEmuAutoZOffsetStep
	mov r6, r0
	mov r0, #1
	bl NNSi_G2dSetOamSoftEmuAutoZOffsetFlag
	ldr r0, _020BA47C ; =_021D94D8
	ldr r0, [r0, #8]
	ldr r0, [r0, #0x80]
	bl NNSi_G2dSetOamSoftEmuAutoZOffsetStep
	ldr r0, [r5]
	mov r7, #0
	ldrh r0, [r0]
	cmp r0, #0
	bls _020BA420
	mov r8, #0x28
_020BA3FC:
	mla r0, r7, r8, r4
	bl DrawNode_
	ldr r1, [r5]
	add r0, r7, #1
	mov r0, r0, lsl #0x10
	ldrh r1, [r1]
	mov r7, r0, lsr #0x10
	cmp r1, r0, lsr #16
	bhi _020BA3FC
_020BA420:
	mov r0, #0
	bl NNSi_G2dSetOamSoftEmuAutoZOffsetFlag
	mov r0, r6
	bl NNSi_G2dSetOamSoftEmuAutoZOffsetStep
	bl NNSi_G2dResetOamSoftEmuAutoZOffset
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020BA438:
	ldr r0, [r5]
	mov r6, #0
	ldrh r0, [r0]
	cmp r0, #0
	ldmlsia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	mov r7, #0x28
_020BA450:
	mla r0, r6, r7, r4
	bl DrawNode_
	ldr r1, [r5]
	add r0, r6, #1
	mov r0, r0, lsl #0x10
	ldrh r1, [r1]
	mov r6, r0, lsr #0x10
	cmp r1, r0, lsr #16
	bhi _020BA450
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_020BA478: .word _021DA068
_020BA47C: .word _021D94D8
_020BA480: .word _021D9FD8
	arm_func_end NNS_G2dDrawMultiCell
