	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"
	.public SetMCDataToMCInstanceImpl_
	.extern NNS_G2dGetAnimSequenceByIdx
	.extern NNS_G2dSetCellAnimationSequence
	.extern NNS_G2dTickCellAnimation
	.extern NNSi_G2dIsAnimCtrlLoopAnim
	.extern NNSi_G2dSrtcSetTrans

.text
	arm_func_start SetMCDataToMCInstanceImpl_
SetMCDataToMCInstanceImpl_: ; 0x020B8C1C
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	mov sl, r1
	str r0, [sp]
	str sl, [r0]
	ldr r0, [r0, #8]
	mov sb, r2
	cmp r0, #1
	bne _020B8D84
	ldrh r1, [sl, #2]
	ldr r0, [sp]
	mov r4, #0
	ldr r5, [r0, #0xc]
	cmp r1, #0
	bls _020B8C7C
	mov r3, r4
	mov r0, #0x5c
_020B8C5C:
	mla r1, r4, r0, r5
	str r3, [r1, #0x58]
	ldrh r2, [sl, #2]
	add r1, r4, #1
	mov r1, r1, lsl #0x10
	cmp r2, r1, lsr #16
	mov r4, r1, lsr #0x10
	bhi _020B8C5C
_020B8C7C:
	ldrh r0, [sl]
	mov r8, #0
	cmp r0, #0
	ldmlsia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020B8C8C:
	ldr r0, [sl, #4]
	mov r1, #0x5c
	add r6, r0, r8, lsl #3
	ldrh r0, [r6, #6]
	and r0, r0, #0xff00
	mov r0, r0, lsl #8
	mov r0, r0, lsr #0x10
	mla r7, r0, r1, r5
	ldr r0, [r7, #0x58]
	cmp r0, #0
	bne _020B8D68
	ldr r0, [sp]
	ldrh r1, [r6]
	ldr r0, [r0, #4]
	bl NNS_G2dGetAnimSequenceByIdx
	mov r4, r0
	mov r0, r7
	mov r1, r4
	bl NNS_G2dSetCellAnimationSequence
	mov r0, #1
	str r0, [r7, #0xc]
	ldrh r0, [r6, #6]
	and r0, r0, #0xf
	cmp r0, #1
	bne _020B8D60
	ldrh r3, [r4]
	mov r6, #0
	mov r1, r6
	cmp r3, #0
	bls _020B8D20
	ldr r2, [r4, #0xc]
_020B8D08:
	add r0, r2, r1, lsl #3
	ldrh r0, [r0, #4]
	add r1, r1, #1
	cmp r1, r3
	add r6, r6, r0
	blo _020B8D08
_020B8D20:
	mov r0, r7
	bl NNSi_G2dIsAnimCtrlLoopAnim
	cmp r0, #0
	beq _020B8D4C
	mov r0, sb
	mov r1, r6
	bl _u32_div_f
	mov r0, r7
	mov r1, r1, lsl #0xc
	bl NNS_G2dTickCellAnimation
	b _020B8D60
_020B8D4C:
	cmp sb, r6
	movlo r6, sb
	mov r0, r7
	mov r1, r6, lsl #0xc
	bl NNS_G2dTickCellAnimation
_020B8D60:
	mov r0, #1
	str r0, [r7, #0x58]
_020B8D68:
	ldrh r1, [sl]
	add r0, r8, #1
	mov r0, r0, lsl #0x10
	cmp r1, r0, lsr #16
	mov r8, r0, lsr #0x10
	bhi _020B8C8C
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020B8D84:
	ldr r0, [sp]
	ldrh r1, [sl]
	ldr fp, [r0, #0xc]
	mov r7, #0
	cmp r1, #0
	ldmlsia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020B8D9C:
	mov r0, #0x28
	mul r2, r7, r0
	ldr r3, [sl, #4]
	mov r0, r7, lsl #3
	ldrh r1, [r3, r0]
	ldr r0, [sp]
	ldr r6, [fp, r2]
	ldr r0, [r0, #4]
	add r5, r3, r7, lsl #3
	bl NNS_G2dGetAnimSequenceByIdx
	mov r4, r0
	mov r0, r6
	mov r1, r4
	bl NNS_G2dSetCellAnimationSequence
	mov r0, #1
	str r0, [r6, #0xc]
	ldrh r0, [r5, #6]
	and r0, r0, #0xf
	cmp r0, #1
	bne _020B8E5C
	ldrh r3, [r4]
	mov r8, #0
	mov r1, r8
	cmp r3, #0
	bls _020B8E1C
	ldr r2, [r4, #0xc]
_020B8E04:
	add r0, r2, r1, lsl #3
	ldrh r0, [r0, #4]
	add r1, r1, #1
	cmp r1, r3
	add r8, r8, r0
	blo _020B8E04
_020B8E1C:
	mov r0, r6
	bl NNSi_G2dIsAnimCtrlLoopAnim
	cmp r0, #0
	beq _020B8E48
	mov r0, sb
	mov r1, r8
	bl _u32_div_f
	mov r0, r6
	mov r1, r1, lsl #0xc
	bl NNS_G2dTickCellAnimation
	b _020B8E5C
_020B8E48:
	cmp sb, r8
	movlo r8, sb
	mov r0, r6
	mov r1, r8, lsl #0xc
	bl NNS_G2dTickCellAnimation
_020B8E5C:
	mov r0, #0x28
	mla r3, r7, r0, fp
	mov r0, #1
	str r0, [r3, #8]
	ldrsh r1, [r5, #2]
	ldrsh r2, [r5, #4]
	add r0, r3, #0xc
	bl NNSi_G2dSrtcSetTrans
	add r0, r7, #1
	mov r0, r0, lsl #0x10
	ldrh r1, [sl]
	mov r7, r0, lsr #0x10
	cmp r1, r0, lsr #16
	bhi _020B8D9C
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	arm_func_end SetMCDataToMCInstanceImpl_
