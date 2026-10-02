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
	.include "asm/macros.inc"
	.include "overlay_13_arm_2.inc"
	.include "global.inc"
	.public ov13_0223DAFC
	.extern ov13_0223D6A8
	.extern ov13_0223D724
	.extern ov13_0223DC28

.text
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
