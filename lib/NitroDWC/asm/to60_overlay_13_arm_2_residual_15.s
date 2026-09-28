	.include "asm/macros.inc"
	.include "overlay_13_arm_2.inc"
	.include "global.inc"
	.public ov13_0223D448
	.public ov13_0223D45C
	.public ov13_0223D4C8
	.public ov13_0223D66C
	.public ov13_0223E80C
	.public ov13_0223E8FC
	.public ov13_0223EA58
	.public ov13_0223EAA0
	.public ov13_0223EAD4
	.public ov13_0223EE58
	.public ov13_0223EE9C
	.public ov13_0223F664
	.extern ov13_0223A7F4
	.extern ov13_0223C9D4
	.extern ov13_0223CA14
	.extern ov13_0223CB34
	.extern ov13_0223CEA0
	.extern ov13_0223CF80
	.extern ov13_0223CFE8
	.extern ov13_0223D0E0
	.extern ov13_0223F8A0
	.extern ov13_0223F904
	.extern ov13_0223F954
	.extern ov13_0224022C
	.extern ov13_0224128C
	.extern ov13_0224593C
	.extern ov13_022468D8
	.extern ov13_02247120
	.extern ov13_02247200
	.extern ov13_02247208
	.extern ov13_0224720C
	.extern ov13_02247238
	.extern ov13_02247264
	.extern ov13_02247284
	.extern ov13_022472B4
	.extern ov13_022472DC
	.extern ov13_022472F4
	.extern ov13_0224730C
	.extern ov13_02247324
	.extern ov13_0224733C
	.extern ov13_02247358
	.extern ov13_02247388
	.extern ov13_022473BC
	.extern ov13_022473E0
	.extern ov13_02247404
	.extern ov13_0224741C
	.extern ov13_0224F5BC
	.extern ov13_0224F5C0
	.extern ov13_0224F5C8
	.extern ov13_0224F5CC

.text
	arm_func_start ov13_0223D448
ov13_0223D448: ; 0x0223D448
	ldr r1, _0223D458 ; =ov13_0224F5BC
	ldr r1, [r1]
	strh r0, [r1]
	bx lr
	.balign 4, 0
_0223D458: .word ov13_0224F5BC
	arm_func_end ov13_0223D448

	arm_func_start ov13_0223D45C
ov13_0223D45C: ; 0x0223D45C
	ldr r0, _0223D46C ; =ov13_0224F5BC
	ldr r0, [r0]
	ldrh r0, [r0]
	bx lr
	.balign 4, 0
_0223D46C: .word ov13_0224F5BC
	arm_func_end ov13_0223D45C

	arm_func_start ov13_0223D470
ov13_0223D470: ; 0x0223D470
	ldr r2, _0223D4BC ; =ov13_0224F5BC
	ldr r1, _0223D4C0 ; =ov13_022468D8
	ldr ip, [r2]
	ldr r2, _0223D4C4 ; =ov13_022468D8
	add r3, ip, #2
	str r3, [r1]
	add r3, ip, #4
	str r3, [r1, #4]
	add r3, ip, #6
	str r3, [r1, #8]
	add r3, ip, #8
	str r3, [r1, #0xc]
	add r3, ip, #0xa
	str r3, [r1, #0x10]
	add r3, ip, #0xc
	str r3, [r1, #0x14]
	ldr r0, [r2, r0, lsl #2]
	ldrh r0, [r0]
	bx lr
	.balign 4, 0
_0223D4BC: .word ov13_0224F5BC
_0223D4C0: .word ov13_022468D8
_0223D4C4: .word ov13_022468D8
	arm_func_end ov13_0223D470

	arm_func_start ov13_0223D4C8
ov13_0223D4C8: ; 0x0223D4C8
	stmdb sp!, {r3, r4, r5, lr}
	sub sp, sp, #0x10
	mov r4, r0
	bl OS_DisableInterrupts
	ldr r1, _0223D58C ; =ov13_0224F5BC
	mov r3, #1
	ldr r2, [r1]
	mov r3, r3, lsl r4
	ldrh r1, [r2, #2]
	mov r4, r3, lsl #0x10
	mov r5, r0
	tst r1, r4, lsr #16
	bne _0223D50C
	bl OS_RestoreInterrupts
	add sp, sp, #0x10
	mov r0, #0
	ldmia sp!, {r3, r4, r5, pc}
_0223D50C:
	mov r0, r2
	add r1, sp, #0
	mov r2, #0xe
	bl MI_CpuCopy8
	mov r0, r5
	bl OS_RestoreInterrupts
	ldrh r0, [sp, #4]
	tst r0, r4, lsr #16
	addne sp, sp, #0x10
	movne r0, #2
	ldmneia sp!, {r3, r4, r5, pc}
	ldrh r0, [sp, #6]
	tst r0, r4, lsr #16
	addne sp, sp, #0x10
	movne r0, #3
	ldmneia sp!, {r3, r4, r5, pc}
	ldrh r0, [sp, #8]
	tst r0, r4, lsr #16
	addne sp, sp, #0x10
	movne r0, #4
	ldmneia sp!, {r3, r4, r5, pc}
	ldrh r0, [sp, #0xa]
	tst r0, r4, lsr #16
	addne sp, sp, #0x10
	movne r0, #5
	ldmneia sp!, {r3, r4, r5, pc}
	ldrh r0, [sp, #0xc]
	tst r0, r4, lsr #16
	movne r0, #6
	moveq r0, #1
	add sp, sp, #0x10
	ldmia sp!, {r3, r4, r5, pc}
	.balign 4, 0
_0223D58C: .word ov13_0224F5BC
	arm_func_end ov13_0223D4C8

	arm_func_start ov13_0223D590
ov13_0223D590: ; 0x0223D590
	ldr r1, _0223D5C0 ; =ov13_0224F5BC
	mov r2, #1
	ldr r3, [r1]
	ldrh r1, [r3, #2]
	tst r1, r2, lsl r0
	moveq r0, #0
	bxeq lr
	sub r1, r0, #1
	add r2, r3, #0xe
	mov r0, #0x1e
	mla r0, r1, r0, r2
	bx lr
	.balign 4, 0
_0223D5C0: .word ov13_0224F5BC
	arm_func_end ov13_0223D590

	arm_func_start ov13_0223D5C4
ov13_0223D5C4: ; 0x0223D5C4
	stmdb sp!, {r4, r5, r6, r7, r8, lr}
	ldr r1, _0223D668 ; =ov13_0224F5BC
	mov lr, #1
	ldr r5, [r1]
	mov ip, lr
	ldrh r4, [r5, #2]
	add r1, r5, #0x24
	mov r6, #0x1e
_0223D5E4:
	tst r4, ip, lsl lr
	beq _0223D64C
	sub r2, lr, #1
	mul r3, r2, r6
	ldrb r8, [r0]
	ldrb r7, [r1, r3]
	add r2, r1, r3
	cmp r8, r7
	ldreqb r8, [r0, #1]
	ldreqb r7, [r2, #1]
	cmpeq r8, r7
	ldreqb r8, [r0, #2]
	ldreqb r7, [r2, #2]
	cmpeq r8, r7
	ldreqb r8, [r0, #3]
	ldreqb r7, [r2, #3]
	cmpeq r8, r7
	ldreqb r8, [r0, #4]
	ldreqb r7, [r2, #4]
	cmpeq r8, r7
	ldreqb r7, [r0, #5]
	ldreqb r2, [r2, #5]
	cmpeq r7, r2
	addeq r0, r5, r3
	ldreqh r0, [r0, #0x2a]
	ldmeqia sp!, {r4, r5, r6, r7, r8, pc}
_0223D64C:
	add r2, lr, #1
	mov r2, r2, lsl #0x10
	mov lr, r2, lsr #0x10
	cmp lr, #2
	blo _0223D5E4
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	.balign 4, 0
_0223D668: .word ov13_0224F5BC
	arm_func_end ov13_0223D5C4

	arm_func_start ov13_0223D66C
ov13_0223D66C: ; 0x0223D66C
	ldr r1, _0223D6A4 ; =ov13_0224F5C0
	mov r2, #0
	str r0, [r1, #4]
	str r2, [r0, #0x40]
	ldr r0, [r1, #4]
	add r0, r0, #0x1000
	str r2, [r0, #0x3a8]
	ldr r0, [r1, #4]
	add r0, r0, #0x1000
	str r2, [r0, #0x3ac]
	ldr r0, [r1, #4]
	add r0, r0, #0x1000
	str r2, [r0, #0x3b0]
	bx lr
	.balign 4, 0
_0223D6A4: .word ov13_0224F5C0
	arm_func_end ov13_0223D66C

	arm_func_start ov13_0223D6A8
ov13_0223D6A8: ; 0x0223D6A8
	stmdb sp!, {r4, lr}
	ldr r1, _0223D714 ; =ov13_0224F5C0
	mov r4, r0
	ldr r3, [r1]
	cmp r3, #0
	beq _0223D6DC
	ldr r1, [r1, #4]
	ldr r0, _0223D718 ; =ov13_02247120
	ldr r2, [r1, #0x40]
	ldr r1, _0223D71C ; =ov13_02247200
	ldr r2, [r0, r2, lsl #2]
	mov r0, #0x8000000
	blx r3
_0223D6DC:
	ldr r0, _0223D714 ; =ov13_0224F5C0
	ldr r1, [r0, #4]
	str r4, [r1, #0x40]
	ldr r3, [r0]
	cmp r3, #0
	ldmeqia sp!, {r4, pc}
	ldr r1, [r0, #4]
	ldr r0, _0223D718 ; =ov13_02247120
	ldr r2, [r1, #0x40]
	ldr r1, _0223D720 ; =ov13_02247208
	ldr r2, [r0, r2, lsl #2]
	mov r0, #0x8000000
	blx r3
	ldmia sp!, {r4, pc}
	.balign 4, 0
_0223D714: .word ov13_0224F5C0
_0223D718: .word ov13_02247120
_0223D71C: .word ov13_02247200
_0223D720: .word ov13_02247208
	arm_func_end ov13_0223D6A8

	arm_func_start ov13_0223D724
ov13_0223D724: ; 0x0223D724
	ldr r1, _0223D740 ; =ov13_0224F5C0
	ldr r2, [r1, #4]
	ldr r1, [r2, #0x40]
	sub r1, r1, #9
	cmp r1, #1
	strhi r0, [r2, #0x54]
	bx lr
	.balign 4, 0
_0223D740: .word ov13_0224F5C0
	arm_func_end ov13_0223D724

	arm_func_start ov13_0223D744
ov13_0223D744: ; 0x0223D744
	stmdb sp!, {r3, lr}
	mov r0, #3
	bl ov13_0223D6A8
	ldr r1, _0223D780 ; =ov13_0224F5C0
	ldr r0, _0223D784 ; =ov13_0223D788
	ldr r1, [r1, #4]
	bl WM_SetParentParameter
	cmp r0, #2
	moveq r0, #1
	ldmeqia sp!, {r3, pc}
	bl ov13_0223D724
	mov r0, #9
	bl ov13_0223D6A8
	mov r0, #0
	ldmia sp!, {r3, pc}
	.balign 4, 0
_0223D780: .word ov13_0224F5C0
_0223D784: .word ov13_0223D788
	arm_func_end ov13_0223D744

	arm_func_start ov13_0223D788
ov13_0223D788: ; 0x0223D788
	stmdb sp!, {r3, lr}
	ldrh r0, [r0, #2]
	cmp r0, #0
	beq _0223D7A8
	bl ov13_0223D724
	mov r0, #9
	bl ov13_0223D6A8
	ldmia sp!, {r3, pc}
_0223D7A8:
	ldr r0, _0223D7F0 ; =ov13_0224F5C0
	ldr r0, [r0, #4]
	add r0, r0, #0x1000
	ldr r0, [r0, #0x3ac]
	cmp r0, #0
	beq _0223D7D8
	bl ov13_0223D7F4
	cmp r0, #0
	ldmneia sp!, {r3, pc}
	mov r0, #9
	bl ov13_0223D6A8
	ldmia sp!, {r3, pc}
_0223D7D8:
	bl ov13_0223D890
	cmp r0, #0
	ldmneia sp!, {r3, pc}
	mov r0, #9
	bl ov13_0223D6A8
	ldmia sp!, {r3, pc}
	.balign 4, 0
_0223D7F0: .word ov13_0224F5C0
	arm_func_end ov13_0223D788

	arm_func_start ov13_0223D7F4
ov13_0223D7F4: ; 0x0223D7F4
	stmdb sp!, {r3, lr}
	mov r0, #3
	bl ov13_0223D6A8
	ldr r0, _0223D850 ; =ov13_0224F5C0
	ldr r1, [r0, #4]
	add r0, r1, #0x1000
	ldr r2, [r0, #0x3ac]
	add r0, r1, #0x13c0
	blx r2
	ldr r2, _0223D850 ; =ov13_0224F5C0
	mov r1, r0
	ldr r2, [r2, #4]
	ldr r0, _0223D854 ; =ov13_0223D858
	add r2, r2, #0x13c0
	bl WM_SetWEPKey
	cmp r0, #2
	moveq r0, #1
	ldmeqia sp!, {r3, pc}
	bl ov13_0223D724
	mov r0, #9
	bl ov13_0223D6A8
	mov r0, #0
	ldmia sp!, {r3, pc}
	.balign 4, 0
_0223D850: .word ov13_0224F5C0
_0223D854: .word ov13_0223D858
	arm_func_end ov13_0223D7F4

	arm_func_start ov13_0223D858
ov13_0223D858: ; 0x0223D858
	stmdb sp!, {r3, lr}
	ldrh r0, [r0, #2]
	cmp r0, #0
	beq _0223D878
	bl ov13_0223D724
	mov r0, #9
	bl ov13_0223D6A8
	ldmia sp!, {r3, pc}
_0223D878:
	bl ov13_0223D890
	cmp r0, #0
	ldmneia sp!, {r3, pc}
	mov r0, #9
	bl ov13_0223D6A8
	ldmia sp!, {r3, pc}
	arm_func_end ov13_0223D858

	arm_func_start ov13_0223D890
ov13_0223D890: ; 0x0223D890
	stmdb sp!, {r3, lr}
	ldr r0, _0223D8EC ; =ov13_0224F5C0
	ldr r0, [r0, #4]
	ldr r0, [r0, #0x40]
	sub r0, r0, #4
	cmp r0, #2
	movls r0, #1
	ldmlsia sp!, {r3, pc}
	ldr r0, _0223D8F0 ; =ov13_0223D8F4
	bl WM_StartParent
	cmp r0, #2
	beq _0223D8CC
	bl ov13_0223D724
	mov r0, #0
	ldmia sp!, {r3, pc}
_0223D8CC:
	ldr r1, _0223D8EC ; =ov13_0224F5C0
	mov r3, #0
	ldr r2, [r1, #4]
	mov r0, #1
	strh r3, [r2, #0x50]
	ldr r1, [r1, #4]
	strh r0, [r1, #0x52]
	ldmia sp!, {r3, pc}
	.balign 4, 0
_0223D8EC: .word ov13_0224F5C0
_0223D8F0: .word ov13_0223D8F4
	arm_func_end ov13_0223D890

	arm_func_start ov13_0223D8F4
ov13_0223D8F4: ; 0x0223D8F4
	stmdb sp!, {r3, r4, r5, lr}
	mov r4, r0
	ldrh r2, [r4, #0x10]
	ldrh r0, [r4, #2]
	mov r1, #1
	mov r1, r1, lsl r2
	cmp r0, #0
	mov r5, r1, lsl #0x10
	beq _0223D928
	bl ov13_0223D724
	mov r0, #9
	bl ov13_0223D6A8
	ldmia sp!, {r3, r4, r5, pc}
_0223D928:
	ldrh ip, [r4, #8]
	cmp ip, #7
	bgt _0223D958
	bge _0223D964
	cmp ip, #2
	bgt _0223DA30
	cmp ip, #0
	blt _0223DA30
	beq _0223DA18
	cmp ip, #2
	ldmeqia sp!, {r3, r4, r5, pc}
	b _0223DA30
_0223D958:
	cmp ip, #9
	beq _0223D9E0
	b _0223DA30
_0223D964:
	ldr r0, _0223DA54 ; =ov13_0224F5C0
	ldr r3, [r0]
	cmp r3, #0
	beq _0223D980
	ldr r1, _0223DA58 ; =ov13_0224720C
	mov r0, #0x8000000
	blx r3
_0223D980:
	ldr r0, _0223DA54 ; =ov13_0224F5C0
	ldr r0, [r0, #4]
	ldr r1, [r0, #0x4c]
	cmp r1, #0
	beq _0223D9C8
	mov r0, r4
	blx r1
	cmp r0, #0
	bne _0223D9C8
	ldrh r1, [r4, #0x10]
	mov r0, #0
	bl WM_Disconnect
	cmp r0, #2
	ldmeqia sp!, {r3, r4, r5, pc}
	bl ov13_0223D724
	mov r0, #9
	bl ov13_0223D6A8
	ldmia sp!, {r3, r4, r5, pc}
_0223D9C8:
	ldr r0, _0223DA54 ; =ov13_0224F5C0
	ldr r1, [r0, #4]
	ldrh r0, [r1, #0x52]
	orr r0, r0, r5, lsr #16
	strh r0, [r1, #0x52]
	ldmia sp!, {r3, r4, r5, pc}
_0223D9E0:
	ldr r0, _0223DA54 ; =ov13_0224F5C0
	ldr r3, [r0]
	cmp r3, #0
	beq _0223D9FC
	ldr r1, _0223DA5C ; =ov13_02247238
	mov r0, #0x8000000
	blx r3
_0223D9FC:
	ldr r0, _0223DA54 ; =ov13_0224F5C0
	mvn r1, r5, lsr #16
	ldr r2, [r0, #4]
	ldrh r0, [r2, #0x52]
	and r0, r0, r1
	strh r0, [r2, #0x52]
	ldmia sp!, {r3, r4, r5, pc}
_0223DA18:
	bl ov13_0223DA64
	cmp r0, #0
	ldmneia sp!, {r3, r4, r5, pc}
	mov r0, #9
	bl ov13_0223D6A8
	ldmia sp!, {r3, r4, r5, pc}
_0223DA30:
	ldr r0, _0223DA54 ; =ov13_0224F5C0
	ldr r3, [r0]
	cmp r3, #0
	ldmeqia sp!, {r3, r4, r5, pc}
	ldr r1, _0223DA60 ; =ov13_02247264
	mov r2, ip
	mov r0, #0x8000000
	blx r3
	ldmia sp!, {r3, r4, r5, pc}
	.balign 4, 0
_0223DA54: .word ov13_0224F5C0
_0223DA58: .word ov13_0224720C
_0223DA5C: .word ov13_02247238
_0223DA60: .word ov13_02247264
	arm_func_end ov13_0223D8F4

	arm_func_start ov13_0223DA64
ov13_0223DA64: ; 0x0223DA64
	stmdb sp!, {r3, lr}
	sub sp, sp, #8
	ldr r0, _0223DAF4 ; =ov13_0224F5C0
	ldr r0, [r0, #4]
	ldr r0, [r0, #0x40]
	sub r0, r0, #4
	cmp r0, #2
	addls sp, sp, #8
	movls r0, #1
	ldmlsia sp!, {r3, pc}
	mov r0, #4
	bl ov13_0223D6A8
	ldr r0, _0223DAF4 ; =ov13_0224F5C0
	mov ip, #1
	ldr lr, [r0, #4]
	ldr r0, _0223DAF8 ; =ov13_0223DAFC
	add r3, lr, #0x1000
	ldr r1, [r3, #0x2a0]
	add r2, lr, #0x60
	mov r1, r1, lsl #0x10
	mov r1, r1, lsr #0x10
	stmia sp, {r1, ip}
	ldr r3, [r3, #0x2a4]
	add r1, r2, #0x1000
	mov r2, r3, lsl #0x10
	mov r2, r2, lsr #0x10
	add r3, lr, #0xf80
	bl WM_StartMP
	cmp r0, #2
	addeq sp, sp, #8
	moveq r0, #1
	ldmeqia sp!, {r3, pc}
	bl ov13_0223D724
	mov r0, #0
	add sp, sp, #8
	ldmia sp!, {r3, pc}
	.balign 4, 0
_0223DAF4: .word ov13_0224F5C0
_0223DAF8: .word ov13_0223DAFC
	arm_func_end ov13_0223DA64

	arm_func_start ov13_0223DAFC
ov13_0223DAFC: ; 0x0223DAFC
	stmdb sp!, {r3, lr}
	ldrh r1, [r0, #2]
	cmp r1, #0
	beq _0223DB20
	mov r0, r1
	bl ov13_0223D724
	mov r0, #9
	bl ov13_0223D6A8
	ldmia sp!, {r3, pc}
_0223DB20:
	ldrh r2, [r0, #4]
	sub r0, r2, #0xa
	cmp r0, #3
	addls pc, pc, r0, lsl #2
	b _0223DBFC
_0223DB34: ; jump table
	b _0223DB44 ; case 0
	ldmia sp!, {r3, pc} ; case 1
	b _0223DBFC ; case 2
	b _0223DBFC ; case 3
_0223DB44:
	ldr r0, _0223DC1C ; =ov13_0224F5C0
	ldr r1, [r0, #4]
	ldr r0, [r1, #0x44]
	cmp r0, #2
	bne _0223DBA4
	ldr r0, [r1, #0x40]
	cmp r0, #4
	bne _0223DB98
	bl ov13_0223DC28
	cmp r0, #0
	ldmneia sp!, {r3, pc}
	ldr r0, _0223DC1C ; =ov13_0224F5C0
	ldr r2, [r0]
	cmp r2, #0
	beq _0223DB8C
	ldr r1, _0223DC20 ; =ov13_02247284
	mov r0, #0x8000000
	blx r2
_0223DB8C:
	mov r0, #9
	bl ov13_0223D6A8
	ldmia sp!, {r3, pc}
_0223DB98:
	cmp r0, #6
	bne _0223DBF0
	ldmia sp!, {r3, pc}
_0223DBA4:
	cmp r0, #4
	bne _0223DBF0
	add r0, r1, #0x3e0
	mov ip, #1
	add r0, r0, #0x1000
	mov r1, #0xd
	mov r2, #7
	mov r3, #0x44
	str ip, [sp]
	bl WM_StartDataSharing
	cmp r0, #0
	beq _0223DBE4
	bl ov13_0223D724
	mov r0, #9
	bl ov13_0223D6A8
	ldmia sp!, {r3, pc}
_0223DBE4:
	mov r0, #5
	bl ov13_0223D6A8
	ldmia sp!, {r3, pc}
_0223DBF0:
	mov r0, #4
	bl ov13_0223D6A8
	ldmia sp!, {r3, pc}
_0223DBFC:
	ldr r0, _0223DC1C ; =ov13_0224F5C0
	ldr r3, [r0]
	cmp r3, #0
	ldmeqia sp!, {r3, pc}
	ldr r1, _0223DC24 ; =ov13_02247264
	mov r0, #0x8000000
	blx r3
	ldmia sp!, {r3, pc}
	.balign 4, 0
_0223DC1C: .word ov13_0224F5C0
_0223DC20: .word ov13_02247284
_0223DC24: .word ov13_02247264
	arm_func_end ov13_0223DAFC

	arm_func_start ov13_0223DC28
ov13_0223DC28: ; 0x0223DC28
	stmdb sp!, {r3, lr}
	mov r0, #6
	bl ov13_0223D6A8
	ldr r0, _0223DC60 ; =ov13_0224F5C0
	mov r1, #0xd
	ldr r0, [r0, #4]
	add r0, r0, #0x1e00
	bl WM_StartKeySharing
	cmp r0, #2
	moveq r0, #1
	ldmeqia sp!, {r3, pc}
	bl ov13_0223D724
	mov r0, #0
	ldmia sp!, {r3, pc}
	.balign 4, 0
_0223DC60: .word ov13_0224F5C0
	arm_func_end ov13_0223DC28

	arm_func_start ov13_0223DC64
ov13_0223DC64: ; 0x0223DC64
	stmdb sp!, {r3, lr}
	ldr r0, _0223DC90 ; =ov13_0224F5C0
	ldr r0, [r0, #4]
	add r0, r0, #0x1e00
	bl WM_EndKeySharing
	cmp r0, #2
	moveq r0, #1
	ldmeqia sp!, {r3, pc}
	bl ov13_0223D724
	mov r0, #0
	ldmia sp!, {r3, pc}
	.balign 4, 0
_0223DC90: .word ov13_0224F5C0
	arm_func_end ov13_0223DC64

	arm_func_start ov13_0223DC94
ov13_0223DC94: ; 0x0223DC94
	stmdb sp!, {r3, lr}
	mov r0, #3
	bl ov13_0223D6A8
	ldr r0, _0223DCC0 ; =ov13_0223DCC4
	bl WM_EndMP
	cmp r0, #2
	moveq r0, #1
	ldmeqia sp!, {r3, pc}
	bl ov13_0223D724
	mov r0, #0
	ldmia sp!, {r3, pc}
	.balign 4, 0
_0223DCC0: .word ov13_0223DCC4
	arm_func_end ov13_0223DC94

	arm_func_start ov13_0223DCC4
ov13_0223DCC4: ; 0x0223DCC4
	stmdb sp!, {r3, lr}
	ldrh r0, [r0, #2]
	cmp r0, #0
	beq _0223DCE0
	bl ov13_0223D724
	bl ov13_0223E698
	ldmia sp!, {r3, pc}
_0223DCE0:
	bl ov13_0223DD18
	cmp r0, #0
	ldmneia sp!, {r3, pc}
	ldr r0, _0223DD10 ; =ov13_0224F5C0
	ldr r2, [r0]
	cmp r2, #0
	beq _0223DD08
	ldr r1, _0223DD14 ; =ov13_022472B4
	mov r0, #0x8000000
	blx r2
_0223DD08:
	bl ov13_0223E698
	ldmia sp!, {r3, pc}
	.balign 4, 0
_0223DD10: .word ov13_0224F5C0
_0223DD14: .word ov13_022472B4
	arm_func_end ov13_0223DCC4

	arm_func_start ov13_0223DD18
ov13_0223DD18: ; 0x0223DD18
	stmdb sp!, {r3, lr}
	ldr r0, _0223DD3C ; =ov13_0223DD40
	bl WM_EndParent
	cmp r0, #2
	moveq r0, #1
	ldmeqia sp!, {r3, pc}
	bl ov13_0223D724
	mov r0, #0
	ldmia sp!, {r3, pc}
	.balign 4, 0
_0223DD3C: .word ov13_0223DD40
	arm_func_end ov13_0223DD18

	arm_func_start ov13_0223DD40
ov13_0223DD40: ; 0x0223DD40
	stmdb sp!, {r3, lr}
	ldrh r0, [r0, #2]
	cmp r0, #0
	beq _0223DD58
	bl ov13_0223D724
	ldmia sp!, {r3, pc}
_0223DD58:
	mov r0, #1
	bl ov13_0223D6A8
	ldmia sp!, {r3, pc}
	arm_func_end ov13_0223DD40

	arm_func_start ov13_0223DD64
ov13_0223DD64: ; 0x0223DD64
	stmdb sp!, {r3, lr}
	ldr r0, _0223DDB0 ; =ov13_0224F5C0
	ldr r0, [r0, #4]
	ldr r0, [r0, #0x40]
	cmp r0, #6
	movne r0, #0
	ldmneia sp!, {r3, pc}
	mov r0, #3
	bl ov13_0223D6A8
	ldr r0, _0223DDB0 ; =ov13_0224F5C0
	ldr r0, [r0, #4]
	add r0, r0, #0x1e00
	bl WM_EndKeySharing
	cmp r0, #2
	moveq r0, #1
	ldmeqia sp!, {r3, pc}
	bl ov13_0223D724
	mov r0, #0
	ldmia sp!, {r3, pc}
	.balign 4, 0
_0223DDB0: .word ov13_0224F5C0
	arm_func_end ov13_0223DD64

	arm_func_start ov13_0223DDB4
ov13_0223DDB4: ; 0x0223DDB4
	stmdb sp!, {r3, lr}
	mov r0, #3
	bl ov13_0223D6A8
	ldr r0, _0223DDE0 ; =ov13_0223DDE4
	bl WM_EndMP
	cmp r0, #2
	moveq r0, #1
	ldmeqia sp!, {r3, pc}
	bl ov13_0223D724
	mov r0, #0
	ldmia sp!, {r3, pc}
	.balign 4, 0
_0223DDE0: .word ov13_0223DDE4
	arm_func_end ov13_0223DDB4

	arm_func_start ov13_0223DDE4
ov13_0223DDE4: ; 0x0223DDE4
	stmdb sp!, {r3, lr}
	ldrh r0, [r0, #2]
	cmp r0, #0
	beq _0223DE00
	bl ov13_0223D724
	bl ov13_0223E6B4
	ldmia sp!, {r3, pc}
_0223DE00:
	bl ov13_0223DE18
	cmp r0, #0
	ldmneia sp!, {r3, pc}
	mov r0, #9
	bl ov13_0223D6A8
	ldmia sp!, {r3, pc}
	arm_func_end ov13_0223DDE4

	arm_func_start ov13_0223DE18
ov13_0223DE18: ; 0x0223DE18
	stmdb sp!, {r3, lr}
	mov r0, #3
	bl ov13_0223D6A8
	ldr r0, _0223DE4C ; =ov13_0223DE50
	mov r1, #0
	bl WM_Disconnect
	cmp r0, #2
	moveq r0, #1
	ldmeqia sp!, {r3, pc}
	bl ov13_0223D724
	bl ov13_0223E698
	mov r0, #0
	ldmia sp!, {r3, pc}
	.balign 4, 0
_0223DE4C: .word ov13_0223DE50
	arm_func_end ov13_0223DE18

	arm_func_start ov13_0223DE50
ov13_0223DE50: ; 0x0223DE50
	stmdb sp!, {r3, lr}
	ldrh r0, [r0, #2]
	cmp r0, #0
	beq _0223DE68
	bl ov13_0223D724
	ldmia sp!, {r3, pc}
_0223DE68:
	mov r0, #1
	bl ov13_0223D6A8
	ldmia sp!, {r3, pc}
	arm_func_end ov13_0223DE50

	arm_func_start ov13_0223DE74
ov13_0223DE74: ; 0x0223DE74
	stmdb sp!, {r3, lr}
	mov r0, #3
	bl ov13_0223D6A8
	ldr r0, _0223DEA0 ; =ov13_0223DEA4
	bl WM_Reset
	cmp r0, #2
	moveq r0, #1
	ldmeqia sp!, {r3, pc}
	bl ov13_0223D724
	mov r0, #0
	ldmia sp!, {r3, pc}
	.balign 4, 0
_0223DEA0: .word ov13_0223DEA4
	arm_func_end ov13_0223DE74

	arm_func_start ov13_0223DEA4
ov13_0223DEA4: ; 0x0223DEA4
	stmdb sp!, {r4, lr}
	mov r4, r0
	ldrh r0, [r4, #2]
	cmp r0, #0
	beq _0223DECC
	mov r0, #9
	bl ov13_0223D6A8
	ldrh r0, [r4, #2]
	bl ov13_0223D724
	ldmia sp!, {r4, pc}
_0223DECC:
	mov r0, #1
	bl ov13_0223D6A8
	ldmia sp!, {r4, pc}
	arm_func_end ov13_0223DEA4

	arm_func_start ov13_0223DED8
ov13_0223DED8: ; 0x0223DED8
	stmdb sp!, {r3, lr}
	ldrh r0, [r0, #2]
	cmp r0, #0
	beq _0223DEF4
	mov r0, #0xa
	bl ov13_0223D6A8
	ldmia sp!, {r3, pc}
_0223DEF4:
	mov r0, #0
	bl ov13_0223D6A8
	ldmia sp!, {r3, pc}
	arm_func_end ov13_0223DED8

	arm_func_start ov13_0223DF00
ov13_0223DF00: ; 0x0223DF00
	ldr r1, _0223DF10 ; =ov13_0224F5C0
	ldr r1, [r1, #4]
	str r0, [r1, #8]
	bx lr
	.balign 4, 0
_0223DF10: .word ov13_0224F5C0
	arm_func_end ov13_0223DF00

	arm_func_start ov13_0223DF14
ov13_0223DF14: ; 0x0223DF14
	ldr r0, _0223DF24 ; =ov13_0224F5C0
	ldr r0, [r0, #4]
	ldrh r0, [r0, #0x52]
	bx lr
	.balign 4, 0
_0223DF24: .word ov13_0224F5C0
	arm_func_end ov13_0223DF14

	arm_func_start ov13_0223DF28
ov13_0223DF28: ; 0x0223DF28
	ldr r0, _0223DF38 ; =ov13_0224F5C0
	ldr r0, [r0, #4]
	ldr r0, [r0, #0x40]
	bx lr
	.balign 4, 0
_0223DF38: .word ov13_0224F5C0
	arm_func_end ov13_0223DF28

	arm_func_start ov13_0223DF3C
ov13_0223DF3C: ; 0x0223DF3C
	stmdb sp!, {r3, lr}
	sub sp, sp, #8
	add r0, sp, #0
	bl OS_GetMacAddress
	ldr r2, _0223E008 ; =0x027FFC3C
	ldrh r0, [sp]
	ldr r3, [r2]
	ldrh r2, [sp, #2]
	add r0, r0, r3
	ldr r1, _0223E00C ; =ov13_0224F5C0
	add r2, r2, r0
	ldrh r3, [sp, #4]
	ldr r0, [r1, #4]
	add r2, r3, r2
	str r2, [r0, #0x58]
	ldr ip, [r1, #4]
	ldr r0, _0223E010 ; =0x00010DCD
	ldr r2, [ip, #0x58]
	mov r3, #0
	mul r0, r2, r0
	add r0, r0, #0x39
	add r0, r0, #0x3000
	str r0, [ip, #0x58]
	ldr r0, [r1, #4]
	mov r2, #0x65
	strh r3, [r0, #0x5c]
	ldr r1, [r1, #4]
	mov r0, #3
	strh r2, [r1, #0x5e]
	bl ov13_0223D6A8
	mov r0, #1
	bl ov13_0223E014
	cmp r0, #0x18
	bne _0223DFE0
	mov r0, #0x18
	bl ov13_0223D724
	mov r0, #9
	bl ov13_0223D6A8
	add sp, sp, #8
	mov r0, #0
	ldmia sp!, {r3, pc}
_0223DFE0:
	cmp r0, #2
	addeq sp, sp, #8
	moveq r0, #1
	ldmeqia sp!, {r3, pc}
	bl ov13_0223D724
	mov r0, #9
	bl ov13_0223D6A8
	mov r0, #0
	add sp, sp, #8
	ldmia sp!, {r3, pc}
	.balign 4, 0
_0223E008: .word 0x027FFC3C
_0223E00C: .word ov13_0224F5C0
_0223E010: .word 0x00010DCD
	arm_func_end ov13_0223DF3C

	arm_func_start ov13_0223E014
ov13_0223E014: ; 0x0223E014
	stmdb sp!, {r4, lr}
	mov r4, r0
	bl WM_GetAllowedChannel
	cmp r0, #0x8000
	bne _0223E040
	mov r0, #3
	bl ov13_0223D724
	mov r0, #9
	bl ov13_0223D6A8
	mov r0, #3
	ldmia sp!, {r4, pc}
_0223E040:
	cmp r0, #0
	bne _0223E060
	mov r0, #0x16
	bl ov13_0223D724
	mov r0, #9
	bl ov13_0223D6A8
	mov r0, #0x18
	ldmia sp!, {r4, pc}
_0223E060:
	sub r1, r4, #1
	mov r2, #1
	tst r0, r2, lsl r1
	bne _0223E094
_0223E070:
	add r1, r4, #1
	mov r1, r1, lsl #0x10
	mov r4, r1, lsr #0x10
	cmp r4, #0x10
	movhi r0, #0x18
	ldmhiia sp!, {r4, pc}
	sub r1, r4, #1
	tst r0, r2, lsl r1
	beq _0223E070
_0223E094:
	ldr r0, _0223E0AC ; =ov13_0223E0B0
	mov r1, r4
	bl ov13_0223E188
	mov r0, r0, lsl #0x10
	mov r0, r0, lsr #0x10
	ldmia sp!, {r4, pc}
	.balign 4, 0
_0223E0AC: .word ov13_0223E0B0
	arm_func_end ov13_0223E014

	arm_func_start ov13_0223E0B0
ov13_0223E0B0: ; 0x0223E0B0
	stmdb sp!, {r4, lr}
	mov r4, r0
	ldrh r0, [r4, #2]
	cmp r0, #0
	beq _0223E0D4
	bl ov13_0223D724
	mov r0, #9
	bl ov13_0223D6A8
	ldmia sp!, {r4, pc}
_0223E0D4:
	ldr r0, _0223E180 ; =ov13_0224F5C0
	ldr ip, [r0]
	cmp ip, #0
	beq _0223E0F8
	ldrh r2, [r4, #8]
	ldrh r3, [r4, #0xa]
	ldr r1, _0223E184 ; =ov13_0224730C
	mov r0, #0x8000000
	blx ip
_0223E0F8:
	ldr r0, _0223E180 ; =ov13_0224F5C0
	ldrh r2, [r4, #0xa]
	ldr r3, [r0, #4]
	ldrh ip, [r4, #8]
	ldrh r1, [r3, #0x5e]
	cmp r1, r2
	bls _0223E130
	strh r2, [r3, #0x5e]
	sub r1, ip, #1
	mov r2, #1
	mov r1, r2, lsl r1
	ldr r0, [r0, #4]
	strh r1, [r0, #0x60]
	b _0223E148
_0223E130:
	bne _0223E148
	ldrh r2, [r3, #0x60]
	sub r0, ip, #1
	mov r1, #1
	orr r0, r2, r1, lsl r0
	strh r0, [r3, #0x60]
_0223E148:
	add r0, ip, #1
	mov r0, r0, lsl #0x10
	mov r0, r0, lsr #0x10
	bl ov13_0223E014
	cmp r0, #0x18
	bne _0223E16C
	mov r0, #7
	bl ov13_0223D6A8
	ldmia sp!, {r4, pc}
_0223E16C:
	cmp r0, #2
	ldmeqia sp!, {r4, pc}
	mov r0, #9
	bl ov13_0223D6A8
	ldmia sp!, {r4, pc}
	.balign 4, 0
_0223E180: .word ov13_0224F5C0
_0223E184: .word ov13_0224730C
	arm_func_end ov13_0223E0B0

	arm_func_start ov13_0223E188
ov13_0223E188: ; 0x0223E188
	stmdb sp!, {r3, lr}
	mov r3, r1
	mov ip, #0x1e
	mov r1, #3
	mov r2, #0x11
	str ip, [sp]
	bl WM_MeasureChannel
	ldmia sp!, {r3, pc}
	arm_func_end ov13_0223E188

	arm_func_start ov13_0223E1A8
ov13_0223E1A8: ; 0x0223E1A8
	stmdb sp!, {r3, lr}
	ldr r0, _0223E218 ; =ov13_0224F5C0
	ldr r0, [r0, #4]
	ldr r0, [r0, #0x40]
	cmp r0, #7
	beq _0223E1C4
	bl OS_Terminate
_0223E1C4:
	mov r0, #1
	bl ov13_0223D6A8
	ldr r0, _0223E218 ; =ov13_0224F5C0
	ldr r0, [r0, #4]
	ldrh r0, [r0, #0x60]
	bl ov13_0223E220
	ldr r1, _0223E218 ; =ov13_0224F5C0
	ldr r2, [r1, #4]
	strh r0, [r2, #0x5c]
	ldr r3, [r1]
	cmp r3, #0
	beq _0223E208
	ldr r0, [r1, #4]
	ldr r1, _0223E21C ; =ov13_02247324
	ldrh r2, [r0, #0x5c]
	mov r0, #0x8000000
	blx r3
_0223E208:
	ldr r0, _0223E218 ; =ov13_0224F5C0
	ldr r0, [r0, #4]
	ldrh r0, [r0, #0x5c]
	ldmia sp!, {r3, pc}
	.balign 4, 0
_0223E218: .word ov13_0224F5C0
_0223E21C: .word ov13_02247324
	arm_func_end ov13_0223E1A8

	arm_func_start ov13_0223E220
ov13_0223E220: ; 0x0223E220
	stmdb sp!, {r4, lr}
	mov r3, #0
	mov r1, r3
	mov ip, r3
	mov r4, #1
_0223E234:
	tst r0, r4, lsl ip
	beq _0223E254
	add r3, ip, #1
	add r2, r1, #1
	mov r1, r3, lsl #0x10
	mov r2, r2, lsl #0x10
	mov r3, r1, asr #0x10
	mov r1, r2, lsr #0x10
_0223E254:
	add r2, ip, #1
	mov r2, r2, lsl #0x10
	mov ip, r2, asr #0x10
	cmp ip, #0x10
	blt _0223E234
	cmp r1, #1
	movls r0, r3
	ldmlsia sp!, {r4, pc}
	ldr ip, _0223E300 ; =ov13_0224F5C0
	ldr r3, _0223E304 ; =0x00010DCD
	ldr r4, [ip, #4]
	mov r2, #0
	ldr lr, [r4, #0x58]
	mul r3, lr, r3
	add r3, r3, #0x39
	add r3, r3, #0x3000
	str r3, [r4, #0x58]
	ldr r3, [ip, #4]
	ldr r3, [r3, #0x58]
	and r3, r3, #0xff
	mul r3, r1, r3
	mov r1, r3, lsl #8
	mov r3, r1, lsr #0x10
_0223E2B0:
	tst r0, #1
	beq _0223E2DC
	cmp r3, #0
	bne _0223E2D0
	add r0, r2, #1
	mov r0, r0, lsl #0x10
	mov r0, r0, asr #0x10
	ldmia sp!, {r4, pc}
_0223E2D0:
	sub r1, r3, #1
	mov r1, r1, lsl #0x10
	mov r3, r1, lsr #0x10
_0223E2DC:
	add r1, r2, #1
	mov r1, r1, lsl #0x10
	mov r0, r0, lsl #0xf
	mov r2, r1, asr #0x10
	cmp r2, #0x10
	mov r0, r0, lsr #0x10
	blt _0223E2B0
	mov r0, #0
	ldmia sp!, {r4, pc}
	.balign 4, 0
_0223E300: .word ov13_0224F5C0
_0223E304: .word 0x00010DCD
	arm_func_end ov13_0223E220

	arm_func_start ov13_0223E308
ov13_0223E308: ; 0x0223E308
	stmdb sp!, {r3, lr}
	ldr r2, _0223E37C ; =ov13_0224F5C0
	mov r0, #0
	ldr r1, [r2, #4]
	mov r3, #1
	add r1, r1, #0x1000
	str r0, [r1, #0x2a4]
	ldr r1, [r2, #4]
	add r1, r1, #0x1000
	str r0, [r1, #0x2a0]
	ldr r1, [r2, #4]
	str r0, [r1, #0x48]
	ldr r1, [r2, #4]
	strh r0, [r1, #0x50]
	ldr r1, [r2, #4]
	strh r3, [r1, #0x52]
	ldr r1, [r2, #4]
	str r0, [r1, #0x54]
	ldr r1, [r2, #4]
	str r0, [r1]
	ldr r1, [r2, #4]
	strh r0, [r1, #4]
	ldr r1, [r2, #4]
	str r0, [r1, #0x4c]
	bl ov13_0223E3A0
	cmp r0, #0
	moveq r0, #0
	movne r0, #1
	ldmia sp!, {r3, pc}
	.balign 4, 0
_0223E37C: .word ov13_0224F5C0
	arm_func_end ov13_0223E308

	arm_func_start ov13_0223E380
ov13_0223E380: ; 0x0223E380
	stmdb sp!, {r3, lr}
	ldrh r0, [r0, #2]
	cmp r0, #8
	ldmneia sp!, {r3, pc}
	mov r0, #9
	bl ov13_0223D6A8
	bl OS_Terminate
	ldmia sp!, {r3, pc}
	arm_func_end ov13_0223E380

	arm_func_start ov13_0223E3A0
ov13_0223E3A0: ; 0x0223E3A0
	stmdb sp!, {r3, lr}
	mov r0, #3
	bl ov13_0223D6A8
	ldr r0, _0223E3E4 ; =ov13_0224F5C0
	ldr r1, _0223E3E8 ; =ov13_0223E3EC
	ldr r0, [r0, #4]
	mov r2, #2
	add r0, r0, #0x80
	bl WM_Initialize
	cmp r0, #2
	moveq r0, #1
	ldmeqia sp!, {r3, pc}
	bl ov13_0223D724
	mov r0, #0xa
	bl ov13_0223D6A8
	mov r0, #0
	ldmia sp!, {r3, pc}
	.balign 4, 0
_0223E3E4: .word ov13_0224F5C0
_0223E3E8: .word ov13_0223E3EC
	arm_func_end ov13_0223E3A0

	arm_func_start ov13_0223E3EC
ov13_0223E3EC: ; 0x0223E3EC
	stmdb sp!, {r3, lr}
	ldrh r0, [r0, #2]
	cmp r0, #0
	beq _0223E40C
	bl ov13_0223D724
	mov r0, #0xa
	bl ov13_0223D6A8
	ldmia sp!, {r3, pc}
_0223E40C:
	ldr r0, _0223E438 ; =ov13_0223E380
	bl WM_SetIndCallback
	cmp r0, #0
	beq _0223E42C
	bl ov13_0223D724
	mov r0, #0xa
	bl ov13_0223D6A8
	ldmia sp!, {r3, pc}
_0223E42C:
	mov r0, #1
	bl ov13_0223D6A8
	ldmia sp!, {r3, pc}
	.balign 4, 0
_0223E438: .word ov13_0223E380
	arm_func_end ov13_0223E3EC

	arm_func_start ov13_0223E43C
ov13_0223E43C: ; 0x0223E43C
	stmdb sp!, {r4, r5, r6, lr}
	ldr r3, _0223E5A4 ; =ov13_0224F5C0
	mov r6, r0
	ldr r0, [r3, #4]
	mov r5, r1
	ldr r0, [r0, #0x40]
	mov r4, r2
	cmp r0, #1
	beq _0223E468
	bl OS_Terminate
	movs r0, #0
_0223E468:
	ldr r1, _0223E5A4 ; =ov13_0224F5C0
	mov r3, #0x180
	ldr r0, [r1, #4]
	mov r2, #0xe0
	add r0, r0, #0x1000
	str r3, [r0, #0x2a4]
	ldr r0, [r1, #4]
	add r0, r0, #0x1000
	str r2, [r0, #0x2a0]
	ldr r3, [r1]
	cmp r3, #0
	beq _0223E4B0
	ldr r0, [r1, #4]
	ldr r1, _0223E5A8 ; =ov13_022472DC
	add r0, r0, #0x1000
	ldr r2, [r0, #0x2a4]
	mov r0, #0x8000000
	blx r3
_0223E4B0:
	ldr r0, _0223E5A4 ; =ov13_0224F5C0
	ldr r3, [r0]
	cmp r3, #0
	beq _0223E4D8
	ldr r0, [r0, #4]
	ldr r1, _0223E5AC ; =ov13_022472F4
	add r0, r0, #0x1000
	ldr r2, [r0, #0x2a0]
	mov r0, #0x8000000
	blx r3
_0223E4D8:
	ldr r1, _0223E5A4 ; =ov13_0224F5C0
	mov r0, #3
	ldr r1, [r1, #4]
	str r6, [r1, #0x44]
	bl ov13_0223D6A8
	ldr r0, _0223E5A4 ; =ov13_0224F5C0
	ldr r1, [r0, #4]
	strh r5, [r1, #0xc]
	ldr r0, [r0, #4]
	strh r4, [r0, #0x32]
	bl WM_GetDispersionBeaconPeriod
	ldr r1, _0223E5A4 ; =ov13_0224F5C0
	mov r2, #0xd0
	ldr r3, [r1, #4]
	mov r4, #0x44
	strh r0, [r3, #0x18]
	ldr r0, [r1, #4]
	mov r3, #2
	strh r2, [r0, #0x34]
	ldr r0, [r1, #4]
	mov r2, #0
	strh r4, [r0, #0x36]
	ldr r0, [r1, #4]
	mov r4, #1
	strh r3, [r0, #0x10]
	ldr r0, [r1, #4]
	cmp r6, #2
	strh r2, [r0, #0x16]
	ldr r0, [r1, #4]
	strh r2, [r0, #0x12]
	ldr r0, [r1, #4]
	strh r4, [r0, #0xe]
	ldr r0, _0223E5A4 ; =ov13_0224F5C0
	movne r4, r2
	ldr r1, [r0, #4]
	cmp r6, #0
	cmpne r6, #2
	strh r4, [r1, #0x14]
	cmpne r6, #4
	bne _0223E580
	bl ov13_0223D744
	ldmia sp!, {r4, r5, r6, pc}
_0223E580:
	ldr r3, [r0]
	cmp r3, #0
	beq _0223E59C
	ldr r1, _0223E5B0 ; =ov13_0224733C
	mov r2, r6
	mov r0, #0x8000000
	blx r3
_0223E59C:
	mov r0, #0
	ldmia sp!, {r4, r5, r6, pc}
	.balign 4, 0
_0223E5A4: .word ov13_0224F5C0
_0223E5A8: .word ov13_022472DC
_0223E5AC: .word ov13_022472F4
_0223E5B0: .word ov13_0224733C
	arm_func_end ov13_0223E43C

	arm_func_start ov13_0223E5B4
ov13_0223E5B4: ; 0x0223E5B4
	ldr r1, _0223E5C4 ; =ov13_0224F5C0
	ldr r1, [r1, #4]
	str r0, [r1, #0x4c]
	bx lr
	.balign 4, 0
_0223E5C4: .word ov13_0224F5C0
	arm_func_end ov13_0223E5B4

	arm_func_start ov13_0223E5C8
ov13_0223E5C8: ; 0x0223E5C8
	ldr r1, _0223E5E8 ; =ov13_0224F5C0
	ldr ip, _0223E5EC ; =WM_GetSharedDataAddress
	ldr r1, [r1, #4]
	mov r2, r0
	add r0, r1, #0x3e0
	add r0, r0, #0x1000
	add r1, r1, #0x1c00
	bx ip
	.balign 4, 0
_0223E5E8: .word ov13_0224F5C0
_0223E5EC: .word WM_GetSharedDataAddress
	arm_func_end ov13_0223E5C8

	arm_func_start ov13_0223E5F0
ov13_0223E5F0: ; 0x0223E5F0
	stmdb sp!, {r4, lr}
	ldr r2, _0223E68C ; =ov13_0224F5C0
	mov r1, r0
	ldr r2, [r2, #4]
	add r0, r2, #0x3e0
	add r0, r0, #0x1000
	add r2, r2, #0x1c00
	bl WM_StepDataSharing
	mov r4, r0
	cmp r4, #7
	bne _0223E640
	ldr r0, _0223E68C ; =ov13_0224F5C0
	ldr r2, [r0]
	cmp r2, #0
	beq _0223E638
	ldr r1, _0223E690 ; =ov13_02247358
	mov r0, #0x8000000
	blx r2
_0223E638:
	mov r0, #0
	ldmia sp!, {r4, pc}
_0223E640:
	cmp r4, #5
	bne _0223E674
	ldr r0, _0223E68C ; =ov13_0224F5C0
	ldr r2, [r0]
	cmp r2, #0
	beq _0223E664
	ldr r1, _0223E694 ; =ov13_02247388
	mov r0, #0x8000000
	blx r2
_0223E664:
	mov r0, r4
	bl ov13_0223D724
	mov r0, #0
	ldmia sp!, {r4, pc}
_0223E674:
	cmp r4, #0
	moveq r0, #1
	ldmeqia sp!, {r4, pc}
	bl ov13_0223D724
	mov r0, #0
	ldmia sp!, {r4, pc}
	.balign 4, 0
_0223E68C: .word ov13_0224F5C0
_0223E690: .word ov13_02247358
_0223E694: .word ov13_02247388
	arm_func_end ov13_0223E5F0

	arm_func_start ov13_0223E698
ov13_0223E698: ; 0x0223E698
	stmdb sp!, {r3, lr}
	bl ov13_0223DE74
	cmp r0, #0
	ldmneia sp!, {r3, pc}
	mov r0, #0xa
	bl ov13_0223D6A8
	ldmia sp!, {r3, pc}
	arm_func_end ov13_0223E698

	arm_func_start ov13_0223E6B4
ov13_0223E6B4: ; 0x0223E6B4
	stmdb sp!, {r3, lr}
	ldr r0, _0223E7B0 ; =ov13_0224F5C0
	ldr r1, [r0, #4]
	ldr r2, [r1, #0x40]
	cmp r2, #1
	bne _0223E6E8
	ldr r2, [r0]
	cmp r2, #0
	ldmeqia sp!, {r3, pc}
	ldr r1, _0223E7B4 ; =ov13_022473BC
	mov r0, #0x8000000
	blx r2
	ldmia sp!, {r3, pc}
_0223E6E8:
	ldr r3, [r0]
	cmp r3, #0
	beq _0223E700
	ldr r1, _0223E7B8 ; =ov13_022473E0
	mov r0, #0x8000000
	blx r3
_0223E700:
	ldr r0, _0223E7B0 ; =ov13_0224F5C0
	ldr r0, [r0, #4]
	ldr r0, [r0, #0x40]
	cmp r0, #6
	cmpne r0, #5
	cmpne r0, #4
	mov r0, #3
	beq _0223E72C
	bl ov13_0223D6A8
	bl ov13_0223E698
	ldmia sp!, {r3, pc}
_0223E72C:
	bl ov13_0223D6A8
	ldr r0, _0223E7B0 ; =ov13_0224F5C0
	ldr r0, [r0, #4]
	ldr r0, [r0, #0x44]
	cmp r0, #5
	addls pc, pc, r0, lsl #2
	ldmia sp!, {r3, pc}
_0223E748: ; jump table
	b _0223E79C ; case 0
	b _0223E774 ; case 1
	b _0223E788 ; case 2
	b _0223E760 ; case 3
	b _0223E79C ; case 4
	b _0223E774 ; case 5
_0223E760:
	bl ov13_0223DD64
	cmp r0, #0
	ldmneia sp!, {r3, pc}
	bl ov13_0223E698
	ldmia sp!, {r3, pc}
_0223E774:
	bl ov13_0223DDB4
	cmp r0, #0
	ldmneia sp!, {r3, pc}
	bl ov13_0223E698
	ldmia sp!, {r3, pc}
_0223E788:
	bl ov13_0223DC64
	cmp r0, #0
	ldmneia sp!, {r3, pc}
	bl ov13_0223E698
	ldmia sp!, {r3, pc}
_0223E79C:
	bl ov13_0223DC94
	cmp r0, #0
	ldmneia sp!, {r3, pc}
	bl ov13_0223E698
	ldmia sp!, {r3, pc}
	.balign 4, 0
_0223E7B0: .word ov13_0224F5C0
_0223E7B4: .word ov13_022473BC
_0223E7B8: .word ov13_022473E0
	arm_func_end ov13_0223E6B4

	arm_func_start ov13_0223E7BC
ov13_0223E7BC: ; 0x0223E7BC
	stmdb sp!, {r3, lr}
	ldr r0, _0223E804 ; =ov13_0224F5C0
	ldr r0, [r0, #4]
	ldr r0, [r0, #0x40]
	cmp r0, #1
	beq _0223E7D8
	bl OS_Terminate
_0223E7D8:
	mov r0, #3
	bl ov13_0223D6A8
	ldr r0, _0223E808 ; =ov13_0223DED8
	bl WM_End
	cmp r0, #2
	moveq r0, #1
	ldmeqia sp!, {r3, pc}
	mov r0, #9
	bl ov13_0223D6A8
	mov r0, #0
	ldmia sp!, {r3, pc}
	.balign 4, 0
_0223E804: .word ov13_0224F5C0
_0223E808: .word ov13_0223DED8
	arm_func_end ov13_0223E7BC

	arm_func_start ov13_0223E80C
ov13_0223E80C: ; 0x0223E80C
	stmdb sp!, {r4, lr}
	ldr r2, _0223E8F8 ; =ov13_0224F5C8
	mov r4, r1
	str r0, [r2]
	add r0, r0, #0xb00
	bl ov13_0223C9D4
	ldr r1, _0223E8F8 ; =ov13_0224F5C8
	mov r3, #0
	ldr r0, [r1]
	mov r2, #1
	add r0, r0, #0x600
	strh r3, [r0, #0x48]
	ldr r0, [r1]
	add r0, r0, #0x600
	strh r3, [r0, #0x4a]
	ldr r0, [r1]
	strb r2, [r0, #0xa90]
	ldr r0, [r1]
	strb r2, [r0, #0xa91]
	ldr r0, [r1]
	str r3, [r0, #0xa9c]
	bl ov13_0223F3BC
	ldr r0, _0223E8F8 ; =ov13_0224F5C8
	ldr r3, [r4]
	ldr r2, [r0]
	mov r1, #2
	str r3, [r2, #0xab4]
	ldr r3, [r4, #4]
	ldr r2, [r0]
	str r3, [r2, #0xab8]
	ldr r3, [r4, #8]
	ldr r2, [r0]
	str r3, [r2, #0xabc]
	ldr r3, [r4, #0xc]
	ldr r2, [r0]
	str r3, [r2, #0xac0]
	ldr r3, [r4, #0x10]
	ldr r2, [r0]
	str r3, [r2, #0xac4]
	ldr r3, [r4, #0x14]
	ldr r2, [r0]
	str r3, [r2, #0xac8]
	ldrb r3, [r4, #0x18]
	ldr r2, [r0]
	strb r3, [r2, #0xa92]
	ldr r0, [r0]
	strb r1, [r0, #0xacc]
	bl OS_GetTick
	ldr r0, _0223E8F8 ; =ov13_0224F5C8
	ldr r0, [r0]
	add r0, r0, #0x24c
	add r0, r0, #0x400
	bl DWCi_BACKUPlRead
	bl OS_GetTick
	bl ov13_0223A7F4
	ldr r1, _0223E8F8 ; =ov13_0224F5C8
	ldr r1, [r1]
	str r0, [r1, #0xaa4]
	ldmia sp!, {r4, pc}
	.balign 4, 0
_0223E8F8: .word ov13_0224F5C8
	arm_func_end ov13_0223E80C

	arm_func_start ov13_0223E8FC
ov13_0223E8FC: ; 0x0223E8FC
	stmdb sp!, {r3, lr}
	ldr r0, _0223E9D4 ; =ov13_0224F5C8
	ldr r2, [r0]
	ldrb r1, [r2, #0xa90]
	cmp r1, #1
	cmpne r1, #0x14
	cmpne r1, #0x17
	cmpne r1, #0x1a
	cmpne r1, #0x1d
	bne _0223E944
	mov r1, #0x22
	strb r1, [r2, #0xa90]
	ldr r0, _0223E9D4 ; =ov13_0224F5C8
	mov r1, #0
	ldr r0, [r0]
	strb r1, [r0, #0xaac]
	mov r0, #1
	ldmia sp!, {r3, pc}
_0223E944:
	cmp r1, #4
	cmpne r1, #5
	cmpne r1, #6
	cmpne r1, #0xd
	bne _0223E998
	cmp r1, #4
	bne _0223E970
	ldr r0, [r2, #0xa98]
	cmp r0, #6
	movlo r0, #0
	ldmloia sp!, {r3, pc}
_0223E970:
	bl MB_End
	ldr r0, _0223E9D4 ; =ov13_0224F5C8
	mov r3, #0x10
	ldr r1, [r0]
	mov r2, #2
	strb r3, [r1, #0xa90]
	ldr r1, [r0]
	mov r0, #1
	strb r2, [r1, #0xaac]
	ldmia sp!, {r3, pc}
_0223E998:
	add r0, r1, #0xf7
	and r0, r0, #0xff
	cmp r0, #1
	bhi _0223E9B8
	mov r0, #0x20
	strb r0, [r2, #0xa90]
	mov r0, #1
	ldmia sp!, {r3, pc}
_0223E9B8:
	cmp r1, #0xc
	movne r0, #0
	ldmneia sp!, {r3, pc}
	mov r0, #0x22
	strb r0, [r2, #0xa90]
	mov r0, #1
	ldmia sp!, {r3, pc}
	.balign 4, 0
_0223E9D4: .word ov13_0224F5C8
	arm_func_end ov13_0223E8FC

	arm_func_start ov13_0223E9D8
ov13_0223E9D8: ; 0x0223E9D8
	stmdb sp!, {r3, lr}
	ldr r0, _0223EA54 ; =ov13_0224F5C8
	ldr r0, [r0]
	ldr r0, [r0, #0xac8]
	bl ov13_0223DF00
	ldr r0, _0223EA54 ; =ov13_0224F5C8
	mov r1, #1
	ldr r0, [r0]
	strb r1, [r0, #0xa90]
	bl WM_GetNextTgid
	ldr r3, _0223EA54 ; =ov13_0224F5C8
	mov r2, #0x40
	ldr r1, [r3]
	add r1, r1, #0x600
	strh r0, [r1, #0x48]
	ldr r1, [r3]
	ldr r0, [r1, #0xaa4]
	add r1, r1, #0xa50
	bl MI_CpuCopy8
	ldr r0, _0223EA54 ; =ov13_0224F5C8
	mov r2, #0
	ldr r1, [r0]
	strb r2, [r1, #0xa93]
	ldr r1, [r0]
	str r2, [r1, #0x204]
	ldr r0, [r0]
	add r0, r0, #0x600
	ldrh r1, [r0, #0x48]
	add r1, r1, #1
	strh r1, [r0, #0x48]
	ldmia sp!, {r3, pc}
	.balign 4, 0
_0223EA54: .word ov13_0224F5C8
	arm_func_end ov13_0223E9D8

	arm_func_start ov13_0223EA58
ov13_0223EA58: ; 0x0223EA58
	stmdb sp!, {r3, lr}
	ldr r0, _0223EA9C ; =ov13_0224F5C8
	ldr r0, [r0]
	ldrb r0, [r0, #0xa90]
	cmp r0, #1
	cmpne r0, #0x1a
	cmpne r0, #0x1d
	movne r0, #0
	ldmneia sp!, {r3, pc}
	bl ov13_0223E9D8
	bl ov13_0223E308
	ldr r0, _0223EA9C ; =ov13_0224F5C8
	mov r2, #2
	ldr r1, [r0]
	mov r0, #1
	strb r2, [r1, #0xa90]
	ldmia sp!, {r3, pc}
	.balign 4, 0
_0223EA9C: .word ov13_0224F5C8
	arm_func_end ov13_0223EA58

	arm_func_start ov13_0223EAA0
ov13_0223EAA0: ; 0x0223EAA0
	stmdb sp!, {r3, lr}
	ldr r0, _0223EAD0 ; =ov13_0224F5C8
	ldr r1, [r0]
	ldrb r0, [r1, #0xa90]
	cmp r0, #5
	movne r0, #0
	ldmneia sp!, {r3, pc}
	mov r0, #6
	strb r0, [r1, #0xa90]
	bl ov13_0223CEA0
	mov r0, #1
	ldmia sp!, {r3, pc}
	.balign 4, 0
_0223EAD0: .word ov13_0224F5C8
	arm_func_end ov13_0223EAA0

	arm_func_start ov13_0223EAD4
ov13_0223EAD4: ; 0x0223EAD4
	stmdb sp!, {r3, lr}
	ldr r1, _0223EE50 ; =ov13_0224F5C8
	ldr r0, [r1]
	ldrb r2, [r0, #0xa90]
	cmp r2, #0x22
	addls pc, pc, r2, lsl #2
	ldmia sp!, {r3, pc}
_0223EAF0: ; jump table
	ldmia sp!, {r3, pc} ; case 0
	b _0223EB7C ; case 1
	b _0223EBB8 ; case 2
	b _0223EBC0 ; case 3
	b _0223EBD8 ; case 4
	b _0223EBEC ; case 5
	b _0223EBEC ; case 6
	b _0223EBF4 ; case 7
	b _0223EBFC ; case 8
	b _0223EBFC ; case 9
	b _0223EBFC ; case 10
	b _0223EC04 ; case 11
	ldmia sp!, {r3, pc} ; case 12
	ldmia sp!, {r3, pc} ; case 13
	ldmia sp!, {r3, pc} ; case 14
	ldmia sp!, {r3, pc} ; case 15
	b _0223EC0C ; case 16
	b _0223EC24 ; case 17
	b _0223EC40 ; case 18
	b _0223EC64 ; case 19
	ldmia sp!, {r3, pc} ; case 20
	b _0223EC9C ; case 21
	b _0223ECC0 ; case 22
	ldmia sp!, {r3, pc} ; case 23
	b _0223ECF8 ; case 24
	b _0223ED1C ; case 25
	ldmia sp!, {r3, pc} ; case 26
	b _0223ED54 ; case 27
	b _0223ED78 ; case 28
	ldmia sp!, {r3, pc} ; case 29
	b _0223EE48 ; case 30
	ldmia sp!, {r3, pc} ; case 31
	b _0223EDEC ; case 32
	b _0223EE10 ; case 33
	ldmia sp!, {r3, pc} ; case 34
_0223EB7C:
	ldrb r2, [r0, #0xaac]
	cmp r2, #1
	bne _0223EB98
	mov r1, #0
	strb r1, [r0, #0xaac]
	bl ov13_0223EA58
	ldmia sp!, {r3, pc}
_0223EB98:
	cmp r2, #2
	ldmneia sp!, {r3, pc}
	mov r2, #0
	strb r2, [r0, #0xaac]
	ldr r0, [r1]
	mov r1, #0x22
	strb r1, [r0, #0xa90]
	ldmia sp!, {r3, pc}
_0223EBB8:
	bl ov13_0223EEAC
	ldmia sp!, {r3, pc}
_0223EBC0:
	bl ov13_0223EF54
	ldr r0, _0223EE50 ; =ov13_0224F5C8
	mov r1, #4
	ldr r0, [r0]
	strb r1, [r0, #0xa90]
	ldmia sp!, {r3, pc}
_0223EBD8:
	ldr r1, [r0, #0xa98]
	add r1, r1, #1
	str r1, [r0, #0xa98]
	bl ov13_0223EF78
	ldmia sp!, {r3, pc}
_0223EBEC:
	bl ov13_0223EF78
	ldmia sp!, {r3, pc}
_0223EBF4:
	bl ov13_0223F130
	ldmia sp!, {r3, pc}
_0223EBFC:
	bl ov13_0223F15C
	ldmia sp!, {r3, pc}
_0223EC04:
	bl ov13_0223F338
	ldmia sp!, {r3, pc}
_0223EC0C:
	mov r2, #0
	str r2, [r0, #0xa9c]
	ldr r0, [r1]
	mov r1, #0x11
	strb r1, [r0, #0xa90]
	ldmia sp!, {r3, pc}
_0223EC24:
	ldr r2, [r0, #0xa9c]
	add r1, r2, #1
	str r1, [r0, #0xa9c]
	cmp r2, #0x1e
	ldmlsia sp!, {r3, pc}
	bl ov13_0223F350
	ldmia sp!, {r3, pc}
_0223EC40:
	bl MB_End
	ldr r0, _0223EE50 ; =ov13_0224F5C8
	mov r3, #0
	ldr r2, [r0]
	mov r1, #0x16
	str r3, [r2, #0xa9c]
	ldr r0, [r0]
	strb r1, [r0, #0xa90]
	ldmia sp!, {r3, pc}
_0223EC64:
	ldr r2, [r0, #0xa9c]
	add r1, r2, #1
	str r1, [r0, #0xa9c]
	cmp r2, #0x1e
	ldmlsia sp!, {r3, pc}
	bl ov13_0223DF28
	cmp r0, #1
	ldmneia sp!, {r3, pc}
	bl ov13_0223E7BC
	ldr r0, _0223EE50 ; =ov13_0224F5C8
	mov r1, #0x14
	ldr r0, [r0]
	strb r1, [r0, #0xa90]
	ldmia sp!, {r3, pc}
_0223EC9C:
	bl ov13_0223E6B4
	ldr r0, _0223EE50 ; =ov13_0224F5C8
	mov r3, #0
	ldr r2, [r0]
	mov r1, #0x16
	str r3, [r2, #0xa9c]
	ldr r0, [r0]
	strb r1, [r0, #0xa90]
	ldmia sp!, {r3, pc}
_0223ECC0:
	ldr r2, [r0, #0xa9c]
	add r1, r2, #1
	str r1, [r0, #0xa9c]
	cmp r2, #0x1e
	ldmlsia sp!, {r3, pc}
	bl ov13_0223DF28
	cmp r0, #1
	ldmneia sp!, {r3, pc}
	bl ov13_0223E7BC
	ldr r0, _0223EE50 ; =ov13_0224F5C8
	mov r1, #0x17
	ldr r0, [r0]
	strb r1, [r0, #0xa90]
	ldmia sp!, {r3, pc}
_0223ECF8:
	bl ov13_0223E6B4
	ldr r0, _0223EE50 ; =ov13_0224F5C8
	mov r3, #0
	ldr r2, [r0]
	mov r1, #0x19
	str r3, [r2, #0xa9c]
	ldr r0, [r0]
	strb r1, [r0, #0xa90]
	ldmia sp!, {r3, pc}
_0223ED1C:
	ldr r2, [r0, #0xa9c]
	add r1, r2, #1
	str r1, [r0, #0xa9c]
	cmp r2, #0x1e
	ldmlsia sp!, {r3, pc}
	bl ov13_0223DF28
	cmp r0, #1
	ldmneia sp!, {r3, pc}
	bl ov13_0223E7BC
	ldr r0, _0223EE50 ; =ov13_0224F5C8
	mov r1, #0x1a
	ldr r0, [r0]
	strb r1, [r0, #0xa90]
	ldmia sp!, {r3, pc}
_0223ED54:
	bl ov13_0223E6B4
	ldr r0, _0223EE50 ; =ov13_0224F5C8
	mov r3, #0
	ldr r2, [r0]
	mov r1, #0x1c
	str r3, [r2, #0xa9c]
	ldr r0, [r0]
	strb r1, [r0, #0xa90]
	ldmia sp!, {r3, pc}
_0223ED78:
	ldr r2, [r0, #0xa9c]
	add r1, r2, #1
	str r1, [r0, #0xa9c]
	cmp r2, #0x1e
	ldmlsia sp!, {r3, pc}
	bl ov13_0223DF28
	cmp r0, #1
	bne _0223EDB0
	bl ov13_0223E7BC
	ldr r0, _0223EE50 ; =ov13_0224F5C8
	mov r1, #0x1d
	ldr r0, [r0]
	strb r1, [r0, #0xa90]
	ldmia sp!, {r3, pc}
_0223EDB0:
	ldr r1, _0223EE50 ; =ov13_0224F5C8
	ldr r0, _0223EE54 ; =0x88888889
	ldr r1, [r1]
	mov r2, #0x1e
	ldr r3, [r1, #0xa9c]
	umull r0, r1, r3, r0
	mov r1, r1, lsr #4
	umull r0, r1, r2, r1
	sub r1, r3, r0
	cmp r1, #1
	ldmneia sp!, {r3, pc}
	cmp r3, #0x37
	ldmlsia sp!, {r3, pc}
	bl ov13_0223E6B4
	ldmia sp!, {r3, pc}
_0223EDEC:
	bl ov13_0223E6B4
	ldr r0, _0223EE50 ; =ov13_0224F5C8
	mov r3, #0
	ldr r2, [r0]
	mov r1, #0x21
	str r3, [r2, #0xa9c]
	ldr r0, [r0]
	strb r1, [r0, #0xa90]
	ldmia sp!, {r3, pc}
_0223EE10:
	ldr r2, [r0, #0xa9c]
	add r1, r2, #1
	str r1, [r0, #0xa9c]
	cmp r2, #0x1e
	ldmlsia sp!, {r3, pc}
	bl ov13_0223DF28
	cmp r0, #1
	ldmneia sp!, {r3, pc}
	bl ov13_0223E7BC
	ldr r0, _0223EE50 ; =ov13_0224F5C8
	mov r1, #0x22
	ldr r0, [r0]
	strb r1, [r0, #0xa90]
	ldmia sp!, {r3, pc}
_0223EE48:
	bl ov13_0223E6B4
	ldmia sp!, {r3, pc}
	.balign 4, 0
_0223EE50: .word ov13_0224F5C8
_0223EE54: .word 0x88888889
	arm_func_end ov13_0223EAD4

	arm_func_start ov13_0223EE58
ov13_0223EE58: ; 0x0223EE58
	ldr r2, _0223EE98 ; =ov13_0224F5C8
	ldr r3, [r2]
	ldrb r3, [r3, #0xa90]
	strb r3, [r0]
	ldr r0, [r2]
	ldrb r2, [r0, #0xa90]
	ldrb r0, [r0, #0xa91]
	cmp r2, r0
	movne r0, #1
	moveq r0, #0
	strb r0, [r1]
	ldr r0, _0223EE98 ; =ov13_0224F5C8
	ldr r1, [r0]
	ldrb r0, [r1, #0xa90]
	strb r0, [r1, #0xa91]
	bx lr
	.balign 4, 0
_0223EE98: .word ov13_0224F5C8
	arm_func_end ov13_0223EE58

	arm_func_start ov13_0223EE9C
ov13_0223EE9C: ; 0x0223EE9C
	ldr ip, _0223EEA8 ; =MB_CommGetChildUser
	mov r0, #1
	bx ip
	.balign 4, 0
_0223EEA8: .word MB_CommGetChildUser
	arm_func_end ov13_0223EE9C

	arm_func_start ov13_0223EEAC
ov13_0223EEAC: ; 0x0223EEAC
	stmdb sp!, {r3, lr}
	bl ov13_0223DF28
	cmp r0, #9
	addls pc, pc, r0, lsl #2
	b _0223EF48
_0223EEC0: ; jump table
	b _0223EF20 ; case 0
	b _0223EEE8 ; case 1
	b _0223EF48 ; case 2
	ldmia sp!, {r3, pc} ; case 3
	b _0223EF48 ; case 4
	b _0223EF48 ; case 5
	b _0223EF48 ; case 6
	b _0223EEF0 ; case 7
	b _0223EF48 ; case 8
	b _0223EF40 ; case 9
_0223EEE8:
	bl ov13_0223DF3C
	ldmia sp!, {r3, pc}
_0223EEF0:
	bl ov13_0223E1A8
	ldr r2, _0223EF50 ; =ov13_0224F5C8
	mov ip, #0
	ldr r1, [r2]
	mov r3, #3
	add r1, r1, #0x600
	strh r0, [r1, #0x4a]
	ldr r0, [r2]
	str ip, [r0, #0xa98]
	ldr r0, [r2]
	strb r3, [r0, #0xa90]
	ldmia sp!, {r3, pc}
_0223EF20:
	ldr r0, _0223EF50 ; =ov13_0224F5C8
	mov r3, #0
	ldr r2, [r0]
	mov r1, #3
	str r3, [r2, #0xa98]
	ldr r0, [r0]
	strb r1, [r0, #0xa90]
	ldmia sp!, {r3, pc}
_0223EF40:
	bl ov13_0223E698
	ldmia sp!, {r3, pc}
_0223EF48:
	bl OS_Terminate
	ldmia sp!, {r3, pc}
	.balign 4, 0
_0223EF50: .word ov13_0224F5C8
	arm_func_end ov13_0223EEAC

	arm_func_start ov13_0223EF54
ov13_0223EF54: ; 0x0223EF54
	ldr r0, _0223EF70 ; =ov13_0224F5C8
	ldr ip, _0223EF74 ; =ov13_0223CA14
	ldr r2, [r0]
	add r0, r2, #0x600
	ldrh r1, [r0, #0x48]
	ldr r0, [r2, #0xac8]
	bx ip
	.balign 4, 0
_0223EF70: .word ov13_0224F5C8
_0223EF74: .word ov13_0223CA14
	arm_func_end ov13_0223EF54

	arm_func_start ov13_0223EF78
ov13_0223EF78: ; 0x0223EF78
	stmdb sp!, {r3, lr}
	bl ov13_0223D45C
	cmp r0, #7
	addls pc, pc, r0, lsl #2
	ldmia sp!, {r3, pc}
_0223EF8C: ; jump table
	b _0223F0DC ; case 0
	b _0223EFAC ; case 1
	b _0223EFCC ; case 2
	b _0223F05C ; case 3
	ldmia sp!, {r3, pc} ; case 4
	b _0223F0B0 ; case 5
	ldmia sp!, {r3, pc} ; case 6
	b _0223F0C4 ; case 7
_0223EFAC:
	ldr r0, _0223F12C ; =ov13_0224F5C8
	ldr r2, [r0]
	add r0, r2, #0x600
	ldrh r1, [r0, #0x4a]
	add r0, r2, #0x2b4
	add r0, r0, #0x800
	bl ov13_0223CB34
	ldmia sp!, {r3, pc}
_0223EFCC:
	mov r0, #2
	bl ov13_0223D470
	cmp r0, #0
	beq _0223EFF0
	ldr r0, _0223F12C ; =ov13_0224F5C8
	mov r1, #5
	ldr r0, [r0]
	strb r1, [r0, #0xa90]
	ldmia sp!, {r3, pc}
_0223EFF0:
	mov r0, #3
	bl ov13_0223D470
	cmp r0, #0
	bne _0223F010
	mov r0, #4
	bl ov13_0223D470
	cmp r0, #0
	beq _0223F024
_0223F010:
	ldr r0, _0223F12C ; =ov13_0224F5C8
	mov r1, #6
	ldr r0, [r0]
	strb r1, [r0, #0xa90]
	ldmia sp!, {r3, pc}
_0223F024:
	ldr r0, _0223F12C ; =ov13_0224F5C8
	ldr r0, [r0]
	ldrb r0, [r0, #0xa90]
	cmp r0, #5
	ldmneia sp!, {r3, pc}
	mov r0, #2
	bl ov13_0223D470
	cmp r0, #0
	ldmneia sp!, {r3, pc}
	ldr r0, _0223F12C ; =ov13_0224F5C8
	mov r1, #0xd
	ldr r0, [r0]
	strb r1, [r0, #0xa90]
	ldmia sp!, {r3, pc}
_0223F05C:
	bl ov13_0223CF80
	cmp r0, #0
	beq _0223F070
	bl ov13_0223CFE8
	ldmia sp!, {r3, pc}
_0223F070:
	ldr r0, _0223F12C ; =ov13_0224F5C8
	ldr r0, [r0]
	ldrb r0, [r0, #0xa90]
	add r0, r0, #0xfa
	and r0, r0, #0xff
	cmp r0, #1
	ldmhiia sp!, {r3, pc}
	mov r0, #3
	bl ov13_0223D470
	cmp r0, #0
	ldmneia sp!, {r3, pc}
	ldr r0, _0223F12C ; =ov13_0224F5C8
	mov r1, #0x12
	ldr r0, [r0]
	strb r1, [r0, #0xa90]
	ldmia sp!, {r3, pc}
_0223F0B0:
	ldr r0, _0223F12C ; =ov13_0224F5C8
	mov r1, #7
	ldr r0, [r0]
	strb r1, [r0, #0xa90]
	ldmia sp!, {r3, pc}
_0223F0C4:
	bl ov13_0223D0E0
	ldr r0, _0223F12C ; =ov13_0224F5C8
	mov r1, #1
	ldr r0, [r0]
	strb r1, [r0, #0xa90]
	ldmia sp!, {r3, pc}
_0223F0DC:
	bl ov13_0223DF28
	cmp r0, #0
	beq _0223F104
	cmp r0, #1
	beq _0223F0FC
	cmp r0, #3
	ldmeqia sp!, {r3, pc}
	b _0223F118
_0223F0FC:
	bl ov13_0223E7BC
	ldmia sp!, {r3, pc}
_0223F104:
	ldr r0, _0223F12C ; =ov13_0224F5C8
	mov r1, #0x1f
	ldr r0, [r0]
	strb r1, [r0, #0xa90]
	ldmia sp!, {r3, pc}
_0223F118:
	ldr r0, _0223F12C ; =ov13_0224F5C8
	mov r1, #0x1f
	ldr r0, [r0]
	strb r1, [r0, #0xa90]
	ldmia sp!, {r3, pc}
	.balign 4, 0
_0223F12C: .word ov13_0224F5C8
	arm_func_end ov13_0223EF78

	arm_func_start ov13_0223F130
ov13_0223F130: ; 0x0223F130
	stmdb sp!, {r3, lr}
	bl ov13_0223F3BC
	ldr r0, _0223F154 ; =ov13_0223F380
	bl ov13_0223E5B4
	ldr r0, _0223F158 ; =ov13_0224F5C8
	mov r1, #8
	ldr r0, [r0]
	strb r1, [r0, #0xa90]
	ldmia sp!, {r3, pc}
	.balign 4, 0
_0223F154: .word ov13_0223F380
_0223F158: .word ov13_0224F5C8
	arm_func_end ov13_0223F130

	arm_func_start ov13_0223F15C
ov13_0223F15C: ; 0x0223F15C
	stmdb sp!, {r3, lr}
	bl ov13_0223DF28
	cmp r0, #6
	addls pc, pc, r0, lsl #2
	ldmia sp!, {r3, pc}
_0223F170: ; jump table
	ldmia sp!, {r3, pc} ; case 0
	b _0223F18C ; case 1
	ldmia sp!, {r3, pc} ; case 2
	ldmia sp!, {r3, pc} ; case 3
	b _0223F1B8 ; case 4
	b _0223F1B8 ; case 5
	b _0223F1B8 ; case 6
_0223F18C:
	ldr r1, _0223F334 ; =ov13_0224F5C8
	mov r0, #4
	ldr r1, [r1]
	add r1, r1, #0x600
	ldrh r3, [r1, #0x48]
	ldrh r2, [r1, #0x4a]
	add r1, r3, #1
	mov r1, r1, lsl #0x10
	mov r1, r1, lsr #0x10
	bl ov13_0223E43C
	ldmia sp!, {r3, pc}
_0223F1B8:
	ldr r1, _0223F334 ; =ov13_0224F5C8
	mov r0, #0
	ldr r2, [r1]
	add r1, r2, #0x200
	ldrh r1, [r1]
	ldr ip, [r2, #0xaa4]
	mov r3, r1, lsr #0x1f
	rsb r2, r3, r1, lsl #28
	add r2, r3, r2, ror #28
	add r2, ip, r2, lsl #6
	bl ov13_0223F400
	bl ov13_0223F568
	ldr r0, _0223F334 ; =ov13_0224F5C8
	ldr r1, [r0]
	ldrb r0, [r1, #0xa90]
	cmp r0, #0x1b
	ldmeqia sp!, {r3, pc}
	ldr r0, [r1, #0x204]
	cmp r0, #0x1e0
	movhi r0, #0x1b
	strhib r0, [r1, #0xa90]
	ldmhiia sp!, {r3, pc}
	bl ov13_0223F64C
	cmp r0, #0x10
	beq _0223F228
	bl ov13_0223F64C
	cmp r0, #0x20
	bne _0223F240
_0223F228:
	bl ov13_0223F64C
	ldr r0, _0223F334 ; =ov13_0224F5C8
	mov r1, #0xa
	ldr r0, [r0]
	strb r1, [r0, #0xa90]
	ldmia sp!, {r3, pc}
_0223F240:
	bl ov13_0223F64C
	cmp r0, #0x40
	bne _0223F260
	ldr r0, _0223F334 ; =ov13_0224F5C8
	mov r1, #0xb
	ldr r0, [r0]
	strb r1, [r0, #0xa90]
	ldmia sp!, {r3, pc}
_0223F260:
	bl ov13_0223F64C
	cmp r0, #0xff
	bne _0223F280
	ldr r0, _0223F334 ; =ov13_0224F5C8
	mov r1, #0x1b
	ldr r0, [r0]
	strb r1, [r0, #0xa90]
	ldmia sp!, {r3, pc}
_0223F280:
	bl ov13_0223F64C
	cmp r0, #0x50
	bne _0223F2A0
	ldr r0, _0223F334 ; =ov13_0224F5C8
	mov r1, #0x15
	ldr r0, [r0]
	strb r1, [r0, #0xa90]
	ldmia sp!, {r3, pc}
_0223F2A0:
	bl ov13_0223F64C
	cmp r0, #0x60
	bne _0223F2C0
	ldr r0, _0223F334 ; =ov13_0224F5C8
	mov r1, #0x18
	ldr r0, [r0]
	strb r1, [r0, #0xa90]
	ldmia sp!, {r3, pc}
_0223F2C0:
	bl ov13_0223F64C
	cmp r0, #0x70
	bne _0223F2E0
	ldr r0, _0223F334 ; =ov13_0224F5C8
	mov r1, #0x1b
	ldr r0, [r0]
	strb r1, [r0, #0xa90]
	ldmia sp!, {r3, pc}
_0223F2E0:
	bl ov13_0223F64C
	cmp r0, #0
	bne _0223F300
	ldr r0, _0223F334 ; =ov13_0224F5C8
	mov r1, #8
	ldr r0, [r0]
	strb r1, [r0, #0xa90]
	ldmia sp!, {r3, pc}
_0223F300:
	bl ov13_0223F64C
	cmp r0, #0xbd
	bne _0223F320
	ldr r0, _0223F334 ; =ov13_0224F5C8
	mov r1, #9
	ldr r0, [r0]
	strb r1, [r0, #0xa90]
	ldmia sp!, {r3, pc}
_0223F320:
	ldr r0, _0223F334 ; =ov13_0224F5C8
	mov r1, #0x1f
	ldr r0, [r0]
	strb r1, [r0, #0xa90]
	ldmia sp!, {r3, pc}
	.balign 4, 0
_0223F334: .word ov13_0224F5C8
	arm_func_end ov13_0223F15C

	arm_func_start ov13_0223F338
ov13_0223F338: ; 0x0223F338
	ldr r0, _0223F34C ; =ov13_0224F5C8
	mov r1, #0xc
	ldr r0, [r0]
	strb r1, [r0, #0xa90]
	bx lr
	.balign 4, 0
_0223F34C: .word ov13_0224F5C8
	arm_func_end ov13_0223F338

	arm_func_start ov13_0223F350
ov13_0223F350: ; 0x0223F350
	stmdb sp!, {r3, lr}
	bl ov13_0223DF28
	cmp r0, #1
	movne r0, #0
	ldmneia sp!, {r3, pc}
	bl ov13_0223E7BC
	ldr r1, _0223F37C ; =ov13_0224F5C8
	mov r0, #1
	ldr r1, [r1]
	strb r0, [r1, #0xa90]
	ldmia sp!, {r3, pc}
	.balign 4, 0
_0223F37C: .word ov13_0224F5C8
	arm_func_end ov13_0223F350

	arm_func_start ov13_0223F380
ov13_0223F380: ; 0x0223F380
	stmdb sp!, {r4, lr}
	add r0, r0, #0xa
	bl ov13_0223D5C4
	movs r4, r0
	moveq r0, #0
	ldmeqia sp!, {r4, pc}
	bl ov13_0223D590
	ldr r1, _0223F3B8 ; =ov13_0224F5C8
	sub r2, r4, #1
	ldr r1, [r1]
	add r1, r1, r2, lsl #2
	str r0, [r1, #0xaa0]
	mov r0, #1
	ldmia sp!, {r4, pc}
	.balign 4, 0
_0223F3B8: .word ov13_0224F5C8
	arm_func_end ov13_0223F380

	arm_func_start ov13_0223F3BC
ov13_0223F3BC: ; 0x0223F3BC
	stmdb sp!, {r3, lr}
	ldr r0, _0223F3FC ; =ov13_0224F5C8
	mov r1, #0
	ldr r0, [r0]
	mov r2, #0x100
	add r0, r0, #0x100
	bl MI_CpuFill8
	ldr r0, _0223F3FC ; =ov13_0224F5C8
	mov r1, #0
	ldr r0, [r0]
	mov r2, #0x100
	bl MI_CpuFill8
	ldr r0, _0223F3FC ; =ov13_0224F5C8
	ldr r0, [r0]
	str r0, [r0, #0xab0]
	ldmia sp!, {r3, pc}
	.balign 4, 0
_0223F3FC: .word ov13_0224F5C8
	arm_func_end ov13_0223F3BC

	arm_func_start ov13_0223F400
ov13_0223F400: ; 0x0223F400
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, lr}
	ldr r3, _0223F564 ; =ov13_0224F5C8
	ldr r5, [r3]
	ldrb r4, [r5, #0xa93]
	cmp r4, #1
	bne _0223F448
	ldr r4, [r5, #0xab0]
	strh r0, [r4]
	ldr r4, [r3]
	mov r0, r2
	ldr r4, [r4, #0xab0]
	mov r2, #0x40
	strh r1, [r4, #2]
	ldr r1, [r3]
	ldr r1, [r1, #0xab0]
	add r1, r1, #4
	bl MI_CpuCopy8
	b _0223F474
_0223F448:
	ldr r0, [r5, #0x204]
	mov r1, #0xbc
	add r0, r0, #1
	str r0, [r5, #0x204]
	ldr r0, [r3]
	ldr r0, [r0, #0xab0]
	strh r1, [r0]
	ldr r0, [r3]
	ldrb r1, [r0, #0xa92]
	ldr r0, [r0, #0xab0]
	strb r1, [r0, #4]
_0223F474:
	bl ov13_0223DF28
	cmp r0, #5
	ldmneia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	ldr r0, _0223F564 ; =ov13_0224F5C8
	ldr r0, [r0]
	bl ov13_0223E5F0
	cmp r0, #0
	bne _0223F4AC
	ldr r0, _0223F564 ; =ov13_0224F5C8
	ldr r1, [r0]
	ldr r0, [r1, #0x204]
	add r0, r0, #4
	str r0, [r1, #0x204]
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
_0223F4AC:
	ldr r0, _0223F564 ; =ov13_0224F5C8
	ldr r1, [r0]
	ldrb r0, [r1, #0xa93]
	cmp r0, #0
	bne _0223F4D0
	ldr r0, [r1, #0x204]
	add r0, r0, #1
	str r0, [r1, #0x204]
	b _0223F4F8
_0223F4D0:
	mov r0, #0
	str r0, [r1, #0x204]
	bl ov13_0223DF14
	cmp r0, #3
	beq _0223F4F8
	ldr r0, _0223F564 ; =ov13_0224F5C8
	mov r1, #0x1b
	ldr r0, [r0]
	strb r1, [r0, #0xa90]
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
_0223F4F8:
	mov r7, #0
	mov r6, #0x44
	ldr sb, _0223F564 ; =ov13_0224F5C8
	mov r4, r7
	mov r5, #1
	mov r8, r6
_0223F510:
	mov r0, r7
	bl ov13_0223E5C8
	cmp r0, #0
	ldreq r0, [sb]
	addeq r0, r0, r7, lsl #2
	streq r4, [r0, #0x208]
	beq _0223F54C
	ldr r1, [sb]
	mov r2, r6
	add r1, r1, #0x100
	mla r1, r7, r8, r1
	bl MI_CpuCopy8
	ldr r0, [sb]
	add r0, r0, r7, lsl #2
	str r5, [r0, #0x208]
_0223F54C:
	add r0, r7, #1
	mov r0, r0, lsl #0x10
	mov r7, r0, lsr #0x10
	cmp r7, #2
	blo _0223F510
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	.balign 4, 0
_0223F564: .word ov13_0224F5C8
	arm_func_end ov13_0223F400

	arm_func_start ov13_0223F568
ov13_0223F568: ; 0x0223F568
	stmdb sp!, {r4, r5, r6, r7, r8, lr}
	mov ip, #0
	ldr r4, _0223F648 ; =ov13_0224F5C8
	mov r0, #1
	mov r5, ip
	mov r1, #0xbc
	mov r2, ip
	mov lr, #0x44
_0223F588:
	ldr r3, [r4]
	add r6, r3, ip, lsl #2
	ldr r6, [r6, #0x208]
	cmp r6, #0
	beq _0223F638
	mul r7, ip, lr
	cmp ip, #1
	add r8, r3, #0x100
	bne _0223F638
	ldrb r6, [r3, #0xa93]
	cmp r6, #1
	bne _0223F608
	ldrh r6, [r8, r7]
	cmp r6, #0x10
	ldmneia sp!, {r4, r5, r6, r7, r8, pc}
	ldr r6, [r3, #0xa94]
	add r6, r6, #1
	str r6, [r3, #0xa94]
	ldr r6, [r4]
	ldr r3, [r6, #0xa94]
	tst r3, #1
	bne _0223F638
	add r3, r6, #0x200
	ldrh r6, [r3]
	add r6, r6, #1
	strh r6, [r3]
	ldr r3, [r4]
	add r3, r3, #0x200
	ldrh r6, [r3]
	cmp r6, #0x24
	strhsh r2, [r3]
	b _0223F638
_0223F608:
	add r3, r3, #0x200
	strh r1, [r3, #2]
	ldrh r3, [r8, r7]
	cmp r3, #0xbd
	bne _0223F638
	ldr r3, [r4]
	strb r0, [r3, #0xa93]
	ldr r3, [r4]
	add r3, r3, #0x200
	strh r5, [r3]
	ldr r3, [r4]
	str r5, [r3, #0xa94]
_0223F638:
	add ip, ip, #1
	cmp ip, #0x10
	blt _0223F588
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	.balign 4, 0
_0223F648: .word ov13_0224F5C8
	arm_func_end ov13_0223F568

	arm_func_start ov13_0223F64C
ov13_0223F64C: ; 0x0223F64C
	ldr r0, _0223F660 ; =ov13_0224F5C8
	ldr r0, [r0]
	add r0, r0, #0x100
	ldrh r0, [r0, #0x44]
	bx lr
	.balign 4, 0
_0223F660: .word ov13_0224F5C8
	arm_func_end ov13_0223F64C

	arm_func_start ov13_0223F664
ov13_0223F664: ; 0x0223F664
	stmdb sp!, {r4, lr}
	sub sp, sp, #0xe8
	mov r0, #0xe8
	mov r1, #4
	bl ov13_0224128C
	ldr r1, _0223F7F4 ; =ov13_0224F5CC
	str r0, [r1]
	add r0, sp, #0xa0
	bl FS_InitFile
	ldr r1, _0223F7F8 ; =ov13_02247404
	add r0, sp, #0xa0
	bl FS_OpenFile
	cmp r0, #0
	bne _0223F6A0
	bl OS_Terminate
_0223F6A0:
	bl OS_GetLockID
	ldr r2, _0223F7F4 ; =ov13_0224F5CC
	add r1, sp, #0x18
	ldr r3, [r2]
	mov r2, #8
	strh r0, [r3, #0xe4]
	add r0, sp, #0xa0
	ldr r4, [sp, #0xc4]
	bl FS_ReadFile
	add r0, sp, #0xa0
	add r1, sp, #0x10
	mov r2, #8
	bl FS_ReadFile
	add r0, sp, #0xa0
	bl FS_CloseFile
	ldr r0, _0223F7F4 ; =ov13_0224F5CC
	ldr r0, [r0]
	add r0, r0, #0x88
	bl FS_InitArchive
	ldr r0, _0223F7F4 ; =ov13_0224F5CC
	ldr r1, _0223F7FC ; =ov13_0224593C
	ldr r0, [r0]
	mov r2, #3
	add r0, r0, #0x88
	bl FS_RegisterArchiveName
	cmp r0, #0
	bne _0223F710
	bl OS_Terminate
_0223F710:
	ldr r0, _0223F7F4 ; =ov13_0224F5CC
	ldr r1, _0223F800 ; =ov13_0223F8A0
	ldr r0, [r0]
	ldr r2, _0223F804 ; =0x00000602
	add r0, r0, #0x88
	bl FS_SetArchiveProc
	ldr r0, [sp, #0x18]
	ldr r1, _0223F808 ; =ov13_0223F904
	str r0, [sp]
	ldr r2, [sp, #0x1c]
	ldr r0, _0223F80C ; =ov13_0223F954
	str r2, [sp, #4]
	str r1, [sp, #8]
	str r0, [sp, #0xc]
	ldr r0, _0223F7F4 ; =ov13_0224F5CC
	ldr r2, [sp, #0x10]
	ldr r0, [r0]
	ldr r3, [sp, #0x14]
	mov r1, r4
	add r0, r0, #0x88
	bl FS_LoadArchive
	cmp r0, #0
	bne _0223F770
	bl OS_Terminate
_0223F770:
	ldr r0, _0223F7F4 ; =ov13_0224F5CC
	mov r1, #0
	ldr r0, [r0]
	mov r2, r1
	add r0, r0, #0x88
	bl FS_LoadArchiveTables
	mov r1, #4
	mov r4, r0
	bl ov13_0224128C
	ldr r1, _0223F7F4 ; =ov13_0224F5CC
	mov r2, r4
	ldr r3, [r1]
	str r0, [r3]
	ldr r0, [r1]
	ldr r1, [r0], #0x88
	bl FS_LoadArchiveTables
	ldr r1, _0223F7F4 ; =ov13_0224F5CC
	mov r0, #0x20
	ldr r1, [r1]
	mov r2, #4
	add r1, r1, #4
	bl ov13_0224022C
	ldr r2, _0223F7F4 ; =ov13_0224F5CC
	ldr r1, _0223F810 ; =ov13_0224741C
	ldr r3, [r2]
	ldr r2, _0223F7FC ; =ov13_0224593C
	str r0, [r3, #0x84]
	add r0, sp, #0x20
	bl OS_SPrintf
	add r0, sp, #0x20
	bl FS_ChangeDir
	add sp, sp, #0xe8
	ldmia sp!, {r4, pc}
	.balign 4, 0
_0223F7F4: .word ov13_0224F5CC
_0223F7F8: .word ov13_02247404
_0223F7FC: .word ov13_0224593C
_0223F800: .word ov13_0223F8A0
_0223F804: .word 0x00000602
_0223F808: .word ov13_0223F904
_0223F80C: .word ov13_0223F954
_0223F810: .word ov13_0224741C
	arm_func_end ov13_0223F664
