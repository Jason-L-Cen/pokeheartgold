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
	.extern ov00_02210E58
	.extern ov00_02210ECC
	.include "asm/macros.inc"
	.include "overlay_00_arm.inc"
	.include "global.inc"
	.public ov00_0221075C
	.extern ov00_0220F9A8
	.extern ov00_0220FB6C
	.extern ov00_0220FB90
	.extern ov00_022104D4
	.extern ov00_02210590

.text
	arm_func_start ov00_02210658
ov00_02210658: ; 0x02210658
	stmdb sp!, {r3, r4, lr}
	sub sp, sp, #4
	mov r3, r1
	ldrb r1, [r3, #7]
	mov r4, r0
	cmp r1, #1
	beq _02210684
	cmp r1, #2
	beq _02210718
	add sp, sp, #4
	ldmia sp!, {r3, r4, pc}
_02210684:
	ldrb r0, [r3, #0xc]
	cmp r0, #2
	addhi sp, sp, #4
	ldmhiia sp!, {r3, r4, pc}
	add r0, r4, r0, lsl #2
	mov r2, #1
	str r2, [r0, #0x14]
	ldr r0, [r4, #0x10]
	cmp r0, #0
	addne sp, sp, #4
	ldmneia sp!, {r3, r4, pc}
	ldr r0, [r4, #0x18]
	cmp r0, #0
	ldrne r0, [r4, #0x1c]
	cmpne r0, #0
	addeq sp, sp, #4
	ldmeqia sp!, {r3, r4, pc}
	ldr r1, [r4, #4]
	sub r0, r2, #2
	cmp r1, r0
	beq _022106E8
	ldr r0, [r4, #0x14]
	cmp r0, #0
	addeq sp, sp, #4
	ldmeqia sp!, {r3, r4, pc}
_022106E8:
	mov r0, #1
	str r0, [r4, #0x10]
	bl ov00_021FF368
	add r0, r0, #0x710
	add r0, r0, #0x2000
	str r0, [r4, #0x28]
	ldr r0, [r4, #0x10]
	ldr r1, [r4, #0x3c]
	ldr r2, [r4, #0x34]
	blx r2
	add sp, sp, #4
	ldmia sp!, {r3, r4, pc}
_02210718:
	mov r0, #3
	strb r0, [r3, #7]
	ldrh r0, [r2, #2]
	mov ip, #0x15
	mov r1, r0, asr #8
	mov r0, r0, lsl #8
	str ip, [sp]
	and r1, r1, #0xff
	and r0, r0, #0xff00
	orr r0, r1, r0
	mov ip, r0, lsl #0x10
	ldr r1, [r2, #4]
	ldr r0, [r4]
	mov r2, ip, lsr #0x10
	bl ov00_0220FB90
	add sp, sp, #4
	ldmia sp!, {r3, r4, pc}
	arm_func_end ov00_02210658

	arm_func_start ov00_0221075C
ov00_0221075C: ; 0x0221075C
	stmdb sp!, {r4, r5, r6, r7, lr}
	sub sp, sp, #0x2c
	mov r6, r0
	mov r7, r1
	mov r5, r2
	bl ov00_0220FB6C
	cmp r0, #0
	addeq sp, sp, #0x2c
	ldmeqia sp!, {r4, r5, r6, r7, pc}
	ldrb r4, [r6, #7]
	cmp r4, #5
	cmpne r4, #7
	bne _0221082C
	cmp r7, #0x14
	addlt sp, sp, #0x2c
	ldmltia sp!, {r4, r5, r6, r7, pc}
	add r3, sp, #0x18
	mov r2, #0xa
_022107A4:
	ldrb r1, [r6]
	ldrb r0, [r6, #1]
	add r6, r6, #2
	subs r2, r2, #1
	strb r1, [r3]
	strb r0, [r3, #1]
	add r3, r3, #2
	bne _022107A4
	ldr r3, [sp, #0x20]
	mov r1, r3, lsr #0x18
	mov r0, r3, lsr #8
	mov r2, r3, lsl #8
	and r1, r1, #0xff
	and r0, r0, #0xff00
	mov r3, r3, lsl #0x18
	orr r0, r1, r0
	and r2, r2, #0xff0000
	and r1, r3, #0xff000000
	orr r0, r2, r0
	orr r0, r1, r0
	bl ov00_0220F9A8
	cmp r0, #0
	addeq sp, sp, #0x2c
	ldmeqia sp!, {r4, r5, r6, r7, pc}
	cmp r4, #5
	add r1, sp, #0x18
	mov r2, r5
	bne _02210820
	bl ov00_022104D4
	add sp, sp, #0x2c
	ldmia sp!, {r4, r5, r6, r7, pc}
_02210820:
	bl ov00_02210590
	add sp, sp, #0x2c
	ldmia sp!, {r4, r5, r6, r7, pc}
_0221082C:
	cmp r7, #0x15
	addlt sp, sp, #0x2c
	ldmltia sp!, {r4, r5, r6, r7, pc}
	add r3, sp, #0
	mov r2, #0xa
_02210840:
	ldrb r1, [r6]
	ldrb r0, [r6, #1]
	add r6, r6, #2
	subs r2, r2, #1
	strb r1, [r3]
	strb r0, [r3, #1]
	add r3, r3, #2
	bne _02210840
	ldrb r0, [r6]
	strb r0, [r3]
	ldr r3, [sp, #8]
	mov r1, r3, lsr #0x18
	mov r0, r3, lsr #8
	mov r2, r3, lsl #8
	and r1, r1, #0xff
	and r0, r0, #0xff00
	mov r3, r3, lsl #0x18
	orr r0, r1, r0
	and r2, r2, #0xff0000
	and r1, r3, #0xff000000
	orr r0, r2, r0
	orr r0, r1, r0
	bl ov00_0220F9A8
	cmp r0, #0
	addeq sp, sp, #0x2c
	ldmeqia sp!, {r4, r5, r6, r7, pc}
	add r1, sp, #0
	mov r2, r5
	bl ov00_02210658
	add sp, sp, #0x2c
	ldmia sp!, {r4, r5, r6, r7, pc}
	arm_func_end ov00_0221075C

	arm_func_start ov00_022108BC
ov00_022108BC: ; 0x022108BC
	stmdb sp!, {r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0x40
	movs r5, r0
	mov r8, r1
	mov r4, r2
	mov r7, r3
	ldreq r6, _02210A68 ; =ov00_02219D74
	beq _022108EC
	mov r0, #0x110
	bl ov00_021EC3A8
	mov r6, r0
	str r0, [r5]
_022108EC:
	bl ov00_021FF368
	bl srand
	mov r1, r7
	add r0, r6, #4
	bl strcpy
	ldr r1, [sp, #0x58]
	add r0, r6, #0x44
	bl strcpy
	str r4, [r6, #0xc0]
	mov r5, #0
	str r5, [r6, #0xac]
	str r5, [r6, #0xb0]
	str r8, [r6]
	mov r1, #1
	str r1, [r6, #0xb8]
	ldr r0, [sp, #0x7c]
	ldr r1, [sp, #0x64]
	str r0, [r6, #0x10c]
	str r1, [r6, #0x88]
	ldr r0, [sp, #0x68]
	ldr r1, [sp, #0x6c]
	str r0, [r6, #0x8c]
	str r1, [r6, #0x90]
	ldr r0, [sp, #0x70]
	ldr r1, [sp, #0x74]
	str r0, [r6, #0x94]
	ldr r0, [sp, #0x78]
	str r1, [r6, #0x98]
	str r0, [r6, #0x9c]
	str r5, [r6, #0xa0]
	str r5, [r6, #0xa4]
	ldr r0, [sp, #0x5c]
	str r5, [r6, #0xd4]
	str r0, [r6, #0xbc]
	ldr r0, [sp, #0x60]
	str r5, [r6, #0xc4]
	str r0, [r6, #0xc8]
	str r5, [r6, #0x104]
	add r0, r6, #0x100
	strh r5, [r0, #8]
	str r5, [r6, #0xa8]
	ldr r4, _02210A6C ; =0x80808081
	str r5, [r6, #0xb4]
	mov r8, #0xff
_0221099C:
	bl rand
	smull r1, r2, r4, r0
	add r3, r6, r5
	add r5, r5, #1
	add r2, r0, r2
	mov r1, r0, lsr #0x1f
	add r2, r1, r2, asr #7
	smull r1, r2, r8, r2
	sub r2, r0, r1
	strb r2, [r3, #0x84]
	cmp r5, #4
	blt _0221099C
	mov r2, #0
	mvn r1, #0
_022109D4:
	add r0, r6, r2, lsl #2
	add r2, r2, #1
	str r1, [r0, #0xd8]
	cmp r2, #0xa
	blt _022109D4
	mov r1, #0
	ldr r0, _02210A70 ; =ov00_0221B878
	str r1, [r6, #0x100]
	ldr r0, [r0]
	cmp r0, #0
	bne _02210A04
	bl ov00_02210E58
_02210A04:
	ldr r0, [sp, #0x5c]
	cmp r0, #0
	beq _02210A50
	ldr r0, _02210A74 ; =ov00_0221B890
	ldrsb r4, [r0]
	cmp r4, #0
	bne _02210A30
	ldr r1, _02210A78 ; =ov00_02219E84
	add r0, sp, #0
	mov r2, r7
	bl sprintf
_02210A30:
	cmp r4, #0
	ldrne r0, _02210A74 ; =ov00_0221B890
	ldr r1, _02210A7C ; =0x00006CFC
	addeq r0, sp, #0
	add r2, r6, #0xcc
	mov r3, #0
	bl ov00_02210ECC
	b _02210A54
_02210A50:
	mov r0, #1
_02210A54:
	cmp r0, #0
	moveq r0, #3
	movne r0, #0
	add sp, sp, #0x40
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	.balign 4, 0
_02210A68: .word ov00_02219D74
_02210A6C: .word 0x80808081
_02210A70: .word ov00_0221B878
_02210A74: .word ov00_0221B890
_02210A78: .word ov00_02219E84
_02210A7C: .word 0x00006CFC
	arm_func_end ov00_022108BC
