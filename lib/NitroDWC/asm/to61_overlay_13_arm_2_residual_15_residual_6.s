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
	.public ov13_0223DF3C
	.extern ov13_0223D6A8
	.extern ov13_0223D724
	.extern ov13_0223E014

.text
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
