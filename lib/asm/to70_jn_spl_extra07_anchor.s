	.public _021105DC
	.public _021105E4
	.public _021105EC
	.public spl_rndm_seed
	.extern drawXYPlane
	.extern drawXZPlane
	.extern rotTypeXYZ
	.extern rotTypeY
	.include "asm/macros.inc"
	.include "jn_spl.inc"
	.include "global.inc"

	.bss

spl_rndm_seed:
	.space 0x4

.text
	.data

_021105DC:
	.word drawXYPlane
	.word drawXZPlane
_021105E4:
	.word rotTypeY
	.word rotTypeXYZ
_021105EC:
	.short 0, 0x1000, 0
	.balign 4, 0
