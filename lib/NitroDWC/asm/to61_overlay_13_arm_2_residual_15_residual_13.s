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
	.public ov13_0223EF78
	.extern ov13_0223D45C
	.extern ov13_0223D470
	.extern ov13_0223DF28
	.extern ov13_0223E7BC

.text
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
