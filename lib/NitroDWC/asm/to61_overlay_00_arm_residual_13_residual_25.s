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
	.include "asm/macros.inc"
	.include "overlay_00_arm.inc"
	.include "global.inc"
	.public ov00_02214BF0
	.public ov00_02214C60
	.public ov00_02214F2C
	.extern ov00_02212CD0
	.extern ov00_02212D10
	.extern ov00_022132C4
	.extern ov00_02213370
	.extern ov00_02213378
	.extern ov00_02213384
	.extern ov00_0221338C
	.extern ov00_02213394
	.extern ov00_0221339C
	.extern ov00_02213C6C
	.extern ov00_02214A44
	.extern ov00_02214A70
	.extern ov00_02214AF4

.text
	arm_func_start ov00_02214BF0
ov00_02214BF0: ; 0x02214BF0
	stmdb sp!, {r4, lr}
	cmp r2, #5
	ldmltia sp!, {r4, pc}
	ldrb r4, [r1]
	ldrb lr, [r1, #1]
	ldrb ip, [r1, #2]
	tst r4, #0x10
	strb lr, [r3]
	strb ip, [r3, #1]
	ldrb lr, [r1, #3]
	ldrb ip, [r1, #4]
	strb lr, [r3, #2]
	strb ip, [r3, #3]
	beq _02214C4C
	sub r0, r2, #5
	cmp r0, #2
	ldmltia sp!, {r4, pc}
	ldrb r2, [r1, #5]
	ldrb r0, [r1, #6]
	ldr r1, [sp, #8]
	strb r2, [r1]
	strb r0, [r1, #1]
	ldmia sp!, {r4, pc}
_02214C4C:
	add r0, r0, #0x400
	ldrh r1, [r0, #0xa8]
	ldr r0, [sp, #8]
	strh r1, [r0]
	ldmia sp!, {r4, pc}
	arm_func_end ov00_02214BF0

	arm_func_start ov00_02214C60
ov00_02214C60: ; 0x02214C60
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x10
	mov r8, r2
	ldrb r2, [r8], #5
	mov sb, r1
	mov sl, r0
	mov r7, r3
	str r2, [sp]
	mov r0, sb
	mov r1, r2
	str r7, [sp, #4]
	ldr r6, [sp, #0x38]
	bl ov00_02213370
	ldr r0, [sp]
	sub r7, r7, #5
	tst r0, #0x10
	ldr r0, [sp]
	addne r8, r8, #2
	subne r7, r7, #2
	tst r0, #2
	moveq r0, #0
	streq r0, [sp, #0xc]
	beq _02214CE8
	ldrb r1, [r8]
	ldrb r0, [r8, #1]
	add r2, sp, #0xc
	sub r7, r7, #4
	strb r1, [r2]
	strb r0, [r2, #1]
	ldrb r1, [r8, #2]
	ldrb r0, [r8, #3]
	add r8, r8, #4
	strb r1, [r2, #2]
	strb r0, [r2, #3]
_02214CE8:
	ldr r0, [sp]
	tst r0, #0x20
	addeq r0, sl, #0x400
	ldreqh r0, [r0, #0xa8]
	streqh r0, [sp, #8]
	beq _02214D1C
	ldrb r1, [r8]
	ldrb r0, [r8, #1]
	add r2, sp, #8
	add r8, r8, #2
	strb r1, [r2]
	strb r0, [r2, #1]
	sub r7, r7, #2
_02214D1C:
	ldrh r2, [sp, #8]
	ldr r1, [sp, #0xc]
	mov r0, sb
	bl ov00_02213378
	ldr r0, [sp]
	tst r0, #8
	beq _02214D70
	ldrb r2, [r8]
	ldrb r1, [r8, #1]
	add r3, sp, #0xc
	mov r0, sb
	strb r2, [r3]
	strb r1, [r3, #1]
	ldrb r2, [r8, #2]
	ldrb r1, [r8, #3]
	add r8, r8, #4
	sub r7, r7, #4
	strb r2, [r3, #2]
	strb r1, [r3, #3]
	ldr r1, [sp, #0xc]
	bl ov00_02213384
_02214D70:
	ldr r0, [sp]
	tst r0, #0x40
	beq _02214EA0
	ldr r0, [sl, #8]
	bl ArrayLength
	mov r4, r0
	cmp r4, #0
	mov r5, #0
	ble _02214E88
	add fp, sp, #0xa
_02214D98:
	ldr r0, [sl, #8]
	mov r1, r5
	bl ov00_021FE6EC
	mov r1, r0
	ldr r0, [r1, #4]
	cmp r0, #0
	beq _02214E28
	cmp r0, #1
	beq _02214DC8
	cmp r0, #2
	beq _02214DE0
	b _02214E7C
_02214DC8:
	ldrb r2, [r8], #1
	ldr r1, [r1]
	mov r0, sb
	bl ov00_02212D10
	sub r7, r7, #1
	b _02214E7C
_02214DE0:
	ldrb r3, [r8]
	ldrb r2, [r8, #1]
	mov r0, sb
	strb r3, [fp]
	strb r2, [fp, #1]
	ldrh r3, [sp, #0xa]
	ldr r1, [r1]
	mov r2, r3, asr #8
	and r2, r2, #0xff
	mov r3, r3, lsl #8
	and r3, r3, #0xff00
	orr r2, r2, r3
	mov r2, r2, lsl #0x10
	mov r2, r2, lsr #0x10
	bl ov00_02212D10
	add r8, r8, #2
	sub r7, r7, #2
	b _02214E7C
_02214E28:
	cmp r6, #0
	ldrneb r0, [r8], #1
	subne r7, r7, #1
	moveq r0, #0xff
	cmp r0, #0xff
	bne _02214E68
	ldr r1, [r1]
	mov r0, sb
	mov r2, r8
	bl ov00_02212CD0
	mov r0, r8
	bl strlen
	add r0, r0, #1
	add r8, r8, r0
	sub r7, r7, r0
	b _02214E7C
_02214E68:
	add r0, sl, r0, lsl #2
	ldr r1, [r1]
	ldr r2, [r0, #0x84]
	mov r0, sb
	bl ov00_02212CD0
_02214E7C:
	add r5, r5, #1
	cmp r5, r4
	blt _02214D98
_02214E88:
	mov r0, sb
	bl ov00_02213394
	orr r1, r0, #1
	mov r0, sb
	and r1, r1, #0xff
	bl ov00_0221338C
_02214EA0:
	ldr r0, [sp]
	tst r0, #0x80
	beq _02214F1C
	b _02214EEC
_02214EB0:
	mov r0, r8
	mov r4, r8
	bl strlen
	add r3, r0, #1
	add r8, r8, r3
	mov r0, sb
	mov r1, r4
	mov r2, r8
	sub r7, r7, r3
	bl ov00_02212CD0
	mov r0, r8
	bl strlen
	add r0, r0, #1
	add r8, r8, r0
	sub r7, r7, r0
_02214EEC:
	ldrsb r0, [r8]
	cmp r0, #0
	beq _02214F00
	cmp r7, #0
	bgt _02214EB0
_02214F00:
	mov r0, sb
	sub r7, r7, #1
	bl ov00_02213394
	orr r1, r0, #2
	mov r0, sb
	and r1, r1, #0xff
	bl ov00_0221338C
_02214F1C:
	ldr r0, [sp, #4]
	sub r0, r0, r7
	add sp, sp, #0x10
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	arm_func_end ov00_02214C60

	arm_func_start ov00_02214F2C
ov00_02214F2C: ; 0x02214F2C
	stmdb sp!, {r3, r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0xc
	mov r4, r2
	cmp r4, #1
	mov r6, r0
	mov r5, r1
	addlt sp, sp, #0xc
	movlt r0, #0
	ldmltia sp!, {r3, r4, r5, r6, r7, r8, pc}
	ldrb r8, [r5]
	mov r0, r8
	bl ov00_02214A44
	mov r7, r0
	cmp r4, r7
	addlt sp, sp, #0xc
	movlt r0, #0
	ldmltia sp!, {r3, r4, r5, r6, r7, r8, pc}
	tst r8, #0x40
	beq _02214F98
	mov r0, r6
	add r1, r5, r7
	sub r2, r4, r7
	bl ov00_02214AF4
	cmp r0, #0
	addeq sp, sp, #0xc
	moveq r0, #0
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, pc}
_02214F98:
	tst r8, #0x80
	beq _02214FBC
	add r0, r5, r7
	sub r1, r4, r7
	bl ov00_02214A70
	cmp r0, #0
	addeq sp, sp, #0xc
	moveq r0, #0
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, pc}
_02214FBC:
	ldr r1, _02215058 ; =ov00_0221A63C
	add r0, r5, #1
	mov r2, #4
	bl memcmp
	cmp r0, #0
	addeq sp, sp, #0xc
	mvneq r0, #0
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, pc}
	add r7, sp, #4
	add r3, sp, #8
	mov r0, r6
	mov r1, r5
	mov r2, r4
	str r7, [sp]
	bl ov00_02214BF0
	ldrh r2, [sp, #4]
	ldr r1, [sp, #8]
	mov r0, r6
	bl ov00_022132C4
	mov r7, r0
	bl ov00_0221339C
	cmp r0, #0
	addne sp, sp, #0xc
	mvnne r0, #1
	ldmneia sp!, {r3, r4, r5, r6, r7, r8, pc}
	mov ip, #1
	mov r0, r6
	mov r1, r7
	mov r2, r5
	mov r3, r4
	str ip, [sp]
	bl ov00_02214C60
	mov r4, r0
	mov r0, r6
	mov r1, r7
	bl ov00_02213C6C
	mov r0, r4
	add sp, sp, #0xc
	ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
	.balign 4, 0
_02215058: .word ov00_0221A63C
	arm_func_end ov00_02214F2C
