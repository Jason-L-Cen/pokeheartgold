	.public E2scales
	.public _021D6F58
	.public _021D6F60
	.public _021D6F6C
	.public _021D6F78
	.public _021D6F84
	.public gVctSsp
	.public gVctRejectSession
	.public gVctServerSession
	.public gVctPacketBuffers
	.public gVctVadThreshold
	.public _021D7008
	.public gVctControl
	.public __vct
	.public sDelayLine
	.public sH
	.public sSpkBuffer
	.public sTap
	.include "asm/macros.inc"
	.include "libVCT.inc"
	.include "global.inc"

	.bss

_021D6F58: ; 0x021D6F58
	.space 0x8

_021D6F60: ; 0x021D6F60
	.space 0xC

_021D6F6C: ; 0x021D6F6C
	.space 0xC

_021D6F78: ; 0x021D6F78
	.space 0xC

_021D6F84: ; 0x021D6F84
	.space 0xC

gVctSsp: ; 0x021D6F90
	.space 0x24

gVctRejectSession: ; 0x021D6FB4
	.space 0x18

gVctServerSession: ; 0x021D6FCC
	.space 0x18

gVctPacketBuffers: ; 0x021D6FE4
	.space 0x8

