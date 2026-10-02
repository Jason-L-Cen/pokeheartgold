	.extern ArrayLength
	.extern ov00_021EC3A8
	.extern ov00_021EC3D8
	.extern ov00_021FE5B4
	.extern ov00_021FE678
	.extern ov00_021FE6EC
	.extern ov00_021FE754
	.extern ov00_021FE8D8
	.extern ov00_021FE968
	.extern ov00_021FEA20
	.extern ov00_021FEC3C
	.extern ov00_021FED4C
	.extern ov00_021FEE8C
	.extern ov00_021FEF00
	.extern ov00_021FEF74
	.extern ov00_021FF01C
	.extern ov00_021FF0C0
	.extern ov00_021FF264
	.extern ov00_021FF368
	.extern ov00_021FF3C0
	.extern ov00_021FF3CC
	.extern ov00_021FF3D0
	.extern ov00_021FF3D4
	.extern ov00_021FF694
	.extern ov00_021FF6BC
	.extern ov00_021FF6E4
	.extern ov00_021FF770
	.extern ov00_021FF804
	.extern ov00_021FF818
	.extern ov00_021FF840
	.extern ov00_021FF8A0
	.extern ov00_021FF92C
	.extern ov00_021FF940
	.extern ov00_021FF970
	.extern ov00_021FF984
	.extern ov00_021FFA18
	.extern ov00_021FFA44
	.extern ov00_021FFA60
	.extern ov00_0220BF44
	.extern ov00_0220BF70
	.extern ov00_0220BF80
	.extern ov00_0220BFD0
	.extern ov00_0220C03C
	.extern ov00_0220C0C4
	.extern ov00_0220C164
	.extern ov00_0220C860
	.extern ov00_0220CC38
	.extern ov00_0220CE00
	.extern ov00_0220CF14
	.extern ov00_0220D238
	.extern ov00_0220D338
	.extern ov00_0220D404
	.extern ov00_0220E490
	.extern ov00_022163E8
	.extern ov00_022163F0
	.extern ov00_02219C94
	.extern ov00_02219CB4
	.extern ov00_02219CB8
	.extern ov00_02219CC0
	.extern ov00_02219CD0
	.extern ov00_02219CE4
	.extern ov00_02219CEC
	.extern ov00_02219CF0
	.extern ov00_02219CF4
	.extern ov00_02219D14
	.extern ov00_02219D24
	.extern ov00_02219D28
	.extern ov00_02219D30
	.extern ov00_02219D38
	.extern ov00_02219D54
	.extern ov00_02219D70
	.extern ov00_02219D74
	.extern ov00_02219E84
	.extern ov00_02219EA4
	.extern ov00_02219EDC
	.extern ov00_02219EE0
	.extern ov00_02219EF0
	.extern ov00_02219EFC
	.extern ov00_02219F04
	.extern ov00_02219F08
	.extern ov00_02219F10
	.extern ov00_02219F24
	.extern ov00_02219F30
	.extern ov00_02219F3C
	.extern ov00_02219F44
	.extern ov00_02219F48
	.extern ov00_02219F4C
	.extern ov00_02219F5C
	.extern ov00_02219F68
	.extern ov00_02219F74
	.extern ov00_0221A0B8
	.extern ov00_0221A4B0
	.extern ov00_0221A4C0
	.extern ov00_0221A4CC
	.extern ov00_0221A4E4
	.extern ov00_0221A4FC
	.extern ov00_0221A500
	.extern ov00_0221A508
	.extern ov00_0221A514
	.extern ov00_0221A51C
	.extern ov00_0221A520
	.extern ov00_0221A528
	.extern ov00_0221A52C
	.extern ov00_0221A530
	.extern ov00_0221A550
	.extern ov00_0221A560
	.extern ov00_0221A574
	.extern ov00_0221A588
	.extern ov00_0221A59C
	.extern ov00_0221A5B8
	.extern ov00_0221A5BC
	.extern ov00_0221A5C8
	.extern ov00_0221A5D8
	.extern ov00_0221A5E8
	.extern ov00_0221A5FC
	.extern ov00_0221A618
	.extern ov00_0221A638
	.extern ov00_0221A63C
	.extern ov00_0221A644
	.extern ov00_0221A650
	.extern ov00_0221B0D0
	.extern ov00_0221B16C
	.extern ov00_0221B634
	.extern ov00_0221B638
	.extern ov00_0221B664
	.extern ov00_0221B678
	.extern ov00_0221B878
	.extern ov00_0221B87C
	.extern ov00_0221B890
	.extern ov00_0221B8D0
	.extern ov00_0221B9D0
	.extern ov00_0221B9D4
	.extern ov00_0221B9DC
	.extern ov00_022122B8
	.extern ov00_022132C4
	.extern ov00_02213370
	.extern ov00_0221339C
	.extern ov00_02213C6C
	.extern ov00_02213CFC
	.extern ov00_02213DA0
	.extern ov00_02213DFC
	.extern ov00_02213E10
	.extern ov00_02213EE8
	.extern ov00_02213FD4
	.extern ov00_02214130
	.extern ov00_02214558
	.extern ov00_022145F0
	.extern ov00_022148A4
	.extern ov00_022149A4
	.extern ov00_02214BF0
	.extern ov00_02214C60
	.extern ov00_02214F2C
	.extern ov00_0221505C
	.include "asm/macros.inc"
	.include "overlay_00_arm.inc"
	.include "global.inc"
	.public ov00_02215064
	.public ov00_02215490

.text
	arm_func_start ov00_02215064
ov00_02215064: ; 0x02215064
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, lr}
	sub sp, sp, #8
	mov r7, r0
	ldr r1, [r7, #0x5c8]
	ldr r5, [r7, #0x7c]
	ldr r6, [r7, #0x80]
	cmp r1, #4
	addls pc, pc, r1, lsl #2
	b _02215424
_02215088: ; jump table
	b _0221509C ; case 0
	b _022150FC ; case 1
	b _022151FC ; case 2
	b _022152E8 ; case 3
	b _02215394 ; case 4
_0221509C:
	cmp r6, #1
	blt _02215424
	ldrb r1, [r5]
	eor r1, r1, #0xec
	add r3, r1, #2
	cmp r6, r3
	blt _02215424
	sub r1, r3, #1
	ldrb r1, [r5, r1]
	eor r2, r1, #0xea
	add r4, r3, r2
	cmp r6, r4
	blt _02215424
	add r1, r5, r3
	bl ov00_022149A4
	add r5, r5, r4
	sub r6, r6, r4
	add r0, r7, #0xbc
	mov r3, #1
	mov r1, r5
	mov r2, r6
	add r0, r0, #0x400
	str r3, [r7, #0x5c8]
	bl ov00_022122B8
_022150FC:
	cmp r6, #6
	blt _02215424
	ldrb r3, [r5]
	ldrb r1, [r5, #1]
	ldr r2, _02215478 ; =ov00_0221B9D4
	mov r0, r7
	strb r3, [r7, #0x4a0]
	strb r1, [r7, #0x4a1]
	ldrb r4, [r5, #2]
	ldrb r3, [r5, #3]
	mov r1, #6
	strb r4, [r7, #0x4a2]
	strb r3, [r7, #0x4a3]
	ldr r2, [r2]
	ldr r3, [r7, #0x494]
	ldr r4, [r7, #0x488]
	blx r4
	ldrb r3, [r5, #4]
	ldrb r2, [r5, #5]
	add r0, r7, #0x400
	ldr r1, _0221547C ; =0x0000FFFF
	strb r3, [r7, #0x4a8]
	strb r2, [r7, #0x4a9]
	ldrh r0, [r0, #0xa8]
	cmp r0, r1
	bne _022151B0
	add r0, r5, #6
	sub r1, r6, #6
	bl ov00_02213FD4
	mvn r1, #0
	cmp r0, r1
	beq _02215424
	mov r0, r7
	add r1, r5, #6
	bl ov00_0221505C
	ldr r0, _02215478 ; =ov00_0221B9D4
	ldr r3, [r7, #0x494]
	ldr r2, [r0]
	ldr r4, [r7, #0x488]
	mov r0, r7
	mov r1, #5
	blx r4
	ldr r0, [r7, #0x7c]
	cmp r0, #0
	beq _02215424
_022151B0:
	ldr r0, [r7, #0x5c4]
	add r5, r5, #6
	tst r0, #2
	sub r6, r6, #6
	bne _022151D8
	add r0, r7, #0x400
	ldrh r1, [r0, #0xa8]
	ldr r0, _0221547C ; =0x0000FFFF
	cmp r1, r0
	bne _022151EC
_022151D8:
	mov r0, #5
	str r0, [r7, #0x5c8]
	mov r0, #2
	str r0, [r7]
	b _02215424
_022151EC:
	mov r0, #2
	str r0, [r7, #0x5c8]
	sub r0, r0, #3
	str r0, [r7, #0x484]
_022151FC:
	ldr r1, [r7, #0x484]
	mvn r0, #0
	cmp r1, r0
	bne _02215244
	cmp r6, #1
	blt _02215424
	ldrb r1, [r5]
	mov r0, #8
	mov r2, #0
	str r1, [r7, #0x484]
	bl ov00_021FE5B4
	cmp r0, #0
	str r0, [r7, #8]
	addeq sp, sp, #8
	moveq r0, #5
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	add r5, r5, #1
	sub r6, r6, #1
_02215244:
	ldr r0, [r7, #8]
	bl ArrayLength
	ldr r1, [r7, #0x484]
	cmp r1, r0
	ble _022152C4
	add r8, sp, #0
	mvn r4, #0
_02215260:
	cmp r6, #2
	blt _022152C4
	add r0, r5, #1
	sub r1, r6, #1
	bl ov00_02213FD4
	mov sb, r0
	cmp sb, r4
	beq _022152C4
	ldrb r2, [r5]
	mov r0, r7
	add r1, r5, #1
	str r2, [sp, #4]
	bl ov00_02213EE8
	str r0, [sp]
	ldr r0, [r7, #8]
	mov r1, r8
	bl ov00_021FE754
	add r1, sb, #1
	ldr r0, [r7, #8]
	add r5, r5, r1
	sub r6, r6, r1
	bl ArrayLength
	ldr r1, [r7, #0x484]
	cmp r1, r0
	bgt _02215260
_022152C4:
	ldr r0, [r7, #8]
	bl ArrayLength
	ldr r1, [r7, #0x484]
	cmp r1, r0
	bgt _02215424
	mov r0, #3
	str r0, [r7, #0x5c8]
	sub r0, r0, #4
	str r0, [r7, #0x484]
_022152E8:
	ldr r1, [r7, #0x484]
	mvn r0, #0
	cmp r1, r0
	bne _02215314
	cmp r6, #1
	blt _02215424
	ldrb r1, [r5], #1
	mov r0, #0
	sub r6, r6, #1
	str r1, [r7, #0x484]
	str r0, [r7, #0x480]
_02215314:
	ldr r1, [r7, #0x484]
	ldr r0, [r7, #0x480]
	cmp r1, r0
	ble _0221537C
	mvn r8, #0
_02215328:
	mov r0, r5
	mov r1, r6
	bl ov00_02213FD4
	mov r4, r0
	cmp r4, r8
	beq _0221537C
	mov r0, r7
	mov r1, r5
	bl ov00_02213EE8
	ldr r1, [r7, #0x480]
	add r5, r5, r4
	mov r2, r1
	add r1, r1, #1
	str r1, [r7, #0x480]
	add r1, r7, r2, lsl #2
	str r0, [r1, #0x84]
	ldr r1, [r7, #0x484]
	ldr r0, [r7, #0x480]
	sub r6, r6, r4
	cmp r1, r0
	bgt _02215328
_0221537C:
	ldr r1, [r7, #0x484]
	ldr r0, [r7, #0x480]
	cmp r1, r0
	bgt _02215424
	mov r0, #4
	str r0, [r7, #0x5c8]
_02215394:
	cmp r6, #5
	blt _02215424
	mvn r4, #0
	sub r8, r4, #1
	mov sb, #0
_022153A8:
	mov r0, r7
	mov r1, r5
	mov r2, r6
	bl ov00_02214F2C
	cmp r0, r8
	addeq sp, sp, #8
	moveq r0, #5
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	cmp r0, r4
	bne _02215408
	mov r0, #5
	str r0, [r7, #0x5c8]
	mov r0, #2
	str r0, [r7]
	ldr r0, _02215478 ; =ov00_0221B9D4
	ldr r3, [r7, #0x494]
	ldr r2, [r0]
	ldr r4, [r7, #0x488]
	mov r0, r7
	mov r1, #3
	sub r6, r6, #5
	add r5, r5, #5
	blx r4
	b _02215424
_02215408:
	ldr r1, [r7, #0x7c]
	add r5, r5, r0
	cmp r1, #0
	sub r6, r6, r0
	moveq r0, sb
	cmp r0, #0
	bne _022153A8
_02215424:
	cmp r6, #0
	bge _02215440
	ldr r0, _02215480 ; =ov00_0221A644
	ldr r1, _02215484 ; =ov00_0221A5C8
	ldr r2, _02215488 ; =ov00_0221A588
	ldr r3, _0221548C ; =0x000004AF
	bl __msl_assertion_failed
_02215440:
	ldr r0, [r7, #0x7c]
	cmp r0, #0
	addeq sp, sp, #8
	moveq r0, #0
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	cmp r6, #0
	beq _02215468
	mov r1, r5
	mov r2, r6
	bl memmove
_02215468:
	str r6, [r7, #0x80]
	mov r0, #0
	add sp, sp, #8
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	.balign 4, 0
_02215478: .word ov00_0221B9D4
_0221547C: .word 0x0000FFFF
_02215480: .word ov00_0221A644
_02215484: .word ov00_0221A5C8
_02215488: .word ov00_0221A588
_0221548C: .word 0x000004AF
	arm_func_end ov00_02215064

	arm_func_start ov00_02215490
ov00_02215490: ; 0x02215490
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #8
	mov sl, r0
	ldr r3, [sl, #8]
	mov sb, r1
	mov r8, r2
	cmp r3, #0
	sub r8, r8, #1
	ldrb r7, [sb], #1
	beq _022154BC
	bl ov00_022148A4
_022154BC:
	mov r1, r7
	mov r0, #8
	mov r2, #0
	bl ov00_021FE5B4
	cmp r0, #0
	str r0, [sl, #8]
	addeq sp, sp, #8
	moveq r0, #5
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	cmp r7, #0
	mov r6, #0
	ble _02215560
	add fp, sp, #0
	mvn r4, #0
_022154F4:
	cmp r8, #2
	addlt sp, sp, #8
	movlt r0, #4
	ldmltia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	add r0, sb, #1
	sub r1, r8, #1
	bl ov00_02213FD4
	mov r5, r0
	cmp r5, r4
	addeq sp, sp, #8
	moveq r0, #4
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldrb r2, [sb]
	mov r0, sl
	add r1, sb, #1
	str r2, [sp, #4]
	bl ov00_02213EE8
	str r0, [sp]
	ldr r0, [sl, #8]
	mov r1, fp
	bl ov00_021FE754
	add r0, r5, #1
	add r6, r6, #1
	cmp r6, r7
	add sb, sb, r0
	sub r8, r8, r0
	blt _022154F4
_02215560:
	mov r0, #0
	add sp, sp, #8
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	arm_func_end ov00_02215490
