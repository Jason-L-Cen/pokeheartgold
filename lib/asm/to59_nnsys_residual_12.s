	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"
	.public NNSi_GfdAllocLnkVramAligned
	.public NNSi_GfdFreeLnkVram
	.public NNSi_GfdMergeAllFreeBlocks
	.extern TryToMergeBlockRegion_

.text
	arm_func_start NNSi_GfdAllocLnkVramAligned
NNSi_GfdAllocLnkVramAligned: ; 0x020B6560
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	ldr r7, [r0]
	ldr sb, [sp, #0x28]
	cmp r7, #0
	mov r6, #0
	beq _020B65C8
	sub ip, sb, #1
	mvn r8, ip
	mov fp, r6
_020B6584:
	cmp sb, #1
	ldrls lr, [r7]
	movls r5, fp
	movls r4, r3
	bls _020B65AC
	ldr r5, [r7]
	add r4, r5, ip
	and lr, r8, r4
	sub r5, lr, r5
	add r4, r3, r5
_020B65AC:
	ldr sl, [r7, #4]
	cmp sl, r4
	movhs r6, r7
	bhs _020B65C8
	ldr r7, [r7, #0xc]
	cmp r7, #0
	bne _020B6584
_020B65C8:
	cmp r6, #0
	beq _020B6694
	cmp r5, #0
	beq _020B6628
	ldr r8, [r1]
	cmp r8, #0
	ldrne r3, [r8, #0xc]
	strne r3, [r1]
	cmp r8, #0
	beq _020B6694
	ldr r7, [r6]
	mov r3, #0
	str r7, [r8]
	str r5, [r8, #4]
	str r3, [r8, #8]
	str r3, [r8, #0xc]
	ldr r3, [r0]
	cmp r3, #0
	strne r8, [r3, #8]
	ldr r5, [r0]
	mov r3, #0
	str r5, [r8, #0xc]
	str r3, [r8, #8]
	str r8, [r0]
_020B6628:
	ldr r3, [r6, #4]
	sub r3, r3, r4
	str r3, [r6, #4]
	ldr r3, [r6]
	add r3, r3, r4
	str r3, [r6]
	ldr r3, [r6, #4]
	cmp r3, #0
	bne _020B6688
	ldr r4, [r6, #8]
	ldr r3, [r6, #0xc]
	cmp r4, #0
	strne r3, [r4, #0xc]
	streq r3, [r0]
	cmp r3, #0
	strne r4, [r3, #8]
	ldr r0, [r1]
	cmp r0, #0
	strne r6, [r0, #8]
	ldr r3, [r1]
	mov r0, #0
	str r3, [r6, #0xc]
	str r0, [r6, #8]
	str r6, [r1]
_020B6688:
	str lr, [r2]
	mov r0, #1
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020B6694:
	mov r0, #0
	str r0, [r2]
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	arm_func_end NNSi_GfdAllocLnkVramAligned

	arm_func_start NNSi_GfdMergeAllFreeBlocks
NNSi_GfdMergeAllFreeBlocks: ; 0x020B66A0
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	sub sp, sp, #8
	mov r7, r0
	ldr r5, [r7]
	mov r6, r1
	cmp r5, #0
	addeq sp, sp, #8
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	add r4, sp, #0
_020B66C4:
	ldr r1, [r5]
	mov r0, r7
	str r1, [sp]
	ldr r3, [r5]
	ldr r2, [r5, #4]
	mov r1, r6
	add r3, r3, r2
	mov r2, r4
	str r3, [sp, #4]
	bl TryToMergeBlockRegion_
	cmp r0, #0
	ldreq r5, [r5, #0xc]
	beq _020B6714
	ldr r0, [sp]
	str r0, [r5]
	ldr r1, [sp, #4]
	ldr r0, [sp]
	sub r0, r1, r0
	str r0, [r5, #4]
	ldr r5, [r7]
_020B6714:
	cmp r5, #0
	bne _020B66C4
	add sp, sp, #8
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	arm_func_end NNSi_GfdMergeAllFreeBlocks

	arm_func_start NNSi_GfdFreeLnkVram
NNSi_GfdFreeLnkVram: ; 0x020B6724
	stmdb sp!, {r3, r4, r5, lr}
	sub sp, sp, #8
	add r3, r2, r3
	str r2, [sp]
	add r2, sp, #0
	mov r5, r1
	str r3, [sp, #4]
	mov r4, r0
	bl TryToMergeBlockRegion_
	ldr r3, [r5]
	cmp r3, #0
	ldrne r0, [r3, #0xc]
	strne r0, [r5]
	cmp r3, #0
	addeq sp, sp, #8
	mov r0, #0
	ldmeqia sp!, {r3, r4, r5, pc}
	ldr r1, [sp]
	str r1, [r3]
	ldr r2, [sp, #4]
	ldr r1, [sp]
	sub r1, r2, r1
	str r1, [r3, #4]
	str r0, [r3, #8]
	str r0, [r3, #0xc]
	ldr r0, [r4]
	cmp r0, #0
	strne r3, [r0, #8]
	ldr r1, [r4]
	mov r0, #0
	str r1, [r3, #0xc]
	str r0, [r3, #8]
	str r3, [r4]
	mov r0, #1
	add sp, sp, #8
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end NNSi_GfdFreeLnkVram
