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
	.public ov45_0223F000

.text
	arm_func_start ov45_0223F000
ov45_0223F000: ; 0x0223F000
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x30
	mov sl, r0
	mov r4, #0
	str r4, [sl]
	str r4, [sl, #4]
	add r0, sp, #0x24
	str r2, [sp]
	str r3, [sp, #4]
	str r4, [sl, #8]
	mov r8, #1
	bl ov45_02237A24
	sub r5, sp, #4
	add r6, sp, #0x24
	add r7, sp, #0x18
	add fp, sp, #0xc
	mvn r4, #0
	b _0223F104
_0223F048:
	cmp sb, #0
	beq _0223F0DC
	mov r0, r7
	mov r1, r6
	mov r2, #0
	mov r3, sb
	bl ov45_02236680
	mov r1, #0
	add r0, sp, #0xb
	strb r1, [r0]
	ldr r1, [sl, #4]
	ldr r0, [sl, #8]
	cmp r1, r0
	bhs _0223F0AC
	mov r0, #0xc
	mul r0, r1, r0
	ldr r1, [sl]
	adds r0, r1, r0
	beq _0223F09C
	mov r1, r7
	bl ov45_02237A24
_0223F09C:
	ldr r0, [sl, #4]
	add r0, r0, #1
	str r0, [sl, #4]
	b _0223F0D0
_0223F0AC:
	mov r0, #0
	mov r2, #0
	strb r0, [sp, #0xa]
	mov r2, r2
	strb r2, [r5]
	ldr r2, [r5]
	mov r0, sl
	mov r1, r7
	bl ov45_02237634
_0223F0D0:
	mov r0, r7
	bl ov45_02237BC8
	add r8, r8, #1
_0223F0DC:
	add r2, sb, #1
	mov r0, fp
	mov r1, r6
	mov r3, r4
	bl ov45_02236680
	mov r0, r6
	mov r1, fp
	bl ov45_02237D48
	mov r0, fp
	bl ov45_02237BC8
_0223F104:
	ldr r0, [sp, #4]
	cmp r8, r0
	beq _0223F12C
	ldr r1, [sp]
	mov r0, r6
	mov r2, #0
	bl ov45_02242254
	mov sb, r0
	cmp sb, r4
	bne _0223F048
_0223F12C:
	ldr r0, [sp, #0x24]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	ldrne r0, [sp, #0x28]
	bne _0223F14C
	ldrb r0, [sp, #0x24]
	mov r0, r0, lsl #0x18
	mov r0, r0, lsr #0x19
_0223F14C:
	cmp r0, #0
	beq _0223F1B8
	mov r2, #0
	strb r2, [sp, #9]
	ldr r1, [sl, #4]
	ldr r0, [sl, #8]
	cmp r1, r0
	bhs _0223F198
	mov r0, #0xc
	mul r0, r1, r0
	ldr r1, [sl]
	adds r0, r1, r0
	beq _0223F188
	add r1, sp, #0x24
	bl ov45_02237A24
_0223F188:
	ldr r0, [sl, #4]
	add r0, r0, #1
	str r0, [sl, #4]
	b _0223F1B8
_0223F198:
	strb r2, [sp, #8]
	sub r1, sp, #4
	and r0, r2, #0xff
	strb r0, [r1]
	ldr r2, [r1]
	add r1, sp, #0x24
	mov r0, sl
	bl ov45_02237634
_0223F1B8:
	add r0, sp, #0x24
	bl ov45_02237BC8
	add sp, sp, #0x30
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	arm_func_end ov45_0223F000
