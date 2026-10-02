	.extern GetRegionOfMBlock
	.extern InitMBlock
	.extern InsertMBlock
	.extern NNSi_FndInitHeapHead
	.extern RemoveMBlock
	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"
	.extern AllocUsedBlockFromFreeBlock

.text
	arm_func_start AllocFromTail__ExpHeap
AllocFromTail__ExpHeap: ; 0x020B51BC
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, lr}
	ldrh r4, [r0, #0x36]
	mov r3, r1
	ldr ip, [r0, #0x28]
	and r1, r4, #1
	mov r1, r1, lsl #0x10
	movs r1, r1, lsr #0x10
	mov r1, #0
	moveq r5, #1
	movne r5, #0
	mov r4, r1
	cmp ip, #0
	sub lr, r1, #1
	beq _020B5248
	sub r2, r2, #1
	mvn r2, r2
_020B51FC:
	ldr r8, [ip, #4]
	add sb, ip, #0x10
	add r6, r8, sb
	sub r6, r6, r3
	and r7, r2, r6
	subs r6, r7, sb
	bmi _020B523C
	cmp lr, r8
	bls _020B523C
	mov r1, ip
	mov lr, r8
	mov r4, r7
	cmp r5, #0
	bne _020B5248
	cmp r8, r3
	beq _020B5248
_020B523C:
	ldr ip, [ip, #8]
	cmp ip, #0
	bne _020B51FC
_020B5248:
	cmp r1, #0
	moveq r0, #0
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	mov r5, #1
	mov r2, r4
	add r0, r0, #0x24
	str r5, [sp]
	bl AllocUsedBlockFromFreeBlock
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	arm_func_end AllocFromTail__ExpHeap
