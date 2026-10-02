	.extern GetRegionOfMBlock
	.extern InitMBlock
	.extern InsertMBlock
	.extern NNSi_FndInitHeapHead
	.extern RemoveMBlock
	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"
	.public AllocUsedBlockFromFreeBlock

.text
	arm_func_start AllocUsedBlockFromFreeBlock
AllocUsedBlockFromFreeBlock: ; 0x020B4F88
	stmdb sp!, {r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0x18
	mov r7, r0
	add r0, sp, #0x10
	mov r8, r1
	mov r6, r2
	mov r4, r3
	bl GetRegionOfMBlock
	ldr r3, [sp, #0x14]
	sub r5, r6, #0x10
	add r2, r4, r6
	mov r0, r7
	mov r1, r8
	str r5, [sp, #0x14]
	str r3, [sp, #0xc]
	str r2, [sp, #8]
	bl RemoveMBlock
	ldr r2, [sp, #0x10]
	ldr r1, [sp, #0x14]
	mov r4, r0
	sub r0, r1, r2
	cmp r0, #0x14
	strlo r2, [sp, #0x14]
	blo _020B5008
	ldr r1, _020B5100 ; =0x00004652
	add r0, sp, #0x10
	bl InitMBlock
	mov r1, r0
	mov r0, r7
	mov r2, r4
	bl InsertMBlock
	mov r4, r0
_020B5008:
	ldr r1, [sp, #0xc]
	ldr r0, [sp, #8]
	sub r0, r1, r0
	cmp r0, #0x14
	strlo r1, [sp, #8]
	blo _020B503C
	ldr r1, _020B5100 ; =0x00004652
	add r0, sp, #8
	bl InitMBlock
	mov r1, r0
	mov r0, r7
	mov r2, r4
	bl InsertMBlock
_020B503C:
	ldr r0, [r7, #-4]
	ldr r1, [sp, #0x14]
	ldr r2, [sp, #8]
	and r0, r0, #0xff
	tst r0, #1
	sub r2, r2, r1
	beq _020B5060
	mov r0, #0
	bl MIi_CpuClear32
_020B5060:
	ldr r2, [sp, #8]
	ldr r1, _020B5104 ; =0x00005544
	add r0, sp, #0
	str r5, [sp]
	str r2, [sp, #4]
	bl InitMBlock
	mov r1, r0
	ldrh r3, [r1, #2]
	ldrh r2, [sp, #0x30]
	add r0, r7, #8
	bic r3, r3, #0x8000
	strh r3, [r1, #2]
	ldrh r3, [r1, #2]
	mov r2, r2, lsl #0x1f
	orr r2, r3, r2, lsr #16
	strh r2, [r1, #2]
	ldrh r2, [r1, #2]
	ldr r3, [sp, #0x14]
	bic r2, r2, #0x7f00
	strh r2, [r1, #2]
	sub r2, r1, r3
	mov r2, r2, lsl #0x10
	mov r2, r2, lsr #0x10
	ldrh r3, [r1, #2]
	mov r2, r2, lsl #0x19
	orr r2, r3, r2, lsr #17
	strh r2, [r1, #2]
	ldrh r2, [r1, #2]
	ldrh r3, [r7, #0x10]
	bic r2, r2, #0xff
	strh r2, [r1, #2]
	ldrh r2, [r1, #2]
	and r3, r3, #0xff
	orr r2, r2, r3
	strh r2, [r1, #2]
	ldr r2, [r7, #0xc]
	bl InsertMBlock
	mov r0, r6
	add sp, sp, #0x18
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	.align 2, 0
_020B5100: .word 0x00004652
_020B5104: .word 0x00005544
	arm_func_end AllocUsedBlockFromFreeBlock
