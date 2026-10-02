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
	.public ov00_02213044
	.public ov00_0221321C
	.extern ov00_02212CD0
	.extern ov00_02213FD4

.text
	arm_func_start ov00_02213044
ov00_02213044: ; 0x02213044
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x8c
	mov sl, r1
	ldrsb r1, [sl]
	mov fp, r0
	mov sb, r2
	cmp r1, #0
	beq _022130C4
_02213064:
	mov r0, sl
	mov r1, sb
	bl ov00_02213FD4
	cmp r0, #0
	addlt sp, sp, #0x8c
	ldmltia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	mov r4, sl
	sub sb, sb, r0
	add sl, sl, r0
	mov r0, sl
	mov r1, sb
	bl ov00_02213FD4
	cmp r0, #0
	addlt sp, sp, #0x8c
	ldmltia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	mov r2, sl
	mov r1, r4
	add sl, sl, r0
	sub sb, sb, r0
	mov r0, fp
	bl ov00_02212CD0
	ldrsb r0, [sl]
	cmp r0, #0
	bne _02213064
_022130C4:
	mov r0, #0
	add sl, sl, #1
	sub sb, sb, #1
	str r0, [sp]
_022130D4:
	cmp sb, #2
	addlt sp, sp, #0x8c
	ldmltia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldrb r1, [sl]
	ldrb r0, [sl, #1]
	add r2, sp, #8
	add sl, sl, #2
	strb r1, [r2]
	strb r0, [r2, #1]
	ldrh r0, [sp, #8]
	str sl, [sp, #4]
	sub sb, sb, #2
	mov r1, r0, asr #8
	mov r0, r0, lsl #8
	and r1, r1, #0xff
	and r0, r0, #0xff00
	orr r0, r1, r0
	strh r0, [sp, #8]
	ldrsb r0, [sl]
	mov r5, #0
	cmp r0, #0
	beq _02213164
_0221312C:
	mov r0, sl
	mov r1, sb
	bl ov00_02213FD4
	cmp r0, #0
	addlt sp, sp, #0x8c
	ldmltia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	cmp r0, #0x64
	addgt sp, sp, #0x8c
	ldmgtia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldrsb r1, [sl, r0]!
	add r5, r5, #1
	sub sb, sb, r0
	cmp r1, #0
	bne _0221312C
_02213164:
	ldrh r0, [sp, #8]
	add sl, sl, #1
	sub sb, sb, #1
	cmp r0, #0
	mov r6, #0
	ble _022131FC
_0221317C:
	ldr r8, [sp, #4]
	mov r7, #0
	cmp r5, #0
	ble _022131EC
_0221318C:
	mov r0, sl
	mov r1, sb
	bl ov00_02213FD4
	movs r4, r0
	addmi sp, sp, #0x8c
	ldmmiia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldr r1, _02213218 ; =ov00_0221A520
	add r0, sp, #0xa
	mov r2, r8
	mov r3, r6
	bl sprintf
	mov r0, fp
	add r1, sp, #0xa
	mov r2, sl
	bl ov00_02212CD0
	mov r0, r8
	add sl, sl, r4
	sub sb, sb, r4
	bl strlen
	add r0, r0, #1
	add r7, r7, #1
	add r8, r8, r0
	cmp r7, r5
	blt _0221318C
_022131EC:
	ldrh r0, [sp, #8]
	add r6, r6, #1
	cmp r6, r0
	blt _0221317C
_022131FC:
	ldr r0, [sp]
	add r0, r0, #1
	str r0, [sp]
	cmp r0, #2
	blt _022130D4
	add sp, sp, #0x8c
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.balign 4, 0
_02213218: .word ov00_0221A520
	arm_func_end ov00_02213044

	arm_func_start ov00_0221321C
ov00_0221321C: ; 0x0221321C
	stmdb sp!, {r3, lr}
	ldrsb lr, [r0]
	mov ip, #0
	cmp lr, #0
	beq _0221325C
	ldr r3, _0221326C ; =__lower_mapC
	ldr r2, _02213270 ; =0x9CCF9319
_02213238:
	cmp lr, #0
	blt _0221324C
	cmp lr, #0x80
	bge _0221324C
	ldrb lr, [r3, lr]
_0221324C:
	mla ip, r2, ip, lr
	ldrsb lr, [r0, #1]!
	cmp lr, #0
	bne _02213238
_0221325C:
	mov r0, ip
	bl _u32_div_f
	mov r0, r1
	ldmia sp!, {r3, pc}
	.balign 4, 0
_0221326C: .word __lower_mapC
_02213270: .word 0x9CCF9319
	arm_func_end ov00_0221321C
