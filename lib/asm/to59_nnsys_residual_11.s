	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"
	.public NNSi_GfdAddNewFreeBlock
	.public NNSi_GfdInitLnkVramBlockPool

.text
	arm_func_start NNSi_GfdInitLnkVramBlockPool
NNSi_GfdInitLnkVramBlockPool: ; 0x020B64A4
	stmdb sp!, {r4, lr}
	subs lr, r1, #1
	mov r2, #0
	beq _020B64DC
	mov r4, r0
_020B64B8:
	add r3, r2, #1
	add r2, r0, r2, lsl #4
	add ip, r0, r3, lsl #4
	str ip, [r2, #0xc]
	str r4, [r2, #0x18]
	mov r2, r3
	cmp r3, lr
	add r4, r4, #0x10
	blo _020B64B8
_020B64DC:
	mov r2, #0
	str r2, [r0, #8]
	add r1, r0, r1, lsl #4
	str r2, [r1, #-4]
	ldmia sp!, {r4, pc}
	arm_func_end NNSi_GfdInitLnkVramBlockPool

	arm_func_start NNSi_GfdAddNewFreeBlock
NNSi_GfdAddNewFreeBlock: ; 0x020B64F0
	stmdb sp!, {r3, lr}
	ldr lr, [r1]
	cmp lr, #0
	ldrne ip, [lr, #0xc]
	strne ip, [r1]
	cmp lr, #0
	beq _020B6544
	stmia lr, {r2, r3}
	mov r1, #0
	str r1, [lr, #8]
	str r1, [lr, #0xc]
	ldr r1, [r0]
	cmp r1, #0
	strne lr, [r1, #8]
	ldr r2, [r0]
	mov r1, #0
	str r2, [lr, #0xc]
	str r1, [lr, #8]
	str lr, [r0]
	mov r0, #1
	ldmia sp!, {r3, pc}
_020B6544:
	mov r0, #0
	ldmia sp!, {r3, pc}
	arm_func_end NNSi_GfdAddNewFreeBlock
