	.extern GetRegionOfMBlock
	.extern InitMBlock
	.extern InsertMBlock
	.extern RecycleRegion
	.extern RemoveMBlock
	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"

.text
	arm_func_start NNS_FndResizeForMBlockExpHeap
NNS_FndResizeForMBlockExpHeap: ; 0x020B53D0
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, lr}
	sub sp, sp, #0x10
	mov r4, r1
	mov sb, r2
	sub r8, r4, #0x10
	add r1, sb, #3
	ldr r7, [r8, #4]
	bic sb, r1, #3
	cmp sb, r7
	mov r5, r0
	addeq sp, sp, #0x10
	moveq r0, sb
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	bls _020B54F0
	ldr r6, [r5, #0x24]
	add r0, r8, #0x10
	cmp r6, #0
	add r0, r7, r0
	beq _020B542C
_020B541C:
	cmp r6, r0
	ldrne r6, [r6, #0xc]
	cmpne r6, #0
	bne _020B541C
_020B542C:
	cmp r6, #0
	beq _020B5448
	ldr r0, [r6, #4]
	add r1, r7, #0x10
	add r0, r1, r0
	cmp sb, r0
	bls _020B5454
_020B5448:
	add sp, sp, #0x10
	mov r0, #0
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
_020B5454:
	add r0, sp, #8
	mov r1, r6
	bl GetRegionOfMBlock
	mov r1, r6
	add r0, r5, #0x24
	bl RemoveMBlock
	ldr r2, [sp, #0xc]
	add r3, sb, r4
	ldr r6, [sp, #8]
	sub r1, r2, r3
	str r3, [sp, #8]
	cmp r1, #0x10
	strlo r2, [sp, #8]
	mov r7, r0
	ldr r0, [sp, #8]
	sub r0, r0, r4
	str r0, [r8, #4]
	ldr r1, [sp, #0xc]
	ldr r0, [sp, #8]
	sub r0, r1, r0
	cmp r0, #0x10
	blo _020B54C8
	ldr r1, _020B552C ; =0x00004652
	add r0, sp, #8
	bl InitMBlock
	mov r1, r0
	mov r2, r7
	add r0, r5, #0x24
	bl InsertMBlock
_020B54C8:
	ldr r0, [r5, #0x20]
	ldr r1, [sp, #8]
	and r0, r0, #0xff
	tst r0, #1
	sub r2, r1, r6
	beq _020B5520
	mov r1, r6
	mov r0, #0
	bl MIi_CpuClear32
	b _020B5520
_020B54F0:
	add r0, sb, r4
	str r0, [sp]
	ldr r1, [r8, #4]
	add r0, r8, #0x10
	add r0, r1, r0
	str r0, [sp, #4]
	add r1, sp, #0
	add r0, r5, #0x24
	str sb, [r8, #4]
	bl RecycleRegion
	cmp r0, #0
	streq r7, [r8, #4]
_020B5520:
	ldr r0, [r8, #4]
	add sp, sp, #0x10
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	.align 2, 0
_020B552C: .word 0x00004652
	arm_func_end NNS_FndResizeForMBlockExpHeap
