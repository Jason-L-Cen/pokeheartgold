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

.text
	arm_func_start ov45_0223F32C
ov45_0223F32C: ; 0x0223F32C
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	sub sp, sp, #0xa8
	ldr r0, _0223F5C4 ; =ov45_022577DC
	mov r5, r1
	ldr r0, [r0]
	mov r4, r2
	cmp r0, #0
	beq _0223F35C
	ldr r0, [r0, #4]
	cmp r0, #5
	addeq sp, sp, #0xa8
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
_0223F35C:
	add r0, sp, #0x90
	mov r1, r5
	bl ov45_02237B58
	ldr r0, _0223F5C4 ; =ov45_022577DC
	add r1, sp, #0x90
	ldr r0, [r0]
	bl ov45_02243808
	mov r6, r0
	add r0, sp, #0x90
	bl ov45_02237BC8
	cmp r6, #0
	beq _0223F3AC
	ldr r0, _0223F5C4 ; =ov45_022577DC
	mov r1, r5
	ldr r0, [r0]
	ldr r0, [r0]
	bl ov45_02250F5C
	cmp r0, r6
	addgt sp, sp, #0xa8
	ldmgtia sp!, {r3, r4, r5, r6, r7, pc}
_0223F3AC:
	add r0, sp, #0x84
	mov r1, r4
	bl ov45_02237B58
	add r0, sp, #0x84
	bl ov45_02242A2C
	ldr r1, _0223F5C4 ; =ov45_022577DC
	mov r2, r0
	ldr r1, [r1]
	add r0, sp, #0x9c
	bl ov45_0224367C
	add r0, sp, #0x84
	bl ov45_02237BC8
	add r0, sp, #0x78
	mov r1, r5
	bl ov45_02237B58
	ldr r0, _0223F5C4 ; =ov45_022577DC
	add r1, sp, #0x9c
	ldr r0, [r0]
	add r2, sp, #0x78
	bl ov45_02243280
	mov r6, r0
	add r0, sp, #0x78
	bl ov45_02237BC8
	cmp r6, #0
	beq _0223F4AC
	add r0, sp, #0x6c
	mov r1, r5
	bl ov45_02237B58
	ldr r0, _0223F5C4 ; =ov45_022577DC
	add r1, sp, #0x6c
	ldr r0, [r0]
	bl ov45_02242C3C
	mov r6, r0
	add r0, sp, #0x6c
	bl ov45_02237BC8
	add r0, sp, #0x60
	mov r1, r5
	bl ov45_02237B58
	ldr r1, _0223F5C4 ; =ov45_022577DC
	add r0, sp, #0x54
	ldr r1, [r1]
	add r2, sp, #0x60
	bl ov45_022428D8
	add r0, sp, #0x48
	mov r1, r4
	bl ov45_02237B58
	ldr r1, [sp, #0x54]
	add r0, sp, #0x48
	mov r1, r1, lsl #0x1f
	movs r1, r1, lsr #0x1f
	addeq r7, sp, #0x55
	ldrne r7, [sp, #0x5c]
	bl ov45_02242A2C
	mov r1, r0
	ldr r2, [r6, #0x14]
	ldr r3, [r6, #4]
	mov r0, r7
	blx r3
	add r0, sp, #0x48
	bl ov45_02237BC8
	add r0, sp, #0x54
	bl ov45_02237BC8
	add r0, sp, #0x60
	bl ov45_02237BC8
_0223F4AC:
	add r0, sp, #0x3c
	mov r1, r4
	bl ov45_02237B58
	add r0, sp, #0x30
	mov r1, r5
	bl ov45_02237B58
	ldr r0, _0223F5C4 ; =ov45_022577DC
	add r1, sp, #0x3c
	ldr r0, [r0]
	add r2, sp, #0x30
	bl ov45_02242D30
	mov r4, r0
	add r0, sp, #0x30
	bl ov45_02237BC8
	add r0, sp, #0x3c
	bl ov45_02237BC8
	mvn r0, #0
	cmp r4, r0
	bne _0223F508
	add r0, sp, #0x9c
	bl ov45_02237BC8
	add sp, sp, #0xa8
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_0223F508:
	add r0, sp, #0x24
	mov r1, r5
	bl ov45_02237B58
	ldr r0, _0223F5C4 ; =ov45_022577DC
	add r1, sp, #0x24
	ldr r0, [r0]
	bl ov45_02243794
	mov r6, r0
	add r0, sp, #0x24
	bl ov45_02237BC8
	cmp r6, #0
	beq _0223F5B4
	add r0, sp, #0x18
	mov r1, r5
	bl ov45_02237B58
	ldr r0, _0223F5C4 ; =ov45_022577DC
	add r1, sp, #0x18
	ldr r0, [r0]
	bl ov45_02242C3C
	mov r6, r0
	add r0, sp, #0x18
	bl ov45_02237BC8
	add r0, sp, #0xc
	mov r1, r5
	bl ov45_02237B58
	ldr r1, _0223F5C4 ; =ov45_022577DC
	add r0, sp, #0
	ldr r1, [r1]
	add r2, sp, #0xc
	bl ov45_022428D8
	ldr r0, [sp]
	ldr r2, [r6, #0x14]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r0, sp, #1
	ldrne r0, [sp, #8]
	ldr r3, [r6]
	mov r1, r4
	blx r3
	add r0, sp, #0
	bl ov45_02237BC8
	add r0, sp, #0xc
	bl ov45_02237BC8
_0223F5B4:
	add r0, sp, #0x9c
	bl ov45_02237BC8
	add sp, sp, #0xa8
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.balign 4, 0
_0223F5C4: .word ov45_022577DC
	arm_func_end ov45_0223F32C

	arm_func_start ov45_0223F5C8
ov45_0223F5C8: ; 0x0223F5C8
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	sub sp, sp, #0x48
	ldr r0, _0223F6F0 ; =ov45_022577DC
	mov r7, r1
	ldr r0, [r0]
	mov r6, r2
	cmp r0, #0
	beq _0223F5F8
	ldr r0, [r0, #4]
	cmp r0, #5
	addeq sp, sp, #0x48
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
_0223F5F8:
	add r0, sp, #0x3c
	mov r1, r6
	bl ov45_02237B58
	ldr r0, _0223F6F0 ; =ov45_022577DC
	add r1, sp, #0x3c
	ldr r0, [r0]
	bl ov45_022435F8
	mov r4, r0
	add r0, sp, #0x3c
	bl ov45_02237BC8
	mvn r0, #0
	cmp r4, r0
	addeq sp, sp, #0x48
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	add r0, sp, #0x30
	mov r1, r7
	bl ov45_02237B58
	ldr r0, _0223F6F0 ; =ov45_022577DC
	add r1, sp, #0x30
	ldr r0, [r0]
	bl ov45_02242C3C
	mov r5, r0
	add r0, sp, #0x30
	bl ov45_02237BC8
	add r0, sp, #0x24
	mov r1, r7
	bl ov45_02237B58
	ldr r1, _0223F6F0 ; =ov45_022577DC
	add r0, sp, #0x18
	ldr r1, [r1]
	add r2, sp, #0x24
	bl ov45_022428D8
	ldr r0, [sp, #0x18]
	ldr r2, [r5, #0x14]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r0, sp, #0x19
	ldrne r0, [sp, #0x20]
	ldr r3, [r5, #4]
	mov r1, r4
	blx r3
	add r0, sp, #0x18
	bl ov45_02237BC8
	add r0, sp, #0x24
	bl ov45_02237BC8
	add r0, sp, #0xc
	mov r1, r6
	bl ov45_02237B58
	add r0, sp, #0
	mov r1, r7
	bl ov45_02237B58
	ldr r0, _0223F6F0 ; =ov45_022577DC
	add r1, sp, #0xc
	ldr r0, [r0]
	add r2, sp, #0
	bl ov45_02243280
	add r0, sp, #0
	bl ov45_02237BC8
	add r0, sp, #0xc
	bl ov45_02237BC8
	add sp, sp, #0x48
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.balign 4, 0
_0223F6F0: .word ov45_022577DC
	arm_func_end ov45_0223F5C8
