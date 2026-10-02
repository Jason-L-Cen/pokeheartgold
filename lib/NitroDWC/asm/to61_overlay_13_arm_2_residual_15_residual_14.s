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
	.public ov13_0223F568

.text
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
