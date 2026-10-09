	.public E2scales
	.public _021D6F58
	.public _021D6F60
	.public _021D6F6C
	.public _021D6F78
	.public _021D6F84
	.public _021D6F90
	.public _021D6FB4
	.public _021D6FCC
	.public _021D6FE4
	.public _021D6FEC
	.public _021D7008
	.public _021D848C
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

_021D6F90: ; 0x021D6F90
	.space 0x24

_021D6FB4: ; 0x021D6FB4
	.space 0x18

_021D6FCC: ; 0x021D6FCC
	.space 0x18

_021D6FE4: ; 0x021D6FE4
	.space 0x8

_021D6FEC: ; 0x021D6FEC
	.space 0xC

E2scales: ; 0x021D6FF8
	.space 0x10

_021D7008: ; 0x021D7008
	.space 0x14

sH: ; 0x021D701C
	.space 0x18

sTap: ; 0x021D7034
	.space 0x18

sSpkBuffer: ; 0x021D704C
	.space 0x440

sDelayLine: ; 0x021D748C
	.space 0x1000

_021D848C: ; 0x021D848C
	.space 0xC

__vct: ; 0x021D8498
	.space 0x18

