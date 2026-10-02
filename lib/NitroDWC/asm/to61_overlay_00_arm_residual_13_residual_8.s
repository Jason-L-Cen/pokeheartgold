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
	.public ov00_02211A88
	.extern ov00_022111D8
	.extern ov00_02211208
	.extern ov00_022112A8
	.extern ov00_02211608
	.extern ov00_02211898
	.extern ov00_022118FC
	.extern ov00_02211A24

.text
	arm_func_start ov00_02211A88
ov00_02211A88: ; 0x02211A88
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, lr}
	sub sp, sp, #0x810
	movs r8, r0
	mov r0, #0
	str r0, [sp, #0x80c]
	ldreq r0, _02211D98 ; =ov00_02219D70
	mov r7, r2
	ldreq r8, [r0]
	ldrsb r0, [r1]
	mov r6, r3
	cmp r0, #0x3b
	bne _02211AE0
	ldr r3, [r8, #0xd4]
	cmp r3, #0
	addeq sp, sp, #0x810
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	mov r0, r1
	mov r1, r7
	mov r2, r6
	blx r3
	add sp, sp, #0x810
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
_02211AE0:
	cmp r0, #0x5c
	bne _02211B1C
	add r1, sp, #0xc
	mov r0, r8
	bl ov00_02211898
	str r6, [sp]
	mov r0, #8
	str r0, [sp, #4]
	ldr r0, [r8]
	ldr r2, [sp, #0x80c]
	add r1, sp, #0xc
	mov r3, #0
	bl ov00_021FF984
	add sp, sp, #0x810
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
_02211B1C:
	cmp r7, #7
	addlt sp, sp, #0x810
	ldmltia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	and r0, r0, #0xff
	cmp r0, #0xfe
	ldreqb r0, [r1, #1]
	cmpeq r0, #0xfd
	addne sp, sp, #0x810
	ldmneia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	ldr r0, [r8, #0xb8]
	add r4, r1, #3
	cmp r0, #0
	movgt r0, #0
	strgt r0, [r8, #0xb8]
	ldrsb sb, [r1, #2]
	add r5, r1, #7
	add r0, sp, #0xc
	mov r1, sb
	mov r2, r4
	sub r7, r7, #7
	bl ov00_022111D8
	cmp sb, #8
	addls pc, pc, sb, lsl #2
	b _02211D68
_02211B7C: ; jump table
	b _02211BA0 ; case 0
	b _02211BB8 ; case 1
	b _02211BF4 ; case 2
	b _02211D68 ; case 3
	b _02211C2C ; case 4
	b _02211D68 ; case 5
	b _02211CA0 ; case 6
	b _02211D68 ; case 7
	b _02211D68 ; case 8
_02211BA0:
	add r1, sp, #0xc
	mov r0, r8
	mov r2, r5
	mov r3, r7
	bl ov00_02211608
	b _02211D70
_02211BB8:
	cmp r7, #0xd
	blt _02211BDC
	ldr r0, [r8, #0xa8]
	cmp r0, #0
	beq _02211BDC
	add r1, r5, r7
	mov r0, r8
	sub r1, r1, #0xd
	bl ov00_022112A8
_02211BDC:
	add r1, sp, #0xc
	mov r0, r8
	mov r2, r5
	mov r3, r7
	bl ov00_02211208
	b _02211D70
_02211BF4:
	ldr r0, [sp, #0x80c]
	cmp r7, #0x20
	movgt r7, #0x20
	add r3, sp, #0xc
	mov r4, #5
	mov r1, r5
	mov r2, r7
	add r0, r3, r0
	strb r4, [sp, #0xc]
	bl memcpy
	ldr r0, [sp, #0x80c]
	add r0, r0, r7
	str r0, [sp, #0x80c]
	b _02211D70
_02211C2C:
	ldr r1, [r8, #0xb8]
	mvn r0, #0
	cmp r1, r0
	addeq sp, sp, #0x810
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	mov r2, #0
	str r2, [sp, #8]
_02211C48:
	add r0, r8, r2
	ldrsb r1, [r4, r2]
	ldrsb r0, [r0, #0x84]
	cmp r1, r0
	addne sp, sp, #0x810
	ldmneia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	add r2, r2, #1
	str r2, [sp, #8]
	cmp r2, #4
	blt _02211C48
	cmp r7, #2
	addlt sp, sp, #0x810
	ldmltia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	mvn r0, #0
	str r0, [r8, #0xb8]
	ldrsb r0, [r5]
	ldr r2, [r8, #0x10c]
	ldr r3, [r8, #0x9c]
	add r1, r5, #1
	blx r3
	add sp, sp, #0x810
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
_02211CA0:
	mov r2, #0
	str r2, [sp, #8]
_02211CA8:
	add r0, r8, r2
	ldrsb r1, [r4, r2]
	ldrsb r0, [r0, #0x84]
	cmp r1, r0
	addne sp, sp, #0x810
	ldmneia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	add r2, r2, #1
	str r2, [sp, #8]
	cmp r2, #4
	blt _02211CA8
	cmp r7, #4
	addlt sp, sp, #0x810
	ldmltia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	mov r0, #7
	strb r0, [sp, #0xc]
	ldrb r1, [r5]
	ldrb r0, [r5, #1]
	ldr r2, [sp, #0x80c]
	add r4, sp, #0xc
	strb r1, [r4, r2]
	add r4, r4, r2
	strb r0, [r4, #1]
	ldrb r2, [r5, #2]
	ldrb r1, [r5, #3]
	add r3, sp, #8
	mov r0, r8
	strb r2, [r4, #2]
	strb r1, [r4, #3]
	ldr r1, [sp, #0x80c]
	add r1, r1, #4
	str r1, [sp, #0x80c]
	ldrb r2, [r5]
	ldrb r1, [r5, #1]
	strb r2, [r3]
	strb r1, [r3, #1]
	ldrb r2, [r5, #2]
	ldrb r1, [r5, #3]
	strb r2, [r3, #2]
	strb r1, [r3, #3]
	ldr r1, [sp, #8]
	bl ov00_02211A24
	cmp r0, #0
	bne _02211D70
	mov r0, r8
	add r1, r5, #4
	sub r2, r7, #4
	bl ov00_022118FC
	b _02211D70
_02211D68:
	add sp, sp, #0x810
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
_02211D70:
	str r6, [sp]
	mov r0, #8
	str r0, [sp, #4]
	ldr r0, [r8]
	ldr r2, [sp, #0x80c]
	add r1, sp, #0xc
	mov r3, #0
	bl ov00_021FF984
	add sp, sp, #0x810
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	.balign 4, 0
_02211D98: .word ov00_02219D70
	arm_func_end ov00_02211A88
