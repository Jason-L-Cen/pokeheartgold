	.include "asm/macros.inc"
	.include "fx_mtx33.inc"
	.include "global.inc"
	.text

	arm_func_start MTX_Identity33_
MTX_Identity33_: ; 0x020CAFEC
	mov r2, #0x1000
	str r2, [r0, #0x20]
	mov r3, #0
	stmia r0!, {r2, r3}
	mov r1, #0
	stmia r0!, {r1, r3}
	stmia r0!, {r2, r3}
	stmia r0!, {r1, r3}
	bx lr
	arm_func_end MTX_Identity33_
