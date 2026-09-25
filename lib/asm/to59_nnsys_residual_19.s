	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"
	.public NNS_G2dTickAnimCtrl
	.extern SequenceEdgeHandle_

.text
	arm_func_start NNS_G2dTickAnimCtrl
NNS_G2dTickAnimCtrl: ; 0x020B75D8
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	mov sl, r0
	ldr r0, [sl, #0xc]
	mov sb, #0
	cmp r0, #1
	movne r0, sb
	ldmneia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldr r0, [sl, #0x14]
	smull r2, r1, r0, r1
	adds r0, r2, #0x800
	adc r1, r1, sb
	mov r0, r0, lsr #0xc
	orr r0, r0, r1, lsl #20
	bl abs
	mov r4, #1
	ldr r1, [sl, #0x10]
	mov fp, sb
	add r0, r1, r0
	str r0, [sl, #0x10]
	mov r8, r4
	mov r7, sb
	mov r5, sb
	mov r6, r4
	b _020B7754
_020B7638:
	ldr r0, [sl]
	ldr r1, [sl, #0x10]
	ldrh r0, [r0, #4]
	mov sb, r8
	sub r0, r1, r0, lsl #12
	str r0, [sl, #0x10]
	ldr r0, [sl, #0x14]
	cmp r0, #0
	movgt r1, r8
	ldr r0, [sl, #8]
	movle r1, r7
	teq r1, r0
	ldr r0, [sl]
	addne r0, r0, #8
	subeq r0, r0, #8
	str r0, [sl]
	ldr r1, [sl, #0x14]
	ldr r0, [sl]
	cmp r1, #0
	movgt r2, r6
	ldr r1, [sl, #8]
	movle r2, r5
	teq r2, r1
	ldr r1, [sl, #0x1c]
	ldr r2, [r1, #0xc]
	beq _020B76B8
	ldrh r1, [r1]
	add r1, r2, r1, lsl #3
	cmp r0, r1
	movhs r0, r4
	movlo r0, fp
	b _020B76D0
_020B76B8:
	ldrh r1, [r1, #2]
	add r1, r2, r1, lsl #3
	sub r1, r1, #8
	cmp r0, r1
	movls r0, #1
	movhi r0, #0
_020B76D0:
	cmp r0, #0
	beq _020B76E0
	mov r0, sl
	bl SequenceEdgeHandle_
_020B76E0:
	ldr r1, [sl]
	ldrh r0, [r1, #4]
	cmp r0, #0
	strne r1, [sl, #4]
	ldr r0, [sl, #0x20]
	cmp r0, #0
	beq _020B7754
	ldr r0, [sl, #0x1c]
	ldr r2, [sl, #0x20]
	ldr r0, [r0, #0xc]
	cmp r2, #2
	ldr r1, [sl]
	sub r0, r1, r0
	mov r0, r0, lsl #0xd
	mov r1, r0, lsr #0x10
	beq _020B772C
	cmp r2, #3
	beq _020B7748
	b _020B7754
_020B772C:
	ldrh r0, [sl, #0x2c]
	cmp r1, r0
	bne _020B7754
	ldr r0, [sl, #0x24]
	ldr r2, [sl, #0x28]
	blx r2
	b _020B7754
_020B7748:
	ldr r0, [sl, #0x24]
	ldr r2, [sl, #0x28]
	blx r2
_020B7754:
	ldr r0, [sl, #0xc]
	cmp r0, #0
	beq _020B7778
	ldr r0, [sl]
	ldr r1, [sl, #0x10]
	ldrh r0, [r0, #4]
	cmp r1, r0, lsl #12
	movge r0, #1
	bge _020B777C
_020B7778:
	mov r0, #0
_020B777C:
	cmp r0, #0
	bne _020B7638
	mov r0, sb
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	arm_func_end NNS_G2dTickAnimCtrl
