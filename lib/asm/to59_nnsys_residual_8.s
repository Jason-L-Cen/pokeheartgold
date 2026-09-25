	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"
	.public NNS_GfdAllocFrmPlttVram
	.extern _021D84C4

.text
	arm_func_start NNS_GfdAllocFrmPlttVram
NNS_GfdAllocFrmPlttVram: ; 0x020B5D64
	stmdb sp!, {r4, lr}
	cmp r0, #0
	moveq r0, #8
	addne r0, r0, #7
	ldr ip, _020B5E98 ; =0x0007FFF8
	bicne r0, r0, #7
	cmp r0, ip
	mov r3, #0
	movhs r0, #0
	ldmhsia sp!, {r4, pc}
	cmp r2, #1
	bne _020B5E10
	ldr r2, _020B5E9C ; =_021D84C4
	cmp r1, #0
	ldr lr, [r2]
	andeq r2, lr, #0xf
	rsbeq r2, r2, #0x10
	andeq r4, r2, #0xf
	beq _020B5DBC
	and r2, lr, #7
	rsb r2, r2, #8
	and r4, r2, #7
_020B5DBC:
	ldr r2, _020B5E9C ; =_021D84C4
	add ip, r0, r4
	ldr r2, [r2, #4]
	sub r2, r2, lr
	cmp r2, ip
	blo _020B5E08
	cmp r1, #0
	add r1, lr, ip
	beq _020B5DEC
	cmp r1, #0x10000
	movhi lr, #0
	bhi _020B5E78
_020B5DEC:
	ldr r1, _020B5E9C ; =_021D84C4
	add r3, lr, r4
	ldr r2, [r1]
	mov lr, #1
	add r2, r2, ip
	str r2, [r1]
	b _020B5E78
_020B5E08:
	mov lr, #0
	b _020B5E78
_020B5E10:
	ldr r2, _020B5E9C ; =_021D84C4
	ldr lr, [r2, #4]
	cmp lr, r0
	blo _020B5E74
	sub r2, lr, r0
	cmp r1, #0
	andne r4, r2, #7
	andeq r4, r2, #0xf
	ldr r2, _020B5E9C ; =_021D84C4
	add ip, r0, r4
	ldr r2, [r2]
	sub r2, lr, r2
	cmp r2, ip
	blo _020B5E74
	cmp r1, #0
	beq _020B5E5C
	cmp lr, #0x10000
	movhi lr, #0
	bhi _020B5E78
_020B5E5C:
	ldr r1, _020B5E9C ; =_021D84C4
	mov lr, #1
	ldr r2, [r1, #4]
	sub r3, r2, ip
	str r3, [r1, #4]
	b _020B5E78
_020B5E74:
	mov lr, #0
_020B5E78:
	cmp lr, #0
	moveq r0, #0
	ldmeqia sp!, {r4, pc}
	mov r1, r3, lsl #0xd
	mov r2, r0, lsr #3
	mov r0, r1, lsr #0x10
	orr r0, r0, r2, lsl #16
	ldmia sp!, {r4, pc}
	.align 2, 0
_020B5E98: .word 0x0007FFF8
_020B5E9C: .word _021D84C4
	arm_func_end NNS_GfdAllocFrmPlttVram
