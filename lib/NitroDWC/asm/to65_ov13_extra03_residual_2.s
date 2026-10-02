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
	.public ov13_02229594
	.extern ov13_022290F0
	.extern ov13_022294B8
	.extern ov13_0222A0DC
	.extern ov13_0222AAFC

.text
	arm_func_start ov13_02229594
ov13_02229594: ; 0x02229594
	stmdb sp!, {r3, lr}
	ldr r2, _022299C4 ; =ov13_0224F4BC
	ldr r3, _022299C8 ; =ov13_02245134
	ldr ip, [r2]
	add r1, ip, #0x100
	ldrsb lr, [r1, #0x21]
	add r1, r3, lr, lsl #2
	ldrsb r1, [r0, r1]
	strb r1, [ip, #0x121]
	ldr r1, [r2]
	add r2, r1, #0x100
	ldrsb r3, [r2, #0x21]
	cmp r3, #0x2e
	cmpeq r0, #3
	streqb lr, [r1, #0x120]
	beq _022299B4
	cmp r3, #0x33
	bne _022295EC
	cmp r0, #1
	cmpne r0, #3
	streqb lr, [r1, #0x120]
	beq _022299B4
_022295EC:
	cmp r3, #0x34
	bne _0222960C
	cmp r0, #1
	cmpne r0, #3
	bne _0222960C
	cmp lr, #0x2e
	strneb lr, [r1, #0x120]
	b _022299B4
_0222960C:
	mvn r2, #0
	cmp r3, r2
	bne _0222963C
	add r0, r1, #0x100
	ldrsb r0, [r0, #0x20]
	cmp r0, #0x23
	cmpne r0, #0x32
	moveq r0, #0x23
	streqb r0, [r1, #0x121]
	movne r0, #0x22
	strneb r0, [r1, #0x121]
	b _022299B4
_0222963C:
	sub r0, r2, #1
	cmp r3, r0
	bne _022296FC
	add r0, r1, #0x100
	ldrsb r0, [r0, #0x20]
	cmp r0, #0x26
	bgt _02229694
	bge _022296CC
	cmp r0, #5
	bgt _02229688
	cmp r0, #0
	addge pc, pc, r0, lsl #2
	b _022296F0
_02229670: ; jump table
	b _022296B4 ; case 0
	b _022296C0 ; case 1
	b _022296F0 ; case 2
	b _022296CC ; case 3
	b _022296D8 ; case 4
	b _022296E4 ; case 5
_02229688:
	cmp r0, #0x24
	beq _022296C0
	b _022296F0
_02229694:
	cmp r0, #0x28
	bgt _022296AC
	bge _022296E4
	cmp r0, #0x27
	beq _022296D8
	b _022296F0
_022296AC:
	cmp r0, #0x31
	bne _022296F0
_022296B4:
	mov r0, #0x31
	strb r0, [r1, #0x121]
	b _022299B4
_022296C0:
	mov r0, #0x24
	strb r0, [r1, #0x121]
	b _022299B4
_022296CC:
	mov r0, #0x26
	strb r0, [r1, #0x121]
	b _022299B4
_022296D8:
	mov r0, #0x27
	strb r0, [r1, #0x121]
	b _022299B4
_022296E4:
	mov r0, #0x28
	strb r0, [r1, #0x121]
	b _022299B4
_022296F0:
	mov r0, #0x25
	strb r0, [r1, #0x121]
	b _022299B4
_022296FC:
	sub r0, r2, #2
	cmp r3, r0
	bne _022297F4
	add r0, r1, #0x100
	ldrsb r0, [r0, #0x20]
	cmp r0, #0x23
	bgt _0222976C
	bge _022297DC
	cmp r0, #0xb
	bgt _02229760
	cmp r0, #0
	addge pc, pc, r0, lsl #2
	b _022297E8
_02229730: ; jump table
	b _022297E8 ; case 0
	b _022297E8 ; case 1
	b _022297E8 ; case 2
	b _022297E8 ; case 3
	b _022297E8 ; case 4
	b _022297E8 ; case 5
	b _022297AC ; case 6
	b _022297B8 ; case 7
	b _022297E8 ; case 8
	b _022297C4 ; case 9
	b _022297D0 ; case 10
	b _022297DC ; case 11
_02229760:
	cmp r0, #0x22
	beq _022297DC
	b _022297E8
_0222976C:
	cmp r0, #0x2a
	bgt _02229784
	bge _022297B8
	cmp r0, #0x29
	beq _022297AC
	b _022297E8
_02229784:
	cmp r0, #0x32
	bgt _022297E8
	cmp r0, #0x2c
	blt _022297E8
	beq _022297C4
	cmp r0, #0x2d
	beq _022297D0
	cmp r0, #0x32
	beq _022297DC
	b _022297E8
_022297AC:
	mov r0, #0x29
	strb r0, [r1, #0x121]
	b _022299B4
_022297B8:
	mov r0, #0x2a
	strb r0, [r1, #0x121]
	b _022299B4
_022297C4:
	mov r0, #0x2c
	strb r0, [r1, #0x121]
	b _022299B4
_022297D0:
	mov r0, #0x2d
	strb r0, [r1, #0x121]
	b _022299B4
_022297DC:
	mov r0, #0x2e
	strb r0, [r1, #0x121]
	b _022299B4
_022297E8:
	mov r0, #0x2b
	strb r0, [r1, #0x121]
	b _022299B4
_022297F4:
	sub r0, r2, #3
	cmp r3, r0
	bne _022298B4
	add r0, r1, #0x100
	ldrsb r0, [r0, #0x20]
	cmp r0, #0x26
	bgt _0222984C
	bge _02229884
	cmp r0, #5
	bgt _02229840
	cmp r0, #0
	addge pc, pc, r0, lsl #2
	b _022298A8
_02229828: ; jump table
	b _0222986C ; case 0
	b _02229878 ; case 1
	b _022298A8 ; case 2
	b _02229884 ; case 3
	b _02229890 ; case 4
	b _0222989C ; case 5
_02229840:
	cmp r0, #0x24
	beq _02229878
	b _022298A8
_0222984C:
	cmp r0, #0x28
	bgt _02229864
	bge _0222989C
	cmp r0, #0x27
	beq _02229890
	b _022298A8
_02229864:
	cmp r0, #0x31
	bne _022298A8
_0222986C:
	mov r0, #0
	strb r0, [r1, #0x121]
	b _022299B4
_02229878:
	mov r0, #1
	strb r0, [r1, #0x121]
	b _022299B4
_02229884:
	mov r0, #3
	strb r0, [r1, #0x121]
	b _022299B4
_02229890:
	mov r0, #4
	strb r0, [r1, #0x121]
	b _022299B4
_0222989C:
	mov r0, #5
	strb r0, [r1, #0x121]
	b _022299B4
_022298A8:
	mov r0, #2
	strb r0, [r1, #0x121]
	b _022299B4
_022298B4:
	sub r0, r2, #4
	cmp r3, r0
	bne _022299B4
	add r0, r1, #0x100
	ldrsb r0, [r0, #0x20]
	cmp r0, #0x23
	bgt _02229924
	bge _022299A0
	cmp r0, #0xb
	bgt _02229918
	cmp r0, #0
	addge pc, pc, r0, lsl #2
	b _022299AC
_022298E8: ; jump table
	b _022299AC ; case 0
	b _022299AC ; case 1
	b _022299AC ; case 2
	b _022299AC ; case 3
	b _022299AC ; case 4
	b _022299AC ; case 5
	b _02229964 ; case 6
	b _02229970 ; case 7
	b _022299AC ; case 8
	b _0222997C ; case 9
	b _02229988 ; case 10
	b _02229994 ; case 11
_02229918:
	cmp r0, #0x22
	beq _02229994
	b _022299AC
_02229924:
	cmp r0, #0x2a
	bgt _0222993C
	bge _02229970
	cmp r0, #0x29
	beq _02229964
	b _022299AC
_0222993C:
	cmp r0, #0x32
	bgt _022299AC
	cmp r0, #0x2c
	blt _022299AC
	beq _0222997C
	cmp r0, #0x2d
	beq _02229988
	cmp r0, #0x32
	beq _022299A0
	b _022299AC
_02229964:
	mov r0, #6
	strb r0, [r1, #0x121]
	b _022299B4
_02229970:
	mov r0, #7
	strb r0, [r1, #0x121]
	b _022299B4
_0222997C:
	mov r0, #9
	strb r0, [r1, #0x121]
	b _022299B4
_02229988:
	mov r0, #0xa
	strb r0, [r1, #0x121]
	b _022299B4
_02229994:
	mov r0, #0xb
	strb r0, [r1, #0x121]
	b _022299B4
_022299A0:
	mov r0, #0x32
	strb r0, [r1, #0x121]
	b _022299B4
_022299AC:
	mov r0, #8
	strb r0, [r1, #0x121]
_022299B4:
	bl ov13_022294B8
	mov r0, #8
	bl ov13_0223ABB8
	ldmia sp!, {r3, pc}
	.balign 4, 0
_022299C4: .word ov13_0224F4BC
_022299C8: .word ov13_02245134
	arm_func_end ov13_02229594

	arm_func_start ov13_022299CC
ov13_022299CC: ; 0x022299CC
	stmdb sp!, {r4, lr}
	sub sp, sp, #8
	ldr r1, _02229A44 ; =ov13_0224F4BC
	mov r4, r0
	ldr r0, [r1]
	mov r1, #0
	ldr r0, [r0, #0xfc]
	bl ov13_0223FDE0
	ldr r3, [r0]
	ldr r1, _02229A44 ; =ov13_0224F4BC
	ldr r0, _02229A48 ; =0x01FF0000
	and r2, r3, #0xff
	and r0, r3, r0
	mov r3, r0, lsr #0x10
	ldr r1, [r1]
	add r2, r2, #0xc
	ldrb r0, [r1, #0x11d]
	str r2, [sp]
	mov r1, #4
	str r3, [sp, #4]
	bl ov13_022290F0
	ldr r0, [sp]
	cmp r0, #0xc0
	addlt sp, sp, #8
	ldmltia sp!, {r4, pc}
	ldr r1, _02229A4C ; =ov13_02229A50
	mov r0, r4
	bl ov13_022425C8
	add sp, sp, #8
	ldmia sp!, {r4, pc}
	.balign 4, 0
_02229A44: .word ov13_0224F4BC
_02229A48: .word 0x01FF0000
_02229A4C: .word ov13_02229A50
	arm_func_end ov13_022299CC

	arm_func_start ov13_02229A50
ov13_02229A50: ; 0x02229A50
	stmdb sp!, {r4, lr}
	sub sp, sp, #8
	ldr r1, _02229AB8 ; =ov13_0224F4BC
	ldr r2, _02229ABC ; =0x01FF0000
	ldr r3, [r1]
	mov r4, r0
	ldr r1, [r3, #0xc0]
	ldrb r0, [r3, #0x11d]
	ldr ip, [r1]
	mov r1, #3
	and r3, ip, #0xff
	and ip, ip, r2
	add r2, r3, #0xc
	mov r3, ip, lsr #0x10
	str r2, [sp]
	str r3, [sp, #4]
	bl ov13_022290F0
	ldr r0, [sp]
	cmp r0, #0xc0
	addlt sp, sp, #8
	ldmltia sp!, {r4, pc}
	ldr r1, _02229AC0 ; =ov13_02229AC4
	mov r0, r4
	bl ov13_022425C8
	add sp, sp, #8
	ldmia sp!, {r4, pc}
	.balign 4, 0
_02229AB8: .word ov13_0224F4BC
_02229ABC: .word 0x01FF0000
_02229AC0: .word ov13_02229AC4
	arm_func_end ov13_02229A50

	arm_func_start ov13_02229AC4
ov13_02229AC4: ; 0x02229AC4
	stmdb sp!, {r4, lr}
	sub sp, sp, #8
	ldr r1, _02229B2C ; =ov13_0224F4BC
	ldr r2, _02229B30 ; =0x01FF0000
	ldr r3, [r1]
	mov r4, r0
	ldr r1, [r3, #0x90]
	ldrb r0, [r3, #0x11d]
	ldr ip, [r1]
	mov r1, #2
	and r3, ip, #0xff
	and ip, ip, r2
	add r2, r3, #0xc
	mov r3, ip, lsr #0x10
	str r2, [sp]
	str r3, [sp, #4]
	bl ov13_022290F0
	ldr r0, [sp]
	cmp r0, #0xc0
	addlt sp, sp, #8
	ldmltia sp!, {r4, pc}
	ldr r1, _02229B34 ; =ov13_02229B38
	mov r0, r4
	bl ov13_022425C8
	add sp, sp, #8
	ldmia sp!, {r4, pc}
	.balign 4, 0
_02229B2C: .word ov13_0224F4BC
_02229B30: .word 0x01FF0000
_02229B34: .word ov13_02229B38
	arm_func_end ov13_02229AC4

	arm_func_start ov13_02229B38
ov13_02229B38: ; 0x02229B38
	stmdb sp!, {r4, lr}
	sub sp, sp, #8
	ldr r1, _02229BA0 ; =ov13_0224F4BC
	ldr r2, _02229BA4 ; =0x01FF0000
	ldr r3, [r1]
	mov r4, r0
	ldr r1, [r3, #0x60]
	ldrb r0, [r3, #0x11d]
	ldr ip, [r1]
	mov r1, #1
	and r3, ip, #0xff
	and ip, ip, r2
	add r2, r3, #0xc
	mov r3, ip, lsr #0x10
	str r2, [sp]
	str r3, [sp, #4]
	bl ov13_022290F0
	ldr r0, [sp]
	cmp r0, #0xc0
	addlt sp, sp, #8
	ldmltia sp!, {r4, pc}
	ldr r1, _02229BA8 ; =ov13_02229BAC
	mov r0, r4
	bl ov13_022425C8
	add sp, sp, #8
	ldmia sp!, {r4, pc}
	.balign 4, 0
_02229BA0: .word ov13_0224F4BC
_02229BA4: .word 0x01FF0000
_02229BA8: .word ov13_02229BAC
	arm_func_end ov13_02229B38

	arm_func_start ov13_02229BAC
ov13_02229BAC: ; 0x02229BAC
	stmdb sp!, {r4, lr}
	sub sp, sp, #8
	ldr r1, _02229C14 ; =ov13_0224F4BC
	ldr r2, _02229C18 ; =0x01FF0000
	ldr r3, [r1]
	mov r4, r0
	ldr r1, [r3, #0x30]
	ldrb r0, [r3, #0x11d]
	ldr ip, [r1]
	mov r1, #0
	and r3, ip, #0xff
	and ip, ip, r2
	add r2, r3, #0xc
	mov r3, ip, lsr #0x10
	str r2, [sp]
	str r3, [sp, #4]
	bl ov13_022290F0
	ldr r0, [sp]
	cmp r0, #0xc0
	addlt sp, sp, #8
	ldmltia sp!, {r4, pc}
	ldr r1, _02229C1C ; =ov13_02229C20
	mov r0, r4
	bl ov13_022425C8
	add sp, sp, #8
	ldmia sp!, {r4, pc}
	.balign 4, 0
_02229C14: .word ov13_0224F4BC
_02229C18: .word 0x01FF0000
_02229C1C: .word ov13_02229C20
	arm_func_end ov13_02229BAC

	arm_func_start ov13_02229C20
ov13_02229C20: ; 0x02229C20
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	mov r1, r0
	mov r0, #0
	bl ov13_022425D0
	mov r5, #0
	ldr r7, _02229CF8 ; =ov13_0224F4BC
	mov r4, r5
_02229C3C:
	mov r6, r4
_02229C40:
	ldr r0, [r7]
	add r0, r0, r5, lsl #4
	ldr r0, [r0, r6, lsl #2]
	bl ov13_02240874
	cmp r5, #0
	bne _02229C68
	ldr r0, [r7]
	add r0, r0, r6, lsl #2
	ldr r0, [r0, #0x104]
	bl ov13_0223FD60
_02229C68:
	add r6, r6, #1
	cmp r6, #4
	blt _02229C40
	add r5, r5, #1
	cmp r5, #3
	blt _02229C3C
	ldr r4, _02229CF8 ; =ov13_0224F4BC
	mov r5, #0
_02229C88:
	ldr r0, [r4]
	add r0, r0, r5, lsl #2
	ldr r0, [r0, #0xfc]
	bl ov13_0223FD60
	add r5, r5, #1
	cmp r5, #2
	blt _02229C88
	ldr r4, _02229CF8 ; =ov13_0224F4BC
	mov r5, #0
_02229CAC:
	ldr r0, [r4]
	add r0, r0, r5, lsl #2
	ldr r0, [r0, #0xec]
	bl ov13_02241D98
	add r5, r5, #1
	cmp r5, #4
	blt _02229CAC
	ldr r4, _02229CF8 ; =ov13_0224F4BC
	mov r5, #0
_02229CD0:
	ldr r0, [r4]
	add r0, r0, r5, lsl #2
	ldr r0, [r0, #0x30]
	bl ov13_02241D98
	add r5, r5, #1
	cmp r5, #0x2f
	blt _02229CD0
	ldr r0, _02229CFC ; =ov13_0224F4BC
	bl ov13_022412B0
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.balign 4, 0
_02229CF8: .word ov13_0224F4BC
_02229CFC: .word ov13_0224F4BC
	arm_func_end ov13_02229C20

	arm_func_start ov13_02229D00
ov13_02229D00: ; 0x02229D00
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x24
	ldr r0, _0222A020 ; =ov13_02245228
	mov r3, #0x1c
	ldrh r5, [r0, #0x20]
	ldrh r4, [r0, #0x22]
	mov r2, #0x14
	mov r0, #0x6c
	mov r1, #4
	strh r5, [sp, #0x1c]
	strh r4, [sp, #0x1e]
	strh r3, [sp, #0x20]
	strh r2, [sp, #0x22]
	bl ov13_0224128C
	ldr r8, _0222A024 ; =ov13_0224F4C0
	mov r6, #0
	mov r1, #0x1f
	str r0, [r8]
	strb r1, [r0, #0x60]
	ldr r0, [r8]
	mov r1, #1
	strb r6, [r0, #0x63]
	ldr r0, [r8]
	ldr r7, _0222A028 ; =0xC1FFFCFF
	strb r1, [r0, #0x66]
	ldr r0, [r8]
	mov r5, r6
	strb r1, [r0, #0x67]
	ldr r0, [r8]
	mov r4, #0x36
	strb r1, [r0, #0x68]
_02229D7C:
	mov r0, r5
	mov r1, r4
	bl ov13_02240184
	ldr r1, [r8]
	add r1, r1, r6, lsl #2
	str r0, [r1, #0x10]
	ldr r0, [r8]
	add r0, r0, r6, lsl #2
	ldr r1, [r0, #0x10]
	ldr r0, [r1]
	and r0, r0, r7
	orr r0, r0, #0x200
	str r0, [r1]
	ldr r0, [r8]
	add r0, r0, r6, lsl #2
	ldr r1, [r0, #0x10]
	add r6, r6, #1
	ldrh r0, [r1, #4]
	cmp r6, #0xa
	bic r0, r0, #0xc00
	orr r0, r0, #0xc00
	strh r0, [r1, #4]
	blt _02229D7C
	mov r6, #0
	ldr r5, _0222A02C ; =ov13_02245214
	ldr r8, _0222A024 ; =ov13_0224F4C0
	ldr r7, _0222A028 ; =0xC1FFFCFF
	mov r4, r6
_02229DEC:
	ldrb r1, [r5], #1
	mov r0, r4
	bl ov13_02240184
	ldr r1, [r8]
	add r1, r1, r6, lsl #2
	str r0, [r1, #0x38]
	ldr r0, [r8]
	add r0, r0, r6, lsl #2
	ldr r1, [r0, #0x38]
	ldr r0, [r1]
	and r0, r0, r7
	orr r0, r0, #0x200
	str r0, [r1]
	ldr r0, [r8]
	add r0, r0, r6, lsl #2
	ldr r1, [r0, #0x38]
	add r6, r6, #1
	ldrh r0, [r1, #4]
	cmp r6, #2
	bic r0, r0, #0xc00
	orr r0, r0, #0xc00
	strh r0, [r1, #4]
	blt _02229DEC
	mov sl, #0
	ldr sb, _0222A030 ; =ov13_02245218
	ldr r4, _0222A024 ; =ov13_0224F4C0
	mov r8, sl
	mov r7, #1
	mvn r6, #0
	mov r5, #0x200
	mov fp, sl
_02229E68:
	ldrb r1, [sb], #1
	mov r0, r8
	mov r2, r7
	bl ov13_022401B0
	ldr r2, [r4]
	mov r1, r6
	add r2, r2, sl, lsl #2
	str r0, [r2, #0x40]
	ldr r0, [r4]
	mov r2, r5
	add r0, r0, sl, lsl #2
	ldr r0, [r0, #0x40]
	mov r3, fp
	bl ov13_0223FDF4
	ldr r0, [r4]
	mov r1, r6
	add r0, r0, sl, lsl #2
	ldr r0, [r0, #0x40]
	mov r2, #3
	bl ov13_02240008
	add sl, sl, #1
	cmp sl, #2
	blt _02229E68
	mov sl, #0
	str sl, [sp, #0x10]
	strh sl, [sp, #0x16]
_02229ED0:
	add r1, sp, #0x18
	mov r0, #0
	str r1, [sp]
	mov r3, r0
	mov r1, #0xc
	mov r2, #4
	str r0, [sp, #4]
	bl ov13_02240798
	ldr fp, _0222A024 ; =ov13_0224F4C0
	mov sb, #0
	ldr r1, [fp]
	ldr r8, [sp, #0x10]
	ldr r7, _0222A034 ; =ov13_02245260
	str r0, [r1, sl, lsl #2]
	strh sb, [sp, #0x1c]
	mov r6, #2
	mov r5, #0x480
	add r4, sp, #0x14
_02229F18:
	mov r0, r8, lsl #1
	ldrh r2, [r7, r0]
	ldrh r0, [sp, #0x22]
	ldrh r1, [sp, #0x1c]
	strh r2, [sp, #0x14]
	stmia sp, {r0, r6}
	str r5, [sp, #8]
	str r4, [sp, #0xc]
	ldr r0, [fp]
	ldrh r2, [sp, #0x1e]
	ldrh r3, [sp, #0x20]
	ldr r0, [r0, sl, lsl #2]
	bl ov13_02240CC0
	ldrh r0, [sp, #0x1c]
	add sb, sb, #1
	cmp sb, #3
	add r0, r0, #0x20
	strh r0, [sp, #0x1c]
	add r8, r8, #1
	blt _02229F18
	mov r0, #0
	ldr r1, [sp, #0x18]
	mov r2, r0
	bl ov13_0223FC04
	ldr r1, [sp, #0x10]
	ldr r2, _0222A024 ; =ov13_0224F4C0
	add r1, r1, #3
	str r1, [sp, #0x10]
	ldr r1, [r2]
	add r1, r1, sl, lsl #2
	add sl, sl, #1
	str r0, [r1, #0x48]
	cmp sl, #4
	blt _02229ED0
	mov r0, #0
	mov r1, #0x44
	mov r2, #1
	bl ov13_022401B0
	ldr r3, _0222A024 ; =ov13_0224F4C0
	mvn r1, #0
	ldr r4, [r3]
	mov r2, #0x200
	str r0, [r4, #0x58]
	ldr r0, [r3]
	mov r3, #0
	ldr r0, [r0, #0x58]
	bl ov13_0223FDF4
	ldr r0, _0222A024 ; =ov13_0224F4C0
	mvn r1, #0
	ldr r0, [r0]
	mov r2, #2
	ldr r0, [r0, #0x58]
	bl ov13_02240008
	mov r0, #0
	ldr r1, _0222A038 ; =ov13_0222A0DC
	mov r2, r0
	mov r3, #0x78
	bl ov13_02242528
	ldr r2, _0222A024 ; =ov13_0224F4C0
	mov r1, #0xc0
	ldr r2, [r2]
	str r0, [r2, #0x5c]
	mov r0, #0
	bl ov13_0222AAFC
	add sp, sp, #0x24
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.balign 4, 0
_0222A020: .word ov13_02245228
_0222A024: .word ov13_0224F4C0
_0222A028: .word 0xC1FFFCFF
_0222A02C: .word ov13_02245214
_0222A030: .word ov13_02245218
_0222A034: .word ov13_02245260
_0222A038: .word ov13_0222A0DC
	arm_func_end ov13_02229D00
