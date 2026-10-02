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
	.public ov13_0223E6B4
	.extern ov13_0223D6A8
	.extern ov13_0223DC64
	.extern ov13_0223DC94
	.extern ov13_0223DD64
	.extern ov13_0223DDB4
	.extern ov13_0223E698

.text
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
