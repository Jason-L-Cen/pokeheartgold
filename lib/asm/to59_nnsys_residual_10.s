	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"
	.public TryToMergeBlockRegion_

.text
	arm_func_start TryToMergeBlockRegion_
TryToMergeBlockRegion_: ; 0x020B63B4
	stmdb sp!, {r4, r5, r6, r7, r8, lr}
	ldr r3, [r0]
	mov lr, #0
	cmp r3, #0
	beq _020B6490
	mov r8, lr
	mov r4, lr
_020B63D0:
	ldr r6, [r3]
	ldr r5, [r2, #4]
	ldr ip, [r3, #0xc]
	cmp r6, r5
	bne _020B642C
	ldr r5, [r3, #4]
	orr lr, lr, #1
	add r5, r6, r5
	str r5, [r2, #4]
	ldr r6, [r3, #8]
	ldr r5, [r3, #0xc]
	cmp r6, #0
	strne r5, [r6, #0xc]
	streq r5, [r0]
	cmp r5, #0
	strne r6, [r5, #8]
	ldr r5, [r1]
	cmp r5, #0
	strne r3, [r5, #8]
	ldr r5, [r1]
	str r5, [r3, #0xc]
	str r8, [r3, #8]
	str r3, [r1]
_020B642C:
	ldr r7, [r3]
	ldr r5, [r3, #4]
	ldr r6, [r2]
	add r5, r7, r5
	cmp r6, r5
	bne _020B6484
	str r7, [r2]
	ldr r6, [r3, #8]
	ldr r5, [r3, #0xc]
	cmp r6, #0
	strne r5, [r6, #0xc]
	streq r5, [r0]
	cmp r5, #0
	strne r6, [r5, #8]
	ldr r5, [r1]
	orr lr, lr, #1
	cmp r5, #0
	strne r3, [r5, #8]
	ldr r5, [r1]
	str r5, [r3, #0xc]
	str r4, [r3, #8]
	str r3, [r1]
_020B6484:
	mov r3, ip
	cmp ip, #0
	bne _020B63D0
_020B6490:
	mov r0, lr
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	arm_func_end TryToMergeBlockRegion_
