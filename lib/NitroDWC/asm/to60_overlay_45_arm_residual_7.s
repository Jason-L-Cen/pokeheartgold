	.include "asm/macros.inc"
	.include "overlay_45_arm.inc"
	.include "global.inc"
	.public ov45_0223E100
	.extern ov45_02231C78
	.extern ov45_02232760
	.extern ov45_022327B8
	.extern ov45_022379D4
	.extern ov45_022383E0
	.extern ov45_02254FB0

.text
	arm_func_start ov45_0223E100
ov45_0223E100: ; 0x0223E100
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, lr}
	sub sp, sp, #0xd4
	ldr ip, _0223E490 ; =ov45_02254FB0
	mov r6, r0
	mov sb, r1
	add r8, sp, #0x9c
	mov r7, #0
	mov sl, #5
	add r0, sp, #0x78
	add r1, sp, #0x6c
	mov r5, r2
	mov r4, r3
	str ip, [sp, #0x98]
	str sl, [r8]
	str r7, [r8, #4]
	str r7, [r8, #8]
	str r7, [r8, #0xc]
	add r7, sp, #0x98
	bl ov00_021ECB94
	add r0, sp, #0x78
	add r1, sp, #0x6c
	bl RTC_ConvertDateTimeToSecond
	str sb, [r7, #0x2c]
	stmib r8, {r0, r1}
	mov r0, #1
	str r0, [r8, #0x10]
	str r0, [r7, #0x24]
	mov r0, #0
	str r0, [r7, #0x18]
	str r0, [r7, #0x1c]
	str r0, [r7, #0x20]
	str r0, [r7, #0x28]
	str r0, [r7, #0x30]
	str r0, [r7, #0x34]
	str r0, [r7, #0x38]
	mov r1, r8
	add r0, r6, #0x14
	mov r2, #0x10
	bl memcmp
	cmp r0, #0
	moveq r0, #1
	movne r0, #0
	cmp r0, #0
	bne _0223E1D8
	ldr r0, [r7, #4]
	add r1, r7, #8
	str r0, [r6, #0x14]
	add r0, r6, #0x18
	ldmia r1, {r2, r3}
	stmia r0, {r2, r3}
	ldr r1, [r7, #0x10]
	mov r0, #1
	str r1, [r6, #0x20]
	str r0, [r6, #0x24]
_0223E1D8:
	ldr sb, [r6, #0x2c]
	ldr r0, [r7, #0x1c]
	mov r8, #0
	cmp sb, r0
	bne _0223E224
	cmp sb, #0
	beq _0223E220
	add r0, r6, #0x28
	bl ov45_02231C78
	mov sl, r0
	add r0, r7, #0x18
	bl ov45_02231C78
	mov r1, r0
	mov r0, sl
	mov r2, sb
	bl memcmp
	cmp r0, #0
	bne _0223E224
_0223E220:
	mov r8, #1
_0223E224:
	cmp r8, #0
	bne _0223E268
	add r0, r6, #0x28
	add r1, r7, #0x18
	cmp r0, r1
	beq _0223E260
	mov r3, #0
	ldr r1, [r7, #0x18]
	ldr r8, [r7, #0x1c]
	strb r3, [sp, #0xd]
	sub r2, sp, #4
	strb r3, [r2]
	ldr r3, [r2]
	add r2, r1, r8
	bl ov45_02232760
_0223E260:
	mov r0, #1
	str r0, [r6, #0x34]
_0223E268:
	ldr r1, [r7, #0x28]
	add r0, sp, #0xb0
	str r1, [r6, #0x38]
	ldr r1, [r7, #0x2c]
	str r1, [r6, #0x3c]
	ldr r1, [r7, #0x30]
	str r1, [r6, #0x40]
	ldr r1, [r7, #0x34]
	str r1, [r6, #0x44]
	ldr r1, [r7, #0x38]
	str r1, [r6, #0x48]
	bl ov45_022379D4
	mov r2, #0
	mov r3, #5
	add r0, sp, #0x5c
	add r1, sp, #0x50
	str r3, [sp, #0x88]
	str r2, [sp, #0x8c]
	str r2, [sp, #0x90]
	str r2, [sp, #0x94]
	bl ov00_021ECB94
	add r0, sp, #0x5c
	add r1, sp, #0x50
	bl RTC_ConvertDateTimeToSecond
	str r1, [sp, #0x90]
	ldr r1, [sp, #0x88]
	str r0, [sp, #0x8c]
	add r0, sp, #0x8c
	str r1, [sp, #0x3c]
	ldmia r0, {r2, r3}
	add r1, sp, #0x40
	stmia r1, {r2, r3}
	ldr r0, [sp, #0x94]
	add r7, sp, #0x3c
	str r0, [sp, #0x48]
	mov r0, #1
	str r0, [sp, #0x4c]
	add r0, r6, #0x14
	mov r1, r7
	mov r2, #0x10
	bl memcmp
	cmp r0, #0
	moveq r0, #1
	movne r0, #0
	cmp r0, #0
	bne _0223E348
	ldr r0, [r7]
	add r1, r7, #4
	str r0, [r6, #0x14]
	add r0, r6, #0x18
	ldmia r1, {r2, r3}
	stmia r0, {r2, r3}
	ldr r1, [r7, #0xc]
	mov r0, #1
	str r1, [r6, #0x20]
	str r0, [r6, #0x24]
_0223E348:
	mov r1, #1
	str r1, [r6, #0x40]
	mov r0, #0
	add r2, r5, r4
	str r0, [sp, #0x20]
	str r0, [sp, #0x24]
	str r0, [sp, #0x28]
	strb r0, [sp, #9]
	strb r0, [sp, #0xa]
	strb r0, [sp, #8]
	strb r0, [sp, #4]
	sub r1, sp, #4
	ldr r0, [sp, #0x1c]
	str r2, [r1]
	str r0, [r1, #4]
	ldr r3, [r1]
	add r0, sp, #0x20
	add r1, sp, #0x10
	str r5, [sp, #0x10]
	str r2, [sp, #0x18]
	ldmia r1, {r1, r2}
	bl ov45_022327B8
	mov r2, #0
	ldrb r0, [sp, #0xc]
	ldr r1, [sp, #0x20]
	ldr r4, [sp, #0x24]
	str r2, [sp, #0x2c]
	str r2, [sp, #0x30]
	str r2, [sp, #0x34]
	sub r2, sp, #4
	strb r0, [r2]
	add r5, sp, #0x2c
	ldr r3, [r2]
	mov r0, r5
	add r2, r1, r4
	bl ov45_022383E0
	mov r0, #1
	str r0, [r5, #0xc]
	ldr r8, [r6, #0x2c]
	ldr r0, [sp, #0x30]
	mov r7, #0
	cmp r8, r0
	bne _0223E42C
	cmp r8, #0
	beq _0223E428
	add r0, r6, #0x28
	bl ov45_02231C78
	mov r4, r0
	mov r0, r5
	bl ov45_02231C78
	mov r1, r0
	mov r0, r4
	mov r2, r8
	bl memcmp
	cmp r0, #0
	bne _0223E42C
_0223E428:
	mov r7, #1
_0223E42C:
	cmp r7, #0
	bne _0223E468
	add r0, r6, #0x28
	cmp r0, r5
	beq _0223E460
	mov r3, #0
	ldmia r5, {r1, r4}
	strb r3, [sp, #0xb]
	sub r2, sp, #4
	strb r3, [r2]
	ldr r3, [r2]
	add r2, r1, r4
	bl ov45_02232760
_0223E460:
	mov r0, #1
	str r0, [r6, #0x34]
_0223E468:
	add r0, sp, #0x2c
	bl ov45_022379D4
	mov r1, #1
	add r0, sp, #0x20
	str r1, [r6, #0x44]
	bl ov45_022379D4
	mov r0, #1
	str r0, [r6, #0x48]
	add sp, sp, #0xd4
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, pc}
	.balign 4, 0
_0223E490: .word ov45_02254FB0
	arm_func_end ov45_0223E100
