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
	.public ov00_02211608
	.public ov00_022116C0
	.extern ov00_02210DFC
	.extern ov00_022115B0

.text
	arm_func_start ov00_02211608
ov00_02211608: ; 0x02211608
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	sub sp, sp, #0x10
	mov r5, #0
	cmp r3, #3
	mov r6, r5
	mov r7, r5
	addlt sp, sp, #0x10
	ldmltia sp!, {r3, r4, r5, r6, r7, pc}
	ldrb ip, [r2], #1
	sub r3, r3, #1
	cmp ip, #0
	cmpne ip, #0xff
	movne r5, r2
	subne r3, r3, ip
	addne r2, r2, ip
	cmp r3, #2
	addlt sp, sp, #0x10
	ldmltia sp!, {r3, r4, r5, r6, r7, pc}
	ldrb lr, [r2], #1
	sub r3, r3, #1
	cmp lr, #0
	cmpne lr, #0xff
	movne r6, r2
	subne r3, r3, lr
	addne r2, r2, lr
	cmp r3, #1
	addlt sp, sp, #0x10
	ldmltia sp!, {r3, r4, r5, r6, r7, pc}
	ldrb r4, [r2]
	sub r3, r3, #1
	cmp r4, #0
	cmpne r4, #0xff
	subne r3, r3, r4
	addne r7, r2, #1
	cmp r3, #0
	addlt sp, sp, #0x10
	ldmltia sp!, {r3, r4, r5, r6, r7, pc}
	str lr, [sp]
	str r6, [sp, #4]
	str r4, [sp, #8]
	mov r2, ip
	mov r3, r5
	str r7, [sp, #0xc]
	bl ov00_022115B0
	add sp, sp, #0x10
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	arm_func_end ov00_02211608

	arm_func_start ov00_022116C0
ov00_022116C0: ; 0x022116C0
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x188
	mov r8, r2
	sub r2, r8, #1
	mov r3, #0
	cmp r2, #1
	mov sl, r0
	mov sb, r1
	str r3, [sp, #0x104]
	movhi fp, #1
	bhi _02211700
	ldr r1, [sl, #0x10c]
	ldr r2, [sl, #0x98]
	mov r0, r8
	blx r2
	mov fp, r0
_02211700:
	ldr r2, [sl, #0x10c]
	ldr r3, [sl, #0x94]
	add r1, sp, #4
	mov r0, r8
	blx r3
	ldr r1, [sp, #0x104]
	mov r0, #0
	str r0, [sp]
	cmp r1, #0
	addle sp, sp, #0x188
	ldmleia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	add r6, sp, #4
_02211730:
	ldrb r1, [r6]
	ldr r0, _02211888 ; =ov00_0221A0B8
	ldr r5, [r0, r1, lsl #2]
	cmp r5, #0
	ldreq r5, _0221188C ; =ov00_02219EFC
	cmp r8, #0
	bne _022117AC
	mov r0, sb
	mov r1, r5
	bl ov00_02210DFC
	ldr r0, [sb, #0x800]
	mov r1, #0x5c
	add r0, sb, r0
	strb r1, [r0, #-1]
	ldrb r0, [r6]
	ldr r2, [sl, #0x10c]
	ldr r3, [sl, #0x88]
	mov r1, sb
	ldr r4, [sb, #0x800]
	blx r3
	ldr r0, [sb, #0x800]
	cmp r4, r0
	bne _02211798
	ldr r1, _02211890 ; =ov00_02219F04
	mov r0, sb
	bl ov00_02210DFC
_02211798:
	ldr r0, [sb, #0x800]
	mov r1, #0x5c
	add r0, sb, r0
	strb r1, [r0, #-1]
	b _02211864
_022117AC:
	cmp fp, #0
	mov r4, #0
	ble _02211864
_022117B8:
	ldr r1, _02211894 ; =ov00_02219F08
	add r0, sp, #0x108
	mov r2, r5
	mov r3, r4
	bl sprintf
	mov r0, sb
	add r1, sp, #0x108
	bl ov00_02210DFC
	ldr r0, [sb, #0x800]
	cmp r8, #1
	add r1, sb, r0
	mov r0, #0x5c
	strb r0, [r1, #-1]
	ldr r7, [sb, #0x800]
	bne _02211810
	ldrb r0, [r6]
	mov r1, r4
	mov r2, sb
	ldr r3, [sl, #0x10c]
	ldr ip, [sl, #0x8c]
	blx ip
	b _02211830
_02211810:
	cmp r8, #2
	bne _02211830
	ldrb r0, [r6]
	mov r1, r4
	mov r2, sb
	ldr r3, [sl, #0x10c]
	ldr ip, [sl, #0x90]
	blx ip
_02211830:
	ldr r0, [sb, #0x800]
	cmp r7, r0
	bne _02211848
	ldr r1, _02211890 ; =ov00_02219F04
	mov r0, sb
	bl ov00_02210DFC
_02211848:
	ldr r0, [sb, #0x800]
	add r4, r4, #1
	add r1, sb, r0
	mov r0, #0x5c
	strb r0, [r1, #-1]
	cmp r4, fp
	blt _022117B8
_02211864:
	ldr r0, [sp]
	ldr r1, [sp, #0x104]
	add r0, r0, #1
	str r0, [sp]
	cmp r0, r1
	add r6, r6, #1
	blt _02211730
	add sp, sp, #0x188
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.balign 4, 0
_02211888: .word ov00_0221A0B8
_0221188C: .word ov00_02219EFC
_02211890: .word ov00_02219F04
_02211894: .word ov00_02219F08
	arm_func_end ov00_022116C0
