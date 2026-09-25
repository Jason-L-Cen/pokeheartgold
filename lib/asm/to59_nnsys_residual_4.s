	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"
	.public AllocFromHead__ExpHeap
	.public AllocFromTail__ExpHeap
	.public RecycleRegion
	.extern GetRegionOfMBlock
	.extern InitMBlock
	.extern InsertMBlock
	.extern NNSi_FndInitHeapHead
	.extern RemoveMBlock

.text
	arm_func_start InitExpHeap
InitExpHeap: ; 0x020B4F14
	stmdb sp!, {r3, r4, lr}
	sub sp, sp, #0xc
	mov r3, r1
	mov r4, r0
	str r2, [sp]
	ldr r1, _020B4F80 ; =0x45585048
	add r2, r4, #0x38
	bl NNSi_FndInitHeapHead
	mov r0, #0
	strh r0, [r4, #0x34]
	bic r0, r0, #1
	strh r0, [r4, #0x36]
	ldr r1, [r4, #0x18]
	add r0, sp, #4
	str r1, [sp, #4]
	ldr r2, [r4, #0x1c]
	ldr r1, _020B4F84 ; =0x00004652
	str r2, [sp, #8]
	bl InitMBlock
	str r0, [r4, #0x24]
	str r0, [r4, #0x28]
	mov r1, #0
	str r1, [r4, #0x2c]
	mov r0, r4
	str r1, [r4, #0x30]
	add sp, sp, #0xc
	ldmia sp!, {r3, r4, pc}
	.align 2, 0
_020B4F80: .word 0x45585048
_020B4F84: .word 0x00004652
	arm_func_end InitExpHeap

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

	arm_func_start AllocFromHead__ExpHeap
AllocFromHead__ExpHeap: ; 0x020B5108
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, lr}
	ldrh r4, [r0, #0x36]
	mov r3, r1
	ldr ip, [r0, #0x24]
	and r1, r4, #1
	mov r1, r1, lsl #0x10
	movs r1, r1, lsr #0x10
	mov r1, #0
	moveq r6, #1
	movne r6, #0
	mov r4, r1
	cmp ip, #0
	sub lr, r1, #1
	beq _020B5198
	sub r2, r2, #1
	mvn r5, r2
_020B5148:
	add r8, ip, #0x10
	add r7, r2, r8
	and sb, r5, r7
	sub r7, sb, r8
	ldr r8, [ip, #4]
	add r7, r3, r7
	cmp r8, r7
	blo _020B518C
	cmp lr, r8
	bls _020B518C
	mov r1, ip
	mov lr, r8
	mov r4, sb
	cmp r6, #0
	bne _020B5198
	cmp r8, r3
	beq _020B5198
_020B518C:
	ldr ip, [ip, #0xc]
	cmp ip, #0
	bne _020B5148
_020B5198:
	cmp r1, #0
	moveq r0, #0
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	mov r5, #0
	mov r2, r4
	add r0, r0, #0x24
	str r5, [sp]
	bl AllocUsedBlockFromFreeBlock
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	arm_func_end AllocFromHead__ExpHeap

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

	arm_func_start RecycleRegion
RecycleRegion: ; 0x020B526C
	stmdb sp!, {r4, r5, r6, lr}
	sub sp, sp, #8
	mov r5, r1
	ldr r2, [r5]
	ldr r1, [r5, #4]
	mov r6, r0
	str r2, [sp]
	str r1, [sp, #4]
	ldr r1, [r6]
	mov r4, #0
	cmp r1, #0
	beq _020B52E0
	ldr r0, [r5]
_020B52A0:
	cmp r1, r0
	movlo r4, r1
	blo _020B52D4
	ldr r0, [r5, #4]
	cmp r1, r0
	bne _020B52E0
	ldr r2, [r1, #4]
	add r0, r1, #0x10
	add r2, r2, r0
	mov r0, r6
	str r2, [sp, #4]
	bl RemoveMBlock
	b _020B52E0
_020B52D4:
	ldr r1, [r1, #0xc]
	cmp r1, #0
	bne _020B52A0
_020B52E0:
	cmp r4, #0
	beq _020B5314
	ldr r2, [r4, #4]
	add r1, r4, #0x10
	ldr r0, [r5]
	add r1, r2, r1
	cmp r1, r0
	bne _020B5314
	mov r0, r6
	mov r1, r4
	str r4, [sp]
	bl RemoveMBlock
	mov r4, r0
_020B5314:
	ldr r1, [sp, #4]
	ldr r0, [sp]
	sub r0, r1, r0
	cmp r0, #0x10
	addlo sp, sp, #8
	movlo r0, #0
	ldmloia sp!, {r4, r5, r6, pc}
	ldr r1, _020B5358 ; =0x00004652
	add r0, sp, #0
	bl InitMBlock
	mov r1, r0
	mov r0, r6
	mov r2, r4
	bl InsertMBlock
	mov r0, #1
	add sp, sp, #8
	ldmia sp!, {r4, r5, r6, pc}
	.align 2, 0
_020B5358: .word 0x00004652
	arm_func_end RecycleRegion

	arm_func_start NNS_FndCreateExpHeapEx
NNS_FndCreateExpHeapEx: ; 0x020B535C
	stmdb sp!, {r3, lr}
	add r1, r1, r0
	add r0, r0, #3
	bic r1, r1, #3
	bic r0, r0, #3
	cmp r0, r1
	bhi _020B5384
	sub r3, r1, r0
	cmp r3, #0x4c
	bhs _020B538C
_020B5384:
	mov r0, #0
	ldmia sp!, {r3, pc}
_020B538C:
	bl InitExpHeap
	ldmia sp!, {r3, pc}
	arm_func_end NNS_FndCreateExpHeapEx
