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
	.public ov45_02240850
	.extern ov45_0223F8F4
	.extern ov45_0223FD8C

.text
	arm_func_start ov45_02240718
ov45_02240718: ; 0x02240718
	stmdb sp!, {r4, r5, r6, r7, r8, sb, lr}
	sub sp, sp, #0x2c
	ldr r4, _02240848 ; =ov45_022577DC
	mov r8, r1
	ldr r1, [r4]
	mov r7, r2
	mov r6, r3
	cmp r1, #0
	beq _0224074C
	ldr r1, [r1, #4]
	cmp r1, #5
	addeq sp, sp, #0x2c
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, pc}
_0224074C:
	cmp r8, #0
	addeq sp, sp, #0x2c
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, pc}
	ldr r3, [sp, #0x48]
	mov r1, r7
	mov r2, r6
	bl ov45_0223F8F4
	ldr r0, _02240848 ; =ov45_022577DC
	ldr r5, [sp, #0x48]
	ldr r4, [r0]
	ldr lr, _0224084C ; =ov45_02255238
	add r3, sp, #8
	mov ip, #0
	add r0, sp, #4
	add r2, sp, #0
	add r1, r4, #0x90
	str lr, [sp, #8]
	str r5, [sp, #0xc]
	str ip, [sp, #0x10]
	str r3, [sp]
	bl ov45_0223FD8C
	ldr r1, [sp, #4]
	add r0, r4, #0x94
	cmp r1, r0
	moveq r4, #0
	ldrne r4, [r1, #0xc]
	add r0, sp, #0x20
	mov r1, r7
	bl ov45_02237B58
	ldr r1, _02240848 ; =ov45_022577DC
	add r0, sp, #0x14
	ldr r1, [r1]
	add r2, sp, #0x20
	bl ov45_022428D8
	ldr r0, [sp, #0x14]
	ldr r7, [r4, #0xc]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	mov r0, r4
	ldr r1, [r0]
	addeq sb, sp, #0x15
	ldrne sb, [sp, #0x1c]
	cmp r8, #1
	moveq r8, #1
	ldr r1, [r1, #0xc]
	movne r8, #0
	blx r1
	mov r3, r0
	mov r0, r8
	mov r1, sb
	mov r2, r6
	blx r7
	add r0, sp, #0x14
	bl ov45_02237BC8
	add r0, sp, #0x20
	bl ov45_02237BC8
	ldr r0, _02240848 ; =ov45_022577DC
	mov r1, r5
	ldr r0, [r0]
	add r0, r0, #0x90
	bl ov45_02242510
	add sp, sp, #0x2c
	ldmia sp!, {r4, r5, r6, r7, r8, sb, pc}
	.balign 4, 0
_02240848: .word ov45_022577DC
_0224084C: .word ov45_02255238
	arm_func_end ov45_02240718

	arm_func_start ov45_02240850
ov45_02240850: ; 0x02240850
	ldr r0, _02240870 ; =ov45_022577DC
	mov r1, #3
	ldr r2, [r0]
	mov r0, #5
	str r1, [r2, #0x58]
	str r0, [r2, #4]
	mov r0, #0
	bx lr
	.balign 4, 0
_02240870: .word ov45_022577DC
	arm_func_end ov45_02240850

	arm_func_start ov45_02240874
ov45_02240874: ; 0x02240874
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x28
	ldr r0, _02240A54 ; =ov45_022577DC
	ldr r1, [r0]
	cmp r1, #0
	addeq sp, sp, #0x28
	moveq r0, #0
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	beq _022408AC
	ldr r0, [r1, #4]
	cmp r0, #5
	addeq sp, sp, #0x28
	moveq r0, #5
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_022408AC:
	ldr r0, [r1]
	cmp r0, #0
	beq _022408BC
	bl ov45_02250590
_022408BC:
	ldr r2, _02240A54 ; =ov45_022577DC
	add r0, sp, #0xc
	add r1, sp, #0x1c
	ldr r5, [r2]
	bl ov00_021ECB94
	add r0, sp, #0xc
	add r1, sp, #0x1c
	bl RTC_ConvertDateTimeToSecond
	ldr r2, [r5, #0xa8]
	add r7, r5, #0xa0
	mov r6, r0
	mov sb, r1
	str r2, [sp, #8]
	cmp r2, r7
	beq _0224098C
	mov fp, #1
	mov sl, #0
	add r4, sp, #8
_02240904:
	ldr r1, [r2, #0x10]
	ldr r0, [r2, #0x14]
	subs r1, r6, r1
	ldr r8, [r2, #0x18]
	sbc r0, sb, r0
	ldr r3, [r2, #0x1c]
	subs r1, r8, r1
	sbcs r1, r3, r0
	bge _02240948
	str r6, [r2, #0x10]
	str sb, [r2, #0x14]
	ldr r0, [r2, #0x20]
	ldr r1, [r2, #0x24]
	blx r1
	cmp r0, #0
	moveq r0, sl
	beq _0224094C
_02240948:
	mov r0, fp
_0224094C:
	cmp r0, #0
	bne _02240978
	ldr r8, [sp, #8]
	mov r0, r4
	bl ov45_02236E58
	add r0, sp, #4
	mov r2, r8
	add r1, r5, #0x9c
	str r8, [sp]
	bl ov45_02236C40
	b _02240980
_02240978:
	mov r0, r4
	bl ov45_02236E58
_02240980:
	ldr r2, [sp, #8]
	cmp r2, r7
	bne _02240904
_0224098C:
	ldr r0, _02240A54 ; =ov45_022577DC
	ldr r3, [r0]
	cmp r3, #0
	beq _022409B0
	ldr r0, [r3, #4]
	cmp r0, #5
	addeq sp, sp, #0x28
	moveq r0, #5
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_022409B0:
	ldr r1, [r3, #0x54]
	ldr r0, _02240A54 ; =ov45_022577DC
	cmp r1, #3
	ldr r2, [r0]
	blo _022409F0
	mov r1, #0
	str r1, [r3, #0x54]
	ldr r2, [r0]
	mov r1, #3
	str r1, [r2, #0x58]
	mov r1, #5
	str r1, [r2, #4]
	ldr r0, [r0]
	add sp, sp, #0x28
	ldr r0, [r0, #4]
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_022409F0:
	ldr r0, [r2, #4]
	cmp r0, #4
	addls pc, pc, r0, lsl #2
	b _02240A40
_02240A00: ; jump table
	b _02240A40 ; case 0
	b _02240A40 ; case 1
	b _02240A14 ; case 2
	b _02240A40 ; case 3
	b _02240A40 ; case 4
_02240A14:
	ldr r1, [r2, #0x5c]
	add r0, r1, #1
	str r0, [r2, #0x5c]
	cmp r1, #0x3c
	bne _02240A40
	mov r0, #0
	str r0, [r2, #0x5c]
	ldr r0, [r2, #4]
	cmp r0, #5
	movne r0, #3
	strne r0, [r2, #4]
_02240A40:
	ldr r0, _02240A54 ; =ov45_022577DC
	ldr r0, [r0]
	ldr r0, [r0, #4]
	add sp, sp, #0x28
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.balign 4, 0
_02240A54: .word ov45_022577DC
	arm_func_end ov45_02240874
