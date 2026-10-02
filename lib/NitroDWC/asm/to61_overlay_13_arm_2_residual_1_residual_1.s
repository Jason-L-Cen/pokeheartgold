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
	.public ov13_02227130
	.extern ov13_022272FC
	.extern ov13_02227390

.text
	arm_func_start ov13_02226F58
ov13_02226F58: ; 0x02226F58
	stmdb sp!, {r4, r5, r6, lr}
	mov r4, r0
	mov r0, #0xc
	mov r1, #4
	bl ov13_0224128C
	ldr r2, _02227018 ; =ov13_0224F464
	mov r1, #0x47
	str r0, [r2]
	strb r4, [r0, #8]
	mov r0, #0
	bl ov13_02240184
	ldr lr, _02227018 ; =ov13_0224F464
	ldr r1, _0222701C ; =ov13_02244EB0
	ldr r2, [lr]
	ldrb r4, [r1, r4]
	str r0, [r2, #4]
	ldr r0, [lr]
	ldr ip, _02227020 ; =0xFE00FF00
	ldr r5, [r0, #4]
	ldr r1, _02227024 ; =ov13_02227064
	ldrh r3, [r5, #4]
	mov r0, #1
	mov r2, #0
	bic r3, r3, #0xc00
	orr r3, r3, #0x400
	strh r3, [r5, #4]
	ldr r5, [lr]
	mov r3, #0x78
	ldr r6, [r5, #4]
	ldr r5, [r6]
	bic r5, r5, #0xc00
	str r5, [r6]
	ldrh r5, [r6, #4]
	bic r5, r5, #0xf000
	orr r4, r5, r4, lsl #12
	strh r4, [r6, #4]
	ldr r4, [lr]
	ldr r4, [r4, #4]
	ldr lr, [r4]
	and ip, lr, ip
	orr ip, ip, #0x8b
	orr ip, ip, #0xe60000
	str ip, [r4]
	bl ov13_02242528
	ldr r1, _02227018 ; =ov13_0224F464
	ldr r1, [r1]
	str r0, [r1]
	ldmia sp!, {r4, r5, r6, pc}
	.balign 4, 0
_02227018: .word ov13_0224F464
_0222701C: .word ov13_02244EB0
_02227020: .word 0xFE00FF00
_02227024: .word ov13_02227064
	arm_func_end ov13_02226F58

	arm_func_start ov13_02227028
ov13_02227028: ; 0x02227028
	stmdb sp!, {r3, lr}
	ldr r1, _0222705C ; =ov13_0224F464
	mov r0, #1
	ldr r1, [r1]
	ldr r1, [r1]
	bl ov13_022425F0
	ldr r0, _0222705C ; =ov13_0224F464
	ldr r0, [r0]
	ldr r0, [r0, #4]
	bl ov13_02241D98
	ldr r0, _02227060 ; =ov13_0224F464
	bl ov13_022412B0
	ldmia sp!, {r3, pc}
	.balign 4, 0
_0222705C: .word ov13_0224F464
_02227060: .word ov13_0224F464
	arm_func_end ov13_02227028

	arm_func_start ov13_02227064
ov13_02227064: ; 0x02227064
	stmdb sp!, {r3, lr}
	ldr r0, _02227124 ; =ov13_0224F464
	mov r1, #0x28
	ldr r0, [r0]
	ldrb r0, [r0, #9]
	add r0, r0, #1
	bl FX_ModS32
	ldr r2, _02227124 ; =ov13_0224F464
	mov r1, #5
	ldr r3, [r2]
	strb r0, [r3, #9]
	ldr r0, [r2]
	ldrb r0, [r0, #9]
	bl FX_DivS32
	ldr r2, _02227124 ; =ov13_0224F464
	add r1, r0, #0x47
	ldr r2, [r2]
	mov r0, #0
	ldr r2, [r2, #4]
	bl ov13_022400F0
	ldr r1, _02227124 ; =ov13_0224F464
	ldr ip, _02227128 ; =ov13_02244EB0
	ldr r2, [r1]
	ldr r0, _0222712C ; =0xFE00FF00
	ldr r3, [r2, #4]
	ldrh r2, [r3, #4]
	bic r2, r2, #0xc00
	orr r2, r2, #0x400
	strh r2, [r3, #4]
	ldr r2, [r1]
	ldrb lr, [r2, #8]
	ldr r3, [r2, #4]
	ldr r2, [r3]
	ldrb ip, [ip, lr]
	bic r2, r2, #0xc00
	str r2, [r3]
	ldrh r2, [r3, #4]
	bic r2, r2, #0xf000
	orr r2, r2, ip, lsl #12
	strh r2, [r3, #4]
	ldr r1, [r1]
	ldr r2, [r1, #4]
	ldr r1, [r2]
	and r0, r1, r0
	orr r0, r0, #0x8b
	orr r0, r0, #0xe60000
	str r0, [r2]
	ldmia sp!, {r3, pc}
	.balign 4, 0
_02227124: .word ov13_0224F464
_02227128: .word ov13_02244EB0
_0222712C: .word 0xFE00FF00
	arm_func_end ov13_02227064

	arm_func_start ov13_02227130
ov13_02227130: ; 0x02227130
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	ldr r2, _02227218 ; =ov13_02244EE0
	mov r5, r0
	mov r0, #0x1c
	mov r1, #4
	ldrb r8, [r2, r5]
	bl ov13_0224128C
	ldr r4, _0222721C ; =ov13_0224F468
	mvn r7, #1
	str r0, [r4]
	strb r7, [r0, #0x16]
	ldr r0, [r4]
	cmp r8, #0
	strb r5, [r0, #0x17]
	mov sb, #0
	ble _022271BC
	ldr r0, _02227220 ; =ov13_02244EC4
	mov r6, sb
	add sl, r0, r5, lsl #1
	mov r5, #1
	mov fp, r5
_02227184:
	ldrb r1, [sl], #1
	mov r0, r6
	mov r2, r5
	bl ov13_022401B0
	ldr r1, [r4]
	mov r2, fp
	str r0, [r1, sb, lsl #2]
	ldr r0, [r4]
	add r1, r7, #1
	ldr r0, [r0, sb, lsl #2]
	bl ov13_02240008
	add sb, sb, #1
	cmp sb, r8
	blt _02227184
_022271BC:
	mov r1, #1
	mov r2, r1
	mov r0, #0
	bl ov13_022401B0
	ldr r3, _0222721C ; =ov13_0224F468
	mvn r1, #0
	ldr r4, [r3]
	mov r2, #1
	str r0, [r4, #8]
	ldr r0, [r3]
	ldr r0, [r0, #8]
	bl ov13_02240008
	mov r0, #0xc0
	bl ov13_022272FC
	mov r0, #0
	ldr r1, _02227224 ; =ov13_02227390
	mov r2, r0
	mov r3, #0x78
	bl ov13_02242528
	ldr r1, _0222721C ; =ov13_0224F468
	ldr r1, [r1]
	str r0, [r1, #0xc]
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.balign 4, 0
_02227218: .word ov13_02244EE0
_0222721C: .word ov13_0224F468
_02227220: .word ov13_02244EC4
_02227224: .word ov13_02227390
	arm_func_end ov13_02227130
