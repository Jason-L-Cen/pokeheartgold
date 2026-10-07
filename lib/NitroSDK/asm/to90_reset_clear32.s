	.include "asm/macros.inc"
	.public OSi_CpuClear32

.section .itcm,4,1,7
	.type OSi_CpuClear32, @function
	arm_func_start OSi_CpuClear32
OSi_CpuClear32:
	add	r12, r1, r2
	cmp	r1, r12
	stmltia	r1!, {r0}
	blt . - 0x8
	bx lr
	arm_func_end OSi_CpuClear32
