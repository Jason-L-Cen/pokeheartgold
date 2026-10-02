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
	.public ov00_02212724
	.extern ov00_02212384
	.extern ov00_02212CD0
	.extern ov00_02213044
	.extern ov00_02213FD4

.text
	arm_func_start ov00_02212724
ov00_02212724: ; 0x02212724
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, lr}
	mov r8, r2
	ldrsb r2, [r8]
	mov sl, r0
	mov sb, r1
	mov r7, r3
	cmp r2, #0
	ldmneia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
	ldrb r0, [sb, #0x14]
	add r8, r8, #5
	sub r7, r7, #5
	tst r0, #4
	beq _022127C0
	ldr r0, [sl, #0x40]
	mov r6, #0
	cmp r0, #0
	ble _022127B0
	ldr r4, _02212820 ; =ov00_0221A0B8
_0221276C:
	mov r0, r8
	mov r1, r7
	bl ov00_02213FD4
	movs r5, r0
	bmi _022127B0
	add r0, sl, r6
	ldrb r1, [r0, #0x2c]
	mov r0, sb
	mov r2, r8
	ldr r1, [r4, r1, lsl #2]
	bl ov00_02212CD0
	ldr r0, [sl, #0x40]
	add r6, r6, #1
	cmp r6, r0
	add r8, r8, r5
	sub r7, r7, r5
	blt _0221276C
_022127B0:
	ldrb r0, [sb, #0x14]
	orr r0, r0, #0x41
	strb r0, [sb, #0x14]
	b _022127DC
_022127C0:
	mov r0, sb
	mov r1, r8
	mov r2, r7
	bl ov00_02213044
	ldrb r0, [sb, #0x14]
	orr r0, r0, #0x43
	strb r0, [sb, #0x14]
_022127DC:
	ldrb r0, [sb, #0x14]
	and r0, r0, #0xf3
	strb r0, [sb, #0x14]
	bl ov00_021FF368
	ldr r2, [sb, #0x1c]
	mov r1, sb
	sub r0, r0, r2
	str r0, [sb, #0x1c]
	add r0, sl, #8
	bl ov00_02212384
	ldr r3, [sl, #0x48]
	ldr r4, [sl, #0x44]
	mov r0, sl
	mov r2, sb
	mov r1, #0
	blx r4
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
	.balign 4, 0
_02212820: .word ov00_0221A0B8
	arm_func_end ov00_02212724
