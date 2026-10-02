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
	.extern ov00_0220EE88
	.extern ov00_0220EEA8
	.extern ov00_0220EEDC
	.extern ov00_0220F7D4

.text
	arm_func_start ov00_0220EF24
ov00_0220EF24: ; 0x0220EF24
	stmdb sp!, {r4, r5, r6, r7, lr}
	sub sp, sp, #0x1c
	mov r4, r1
	mov r6, r3
	mov r5, r0
	mov r7, r2
	bl ov00_021FF3CC
	cmp r6, #0
	moveq r6, #0x10000
	cmp r7, #0
	add r1, sp, #0x10
	add r2, sp, #8
	mov r0, r4
	moveq r7, #0x10000
	bl ov00_0220F7D4
	cmp r0, #0
	addeq sp, sp, #0x1c
	moveq r0, #4
	ldmeqia sp!, {r4, r5, r6, r7, pc}
	mov r0, #0x44
	bl ov00_021EC3A8
	movs r4, r0
	addeq sp, sp, #0x1c
	moveq r0, #1
	ldmeqia sp!, {r4, r5, r6, r7, pc}
	mov r1, #0
	mov r2, #0x44
	bl memset
	mvn r0, #0
	str r0, [r4]
	str r6, [r4, #0x3c]
	ldr r1, [sp, #0x30]
	str r7, [r4, #0x38]
	str r1, [r4, #0x24]
	ldr r0, _0220F138 ; =ov00_0220EEA8
	ldr r3, _0220F13C ; =ov00_0220EE88
	str r0, [sp]
	mov ip, #0
	mov r0, #4
	mov r1, #0x20
	mov r2, #2
	str ip, [sp, #4]
	bl ov00_021FED4C
	str r0, [r4, #0xc]
	cmp r0, #0
	bne _0220EFF0
	mov r0, r4
	bl ov00_021EC3D8
	add sp, sp, #0x1c
	mov r0, #1
	ldmia sp!, {r4, r5, r6, r7, pc}
_0220EFF0:
	mov r0, #4
	ldr r2, _0220F140 ; =ov00_0220EEDC
	mov r1, r0
	bl ov00_021FE5B4
	str r0, [r4, #0x10]
	cmp r0, #0
	bne _0220F028
	ldr r0, [r4, #0xc]
	bl ov00_021FEE8C
	mov r0, r4
	bl ov00_021EC3D8
	add sp, sp, #0x1c
	mov r0, #1
	ldmia sp!, {r4, r5, r6, r7, pc}
_0220F028:
	mov r0, #2
	mov r1, r0
	mov r2, #0
	bl ov00_021FF804
	mvn r1, #0
	str r0, [r4]
	cmp r0, r1
	bne _0220F06C
	ldr r0, [r4, #0xc]
	bl ov00_021FEE8C
	ldr r0, [r4, #0x10]
	bl ov00_021FE678
	mov r0, r4
	bl ov00_021EC3D8
	add sp, sp, #0x1c
	mov r0, #3
	ldmia sp!, {r4, r5, r6, r7, pc}
_0220F06C:
	ldrh r0, [sp, #8]
	add r1, sp, #0x14
	mov ip, #0
	mov r2, r0, asr #8
	mov r0, r0, lsl #8
	and r2, r2, #0xff
	and r0, r0, #0xff00
	orr r0, r2, r0
	str ip, [r1]
	mov r2, #2
	strb r2, [sp, #0x15]
	ldr r3, [sp, #0x10]
	str ip, [r1, #4]
	str r3, [sp, #0x18]
	strh r0, [sp, #0x16]
	ldr r0, [r4]
	mov r2, #8
	bl ov00_021FF840
	mvn r1, #0
	cmp r0, r1
	bne _0220F0EC
	ldr r0, [r4]
	bl ov00_021FF818
	ldr r0, [r4, #0xc]
	bl ov00_021FEE8C
	ldr r0, [r4, #0x10]
	bl ov00_021FE678
	mov r0, r4
	bl ov00_021EC3D8
	add sp, sp, #0x1c
	mov r0, #3
	ldmia sp!, {r4, r5, r6, r7, pc}
_0220F0EC:
	mov r0, #8
	str r0, [sp, #0xc]
	ldr r0, [r4]
	add r1, sp, #0x14
	add r2, sp, #0xc
	bl ov00_021FFA18
	ldr r1, [sp, #0x18]
	mov r0, #0
	str r1, [r4, #4]
	ldrh r1, [sp, #0x16]
	mov r2, r1, asr #8
	mov r1, r1, lsl #8
	and r2, r2, #0xff
	and r1, r1, #0xff00
	orr r1, r2, r1
	strh r1, [r4, #8]
	str r4, [r5]
	add sp, sp, #0x1c
	ldmia sp!, {r4, r5, r6, r7, pc}
	.balign 4, 0
_0220F138: .word ov00_0220EEA8
_0220F13C: .word ov00_0220EE88
_0220F140: .word ov00_0220EEDC
	arm_func_end ov00_0220EF24
