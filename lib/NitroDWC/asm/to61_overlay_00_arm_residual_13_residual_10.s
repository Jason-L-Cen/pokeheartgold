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
	.public ov00_02212404
	.extern ov00_022122F4

.text
	arm_func_start ov00_02212404
ov00_02212404: ; 0x02212404
	stmdb sp!, {r3, r4, r5, lr}
	sub sp, sp, #0x110
	mov r5, r0
	mov r4, r1
	add r0, r5, #8
	bl ov00_022122F4
	bl ov00_021FF368
	str r0, [r4, #0x1c]
	mov r0, #2
	strb r0, [sp, #9]
	ldrb r1, [r4, #0x14]
	tst r1, #0x20
	addne sp, sp, #0x110
	ldmneia sp!, {r3, r4, r5, pc}
	ldr r0, [r5]
	cmp r0, #1
	bne _02212508
	mov r2, #0xfe
	mov r1, #0xfd
	mov r0, #0
	strb r2, [sp, #0x10]
	strb r1, [sp, #0x11]
	strb r0, [sp, #0x12]
	ldrb r2, [r4, #0x1c]
	ldrb r1, [r4, #0x1d]
	add r3, sp, #0x13
	strb r2, [r3]
	strb r1, [r3, #1]
	ldrb r2, [r4, #0x1e]
	ldrb r1, [r4, #0x1f]
	strb r2, [r3, #2]
	strb r1, [r3, #3]
	ldrb r1, [r4, #0x14]
	tst r1, #4
	beq _022124F0
	ldr r1, [r5, #0x40]
	strb r1, [sp, #0x17]
	ldr r3, [r5, #0x40]
	cmp r3, #0
	ble _022124C8
	add r2, sp, #0x10
_022124A8:
	add r1, r5, r0
	ldrb r1, [r1, #0x2c]
	add r0, r0, #1
	strb r1, [r2, #8]
	ldr r3, [r5, #0x40]
	add r2, r2, #1
	cmp r0, r3
	blt _022124A8
_022124C8:
	add r1, sp, #0x10
	add r0, r3, #8
	mov r2, #0
	strb r2, [r1, r0]
	ldr r0, [r5, #0x40]
	add r0, r0, #9
	strb r2, [r1, r0]
	ldr r0, [r5, #0x40]
	add r2, r0, #0xa
	b _02212578
_022124F0:
	mov r0, #0xff
	strb r0, [sp, #0x17]
	strb r0, [sp, #0x18]
	strb r0, [sp, #0x19]
	mov r2, #0xa
	b _02212578
_02212508:
	tst r1, #4
	add ip, sp, #0x10
	beq _0221254C
	ldr r3, _022125DC ; =ov00_0221A4B0
	mov r2, #6
_0221251C:
	ldrb r1, [r3]
	ldrb r0, [r3, #1]
	add r3, r3, #2
	strb r1, [ip]
	strb r0, [ip, #1]
	add ip, ip, #2
	subs r2, r2, #1
	bne _0221251C
	ldrb r0, [r3]
	mov r2, #0xd
	strb r0, [ip]
	b _02212578
_0221254C:
	ldr r3, _022125E0 ; =ov00_0221A4C0
	mov r2, #4
_02212554:
	ldrb r1, [r3]
	ldrb r0, [r3, #1]
	add r3, r3, #2
	strb r1, [ip]
	strb r0, [ip, #1]
	add ip, ip, #2
	subs r2, r2, #1
	bne _02212554
	mov r2, #8
_02212578:
	ldr r1, [r4]
	ldr r0, [r5, #0x28]
	cmp r1, r0
	bne _022125A8
	ldrb r0, [r4, #0x15]
	tst r0, #2
	beq _022125A8
	ldr r0, [r4, #8]
	str r0, [sp, #0xc]
	ldrh r0, [r4, #0xc]
	strh r0, [sp, #0xa]
	b _022125B4
_022125A8:
	str r1, [sp, #0xc]
	ldrh r0, [r4, #4]
	strh r0, [sp, #0xa]
_022125B4:
	add r1, sp, #8
	str r1, [sp]
	mov r0, #8
	str r0, [sp, #4]
	ldr r0, [r5, #0x20]
	add r1, sp, #0x10
	mov r3, #0
	bl ov00_021FF984
	add sp, sp, #0x110
	ldmia sp!, {r3, r4, r5, pc}
	.balign 4, 0
_022125DC: .word ov00_0221A4B0
_022125E0: .word ov00_0221A4C0
	arm_func_end ov00_02212404
