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
	.public ov13_0223E43C
	.extern ov13_0223D6A8
	.extern ov13_0223D744

.text
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
