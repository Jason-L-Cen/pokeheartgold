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
	.include "asm/macros.inc"
	.include "overlay_13_arm_2.inc"
	.include "global.inc"
	.public ov13_02228C68
	.extern ov13_02228E6C

.text
	arm_func_start ov13_02228C68
ov13_02228C68: ; 0x02228C68
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	sub sp, sp, #8
	ldr r0, _02228E4C ; =ov13_02245958
	bl ov13_02241754
	cmp r0, #0
	beq _02228E2C
	ldr r7, _02228E50 ; =ov13_02244FA4
	ldr r5, _02228E54 ; =ov13_02244F68
	mov r6, #0
	add r4, sp, #0
_02228C90:
	mov r0, r7
	mov r1, r5
	mov r2, r4
	bl ov13_02240E74
	mov r0, r4
	bl ov13_02241754
	cmp r0, #0
	beq _02228CD4
	ldr r0, _02228E58 ; =ov13_0224F4BC
	ldr r0, [r0]
	add r0, r0, #0x100
	ldrsb r0, [r0, #0x1e]
	cmp r0, r6
	bne _02228E2C
	mov r0, r6
	bl ov13_02228E6C
	b _02228E34
_02228CD4:
	add r6, r6, #1
	cmp r6, #0x2f
	add r7, r7, #4
	blt _02228C90
	ldr r6, _02228E5C ; =ov13_02244F9C
	ldr r7, _02228E60 ; =ov13_02244F8C
	mov r4, #2
	add r5, sp, #0
_02228CF4:
	mov r0, r7
	mov r1, r6
	mov r2, r5
	bl ov13_02240E74
	mov r0, r5
	bl ov13_02241754
	cmp r0, #0
	beq _02228DB4
	ldr r1, _02228E58 ; =ov13_0224F4BC
	add r0, r4, #0x2f
	ldr r1, [r1]
	add r1, r1, #0x100
	ldrsb r1, [r1, #0x1e]
	cmp r1, r0
	bne _02228E2C
	bl ov13_02228E6C
	cmp r4, #3
	addne sp, sp, #8
	ldmneia sp!, {r3, r4, r5, r6, r7, pc}
	ldr r0, _02228E58 ; =ov13_0224F4BC
	ldr r2, [r0]
	ldrb r1, [r2, #0x122]
	add r1, r1, #1
	strb r1, [r2, #0x122]
	ldr r2, [r0]
	ldrb r1, [r2, #0x122]
	cmp r1, #0x28
	addlo sp, sp, #8
	ldmloia sp!, {r3, r4, r5, r6, r7, pc}
	ldrb r1, [r2, #0x123]
	cmp r1, #0
	bne _02228D94
	mov r0, #9
	bl ov13_0223ABB8
	ldr r0, _02228E58 ; =ov13_0224F4BC
	mvn r1, #0
	ldr r0, [r0]
	add sp, sp, #8
	strb r1, [r0, #0x11e]
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_02228D94:
	mov r1, #0x80
	strb r1, [r2, #0x11c]
	ldr r1, [r0]
	add sp, sp, #8
	ldrb r0, [r1, #0x122]
	sub r0, r0, #7
	strb r0, [r1, #0x122]
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_02228DB4:
	add r4, r4, #1
	cmp r4, #4
	add r6, r6, #4
	add r7, r7, #4
	blt _02228CF4
	ldr r6, _02228E64 ; =ov13_02244F7C
	ldr r5, _02228E68 ; =ov13_02244F64
	mov r7, #0
	add r4, sp, #0
_02228DD8:
	mov r0, r6
	mov r1, r5
	mov r2, r4
	bl ov13_02240E74
	mov r0, r4
	bl ov13_02241754
	cmp r0, #0
	beq _02228E1C
	ldr r1, _02228E58 ; =ov13_0224F4BC
	add r0, r7, #0x33
	ldr r1, [r1]
	add r1, r1, #0x100
	ldrsb r1, [r1, #0x1e]
	cmp r1, r0
	bne _02228E2C
	bl ov13_02228E6C
	b _02228E34
_02228E1C:
	add r7, r7, #1
	cmp r7, #2
	add r6, r6, #4
	blt _02228DD8
_02228E2C:
	mvn r0, #0
	bl ov13_02228E6C
_02228E34:
	ldr r0, _02228E58 ; =ov13_0224F4BC
	mov r1, #0
	ldr r0, [r0]
	strb r1, [r0, #0x122]
	add sp, sp, #8
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.balign 4, 0
_02228E4C: .word ov13_02245958
_02228E50: .word ov13_02244FA4
_02228E54: .word ov13_02244F68
_02228E58: .word ov13_0224F4BC
_02228E5C: .word ov13_02244F9C
_02228E60: .word ov13_02244F8C
_02228E64: .word ov13_02244F7C
_02228E68: .word ov13_02244F64
	arm_func_end ov13_02228C68
