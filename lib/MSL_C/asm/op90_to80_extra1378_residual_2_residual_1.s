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
	.extern double2hex
	.extern long2str
	.extern longlong2str
	.extern parse_format
	.extern round_decimal
	.public float2str

.text
	arm_func_start float2str
float2str: ; 0x020E6D78
	stmdb sp!, {r0, r1, r2, r3}
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x2c
	ldr r7, [sp, #0x68]
	ldr r3, _020E74B0 ; =0x000001FD
	ldrb r6, [sp, #0x61]
	ldrb r5, [sp, #0x5f]
	ldrb r4, [sp, #0x5d]
	cmp r7, r3
	mov sl, r0
	mov r8, r1
	mov sb, r2
	addgt sp, sp, #0x2c
	movgt r0, #0
	ldmgtia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	addgt sp, sp, #0x10
	bxgt lr
	mov ip, #0
	mov fp, #0x20
	add r0, sp, #0
	add r3, sp, #4
	mov r1, sl
	mov r2, r8
	strb ip, [sp]
	strh fp, [sp, #2]
	bl __num2dec
	ldrb r0, [sp, #8]
	add r1, sp, #9
	add r0, r1, r0
	b _020E6E08
_020E6DF0:
	ldrb r2, [sp, #8]
	ldrsh r1, [sp, #6]
	sub r2, r2, #1
	add r1, r1, #1
	strb r2, [sp, #8]
	strh r1, [sp, #6]
_020E6E08:
	ldrb r1, [sp, #8]
	cmp r1, #1
	bls _020E6E20
	ldrsb r1, [r0, #-1]!
	cmp r1, #0x30
	beq _020E6DF0
_020E6E20:
	ldrb r0, [sp, #9]
	cmp r0, #0x30
	beq _020E6E40
	cmp r0, #0x49
	beq _020E6E4C
	cmp r0, #0x4e
	beq _020E6FAC
	b _020E7100
_020E6E40:
	mov r0, #0
	strh r0, [sp, #6]
	b _020E7100
_020E6E4C:
	mov r2, #0
	mov r0, sl
	mov r1, r8
	mov r3, r2
	bl _dls
	bhs _020E6F10
	cmp r6, #0
	sub r0, sb, #5
	blt _020E6E78
	cmp r6, #0x80
	blt _020E6E80
_020E6E78:
	mov r1, #0
	b _020E6E90
_020E6E80:
	ldr r1, _020E74B4 ; =__msl_digit
	mov r2, r6, lsl #1
	ldrh r1, [r1, r2]
	and r1, r1, #0x200
_020E6E90:
	cmp r1, #0
	beq _020E6ED4
	ldr r3, _020E74B8 ; =printf_double2hex_neg_inf_upper
	add sp, sp, #0x2c
	ldrb r2, [r3]
	ldrb r1, [r3, #1]
	strb r2, [r0]
	strb r1, [r0, #1]
	ldrb r2, [r3, #2]
	ldrb r1, [r3, #3]
	strb r2, [r0, #2]
	strb r1, [r0, #3]
	ldrb r1, [r3, #4]
	strb r1, [r0, #4]
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	add sp, sp, #0x10
	bx lr
_020E6ED4:
	ldr r3, _020E74BC ; =printf_double2hex_neg_inf_lower
	add sp, sp, #0x2c
	ldrb r2, [r3]
	ldrb r1, [r3, #1]
	strb r2, [r0]
	strb r1, [r0, #1]
	ldrb r2, [r3, #2]
	ldrb r1, [r3, #3]
	strb r2, [r0, #2]
	strb r1, [r0, #3]
	ldrb r1, [r3, #4]
	strb r1, [r0, #4]
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	add sp, sp, #0x10
	bx lr
_020E6F10:
	cmp r6, #0
	sub r0, sb, #4
	blt _020E6F24
	cmp r6, #0x80
	blt _020E6F2C
_020E6F24:
	mov r1, #0
	b _020E6F3C
_020E6F2C:
	ldr r1, _020E74B4 ; =__msl_digit
	mov r2, r6, lsl #1
	ldrh r1, [r1, r2]
	and r1, r1, #0x200
_020E6F3C:
	cmp r1, #0
	beq _020E6F78
	ldr r3, _020E74C0 ; =printf_double2hex_inf_upper
	add sp, sp, #0x2c
	ldrb r2, [r3]
	ldrb r1, [r3, #1]
	strb r2, [r0]
	strb r1, [r0, #1]
	ldrb r2, [r3, #2]
	ldrb r1, [r3, #3]
	strb r2, [r0, #2]
	strb r1, [r0, #3]
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	add sp, sp, #0x10
	bx lr
_020E6F78:
	ldr r3, _020E74C4 ; =printf_double2hex_inf_lower
	add sp, sp, #0x2c
	ldrb r2, [r3]
	ldrb r1, [r3, #1]
	strb r2, [r0]
	strb r1, [r0, #1]
	ldrb r2, [r3, #2]
	ldrb r1, [r3, #3]
	strb r2, [r0, #2]
	strb r1, [r0, #3]
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	add sp, sp, #0x10
	bx lr
_020E6FAC:
	ldrsb r0, [sp, #4]
	cmp r0, #0
	beq _020E7064
	cmp r6, #0
	sub r0, sb, #5
	blt _020E6FCC
	cmp r6, #0x80
	blt _020E6FD4
_020E6FCC:
	mov r1, #0
	b _020E6FE4
_020E6FD4:
	ldr r1, _020E74B4 ; =__msl_digit
	mov r2, r6, lsl #1
	ldrh r1, [r1, r2]
	and r1, r1, #0x200
_020E6FE4:
	cmp r1, #0
	beq _020E7028
	ldr r3, _020E74C8 ; =printf_double2hex_neg_nan_upper
	add sp, sp, #0x2c
	ldrb r2, [r3]
	ldrb r1, [r3, #1]
	strb r2, [r0]
	strb r1, [r0, #1]
	ldrb r2, [r3, #2]
	ldrb r1, [r3, #3]
	strb r2, [r0, #2]
	strb r1, [r0, #3]
	ldrb r1, [r3, #4]
	strb r1, [r0, #4]
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	add sp, sp, #0x10
	bx lr
_020E7028:
	ldr r3, _020E74CC ; =printf_double2hex_neg_nan_lower
	add sp, sp, #0x2c
	ldrb r2, [r3]
	ldrb r1, [r3, #1]
	strb r2, [r0]
	strb r1, [r0, #1]
	ldrb r2, [r3, #2]
	ldrb r1, [r3, #3]
	strb r2, [r0, #2]
	strb r1, [r0, #3]
	ldrb r1, [r3, #4]
	strb r1, [r0, #4]
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	add sp, sp, #0x10
	bx lr
_020E7064:
	cmp r6, #0
	sub r0, sb, #4
	blt _020E7078
	cmp r6, #0x80
	blt _020E7080
_020E7078:
	mov r1, #0
	b _020E7090
_020E7080:
	ldr r1, _020E74B4 ; =__msl_digit
	mov r2, r6, lsl #1
	ldrh r1, [r1, r2]
	and r1, r1, #0x200
_020E7090:
	cmp r1, #0
	beq _020E70CC
	ldr r3, _020E74D0 ; =printf_double2hex_nan_upper
	add sp, sp, #0x2c
	ldrb r2, [r3]
	ldrb r1, [r3, #1]
	strb r2, [r0]
	strb r1, [r0, #1]
	ldrb r2, [r3, #2]
	ldrb r1, [r3, #3]
	strb r2, [r0, #2]
	strb r1, [r0, #3]
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	add sp, sp, #0x10
	bx lr
_020E70CC:
	ldr r3, _020E74D4 ; =printf_double2hex_nan_lower
	add sp, sp, #0x2c
	ldrb r2, [r3]
	ldrb r1, [r3, #1]
	strb r2, [r0]
	strb r1, [r0, #1]
	ldrb r2, [r3, #2]
	ldrb r1, [r3, #3]
	strb r2, [r0, #2]
	strb r1, [r0, #3]
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	add sp, sp, #0x10
	bx lr
_020E7100:
	ldrb r0, [sp, #8]
	ldrsh r1, [sp, #6]
	sub r8, sb, #1
	sub r0, r0, #1
	add r0, r1, r0
	strh r0, [sp, #6]
	mov r0, #0
	strb r0, [r8]
	cmp r6, #0x65
	bgt _020E7154
	bge _020E71E0
	cmp r6, #0x47
	bgt _020E749C
	cmp r6, #0x45
	blt _020E749C
	beq _020E71E0
	cmp r6, #0x46
	beq _020E7328
	cmp r6, #0x47
	beq _020E716C
	b _020E749C
_020E7154:
	cmp r6, #0x66
	bgt _020E7164
	beq _020E7328
	b _020E749C
_020E7164:
	cmp r6, #0x67
	bne _020E749C
_020E716C:
	ldrb r0, [sp, #8]
	cmp r0, r7
	ble _020E7184
	add r0, sp, #4
	mov r1, r7
	bl round_decimal
_020E7184:
	ldrsh r2, [sp, #6]
	mvn r0, #3
	cmp r2, r0
	blt _020E719C
	cmp r2, r7
	blt _020E71BC
_020E719C:
	cmp r5, #0
	ldreqb r0, [sp, #8]
	subne r7, r7, #1
	subeq r7, r0, #1
	cmp r6, #0x67
	moveq r6, #0x65
	movne r6, #0x45
	b _020E71E0
_020E71BC:
	cmp r5, #0
	addne r0, r2, #1
	subne r7, r7, r0
	bne _020E7328
	ldrb r1, [sp, #8]
	add r0, r2, #1
	subs r7, r1, r0
	movmi r7, #0
	b _020E7328
_020E71E0:
	ldrb r0, [sp, #8]
	add r1, r7, #1
	cmp r0, r1
	ble _020E71F8
	add r0, sp, #4
	bl round_decimal
_020E71F8:
	ldrsh lr, [sp, #6]
	mov fp, #0x2b
	mov sl, #0
	cmp lr, #0
	rsblt lr, lr, #0
	movlt fp, #0x2d
	ldr r3, _020E74D8 ; =0x66666667
	mov r0, #0xa
	b _020E724C
_020E721C:
	mov r1, lr, lsr #0x1f
	smull r2, ip, r3, lr
	add ip, r1, ip, asr #2
	smull r1, r2, r0, ip
	sub ip, lr, r1
	add r1, ip, #0x30
	strb r1, [r8, #-1]!
	mov r2, lr
	smull r1, lr, r3, r2
	mov r1, r2, lsr #0x1f
	add lr, r1, lr, asr #2
	add sl, sl, #1
_020E724C:
	cmp lr, #0
	bne _020E721C
	cmp sl, #2
	blt _020E721C
	strb fp, [r8, #-1]
	strb r6, [r8, #-2]!
	sub r1, sb, r8
	ldr r0, _020E74B0 ; =0x000001FD
	add r1, r7, r1
	cmp r1, r0
	addgt sp, sp, #0x2c
	movgt r0, #0
	ldmgtia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	addgt sp, sp, #0x10
	bxgt lr
	ldrb r1, [sp, #8]
	add r0, r7, #1
	cmp r1, r0
	bge _020E72B8
	add r0, r7, #2
	sub r0, r0, r1
	subs r1, r0, #1
	beq _020E72B8
	mov r0, #0x30
_020E72AC:
	strb r0, [r8, #-1]!
	subs r1, r1, #1
	bne _020E72AC
_020E72B8:
	ldrb r1, [sp, #8]
	add r0, sp, #9
	add r2, r0, r1
	subs r1, r1, #1
	beq _020E72DC
_020E72CC:
	ldrsb r0, [r2, #-1]!
	subs r1, r1, #1
	strb r0, [r8, #-1]!
	bne _020E72CC
_020E72DC:
	cmp r7, #0
	cmpeq r5, #0
	movne r0, #0x2e
	strneb r0, [r8, #-1]!
	ldrb r0, [sp, #9]
	strb r0, [r8, #-1]!
	ldrsb r0, [sp, #4]
	cmp r0, #0
	movne r0, #0x2d
	strneb r0, [r8, #-1]!
	bne _020E749C
	cmp r4, #1
	moveq r0, #0x2b
	streqb r0, [r8, #-1]!
	beq _020E749C
	cmp r4, #2
	moveq r0, #0x20
	streqb r0, [r8, #-1]!
	b _020E749C
_020E7328:
	ldrsh r3, [sp, #6]
	ldrb r2, [sp, #8]
	sub r0, r2, r3
	subs r1, r0, #1
	movmi r1, #0
	cmp r1, r7
	ble _020E7368
	sub r1, r1, r7
	add r0, sp, #4
	sub r1, r2, r1
	bl round_decimal
	ldrsh r3, [sp, #6]
	ldrb r2, [sp, #8]
	sub r0, r2, r3
	subs r1, r0, #1
	movmi r1, #0
_020E7368:
	adds r0, r3, #1
	movmi r0, #0
	ldr r3, _020E74B0 ; =0x000001FD
	add r6, r0, r1
	cmp r6, r3
	addgt sp, sp, #0x2c
	movgt r0, #0
	ldmgtia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	addgt sp, sp, #0x10
	bxgt lr
	add r3, sp, #9
	sub r6, r7, r1
	cmp r6, #0
	add r2, r3, r2
	mov sb, #0
	ble _020E73BC
	mov r3, #0x30
_020E73AC:
	add sb, sb, #1
	cmp sb, r6
	strb r3, [r8, #-1]!
	blt _020E73AC
_020E73BC:
	mov r6, #0
	b _020E73D0
_020E73C4:
	ldrsb r3, [r2, #-1]!
	add r6, r6, #1
	strb r3, [r8, #-1]!
_020E73D0:
	cmp r6, r1
	ldrltb r3, [sp, #8]
	cmplt r6, r3
	blt _020E73C4
	cmp r6, r1
	bge _020E73FC
	mov r3, #0x30
_020E73EC:
	add r6, r6, #1
	cmp r6, r1
	strb r3, [r8, #-1]!
	blt _020E73EC
_020E73FC:
	cmp r7, #0
	cmpeq r5, #0
	movne r1, #0x2e
	strneb r1, [r8, #-1]!
	cmp r0, #0
	beq _020E7464
	ldrb r1, [sp, #8]
	mov r5, #0
	sub r1, r0, r1
	cmp r1, #0
	ble _020E7444
	mov r3, #0x30
_020E742C:
	strb r3, [r8, #-1]!
	ldrb r1, [sp, #8]
	add r5, r5, #1
	sub r1, r0, r1
	cmp r5, r1
	blt _020E742C
_020E7444:
	cmp r5, r0
	bge _020E746C
_020E744C:
	ldrsb r1, [r2, #-1]!
	add r5, r5, #1
	cmp r5, r0
	strb r1, [r8, #-1]!
	blt _020E744C
	b _020E746C
_020E7464:
	mov r0, #0x30
	strb r0, [r8, #-1]!
_020E746C:
	ldrsb r0, [sp, #4]
	cmp r0, #0
	movne r0, #0x2d
	strneb r0, [r8, #-1]!
	bne _020E749C
	cmp r4, #1
	moveq r0, #0x2b
	streqb r0, [r8, #-1]!
	beq _020E749C
	cmp r4, #2
	moveq r0, #0x20
	streqb r0, [r8, #-1]!
_020E749C:
	mov r0, r8
	add sp, sp, #0x2c
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	add sp, sp, #0x10
	bx lr
	.align 2, 0
_020E74B0: .word 0x000001FD
_020E74B4: .word __msl_digit
_020E74B8: .word printf_double2hex_neg_inf_upper
_020E74BC: .word printf_double2hex_neg_inf_lower
_020E74C0: .word printf_double2hex_inf_upper
_020E74C4: .word printf_double2hex_inf_lower
_020E74C8: .word printf_double2hex_neg_nan_upper
_020E74CC: .word printf_double2hex_neg_nan_lower
_020E74D0: .word printf_double2hex_nan_upper
_020E74D4: .word printf_double2hex_nan_lower
_020E74D8: .word 0x66666667
	arm_func_end float2str
