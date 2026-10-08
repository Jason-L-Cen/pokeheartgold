	.extern ov13_02228C08
	.extern ov13_02228C38
	.extern ov13_022290F0
	.extern ov13_022294B8
	.extern ov13_0222A0DC
	.extern ov13_0222AAFC
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
	.public ov13_02229C20

.text
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
