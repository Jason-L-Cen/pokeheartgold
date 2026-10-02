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
	.extern ov00_02210E58
	.extern ov00_02210ECC
	.include "asm/macros.inc"
	.include "overlay_00_arm.inc"
	.include "global.inc"
	.public ov00_0220F7D4

.text
	arm_func_start ov00_0220F724
ov00_0220F724: ; 0x0220F724
	stmdb sp!, {r3, r4, r5, lr}
	movs r4, r2
	mov r5, r1
	bne _0220F750
	ldr r2, _0220F7C0 ; =ov00_0221B634
	ldr r3, _0220F7C4 ; =ov00_0221B638
	ldr ip, [r2]
	mov r1, #0x16
	eor ip, ip, #1
	mla r4, ip, r1, r3
	str ip, [r2]
_0220F750:
	cmp r0, #0
	beq _0220F798
	str r0, [sp]
	cmp r5, #0
	beq _0220F780
	bl SOC_InetNtoA
	mov r2, r0
	ldr r1, _0220F7C8 ; =ov00_02219CE4
	mov r0, r4
	mov r3, r5
	bl sprintf
	b _0220F7B8
_0220F780:
	bl SOC_InetNtoA
	mov r2, r0
	ldr r1, _0220F7CC ; =ov00_02219CEC
	mov r0, r4
	bl sprintf
	b _0220F7B8
_0220F798:
	cmp r5, #0
	moveq r0, #0
	streqb r0, [r4]
	beq _0220F7B8
	ldr r1, _0220F7D0 ; =ov00_02219CF0
	mov r0, r4
	mov r2, r5
	bl sprintf
_0220F7B8:
	mov r0, r4
	ldmia sp!, {r3, r4, r5, pc}
	.balign 4, 0
_0220F7C0: .word ov00_0221B634
_0220F7C4: .word ov00_0221B638
_0220F7C8: .word ov00_02219CE4
_0220F7CC: .word ov00_02219CEC
_0220F7D0: .word ov00_02219CF0
	arm_func_end ov00_0220F724

	arm_func_start ov00_0220F7D4
ov00_0220F7D4: ; 0x0220F7D4
	stmdb sp!, {r4, r5, r6, r7, r8, sb, lr}
	sub sp, sp, #0x104
	movs r8, r0
	mov r7, r1
	ldrnesb r1, [r8]
	mov r6, r2
	cmpne r1, #0
	moveq r4, #0
	moveq r5, r4
	beq _0220F928
	mov r1, #0x3a
	bl strchr
	movs r5, r0
	moveq r5, #0
	beq _0220F8E4
	cmp r5, r8
	moveq r8, #0
	moveq r4, r8
	beq _0220F85C
	sub sb, r5, r8
	cmp sb, #0x100
	blt _0220F840
	ldr r0, _0220F944 ; =ov00_02219CF4
	ldr r1, _0220F948 ; =ov00_02219D14
	ldr r2, _0220F94C ; =ov00_02219CD0
	mov r3, #0x81
	bl __msl_assertion_failed
_0220F840:
	add r0, sp, #0
	mov r1, r8
	mov r2, sb
	bl memcpy
	add r8, sp, #0
	mov r0, #0
	strb r0, [r8, sb]
_0220F85C:
	ldrsb r0, [r5, #1]
	add r3, r5, #1
	cmp r0, #0
	beq _0220F8B4
	ldr r1, _0220F950 ; =__msl_digit
	mov r2, #0
_0220F874:
	cmp r0, #0
	blt _0220F884
	cmp r0, #0x80
	blt _0220F88C
_0220F884:
	mov r0, r2
	b _0220F898
_0220F88C:
	mov r0, r0, lsl #1
	ldrh r0, [r1, r0]
	and r0, r0, #8
_0220F898:
	cmp r0, #0
	addeq sp, sp, #0x104
	moveq r0, #0
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, pc}
	ldrsb r0, [r3, #1]!
	cmp r0, #0
	bne _0220F874
_0220F8B4:
	add r0, r5, #1
	bl atol
	cmp r0, #0
	blt _0220F8D0
	ldr r1, _0220F954 ; =0x0000FFFF
	cmp r0, r1
	ble _0220F8DC
_0220F8D0:
	add sp, sp, #0x104
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, sb, pc}
_0220F8DC:
	mov r0, r0, lsl #0x10
	mov r5, r0, lsr #0x10
_0220F8E4:
	cmp r8, #0
	beq _0220F928
	mov r0, r8
	bl ov00_021FFA44
	mov r4, r0
	mvn r0, #0
	cmp r4, r0
	bne _0220F928
	mov r0, r8
	bl SOC_GetHostByName
	cmp r0, #0
	addeq sp, sp, #0x104
	moveq r0, #0
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, pc}
	ldr r0, [r0, #0xc]
	ldr r0, [r0]
	ldr r4, [r0]
_0220F928:
	cmp r7, #0
	strne r4, [r7]
	cmp r6, #0
	strneh r5, [r6]
	mov r0, #1
	add sp, sp, #0x104
	ldmia sp!, {r4, r5, r6, r7, r8, sb, pc}
	.balign 4, 0
_0220F944: .word ov00_02219CF4
_0220F948: .word ov00_02219D14
_0220F94C: .word ov00_02219CD0
_0220F950: .word __msl_digit
_0220F954: .word 0x0000FFFF
	arm_func_end ov00_0220F7D4
