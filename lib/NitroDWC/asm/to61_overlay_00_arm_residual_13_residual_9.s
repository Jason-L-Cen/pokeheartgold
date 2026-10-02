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
	.public ov00_02211DFC
	.extern ov00_02210DC8
	.extern ov00_02210DFC
	.extern ov00_022111D8
	.extern ov00_022115B0

.text
	arm_func_start ov00_02211DFC
ov00_02211DFC: ; 0x02211DFC
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x28
	sub sp, sp, #0x800
	mov sl, r0
	mov sb, r1
	mov r3, #0
	add r0, sp, #0x24
	add r2, sl, #0x84
	mov r1, #3
	str r3, [sp, #0x824]
	bl ov00_022111D8
	ldr r4, _02211FDC ; =ov00_0221B878
	mov r8, #0
	ldr r0, [r4]
	cmp r0, #0
	ble _02211E8C
	ldr fp, _02211FE0 ; =ov00_02219F24
	ldr r5, _02211FE4 ; =ov00_0221B87C
	add r7, sp, #0x10
	add r6, sp, #0x24
_02211E4C:
	mov r0, r7
	mov r1, fp
	mov r2, r8
	bl sprintf
	mov r0, r6
	mov r1, r7
	bl ov00_02210DFC
	ldr r0, [r5, r8, lsl #2]
	bl SOC_InetNtoA
	mov r1, r0
	mov r0, r6
	bl ov00_02210DFC
	add r8, r8, #1
	ldr r0, [r4]
	cmp r8, r0
	blt _02211E4C
_02211E8C:
	ldr r1, _02211FE8 ; =ov00_02219F30
	add r0, sp, #0x24
	bl ov00_02210DFC
	ldr r1, [sl, #0xc0]
	add r0, sp, #0x24
	bl ov00_02210DC8
	ldr r1, _02211FEC ; =ov00_02219F3C
	add r0, sp, #0x24
	bl ov00_02210DFC
	ldr r0, [sl, #0xc8]
	cmp r0, #0
	ldrne r1, _02211FF0 ; =ov00_02219F44
	add r0, sp, #0x24
	ldreq r1, _02211FF4 ; =ov00_02219F48
	bl ov00_02210DFC
	cmp sb, #0
	beq _02211EE8
	ldr r1, _02211FF8 ; =ov00_02219F4C
	add r0, sp, #0x24
	bl ov00_02210DFC
	add r0, sp, #0x24
	mov r1, sb
	bl ov00_02210DC8
_02211EE8:
	ldr r1, _02211FFC ; =ov00_02219F5C
	add r0, sp, #0x24
	bl ov00_02210DFC
	add r0, sp, #0x24
	add r1, sl, #4
	bl ov00_02210DFC
	ldr r0, [sl, #0xa8]
	cmp r0, #0
	beq _02211F40
	ldr r1, _02212000 ; =ov00_02219F68
	add r0, sp, #0x24
	bl ov00_02210DFC
	ldr r1, [sl, #0x104]
	add r0, sp, #0x24
	bl ov00_02210DC8
	ldr r1, _02212004 ; =ov00_02219F74
	add r0, sp, #0x24
	bl ov00_02210DFC
	add r0, sl, #0x100
	ldrh r1, [r0, #8]
	add r0, sp, #0x24
	bl ov00_02210DC8
_02211F40:
	cmp sb, #2
	beq _02211F70
	mov r2, #0xff
	str r2, [sp]
	mov r3, #0
	str r3, [sp, #4]
	str r2, [sp, #8]
	add r1, sp, #0x24
	mov r0, sl
	str r3, [sp, #0xc]
	bl ov00_022115B0
	b _02211F94
_02211F70:
	ldr r2, [sp, #0x824]
	rsb r0, r2, #0x800
	cmp r0, #1
	blt _02211F94
	add r1, r2, #1
	str r1, [sp, #0x824]
	add r0, sp, #0x24
	mov r1, #0
	strb r1, [r0, r2]
_02211F94:
	add r0, sl, #0xcc
	str r0, [sp]
	mov r0, #8
	str r0, [sp, #4]
	ldr r0, [sl]
	ldr r2, [sp, #0x824]
	add r1, sp, #0x24
	mov r3, #0
	bl ov00_021FF984
	bl ov00_021FF368
	str r0, [sl, #0xac]
	str r0, [sl, #0xb0]
	cmp sb, #0
	movne r0, #0
	strne r0, [sl, #0xb4]
	add sp, sp, #0x28
	add sp, sp, #0x800
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.balign 4, 0
_02211FDC: .word ov00_0221B878
_02211FE0: .word ov00_02219F24
_02211FE4: .word ov00_0221B87C
_02211FE8: .word ov00_02219F30
_02211FEC: .word ov00_02219F3C
_02211FF0: .word ov00_02219F44
_02211FF4: .word ov00_02219F48
_02211FF8: .word ov00_02219F4C
_02211FFC: .word ov00_02219F5C
_02212000: .word ov00_02219F68
_02212004: .word ov00_02219F74
	arm_func_end ov00_02211DFC

	arm_func_start ov00_02212008
ov00_02212008: ; 0x02212008
	cmp r0, #0x32
	bxlt lr
	cmp r0, #0xfe
	bxgt lr
	ldr r2, _02212024 ; =ov00_0221A0B8
	str r1, [r2, r0, lsl #2]
	bx lr
	.balign 4, 0
_02212024: .word ov00_0221A0B8
	arm_func_end ov00_02212008
