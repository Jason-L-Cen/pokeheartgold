	.extern __atexit_funcs
	.extern __console_exit
	.public __exit
	.extern __atexit_funcs
	.extern __console_exit
	.include "asm/macros.inc"
	.include "MSL_ARM_abort_exit.inc"
	.include "global.inc"

.text
