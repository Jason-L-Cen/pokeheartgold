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

.text
	arm_func_start ov00_02215CDC
ov00_02215CDC: ; 0x02215CDC
	stmdb sp!, {r0, r1, r2, r3}
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x18
	mov r5, r0
	ldr r1, [r5]
	mov r4, r3
	cmp r1, #1
	bne _02215D10
	mov r1, #0
	mov r2, r1
	mov r3, #2
	str r1, [sp]
	bl ov00_022145F0
_02215D10:
	ldr r0, [r5]
	cmp r0, #1
	addeq sp, sp, #0x18
	moveq r0, #3
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	addeq sp, sp, #0x10
	bxeq lr
	ldr r0, [sp, #0x50]
	add r6, sp, #0x44
	add r0, r0, #9
	mov r0, r0, lsl #0x10
	mov r0, r0, lsr #0x10
	mov r2, r0, asr #8
	mov r1, r0, lsl #8
	and r2, r2, #0xff
	and r1, r1, #0xff00
	orr r1, r2, r1
	strh r1, [sp, #4]
	add r2, sp, #4
	ldrb r3, [r2]
	add r0, sp, #0x48
	add r1, sp, #6
	ldrb r2, [r2, #1]
	strb r3, [r1]
	mov fp, #2
	strb r2, [r1, #1]
	ldrb sb, [r6]
	add sl, sp, #9
	ldrb r8, [r6, #1]
	ldrb r7, [r6, #2]
	ldrb r6, [r6, #3]
	ldrb ip, [r0]
	ldrb r3, [r0, #1]
	add lr, sp, #0xd
	mov r0, r5
	mov r2, #9
	strb fp, [sp, #8]
	strb sb, [sl]
	strb r8, [sl, #1]
	strb r7, [sl, #2]
	strb r6, [sl, #3]
	strb ip, [lr]
	strb r3, [lr, #1]
	bl ov00_02214558
	cmp r0, #0
	addne sp, sp, #0x18
	ldmneia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	addne sp, sp, #0x10
	bxne lr
	ldr r0, [r5, #0x4b0]
	ldr r2, [sp, #0x50]
	mov r1, r4
	mov r3, #0
	bl ov00_021FF970
	cmp r0, #0
	movlt r0, #3
	movge r0, #0
	add sp, sp, #0x18
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	add sp, sp, #0x10
	bx lr
	arm_func_end ov00_02215CDC

	arm_func_start ov00_02215E04
ov00_02215E04: ; 0x02215E04
	stmdb sp!, {r0, r1, r2, r3}
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x10
	ldr r5, [sp, #0x44]
	mov r7, #0xfd
	mov r4, r5, lsr #0x18
	mov r8, r5, lsl #0x18
	mov r3, r5, lsr #8
	mov r6, r5, lsl #8
	and r5, r4, #0xff
	and r4, r3, #0xff00
	orr sb, r5, r4
	and r3, r6, #0xff0000
	mov r6, #0xfc
	mov r5, #0x1e
	mov r4, #0x66
	mov lr, #0x6a
	mov fp, #0xb2
	strb r4, [sp, #7]
	and r8, r8, #0xff000000
	orr r3, r3, sb
	orr r8, r8, r3
	add r3, sp, #0x44
	str r8, [sp, #0x44]
	strb r7, [sp, #4]
	ldrb sl, [r3]
	add ip, sp, #0xa
	ldrb sb, [r3, #1]
	ldrb r8, [r3, #2]
	ldrb r7, [r3, #3]
	add r3, sp, #4
	strb r6, [sp, #5]
	strb r5, [sp, #6]
	strb lr, [sp, #8]
	strb fp, [sp, #9]
	strb sl, [ip]
	strb sb, [ip, #1]
	strb r8, [ip, #2]
	strb r7, [ip, #3]
	mov r4, #0xa
	str r4, [sp]
	bl ov00_02215CDC
	add sp, sp, #0x10
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	add sp, sp, #0x10
	bx lr
	arm_func_end ov00_02215E04
