	.include "asm/macros.inc"
	.include "fx_mtx43.inc"
	.include "global.inc"
	.text

	thumb_func_start MTX_Scale43_
MTX_Scale43_: ; 0x020CB7B4
	stmia r0!, {r1}
	mov r1, #0
	str r3, [r0, #0x1c]
	mov r3, #0
	stmia r0!, {r1, r3}
	stmia r0!, {r1, r2, r3}
	mov r2, #0
	stmia r0!, {r1, r3}
	add r0, #4
	stmia r0!, {r1, r2, r3}
	bx lr
	.align 2, 0
	thumb_func_end MTX_Scale43_
