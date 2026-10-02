	.extern NNS_G3dGeBufferOP_N
	.extern NNS_G3dGetResDataByName
	.extern NNS_G3dGlb
	.extern _02109474
	.extern _02110A74
	.extern _02110A80
	.extern _02110A8C
	.extern _02110B9C
	.extern _021DA700
	.extern _021DA704
	.extern addLink_
	.extern updateHintVec_
	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"
	.public bindMdlTex_Internal_

.text
	arm_func_start bindMdlTex_Internal_
bindMdlTex_Internal_: ; 0x020BE5A8
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	mov sb, r1
	mov r8, r3
	ldr r1, [r8]
	mov sl, r0
	and r0, r1, #0x1c000000
	ldrh r3, [sb]
	cmp r0, #0x14000000
	mov r5, #0
	add r0, sl, r3
	str r0, [sp]
	ldrne r0, [r2, #8]
	ldreq r0, [r2, #0x18]
	mov r0, r0, lsl #0x10
	mov r0, r0, lsr #0x10
	bic fp, r0, #0xe0000000
	ldrb r0, [sb, #2]
	cmp r0, #0
	bls _020BE6C4
	add r4, sl, #4
_020BE5F8:
	ldr r0, [sp]
	cmp sl, #0
	ldrb r2, [r0, r5]
	beq _020BE648
	cmp r4, #0
	beq _020BE634
	ldrb r0, [sl, #5]
	cmp r2, r0
	bhs _020BE634
	ldrh r0, [sl, #0xa]
	add r1, r4, r0
	add r1, r1, #4
	ldrh r0, [r4, r0]
	mla r1, r0, r2, r1
	b _020BE638
_020BE634:
	mov r1, #0
_020BE638:
	cmp r1, #0
	ldrne r0, [r1]
	addne r7, sl, r0
	bne _020BE64C
_020BE648:
	mov r7, #0
_020BE64C:
	ldr r0, [r8]
	ldr r1, [r7, #0x14]
	add r0, r0, fp
	orr r0, r1, r0
	str r0, [r7, #0x14]
	ldr r3, [r8, #4]
	ldr r0, _020BE6D4 ; =0x000007FF
	ldrh r1, [r7, #0x20]
	and r2, r3, r0
	and r0, r3, r0, lsl #11
	mov r6, r0, lsr #0xb
	cmp r2, r1
	moveq r0, #0x1000
	beq _020BE690
	mov r0, r2, lsl #0xc
	mov r1, r1, lsl #0xc
	bl FX_Div
_020BE690:
	str r0, [r7, #0x24]
	ldrh r1, [r7, #0x22]
	cmp r6, r1
	moveq r0, #0x1000
	beq _020BE6B0
	mov r0, r6, lsl #0xc
	mov r1, r1, lsl #0xc
	bl FX_Div
_020BE6B0:
	str r0, [r7, #0x28]
	ldrb r0, [sb, #2]
	add r5, r5, #1
	cmp r5, r0
	blo _020BE5F8
_020BE6C4:
	ldrb r0, [sb, #3]
	orr r0, r0, #1
	strb r0, [sb, #3]
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_020BE6D4: .word 0x000007FF
	arm_func_end bindMdlTex_Internal_
