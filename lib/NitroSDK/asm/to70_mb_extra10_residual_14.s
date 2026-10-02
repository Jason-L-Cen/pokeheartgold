	.extern _start_AutoloadDoneCallback
	.include "asm/macros.inc"
	.include "mb.inc"
	.include "global.inc"
	.public MBi_CheckWmErrcode
	.extern ov13_0224CF40

.text
	arm_func_start MBi_CheckWmErrcode
MBi_CheckWmErrcode: ; 0x022200FC
	stmdb sp!, {r3, lr}
	cmp r1, #2
	cmpne r1, #0
	ldmeqia sp!, {r3, pc}
	ldr r2, _02220130 ; =ov13_0224CF40
	strh r0, [sp]
	ldr r0, [r2, #8]
	strh r1, [sp, #2]
	ldr r2, [r0, #0x51c]
	add r1, sp, #0
	mov r0, #0xff
	blx r2
	ldmia sp!, {r3, pc}
	.balign 4, 0
_02220130: .word ov13_0224CF40
	arm_func_end MBi_CheckWmErrcode
