	.include "asm/macros.inc"
	.include "overlay_39_arm.inc"
	.include "global.inc"
	.public ov39_02229E40
	.extern _0222AB84
	.extern ov39_022298FC
	.extern ov39_02229910
	.extern ov39_02229FC0
	.extern ov39_0222A544

.text
	arm_func_start ov39_02229E40
ov39_02229E40: ; 0x02229E40
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	mov sl, #0
	sub r0, sl, #1
	str r0, [sp]
	bl OS_GetTick
	ldr r2, _02229FB8 ; =0x0007FD88
	mov r3, sl
	bl _ll_udiv
	mvn r4, #0
	ldr fp, _02229FB8 ; =0x0007FD88
	ldr r6, _02229FBC ; =_0222AB84
	mov sb, r0
	sub r5, r4, #5
	add r7, sp, #0
	mov r8, sl
_02229E7C:
	ldr r1, [r6, #0x260]
	ldr r2, [r6, #0x25c]
	ldr r0, [r6, #0x26c]
	mov r3, r8
	add r1, r1, sl
	sub r2, r2, sl
	bl SOC_Recv
	cmp r0, r5
	beq _02229F38
	cmp r0, #0
	bgt _02229EBC
	ldr r0, _02229FBC ; =_0222AB84
	mov r1, #9
	str r1, [r0, #0x250]
	mov r0, #0
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_02229EBC:
	add sl, sl, r0
	cmp sl, #4
	blo _02229EF8
	ldr r0, [sp]
	cmp r0, r4
	bne _02229EF8
	ldr r2, [r6, #0x260]
	ldrb r1, [r2]
	ldrb r0, [r2, #1]
	strb r1, [r7]
	strb r0, [r7, #1]
	ldrb r1, [r2, #2]
	ldrb r0, [r2, #3]
	strb r1, [r7, #2]
	strb r0, [r7, #3]
_02229EF8:
	ldr r0, [sp]
	cmp sl, r0
	bne _02229F38
	cmp r0, r4
	beq _02229F38
	ldr r0, _02229FBC ; =_0222AB84
	sub r1, sl, #4
	ldr r2, [r0, #0x260]
	add r0, r2, #4
	add r2, r2, #4
	bl ov39_0222A544
	ldr r1, [sp]
	ldr r0, _02229FBC ; =_0222AB84
	sub r1, r1, #4
	str r1, [r0, #0x25c]
	b _02229FB0
_02229F38:
	ldr r0, [r6, #0x24c]
	cmp r0, #0
	beq _02229F78
	bl OS_GetTick
	mov r2, fp
	mov r3, #0
	bl _ll_udiv
	ldr r1, [r6, #0x24c]
	subs r0, r0, sb
	cmp r0, r1
	blt _02229F78
	ldr r0, _02229FBC ; =_0222AB84
	mov r1, #0xc
	str r1, [r0, #0x250]
	mov r0, #0
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_02229F78:
	bl ov39_022298FC
	ldr r0, [r6, #0x268]
	cmp r0, #1
	bne _02229FA0
	ldr r0, _02229FBC ; =_0222AB84
	mov r1, #0xd
	str r1, [r0, #0x250]
	bl ov39_02229910
	mov r0, #0
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_02229FA0:
	bl ov39_02229910
	mov r0, #1
	bl ov39_02229FC0
	b _02229E7C
_02229FB0:
	mov r0, #1
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.balign 4, 0
_02229FB8: .word 0x0007FD88
_02229FBC: .word _0222AB84
	arm_func_end ov39_02229E40
