	.include "asm/macros.inc"
	.include "mi_compress.inc"
	.include "global.inc"
	.public SearchLZ

.text
	arm_func_start SearchLZ
SearchLZ: ; 0x020D4F38
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, lr}
	ldr r7, [sp, #0x20]
	mov r4, #2
	cmp r2, #3
	movlo r0, #0
	ldmloia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
	sub ip, r1, #0x1000
	cmp ip, r0
	movlo ip, r0
	sub r6, r1, ip
	cmp r6, #2
	blt _020D5018
_020D4F68:
	mov r0, r1
	ldrb r5, [r1]
	b _020D4F84
_020D4F74:
	add ip, ip, #1
	sub r6, r1, ip
	cmp r6, #2
	blt _020D5018
_020D4F84:
	ldrb r8, [ip]
	cmp r5, r8
	ldreqb sb, [r1, #1]
	ldreqb r8, [ip, #1]
	cmpeq sb, r8
	ldreqb sb, [r1, #2]
	ldreqb r8, [ip, #2]
	cmpeq sb, r8
	bne _020D4F74
	add sl, ip, #3
	add r0, r0, #3
	mov r5, #3
	b _020D4FCC
_020D4FB8:
	add r5, r5, #1
	cmp r5, r7
	add r0, r0, #1
	add sl, sl, #1
	beq _020D4FE8
_020D4FCC:
	sub r8, r0, r1
	cmp r8, r2
	bhs _020D4FE8
	ldrb sb, [r0]
	ldrb r8, [sl]
	cmp sb, r8
	beq _020D4FB8
_020D4FE8:
	cmp r5, r4
	bls _020D5008
	mov r0, r6, lsl #0x10
	cmp r5, r7
	mov r4, r5
	mov lr, r0, lsr #0x10
	cmpne r5, r2
	beq _020D5018
_020D5008:
	add ip, ip, #1
	sub r6, r1, ip
	cmp r6, #2
	bge _020D4F68
_020D5018:
	cmp r4, #3
	movlo r0, #0
	movhs r0, r4
	strhsh lr, [r3]
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
	arm_func_end SearchLZ
