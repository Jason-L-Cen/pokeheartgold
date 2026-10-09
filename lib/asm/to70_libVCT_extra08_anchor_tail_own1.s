	.include "asm/macros.inc"
	.include "libVCT.inc"
	.include "global.inc"
	.public _021D7008
	.public sH
	.public sTap
	.public sSpkBuffer
	.public sDelayLine
	.public _021D848C
	.public __vct
	.bss
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

