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
	.public ov13_0223D8F4
	.extern ov13_0223D6A8
	.extern ov13_0223D724
	.extern ov13_0223DA64

.text
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
