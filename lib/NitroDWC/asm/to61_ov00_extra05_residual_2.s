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
	.public ov00_02215A30
	.extern ov00_02215490
	.extern ov00_0221556C
	.extern ov00_02215728
	.extern ov00_022158AC
	.extern ov00_02215938

.text
	arm_func_start ov00_02215A30
ov00_02215A30: ; 0x02215A30
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	mov sb, r0
	ldr r0, [sb, #0x80]
	mov r8, #0
	cmp r0, #3
	blt _02215BCC
	ldr r5, _02215BE4 ; =ov00_0221A650
	ldr r4, _02215BE8 ; =ov00_0221A5C8
	ldr sl, _02215BEC ; =ov00_0221A574
	ldr fp, _02215BF0 ; =0x000005B4
	mov r6, r8
	add r7, sp, #0
_02215A60:
	ldr r0, [sb, #0x7c]
	ldrb r1, [r0]
	ldrb r0, [r0, #1]
	strb r1, [r7]
	strb r0, [r7, #1]
	ldrh r0, [sp]
	mov r1, r0, asr #8
	mov r0, r0, lsl #8
	and r1, r1, #0xff
	and r0, r0, #0xff00
	orr r0, r1, r0
	strh r0, [sp]
	ldrh r2, [sp]
	cmp r2, #0x1000
	movhi r8, #4
	bhi _02215BCC
	ldr r0, [sb, #0x80]
	cmp r0, r2
	movlt r0, #0
	ldmltia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldr r1, [sb, #0x7c]
	ldrsb r0, [r1, #2]
	cmp r0, #6
	addls pc, pc, r0, lsl #2
	b _02215B70
_02215AC4: ; jump table
	b _02215B70 ; case 0
	b _02215AE0 ; case 1
	b _02215AF8 ; case 2
	b _02215B10 ; case 3
	b _02215B2C ; case 4
	b _02215B44 ; case 5
	b _02215B5C ; case 6
_02215AE0:
	mov r0, sb
	add r1, r1, #3
	sub r2, r2, #3
	bl ov00_02215490
	mov r8, r0
	b _02215B70
_02215AF8:
	mov r0, sb
	add r1, r1, #3
	sub r2, r2, #3
	bl ov00_02215938
	mov r8, r0
	b _02215B70
_02215B10:
	ldr r0, [sb, #0x4b0]
	mov r3, r6
	bl ov00_021FF970
	cmp r0, #0
	bgt _02215B70
	mov r0, #3
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_02215B2C:
	mov r0, sb
	add r1, r1, #3
	sub r2, r2, #3
	bl ov00_022158AC
	mov r8, r0
	b _02215B70
_02215B44:
	mov r0, sb
	add r1, r1, #3
	sub r2, r2, #3
	bl ov00_02215728
	mov r8, r0
	b _02215B70
_02215B5C:
	mov r0, sb
	add r1, r1, #3
	sub r2, r2, #3
	bl ov00_0221556C
	mov r8, r0
_02215B70:
	ldrh r0, [sp]
	ldr r1, [sb, #0x80]
	subs r0, r1, r0
	str r0, [sb, #0x80]
	bpl _02215B98
	mov r0, r5
	mov r1, r4
	mov r2, sl
	mov r3, fp
	bl __msl_assertion_failed
_02215B98:
	ldr r2, [sb, #0x80]
	cmp r2, #0
	ldrne r0, [sb, #0x7c]
	cmpne r0, #0
	beq _02215BB8
	ldrh r1, [sp]
	add r1, r0, r1
	bl memmove
_02215BB8:
	cmp r8, #0
	bne _02215BCC
	ldr r0, [sb, #0x80]
	cmp r0, #3
	bge _02215A60
_02215BCC:
	cmp r8, #0
	beq _02215BDC
	mov r0, sb
	bl ov00_02214130
_02215BDC:
	mov r0, r8
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.balign 4, 0
_02215BE4: .word ov00_0221A650
_02215BE8: .word ov00_0221A5C8
_02215BEC: .word ov00_0221A574
_02215BF0: .word 0x000005B4
	arm_func_end ov00_02215A30
