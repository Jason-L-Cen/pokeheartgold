	.extern ov45_0223259C
	.extern ov45_02232AE8
	.extern ov45_02232D8C
	.extern ov45_02233ABC
	.extern ov45_02236680
	.extern ov45_02236BA8
	.extern ov45_02236C40
	.extern ov45_02236E58
	.extern ov45_02237634
	.extern ov45_02237664
	.extern ov45_022376BC
	.extern ov45_02237A24
	.extern ov45_02237B58
	.extern ov45_02237BC8
	.extern ov45_02237D48
	.extern ov45_0223D644
	.extern ov45_0223DAD4
	.extern ov45_0223EA34
	.extern ov45_02240F8C
	.extern ov45_022412CC
	.extern ov45_022421E4
	.extern ov45_02242254
	.extern ov45_02242394
	.extern ov45_0224246C
	.extern ov45_022424DC
	.extern ov45_02242510
	.extern ov45_0224268C
	.extern ov45_02242858
	.extern ov45_022428C8
	.extern ov45_022428D8
	.extern ov45_02242910
	.extern ov45_022429E0
	.extern ov45_02242A2C
	.extern ov45_02242C3C
	.extern ov45_02242D30
	.extern ov45_02242EF4
	.extern ov45_02243280
	.extern ov45_0224335C
	.extern ov45_022435F8
	.extern ov45_0224367C
	.extern ov45_022436E0
	.extern ov45_02243794
	.extern ov45_022437CC
	.extern ov45_02243808
	.extern ov45_022502D0
	.extern ov45_0225035C
	.extern ov45_02250424
	.extern ov45_02250590
	.extern ov45_02250E6C
	.extern ov45_02250F5C
	.extern ov45_02251024
	.extern ov45_02251208
	.extern ov45_02255238
	.extern ov45_022552C0
	.extern ov45_022552C8
	.extern ov45_022552D8
	.extern ov45_022552DC
	.extern ov45_022552E4
	.extern ov45_022552EC
	.extern ov45_022552F0
	.extern ov45_022552FC
	.extern ov45_02255304
	.extern ov45_02255308
	.extern ov45_02255310
	.extern ov45_02255318
	.extern ov45_0225531C
	.extern ov45_022577DC
	.include "asm/macros.inc"
	.include "overlay_45_arm.inc"
	.include "global.inc"
	.extern ov45_0223ED00
	.extern ov45_0223ED1C
	.extern ov45_0223EDD8
	.extern ov45_0223F1C8
	.extern ov45_0223F92C
	.extern ov45_0223F9C4
	.extern ov45_0223FA3C
	.extern ov45_0223FC5C
	.extern ov45_02240850

.text
	arm_func_start ov45_02240B10
ov45_02240B10: ; 0x02240B10
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, lr}
	sub sp, sp, #0x13c
	mov sl, r0
	ldr r0, _02240F30 ; =_version_NINTENDO_DWC_LOBBY
	mov sb, r1
	mov r8, r2
	mov r7, r3
	bl OSi_ReferSymbol
	ldr r0, _02240F34 ; =ov45_022577DC
	ldr r0, [r0]
	cmp r0, #0
	addne sp, sp, #0x13c
	movne r0, #4
	ldmneia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, pc}
	ldr r1, [sp, #0x160]
	add r0, sp, #0x130
	bl ov45_022429E0
	ldr r1, _02240F38 ; =ov45_02255308
	add r0, sp, #0x124
	bl ov45_02237B58
	ldr r1, _02240F3C ; =ov45_02255310
	add r0, sp, #0x118
	bl ov45_02237B58
	mov r4, #0
	mov r0, #0xac
	mov r5, r4
	bl ov45_02242394
	movs r6, r0
	beq _02240BE0
	add r0, sp, #0xf8
	mov r1, sb
	bl ov45_02237B58
	add r0, sp, #0xec
	mov r1, sl
	mov r4, #1
	bl ov45_02237B58
	ldr r1, [sp, #0x160]
	add r0, sp, #0x130
	str r1, [sp]
	str r0, [sp, #4]
	add r1, sp, #0x124
	str r1, [sp, #8]
	add r0, sp, #0x118
	str r0, [sp, #0xc]
	add r1, sp, #0xec
	add r2, sp, #0xf8
	mov r3, r8
	mov r0, r6
	str r7, [sp, #0x10]
	mov r5, r4
	bl ov45_0224268C
	mov r6, r0
_02240BE0:
	ldr r0, _02240F34 ; =ov45_022577DC
	cmp r5, #0
	str r6, [r0]
	beq _02240BF8
	add r0, sp, #0xec
	bl ov45_02237BC8
_02240BF8:
	cmp r4, #0
	beq _02240C08
	add r0, sp, #0xf8
	bl ov45_02237BC8
_02240C08:
	ldr r0, _02240F34 ; =ov45_022577DC
	ldr r0, [r0]
	cmp r0, #0
	bne _02240C3C
	add r0, sp, #0x118
	bl ov45_02237BC8
	add r0, sp, #0x124
	bl ov45_02237BC8
	add r0, sp, #0x130
	bl ov45_02237BC8
	add sp, sp, #0x13c
	mov r0, #1
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, pc}
_02240C3C:
	bl ov45_0224246C
	cmp r0, #0
	moveq r4, #1
	beq _02240EC8
	add r0, sp, #0x104
	mov r1, #0
	mov r2, #0x14
	bl MI_CpuFill8
	ldr r4, _02240F40 ; =ov45_0223ED00
	ldr r3, _02240F44 ; =ov45_0223ED1C
	ldr r1, _02240F48 ; =ov45_0223F1C8
	ldr r2, _02240F4C ; =ov45_0223EDD8
	ldr r0, _02240F34 ; =ov45_022577DC
	str r1, [sp, #0x110]
	ldr r1, [r0]
	str r4, [sp, #0x104]
	str r3, [sp, #0x108]
	str r2, [sp, #0x10c]
	ldr r2, [r7, #8]
	add r0, sp, #0xe0
	str r2, [sp, #0x114]
	bl ov45_02240F6C
	ldr r2, _02240F50 ; =ov45_02255318
	add r0, sp, #0xd4
	add r1, sp, #0xe0
	bl ov45_02233ABC
	ldr r2, _02240F54 ; =ov45_0225531C
	add r0, sp, #0xc8
	add r1, sp, #0xd4
	bl ov45_02233ABC
	ldr r1, _02240F34 ; =ov45_022577DC
	add r0, sp, #0xbc
	ldr r1, [r1]
	bl ov45_0223FC5C
	ldr r1, _02240F34 ; =ov45_022577DC
	add r0, sp, #0xb0
	ldr r1, [r1]
	bl ov45_02240F7C
	ldr r1, _02240F34 ; =ov45_022577DC
	add r0, sp, #0xa4
	ldr r1, [r1]
	bl ov45_02240F6C
	ldr r1, _02240F34 ; =ov45_022577DC
	add r0, sp, #0x98
	ldr r1, [r1]
	bl ov45_02240F8C
	ldr r0, [sp, #0x98]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	ldr r0, [sp, #0xa4]
	addeq r1, sp, #0x99
	mov r0, r0, lsl #0x1f
	ldrne r1, [sp, #0xa0]
	movs r0, r0, lsr #0x1f
	ldr r0, [sp, #0xb0]
	addeq r4, sp, #0xa5
	mov r0, r0, lsl #0x1f
	ldrne r4, [sp, #0xac]
	movs r0, r0, lsr #0x1f
	ldr r0, [sp, #0xbc]
	addeq r3, sp, #0xb1
	mov r0, r0, lsl #0x1f
	ldrne r3, [sp, #0xb8]
	movs r0, r0, lsr #0x1f
	ldr r0, [sp, #0xc8]
	addeq r2, sp, #0xbd
	mov r0, r0, lsl #0x1f
	ldrne r2, [sp, #0xc4]
	movs r0, r0, lsr #0x1f
	addeq r0, sp, #0xc9
	ldrne r0, [sp, #0xd0]
	str r4, [sp]
	str r1, [sp, #4]
	add r4, sp, #0x104
	str r4, [sp, #8]
	ldr r1, _02240F58 ; =ov45_0223FA3C
	ldr r4, _02240F5C ; =ov45_0223F9C4
	str r1, [sp, #0xc]
	ldr r1, _02240F60 ; =ov45_0223F92C
	str r4, [sp, #0x10]
	str r1, [sp, #0x14]
	ldr r4, [r7, #8]
	ldr r1, _02240F64 ; =0x00001A0B
	str r4, [sp, #0x18]
	mov r4, #0
	str r4, [sp, #0x1c]
	bl ov45_022502D0
	mov r4, r0
	add r0, sp, #0x98
	bl ov45_02237BC8
	add r0, sp, #0xa4
	bl ov45_02237BC8
	add r0, sp, #0xb0
	bl ov45_02237BC8
	add r0, sp, #0xbc
	bl ov45_02237BC8
	add r0, sp, #0xc8
	bl ov45_02237BC8
	add r0, sp, #0xd4
	bl ov45_02237BC8
	add r0, sp, #0xe0
	bl ov45_02237BC8
	cmp r4, #0
	moveq r4, #2
	beq _02240EC8
	ldr r0, _02240F34 ; =ov45_022577DC
	ldr r2, _02240F34 ; =ov45_022577DC
	ldr r1, [r0]
	str r4, [r1]
	ldr r1, [r0]
	ldr r0, [r1, #4]
	cmp r0, #5
	movne r0, #1
	strne r0, [r1, #4]
	add r0, sp, #0x88
	add r1, sp, #0x7c
	ldr r5, [r2]
	bl ov00_021ECB94
	add r0, sp, #0x88
	add r1, sp, #0x7c
	bl RTC_ConvertDateTimeToSecond
	ldr r3, _02240F68 ; =ov45_02240850
	str r0, [sp, #0x2c]
	str r1, [sp, #0x30]
	mov r4, #0x1e
	mov r1, #0
	add r0, sp, #0x44
	add r2, sp, #0x2c
	str r4, [sp, #0x34]
	str r1, [sp, #0x38]
	str r5, [sp, #0x3c]
	str r3, [sp, #0x40]
	bl ov45_02232AE8
	mov r0, #0
	add r2, sp, #0x48
	strb r0, [sp, #0x21]
	strb r0, [sp, #0x20]
	ldr r0, [sp, #0x44]
	add r7, sp, #0x64
	str r0, [sp, #0x60]
	add r1, r5, #0x9c
	add r6, r2, #8
	ldr r4, [r2, #0x10]
	ldr r0, [r2, #0x14]
	ldmia r2, {r2, r3}
	stmia r7, {r2, r3}
	add r5, r7, #8
	ldmia r6, {r2, r3}
	stmia r5, {r2, r3}
	str r0, [r7, #0x14]
	add r0, sp, #0x24
	add r2, sp, #0x60
	str r4, [r7, #0x10]
	bl ov45_02236BA8
	add r0, sp, #0x118
	bl ov45_02237BC8
	add r0, sp, #0x124
	bl ov45_02237BC8
	add r0, sp, #0x130
	bl ov45_02237BC8
	add sp, sp, #0x13c
	mov r0, #0
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, pc}
_02240EC8:
	bl ov45_022424DC
	ldr r0, _02240F34 ; =ov45_022577DC
	ldr r5, [r0]
	cmp r5, #0
	beq _02240F0C
	beq _02240F00
	mov r0, r5
	bl ov45_02242858
	cmp r5, #0
	beq _02240F00
	mov r0, #0
	mov r1, r5
	mov r2, r0
	bl ov00_021EC2EC
_02240F00:
	ldr r0, _02240F34 ; =ov45_022577DC
	mov r1, #0
	str r1, [r0]
_02240F0C:
	add r0, sp, #0x118
	bl ov45_02237BC8
	add r0, sp, #0x124
	bl ov45_02237BC8
	add r0, sp, #0x130
	bl ov45_02237BC8
	mov r0, r4
	add sp, sp, #0x13c
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, pc}
	.balign 4, 0
_02240F30: .word _version_NINTENDO_DWC_LOBBY
_02240F34: .word ov45_022577DC
_02240F38: .word ov45_02255308
_02240F3C: .word ov45_02255310
_02240F40: .word ov45_0223ED00
_02240F44: .word ov45_0223ED1C
_02240F48: .word ov45_0223F1C8
_02240F4C: .word ov45_0223EDD8
_02240F50: .word ov45_02255318
_02240F54: .word ov45_0225531C
_02240F58: .word ov45_0223FA3C
_02240F5C: .word ov45_0223F9C4
_02240F60: .word ov45_0223F92C
_02240F64: .word 0x00001A0B
_02240F68: .word ov45_02240850
	arm_func_end ov45_02240B10

	arm_func_start ov45_02240F6C
ov45_02240F6C: ; 0x02240F6C
	ldr ip, _02240F78 ; =ov45_02237A24
	add r1, r1, #0x30
	bx ip
	.balign 4, 0
_02240F78: .word ov45_02237A24
	arm_func_end ov45_02240F6C

	arm_func_start ov45_02240F7C
ov45_02240F7C: ; 0x02240F7C
	ldr ip, _02240F88 ; =ov45_02237A24
	add r1, r1, #0x24
	bx ip
	.balign 4, 0
_02240F88: .word ov45_02237A24
	arm_func_end ov45_02240F7C
