	.include "asm/macros.inc"
	.include "overlay_13_arm_2.inc"
	.include "global.inc"
	.public ov13_0223C340
	.extern ov13_0223C73C
	.extern ov13_02240E74
	.extern ov13_02241694
	.extern ov13_022417DC
	.extern ov13_0224588A
	.extern ov13_02245894
	.extern ov13_02245895
	.extern ov13_022458A8
	.extern ov13_02245904
	.extern ov13_02245934
	.extern ov13_0224F5B8

.text
	arm_func_start ov13_0223C340
ov13_0223C340: ; 0x0223C340
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, lr}
	sub sp, sp, #8
	ldr r4, _0223C480 ; =ov13_0224F5B8
	ldr sb, _0223C484 ; =ov13_02245934
	ldr r0, [r4]
	mov sl, #0
	ldrb r1, [r0, #0x1c]
	ldrb r0, [sb, r1]
	cmp r0, #0
	ble _0223C3CC
	ldr r8, _0223C488 ; =ov13_02245904
	ldr r7, _0223C48C ; =ov13_0224588A
	ldr r6, _0223C490 ; =ov13_022458A8
	add r5, sp, #0
_0223C378:
	add r0, r7, r1, lsl #1
	ldrb r0, [sl, r0]
	add r3, r8, r1, lsl #3
	mov r2, r5
	add r0, r3, r0, lsl #2
	add r1, r6, r1, lsl #2
	bl ov13_02240E74
	mov r0, r5
	bl ov13_022417DC
	cmp r0, #0
	beq _0223C3B4
	ldr r0, _0223C480 ; =ov13_0224F5B8
	ldr r0, [r0]
	strb sl, [r0, #0x1b]
	b _0223C3CC
_0223C3B4:
	ldr r0, [r4]
	add sl, sl, #1
	ldrb r1, [r0, #0x1c]
	ldrb r0, [sb, r1]
	cmp sl, r0
	blt _0223C378
_0223C3CC:
	mov r0, #1
	bl ov13_02241694
	cmp r0, #0
	beq _0223C3F8
	ldr r0, _0223C480 ; =ov13_0224F5B8
	ldr r1, _0223C494 ; =ov13_02245894
	ldr r2, [r0]
	ldrb r0, [r2, #0x1c]
	mov r0, r0, lsl #1
	ldrsb r0, [r1, r0]
	strb r0, [r2, #0x1b]
_0223C3F8:
	mov r0, #2
	bl ov13_02241694
	cmp r0, #0
	beq _0223C424
	ldr r0, _0223C480 ; =ov13_0224F5B8
	ldr r1, _0223C498 ; =ov13_02245895
	ldr r2, [r0]
	ldrb r0, [r2, #0x1c]
	mov r0, r0, lsl #1
	ldrsb r0, [r1, r0]
	strb r0, [r2, #0x1b]
_0223C424:
	ldr r0, _0223C480 ; =ov13_0224F5B8
	ldr r2, _0223C484 ; =ov13_02245934
	ldr r4, [r0]
	mov r0, #0
	ldrb r1, [r4, #0x1c]
	ldrb r1, [r2, r1]
	cmp r1, #0
	ble _0223C470
	ldrsb r3, [r4, #0x1b]
_0223C448:
	cmp r0, r3
	bne _0223C45C
	bl ov13_0223C73C
	add sp, sp, #8
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
_0223C45C:
	ldrb r1, [r4, #0x1c]
	add r0, r0, #1
	ldrb r1, [r2, r1]
	cmp r0, r1
	blt _0223C448
_0223C470:
	mvn r0, #0
	strb r0, [r4, #0x1b]
	add sp, sp, #8
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
	.balign 4, 0
_0223C480: .word ov13_0224F5B8
_0223C484: .word ov13_02245934
_0223C488: .word ov13_02245904
_0223C48C: .word ov13_0224588A
_0223C490: .word ov13_022458A8
_0223C494: .word ov13_02245894
_0223C498: .word ov13_02245895
	arm_func_end ov13_0223C340
