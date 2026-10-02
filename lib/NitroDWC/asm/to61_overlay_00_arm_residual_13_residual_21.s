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
	.public ov00_02214004
	.public ov00_02214130
	.public ov00_02214244
	.extern ov00_02212C08
	.extern ov00_02213EC0
	.extern ov00_02214908
	.extern ov00_0221505C

.text
	arm_func_start ov00_02214004
ov00_02214004: ; 0x02214004
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	movs r7, r0
	mov r6, r1
	mov r5, r2
	mov r4, r3
	bne _02214030
	ldr r0, _02214114 ; =ov00_0221A5D8
	ldr r1, _02214118 ; =ov00_0221A5C8
	ldr r2, _0221411C ; =ov00_0221A560
	mov r3, #0x11c
	bl __msl_assertion_failed
_02214030:
	ldr r0, [sp, #0x1c]
	cmp r0, #0
	bne _0221404C
	ldr r0, _02214120 ; =ov00_0221B0D0
	ldr r0, [r0]
	cmp r0, #1
	ldmneia sp!, {r3, r4, r5, r6, r7, pc}
_0221404C:
	mov r1, #1
	mov r0, r7
	str r1, [r7]
	bl ov00_02213EC0
	mov r0, r7
	bl ov00_02212C08
	mov r1, r6
	add r0, r7, #0xc
	bl strcpy
	mov r1, r5
	add r0, r7, #0x30
	bl strcpy
	mov r1, r4
	add r0, r7, #0x54
	bl strcpy
	ldr r1, [sp, #0x20]
	mov r0, #0
	str r1, [r7, #0x488]
	str r0, [r7, #0x48c]
	cmp r1, #0
	bne _022140B4
	ldr r0, _02214124 ; =ov00_0221A5E8
	ldr r1, _02214118 ; =ov00_0221A5C8
	ldr r2, _0221411C ; =ov00_0221A560
	ldr r3, _02214128 ; =0x00000132
	bl __msl_assertion_failed
_022140B4:
	ldr r0, [sp, #0x24]
	ldr r1, _0221412C ; =ov00_0221A5B8
	str r0, [r7, #0x494]
	mov r3, #0
	str r1, [r7, #0x498]
	str r3, [r7, #0x4a0]
	sub r0, r3, #1
	str r0, [r7, #0x4b0]
	str r3, [r7, #0x7c]
	str r3, [r7, #0x80]
	str r3, [r7, #8]
	str r0, [r7, #0x484]
	str r3, [r7, #0x480]
	ldr r2, [sp, #0x18]
	str r3, [r7, #0x4a4]
	mov r0, r7
	str r2, [r7, #0x4b8]
	bl ov00_0221505C
	mov r0, #0
	str r0, [r7, #0x5cc]
	bl ov00_021FF368
	bl srand
	bl ov00_021FF3CC
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.balign 4, 0
_02214114: .word ov00_0221A5D8
_02214118: .word ov00_0221A5C8
_0221411C: .word ov00_0221A560
_02214120: .word ov00_0221B0D0
_02214124: .word ov00_0221A5E8
_02214128: .word 0x00000132
_0221412C: .word ov00_0221A5B8
	arm_func_end ov00_02214004

	arm_func_start ov00_02214130
ov00_02214130: ; 0x02214130
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	mov r4, r0
	ldr r5, [r4, #0x80]
	cmp r5, #0
	ble _022141BC
	ldr r0, _022141E4 ; =ov00_0221A52C
	ldr r6, [r0]
	mov r0, r6
	bl strlen
	cmp r5, r0
	bls _022141BC
	ldr r0, _022141E4 ; =ov00_0221A52C
	ldr r5, [r4, #0x7c]
	ldr r7, [r0]
	mov r0, r7
	bl strlen
	mov r2, r0
	mov r0, r5
	mov r1, r7
	bl strncmp
	cmp r0, #0
	bne _022141BC
	mov r0, r6
	bl strlen
	mov r1, r0
	mov r0, r4
	add r1, r5, r1
	bl ov00_0221505C
	ldr r0, _022141E8 ; =ov00_0221B9D4
	ldr r3, [r4, #0x494]
	ldr r2, [r0]
	ldr ip, [r4, #0x488]
	mov r0, r4
	mov r1, #5
	blx ip
_022141BC:
	ldr r0, _022141E8 ; =ov00_0221B9D4
	ldr r3, [r4, #0x494]
	ldr r2, [r0]
	ldr ip, [r4, #0x488]
	mov r0, r4
	mov r1, #4
	blx ip
	mov r0, r4
	bl ov00_02214908
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.balign 4, 0
_022141E4: .word ov00_0221A52C
_022141E8: .word ov00_0221B9D4
	arm_func_end ov00_02214130

	arm_func_start ov00_022141EC
ov00_022141EC: ; 0x022141EC
	stmdb sp!, {r3, lr}
	ldrsb lr, [r0]
	mov ip, #0
	cmp lr, #0
	beq _0221422C
	ldr r3, _0221423C ; =__lower_mapC
	ldr r2, _02214240 ; =0x9CCF9319
_02214208:
	cmp lr, #0
	blt _0221421C
	cmp lr, #0x80
	bge _0221421C
	ldrb lr, [r3, lr]
_0221421C:
	mla ip, r2, ip, lr
	ldrsb lr, [r0, #1]!
	cmp lr, #0
	bne _02214208
_0221422C:
	mov r0, ip
	bl _u32_div_f
	mov r0, r1
	ldmia sp!, {r3, pc}
	.balign 4, 0
_0221423C: .word __lower_mapC
_02214240: .word 0x9CCF9319
	arm_func_end ov00_022141EC

	arm_func_start ov00_02214244
ov00_02214244: ; 0x02214244
	stmdb sp!, {r4, lr}
	sub sp, sp, #0x88
	mov r4, r0
	add r0, r4, #0xc
	mov r1, #0x14
	bl ov00_022141EC
	ldr r1, _02214364 ; =ov00_0221B9DC
	mov r3, r0
	ldr r1, [r1, #4]
	add r0, sp, #8
	cmp r1, #0
	beq _0221427C
	bl strcpy
	b _02214288
_0221427C:
	ldr r1, _02214368 ; =ov00_0221A5FC
	add r2, r4, #0xc
	bl sprintf
_02214288:
	ldr r1, _0221436C ; =0x0000EE70
	mov r2, #2
	add r0, sp, #8
	strb r2, [sp, #1]
	strh r1, [sp, #2]
	bl ov00_021FFA44
	mvn r1, #0
	str r0, [sp, #4]
	cmp r0, r1
	bne _022142F4
	add r0, sp, #8
	bl SOC_GetHostByName
	cmp r0, #0
	addeq sp, sp, #0x88
	moveq r0, #2
	ldmeqia sp!, {r4, pc}
	ldr r0, [r0, #0xc]
	add r3, sp, #4
	ldr r2, [r0]
	ldrb r1, [r2]
	ldrb r0, [r2, #1]
	strb r1, [r3]
	strb r0, [r3, #1]
	ldrb r1, [r2, #2]
	ldrb r0, [r2, #3]
	strb r1, [r3, #2]
	strb r0, [r3, #3]
_022142F4:
	ldr r0, [r4, #0x4b0]
	mvn r1, #0
	cmp r0, r1
	bne _0221432C
	mov r0, #2
	mov r1, #1
	mov r2, #0
	bl ov00_021FF804
	mvn r1, #0
	cmp r0, r1
	str r0, [r4, #0x4b0]
	addeq sp, sp, #0x88
	moveq r0, #1
	ldmeqia sp!, {r4, pc}
_0221432C:
	add r1, sp, #0
	mov r2, #8
	bl ov00_021FF8A0
	cmp r0, #0
	addeq sp, sp, #0x88
	moveq r0, #0
	ldmeqia sp!, {r4, pc}
	ldr r0, [r4, #0x4b0]
	bl ov00_021FF818
	mvn r0, #0
	str r0, [r4, #0x4b0]
	mov r0, #3
	add sp, sp, #0x88
	ldmia sp!, {r4, pc}
	.balign 4, 0
_02214364: .word ov00_0221B9DC
_02214368: .word ov00_0221A5FC
_0221436C: .word 0x0000EE70
	arm_func_end ov00_02214244
