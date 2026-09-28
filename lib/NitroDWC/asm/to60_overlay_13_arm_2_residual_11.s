	.include "asm/macros.inc"
	.include "overlay_13_arm_2.inc"
	.include "global.inc"
	.public ov13_0223B0D8
	.extern ov13_0223B178
	.extern ov13_0223FF18
	.extern ov13_02240068
	.extern ov13_022425C8
	.extern ov13_0224F5AC

.text
	arm_func_start ov13_0223B0D8
ov13_0223B0D8: ; 0x0223B0D8
	stmdb sp!, {r4, lr}
	sub sp, sp, #8
	ldr r1, _0223B170 ; =ov13_0224F5AC
	mov r4, r0
	ldr r0, [r1, #4]
	add r2, sp, #4
	ldr r0, [r0, #0x10]
	add r3, sp, #0
	mov r1, #0
	bl ov13_02240068
	ldr r0, [sp, #4]
	add r2, r0, #8
	str r2, [sp, #4]
	cmp r2, #0xb0
	blt _0223B11C
	cmp r2, #0x100
	ble _0223B13C
_0223B11C:
	ldr r0, _0223B170 ; =ov13_0224F5AC
	mvn r1, #0
	ldr r0, [r0, #4]
	mov r3, #0x21
	ldr r0, [r0, #0x10]
	bl ov13_0223FF18
	add sp, sp, #8
	ldmia sp!, {r4, pc}
_0223B13C:
	ldr r0, _0223B170 ; =ov13_0224F5AC
	mov r2, #0xb0
	ldr r0, [r0, #4]
	str r2, [sp, #4]
	ldr r0, [r0, #0x10]
	sub r1, r2, #0xb1
	mov r3, #0x21
	bl ov13_0223FF18
	ldr r1, _0223B174 ; =ov13_0223B178
	mov r0, r4
	bl ov13_022425C8
	add sp, sp, #8
	ldmia sp!, {r4, pc}
	.balign 4, 0
_0223B170: .word ov13_0224F5AC
_0223B174: .word ov13_0223B178
	arm_func_end ov13_0223B0D8
