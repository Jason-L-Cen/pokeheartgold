	.include "asm/macros.inc"
	.include "nitro.inc"
	.include "global.inc"
	.extern STD_GetStringLength
	.section .text.unified_46,4,1,6
	arm_func_start MATH_QSort
MATH_QSort: ; 0x020E3A84
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	cmp r1, #1
	ble _020E3D6C
	ldr r4, [sp, #0x24]
	mov fp, r3
	mov r8, r2
	cmp r4, #0
	bne _020E3ABC
	clz r2, r1
	rsb r2, r2, #0x20
	mov r2, r2, lsl #3
	sub sp, sp, r2
	mov r4, sp
	str r2, [sp, #-4]!
_020E3ABC:
	sub r1, r1, #1
	mla r1, r1, r8, r0
	mov r5, r4
	str r0, [r4], #4
	str r1, [r4], #4
	clz r2, r8
	rsb r2, r2, #0x20
	str r2, [sp, #-4]!
_020E3ADC:
	cmp r4, r5
	beq _020E3D3C
	ldr r7, [r4, #-4]
	ldr r6, [r4, #-8]!
	sub r2, r7, r6
	cmp r2, r8
	bne _020E3B50
	mov r0, r6
	mov r1, r7
	blx fp
	cmp r0, #0
	ble _020E3ADC
	mov r0, r8
	tst r0, #3
	beq _020E3B34
_020E3B18:
	ldrb r1, [r6]
	subs r0, r0, #1
	swpb r1, r1, [r7]
	add r7, r7, #1
	strb r1, [r6], #1
	bne _020E3B18
	b _020E3ADC
_020E3B34:
	ldr r1, [r6]
	subs r0, r0, #4
	swp r1, r1, [r7]
	add r7, r7, #4
	str r1, [r6], #4
	bne _020E3B34
	b _020E3ADC
_020E3B50:
	ldr r3, [sp]
	sub r2, r7, r6
	mov r2, r2, lsr r3
	mla r2, r2, r8, r6
	mov r3, r6
	mov r0, r8
	mov r2, r2
	tst r0, #3
	beq _020E3B90
_020E3B74:
	ldrb r1, [r2]
	subs r0, r0, #1
	swpb r1, r1, [r3]
	add r3, r3, #1
	strb r1, [r2], #1
	bne _020E3B74
	b _020E3BA8
_020E3B90:
	ldr r1, [r2]
	subs r0, r0, #4
	swp r1, r1, [r3]
	add r3, r3, #4
	str r1, [r2], #4
	bne _020E3B90
_020E3BA8:
	mov sb, r6
	mov sl, r7
	add sb, sb, r8
_020E3BB4:
	cmp sb, r7
	bge _020E3BDC
	mov r1, r6
	mov r0, sb
	blx fp
	cmp r0, #0
	blt _020E3BD4
	b _020E3BD8
_020E3BD4:
	add sb, sb, r8
_020E3BD8:
	blt _020E3BB4
_020E3BDC:
	mov r1, r6
	mov r0, sl
	blx fp
	cmp r0, #0
	bgt _020E3BF4
	b _020E3BF8
_020E3BF4:
	sub sl, sl, r8
_020E3BF8:
	bgt _020E3BDC
	cmp sb, sl
	bge _020E3C5C
	mov r2, sb
	mov r3, sl
	mov r0, r8
	tst r0, #3
	beq _020E3C34
_020E3C18:
	ldrb r1, [r2]
	subs r0, r0, #1
	swpb r1, r1, [r3]
	add r3, r3, #1
	strb r1, [r2], #1
	bne _020E3C18
	b _020E3C4C
_020E3C34:
	ldr r1, [r2]
	subs r0, r0, #4
	swp r1, r1, [r3]
	add r3, r3, #4
	str r1, [r2], #4
	bne _020E3C34
_020E3C4C:
	add sb, sb, r8
	sub sl, sl, r8
	cmp sb, sl
	ble _020E3BB4
_020E3C5C:
	mov r2, r6
	mov r3, sl
	mov r0, r8
	tst r0, #3
	beq _020E3C8C
_020E3C70:
	ldrb r1, [r2]
	subs r0, r0, #1
	swpb r1, r1, [r3]
	add r3, r3, #1
	strb r1, [r2], #1
	bne _020E3C70
	b _020E3CA4
_020E3C8C:
	ldr r1, [r2]
	subs r0, r0, #4
	swp r1, r1, [r3]
	add r3, r3, #4
	str r1, [r2], #4
	bne _020E3C8C
_020E3CA4:
	sub r2, sl, r6
	sub r3, r7, sl
	cmp r2, r3
	ble _020E3CF8
	sub r2, sl, r8
	cmp r6, r2
	blt _020E3CC4
	b _020E3CC8
_020E3CC4:
	str r6, [r4], #4
_020E3CC8:
	blt _020E3CD0
	b _020E3CD4
_020E3CD0:
	str r2, [r4], #4
_020E3CD4:
	add r2, sl, r8
	cmp r2, r7
	blt _020E3CE4
	b _020E3CE8
_020E3CE4:
	str r2, [r4], #4
_020E3CE8:
	blt _020E3CF0
	b _020E3CF4
_020E3CF0:
	str r7, [r4], #4
_020E3CF4:
	b _020E3ADC
_020E3CF8:
	add r2, sl, r8
	cmp r2, r7
	blt _020E3D08
	b _020E3D0C
_020E3D08:
	str r2, [r4], #4
_020E3D0C:
	blt _020E3D14
	b _020E3D18
_020E3D14:
	str r7, [r4], #4
_020E3D18:
	sub r2, sl, r8
	cmp r6, r2
	blt _020E3D28
	b _020E3D2C
_020E3D28:
	str r6, [r4], #4
_020E3D2C:
	blt _020E3D34
	b _020E3D38
_020E3D34:
	str r2, [r4], #4
_020E3D38:
	b _020E3ADC
_020E3D3C:
	add sp, sp, #4
	sub r4, r4, #4
	cmp r4, sp
	beq _020E3D50
	b _020E3D54
_020E3D50:
	ldr r0, [sp]
_020E3D54:
	beq _020E3D5C
	b _020E3D60
_020E3D5C:
	add r0, r0, #4
_020E3D60:
	beq _020E3D68
	b _020E3D6C
_020E3D68:
	add sp, sp, r0
_020E3D6C:
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	bx lr
	arm_func_end MATH_QSort

	arm_func_start STD_CopyLString
STD_CopyLString: ; 0x020E3D74
	stmdb sp!, {r4, lr}
	sub ip, r2, #1
	mov r4, r1
	cmp ip, #0
	mov lr, #0
	ble _020E3DB0
_020E3D8C:
	ldrsb r3, [r4]
	strb r3, [r0, lr]
	ldrsb r3, [r4]
	cmp r3, #0
	beq _020E3DB0
	add lr, lr, #1
	cmp lr, ip
	add r4, r4, #1
	blt _020E3D8C
_020E3DB0:
	sub r3, r2, #1
	cmp lr, r3
	blt _020E3DC8
	cmp r2, #0
	movne r2, #0
	strneb r2, [r0, lr]
_020E3DC8:
	mov r0, r1
	bl STD_GetStringLength
	ldmia sp!, {r4, pc}
	arm_func_end STD_CopyLString
