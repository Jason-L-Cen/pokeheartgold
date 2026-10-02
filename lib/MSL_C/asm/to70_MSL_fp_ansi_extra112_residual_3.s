	.include "asm/macros.inc"
	.include "MSL_fp_ansi.inc"
	.include "global.inc"
	.public __dec2num
	.extern __equals_dec
	.extern __less_dec
	.extern __minus_dec
	.extern __num2dec_internal
	.extern __str2dec
	.extern digit_values
	.extern fp_str$6F5C

.text
	arm_func_start __dec2num
__dec2num: ; 0x020EFBD0
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0xfc
	mov r4, r0
	ldrb r0, [r4, #4]
	cmp r0, #0
	bne _020EFC10
	ldrsb r0, [r4]
	mov r2, #0
	cmp r0, #0
	ldreq r3, _020F01D4 ; =0x3FF00000
	mov r0, #0
	ldrne r3, _020F01D8 ; =0xBFF00000
	mov r1, r0
	bl copysign
	add sp, sp, #0xfc
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020EFC10:
	ldrb r0, [r4, #5]
	cmp r0, #0x30
	beq _020EFC30
	cmp r0, #0x49
	beq _020EFC58
	cmp r0, #0x4e
	beq _020EFC8C
	b _020EFCD4
_020EFC30:
	ldrsb r0, [r4]
	mov r2, #0
	cmp r0, #0
	ldreq r3, _020F01D4 ; =0x3FF00000
	mov r0, #0
	ldrne r3, _020F01D8 ; =0xBFF00000
	mov r1, r0
	bl copysign
	add sp, sp, #0xfc
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020EFC58:
	ldrsb r0, [r4]
	mov r5, #0
	cmp r0, #0
	ldreq r4, _020F01D4 ; =0x3FF00000
	ldr r0, _020F01DC ; =__float_huge
	ldrne r4, _020F01D8 ; =0xBFF00000
	ldr r0, [r0]
	bl _f2d
	mov r2, r5
	mov r3, r4
	bl copysign
	add sp, sp, #0xfc
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020EFC8C:
	ldr r1, _020F01E0 ; =0x7FF00000
	add r3, sp, #0x10
	mov r2, #0
	str r2, [r3]
	str r1, [r3, #4]
	ldrsb r0, [r4]
	cmp r0, #0
	beq _020EFCBC
	orr r0, r1, #0x80000000
	orr r1, r2, r2
	str r1, [r3]
	str r0, [r3, #4]
_020EFCBC:
	ldmia r3, {r0, r1}
	orr r0, r0, #0
	orr r1, r1, #0x80000
	stmia r3, {r0, r1}
	add sp, sp, #0xfc
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020EFCD4:
	add r3, sp, #0xd6
	mov r5, r4
	mov r2, #9
_020EFCE0:
	ldrh r1, [r5]
	ldrh r0, [r5, #2]
	add r5, r5, #4
	subs r2, r2, #1
	strh r1, [r3]
	strh r0, [r3, #2]
	add r3, r3, #4
	bne _020EFCE0
	ldrh r0, [r5]
	add r1, sp, #0xdb
	strh r0, [r3]
	ldrb r0, [sp, #0xda]
	add r5, r1, r0
	cmp r1, r5
	bhs _020EFD30
_020EFD1C:
	ldrb r0, [r1]
	sub r0, r0, #0x30
	strb r0, [r1], #1
	cmp r1, r5
	blo _020EFD1C
_020EFD30:
	ldrb r1, [sp, #0xda]
	ldrsh r2, [sp, #0xd8]
	add r0, sp, #0xb0
	sub r1, r1, #1
	add r1, r2, r1
	strh r1, [sp, #0xd8]
	ldr r1, _020F01E4 ; =fp_str$6F5C
	mov r2, #0x134
	ldrsh fp, [sp, #0xd8]
	bl __str2dec
	add r0, sp, #0xb0
	add r1, sp, #0xd6
	bl __less_dec
	cmp r0, #0
	beq _020EFDA0
	ldrsb r0, [r4]
	mov r5, #0
	cmp r0, #0
	ldreq r4, _020F01D4 ; =0x3FF00000
	ldr r0, _020F01DC ; =__float_huge
	ldrne r4, _020F01D8 ; =0xBFF00000
	ldr r0, [r0]
	bl _f2d
	mov r2, r5
	mov r3, r4
	bl copysign
	add sp, sp, #0xfc
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020EFDA0:
	add r1, sp, #0xdb
	ldrb r0, [r1]
	add r8, r1, #1
	bl _dfltu
	str r0, [sp, #8]
	str r1, [sp, #0xc]
	cmp r8, r5
	bhs _020EFE78
_020EFDC0:
	sub r0, r5, r8
	mov r1, r0, lsr #0x1f
	rsb r0, r1, r0, lsl #29
	adds r7, r1, r0, ror #29
	moveq r7, #8
	mov r6, #0
	mov r2, #0
	cmp r7, #0
	ble _020EFDFC
	mov r0, #0xa
_020EFDE8:
	ldrb r1, [r8], #1
	add r2, r2, #1
	cmp r2, r7
	mla r6, r0, r6, r1
	blt _020EFDE8
_020EFDFC:
	ldr r0, _020F01E8 ; =digit_values
	ldr r1, [sp, #0xc]
	add r3, r0, r7, lsl #3
	ldr r2, [r3, #-8]
	ldr r0, [sp, #8]
	ldr r3, [r3, #-4]
	bl _dmul
	mov r4, r0
	mov sb, r1
	mov r0, r6
	bl _dfltu
	mov r2, r0
	mov r3, r1
	mov r0, r4
	mov r1, sb
	bl _dadd
	cmp r6, #0
	mov r6, r0
	mov sl, r1
	beq _020EFE64
	mov r0, r4
	mov r1, sb
	mov r2, r6
	mov r3, sl
	bl _deq
	beq _020EFE78
_020EFE64:
	str r6, [sp, #8]
	str sl, [sp, #0xc]
	cmp r8, r5
	sub fp, fp, r7
	blo _020EFDC0
_020EFE78:
	cmp fp, #0
	bge _020EFEBC
	rsb r0, fp, #0
	bl _dflt
	mov r3, r1
	mov r2, r0
	ldr r1, _020F01EC ; =0x40140000
	mov r0, #0
	bl pow
	mov r2, r0
	mov r3, r1
	ldr r0, [sp, #8]
	ldr r1, [sp, #0xc]
	bl _ddiv
	str r0, [sp, #8]
	str r1, [sp, #0xc]
	b _020EFEF4
_020EFEBC:
	mov r0, fp
	bl _dflt
	mov r3, r1
	mov r2, r0
	ldr r1, _020F01EC ; =0x40140000
	mov r0, #0
	bl pow
	mov r2, r0
	mov r3, r1
	ldr r0, [sp, #8]
	ldr r1, [sp, #0xc]
	bl _dmul
	str r0, [sp, #8]
	str r1, [sp, #0xc]
_020EFEF4:
	ldr r0, [sp, #8]
	ldr r1, [sp, #0xc]
	mov r2, fp
	bl ldexp
	str r0, [sp, #8]
	str r1, [sp, #0xc]
	bl __fpclassifyd
	cmp r0, #2
	bne _020EFF28
	ldr r0, _020F01F0 ; =0x7FEFFFFF
	mvn r1, #0
	str r1, [sp, #8]
	str r0, [sp, #0xc]
_020EFF28:
	ldr r1, [sp, #8]
	ldr r2, [sp, #0xc]
	add r0, sp, #0x8a
	add r4, sp, #0
	mov r5, #0
	bl __num2dec_internal
	add r0, sp, #0x8a
	add r1, sp, #0xd6
	bl __equals_dec
	cmp r0, #0
	bne _020F019C
	add r0, sp, #0x8a
	add r1, sp, #0xd6
	bl __less_dec
	cmp r0, #0
	movne r5, #1
	ldr r1, [sp, #8]
	ldr r0, [sp, #0xc]
	cmp r5, #0
	moveq r6, #1
	str r1, [sp]
	str r0, [sp, #4]
	movne r6, #0
_020EFF84:
	cmp r6, #0
	bne _020EFFAC
	ldmia r4, {r0, r1}
	adds r0, r0, #1
	adc r1, r1, #0
	stmia r4, {r0, r1}
	bl __fpclassifyd
	cmp r0, #2
	beq _020F019C
	b _020EFFC4
_020EFFAC:
	ldr r1, [r4]
	ldr r0, [r4, #4]
	subs r1, r1, #1
	sbc r0, r0, #0
	str r1, [r4]
	str r0, [r4, #4]
_020EFFC4:
	ldr r1, [sp]
	ldr r2, [sp, #4]
	add r0, sp, #0x64
	bl __num2dec_internal
	cmp r5, #0
	beq _020EFFF0
	add r0, sp, #0x64
	add r1, sp, #0xd6
	bl __less_dec
	cmp r0, #0
	beq _020F0114
_020EFFF0:
	cmp r5, #0
	bne _020F00CC
	add r0, sp, #0xd6
	add r1, sp, #0x64
	bl __less_dec
	cmp r0, #0
	bne _020F00CC
	add r3, sp, #0x8a
	add r5, sp, #0x3e
	mov r2, #9
_020F0018:
	ldrh r1, [r3]
	ldrh r0, [r3, #2]
	add r3, r3, #4
	strh r1, [r5]
	strh r0, [r5, #2]
	add r5, r5, #4
	subs r2, r2, #1
	bne _020F0018
	ldrh r0, [r3]
	add r3, sp, #0x64
	add r4, sp, #0x8a
	strh r0, [r5]
	mov r2, #9
_020F004C:
	ldrh r1, [r3]
	ldrh r0, [r3, #2]
	add r3, r3, #4
	strh r1, [r4]
	strh r0, [r4, #2]
	add r4, r4, #4
	subs r2, r2, #1
	bne _020F004C
	ldrh r0, [r3]
	add r3, sp, #0x3e
	add r5, sp, #0x64
	strh r0, [r4]
	mov r2, #9
_020F0080:
	ldrh r1, [r3]
	ldrh r0, [r3, #2]
	add r3, r3, #4
	strh r1, [r5]
	strh r0, [r5, #2]
	add r5, r5, #4
	subs r2, r2, #1
	bne _020F0080
	ldrh r4, [r3]
	ldr r3, [sp, #8]
	ldr r1, [sp]
	ldr r2, [sp, #0xc]
	ldr r0, [sp, #4]
	strh r4, [r5]
	str r1, [sp, #8]
	str r0, [sp, #0xc]
	str r3, [sp]
	str r2, [sp, #4]
	b _020F0114
_020F00CC:
	add r7, sp, #0x64
	add r3, sp, #0x8a
	mov r2, #9
_020F00D8:
	ldrh r1, [r7]
	ldrh r0, [r7, #2]
	add r7, r7, #4
	strh r1, [r3]
	strh r0, [r3, #2]
	add r3, r3, #4
	subs r2, r2, #1
	bne _020F00D8
	ldrh r2, [r7]
	ldr r1, [sp]
	ldr r0, [sp, #4]
	strh r2, [r3]
	str r1, [sp, #8]
	str r0, [sp, #0xc]
	b _020EFF84
_020F0114:
	add r0, sp, #0x3e
	add r1, sp, #0xd6
	add r2, sp, #0x8a
	bl __minus_dec
	add r0, sp, #0x18
	add r1, sp, #0x64
	add r2, sp, #0xd6
	bl __minus_dec
	add r0, sp, #0x3e
	add r1, sp, #0x18
	bl __equals_dec
	cmp r0, #0
	beq _020F0178
	ldr r1, [sp, #8]
	ldr r0, [sp, #0xc]
	and r1, r1, #1
	and r0, r0, #0
	cmp r0, #0
	cmpeq r1, #0
	beq _020F019C
	ldr r1, [sp]
	ldr r0, [sp, #4]
	str r1, [sp, #8]
	str r0, [sp, #0xc]
	b _020F019C
_020F0178:
	add r0, sp, #0x3e
	add r1, sp, #0x18
	bl __less_dec
	cmp r0, #0
	bne _020F019C
	ldr r1, [sp]
	ldr r0, [sp, #4]
	str r1, [sp, #8]
	str r0, [sp, #0xc]
_020F019C:
	ldrsb r0, [sp, #0xd6]
	cmp r0, #0
	beq _020F01C4
	mov r0, #0
	ldr r2, [sp, #8]
	ldr r3, [sp, #0xc]
	mov r1, r0
	bl _dsub
	str r0, [sp, #8]
	str r1, [sp, #0xc]
_020F01C4:
	ldr r0, [sp, #8]
	ldr r1, [sp, #0xc]
	add sp, sp, #0xfc
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_020F01D4: .word 0x3FF00000
_020F01D8: .word 0xBFF00000
_020F01DC: .word __float_huge
_020F01E0: .word 0x7FF00000
_020F01E4: .word fp_str$6F5C
_020F01E8: .word digit_values
_020F01EC: .word 0x40140000
_020F01F0: .word 0x7FEFFFFF
	arm_func_end __dec2num
