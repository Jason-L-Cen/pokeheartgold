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
	.extern ov13_0223DF28
	.extern ov13_0223E6B4
	.extern ov13_0223E7BC
	.extern ov13_0223EA58
	.extern ov13_0223EEAC
	.extern ov13_0223EF54
	.extern ov13_0223EF78
	.extern ov13_0223F130
	.extern ov13_0223F15C
	.extern ov13_0223F338
	.extern ov13_0223F350

.text
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
