	.extern buff_ptr$6E40
	.extern double2hex__wide
	.extern long2str__wide
	.extern longlong2str__wide
	.extern parse_format__wide
	.extern round_decimal__wide
	.extern strptr$6E44
	.extern wstr$6DE4
	.extern wstr$6DF0
	.extern wstr$6DFC
	.extern wstr$6E08
	.extern wstr$6E10
	.extern wstr$6E18
	.extern wstr$6E24
	.extern wstr$6E30
	.extern wstr$6E38
	.include "asm/macros.inc"
	.include "MSL_Common_wprintf.inc"
	.include "global.inc"
	.public float2str__wide

.text
	arm_func_start float2str__wide
float2str__wide: ; 0x020EC870
	stmdb sp!, {r0, r1, r2, r3}
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x22c
	mov sl, r0
	add r0, sp, #0x250
	ldr r7, [sp, #0x268]
	ldr r3, _020ECE90 ; =0x000001FD
	ldrh r6, [r0, #0x12]
	cmp r7, r3
	ldrb r5, [sp, #0x25f]
	ldrb r4, [sp, #0x25d]
	mov r8, r1
	mov sb, r2
	addgt sp, sp, #0x22c
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
	b _020EC904
_020EC8EC:
	ldrb r2, [sp, #8]
	ldrsh r1, [sp, #6]
	sub r2, r2, #1
	add r1, r1, #1
	strb r2, [sp, #8]
	strh r1, [sp, #6]
_020EC904:
	ldrb r1, [sp, #8]
	cmp r1, #1
	bls _020EC91C
	ldrsb r1, [r0, #-1]!
	cmp r1, #0x30
	beq _020EC8EC
_020EC91C:
	ldrb r0, [sp, #9]
	cmp r0, #0x30
	beq _020EC93C
	cmp r0, #0x49
	beq _020EC948
	cmp r0, #0x4e
	beq _020ECA00
	b _020ECAAC
_020EC93C:
	mov r0, #0
	strh r0, [sp, #6]
	b _020ECAAC
_020EC948:
	mov r2, #0
	mov r0, sl
	mov r1, r8
	mov r3, r2
	bl _dls
	bhs _020EC9A8
	cmp r6, #0x80
	sub r4, sb, #0xa
	movhs r0, #0
	bhs _020EC980
	ldr r0, _020ECE94 ; =__wctype_mapC
	mov r1, r6, lsl #1
	ldrh r0, [r0, r1]
	and r0, r0, #0x200
_020EC980:
	cmp r0, #0
	beq _020EC998
	ldr r1, _020ECE98 ; =wstr$6DF0
	mov r0, r4
	bl wcscpy
	b _020EC9EC
_020EC998:
	ldr r1, _020ECE9C ; =wstr$6DFC
	mov r0, r4
	bl wcscpy
	b _020EC9EC
_020EC9A8:
	cmp r6, #0x80
	sub r4, sb, #8
	movhs r0, #0
	bhs _020EC9C8
	ldr r0, _020ECE94 ; =__wctype_mapC
	mov r1, r6, lsl #1
	ldrh r0, [r0, r1]
	and r0, r0, #0x200
_020EC9C8:
	cmp r0, #0
	beq _020EC9E0
	ldr r1, _020ECEA0 ; =wstr$6E08
	mov r0, r4
	bl wcscpy
	b _020EC9EC
_020EC9E0:
	ldr r1, _020ECEA4 ; =wstr$6E10
	mov r0, r4
	bl wcscpy
_020EC9EC:
	add sp, sp, #0x22c
	mov r0, r4
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	add sp, sp, #0x10
	bx lr
_020ECA00:
	ldrsb r0, [sp, #4]
	cmp r0, #0
	beq _020ECA54
	cmp r6, #0x80
	sub r4, sb, #0xa
	movhs r0, #0
	bhs _020ECA2C
	ldr r0, _020ECE94 ; =__wctype_mapC
	mov r1, r6, lsl #1
	ldrh r0, [r0, r1]
	and r0, r0, #0x200
_020ECA2C:
	cmp r0, #0
	beq _020ECA44
	ldr r1, _020ECEA8 ; =wstr$6E18
	mov r0, r4
	bl wcscpy
	b _020ECA98
_020ECA44:
	ldr r1, _020ECEAC ; =wstr$6E24
	mov r0, r4
	bl wcscpy
	b _020ECA98
_020ECA54:
	cmp r6, #0x80
	sub r4, sb, #8
	movhs r0, #0
	bhs _020ECA74
	ldr r0, _020ECE94 ; =__wctype_mapC
	mov r1, r6, lsl #1
	ldrh r0, [r0, r1]
	and r0, r0, #0x200
_020ECA74:
	cmp r0, #0
	beq _020ECA8C
	ldr r1, _020ECEB0 ; =wstr$6E30
	mov r0, r4
	bl wcscpy
	b _020ECA98
_020ECA8C:
	ldr r1, _020ECEB4 ; =wstr$6E38
	mov r0, r4
	bl wcscpy
_020ECA98:
	add sp, sp, #0x22c
	mov r0, r4
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	add sp, sp, #0x10
	bx lr
_020ECAAC:
	ldrb r1, [sp, #8]
	ldrsh r2, [sp, #6]
	add r0, sp, #0x200
	sub r1, r1, #1
	add r0, r0, #0x2a
	add r1, r2, r1
	sub r8, r0, #1
	strh r1, [sp, #6]
	mov r0, #0
	strb r0, [r8]
	cmp r6, #0x65
	bgt _020ECB08
	bge _020ECB94
	cmp r6, #0x47
	bgt _020ECE54
	cmp r6, #0x45
	blt _020ECE54
	beq _020ECB94
	cmp r6, #0x46
	beq _020ECCE0
	cmp r6, #0x47
	beq _020ECB20
	b _020ECE54
_020ECB08:
	cmp r6, #0x66
	bgt _020ECB18
	beq _020ECCE0
	b _020ECE54
_020ECB18:
	cmp r6, #0x67
	bne _020ECE54
_020ECB20:
	ldrb r0, [sp, #8]
	cmp r0, r7
	ble _020ECB38
	add r0, sp, #4
	mov r1, r7
	bl round_decimal__wide
_020ECB38:
	ldrsh r2, [sp, #6]
	mvn r0, #3
	cmp r2, r0
	blt _020ECB50
	cmp r2, r7
	blt _020ECB70
_020ECB50:
	cmp r5, #0
	ldreqb r0, [sp, #8]
	subne r7, r7, #1
	subeq r7, r0, #1
	cmp r6, #0x67
	moveq r6, #0x65
	movne r6, #0x45
	b _020ECB94
_020ECB70:
	cmp r5, #0
	addne r0, r2, #1
	subne r7, r7, r0
	bne _020ECCE0
	ldrb r1, [sp, #8]
	add r0, r2, #1
	subs r7, r1, r0
	movmi r7, #0
	b _020ECCE0
_020ECB94:
	ldrb r0, [sp, #8]
	add r1, r7, #1
	cmp r0, r1
	ble _020ECBAC
	add r0, sp, #4
	bl round_decimal__wide
_020ECBAC:
	ldrsh lr, [sp, #6]
	mov fp, #0x2b
	mov sl, #0
	cmp lr, #0
	rsblt lr, lr, #0
	movlt fp, #0x2d
	ldr r3, _020ECEB8 ; =0x66666667
	mov r0, #0xa
	b _020ECC00
_020ECBD0:
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
_020ECC00:
	cmp lr, #0
	bne _020ECBD0
	cmp sl, #2
	blt _020ECBD0
	add r0, sp, #0x2a
	strb fp, [r8, #-1]
	strb r6, [r8, #-2]!
	sub r1, r0, r8
	ldr r0, _020ECE90 ; =0x000001FD
	add r1, r1, r7
	cmp r1, r0
	addgt sp, sp, #0x22c
	movgt r0, #0
	ldmgtia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	addgt sp, sp, #0x10
	bxgt lr
	ldrb r1, [sp, #8]
	add r0, r7, #1
	cmp r1, r0
	bge _020ECC70
	add r0, r7, #2
	sub r0, r0, r1
	subs r1, r0, #1
	beq _020ECC70
	mov r0, #0x30
_020ECC64:
	strb r0, [r8, #-1]!
	subs r1, r1, #1
	bne _020ECC64
_020ECC70:
	ldrb r1, [sp, #8]
	add r0, sp, #9
	add r2, r0, r1
	subs r1, r1, #1
	beq _020ECC94
_020ECC84:
	ldrsb r0, [r2, #-1]!
	subs r1, r1, #1
	strb r0, [r8, #-1]!
	bne _020ECC84
_020ECC94:
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
	bne _020ECE54
	cmp r4, #1
	moveq r0, #0x2b
	streqb r0, [r8, #-1]!
	beq _020ECE54
	cmp r4, #2
	moveq r0, #0x20
	streqb r0, [r8, #-1]!
	b _020ECE54
_020ECCE0:
	ldrsh r3, [sp, #6]
	ldrb r2, [sp, #8]
	sub r0, r2, r3
	subs r1, r0, #1
	movmi r1, #0
	cmp r1, r7
	ble _020ECD20
	sub r1, r1, r7
	add r0, sp, #4
	sub r1, r2, r1
	bl round_decimal__wide
	ldrsh r3, [sp, #6]
	ldrb r2, [sp, #8]
	sub r0, r2, r3
	subs r1, r0, #1
	movmi r1, #0
_020ECD20:
	adds r0, r3, #1
	movmi r0, #0
	ldr r3, _020ECE90 ; =0x000001FD
	add r6, r0, r1
	cmp r6, r3
	addgt sp, sp, #0x22c
	movgt r0, #0
	ldmgtia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	addgt sp, sp, #0x10
	bxgt lr
	add r3, sp, #9
	sub r6, r7, r1
	cmp r6, #0
	add r2, r3, r2
	mov sl, #0
	ble _020ECD74
	mov r3, #0x30
_020ECD64:
	add sl, sl, #1
	cmp sl, r6
	strb r3, [r8, #-1]!
	blt _020ECD64
_020ECD74:
	mov r6, #0
	b _020ECD88
_020ECD7C:
	ldrsb r3, [r2, #-1]!
	add r6, r6, #1
	strb r3, [r8, #-1]!
_020ECD88:
	cmp r6, r1
	ldrltb r3, [sp, #8]
	cmplt r6, r3
	blt _020ECD7C
	cmp r6, r1
	bge _020ECDB4
	mov r3, #0x30
_020ECDA4:
	add r6, r6, #1
	cmp r6, r1
	strb r3, [r8, #-1]!
	blt _020ECDA4
_020ECDB4:
	cmp r7, #0
	cmpeq r5, #0
	movne r1, #0x2e
	strneb r1, [r8, #-1]!
	cmp r0, #0
	beq _020ECE1C
	ldrb r1, [sp, #8]
	mov r5, #0
	sub r1, r0, r1
	cmp r1, #0
	ble _020ECDFC
	mov r3, #0x30
_020ECDE4:
	strb r3, [r8, #-1]!
	ldrb r1, [sp, #8]
	add r5, r5, #1
	sub r1, r0, r1
	cmp r5, r1
	blt _020ECDE4
_020ECDFC:
	cmp r5, r0
	bge _020ECE24
_020ECE04:
	ldrsb r1, [r2, #-1]!
	add r5, r5, #1
	cmp r5, r0
	strb r1, [r8, #-1]!
	blt _020ECE04
	b _020ECE24
_020ECE1C:
	mov r0, #0x30
	strb r0, [r8, #-1]!
_020ECE24:
	ldrsb r0, [sp, #4]
	cmp r0, #0
	movne r0, #0x2d
	strneb r0, [r8, #-1]!
	bne _020ECE54
	cmp r4, #1
	moveq r0, #0x2b
	streqb r0, [r8, #-1]!
	beq _020ECE54
	cmp r4, #2
	moveq r0, #0x20
	streqb r0, [r8, #-1]!
_020ECE54:
	mov r0, r8
	bl strlen
	sub r1, sb, r0, lsl #1
	mov r0, r8
	sub r4, r1, #2
	bl strlen
	mov r2, r0
	mov r0, r4
	mov r1, r8
	bl mbstowcs
	mov r0, r4
	add sp, sp, #0x22c
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	add sp, sp, #0x10
	bx lr
	.align 2, 0
_020ECE90: .word 0x000001FD
_020ECE94: .word __wctype_mapC
_020ECE98: .word wstr$6DF0
_020ECE9C: .word wstr$6DFC
_020ECEA0: .word wstr$6E08
_020ECEA4: .word wstr$6E10
_020ECEA8: .word wstr$6E18
_020ECEAC: .word wstr$6E24
_020ECEB0: .word wstr$6E30
_020ECEB4: .word wstr$6E38
_020ECEB8: .word 0x66666667
	arm_func_end float2str__wide
