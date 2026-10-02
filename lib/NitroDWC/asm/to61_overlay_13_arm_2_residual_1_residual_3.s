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
	.extern ov13_02227FAC

.text
	arm_func_start ov13_02227EB0
ov13_02227EB0: ; 0x02227EB0
	stmdb sp!, {r3, r4, r5, lr}
	ldr r1, _02227F58 ; =ov13_0224F4B8
	mov r4, r0
	ldr r0, [r1]
	cmp r0, #0
	ldmneia sp!, {r3, r4, r5, pc}
	mov r0, #0xc
	mov r1, #4
	bl ov13_0224128C
	ldr r1, _02227F5C ; =ov13_02244F44
	ldr r2, _02227F58 ; =ov13_0224F4B8
	ldrb r1, [r1, r4, lsl #2]
	str r0, [r2]
	strb r4, [r0, #8]
	mov r0, #0
	bl ov13_02240184
	ldr lr, _02227F58 ; =ov13_0224F4B8
	ldr ip, _02227F60 ; =0xFE00FF00
	ldr r2, [lr]
	ldr r1, _02227F64 ; =ov13_02227FAC
	str r0, [r2]
	ldr r2, [lr]
	mov r0, #0
	ldr r5, [r2]
	mov r2, r0
	ldr r4, [r5]
	mov r3, #0x78
	and r4, r4, ip
	orr r4, r4, #0x26
	orr r4, r4, #0xe50000
	str r4, [r5]
	ldr ip, [lr]
	ldr lr, [ip]
	ldrh ip, [lr, #4]
	bic ip, ip, #0xc00
	orr ip, ip, #0x800
	strh ip, [lr, #4]
	bl ov13_02242528
	ldr r1, _02227F58 ; =ov13_0224F4B8
	ldr r1, [r1]
	str r0, [r1, #4]
	ldmia sp!, {r3, r4, r5, pc}
	.balign 4, 0
_02227F58: .word ov13_0224F4B8
_02227F5C: .word ov13_02244F44
_02227F60: .word 0xFE00FF00
_02227F64: .word ov13_02227FAC
	arm_func_end ov13_02227EB0

	arm_func_start ov13_02227F68
ov13_02227F68: ; 0x02227F68
	stmdb sp!, {r3, lr}
	ldr r0, _02227FA4 ; =ov13_0224F4B8
	ldr r0, [r0]
	cmp r0, #0
	ldmeqia sp!, {r3, pc}
	ldr r1, [r0, #4]
	mov r0, #0
	bl ov13_022425F0
	ldr r0, _02227FA4 ; =ov13_0224F4B8
	ldr r0, [r0]
	ldr r0, [r0]
	bl ov13_02241D98
	ldr r0, _02227FA8 ; =ov13_0224F4B8
	bl ov13_022412B0
	ldmia sp!, {r3, pc}
	.balign 4, 0
_02227FA4: .word ov13_0224F4B8
_02227FA8: .word ov13_0224F4B8
	arm_func_end ov13_02227F68
