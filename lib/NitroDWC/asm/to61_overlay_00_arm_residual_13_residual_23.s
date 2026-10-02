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
	.public ov00_02214558
	.public ov00_022145F0
	.extern ov00_02214130
	.extern ov00_02214244
	.extern ov00_02214370
	.extern ov00_022143C0
	.extern ov00_022143E4
	.extern ov00_02214438
	.extern ov00_02214908

.text
	arm_func_start ov00_0221446C
ov00_0221446C: ; 0x0221446C
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	mov sl, r0
	bl rand
	ldr r5, _02214554 ; =0x2C0B02C1
	mov r8, #0
	smull r2, r4, r5, r0
	mov r1, r0, lsr #0x1f
	add r4, r1, r4, asr #4
	mov r3, #0x5d
	smull r1, r2, r3, r4
	sub r4, r0, r1
	add r0, r4, #0x21
	mov r7, #1
	strb r0, [sl, #0x74]
	mov fp, r8
	mov r6, r7
	mov r4, r3
_022144B0:
	add sb, sl, r7
	ldrsb r3, [sb, #0x73]
	ldrsb r0, [sl, #0x74]
	cmp r3, r0
	eor r3, r7, r3
	movlt r1, r6
	and r3, r3, #1
	movge r1, fp
	cmp r0, #0x4f
	movlt r2, #1
	and r0, r0, #1
	eor r3, r8, r3
	movge r2, #0
	eor r0, r0, r3
	eor r0, r0, r2
	eor r8, r0, r1
	bl rand
	smull r2, r3, r5, r0
	mov r1, r0, lsr #0x1f
	add r3, r1, r3, asr #4
	smull r1, r2, r4, r3
	sub r3, r0, r1
	add r0, r3, #0x21
	cmp r8, #0
	strb r0, [sb, #0x74]
	beq _02214524
	ldrsb r0, [sb, #0x74]
	tst r0, #1
	beq _02214538
_02214524:
	cmp r8, #0
	ldreqsb r0, [sb, #0x74]
	andeq r0, r0, #1
	cmpeq r0, #1
	bne _02214544
_02214538:
	ldrsb r0, [sb, #0x74]
	add r0, r0, #1
	strb r0, [sb, #0x74]
_02214544:
	add r7, r7, #1
	cmp r7, #8
	blt _022144B0
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.balign 4, 0
_02214554: .word 0x2C0B02C1
	arm_func_end ov00_0221446C

	arm_func_start ov00_02214558
ov00_02214558: ; 0x02214558
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	mov sl, r0
	mov sb, r1
	mov r8, r2
	mov r7, #1
	mov fp, #2
	mov r5, #0
_02214574:
	ldr r0, [sl, #0x4b0]
	mov r1, sb
	mov r2, r8
	mov r3, r5
	sub r7, r7, #1
	bl ov00_021FF970
	mov r6, r0
	cmp r6, #0
	bgt _022145E0
	cmp r7, #0
	blt _022145E0
	mov r0, sl
	bl ov00_02214908
	mov r1, #0
	mov r2, r1
	mov r0, sl
	mov r3, fp
	str r2, [sp]
	bl ov00_022145F0
	movs r4, r0
	beq _022145D8
	mov r0, sl
	bl ov00_02214130
	mov r0, r4
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_022145D8:
	cmp r7, #0
	bge _02214574
_022145E0:
	cmp r6, #0
	movle r0, #3
	movgt r0, #0
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	arm_func_end ov00_02214558

	arm_func_start ov00_022145F0
ov00_022145F0: ; 0x022145F0
	stmdb sp!, {r4, r5, r6, r7, lr}
	sub sp, sp, #0x30c
	mov r5, r0
	ldr r0, [r5]
	mov r7, r1
	mov r6, r2
	mov r4, r3
	cmp r0, #1
	beq _02214628
	ldr r0, _0221484C ; =ov00_0221A618
	ldr r1, _02214850 ; =ov00_0221A5C8
	ldr r2, _02214854 ; =ov00_0221A59C
	ldr r3, _02214858 ; =0x0000020A
	bl __msl_assertion_failed
_02214628:
	cmp r7, #0
	ldreq r7, _0221485C ; =ov00_0221A5B8
	cmp r6, #0
	ldreq r6, _0221485C ; =ov00_0221A5B8
	mov r0, r7
	bl strlen
	cmp r0, #0x100
	addhi sp, sp, #0x30c
	movhi r0, #6
	ldmhiia sp!, {r4, r5, r6, r7, pc}
	mov r0, r6
	bl strlen
	cmp r0, #0x100
	addhi sp, sp, #0x30c
	movhi r0, #6
	ldmhiia sp!, {r4, r5, r6, r7, pc}
	mov r0, r5
	bl ov00_02214244
	cmp r0, #0
	addne sp, sp, #0x30c
	ldmneia sp!, {r4, r5, r6, r7, pc}
	mov r0, r5
	str r4, [r5, #0x5c4]
	bl ov00_0221446C
	add r3, sp, #0xe
	mov ip, #2
	add r0, sp, #4
	add r2, sp, #8
	mov r1, #0
	str ip, [sp, #8]
	str r3, [sp, #4]
	bl ov00_022143C0
	add r0, sp, #4
	add r2, sp, #8
	mov r1, #1
	bl ov00_022143C0
	add r0, sp, #4
	mov r1, #3
	add r2, sp, #8
	bl ov00_022143C0
	ldr r1, [r5, #0x4b8]
	add r0, sp, #4
	add r2, sp, #8
	bl ov00_022143E4
	add r0, sp, #4
	add r1, r5, #0xc
	add r2, sp, #8
	bl ov00_02214370
	add r0, sp, #4
	add r1, r5, #0x30
	add r2, sp, #8
	bl ov00_02214370
	add r0, sp, #4
	add r1, r5, #0x74
	mov r2, #8
	add r3, sp, #8
	bl ov00_02214438
	mov r1, r6
	add r0, sp, #4
	add r2, sp, #8
	bl ov00_02214370
	mov r1, r7
	add r0, sp, #4
	add r2, sp, #8
	bl ov00_02214370
	mov r1, r4, lsl #0x18
	and ip, r1, #0xff000000
	mov r1, r4, lsl #8
	and r3, r1, #0xff0000
	mov r1, r4, lsr #0x18
	and r2, r1, #0xff
	mov r1, r4, lsr #8
	and r1, r1, #0xff00
	orr r1, r2, r1
	orr r1, r3, r1
	add r0, sp, #4
	orr r1, ip, r1
	add r2, sp, #8
	bl ov00_022143E4
	ldr r0, [r5, #0x5c4]
	tst r0, #8
	beq _02214780
	ldr r1, [r5, #0x4a4]
	add r0, sp, #4
	add r2, sp, #8
	bl ov00_022143E4
_02214780:
	ldr r0, [r5, #0x5c4]
	tst r0, #0x80
	beq _0221479C
	ldr r1, [sp, #0x320]
	add r0, sp, #4
	add r2, sp, #8
	bl ov00_022143E4
_0221479C:
	ldr r2, [sp, #8]
	add r4, sp, #0
	mov r0, r2, lsl #0x10
	mov r0, r0, lsr #0x10
	mov r1, r0, asr #8
	mov r0, r0, lsl #8
	and r1, r1, #0xff
	and r0, r0, #0xff00
	orr r0, r1, r0
	strh r0, [sp]
	ldrb r3, [r4]
	add r1, sp, #0xc
	ldrb r0, [r4, #1]
	strb r3, [r1]
	mov r3, #0
	strb r0, [r1, #1]
	ldr r0, [r5, #0x4b0]
	bl ov00_021FF970
	cmp r0, #0
	bgt _02214800
	mov r0, r5
	bl ov00_02214908
	add sp, sp, #0x30c
	mov r0, #3
	ldmia sp!, {r4, r5, r6, r7, pc}
_02214800:
	mov r0, #3
	str r0, [r5]
	mov r0, #0
	str r0, [r5, #0x5c8]
	ldr r0, [r5, #0x7c]
	cmp r0, #0
	bne _02214840
	mov r0, #0x1000
	bl ov00_021EC3A8
	cmp r0, #0
	str r0, [r5, #0x7c]
	addeq sp, sp, #0x30c
	moveq r0, #5
	ldmeqia sp!, {r4, r5, r6, r7, pc}
	mov r0, #0
	str r0, [r5, #0x80]
_02214840:
	mov r0, #0
	add sp, sp, #0x30c
	ldmia sp!, {r4, r5, r6, r7, pc}
	.balign 4, 0
_0221484C: .word ov00_0221A618
_02214850: .word ov00_0221A5C8
_02214854: .word ov00_0221A59C
_02214858: .word 0x0000020A
_0221485C: .word ov00_0221A5B8
	arm_func_end ov00_022145F0
