	.extern __str2dec
	.extern __timesdec
	.extern __two_exp
	.extern __ull2dec
	.extern fp_str$6E88
	.extern fp_str$6EB8
	.extern fp_str$6EE0
	.extern fp_str$6EF8
	.extern fp_str$6F08
	.extern fp_str$6F10
	.extern fp_str$6F18
	.extern fp_str$6F20
	.extern fp_str$6F28
	.extern fp_str$6F2C
	.extern fp_str$6F30
	.extern fp_str$6F34
	.extern fp_str$6F38
	.extern fp_str$6F3C
	.extern fp_str$6F40
	.extern fp_str$6F44
	.extern fp_str$6F48
	.extern fp_str$6F4C
	.extern fp_str$6F50
	.extern fp_str$6F54
	.extern fp_str$6F58
	.include "asm/macros.inc"
	.include "MSL_fp_ansi.inc"
	.include "global.inc"

.text
	arm_func_start __minus_dec
__minus_dec: ; 0x020EF704
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	mov r6, r0
	mov r5, #9
_020EF710:
	ldrh r4, [r1]
	ldrh r3, [r1, #2]
	add r1, r1, #4
	subs r5, r5, #1
	strh r4, [r6]
	strh r3, [r6, #2]
	add r6, r6, #4
	bne _020EF710
	ldrh r1, [r1]
	strh r1, [r6]
	ldrb r1, [r2, #5]
	cmp r1, #0
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	ldrb r7, [r0, #4]
	ldrb r3, [r2, #4]
	ldrsh r4, [r0, #2]
	mov r1, r7
	cmp r7, r3
	movlt r1, r3
	ldrsh r3, [r2, #2]
	sub r6, r4, r3
	add r1, r1, r6
	cmp r1, #0x20
	movgt r1, #0x20
	cmp r7, r1
	bge _020EF79C
	mov r4, #0
_020EF77C:
	ldrb r5, [r0, #4]
	add r3, r0, r7
	add r5, r5, #1
	strb r5, [r0, #4]
	strb r4, [r3, #5]
	ldrb r7, [r0, #4]
	cmp r7, r1
	blt _020EF77C
_020EF79C:
	ldrb r4, [r2, #4]
	add r3, r0, #5
	add ip, r3, r1
	add r4, r4, r6
	cmp r4, r1
	addlt ip, r3, r4
	sub r4, ip, r3
	add r1, r2, #5
	sub r4, r4, r6
	add lr, r1, r4
	mov r4, lr
	b _020EF830
_020EF7CC:
	ldrb r7, [ip, #-1]!
	ldrb r5, [lr, #-1]!
	cmp r7, r5
	bhs _020EF820
	ldrb r7, [ip, #-1]
	sub r5, ip, #1
	cmp r7, #0
	bne _020EF7F8
_020EF7EC:
	ldrb r7, [r5, #-1]!
	cmp r7, #0
	beq _020EF7EC
_020EF7F8:
	cmp r5, ip
	beq _020EF820
_020EF800:
	ldrb r7, [r5]
	sub r7, r7, #1
	strb r7, [r5]
	ldrb r7, [r5, #1]!
	cmp r5, ip
	add r7, r7, #0xa
	strb r7, [r5]
	bne _020EF800
_020EF820:
	ldrb r7, [ip]
	ldrb r5, [lr]
	sub r5, r7, r5
	strb r5, [ip]
_020EF830:
	cmp ip, r3
	cmphi lr, r1
	bhi _020EF7CC
	ldrb r5, [r2, #4]
	sub lr, r4, r1
	cmp lr, r5
	bge _020EF90C
	ldrb r1, [r4]
	mov r7, #0
	cmp r1, #5
	movlo r7, #1
	blo _020EF8A8
	bne _020EF8A8
	add r1, r2, #5
	add r2, r1, r5
	add r4, r4, #1
	cmp r4, r2
	bhs _020EF890
_020EF878:
	ldrb r1, [r4]
	cmp r1, #0
	bne _020EF90C
	add r4, r4, #1
	cmp r4, r2
	blo _020EF878
_020EF890:
	add r1, r3, lr
	add r2, r1, r6
	ldrb r1, [r2, #-1]
	sub ip, r2, #1
	tst r1, #1
	movne r7, #1
_020EF8A8:
	cmp r7, #0
	beq _020EF90C
	ldrb r1, [ip]
	cmp r1, #1
	bhs _020EF900
	ldrb r1, [ip, #-1]
	sub r2, ip, #1
	cmp r1, #0
	bne _020EF8D8
_020EF8CC:
	ldrb r1, [r2, #-1]!
	cmp r1, #0
	beq _020EF8CC
_020EF8D8:
	cmp r2, ip
	beq _020EF900
_020EF8E0:
	ldrb r1, [r2]
	sub r1, r1, #1
	strb r1, [r2]
	ldrb r1, [r2, #1]!
	cmp r2, ip
	add r1, r1, #0xa
	strb r1, [r2]
	bne _020EF8E0
_020EF900:
	ldrb r1, [ip]
	sub r1, r1, #1
	strb r1, [ip]
_020EF90C:
	ldrb r1, [r3]
	mov r5, r3
	cmp r1, #0
	bne _020EF928
_020EF91C:
	ldrb r1, [r5, #1]!
	cmp r1, #0
	beq _020EF91C
_020EF928:
	cmp r5, r3
	bls _020EF970
	ldrsh r1, [r0, #2]
	sub r2, r5, r3
	and r4, r2, #0xff
	sub r1, r1, r4
	strh r1, [r0, #2]
	ldrb r1, [r0, #4]
	add r2, r3, r1
	cmp r5, r2
	bhs _020EF964
_020EF954:
	ldrb r1, [r5], #1
	cmp r5, r2
	strb r1, [r3], #1
	blo _020EF954
_020EF964:
	ldrb r1, [r0, #4]
	sub r1, r1, r4
	strb r1, [r0, #4]
_020EF970:
	ldrb r1, [r0, #4]
	add r2, r0, #5
	add r3, r2, r1
	cmp r3, r2
	bls _020EF998
_020EF984:
	ldrb r1, [r3, #-1]!
	cmp r1, #0
	bne _020EF998
	cmp r3, r2
	bhi _020EF984
_020EF998:
	sub r1, r3, r2
	add r1, r1, #1
	strb r1, [r0, #4]
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	arm_func_end __minus_dec
