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
	.public ov13_0223EEAC
	.extern ov13_0223DF28
	.extern ov13_0223DF3C
	.extern ov13_0223E1A8
	.extern ov13_0223E698

.text
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
