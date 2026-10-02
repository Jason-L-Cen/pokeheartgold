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
	.public ov00_02210FD0
	.public ov00_02211094
	.extern ov00_02210F74
	.extern ov00_02210F88

.text
	arm_func_start ov00_02210FD0
ov00_02210FD0: ; 0x02210FD0
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, lr}
	sub sp, sp, #8
	mov r7, r1
	mov r8, r0
	mov r6, r2
	cmp r7, #0
	mov r4, #0
	ble _02211084
_02210FF0:
	mov r2, #0
	add r3, sp, #4
	mov r0, r2
_02210FFC:
	cmp r4, r7
	ldrltb r1, [r8], #1
	add r2, r2, #1
	add r4, r4, #1
	strltb r1, [r3]
	strgeb r0, [r3]
	cmp r2, #2
	add r3, r3, #1
	ble _02210FFC
	ldrb r0, [sp, #4]
	ldrb r5, [sp, #5]
	ldrb r2, [sp, #6]
	mov r3, r0, lsl #0x1e
	mov r1, r5, asr #4
	add ip, r1, r3, lsr #26
	and r1, r2, #0x3f
	mov r3, r5, lsl #0x1c
	mov r2, r2, asr #6
	mov r0, r0, asr #2
	add r2, r2, r3, lsr #26
	add r5, sp, #0
	strb r0, [sp]
	strb ip, [sp, #1]
	strb r2, [sp, #2]
	strb r1, [sp, #3]
	mov sb, #0
_02211064:
	ldrb r0, [r5], #1
	bl ov00_02210F88
	add sb, sb, #1
	cmp sb, #3
	strb r0, [r6], #1
	ble _02211064
	cmp r4, r7
	blt _02210FF0
_02211084:
	mov r0, #0
	strb r0, [r6]
	add sp, sp, #8
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	arm_func_end ov00_02210FD0

	arm_func_start ov00_02211094
ov00_02211094: ; 0x02211094
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x104
	str r1, [sp]
	add r4, sp, #4
	mov sl, r0
	mov sb, r2
	mov r8, r3
	mov r1, #0
_022110B4:
	add r0, r1, #1
	mov r0, r0, lsl #0x10
	strb r1, [r4], #1
	mov r1, r0, asr #0x10
	cmp r1, #0x100
	blt _022110B4
	mov r4, #0
	add r6, sp, #4
	mov r5, r4
	mov r7, r4
	mov fp, r6
_022110E0:
	ldrb r3, [r6]
	ldrb r2, [sl, r4]
	ldr r1, [sp]
	add r0, r4, #1
	add r2, r3, r2
	add r2, r5, r2
	mov r3, r2, lsr #0x1f
	rsb r2, r3, r2, lsl #24
	add r2, r3, r2, ror #24
	and r5, r2, #0xff
	bl _s32_div_f
	and r4, r1, #0xff
	mov r0, r6
	add r1, fp, r5
	bl ov00_02210F74
	add r0, r7, #1
	mov r0, r0, lsl #0x10
	mov r7, r0, asr #0x10
	cmp r7, #0x100
	add r6, r6, #1
	blt _022110E0
	mov r6, #0
	cmp r8, #0
	mov r7, r6
	mov r5, r6
	addle sp, sp, #0x104
	ldmleia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	add r4, sp, #4
_02211150:
	ldrb r0, [sb, r5]
	add r0, r6, r0
	add r0, r0, #1
	mov r1, r0, lsr #0x1f
	rsb r0, r1, r0, lsl #24
	add r0, r1, r0, ror #24
	and r6, r0, #0xff
	ldrb r1, [r4, r6]
	add r0, r4, r6
	add r1, r1, r7
	mov r2, r1, lsr #0x1f
	rsb r1, r2, r1, lsl #24
	add r1, r2, r1, ror #24
	and r7, r1, #0xff
	add r1, r4, r7
	bl ov00_02210F74
	ldrb r3, [r4, r6]
	ldrb r1, [r4, r7]
	add r0, r5, #1
	mov r2, r0, lsl #0x10
	add r0, r3, r1
	mov r1, r0, lsr #0x1f
	rsb r0, r1, r0, lsl #24
	add r0, r1, r0, ror #24
	and r0, r0, #0xff
	ldrb r1, [sb, r5]
	ldrb r0, [r4, r0]
	cmp r8, r2, asr #16
	eor r0, r1, r0
	strb r0, [sb, r5]
	mov r5, r2, asr #0x10
	bgt _02211150
	add sp, sp, #0x104
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	arm_func_end ov00_02211094
