	.extern _start_AutoloadDoneCallback
	.include "asm/macros.inc"
	.include "mb.inc"
	.include "global.inc"
	.public MBi_CallReset
	.extern MBi_CheckWmErrcode
	.extern ov13_0224CF40

.text
	arm_func_start MBi_CallReset
MBi_CallReset: ; 0x0221FC78
	stmdb sp!, {r4, lr}
	ldr r0, _0221FCAC ; =ov13_0224CF40
	ldr r0, [r0, #8]
	ldr r0, [r0, #0x508]
	bl WM_Reset
	mov r4, r0
	mov r1, r4
	mov r0, #1
	bl MBi_CheckWmErrcode
	cmp r4, #2
	moveq r4, #0
	mov r0, r4
	ldmia sp!, {r4, pc}
	.balign 4, 0
_0221FCAC: .word ov13_0224CF40
	arm_func_end MBi_CallReset
