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

.text
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
