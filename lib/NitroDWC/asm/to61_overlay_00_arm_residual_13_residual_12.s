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
	.public ov00_022128C4
	.extern ov00_02212724
	.extern ov00_02212824
	.extern ov00_022128BC

.text
	arm_func_start ov00_022128C4
ov00_022128C4: ; 0x022128C4
	stmdb sp!, {r4, r5, r6, r7, r8, sb, lr}
	sub sp, sp, #0x14
	sub sp, sp, #0x800
	movs r8, r1
	mov r1, #8
	mov sb, r0
	str r1, [sp, #8]
	ldrne r7, [sb, #0x24]
	ldreq r7, [sb, #0x20]
	mov r0, r7
	bl ov00_021FF694
	cmp r0, #0
	addeq sp, sp, #0x14
	addeq sp, sp, #0x800
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, pc}
_02212900:
	add r0, sp, #0xc
	str r0, [sp]
	add r4, sp, #8
	ldr r2, _02212A54 ; =0x000007FF
	add r1, sp, #0x14
	mov r0, r7
	mov r3, #0
	str r4, [sp, #4]
	bl ov00_021FF940
	mov r5, r0
	mvn r0, #0
	cmp r5, r0
	addeq sp, sp, #0x14
	addeq sp, sp, #0x800
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, pc}
	add r4, sp, #0x14
	mov r0, #0
	strb r0, [r4, r5]
	ldr r6, [sb, #8]
	cmp r6, #0
	beq _02212A38
_02212954:
	cmp r8, #0
	beq _02212978
	ldrb r0, [r6, #0x15]
	tst r0, #8
	beq _02212978
	ldr r1, [r6, #0x10]
	ldr r0, [sp, #0x10]
	cmp r1, r0
	beq _022129D0
_02212978:
	ldr r3, [sp, #0x10]
	ldr r2, [r6]
	cmp r2, r3
	bne _022129A0
	ldrh r1, [r6, #4]
	ldrh r0, [sp, #0xe]
	cmp r1, r0
	beq _022129D0
	cmp r8, #0
	bne _022129D0
_022129A0:
	ldr r0, [sb, #0x28]
	cmp r2, r0
	bne _02212A2C
	ldrb r0, [r6, #0x15]
	tst r0, #2
	beq _02212A2C
	ldr r0, [r6, #8]
	cmp r0, r3
	ldreqh r1, [r6, #0xc]
	ldreqh r0, [sp, #0xe]
	cmpeq r1, r0
	bne _02212A2C
_022129D0:
	cmp r8, #0
	beq _022129F8
	mov r0, sb
	mov r1, r6
	mov r2, r4
	mov r3, r5
	bl ov00_022128BC
	cmp r0, #0
	bne _02212A38
	b _02212A2C
_022129F8:
	ldr r0, [sb]
	add r2, sp, #0x14
	cmp r0, #1
	mov r0, sb
	bne _02212A1C
	mov r1, r6
	mov r3, r5
	bl ov00_02212724
	b _02212A38
_02212A1C:
	mov r1, r6
	mov r3, r5
	bl ov00_02212824
	b _02212A38
_02212A2C:
	ldr r6, [r6, #0x20]
	cmp r6, #0
	bne _02212954
_02212A38:
	mov r0, r7
	bl ov00_021FF694
	cmp r0, #0
	bne _02212900
	add sp, sp, #0x14
	add sp, sp, #0x800
	ldmia sp!, {r4, r5, r6, r7, r8, sb, pc}
	.balign 4, 0
_02212A54: .word 0x000007FF
	arm_func_end ov00_022128C4
