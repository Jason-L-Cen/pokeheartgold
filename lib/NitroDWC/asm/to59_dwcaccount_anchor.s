	.include "asm/macros.inc"
	.include "dwcaccount.inc"
	.include "global.inc"

	.public _021105F4
	.public _02110618
.text
	.data

_021105F4:
	.asciz "0123456789abcdefghijklmnopqrstuv"
	.balign 4, 0
_02110618:
	.asciz "%s%c%c%c%c%s"
