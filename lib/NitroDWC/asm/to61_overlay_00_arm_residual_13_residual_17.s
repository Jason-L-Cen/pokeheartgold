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
	.public ov00_02213708
	.extern ov00_02212B88
	.extern ov00_02213938
	.extern ov00_022145F0

.text
	arm_func_start ov00_02213708
ov00_02213708: ; 0x02213708
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x108
	ldr r8, [sp, #0x130]
	mov sl, r0
	str r1, [sp, #4]
	add r4, sp, #8
	mov sb, r3
	mov r1, #0x40
	mov r0, #0
_0221372C:
	strb r0, [r4]
	strb r0, [r4, #1]
	strb r0, [r4, #2]
	strb r0, [r4, #3]
	add r4, r4, #4
	subs r1, r1, #1
	bne _0221372C
	mov r5, #0
	str r2, [sl, #0x620]
	mov r6, r5
	str r5, [sl, #0x40]
	cmp r8, #0
	ble _022137B4
	ldr r4, _02213830 ; =ov00_0221A0B8
	add fp, sp, #8
_02213768:
	ldrb r0, [sb, r6]
	ldr r7, [r4, r0, lsl #2]
	mov r0, r7
	bl strlen
	add r0, r5, r0
	add r0, r0, #1
	cmp r0, #0x100
	bge _022137B4
	ldr r1, _02213834 ; =ov00_0221A528
	mov r2, r7
	add r0, fp, r5
	bl sprintf
	add r5, r5, r0
	ldrb r1, [sb, r6]
	mov r0, sl
	bl ov00_02212B88
	add r6, r6, #1
	cmp r6, r8
	blt _02213768
_022137B4:
	ldr r4, [sp, #0x13c]
	ldr r2, [sp, #0x134]
	ldr r3, [sp, #0x138]
	add r1, sp, #8
	add r0, sl, #0x4c
	str r4, [sp]
	bl ov00_022145F0
	cmp r0, #0
	addne sp, sp, #0x108
	ldmneia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldr r1, [sp, #4]
	cmp r1, #0
	addne sp, sp, #0x108
	ldmneia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	mov r4, #0xa
	b _02213804
_022137F4:
	mov r0, r4
	bl ov00_021FF3C0
	mov r0, sl
	bl ov00_02213938
_02213804:
	ldr r1, [sl, #0x4c]
	cmp r1, #3
	beq _022137F4
	ldr r1, [sl, #0x10]
	cmp r1, #0
	addle sp, sp, #0x108
	ldmleia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	cmp r0, #0
	beq _022137F4
	add sp, sp, #0x108
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.balign 4, 0
_02213830: .word ov00_0221A0B8
_02213834: .word ov00_0221A528
	arm_func_end ov00_02213708
