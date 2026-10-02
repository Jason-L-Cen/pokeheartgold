	.extern _02110934
	.extern _0211093C
	.extern _02110950
	.extern _02110968
	.extern _02110980
	.extern _021D84C0
	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"

.text
	arm_func_start NNS_GfdAllocFrmTexVram
NNS_GfdAllocFrmTexVram: ; 0x020B5B20
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, lr}
	cmp r0, #0
	moveq r3, #0x10
	addne r0, r0, #0xf
	bicne r3, r0, #0xf
	ldr r0, _020B5C7C ; =0x0007FFF0
	cmp r3, r0
	movhs r0, #0
	ldmhsia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	cmp r1, #0
	beq _020B5C04
	mov r4, #0
	ldr r8, _020B5C80 ; =_02110980
	ldr sb, _020B5C84 ; =_02110968
	ldr r0, _020B5C88 ; =_02110934
	mov r7, r4
_020B5B60:
	ldr lr, [r0, r4, lsl #2]
	ldr r5, [lr, #8]
	cmp r5, #0
	beq _020B5BF0
	ldmia lr, {r5, r6}
	sub r5, r6, r5
	cmp r5, r3
	blo _020B5BF0
	ldrh r5, [lr, #0x10]
	cmp r5, #0
	beq _020B5B98
	cmp r5, #3
	beq _020B5BA0
	b _020B5BA8
_020B5B98:
	mov ip, sb
	b _020B5BAC
_020B5BA0:
	mov ip, r8
	b _020B5BAC
_020B5BA8:
	mov ip, r7
_020B5BAC:
	ldr r5, [ip, #8]
	cmp r5, #0
	beq _020B5BF0
	ldmia ip, {r5, r6}
	sub r5, r6, r5
	cmp r5, r3, lsr #1
	blo _020B5BF0
	ldr r4, [lr]
	mov r0, #1
	add r2, r4, r3
	str r2, [lr]
	ldr r2, [ip]
	add r2, r2, r3, lsr #1
	str r2, [ip]
	ldr r2, [lr, #0x14]
	add r2, r4, r2
	b _020B5C58
_020B5BF0:
	add r4, r4, #1
	cmp r4, #2
	blt _020B5B60
	mov r0, #0
	b _020B5C58
_020B5C04:
	ldr lr, _020B5C8C ; =_0211093C
	mov r5, #0
_020B5C0C:
	ldr ip, [lr, r5, lsl #2]
	ldr r0, [ip, #8]
	cmp r0, #0
	beq _020B5C48
	ldmia ip, {r0, r4}
	sub r0, r4, r0
	cmp r0, r3
	blo _020B5C48
	ldr r2, [ip, #4]
	mov r0, #1
	sub r4, r2, r3
	str r4, [ip, #4]
	ldr r2, [ip, #0x14]
	add r2, r4, r2
	b _020B5C58
_020B5C48:
	add r5, r5, #1
	cmp r5, #5
	blt _020B5C0C
	mov r0, #0
_020B5C58:
	cmp r0, #0
	moveq r0, #0
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	mov r0, r2, lsl #0xd
	mov r2, r3, lsr #4
	mov r0, r0, lsr #0x10
	orr r0, r0, r2, lsl #16
	orr r0, r0, r1, lsl #31
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	.align 2, 0
_020B5C7C: .word 0x0007FFF0
_020B5C80: .word _02110980
_020B5C84: .word _02110968
_020B5C88: .word _02110934
_020B5C8C: .word _0211093C
	arm_func_end NNS_GfdAllocFrmTexVram
