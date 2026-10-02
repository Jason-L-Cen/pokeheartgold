	.extern ov13_0222B898
	.extern ov13_0222B8E8
	.extern ov13_0222B90C
	.extern ov13_0222B970
	.extern ov13_0222B980
	.extern ov13_0222BADC
	.extern ov13_0222BAF4
	.extern ov13_0222C1BC
	.extern ov13_02239FF0
	.extern ov13_0223A6E4
	.extern ov13_0223AAFC
	.extern ov13_0223ABB8
	.extern ov13_0223AC4C
	.extern ov13_0223B1E0
	.extern ov13_0223F664
	.extern ov13_0223F95C
	.extern ov13_0223FA50
	.extern ov13_0223FADC
	.extern ov13_0223FC04
	.extern ov13_0223FD60
	.extern ov13_0223FDE0
	.extern ov13_0223FDF4
	.extern ov13_0223FE98
	.extern ov13_0223FF18
	.extern ov13_02240008
	.extern ov13_02240068
	.extern ov13_022400F0
	.extern ov13_02240184
	.extern ov13_022401B0
	.extern ov13_02240338
	.extern ov13_02240648
	.extern ov13_02240798
	.extern ov13_02240874
	.extern ov13_022408A0
	.extern ov13_02240B04
	.extern ov13_02240B28
	.extern ov13_02240CC0
	.extern ov13_02240D94
	.extern ov13_02240E2C
	.extern ov13_02240E74
	.extern ov13_022411DC
	.extern ov13_0224128C
	.extern ov13_022412B0
	.extern ov13_022412F4
	.extern ov13_02241330
	.extern ov13_022413E4
	.extern ov13_02241694
	.extern ov13_022416D4
	.extern ov13_02241714
	.extern ov13_02241754
	.extern ov13_022417DC
	.extern ov13_022418EC
	.extern ov13_02241A44
	.extern ov13_02241AC0
	.extern ov13_02241C68
	.extern ov13_02241D98
	.extern ov13_02241DFC
	.extern ov13_022420A0
	.extern ov13_022422E0
	.extern ov13_0224231C
	.extern ov13_02242478
	.extern ov13_02242528
	.extern ov13_022425C8
	.extern ov13_022425D0
	.extern ov13_022425F0
	.extern ov13_02244EB0
	.extern ov13_02244EB8
	.extern ov13_02244EBC
	.extern ov13_02244EBE
	.extern ov13_02244EC4
	.extern ov13_02244ED2
	.extern ov13_02244EE0
	.extern ov13_02244EF8
	.extern ov13_02244F14
	.extern ov13_02244F16
	.extern ov13_02244F30
	.extern ov13_02244F38
	.extern ov13_02244F44
	.extern ov13_02244F4C
	.extern ov13_02244F50
	.extern ov13_02244F54
	.extern ov13_02244F58
	.extern ov13_02244F5C
	.extern ov13_02244F60
	.extern ov13_02244F64
	.extern ov13_02244F68
	.extern ov13_02244F7C
	.extern ov13_02244F84
	.extern ov13_02244F88
	.extern ov13_02244F8C
	.extern ov13_02244F94
	.extern ov13_02244F98
	.extern ov13_02244F9C
	.extern ov13_02244FA4
	.extern ov13_02245060
	.extern ov13_02245062
	.extern ov13_02245134
	.extern ov13_02245208
	.extern ov13_02245210
	.extern ov13_02245214
	.extern ov13_02245218
	.extern ov13_0224521C
	.extern ov13_02245220
	.extern ov13_02245224
	.extern ov13_02245228
	.extern ov13_02245230
	.extern ov13_02245234
	.extern ov13_02245250
	.extern ov13_02245258
	.extern ov13_02245260
	.extern ov13_0224527A
	.extern ov13_022452A2
	.extern ov13_022452A4
	.extern ov13_022452DA
	.extern ov13_02245314
	.extern ov13_02245320
	.extern ov13_02245958
	.extern ov13_02245B20
	.extern ov13_02245D04
	.extern ov13_02245D54
	.extern ov13_02245D68
	.extern ov13_02245D74
	.extern ov13_02245D80
	.extern ov13_0224F464
	.extern ov13_0224F468
	.extern ov13_0224F46C
	.extern ov13_0224F470
	.extern ov13_0224F4B0
	.extern ov13_0224F4B4
	.extern ov13_0224F4B8
	.extern ov13_0224F4BC
	.extern ov13_0224F4C0
	.extern ov13_0224F4C4
	.extern ov13_0224F4C8
	.extern ov13_0224F4CC
	.extern ov13_0224F4F8
	.extern ov13_02228C08
	.extern ov13_02228C38
	.include "asm/macros.inc"
	.include "overlay_13_arm_2.inc"
	.include "global.inc"
	.public ov13_0222AEB0
	.public ov13_0222AF5C
	.extern ov13_0222AAFC
	.extern ov13_0222AE24
	.extern ov13_0222B414
	.extern ov13_0222B424

.text
	arm_func_start ov13_0222AEB0
ov13_0222AEB0: ; 0x0222AEB0
	stmdb sp!, {r3, lr}
	ldr r1, _0222AF54 ; =ov13_0224F4C0
	ldr r2, _0222AF58 ; =ov13_022452DA
	ldr r3, [r1]
	ldrsb ip, [r3, #0x63]
	add r2, r2, ip, lsl #2
	ldrsb r2, [r0, r2]
	strb r2, [r3, #0x63]
	ldr r2, [r1]
	ldrsb r1, [r2, #0x63]
	cmp r1, #0xd
	bne _0222AEF0
	cmp r0, #1
	cmpne r0, #3
	streqb ip, [r2, #0x64]
	beq _0222AF44
_0222AEF0:
	mvn r0, #0
	cmp r1, r0
	bne _0222AF1C
	ldrsb r0, [r2, #0x64]
	cmp r0, #1
	cmpne r0, #0xa
	moveq r0, #0xa
	streqb r0, [r2, #0x63]
	movne r0, #0xb
	strneb r0, [r2, #0x63]
	b _0222AF44
_0222AF1C:
	sub r0, r0, #1
	cmp r1, r0
	bne _0222AF44
	ldrsb r0, [r2, #0x64]
	cmp r0, #1
	cmpne r0, #0xa
	moveq r0, #1
	streqb r0, [r2, #0x63]
	movne r0, #2
	strneb r0, [r2, #0x63]
_0222AF44:
	bl ov13_0222AE24
	mov r0, #8
	bl ov13_0223ABB8
	ldmia sp!, {r3, pc}
	.balign 4, 0
_0222AF54: .word ov13_0224F4C0
_0222AF58: .word ov13_022452DA
	arm_func_end ov13_0222AEB0

	arm_func_start ov13_0222AF5C
ov13_0222AF5C: ; 0x0222AF5C
	stmdb sp!, {r4, lr}
	sub sp, sp, #8
	ldr r1, _0222AFC8 ; =ov13_0224F4C0
	mov r4, r0
	ldr r0, [r1]
	mov r1, #0
	ldr r0, [r0, #0x40]
	bl ov13_0223FDE0
	ldr r2, [r0]
	ldr r0, _0222AFCC ; =0x01FF0000
	and r1, r2, #0xff
	and r0, r2, r0
	mov r2, r0, lsr #0x10
	add r1, r1, #0xc
	str r1, [sp]
	mov r0, #4
	str r2, [sp, #4]
	bl ov13_0222AAFC
	ldr r0, [sp]
	cmp r0, #0xc0
	addlt sp, sp, #8
	ldmltia sp!, {r4, pc}
	ldr r1, _0222AFD0 ; =ov13_0222AFD4
	mov r0, r4
	bl ov13_022425C8
	add sp, sp, #8
	ldmia sp!, {r4, pc}
	.balign 4, 0
_0222AFC8: .word ov13_0224F4C0
_0222AFCC: .word 0x01FF0000
_0222AFD0: .word ov13_0222AFD4
	arm_func_end ov13_0222AF5C

	arm_func_start ov13_0222AFD4
ov13_0222AFD4: ; 0x0222AFD4
	stmdb sp!, {r4, lr}
	sub sp, sp, #8
	ldr r1, _0222B038 ; =ov13_0224F4C0
	mov r4, r0
	ldr r0, [r1]
	ldr r1, _0222B03C ; =0x01FF0000
	ldr r2, [r0, #0x34]
	mov r0, #3
	ldr r3, [r2]
	and r2, r3, #0xff
	and r3, r3, r1
	add r1, r2, #0xc
	mov r2, r3, lsr #0x10
	str r1, [sp]
	str r2, [sp, #4]
	bl ov13_0222AAFC
	ldr r0, [sp]
	cmp r0, #0xc0
	addlt sp, sp, #8
	ldmltia sp!, {r4, pc}
	ldr r1, _0222B040 ; =ov13_0222B044
	mov r0, r4
	bl ov13_022425C8
	add sp, sp, #8
	ldmia sp!, {r4, pc}
	.balign 4, 0
_0222B038: .word ov13_0224F4C0
_0222B03C: .word 0x01FF0000
_0222B040: .word ov13_0222B044
	arm_func_end ov13_0222AFD4

	arm_func_start ov13_0222B044
ov13_0222B044: ; 0x0222B044
	stmdb sp!, {r4, lr}
	sub sp, sp, #8
	ldr r1, _0222B0A8 ; =ov13_0224F4C0
	mov r4, r0
	ldr r0, [r1]
	ldr r1, _0222B0AC ; =0x01FF0000
	ldr r2, [r0, #0x28]
	mov r0, #2
	ldr r3, [r2]
	and r2, r3, #0xff
	and r3, r3, r1
	add r1, r2, #0xc
	mov r2, r3, lsr #0x10
	str r1, [sp]
	str r2, [sp, #4]
	bl ov13_0222AAFC
	ldr r0, [sp]
	cmp r0, #0xc0
	addlt sp, sp, #8
	ldmltia sp!, {r4, pc}
	ldr r1, _0222B0B0 ; =ov13_0222B0B4
	mov r0, r4
	bl ov13_022425C8
	add sp, sp, #8
	ldmia sp!, {r4, pc}
	.balign 4, 0
_0222B0A8: .word ov13_0224F4C0
_0222B0AC: .word 0x01FF0000
_0222B0B0: .word ov13_0222B0B4
	arm_func_end ov13_0222B044

	arm_func_start ov13_0222B0B4
ov13_0222B0B4: ; 0x0222B0B4
	stmdb sp!, {r4, lr}
	sub sp, sp, #8
	ldr r1, _0222B118 ; =ov13_0224F4C0
	mov r4, r0
	ldr r0, [r1]
	ldr r1, _0222B11C ; =0x01FF0000
	ldr r2, [r0, #0x1c]
	mov r0, #1
	ldr r3, [r2]
	and r2, r3, #0xff
	and r3, r3, r1
	add r1, r2, #0xc
	mov r2, r3, lsr #0x10
	str r1, [sp]
	str r2, [sp, #4]
	bl ov13_0222AAFC
	ldr r0, [sp]
	cmp r0, #0xc0
	addlt sp, sp, #8
	ldmltia sp!, {r4, pc}
	ldr r1, _0222B120 ; =ov13_0222B124
	mov r0, r4
	bl ov13_022425C8
	add sp, sp, #8
	ldmia sp!, {r4, pc}
	.balign 4, 0
_0222B118: .word ov13_0224F4C0
_0222B11C: .word 0x01FF0000
_0222B120: .word ov13_0222B124
	arm_func_end ov13_0222B0B4

	arm_func_start ov13_0222B124
ov13_0222B124: ; 0x0222B124
	stmdb sp!, {r4, lr}
	sub sp, sp, #8
	ldr r1, _0222B188 ; =ov13_0224F4C0
	mov r4, r0
	ldr r0, [r1]
	ldr r1, _0222B18C ; =0x01FF0000
	ldr r2, [r0, #0x10]
	mov r0, #0
	ldr r3, [r2]
	and r2, r3, #0xff
	and r3, r3, r1
	add r1, r2, #0xc
	mov r2, r3, lsr #0x10
	str r1, [sp]
	str r2, [sp, #4]
	bl ov13_0222AAFC
	ldr r0, [sp]
	cmp r0, #0xc0
	addlt sp, sp, #8
	ldmltia sp!, {r4, pc}
	ldr r1, _0222B190 ; =ov13_0222B194
	mov r0, r4
	bl ov13_022425C8
	add sp, sp, #8
	ldmia sp!, {r4, pc}
	.balign 4, 0
_0222B188: .word ov13_0224F4C0
_0222B18C: .word 0x01FF0000
_0222B190: .word ov13_0222B194
	arm_func_end ov13_0222B124

	arm_func_start ov13_0222B194
ov13_0222B194: ; 0x0222B194
	stmdb sp!, {r3, r4, r5, lr}
	mov r1, r0
	mov r0, #0
	bl ov13_022425D0
	ldr r4, _0222B24C ; =ov13_0224F4C0
	mov r5, #0
_0222B1AC:
	ldr r0, [r4]
	add r0, r0, r5, lsl #2
	ldr r0, [r0, #0x48]
	bl ov13_0223FD60
	ldr r0, [r4]
	ldr r0, [r0, r5, lsl #2]
	bl ov13_02240874
	add r5, r5, #1
	cmp r5, #4
	blt _0222B1AC
	ldr r4, _0222B24C ; =ov13_0224F4C0
	mov r5, #0
_0222B1DC:
	ldr r0, [r4]
	add r0, r0, r5, lsl #2
	ldr r0, [r0, #0x40]
	bl ov13_0223FD60
	add r5, r5, #1
	cmp r5, #2
	blt _0222B1DC
	ldr r4, _0222B24C ; =ov13_0224F4C0
	mov r5, #0
_0222B200:
	ldr r0, [r4]
	add r0, r0, r5, lsl #2
	ldr r0, [r0, #0x38]
	bl ov13_02241D98
	add r5, r5, #1
	cmp r5, #2
	blt _0222B200
	ldr r4, _0222B24C ; =ov13_0224F4C0
	mov r5, #0
_0222B224:
	ldr r0, [r4]
	add r0, r0, r5, lsl #2
	ldr r0, [r0, #0x10]
	bl ov13_02241D98
	add r5, r5, #1
	cmp r5, #0xa
	blt _0222B224
	ldr r0, _0222B250 ; =ov13_0224F4C0
	bl ov13_022412B0
	ldmia sp!, {r3, r4, r5, pc}
	.balign 4, 0
_0222B24C: .word ov13_0224F4C0
_0222B250: .word ov13_0224F4C0
	arm_func_end ov13_0222B194

	arm_func_start ov13_0222B254
ov13_0222B254: ; 0x0222B254
	stmdb sp!, {lr}
	sub sp, sp, #0x104
	mov r0, #0x26c
	mov r1, #4
	bl ov13_0224128C
	ldr r3, _0222B354 ; =ov13_0224F4C4
	add r1, sp, #0
	str r0, [r3, #4]
	mov r0, #0
	mov r2, #0x104
	strb r0, [r3]
	bl MIi_CpuClear16
	mov r3, #0x50
	mov r2, #0xc
	ldr r0, _0222B358 ; =ov13_02245320
	add r1, sp, #4
	strb r3, [sp]
	strh r2, [sp, #2]
	bl MI_CpuCopy8
	ldr r0, _0222B354 ; =ov13_0224F4C4
	mov r2, #3
	ldr r1, [r0, #4]
	add ip, sp, #0
	strh r2, [r1]
	ldr lr, [r0, #4]
	mov r2, #0x41
	add r3, lr, #2
_0222B2C0:
	ldrh r1, [ip]
	ldrh r0, [ip, #2]
	add ip, ip, #4
	subs r2, r2, #1
	strh r1, [r3]
	strh r0, [r3, #2]
	add r3, r3, #4
	bne _0222B2C0
	ldr r1, _0222B354 ; =ov13_0224F4C4
	mov r3, #1
	add r0, lr, #0x100
	strh r3, [r0, #6]
	ldr r0, [r1, #4]
	sub r2, r3, #2
	add r0, r0, #0x100
	strh r2, [r0, #8]
	ldr r0, [r1, #4]
	add r0, r0, #0x100
	strh r3, [r0, #0xa]
	ldr r0, [r1, #4]
	add r0, r0, #0x100
	strh r2, [r0, #0xc]
	ldr r0, [r1, #4]
	add r0, r0, #0x100
	strh r2, [r0, #0xe]
	ldr r0, [r1, #4]
	add r0, r0, #0x110
	bl OS_GetMacAddress
	ldr r0, _0222B35C ; =ov13_0222B414
	ldr r1, _0222B360 ; =ov13_0222B424
	bl ov13_02222C1C
	cmp r0, #0
	addeq sp, sp, #0x104
	ldmeqia sp!, {pc}
	bl OS_Terminate
	add sp, sp, #0x104
	ldmia sp!, {pc}
	.balign 4, 0
_0222B354: .word ov13_0224F4C4
_0222B358: .word ov13_02245320
_0222B35C: .word ov13_0222B414
_0222B360: .word ov13_0222B424
	arm_func_end ov13_0222B254
