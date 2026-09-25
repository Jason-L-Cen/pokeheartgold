	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"
	.public NNS_GfdFreeLnkTexVram
	.public NNS_GfdResetLnkTexVramState
	.extern NNSi_GfdAddNewFreeBlock
	.extern NNSi_GfdFreeLnkVram
	.extern NNSi_GfdInitLnkVramBlockPool
	.extern NNSi_GfdInitLnkVramMan
	.extern NNSi_GfdMergeAllFreeBlocks
	.extern _021092A0
	.extern _021D84E4
	.extern _021D84E8
	.extern _021D84EC

.text
	arm_func_start NNS_GfdFreeLnkTexVram
NNS_GfdFreeLnkTexVram: ; 0x020B68A4
	stmdb sp!, {r3, lr}
	ldr r1, _020B6908 ; =0x7FFF0000
	mov r2, r0, lsl #0x10
	and r1, r0, r1
	mov r1, r1, lsr #0x10
	and r0, r0, #0x80000000
	mov r2, r2, lsr #0xd
	movs r3, r1, lsl #4
	mov r0, r0, lsr #0x1f
	beq _020B6900
	cmp r0, #0
	beq _020B68E4
	ldr r0, _020B690C ; =_021D84E8
	ldr r1, _020B6910 ; =_021D84EC
	bl NNSi_GfdFreeLnkVram
	b _020B68F0
_020B68E4:
	ldr r0, _020B6914 ; =_021D84E4
	ldr r1, _020B6910 ; =_021D84EC
	bl NNSi_GfdFreeLnkVram
_020B68F0:
	cmp r0, #0
	movne r0, #0
	moveq r0, #1
	ldmia sp!, {r3, pc}
_020B6900:
	mov r0, #2
	ldmia sp!, {r3, pc}
	.align 2, 0
_020B6908: .word 0x7FFF0000
_020B690C: .word _021D84E8
_020B6910: .word _021D84EC
_020B6914: .word _021D84E4
	arm_func_end NNS_GfdFreeLnkTexVram

	arm_func_start NNS_GfdResetLnkTexVramState
NNS_GfdResetLnkTexVramState: ; 0x020B6918
	stmdb sp!, {r4, lr}
	sub sp, sp, #0x30
	ldr lr, _020B6B00 ; =_021092A0
	add r4, sp, #0
	ldmia lr!, {r0, r1, r2, r3}
	mov ip, r4
	stmia r4!, {r0, r1, r2, r3}
	ldmia lr!, {r0, r1, r2, r3}
	stmia r4!, {r0, r1, r2, r3}
	ldmia lr, {r0, r1, r2, r3}
	stmia r4, {r0, r1, r2, r3}
	ldr r0, _020B6B04 ; =_021D84E4
	mov r3, #0
	ldr r2, [r0, #0x10]
	ldr r1, [r0, #0xc]
	add r0, r2, r2, lsr #1
	mov r4, r2, lsr #1
	sub r1, r1, r0
_020B6960:
	cmp r3, #0
	cmpne r3, #2
	bne _020B69A0
	ldr lr, [ip]
	cmp lr, #0
	cmpne r2, #0
	beq _020B69A0
	ldr r0, [ip, #8]
	cmp lr, r2
	movhi lr, r2
	add r0, r0, lr
	str r0, [ip, #8]
	ldr r0, [ip]
	sub r2, r2, lr
	sub r0, r0, lr
	str r0, [ip]
_020B69A0:
	add r3, r3, #1
	cmp r3, #4
	add ip, ip, #0xc
	blo _020B6960
	ldr r0, [sp, #0xc]
	add r2, sp, #0
	sub r0, r0, r4
	str r0, [sp, #0xc]
	mov r3, #0
_020B69C4:
	ldr ip, [r2]
	cmp ip, #0
	cmpne r1, #0
	beq _020B69F8
	ldr r0, [r2, #4]
	cmp ip, r1
	movhi ip, r1
	add r0, r0, ip
	str r0, [r2, #4]
	ldr r0, [r2]
	sub r1, r1, ip
	sub r0, r0, ip
	str r0, [r2]
_020B69F8:
	add r3, r3, #1
	cmp r3, #4
	add r2, r2, #0xc
	blo _020B69C4
	ldr r0, _020B6B08 ; =_021D84E4
	bl NNSi_GfdInitLnkVramMan
	ldr r0, _020B6B0C ; =_021D84E8
	bl NNSi_GfdInitLnkVramMan
	ldr r0, _020B6B04 ; =_021D84E4
	ldr r1, [r0, #0x18]
	ldr r0, [r0, #0x14]
	mov r1, r1, lsr #4
	bl NNSi_GfdInitLnkVramBlockPool
	ldr r3, [sp, #8]
	ldr r1, _020B6B04 ; =_021D84E4
	cmp r3, #0
	str r0, [r1, #8]
	beq _020B6A50
	ldr r0, _020B6B0C ; =_021D84E8
	ldr r1, _020B6B10 ; =_021D84EC
	mov r2, #0
	bl NNSi_GfdAddNewFreeBlock
_020B6A50:
	ldr r3, [sp, #4]
	ldr r2, [sp, #8]
	cmp r3, #0
	beq _020B6A6C
	ldr r0, _020B6B08 ; =_021D84E4
	ldr r1, _020B6B10 ; =_021D84EC
	bl NNSi_GfdAddNewFreeBlock
_020B6A6C:
	ldr r3, [sp, #0x20]
	cmp r3, #0
	beq _020B6A88
	ldr r0, _020B6B0C ; =_021D84E8
	ldr r1, _020B6B10 ; =_021D84EC
	mov r2, #0x40000
	bl NNSi_GfdAddNewFreeBlock
_020B6A88:
	ldr r3, [sp, #0x1c]
	ldr r2, [sp, #0x20]
	cmp r3, #0
	beq _020B6AA8
	ldr r0, _020B6B08 ; =_021D84E4
	ldr r1, _020B6B10 ; =_021D84EC
	add r2, r2, #0x40000
	bl NNSi_GfdAddNewFreeBlock
_020B6AA8:
	ldr r3, [sp, #0x28]
	cmp r3, #0
	beq _020B6AC4
	ldr r0, _020B6B08 ; =_021D84E4
	ldr r1, _020B6B10 ; =_021D84EC
	mov r2, #0x60000
	bl NNSi_GfdAddNewFreeBlock
_020B6AC4:
	ldr r3, [sp, #0x10]
	cmp r3, #0
	beq _020B6AE0
	ldr r0, _020B6B08 ; =_021D84E4
	ldr r1, _020B6B10 ; =_021D84EC
	add r2, r4, #0x20000
	bl NNSi_GfdAddNewFreeBlock
_020B6AE0:
	ldr r0, _020B6B08 ; =_021D84E4
	ldr r1, _020B6B10 ; =_021D84EC
	bl NNSi_GfdMergeAllFreeBlocks
	ldr r0, _020B6B0C ; =_021D84E8
	ldr r1, _020B6B10 ; =_021D84EC
	bl NNSi_GfdMergeAllFreeBlocks
	add sp, sp, #0x30
	ldmia sp!, {r4, pc}
	.align 2, 0
_020B6B00: .word _021092A0
_020B6B04: .word _021D84E4
_020B6B08: .word _021D84E4
_020B6B0C: .word _021D84E8
_020B6B10: .word _021D84EC
	arm_func_end NNS_GfdResetLnkTexVramState
