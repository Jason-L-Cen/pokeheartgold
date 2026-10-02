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
	.extern ov13_02228444
	.extern ov13_022290F0

.text
	arm_func_start ov13_02228054
ov13_02228054: ; 0x02228054
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x24
	add r5, sp, #0x1c
	mov r4, #0
	strh r4, [r5, #4]
	mov r3, #0x11
	strh r4, [r5, #6]
	mov r2, #0x12
	mov r0, #0x128
	mov r1, #4
	strh r4, [r5]
	strh r4, [r5, #2]
	strh r3, [sp, #0x20]
	strh r2, [sp, #0x22]
	bl ov13_0224128C
	ldr r8, _022283A0 ; =ov13_0224F4BC
	mov r1, #0xff
	str r0, [r8]
	strb r1, [r0, #0x11c]
	ldr r0, [r8]
	mov r6, r4
	strb r6, [r0, #0x121]
	mov r5, r4
	ldr r0, [r8]
	mov r1, #1
	strb r1, [r0, #0x123]
	ldr r0, [r8]
	ldr r7, _022283A4 ; =0xC1FFFCFF
	strb r1, [r0, #0x124]
	mov r4, #0x34
_022280CC:
	mov r0, r5
	mov r1, r4
	bl ov13_02240184
	ldr r1, [r8]
	add r1, r1, r6, lsl #2
	str r0, [r1, #0x30]
	ldr r0, [r8]
	add r0, r0, r6, lsl #2
	ldr r1, [r0, #0x30]
	ldr r0, [r1]
	and r0, r0, r7
	orr r0, r0, #0x200
	str r0, [r1]
	ldr r0, [r8]
	add r0, r0, r6, lsl #2
	ldr r1, [r0, #0x30]
	add r6, r6, #1
	ldrh r0, [r1, #4]
	cmp r6, #0x2f
	bic r0, r0, #0xc00
	orr r0, r0, #0xc00
	strh r0, [r1, #4]
	blt _022280CC
	mov r6, #0
	ldr r5, _022283A8 ; =ov13_02244F5C
	ldr r8, _022283A0 ; =ov13_0224F4BC
	ldr r7, _022283A4 ; =0xC1FFFCFF
	mov r4, r6
_0222813C:
	ldrb r1, [r5], #1
	mov r0, r4
	bl ov13_02240184
	ldr r1, [r8]
	add r1, r1, r6, lsl #2
	str r0, [r1, #0xec]
	ldr r0, [r8]
	add r0, r0, r6, lsl #2
	ldr r1, [r0, #0xec]
	ldr r0, [r1]
	and r0, r0, r7
	orr r0, r0, #0x200
	str r0, [r1]
	ldr r0, [r8]
	add r0, r0, r6, lsl #2
	ldr r1, [r0, #0xec]
	add r6, r6, #1
	ldrh r0, [r1, #4]
	cmp r6, #4
	bic r0, r0, #0xc00
	orr r0, r0, #0xc00
	strh r0, [r1, #4]
	blt _0222813C
	mov sl, #0
	ldr sb, _022283AC ; =ov13_02244F54
	ldr r4, _022283A0 ; =ov13_0224F4BC
	mov r8, sl
	mov r7, #1
	mvn r6, #0
	mov r5, #0x200
	mov fp, sl
_022281B8:
	ldrb r1, [sb], #1
	mov r0, r8
	mov r2, r7
	bl ov13_022401B0
	ldr r2, [r4]
	mov r1, r6
	add r2, r2, sl, lsl #2
	str r0, [r2, #0xfc]
	ldr r0, [r4]
	mov r2, r5
	add r0, r0, sl, lsl #2
	ldr r0, [r0, #0xfc]
	mov r3, fp
	bl ov13_0223FDF4
	ldr r0, [r4]
	mov r1, r6
	add r0, r0, sl, lsl #2
	ldr r0, [r0, #0xfc]
	mov r2, #3
	bl ov13_02240008
	add sl, sl, #1
	cmp sl, #2
	blt _022281B8
	mov sl, #0
	strh sl, [sp, #0x16]
_0222821C:
	mov sb, #0
	ldr r6, _022283B0 ; =ov13_02245D74
	ldr r4, _022283A0 ; =ov13_0224F4BC
	str sb, [sp, #0x10]
	mov fp, #2
_02228230:
	add r0, sp, #0x18
	str r0, [sp]
	mov r0, #0
	str r0, [sp, #4]
	mov r1, #0x1c
	mov r2, #2
	mov r3, r0
	bl ov13_02240798
	ldr r1, [r4]
	mov r2, sb, lsl #2
	add r1, r1, sl, lsl #4
	str r0, [r1, sb, lsl #2]
	mov r0, #0
	ldr r7, [sp, #0x10]
	strh r0, [sp, #0x1c]
	mov r8, r0
	add r5, r2, sl, lsl #4
_02228274:
	ldr r1, [r6, sl, lsl #2]
	mov r0, r7, lsl #1
	ldrh r1, [r1, r0]
	ldrh r0, [sp, #0x22]
	strh r1, [sp, #0x14]
	stmia sp, {r0, fp}
	mov r0, #0x480
	str r0, [sp, #8]
	add r0, sp, #0x14
	str r0, [sp, #0xc]
	ldr r0, [r4]
	ldrh r1, [sp, #0x1c]
	ldrh r2, [sp, #0x1e]
	ldrh r3, [sp, #0x20]
	ldr r0, [r0, r5]
	bl ov13_02240CC0
	ldrh r0, [sp, #0x1c]
	add r8, r8, #1
	cmp r8, #0xc
	add r0, r0, #0x12
	add r7, r7, #1
	strh r0, [sp, #0x1c]
	blt _02228274
	cmp sl, #0
	bne _022282F4
	ldr r1, [sp, #0x18]
	mov r0, #0
	mov r2, #1
	bl ov13_0223FC04
	ldr r1, [r4]
	add r1, r1, sb, lsl #2
	str r0, [r1, #0x104]
_022282F4:
	ldr r0, [sp, #0x10]
	add sb, sb, #1
	add r0, r0, #0xc
	str r0, [sp, #0x10]
	cmp sb, #4
	blt _02228230
	add sl, sl, #1
	cmp sl, #3
	blt _0222821C
	mov r0, #0
	mov r1, #0x40
	mov r2, #1
	bl ov13_022401B0
	ldr r3, _022283A0 ; =ov13_0224F4BC
	mvn r1, #0
	ldr r4, [r3]
	mov r2, #0x200
	str r0, [r4, #0x114]
	ldr r0, [r3]
	mov r3, #0
	ldr r0, [r0, #0x114]
	bl ov13_0223FDF4
	ldr r0, _022283A0 ; =ov13_0224F4BC
	mvn r1, #0
	ldr r0, [r0]
	mov r2, #2
	ldr r0, [r0, #0x114]
	bl ov13_02240008
	mov r0, #0
	ldr r1, _022283B4 ; =ov13_02228444
	mov r2, r0
	mov r3, #0x78
	bl ov13_02242528
	ldr r3, _022283A0 ; =ov13_0224F4BC
	mov r1, #0
	ldr r4, [r3]
	mov r2, #0xc0
	str r0, [r4, #0x118]
	ldr r0, [r3]
	ldrb r0, [r0, #0x11d]
	bl ov13_022290F0
	add sp, sp, #0x24
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.balign 4, 0
_022283A0: .word ov13_0224F4BC
_022283A4: .word 0xC1FFFCFF
_022283A8: .word ov13_02244F5C
_022283AC: .word ov13_02244F54
_022283B0: .word ov13_02245D74
_022283B4: .word ov13_02228444
	arm_func_end ov13_02228054
