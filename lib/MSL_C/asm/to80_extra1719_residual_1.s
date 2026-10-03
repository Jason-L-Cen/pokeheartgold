	.extern __pformatter_empty_string_1
	.extern __pformatter_empty_string_2
	.extern printf_double2hex_0p0
	.extern printf_double2hex_inf_lower
	.extern printf_double2hex_inf_upper
	.extern printf_double2hex_nan_lower
	.extern printf_double2hex_nan_upper
	.extern printf_double2hex_neg_inf_lower
	.extern printf_double2hex_neg_inf_upper
	.extern printf_double2hex_neg_nan_lower
	.extern printf_double2hex_neg_nan_upper
	.include "asm/macros.inc"
	.include "MSL_Common_printf.inc"
	.include "global.inc"
	.public long2str

.text
	arm_func_start long2str
long2str: ; 0x020E6230
	stmdb sp!, {r0, r1, r2, r3}
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x10
	movs sl, r0
	mov r0, #0
	mov r5, r1
	str r0, [sp, #0xc]
	ldr r7, [sp, #0x4c]
	mov r6, r0
	strb r0, [r5, #-1]!
	ldrb r0, [sp, #0x43]
	str r1, [sp]
	ldrb r8, [sp, #0x45]
	str r0, [sp, #4]
	ldr r0, [sp, #0x48]
	ldrb fp, [sp, #0x41]
	str r0, [sp, #8]
	cmpeq r7, #0
	bne _020E62A4
	ldr r0, [sp, #4]
	cmp r0, #0
	beq _020E6290
	cmp r8, #0x6f
	beq _020E62A4
_020E6290:
	add sp, sp, #0x10
	mov r0, r5
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	add sp, sp, #0x10
	bx lr
_020E62A4:
	cmp r8, #0x69
	bgt _020E62CC
	bge _020E6300
	cmp r8, #0x58
	bgt _020E62C0
	beq _020E632C
	b _020E6334
_020E62C0:
	cmp r8, #0x64
	beq _020E6300
	b _020E6334
_020E62CC:
	cmp r8, #0x6f
	bgt _020E62E0
	moveq r4, #8
	moveq fp, #0
	b _020E6334
_020E62E0:
	cmp r8, #0x78
	bgt _020E6334
	cmp r8, #0x75
	blt _020E6334
	beq _020E6320
	cmp r8, #0x78
	beq _020E632C
	b _020E6334
_020E6300:
	cmp sl, #0
	mov r4, #0xa
	bge _020E6334
	mov r0, #1
	cmp sl, #0x80000000
	rsbne sl, sl, #0
	str r0, [sp, #0xc]
	b _020E6334
_020E6320:
	mov r4, #0xa
	mov fp, #0
	b _020E6334
_020E632C:
	mov r4, #0x10
	mov fp, #0
_020E6334:
	mov r0, sl
	mov r1, r4
	bl _u32_div_f
	mov sb, r1
	mov r0, sl
	mov r1, r4
	bl _u32_div_f
	cmp sb, #0xa
	mov sl, r0
	addlt sb, sb, #0x30
	blt _020E636C
	cmp r8, #0x78
	addeq sb, sb, #0x57
	addne sb, sb, #0x37
_020E636C:
	cmp sl, #0
	strb sb, [r5, #-1]!
	add r6, r6, #1
	bne _020E6334
	cmp r4, #8
	bne _020E63A0
	ldr r0, [sp, #4]
	cmp r0, #0
	ldrnesb r0, [r5]
	cmpne r0, #0x30
	movne r0, #0x30
	strneb r0, [r5, #-1]!
	addne r6, r6, #1
_020E63A0:
	ldrb r0, [sp, #0x40]
	cmp r0, #2
	bne _020E63D4
	ldr r0, [sp, #0xc]
	ldr r7, [sp, #8]
	cmp r0, #0
	cmpeq fp, #0
	subne r7, r7, #1
	cmp r4, #0x10
	bne _020E63D4
	ldr r0, [sp, #4]
	cmp r0, #0
	subne r7, r7, #2
_020E63D4:
	ldr r0, [sp]
	sub r1, r0, r5
	ldr r0, _020E6478 ; =0x000001FD
	add r1, r7, r1
	cmp r1, r0
	addgt sp, sp, #0x10
	movgt r0, #0
	ldmgtia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	addgt sp, sp, #0x10
	bxgt lr
	cmp r6, r7
	bge _020E6418
	mov r0, #0x30
_020E6408:
	add r6, r6, #1
	cmp r6, r7
	strb r0, [r5, #-1]!
	blt _020E6408
_020E6418:
	cmp r4, #0x10
	bne _020E6434
	ldr r0, [sp, #4]
	cmp r0, #0
	movne r0, #0x30
	strneb r8, [r5, #-1]
	strneb r0, [r5, #-2]!
_020E6434:
	ldr r0, [sp, #0xc]
	cmp r0, #0
	movne r0, #0x2d
	strneb r0, [r5, #-1]!
	bne _020E6464
	cmp fp, #1
	moveq r0, #0x2b
	streqb r0, [r5, #-1]!
	beq _020E6464
	cmp fp, #2
	moveq r0, #0x20
	streqb r0, [r5, #-1]!
_020E6464:
	mov r0, r5
	add sp, sp, #0x10
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	add sp, sp, #0x10
	bx lr
	.align 2, 0
_020E6478: .word 0x000001FD
	arm_func_end long2str
