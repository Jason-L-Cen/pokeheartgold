	.include "asm/macros.inc"
.text
	arm_func_start OsCountZeroBits
OsCountZeroBits:
	clz r0, r0
	bx lr
	arm_func_end OsCountZeroBits
