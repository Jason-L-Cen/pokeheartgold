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
	.public ov13_02228EC0
	.public ov13_022290F0
	.public ov13_0222936C
	.public ov13_022293F0
	.public ov13_022294B8
	.public ov13_022299CC
	.extern ov13_02228C08
	.extern ov13_02228C38

.text
	arm_func_start ov13_02228EC0
ov13_02228EC0: ; 0x02228EC0
	stmdb sp!, {r4, lr}
	mov r0, #0x20
	bl ov13_022416D4
	cmp r0, #0
	beq _02228EDC
	mov r0, #0
	bl ov13_02229594
_02228EDC:
	mov r0, #0x40
	bl ov13_022416D4
	cmp r0, #0
	beq _02228EF4
	mov r0, #1
	bl ov13_02229594
_02228EF4:
	mov r0, #0x10
	bl ov13_022416D4
	cmp r0, #0
	beq _02228F0C
	mov r0, #2
	bl ov13_02229594
_02228F0C:
	mov r0, #0x80
	bl ov13_022416D4
	cmp r0, #0
	beq _02228F24
	mov r0, #3
	bl ov13_02229594
_02228F24:
	mov r0, #1
	bl ov13_02241694
	cmp r0, #0
	beq _0222903C
	ldr r2, _022290E0 ; =ov13_0224F4BC
	ldr r0, [r2]
	add r1, r0, #0x100
	ldrsb r4, [r1, #0x21]
	cmp r4, #0x2f
	bge _02228F94
	ldrb r1, [r0, #0x124]
	cmp r1, #0
	bne _02228F64
	mov r0, #9
	bl ov13_0223ABB8
	ldmia sp!, {r4, pc}
_02228F64:
	ldrb r3, [r0, #0x11d]
	ldr r1, _022290E4 ; =ov13_02245D68
	ldr r1, [r1, r3, lsl #2]
	ldrb r1, [r1, r4]
	strb r1, [r0, #0x11c]
	ldr r0, [r2]
	ldrb r0, [r0, #0x11d]
	cmp r0, #1
	ldmneia sp!, {r4, pc}
	mov r0, #0
	bl ov13_0222936C
	ldmia sp!, {r4, pc}
_02228F94:
	sub r1, r4, #0x2f
	cmp r1, #4
	bge _0222902C
	cmp r1, #3
	addls pc, pc, r1, lsl #2
	b _02228FFC
_02228FAC: ; jump table
	b _02228FBC ; case 0
	b _02228FC4 ; case 1
	b _02228FCC ; case 2
	b _02228FE4 ; case 3
_02228FBC:
	bl ov13_02228C08
	ldmia sp!, {r4, pc}
_02228FC4:
	bl ov13_02228C38
	ldmia sp!, {r4, pc}
_02228FCC:
	ldrb r1, [r0, #0x124]
	cmp r1, #0
	bne _02228FFC
	mov r0, #9
	bl ov13_0223ABB8
	ldmia sp!, {r4, pc}
_02228FE4:
	ldrb r1, [r0, #0x123]
	cmp r1, #0
	bne _02228FFC
	mov r0, #9
	bl ov13_0223ABB8
	ldmia sp!, {r4, pc}
_02228FFC:
	ldrb r0, [r0, #0x11d]
	cmp r0, #1
	bne _02229010
	mov r0, #0
	bl ov13_0222936C
_02229010:
	ldr r1, _022290E8 ; =ov13_02245208
	sub r2, r4, #0x2f
	ldr r0, _022290E0 ; =ov13_0224F4BC
	ldrb r1, [r1, r2]
	ldr r0, [r0]
	strb r1, [r0, #0x11c]
	ldmia sp!, {r4, pc}
_0222902C:
	ldr r1, _022290EC ; =ov13_02244F58
	sub r2, r4, #0x33
	ldrb r1, [r1, r2]
	strb r1, [r0, #0x11c]
_0222903C:
	mov r0, #2
	bl ov13_022416D4
	cmp r0, #0
	beq _02229094
	ldr r0, _022290E0 ; =ov13_0224F4BC
	ldr r1, [r0]
	ldrb r0, [r1, #0x123]
	cmp r0, #0
	bne _02229088
	ldrb r0, [r1, #0x125]
	cmp r0, #0
	ldmneia sp!, {r4, pc}
	mov r0, #9
	bl ov13_0223ABB8
	ldr r0, _022290E0 ; =ov13_0224F4BC
	mov r1, #1
	ldr r0, [r0]
	strb r1, [r0, #0x125]
	ldmia sp!, {r4, pc}
_02229088:
	mov r0, #0x80
	strb r0, [r1, #0x11c]
	b _022290B4
_02229094:
	mov r0, #2
	bl ov13_02241714
	cmp r0, #0
	beq _022290B4
	ldr r0, _022290E0 ; =ov13_0224F4BC
	mov r1, #0
	ldr r0, [r0]
	strb r1, [r0, #0x125]
_022290B4:
	mov r0, #0x400
	bl ov13_02241694
	cmp r0, #0
	beq _022290C8
	bl ov13_02228C08
_022290C8:
	mov r0, #0x800
	bl ov13_02241694
	cmp r0, #0
	ldmeqia sp!, {r4, pc}
	bl ov13_02228C38
	ldmia sp!, {r4, pc}
	.balign 4, 0
_022290E0: .word ov13_0224F4BC
_022290E4: .word ov13_02245D68
_022290E8: .word ov13_02245208
_022290EC: .word ov13_02244F58
	arm_func_end ov13_02228EC0

	arm_func_start ov13_022290F0
ov13_022290F0: ; 0x022290F0
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x18
	ldr r3, _02229350 ; =ov13_02244F60
	mov sl, r1
	ldrb r5, [r3, #0x11]
	ldrb r4, [r3, #0x12]
	mov r1, #0xc
	mul r1, sl, r1
	strb r5, [sp, #0x12]
	strb r4, [sp, #0x13]
	ldrb r4, [r3, #0x14]
	ldrb r5, [r3, #0x13]
	str r1, [sp, #4]
	strb r4, [sp, #0x15]
	strb r5, [sp, #0x14]
	ldrb r1, [r3, #0x15]
	mov sb, r2
	add r4, sp, #0x12
	strb r1, [sp, #0x16]
	ldrb r1, [r4, sl]
	ldrb r6, [r3, #0x16]
	ldrb r2, [r3, #0x17]
	cmp r1, #0
	ldr r5, [sp, #4]
	strb r6, [sp, #0xd]
	strb r2, [sp, #0xe]
	ldrb r7, [r3, #0x18]
	ldrb r2, [r3, #0x19]
	mov r6, #0
	strb r7, [sp, #0xf]
	strb r2, [sp, #0x10]
	ldrb r1, [r3, #0x1a]
	strb r1, [sp, #0x11]
	ldrb r2, [r3, #0xc]
	ldrb r1, [r3, #0xd]
	strb r2, [sp, #8]
	strb r1, [sp, #9]
	ldrb r2, [r3, #0xe]
	ldrb r1, [r3, #0xf]
	strb r2, [sp, #0xa]
	strb r1, [sp, #0xb]
	ldrb r1, [r3, #0x10]
	strb r1, [sp, #0xc]
	ble _0222920C
	ldr r1, _02229354 ; =ov13_02244FA4
	ldr fp, _02229358 ; =ov13_0224F4BC
	ldr lr, _0222935C ; =0xC1FFFCFF
	ldr r8, _02229360 ; =0xFE00FF00
	and r7, sb, #0xff
_022291B4:
	ldr r2, [fp]
	mov r3, r5, lsl #2
	add r2, r2, r5, lsl #2
	ldr r2, [r2, #0x30]
	ldrh r3, [r1, r3]
	ldr ip, [r2]
	add r6, r6, #1
	and ip, ip, lr
	str ip, [r2]
	mov r2, r3, lsl #0x17
	ldr r3, [fp]
	add r3, r3, r5, lsl #2
	ldr r3, [r3, #0x30]
	add r5, r5, #1
	ldr ip, [r3]
	and ip, ip, r8
	orr ip, ip, r7
	orr r2, ip, r2, lsr #7
	str r2, [r3]
	ldrb r2, [r4, sl]
	cmp r6, r2
	blt _022291B4
_0222920C:
	cmp sl, #4
	bge _0222924C
	ldr r1, _02229358 ; =ov13_0224F4BC
	ldr r3, _02229354 ; =ov13_02244FA4
	ldr r4, [r1]
	ldr r1, [sp, #4]
	mov r2, #2
	mov r1, r1, lsl #2
	str r2, [sp]
	add r0, r4, r0, lsl #4
	add r2, r4, sl, lsl #2
	ldrh r1, [r3, r1]
	ldr r3, [r2, #0x104]
	ldr r0, [r0, sl, lsl #2]
	mov r2, sb
	bl ov13_02240D94
_0222924C:
	add r0, sl, #3
	mov r1, #4
	bl FX_ModS32
	add r3, sp, #0xd
	ldrb r1, [r3, sl]
	mov r5, #0
	cmp r1, #0
	ble _022292D4
	ldr r2, _02229364 ; =ov13_02244F84
	mov r1, r0, lsl #2
	ldrh r0, [r2, r1]
	ldr r8, _02229358 ; =ov13_0224F4BC
	ldr r6, _0222935C ; =0xC1FFFCFF
	ldr r7, _02229360 ; =0xFE00FF00
	and r4, sb, #0xff
	mov r2, r0, lsl #0x17
_0222928C:
	ldr r0, [r8]
	add r5, r5, #1
	add r0, r1, r0
	ldr fp, [r0, #0xec]
	ldr r0, [fp]
	and r0, r0, r6
	str r0, [fp]
	ldr r0, [r8]
	add r0, r1, r0
	ldr r0, [r0, #0xec]
	ldr fp, [r0]
	and fp, fp, r7
	orr fp, fp, r4
	orr fp, fp, r2, lsr #7
	str fp, [r0]
	ldrb r0, [r3, sl]
	cmp r5, r0
	blt _0222928C
_022292D4:
	add r7, sp, #8
	ldrb r0, [r7, sl]
	mov r8, #0
	cmp r0, #0
	addle sp, sp, #0x18
	ldmleia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldr r5, _02229368 ; =ov13_02244F7C
	ldr r4, _02229358 ; =ov13_0224F4BC
	mvn r6, #0
	mov fp, r8
_022292FC:
	ldr r0, [r4]
	mov r1, r6
	add r0, r0, r8, lsl #2
	ldr r0, [r0, #0xfc]
	mov r2, fp
	mov r3, fp
	bl ov13_0223FDF4
	ldr r0, [r4]
	mov r1, r8, lsl #2
	add r0, r0, r8, lsl #2
	ldr r0, [r0, #0xfc]
	ldrh r2, [r5, r1]
	mov r1, r6
	mov r3, sb
	bl ov13_0223FF18
	ldrb r0, [r7, sl]
	add r8, r8, #1
	cmp r8, r0
	blt _022292FC
	add sp, sp, #0x18
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.balign 4, 0
_02229350: .word ov13_02244F60
_02229354: .word ov13_02244FA4
_02229358: .word ov13_0224F4BC
_0222935C: .word 0xC1FFFCFF
_02229360: .word 0xFE00FF00
_02229364: .word ov13_02244F84
_02229368: .word ov13_02244F7C
	arm_func_end ov13_022290F0

	arm_func_start ov13_0222936C
ov13_0222936C: ; 0x0222936C
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, lr}
	ldr r1, _022293E8 ; =ov13_0224F4BC
	mov r5, #0
	ldr r1, [r1]
	mov r8, r0
	ldr sb, _022293EC ; =ov13_02244FA4
	mov r6, r5
	mov r4, r5
	mov r7, r5
	strb r8, [r1, #0x11d]
_02229394:
	add r0, sb, r7, lsl #2
	ldrh r2, [r0, #2]
	mov r0, r8
	mov r1, r4
	bl ov13_022290F0
	add r4, r4, #1
	cmp r4, #4
	add r7, r7, #0xc
	blt _02229394
	cmp r8, #2
	moveq r5, #1
	beq _022293CC
	cmp r8, #1
	moveq r6, #1
_022293CC:
	mov r1, r5
	mov r0, #0x2f
	bl ov13_022293F0
	mov r1, r6
	mov r0, #0x30
	bl ov13_022293F0
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	.balign 4, 0
_022293E8: .word ov13_0224F4BC
_022293EC: .word ov13_02244FA4
	arm_func_end ov13_0222936C

	arm_func_start ov13_022293F0
ov13_022293F0: ; 0x022293F0
	stmdb sp!, {r3, lr}
	cmp r0, #0
	ldmltia sp!, {r3, pc}
	cmp r0, #0x2f
	bge _0222943C
	ldr r2, _022294AC ; =ov13_0224F4BC
	ldr r3, _022294B0 ; =ov13_02244F4C
	ldr r2, [r2]
	ldrb r1, [r3, r1]
	add r0, r2, r0, lsl #2
	ldr r2, [r0, #0x30]
	ldr r0, [r2]
	bic r0, r0, #0xc00
	str r0, [r2]
	ldrh r0, [r2, #4]
	bic r0, r0, #0xf000
	orr r0, r0, r1, lsl #12
	strh r0, [r2, #4]
	ldmia sp!, {r3, pc}
_0222943C:
	sub ip, r0, #0x2f
	cmp ip, #4
	bge _02229480
	ldr r0, _022294AC ; =ov13_0224F4BC
	ldr r2, _022294B0 ; =ov13_02244F4C
	ldr r3, [r0]
	ldrb r0, [r2, r1]
	add r1, r3, ip, lsl #2
	ldr r2, [r1, #0xec]
	ldr r1, [r2]
	bic r1, r1, #0xc00
	str r1, [r2]
	ldrh r1, [r2, #4]
	bic r1, r1, #0xf000
	orr r0, r1, r0, lsl #12
	strh r0, [r2, #4]
	ldmia sp!, {r3, pc}
_02229480:
	ldr r2, _022294AC ; =ov13_0224F4BC
	ldr r3, _022294B4 ; =ov13_02244F50
	ldr r2, [r2]
	sub r0, r0, #0x33
	add r0, r2, r0, lsl #2
	ldrb r3, [r3, r1]
	ldr r0, [r0, #0xfc]
	mvn r1, #0
	mov r2, #0
	bl ov13_0223FE98
	ldmia sp!, {r3, pc}
	.balign 4, 0
_022294AC: .word ov13_0224F4BC
_022294B0: .word ov13_02244F4C
_022294B4: .word ov13_02244F50
	arm_func_end ov13_022293F0

	arm_func_start ov13_022294B8
ov13_022294B8: ; 0x022294B8
	stmdb sp!, {r4, lr}
	ldr r0, _02229588 ; =ov13_0224F4BC
	ldr r1, [r0]
	add r0, r1, #0x100
	ldrsb r0, [r0, #0x21]
	sub r0, r0, #0x2f
	cmp r0, #5
	addls pc, pc, r0, lsl #2
	b _0222951C
_022294DC: ; jump table
	b _022294F4 ; case 0
	b _022294FC ; case 1
	b _02229504 ; case 2
	b _0222950C ; case 3
	b _02229514 ; case 4
	b _02229514 ; case 5
_022294F4:
	mov r4, #0x42
	b _02229520
_022294FC:
	mov r4, #0x41
	b _02229520
_02229504:
	mov r4, #0x43
	b _02229520
_0222950C:
	mov r4, #0x41
	b _02229520
_02229514:
	mov r4, #0x45
	b _02229520
_0222951C:
	mov r4, #0x40
_02229520:
	ldr r0, [r1, #0x114]
	mov r1, #0
	bl ov13_0223FDE0
	mov r2, r0
	mov r1, r4
	mov r0, #0
	bl ov13_022400F0
	ldr r0, _02229588 ; =ov13_0224F4BC
	mvn r1, #0
	ldr r0, [r0]
	mov r2, #2
	ldr r0, [r0, #0x114]
	bl ov13_02240008
	ldr r0, _02229588 ; =ov13_0224F4BC
	ldr r2, _0222958C ; =ov13_02245060
	ldr r3, [r0]
	mvn r1, #0
	add r0, r3, #0x100
	ldrsb ip, [r0, #0x21]
	ldr r0, [r3, #0x114]
	ldr r3, _02229590 ; =ov13_02245062
	mov ip, ip, lsl #2
	ldrh r2, [r2, ip]
	ldrh r3, [r3, ip]
	bl ov13_0223FF18
	ldmia sp!, {r4, pc}
	.balign 4, 0
_02229588: .word ov13_0224F4BC
_0222958C: .word ov13_02245060
_02229590: .word ov13_02245062
	arm_func_end ov13_022294B8

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

	arm_func_start ov13_0222A03C
ov13_0222A03C: ; 0x0222A03C
	stmdb sp!, {r3, lr}
	ldr r0, _0222A068 ; =ov13_0224F4C0
	ldr r0, [r0]
	ldr r0, [r0, #0x58]
	bl ov13_0223FD60
	ldr r0, _0222A068 ; =ov13_0224F4C0
	ldr r1, _0222A06C ; =ov13_0222AF5C
	ldr r0, [r0]
	ldr r0, [r0, #0x5c]
	bl ov13_022425C8
	ldmia sp!, {r3, pc}
	.balign 4, 0
_0222A068: .word ov13_0224F4C0
_0222A06C: .word ov13_0222AF5C
	arm_func_end ov13_0222A03C

	arm_func_start ov13_0222A070
ov13_0222A070: ; 0x0222A070
	ldr r0, _0222A080 ; =ov13_0224F4C0
	ldr r0, [r0]
	ldrb r0, [r0, #0x60]
	bx lr
	.balign 4, 0
_0222A080: .word ov13_0224F4C0
	arm_func_end ov13_0222A070

	arm_func_start ov13_0222A084
ov13_0222A084: ; 0x0222A084
	ldr r1, _0222A094 ; =ov13_0224F4C0
	ldr r1, [r1]
	strb r0, [r1, #0x66]
	bx lr
	.balign 4, 0
_0222A094: .word ov13_0224F4C0
	arm_func_end ov13_0222A084

	arm_func_start ov13_0222A098
ov13_0222A098: ; 0x0222A098
	ldr r1, _0222A0A8 ; =ov13_0224F4C0
	ldr r1, [r1]
	strb r0, [r1, #0x67]
	bx lr
	.balign 4, 0
_0222A0A8: .word ov13_0224F4C0
	arm_func_end ov13_0222A098

	arm_func_start ov13_0222A0AC
ov13_0222A0AC: ; 0x0222A0AC
	ldr r1, _0222A0BC ; =ov13_0224F4C0
	ldr r1, [r1]
	strb r0, [r1, #0x68]
	bx lr
	.balign 4, 0
_0222A0BC: .word ov13_0224F4C0
	arm_func_end ov13_0222A0AC

	arm_func_start ov13_0222A0C0
ov13_0222A0C0: ; 0x0222A0C0
	ldr r0, _0222A0D8 ; =ov13_0224F4C0
	ldr r0, [r0]
	cmp r0, #0
	movne r0, #1
	moveq r0, #0
	bx lr
	.balign 4, 0
_0222A0D8: .word ov13_0224F4C0
	arm_func_end ov13_0222A0C0

	arm_func_start ov13_0222A0DC
ov13_0222A0DC: ; 0x0222A0DC
	stmdb sp!, {r4, lr}
	sub sp, sp, #8
	ldr r2, _0222A154 ; =ov13_0224F4C0
	mov r4, r0
	ldr r2, [r2]
	ldr r1, _0222A158 ; =0x01FF0000
	ldr r0, [r2, #0x10]
	ldr r0, [r0]
	and r2, r0, r1
	and r0, r0, #0xff
	sub r1, r0, #0xc
	mov r0, r2, lsr #0x10
	str r0, [sp, #4]
	str r1, [sp]
	cmp r1, #0x4c
	mov r0, #0
	ble _0222A12C
	bl ov13_0222AAFC
	add sp, sp, #8
	ldmia sp!, {r4, pc}
_0222A12C:
	mov r1, #0x4c
	bl ov13_0222AAFC
	mov r0, #1
	mov r1, #0xc0
	bl ov13_0222AAFC
	ldr r1, _0222A15C ; =ov13_0222A160
	mov r0, r4
	bl ov13_022425C8
	add sp, sp, #8
	ldmia sp!, {r4, pc}
	.balign 4, 0
_0222A154: .word ov13_0224F4C0
_0222A158: .word 0x01FF0000
_0222A15C: .word ov13_0222A160
	arm_func_end ov13_0222A0DC

	arm_func_start ov13_0222A160
ov13_0222A160: ; 0x0222A160
	stmdb sp!, {r4, lr}
	sub sp, sp, #8
	ldr r2, _0222A1D8 ; =ov13_0224F4C0
	mov r4, r0
	ldr r2, [r2]
	ldr r1, _0222A1DC ; =0x01FF0000
	ldr r0, [r2, #0x1c]
	ldr r0, [r0]
	and r2, r0, r1
	and r0, r0, #0xff
	sub r1, r0, #0xc
	mov r0, r2, lsr #0x10
	str r0, [sp, #4]
	str r1, [sp]
	cmp r1, #0x63
	mov r0, #1
	ble _0222A1B0
	bl ov13_0222AAFC
	add sp, sp, #8
	ldmia sp!, {r4, pc}
_0222A1B0:
	mov r1, #0x63
	bl ov13_0222AAFC
	mov r0, #2
	mov r1, #0xc0
	bl ov13_0222AAFC
	ldr r1, _0222A1E0 ; =ov13_0222A1E4
	mov r0, r4
	bl ov13_022425C8
	add sp, sp, #8
	ldmia sp!, {r4, pc}
	.balign 4, 0
_0222A1D8: .word ov13_0224F4C0
_0222A1DC: .word 0x01FF0000
_0222A1E0: .word ov13_0222A1E4
	arm_func_end ov13_0222A160

	arm_func_start ov13_0222A1E4
ov13_0222A1E4: ; 0x0222A1E4
	stmdb sp!, {r4, lr}
	sub sp, sp, #8
	ldr r2, _0222A25C ; =ov13_0224F4C0
	mov r4, r0
	ldr r2, [r2]
	ldr r1, _0222A260 ; =0x01FF0000
	ldr r0, [r2, #0x28]
	ldr r0, [r0]
	and r2, r0, r1
	and r0, r0, #0xff
	sub r1, r0, #0xc
	mov r0, r2, lsr #0x10
	str r0, [sp, #4]
	str r1, [sp]
	cmp r1, #0x7a
	mov r0, #2
	ble _0222A234
	bl ov13_0222AAFC
	add sp, sp, #8
	ldmia sp!, {r4, pc}
_0222A234:
	mov r1, #0x7a
	bl ov13_0222AAFC
	mov r0, #3
	mov r1, #0xc0
	bl ov13_0222AAFC
	ldr r1, _0222A264 ; =ov13_0222A268
	mov r0, r4
	bl ov13_022425C8
	add sp, sp, #8
	ldmia sp!, {r4, pc}
	.balign 4, 0
_0222A25C: .word ov13_0224F4C0
_0222A260: .word 0x01FF0000
_0222A264: .word ov13_0222A268
	arm_func_end ov13_0222A1E4

	arm_func_start ov13_0222A268
ov13_0222A268: ; 0x0222A268
	stmdb sp!, {r4, lr}
	sub sp, sp, #8
	ldr r2, _0222A2E0 ; =ov13_0224F4C0
	mov r4, r0
	ldr r2, [r2]
	ldr r1, _0222A2E4 ; =0x01FF0000
	ldr r0, [r2, #0x34]
	ldr r0, [r0]
	and r2, r0, r1
	and r0, r0, #0xff
	sub r1, r0, #0xc
	mov r0, r2, lsr #0x10
	str r0, [sp, #4]
	str r1, [sp]
	cmp r1, #0x91
	mov r0, #3
	ble _0222A2B8
	bl ov13_0222AAFC
	add sp, sp, #8
	ldmia sp!, {r4, pc}
_0222A2B8:
	mov r1, #0x91
	bl ov13_0222AAFC
	mov r0, #4
	mov r1, #0xc0
	bl ov13_0222AAFC
	ldr r1, _0222A2E8 ; =ov13_0222A2EC
	mov r0, r4
	bl ov13_022425C8
	add sp, sp, #8
	ldmia sp!, {r4, pc}
	.balign 4, 0
_0222A2E0: .word ov13_0224F4C0
_0222A2E4: .word 0x01FF0000
_0222A2E8: .word ov13_0222A2EC
	arm_func_end ov13_0222A268

	arm_func_start ov13_0222A2EC
ov13_0222A2EC: ; 0x0222A2EC
	stmdb sp!, {r4, lr}
	sub sp, sp, #8
	ldr r1, _0222A358 ; =ov13_0224F4C0
	mov r4, r0
	ldr r0, [r1]
	add r2, sp, #4
	ldr r0, [r0, #0x40]
	add r3, sp, #0
	mov r1, #0
	bl ov13_02240068
	ldr r0, [sp]
	sub r1, r0, #0xc
	str r1, [sp]
	cmp r1, #0xaa
	mov r0, #4
	ble _0222A338
	bl ov13_0222AAFC
	add sp, sp, #8
	ldmia sp!, {r4, pc}
_0222A338:
	mov r1, #0xaa
	bl ov13_0222AAFC
	bl ov13_0222AE24
	ldr r1, _0222A35C ; =ov13_0222A360
	mov r0, r4
	bl ov13_022425C8
	add sp, sp, #8
	ldmia sp!, {r4, pc}
	.balign 4, 0
_0222A358: .word ov13_0224F4C0
_0222A35C: .word ov13_0222A360
	arm_func_end ov13_0222A2EC

	arm_func_start ov13_0222A360
ov13_0222A360: ; 0x0222A360
	stmdb sp!, {r3, lr}
	bl ov13_0222A378
	bl ov13_0222A554
	bl ov13_0222A724
	bl ov13_0222A960
	ldmia sp!, {r3, pc}
	arm_func_end ov13_0222A360

	arm_func_start ov13_0222A378
ov13_0222A378: ; 0x0222A378
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	sub sp, sp, #8
	ldr r0, _0222A534 ; =ov13_02245958
	bl ov13_022417DC
	cmp r0, #0
	addeq sp, sp, #8
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	ldr r0, _0222A538 ; =ov13_0224F4C0
	mvn r1, #0
	ldr r0, [r0]
	ldr r7, _0222A53C ; =ov13_0224527A
	ldr r6, _0222A540 ; =ov13_02245228
	strb r1, [r0, #0x61]
	mov r4, #0
	add r5, sp, #0
_0222A3B4:
	mov r0, r7
	mov r1, r6
	mov r2, r5
	bl ov13_02240E74
	mov r0, r5
	bl ov13_022417DC
	cmp r0, #0
	beq _0222A414
	ldr r0, _0222A538 ; =ov13_0224F4C0
	ldr r0, [r0]
	ldrb r0, [r0, #0x67]
	cmp r0, #0
	bne _0222A3F8
	mov r0, #9
	bl ov13_0223ABB8
	add sp, sp, #8
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_0222A3F8:
	mov r0, #0
	bl ov13_0223ABB8
	ldr r0, _0222A538 ; =ov13_0224F4C0
	add sp, sp, #8
	ldr r0, [r0]
	strb r4, [r0, #0x61]
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_0222A414:
	add r4, r4, #1
	cmp r4, #0xa
	add r7, r7, #4
	blt _0222A3B4
	ldr r7, _0222A544 ; =ov13_02245250
	ldr r6, _0222A548 ; =ov13_02245230
	mov r4, #0
	add r5, sp, #0
_0222A434:
	mov r0, r7
	mov r1, r6
	mov r2, r5
	bl ov13_02240E74
	mov r0, r5
	bl ov13_022417DC
	cmp r0, #0
	beq _0222A4BC
	cmp r4, #0
	bne _0222A470
	ldr r0, _0222A538 ; =ov13_0224F4C0
	ldr r0, [r0]
	ldrb r0, [r0, #0x66]
	cmp r0, #0
	beq _0222A48C
_0222A470:
	cmp r4, #1
	bne _0222A49C
	ldr r0, _0222A538 ; =ov13_0224F4C0
	ldr r0, [r0]
	ldrb r0, [r0, #0x68]
	cmp r0, #0
	bne _0222A49C
_0222A48C:
	mov r0, #9
	bl ov13_0223ABB8
	add sp, sp, #8
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_0222A49C:
	mov r0, #0
	bl ov13_0223ABB8
	ldr r0, _0222A538 ; =ov13_0224F4C0
	add r1, r4, #0xa
	ldr r0, [r0]
	add sp, sp, #8
	strb r1, [r0, #0x61]
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_0222A4BC:
	add r4, r4, #1
	cmp r4, #2
	add r7, r7, #4
	blt _0222A434
	ldr r6, _0222A54C ; =ov13_02245258
	ldr r5, _0222A550 ; =ov13_02245234
	mov r7, #0
	add r4, sp, #0
_0222A4DC:
	mov r0, r6
	mov r1, r5
	mov r2, r4
	bl ov13_02240E74
	mov r0, r4
	bl ov13_022417DC
	cmp r0, #0
	beq _0222A51C
	mov r0, #0
	bl ov13_0223ABB8
	ldr r0, _0222A538 ; =ov13_0224F4C0
	add r1, r7, #0xc
	ldr r0, [r0]
	add sp, sp, #8
	strb r1, [r0, #0x61]
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_0222A51C:
	add r7, r7, #1
	cmp r7, #2
	add r6, r6, #4
	blt _0222A4DC
	add sp, sp, #8
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.balign 4, 0
_0222A534: .word ov13_02245958
_0222A538: .word ov13_0224F4C0
_0222A53C: .word ov13_0224527A
_0222A540: .word ov13_02245228
_0222A544: .word ov13_02245250
_0222A548: .word ov13_02245230
_0222A54C: .word ov13_02245258
_0222A550: .word ov13_02245234
	arm_func_end ov13_0222A378

	arm_func_start ov13_0222A554
ov13_0222A554: ; 0x0222A554
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	sub sp, sp, #8
	ldr r1, _0222A6F8 ; =ov13_0224F4C0
	ldr r0, _0222A6FC ; =ov13_02245958
	ldr r1, [r1]
	mov r2, #0
	strb r2, [r1, #0x60]
	bl ov13_022418EC
	cmp r0, #0
	addeq sp, sp, #8
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	ldr r7, _0222A700 ; =ov13_0224527A
	ldr r6, _0222A704 ; =ov13_02245228
	mov r4, #0
	add r5, sp, #0
_0222A590:
	mov r0, r7
	mov r1, r6
	mov r2, r5
	bl ov13_02240E74
	mov r0, r5
	bl ov13_022418EC
	cmp r0, #0
	beq _0222A5E8
	ldr r0, _0222A6F8 ; =ov13_0224F4C0
	ldr r2, [r0]
	ldrsb r1, [r2, #0x61]
	cmp r1, r4
	addne sp, sp, #8
	ldmneia sp!, {r3, r4, r5, r6, r7, pc}
	ldr r1, _0222A708 ; =ov13_02245314
	ldrb r1, [r1, r4]
	strb r1, [r2, #0x60]
	ldr r0, [r0]
	strb r4, [r0, #0x63]
	bl ov13_0222AE24
	add sp, sp, #8
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_0222A5E8:
	add r4, r4, #1
	cmp r4, #0xa
	add r7, r7, #4
	blt _0222A590
	ldr r7, _0222A70C ; =ov13_02245250
	ldr r6, _0222A710 ; =ov13_02245230
	mov r4, #0
	add r5, sp, #0
_0222A608:
	mov r0, r7
	mov r1, r6
	mov r2, r5
	bl ov13_02240E74
	mov r0, r5
	bl ov13_022418EC
	cmp r0, #0
	beq _0222A664
	ldr r0, _0222A6F8 ; =ov13_0224F4C0
	add r2, r4, #0xa
	ldr r3, [r0]
	ldrsb r1, [r3, #0x61]
	cmp r1, r2
	addne sp, sp, #8
	ldmneia sp!, {r3, r4, r5, r6, r7, pc}
	ldr r1, _0222A714 ; =ov13_02245224
	ldrb r1, [r1, r4]
	strb r1, [r3, #0x60]
	ldr r0, [r0]
	strb r2, [r0, #0x63]
	bl ov13_0222AE24
	add sp, sp, #8
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_0222A664:
	add r4, r4, #1
	cmp r4, #2
	add r7, r7, #4
	blt _0222A608
	ldr r7, _0222A718 ; =ov13_02245258
	ldr r6, _0222A71C ; =ov13_02245234
	mov r4, #0
	add r5, sp, #0
_0222A684:
	mov r0, r7
	mov r1, r6
	mov r2, r5
	bl ov13_02240E74
	mov r0, r5
	bl ov13_022418EC
	cmp r0, #0
	beq _0222A6E0
	ldr r0, _0222A6F8 ; =ov13_0224F4C0
	add r2, r4, #0xc
	ldr r3, [r0]
	ldrsb r1, [r3, #0x61]
	cmp r1, r2
	addne sp, sp, #8
	ldmneia sp!, {r3, r4, r5, r6, r7, pc}
	ldr r1, _0222A720 ; =ov13_02245220
	ldrb r1, [r1, r4]
	strb r1, [r3, #0x60]
	ldr r0, [r0]
	strb r2, [r0, #0x63]
	bl ov13_0222AE24
	add sp, sp, #8
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_0222A6E0:
	add r4, r4, #1
	cmp r4, #2
	add r7, r7, #4
	blt _0222A684
	add sp, sp, #8
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.balign 4, 0
_0222A6F8: .word ov13_0224F4C0
_0222A6FC: .word ov13_02245958
_0222A700: .word ov13_0224527A
_0222A704: .word ov13_02245228
_0222A708: .word ov13_02245314
_0222A70C: .word ov13_02245250
_0222A710: .word ov13_02245230
_0222A714: .word ov13_02245224
_0222A718: .word ov13_02245258
_0222A71C: .word ov13_02245234
_0222A720: .word ov13_02245220
	arm_func_end ov13_0222A554

	arm_func_start ov13_0222A724
ov13_0222A724: ; 0x0222A724
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	sub sp, sp, #8
	ldr r0, _0222A8F4 ; =ov13_02245958
	bl ov13_02241754
	cmp r0, #0
	beq _0222A8D4
	ldr r7, _0222A8F8 ; =ov13_0224527A
	ldr r5, _0222A8FC ; =ov13_02245228
	mov r6, #0
	add r4, sp, #0
_0222A74C:
	mov r0, r7
	mov r1, r5
	mov r2, r4
	bl ov13_02240E74
	mov r0, r4
	bl ov13_02241754
	cmp r0, #0
	beq _0222A78C
	ldr r0, _0222A900 ; =ov13_0224F4C0
	ldr r0, [r0]
	ldrsb r0, [r0, #0x61]
	cmp r0, r6
	bne _0222A8D4
	mov r0, r6
	bl ov13_0222A914
	b _0222A8DC
_0222A78C:
	add r6, r6, #1
	cmp r6, #0xa
	add r7, r7, #4
	blt _0222A74C
	ldr r6, _0222A904 ; =ov13_02245250
	ldr r5, _0222A908 ; =ov13_02245230
	mov r7, #0
	add r4, sp, #0
_0222A7AC:
	mov r0, r6
	mov r1, r5
	mov r2, r4
	bl ov13_02240E74
	mov r0, r4
	bl ov13_02241754
	cmp r0, #0
	beq _0222A864
	ldr r1, _0222A900 ; =ov13_0224F4C0
	add r0, r7, #0xa
	ldr r1, [r1]
	ldrsb r1, [r1, #0x61]
	cmp r1, r0
	bne _0222A8D4
	bl ov13_0222A914
	cmp r7, #0
	bne _0222A8DC
	ldr r0, _0222A900 ; =ov13_0224F4C0
	ldr r2, [r0]
	ldrb r1, [r2, #0x65]
	add r1, r1, #1
	strb r1, [r2, #0x65]
	ldr r2, [r0]
	ldrb r1, [r2, #0x65]
	cmp r1, #0x28
	addlo sp, sp, #8
	ldmloia sp!, {r3, r4, r5, r6, r7, pc}
	ldrb r1, [r2, #0x66]
	cmp r1, #0
	bne _0222A844
	mov r0, #9
	bl ov13_0223ABB8
	ldr r0, _0222A900 ; =ov13_0224F4C0
	mvn r1, #0
	ldr r0, [r0]
	add sp, sp, #8
	strb r1, [r0, #0x61]
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_0222A844:
	mov r1, #0x10
	strb r1, [r2, #0x60]
	ldr r1, [r0]
	add sp, sp, #8
	ldrb r0, [r1, #0x65]
	sub r0, r0, #7
	strb r0, [r1, #0x65]
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_0222A864:
	add r7, r7, #1
	cmp r7, #2
	add r6, r6, #4
	blt _0222A7AC
	ldr r6, _0222A90C ; =ov13_02245258
	ldr r5, _0222A910 ; =ov13_02245234
	mov r7, #0
	add r4, sp, #0
_0222A884:
	mov r0, r6
	mov r1, r5
	mov r2, r4
	bl ov13_02240E74
	mov r0, r4
	bl ov13_02241754
	cmp r0, #0
	beq _0222A8C4
	ldr r1, _0222A900 ; =ov13_0224F4C0
	add r0, r7, #0xc
	ldr r1, [r1]
	ldrsb r1, [r1, #0x61]
	cmp r1, r0
	bne _0222A8D4
	bl ov13_0222A914
	b _0222A8DC
_0222A8C4:
	add r7, r7, #1
	cmp r7, #2
	add r6, r6, #4
	blt _0222A884
_0222A8D4:
	mvn r0, #0
	bl ov13_0222A914
_0222A8DC:
	ldr r0, _0222A900 ; =ov13_0224F4C0
	mov r1, #0
	ldr r0, [r0]
	strb r1, [r0, #0x65]
	add sp, sp, #8
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.balign 4, 0
_0222A8F4: .word ov13_02245958
_0222A8F8: .word ov13_0224527A
_0222A8FC: .word ov13_02245228
_0222A900: .word ov13_0224F4C0
_0222A904: .word ov13_02245250
_0222A908: .word ov13_02245230
_0222A90C: .word ov13_02245258
_0222A910: .word ov13_02245234
	arm_func_end ov13_0222A724

	arm_func_start ov13_0222A914
ov13_0222A914: ; 0x0222A914
	stmdb sp!, {r4, lr}
	ldr r1, _0222A95C ; =ov13_0224F4C0
	mov r4, r0
	ldr r1, [r1]
	ldrsb r1, [r1, #0x62]
	cmp r4, r1
	ldmeqia sp!, {r4, pc}
	mov r1, #1
	bl ov13_0222AD5C
	ldr r0, _0222A95C ; =ov13_0224F4C0
	mov r1, #0
	ldr r0, [r0]
	ldrsb r0, [r0, #0x62]
	bl ov13_0222AD5C
	ldr r0, _0222A95C ; =ov13_0224F4C0
	ldr r0, [r0]
	strb r4, [r0, #0x62]
	ldmia sp!, {r4, pc}
	.balign 4, 0
_0222A95C: .word ov13_0224F4C0
	arm_func_end ov13_0222A914

	arm_func_start ov13_0222A960
ov13_0222A960: ; 0x0222A960
	stmdb sp!, {r3, lr}
	mov r0, #0x20
	bl ov13_022416D4
	cmp r0, #0
	beq _0222A97C
	mov r0, #0
	bl ov13_0222AEB0
_0222A97C:
	mov r0, #0x40
	bl ov13_022416D4
	cmp r0, #0
	beq _0222A994
	mov r0, #1
	bl ov13_0222AEB0
_0222A994:
	mov r0, #0x10
	bl ov13_022416D4
	cmp r0, #0
	beq _0222A9AC
	mov r0, #2
	bl ov13_0222AEB0
_0222A9AC:
	mov r0, #0x80
	bl ov13_022416D4
	cmp r0, #0
	beq _0222A9C4
	mov r0, #3
	bl ov13_0222AEB0
_0222A9C4:
	mov r0, #1
	bl ov13_02241694
	cmp r0, #0
	beq _0222AA70
	ldr r0, _0222AAEC ; =ov13_0224F4C0
	ldr r0, [r0]
	ldrsb r2, [r0, #0x63]
	cmp r2, #0xa
	bge _0222AA10
	ldrb r1, [r0, #0x67]
	cmp r1, #0
	beq _0222AA04
	ldr r1, _0222AAF0 ; =ov13_02245314
	ldrb r1, [r1, r2]
	strb r1, [r0, #0x60]
	ldmia sp!, {r3, pc}
_0222AA04:
	mov r0, #9
	bl ov13_0223ABB8
	ldmia sp!, {r3, pc}
_0222AA10:
	sub r1, r2, #0xa
	cmp r1, #2
	bge _0222AA60
	cmp r1, #0
	ldreqb r1, [r0, #0x66]
	cmpeq r1, #0
	beq _0222AA40
	sub r1, r2, #0xa
	cmp r1, #1
	ldreqb r1, [r0, #0x68]
	cmpeq r1, #0
	bne _0222AA4C
_0222AA40:
	mov r0, #9
	bl ov13_0223ABB8
	ldmia sp!, {r3, pc}
_0222AA4C:
	ldr r1, _0222AAF4 ; =ov13_02245224
	sub r2, r2, #0xa
	ldrb r1, [r1, r2]
	strb r1, [r0, #0x60]
	ldmia sp!, {r3, pc}
_0222AA60:
	ldr r1, _0222AAF8 ; =ov13_02245220
	sub r2, r2, #0xc
	ldrb r1, [r1, r2]
	strb r1, [r0, #0x60]
_0222AA70:
	mov r0, #2
	bl ov13_022416D4
	cmp r0, #0
	beq _0222AAC8
	ldr r0, _0222AAEC ; =ov13_0224F4C0
	ldr r1, [r0]
	ldrb r0, [r1, #0x66]
	cmp r0, #0
	bne _0222AABC
	ldrb r0, [r1, #0x69]
	cmp r0, #0
	ldmneia sp!, {r3, pc}
	mov r0, #9
	bl ov13_0223ABB8
	ldr r0, _0222AAEC ; =ov13_0224F4C0
	mov r1, #1
	ldr r0, [r0]
	strb r1, [r0, #0x69]
	ldmia sp!, {r3, pc}
_0222AABC:
	mov r0, #0x10
	strb r0, [r1, #0x60]
	ldmia sp!, {r3, pc}
_0222AAC8:
	mov r0, #2
	bl ov13_02241714
	cmp r0, #0
	ldmeqia sp!, {r3, pc}
	ldr r0, _0222AAEC ; =ov13_0224F4C0
	mov r1, #0
	ldr r0, [r0]
	strb r1, [r0, #0x69]
	ldmia sp!, {r3, pc}
	.balign 4, 0
_0222AAEC: .word ov13_0224F4C0
_0222AAF0: .word ov13_02245314
_0222AAF4: .word ov13_02245224
_0222AAF8: .word ov13_02245220
	arm_func_end ov13_0222A960

	arm_func_start ov13_0222AAFC
ov13_0222AAFC: ; 0x0222AAFC
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x14
	ldr r2, _0222AD40 ; =ov13_02245228
	mov sl, r0
	ldrb r3, [r2, #0x15]
	ldrb r0, [r2, #0x16]
	ldrb r4, [r2, #0x17]
	strb r3, [sp, #0xe]
	strb r0, [sp, #0xf]
	ldrb r0, [r2, #0x18]
	mov sb, r1
	add r3, sp, #0xe
	strb r4, [sp, #0x10]
	strb r0, [sp, #0x11]
	ldrb r0, [r2, #0x19]
	add lr, sl, sl, lsl #1
	mov r4, lr
	strb r0, [sp, #0x12]
	ldrb r6, [r2, #0x1a]
	ldrb r1, [r2, #0x1b]
	ldrb r0, [r3, sl]
	mov r5, #0
	strb r6, [sp, #9]
	strb r1, [sp, #0xa]
	ldrb r6, [r2, #0x1c]
	ldrb r1, [r2, #0x1d]
	cmp r0, #0
	strb r6, [sp, #0xb]
	strb r1, [sp, #0xc]
	ldrb r0, [r2, #0x1e]
	strb r0, [sp, #0xd]
	ldrb r1, [r2, #0x10]
	ldrb r0, [r2, #0x11]
	strb r1, [sp, #4]
	strb r0, [sp, #5]
	ldrb r1, [r2, #0x12]
	ldrb r0, [r2, #0x13]
	strb r1, [sp, #6]
	strb r0, [sp, #7]
	ldrb r0, [r2, #0x14]
	strb r0, [sp, #8]
	ble _0222AC10
	ldr r0, _0222AD44 ; =ov13_0224527A
	ldr fp, _0222AD48 ; =ov13_0224F4C0
	ldr r7, _0222AD4C ; =0xC1FFFCFF
	ldr r8, _0222AD50 ; =0xFE00FF00
	and r6, sb, #0xff
_0222ABB8:
	ldr r1, [fp]
	mov r2, r4, lsl #2
	add r1, r1, r4, lsl #2
	ldr r1, [r1, #0x10]
	ldrh r2, [r0, r2]
	ldr ip, [r1]
	add r5, r5, #1
	and ip, ip, r7
	str ip, [r1]
	ldr ip, [fp]
	mov r1, r2, lsl #0x17
	add r2, ip, r4, lsl #2
	ldr r2, [r2, #0x10]
	add r4, r4, #1
	ldr ip, [r2]
	and ip, ip, r8
	orr ip, ip, r6
	orr r1, ip, r1, lsr #7
	str r1, [r2]
	ldrb r1, [r3, sl]
	cmp r5, r1
	blt _0222ABB8
_0222AC10:
	cmp sl, #4
	bge _0222AC48
	ldr r0, _0222AD48 ; =ov13_0224F4C0
	ldr r1, _0222AD44 ; =ov13_0224527A
	ldr r3, [r0]
	mov r0, lr, lsl #2
	mov r2, #2
	str r2, [sp]
	add r2, r3, sl, lsl #2
	ldrh r1, [r1, r0]
	ldr r0, [r3, sl, lsl #2]
	ldr r3, [r2, #0x48]
	mov r2, sb
	bl ov13_02240D94
_0222AC48:
	add r3, sp, #9
	ldrb r0, [r3, sl]
	mov r5, #0
	cmp r0, #0
	ble _0222ACC4
	ldr r0, _0222AD54 ; =ov13_02245250
	ldr r8, _0222AD48 ; =ov13_0224F4C0
	ldr r6, _0222AD4C ; =0xC1FFFCFF
	ldr r7, _0222AD50 ; =0xFE00FF00
	and r4, sb, #0xff
_0222AC70:
	ldr r1, [r8]
	mov r2, r5, lsl #2
	add r1, r1, r5, lsl #2
	ldr fp, [r1, #0x38]
	ldrh r1, [r0, r2]
	ldr r2, [fp]
	and r2, r2, r6
	str r2, [fp]
	ldr r2, [r8]
	mov r1, r1, lsl #0x17
	add r2, r2, r5, lsl #2
	ldr r2, [r2, #0x38]
	add r5, r5, #1
	ldr fp, [r2]
	and fp, fp, r7
	orr fp, fp, r4
	orr r1, fp, r1, lsr #7
	str r1, [r2]
	ldrb r1, [r3, sl]
	cmp r5, r1
	blt _0222AC70
_0222ACC4:
	add r7, sp, #4
	ldrb r0, [r7, sl]
	mov r8, #0
	cmp r0, #0
	addle sp, sp, #0x14
	ldmleia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldr r5, _0222AD58 ; =ov13_02245258
	ldr r4, _0222AD48 ; =ov13_0224F4C0
	mvn r6, #0
	mov fp, r8
_0222ACEC:
	ldr r0, [r4]
	mov r1, r6
	add r0, r0, r8, lsl #2
	ldr r0, [r0, #0x40]
	mov r2, fp
	mov r3, fp
	bl ov13_0223FDF4
	ldr r0, [r4]
	mov r1, r8, lsl #2
	add r0, r0, r8, lsl #2
	ldr r0, [r0, #0x40]
	ldrh r2, [r5, r1]
	mov r1, r6
	mov r3, sb
	bl ov13_0223FF18
	ldrb r0, [r7, sl]
	add r8, r8, #1
	cmp r8, r0
	blt _0222ACEC
	add sp, sp, #0x14
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.balign 4, 0
_0222AD40: .word ov13_02245228
_0222AD44: .word ov13_0224527A
_0222AD48: .word ov13_0224F4C0
_0222AD4C: .word 0xC1FFFCFF
_0222AD50: .word 0xFE00FF00
_0222AD54: .word ov13_02245250
_0222AD58: .word ov13_02245258
	arm_func_end ov13_0222AAFC

	arm_func_start ov13_0222AD5C
ov13_0222AD5C: ; 0x0222AD5C
	stmdb sp!, {r3, lr}
	cmp r0, #0
	ldmltia sp!, {r3, pc}
	cmp r0, #0xa
	bge _0222ADA8
	ldr r2, _0222AE18 ; =ov13_0224F4C0
	ldr r3, _0222AE1C ; =ov13_0224521C
	ldr r2, [r2]
	ldrb r1, [r3, r1]
	add r0, r2, r0, lsl #2
	ldr r2, [r0, #0x10]
	ldr r0, [r2]
	bic r0, r0, #0xc00
	str r0, [r2]
	ldrh r0, [r2, #4]
	bic r0, r0, #0xf000
	orr r0, r0, r1, lsl #12
	strh r0, [r2, #4]
	ldmia sp!, {r3, pc}
_0222ADA8:
	sub ip, r0, #0xa
	cmp ip, #2
	bge _0222ADEC
	ldr r0, _0222AE18 ; =ov13_0224F4C0
	ldr r2, _0222AE1C ; =ov13_0224521C
	ldr r3, [r0]
	ldrb r0, [r2, r1]
	add r1, r3, ip, lsl #2
	ldr r2, [r1, #0x38]
	ldr r1, [r2]
	bic r1, r1, #0xc00
	str r1, [r2]
	ldrh r1, [r2, #4]
	bic r1, r1, #0xf000
	orr r0, r1, r0, lsl #12
	strh r0, [r2, #4]
	ldmia sp!, {r3, pc}
_0222ADEC:
	ldr r2, _0222AE18 ; =ov13_0224F4C0
	ldr r3, _0222AE20 ; =ov13_02245210
	ldr r2, [r2]
	sub r0, r0, #0xc
	add r0, r2, r0, lsl #2
	ldrb r3, [r3, r1]
	ldr r0, [r0, #0x40]
	mvn r1, #0
	mov r2, #0
	bl ov13_0223FE98
	ldmia sp!, {r3, pc}
	.balign 4, 0
_0222AE18: .word ov13_0224F4C0
_0222AE1C: .word ov13_0224521C
_0222AE20: .word ov13_02245210
	arm_func_end ov13_0222AD5C

	arm_func_start ov13_0222AE24
ov13_0222AE24: ; 0x0222AE24
	stmdb sp!, {r4, lr}
	ldr r0, _0222AEA4 ; =ov13_0224F4C0
	ldr r1, [r0]
	ldrsb r0, [r1, #0x63]
	cmp r0, #0xb
	movle r4, #0x44
	ldr r0, [r1, #0x58]
	movgt r4, #0x45
	mov r1, #0
	bl ov13_0223FDE0
	mov r2, r0
	mov r1, r4
	mov r0, #0
	bl ov13_022400F0
	ldr r0, _0222AEA4 ; =ov13_0224F4C0
	mvn r1, #0
	ldr r0, [r0]
	mov r2, #2
	ldr r0, [r0, #0x58]
	bl ov13_02240008
	ldr r0, _0222AEA4 ; =ov13_0224F4C0
	ldr r2, _0222AEA8 ; =ov13_022452A2
	ldr r0, [r0]
	ldr r3, _0222AEAC ; =ov13_022452A4
	ldrsb ip, [r0, #0x63]
	ldr r0, [r0, #0x58]
	mvn r1, #0
	mov ip, ip, lsl #2
	ldrh r2, [r2, ip]
	ldrh r3, [r3, ip]
	bl ov13_0223FF18
	ldmia sp!, {r4, pc}
	.balign 4, 0
_0222AEA4: .word ov13_0224F4C0
_0222AEA8: .word ov13_022452A2
_0222AEAC: .word ov13_022452A4
	arm_func_end ov13_0222AE24

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

	arm_func_start ov13_0222B364
ov13_0222B364: ; 0x0222B364
	stmdb sp!, {r4, lr}
	mov r4, r0
	bl ov13_02222CE0
	cmp r4, #0
	beq _0222B3A0
	ldr r0, _0222B3AC ; =ov13_0224F4C4
	ldr r2, [r0, #4]
	ldrb r1, [r2, #0x116]
	cmp r1, #0
	ldreqb r0, [r0]
	cmpeq r0, #1
	bne _0222B3A0
	add r0, r2, #0x17
	add r0, r0, #0x100
	bl ov13_0223A6E4
_0222B3A0:
	ldr r0, _0222B3B0 ; =ov13_0224F4C8
	bl ov13_022412B0
	ldmia sp!, {r4, pc}
	.balign 4, 0
_0222B3AC: .word ov13_0224F4C4
_0222B3B0: .word ov13_0224F4C8
	arm_func_end ov13_0222B364

	arm_func_start ov13_0222B3B4
ov13_0222B3B4: ; 0x0222B3B4
	stmdb sp!, {r3, lr}
	ldr r0, _0222B410 ; =ov13_0224F4C4
	ldr r0, [r0, #4]
	bl ov13_02220908
	cmp r0, #0
	bne _0222B3DC
	ldr r1, _0222B410 ; =ov13_0224F4C4
	mov r0, #1
	strb r0, [r1]
	ldmia sp!, {r3, pc}
_0222B3DC:
	ldr r0, _0222B410 ; =ov13_0224F4C4
	ldr r0, [r0, #4]
	ldrb r0, [r0, #0x116]
	cmp r0, #1
	beq _0222B400
	add r0, r0, #0xfd
	and r0, r0, #0xff
	cmp r0, #2
	bhi _0222B408
_0222B400:
	mov r0, #0
	ldmia sp!, {r3, pc}
_0222B408:
	mov r0, #2
	ldmia sp!, {r3, pc}
	.balign 4, 0
_0222B410: .word ov13_0224F4C4
	arm_func_end ov13_0222B3B4

	arm_func_start ov13_0222B414
ov13_0222B414: ; 0x0222B414
	ldr ip, _0222B420 ; =ov13_0224128C
	mov r1, #0x20
	bx ip
	.balign 4, 0
_0222B420: .word ov13_0224128C
	arm_func_end ov13_0222B414

	arm_func_start ov13_0222B424
ov13_0222B424: ; 0x0222B424
	ldr ip, _0222B42C ; =ov13_022412F4
	bx ip
	.balign 4, 0
_0222B42C: .word ov13_022412F4
	arm_func_end ov13_0222B424

	arm_func_start ov13_0222B430
ov13_0222B430: ; 0x0222B430
	stmdb sp!, {r3, r4, r5, lr}
	ldr r3, _0222B4B4 ; =ov13_0224F4CC
	str r0, [r3, #4]
	mov r0, r1
	mov r1, r2
	bl ov13_0222B4BC
	cmp r0, #0
	mvneq r0, #0
	ldmeqia sp!, {r3, r4, r5, pc}
	ldr r0, _0222B4B4 ; =ov13_0224F4CC
	mov r1, #0
	strb r1, [r0, #1]
	bl ov13_0222B558
	bl ov13_0222B620
	bl ov13_0223AAFC
	ldr r0, _0222B4B8 ; =ov13_0222C1BC
	bl ov13_0222B8E8
	ldr r4, _0222B4B4 ; =ov13_0224F4CC
	mov r5, #0
_0222B47C:
	bl ov13_022413E4
	ldr r0, [r4, #0xc]
	blx r0
	mov r0, r5
	bl ov13_02242478
	bl ov13_02241A44
	bl ov13_022422E0
	bl OS_WaitVBlankIntr
	ldrb r0, [r4, #1]
	cmp r0, #0
	beq _0222B47C
	bl ov13_0222B898
	mov r0, #0
	ldmia sp!, {r3, r4, r5, pc}
	.balign 4, 0
_0222B4B4: .word ov13_0224F4CC
_0222B4B8: .word ov13_0222C1BC
	arm_func_end ov13_0222B430

	arm_func_start ov13_0222B4BC
ov13_0222B4BC: ; 0x0222B4BC
	ldr r2, _0222B554 ; =ov13_0224F4CC
	cmp r0, #0
	strb r0, [r2]
	str r1, [r2, #8]
	blt _0222B4D8
	cmp r0, #6
	ble _0222B4E0
_0222B4D8:
	mov r0, #0
	bx lr
_0222B4E0:
	cmp r0, #1
	bne _0222B4F8
	mov r1, r1, lsr #4
	bics r1, r1, #2
	movne r0, #0
	bxne lr
_0222B4F8:
	ldr r1, _0222B554 ; =ov13_0224F4CC
	ldr r1, [r1, #8]
	mov r2, r1, lsl #0x1c
	mov r2, r2, lsr #0x1c
	cmp r2, #1
	movhi r0, #0
	bxhi lr
	cmp r0, #0
	beq _0222B52C
	mov r1, r1, lsr #4
	tst r1, #1
	movne r0, #0
	bxne lr
_0222B52C:
	cmp r0, #0
	bne _0222B54C
	ldr r0, _0222B554 ; =ov13_0224F4CC
	ldr r0, [r0, #8]
	mov r0, r0, lsr #4
	tst r0, #1
	moveq r0, #0
	bxeq lr
_0222B54C:
	mov r0, #1
	bx lr
	.balign 4, 0
_0222B554: .word ov13_0224F4CC
	arm_func_end ov13_0222B4BC

	arm_func_start ov13_0222B558
ov13_0222B558: ; 0x0222B558
	stmdb sp!, {r3, lr}
	ldr r2, _0222B614 ; =0x04000208
	mov r0, #0
	ldrh r1, [r2]
	strh r0, [r2]
	bl GX_DispOff
	ldr r1, _0222B618 ; =0x04001000
	ldr r0, [r1]
	bic r0, r0, #0x10000
	str r0, [r1]
	bl OS_IsTickAvailable
	cmp r0, #0
	bne _0222B590
	bl OS_Terminate
_0222B590:
	bl OS_IsAlarmAvailable
	cmp r0, #0
	bne _0222B5A0
	bl OS_Terminate
_0222B5A0:
	mov r0, #0
	bl GX_VBlankIntr
	bl FX_Init
	mvn r0, #0
	bl FS_Init
	bl TP_Init
	bl RTC_Init
	bl GX_DispOff
	ldr r1, _0222B618 ; =0x04001000
	ldr r0, [r1]
	bic r0, r0, #0x10000
	str r0, [r1]
	bl ov13_022420A0
	ldr r0, _0222B61C ; =ov13_0224F4CC
	ldr r0, [r0, #4]
	bl ov13_022411DC
	bl ov13_02241AC0
	bl ov13_0224231C
	bl ov13_0223F664
	bl ov13_02241330
	bl ov13_02240338
	mov r0, #0x700
	mov r1, #0x20
	bl ov13_0224128C
	str r0, [sp]
	bl DWC_BM_Init
	add r0, sp, #0
	bl ov13_022412B0
	ldmia sp!, {r3, pc}
	.balign 4, 0
_0222B614: .word 0x04000208
_0222B618: .word 0x04001000
_0222B61C: .word ov13_0224F4CC
	arm_func_end ov13_0222B558

	arm_func_start ov13_0222B620
ov13_0222B620: ; 0x0222B620
	stmdb sp!, {r4, lr}
	mov r0, #0
	bl GX_VBlankIntr
	mov r0, #1
	bl GX_SetBankForBG
	mov r0, #2
	bl GX_SetBankForOBJ
	mov r1, #0
	mov r2, r1
	mov r0, #1
	bl GX_SetGraphicsMode
	mov r3, #0x4000000
	ldr r1, [r3]
	add r0, r3, #0x6c
	bic r1, r1, #0x1f00
	str r1, [r3]
	ldr r2, [r3]
	mov r1, #0
	bic r2, r2, #0xe000
	str r2, [r3]
	bl GXx_SetMasterBrightness_
	mov r3, #0x4000000
	ldr r2, [r3]
	ldr r0, _0222B884 ; =0xFFCFFFEF
	ldr r1, _0222B888 ; =0x00200010
	and r0, r2, r0
	orr r0, r0, #0x10
	add ip, r1, #0x3e00000
	orr r0, r0, #0x200000
	str r0, [r3]
	ldrh r1, [r3, #8]
	mov lr, #0
	add r0, r3, #0x50
	bic r1, r1, #0x40
	strh r1, [r3, #8]
	ldrh r4, [r3, #0xa]
	mov r1, #0x3f
	mov r2, #0x10
	bic r4, r4, #0x40
	strh r4, [r3, #0xa]
	ldrh r4, [r3, #0xc]
	bic r4, r4, #0x40
	strh r4, [r3, #0xc]
	ldrh r4, [r3, #0xe]
	bic r4, r4, #0x40
	strh r4, [r3, #0xe]
	str lr, [ip]
	str lr, [r3, #0x14]
	str lr, [r3, #0x18]
	str lr, [r3, #0x1c]
	bl G2x_SetBlendBrightness_
	mov r0, #0x80
	bl GX_SetBankForSubBG
	mov r0, #0x100
	bl GX_SetBankForSubOBJ
	mov r0, #0
	bl GXS_SetGraphicsMode
	ldr r1, _0222B88C ; =0x04001000
	ldr r0, [r1]
	bic r0, r0, #0x1f00
	str r0, [r1]
	ldr r0, [r1]
	bic r0, r0, #0xe000
	str r0, [r1]
	add r0, r1, #0x6c
	mov r1, #0
	bl GXx_SetMasterBrightness_
	ldr r3, _0222B88C ; =0x04001000
	ldr r0, _0222B884 ; =0xFFCFFFEF
	ldr r1, [r3]
	mov ip, #0
	and r0, r1, r0
	orr r0, r0, #0x10
	str r0, [r3]
	ldrh r2, [r3, #8]
	add r0, r3, #0x50
	mov r1, #0x3f
	bic r2, r2, #0x40
	strh r2, [r3, #8]
	ldrh lr, [r3, #0xa]
	mov r2, #0x10
	bic lr, lr, #0x40
	strh lr, [r3, #0xa]
	ldrh lr, [r3, #0xc]
	bic lr, lr, #0x40
	strh lr, [r3, #0xc]
	ldrh lr, [r3, #0xe]
	bic lr, lr, #0x40
	strh lr, [r3, #0xe]
	str ip, [r3, #0x10]
	str ip, [r3, #0x14]
	str ip, [r3, #0x18]
	str ip, [r3, #0x1c]
	bl G2x_SetBlendBrightness_
	ldr r2, _0222B890 ; =0x04000008
	ldr r3, _0222B894 ; =0x0400100A
	ldrh r0, [r2]
	add r1, r2, #0x1000
	and r0, r0, #0x43
	orr r0, r0, #0xc00
	strh r0, [r2]
	ldrh r0, [r2, #2]
	and r0, r0, #0x43
	orr r0, r0, #0x108
	orr r0, r0, #0xc00
	strh r0, [r2, #2]
	ldrh r0, [r2, #4]
	and r0, r0, #0x43
	orr r0, r0, #0xe10
	strh r0, [r2, #4]
	ldrh r0, [r2, #6]
	and r0, r0, #0x43
	orr r0, r0, #0xf10
	strh r0, [r2, #6]
	ldrh r0, [r1]
	and r0, r0, #0x43
	orr r0, r0, #0xc00
	strh r0, [r1]
	ldrh r0, [r3]
	and r0, r0, #0x43
	orr r0, r0, #0xd00
	strh r0, [r3]
	ldrh r0, [r3, #2]
	and r0, r0, #0x43
	orr r0, r0, #0xe00
	strh r0, [r3, #2]
	ldrh r0, [r3, #4]
	mov r2, #0x4000000
	add r1, r2, #0x304
	and r0, r0, #0x43
	orr r0, r0, #0xf00
	strh r0, [r3, #4]
	ldr r0, [r2]
	bic r0, r0, #0x38000000
	str r0, [r2]
	ldr r0, [r2]
	bic r0, r0, #0x7000000
	str r0, [r2]
	ldrh r0, [r1]
	bic r0, r0, #0x8000
	strh r0, [r1]
	bl ov13_02240648
	bl ov13_02241C68
	bl ov13_02241DFC
	bl ov13_0223FADC
	bl GX_DispOn
	ldr r2, _0222B88C ; =0x04001000
	mov r0, #1
	ldr r1, [r2]
	orr r1, r1, #0x10000
	str r1, [r2]
	bl GX_VBlankIntr
	ldmia sp!, {r4, pc}
	.balign 4, 0
_0222B884: .word 0xFFCFFFEF
_0222B888: .word 0x00200010
_0222B88C: .word 0x04001000
_0222B890: .word 0x04000008
_0222B894: .word 0x0400100A
	arm_func_end ov13_0222B620
