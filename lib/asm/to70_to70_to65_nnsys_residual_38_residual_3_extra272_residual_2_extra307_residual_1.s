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
	.public bindMdlPltt_Internal_

.text
	arm_func_start bindMdlPltt_Internal_
bindMdlPltt_Internal_: ; 0x020BE9CC
	stmdb sp!, {r4, r5, r6, r7, r8, lr}
	ldr r2, [r2, #0x2c]
	ldrh r5, [r1]
	mov r2, r2, lsl #0x10
	mov r2, r2, lsr #0x10
	ldrh r4, [r3, #2]
	bic r2, r2, #0xe0000000
	mov r2, r2, lsl #0x10
	tst r4, #1
	add ip, r0, r5
	mov r5, r2, lsr #0x10
	ldrh r4, [r3]
	bne _020BEA10
	mov r2, r4, lsl #0xf
	mov r3, r5, lsl #0xf
	mov r4, r2, lsr #0x10
	mov r5, r3, lsr #0x10
_020BEA10:
	ldrb r2, [r1, #2]
	mov r3, #0
	cmp r2, #0
	bls _020BEA9C
	add r2, r4, r5
	mov lr, r2, lsl #0x10
	add r2, r0, #4
	mov r4, lr, lsr #0x10
	mov r6, r3
	mov r5, r3
_020BEA38:
	cmp r0, #0
	ldrb r8, [ip, r3]
	beq _020BEA84
	cmp r2, #0
	beq _020BEA70
	ldrb r7, [r0, #5]
	cmp r8, r7
	bhs _020BEA70
	ldrh r7, [r0, #0xa]
	ldrh lr, [r2, r7]
	add r7, r2, r7
	add r7, r7, #4
	mla r7, lr, r8, r7
	b _020BEA74
_020BEA70:
	mov r7, r6
_020BEA74:
	cmp r7, #0
	ldrne lr, [r7]
	addne r7, r0, lr
	bne _020BEA88
_020BEA84:
	mov r7, r5
_020BEA88:
	strh r4, [r7, #0x1c]
	ldrb lr, [r1, #2]
	add r3, r3, #1
	cmp r3, lr
	blo _020BEA38
_020BEA9C:
	ldrb r0, [r1, #3]
	orr r0, r0, #1
	strb r0, [r1, #3]
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	arm_func_end bindMdlPltt_Internal_
