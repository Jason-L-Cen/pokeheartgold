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

	arm_func_start __pformatter
__pformatter: ; 0x020E74DC
	stmdb sp!, {r0, r1, r2, r3}
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x22c
	mov r3, #0x20
	mov fp, r2
	strb r3, [sp, #0x19]
	ldrsb r2, [fp]
	mov sb, r0
	mov r8, r1
	cmp r2, #0
	mov sl, #0
	beq _020E7CE4
_020E750C:
	mov r0, fp
	mov r1, #0x25
	bl strchr
	str r0, [sp, #0xc]
	cmp r0, #0
	bne _020E7560
	mov r0, fp
	bl strlen
	movs r2, r0
	add sl, sl, r2
	beq _020E7CE4
	mov r0, r8
	mov r1, fp
	blx sb
	cmp r0, #0
	bne _020E7CE4
	add sp, sp, #0x22c
	mvn r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	add sp, sp, #0x10
	bx lr
_020E7560:
	subs r2, r0, fp
	add sl, sl, r2
	beq _020E7590
	mov r0, r8
	mov r1, fp
	blx sb
	cmp r0, #0
	addeq sp, sp, #0x22c
	mvneq r0, #0
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	addeq sp, sp, #0x10
	bxeq lr
_020E7590:
	ldr r0, [sp, #0xc]
	add r1, sp, #0x25c
	add r2, sp, #0x1c
	bl parse_format
	ldrb r1, [sp, #0x21]
	mov fp, r0
	cmp r1, #0x61
	bgt _020E75FC
	bge _020E7960
	cmp r1, #0x47
	bgt _020E75F0
	subs r0, r1, #0x41
	addpl pc, pc, r0, lsl #2
	b _020E75E4
_020E75C8: ; jump table
	b _020E7960 ; case 0
	b _020E7B48 ; case 1
	b _020E7B48 ; case 2
	b _020E7B48 ; case 3
	b _020E78F8 ; case 4
	b _020E78F8 ; case 5
	b _020E78F8 ; case 6
_020E75E4:
	cmp r1, #0x25
	beq _020E7B34
	b _020E7B48
_020E75F0:
	cmp r1, #0x58
	beq _020E77BC
	b _020E7B48
_020E75FC:
	cmp r1, #0x75
	bgt _020E7664
	subs r0, r1, #0x64
	addpl pc, pc, r0, lsl #2
	b _020E7658
_020E7610: ; jump table
	b _020E767C ; case 0
	b _020E78F8 ; case 1
	b _020E78F8 ; case 2
	b _020E78F8 ; case 3
	b _020E7B48 ; case 4
	b _020E767C ; case 5
	b _020E7B48 ; case 6
	b _020E7B48 ; case 7
	b _020E7B48 ; case 8
	b _020E7B48 ; case 9
	b _020E7A88 ; case 10
	b _020E77BC ; case 11
	b _020E7B48 ; case 12
	b _020E7B48 ; case 13
	b _020E7B48 ; case 14
	b _020E79C8 ; case 15
	b _020E7B48 ; case 16
	b _020E77BC ; case 17
_020E7658:
	cmp r1, #0x63
	beq _020E7B14
	b _020E7B48
_020E7664:
	cmp r1, #0x78
	bgt _020E7674
	beq _020E77BC
	b _020E7B48
_020E7674:
	cmp r1, #0xff
	b _020E7B48
_020E767C:
	ldrb r0, [sp, #0x20]
	cmp r0, #3
	bne _020E769C
	ldr r1, [sp, #0x25c]
	add r1, r1, #4
	str r1, [sp, #0x25c]
	ldr r5, [r1, #-4]
	b _020E7734
_020E769C:
	cmp r0, #4
	bne _020E76C4
	ldr r1, [sp, #0x25c]
	add r2, r1, #8
	str r2, [sp, #0x25c]
	ldr r1, [r2, #-8]
	str r1, [sp, #0x10]
	ldr r1, [r2, #-4]
	str r1, [sp, #0x14]
	b _020E7734
_020E76C4:
	cmp r0, #6
	bne _020E76EC
	ldr r1, [sp, #0x25c]
	add r2, r1, #8
	str r2, [sp, #0x25c]
	ldr r1, [r2, #-8]
	str r1, [sp, #0x10]
	ldr r1, [r2, #-4]
	str r1, [sp, #0x14]
	b _020E7734
_020E76EC:
	cmp r0, #7
	bne _020E7708
	ldr r1, [sp, #0x25c]
	add r1, r1, #4
	str r1, [sp, #0x25c]
	ldr r5, [r1, #-4]
	b _020E7734
_020E7708:
	cmp r0, #8
	bne _020E7724
	ldr r1, [sp, #0x25c]
	add r1, r1, #4
	str r1, [sp, #0x25c]
	ldr r5, [r1, #-4]
	b _020E7734
_020E7724:
	ldr r1, [sp, #0x25c]
	add r1, r1, #4
	str r1, [sp, #0x25c]
	ldr r5, [r1, #-4]
_020E7734:
	cmp r0, #2
	moveq r1, r5, lsl #0x10
	moveq r5, r1, asr #0x10
	cmp r0, #1
	moveq r1, r5, lsl #0x18
	moveq r5, r1, asr #0x18
	cmp r0, #4
	cmpne r0, #6
	add r0, sp, #0x1c
	bne _020E7788
	sub r4, sp, #4
	ldmia r0, {r0, r1, r2, r3}
	stmia r4, {r0, r1, r2, r3}
	ldr r3, [r4]
	ldr r0, [sp, #0x10]
	ldr r1, [sp, #0x14]
	add r2, sp, #0x22c
	bl longlong2str
	movs r7, r0
	beq _020E7B48
	b _020E77AC
_020E7788:
	sub r4, sp, #8
	ldmia r0, {r0, r1, r2, r3}
	stmia r4, {r0, r1, r2, r3}
	add r1, sp, #0x22c
	mov r0, r5
	ldmia r4, {r2, r3}
	bl long2str
	movs r7, r0
	beq _020E7B48
_020E77AC:
	add r0, sp, #0x200
	add r0, r0, #0x2b
	sub r6, r0, r7
	b _020E7B94
_020E77BC:
	ldrb r0, [sp, #0x20]
	cmp r0, #3
	bne _020E77DC
	ldr r1, [sp, #0x25c]
	add r1, r1, #4
	str r1, [sp, #0x25c]
	ldr r5, [r1, #-4]
	b _020E7874
_020E77DC:
	cmp r0, #4
	bne _020E7804
	ldr r1, [sp, #0x25c]
	add r2, r1, #8
	str r2, [sp, #0x25c]
	ldr r1, [r2, #-8]
	str r1, [sp, #0x10]
	ldr r1, [r2, #-4]
	str r1, [sp, #0x14]
	b _020E7874
_020E7804:
	cmp r0, #6
	bne _020E782C
	ldr r1, [sp, #0x25c]
	add r2, r1, #8
	str r2, [sp, #0x25c]
	ldr r1, [r2, #-8]
	str r1, [sp, #0x10]
	ldr r1, [r2, #-4]
	str r1, [sp, #0x14]
	b _020E7874
_020E782C:
	cmp r0, #7
	bne _020E7848
	ldr r1, [sp, #0x25c]
	add r1, r1, #4
	str r1, [sp, #0x25c]
	ldr r5, [r1, #-4]
	b _020E7874
_020E7848:
	cmp r0, #8
	bne _020E7864
	ldr r1, [sp, #0x25c]
	add r1, r1, #4
	str r1, [sp, #0x25c]
	ldr r5, [r1, #-4]
	b _020E7874
_020E7864:
	ldr r1, [sp, #0x25c]
	add r1, r1, #4
	str r1, [sp, #0x25c]
	ldr r5, [r1, #-4]
_020E7874:
	cmp r0, #2
	moveq r1, r5, lsl #0x10
	moveq r5, r1, lsr #0x10
	cmp r0, #1
	andeq r5, r5, #0xff
	cmp r0, #4
	cmpne r0, #6
	add r0, sp, #0x1c
	bne _020E78C4
	sub r4, sp, #4
	ldmia r0, {r0, r1, r2, r3}
	stmia r4, {r0, r1, r2, r3}
	ldr r3, [r4]
	ldr r0, [sp, #0x10]
	ldr r1, [sp, #0x14]
	add r2, sp, #0x22c
	bl longlong2str
	movs r7, r0
	beq _020E7B48
	b _020E78E8
_020E78C4:
	sub r4, sp, #8
	ldmia r0, {r0, r1, r2, r3}
	stmia r4, {r0, r1, r2, r3}
	add r1, sp, #0x22c
	mov r0, r5
	ldmia r4, {r2, r3}
	bl long2str
	movs r7, r0
	beq _020E7B48
_020E78E8:
	add r0, sp, #0x200
	add r0, r0, #0x2b
	sub r6, r0, r7
	b _020E7B94
_020E78F8:
	ldrb r0, [sp, #0x20]
	cmp r0, #9
	ldrne r0, [sp, #0x25c]
	addne r0, r0, #8
	strne r0, [sp, #0x25c]
	bne _020E791C
	ldr r0, [sp, #0x25c]
	add r0, r0, #8
	str r0, [sp, #0x25c]
_020E791C:
	ldr r7, [r0, #-8]
	ldr r6, [r0, #-4]
	add r0, sp, #0x1c
	sub r4, sp, #4
	ldmia r0, {r0, r1, r2, r3}
	stmia r4, {r0, r1, r2, r3}
	ldr r3, [r4]
	mov r0, r7
	mov r1, r6
	add r2, sp, #0x22c
	bl float2str
	movs r7, r0
	beq _020E7B48
	add r0, sp, #0x200
	add r0, r0, #0x2b
	sub r6, r0, r7
	b _020E7B94
_020E7960:
	ldrb r0, [sp, #0x20]
	cmp r0, #9
	ldrne r0, [sp, #0x25c]
	addne r0, r0, #8
	strne r0, [sp, #0x25c]
	bne _020E7984
	ldr r0, [sp, #0x25c]
	add r0, r0, #8
	str r0, [sp, #0x25c]
_020E7984:
	ldr r7, [r0, #-8]
	ldr r6, [r0, #-4]
	add r0, sp, #0x1c
	sub r4, sp, #4
	ldmia r0, {r0, r1, r2, r3}
	stmia r4, {r0, r1, r2, r3}
	ldr r3, [r4]
	mov r0, r7
	mov r1, r6
	add r2, sp, #0x22c
	bl double2hex
	movs r7, r0
	beq _020E7B48
	add r0, sp, #0x200
	add r0, r0, #0x2b
	sub r6, r0, r7
	b _020E7B94
_020E79C8:
	ldrb r0, [sp, #0x20]
	cmp r0, #5
	bne _020E7A08
	ldr r0, [sp, #0x25c]
	mov r2, #0x200
	add r0, r0, #4
	str r0, [sp, #0x25c]
	ldr r1, [r0, #-4]
	add r0, sp, #0x2c
	cmp r1, #0
	ldreq r1, _020E7CF8 ; =__pformatter_empty_string_1
	bl wcstombs
	cmp r0, #0
	blt _020E7B48
	add r7, sp, #0x2c
	b _020E7A18
_020E7A08:
	ldr r0, [sp, #0x25c]
	add r0, r0, #4
	str r0, [sp, #0x25c]
	ldr r7, [r0, #-4]
_020E7A18:
	ldrb r0, [sp, #0x1f]
	cmp r7, #0
	ldreq r7, _020E7CFC ; =__pformatter_empty_string_2
	cmp r0, #0
	beq _020E7A4C
	ldrb r0, [sp, #0x1e]
	ldrb r6, [r7], #1
	cmp r0, #0
	beq _020E7B94
	ldr r0, [sp, #0x28]
	cmp r6, r0
	movgt r6, r0
	b _020E7B94
_020E7A4C:
	ldrb r0, [sp, #0x1e]
	cmp r0, #0
	beq _020E7A78
	ldr r6, [sp, #0x28]
	mov r0, r7
	mov r2, r6
	mov r1, #0
	bl memchr
	cmp r0, #0
	subne r6, r0, r7
	b _020E7B94
_020E7A78:
	mov r0, r7
	bl strlen
	mov r6, r0
	b _020E7B94
_020E7A88:
	ldr r1, [sp, #0x25c]
	ldrb r0, [sp, #0x20]
	add r1, r1, #4
	str r1, [sp, #0x25c]
	ldr r1, [r1, #-4]
	cmp r0, #8
	addls pc, pc, r0, lsl #2
	b _020E7CD8
_020E7AA8: ; jump table
	b _020E7ACC ; case 0
	b _020E7CD8 ; case 1
	b _020E7AD4 ; case 2
	b _020E7ADC ; case 3
	b _020E7B04 ; case 4
	b _020E7CD8 ; case 5
	b _020E7AE4 ; case 6
	b _020E7AF4 ; case 7
	b _020E7AFC ; case 8
_020E7ACC:
	str sl, [r1]
	b _020E7CD8
_020E7AD4:
	strh sl, [r1]
	b _020E7CD8
_020E7ADC:
	str sl, [r1]
	b _020E7CD8
_020E7AE4:
	str sl, [r1]
	mov r0, sl, asr #0x1f
	str r0, [r1, #4]
	b _020E7CD8
_020E7AF4:
	str sl, [r1]
	b _020E7CD8
_020E7AFC:
	str sl, [r1]
	b _020E7CD8
_020E7B04:
	str sl, [r1]
	mov r0, sl, asr #0x1f
	str r0, [r1, #4]
	b _020E7CD8
_020E7B14:
	ldr r0, [sp, #0x25c]
	add r7, sp, #0x2c
	add r0, r0, #4
	str r0, [sp, #0x25c]
	ldr r0, [r0, #-4]
	mov r6, #1
	strb r0, [sp, #0x2c]
	b _020E7B94
_020E7B34:
	mov r0, #0x25
	strb r0, [sp, #0x2c]
	add r7, sp, #0x2c
	mov r6, #1
	b _020E7B94
_020E7B48:
	ldr r0, [sp, #0xc]
	bl strlen
	movs r4, r0
	beq _020E7B80
	ldr r1, [sp, #0xc]
	mov r0, r8
	mov r2, r4
	blx sb
	cmp r0, #0
	addeq sp, sp, #0x22c
	mvneq r0, #0
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	addeq sp, sp, #0x10
	bxeq lr
_020E7B80:
	add sp, sp, #0x22c
	add r0, sl, r4
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	add sp, sp, #0x10
	bx lr
_020E7B94:
	ldrb r0, [sp, #0x1c]
	mov r4, r6
	cmp r0, #0
	beq _020E7C48
	cmp r0, #2
	moveq r0, #0x30
	movne r0, #0x20
	strb r0, [sp, #0x19]
	ldrsb r0, [r7]
	cmp r0, #0x2b
	cmpne r0, #0x2d
	cmpne r0, #0x20
	bne _020E7C04
	ldrsb r0, [sp, #0x19]
	cmp r0, #0x30
	bne _020E7C04
	mov r0, r8
	mov r1, r7
	mov r2, #1
	blx sb
	cmp r0, #0
	addeq sp, sp, #0x22c
	mvneq r0, #0
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	addeq sp, sp, #0x10
	bxeq lr
	add r7, r7, #1
	sub r6, r6, #1
_020E7C04:
	ldr r0, [sp, #0x24]
	cmp r4, r0
	bge _020E7C48
_020E7C10:
	mov r0, r8
	add r1, sp, #0x19
	mov r2, #1
	blx sb
	cmp r0, #0
	addeq sp, sp, #0x22c
	mvneq r0, #0
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	addeq sp, sp, #0x10
	bxeq lr
	ldr r0, [sp, #0x24]
	add r4, r4, #1
	cmp r4, r0
	blt _020E7C10
_020E7C48:
	cmp r6, #0
	beq _020E7C78
	mov r0, r8
	mov r1, r7
	mov r2, r6
	blx sb
	cmp r0, #0
	addeq sp, sp, #0x22c
	mvneq r0, #0
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	addeq sp, sp, #0x10
	bxeq lr
_020E7C78:
	ldrb r0, [sp, #0x1c]
	cmp r0, #0
	bne _020E7CD4
	ldr r0, [sp, #0x24]
	cmp r4, r0
	bge _020E7CD4
	mov r6, #0x20
	add r7, sp, #0x18
_020E7C98:
	mov r0, r8
	mov r1, r7
	mov r2, #1
	strb r6, [sp, #0x18]
	blx sb
	cmp r0, #0
	addeq sp, sp, #0x22c
	mvneq r0, #0
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	addeq sp, sp, #0x10
	bxeq lr
	ldr r0, [sp, #0x24]
	add r4, r4, #1
	cmp r4, r0
	blt _020E7C98
_020E7CD4:
	add sl, sl, r4
_020E7CD8:
	ldrsb r0, [fp]
	cmp r0, #0
	bne _020E750C
_020E7CE4:
	mov r0, sl
	add sp, sp, #0x22c
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	add sp, sp, #0x10
	bx lr
	.align 2, 0
_020E7CF8: .word __pformatter_empty_string_1
_020E7CFC: .word __pformatter_empty_string_2
	arm_func_end __pformatter
