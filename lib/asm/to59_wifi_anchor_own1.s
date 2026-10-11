	.include "asm/macros.inc"
	.include "wifi.inc"
	.include "global.inc"
	.public gSocSoclConfig
	.public gSocInetNtoABuffer
	.public gSocHostName
	.public _021D4620
	.public gCpsAlloc
	.public _021D463C
	.public _021D464C
	.public gCpsFree
	.public _021D4670
	.public _021D4680
	.public _021D4688
	.public _021D46A0
	.public _021D46AA
	.public _021D4700
	.public _021D4720
	.public _021D4764
	.public _021D47C8
	.public _021D4888
	.public _021D4948
	.public _021D4B08
	.public _021D4B32
	.public _021D4B4C
	.public _021D4B76
	.public _021D4CCC
	.public _021D5660
	.public _021D5E60
	.public _021D5E68
	.public _021D5FD8
	.public _021D5FEB
	.public gWcmWork
	.public gWcmCpsif
	.public gWcmCpsifSendQueue
	.public gWcmCpsifMutex
	.public gWcmRssiCount
	.public gWcmRssi
	.bss
gSocSoclConfig: ; 0x021D44D4
	.space 0x38

gSocInetNtoABuffer: ; 0x021D450C
	.space 0x10

gSocHostName: ; 0x021D451C
	.space 0x104

_021D4620: ; 0x021D4620
	.space 0x14

gCpsAlloc: ; 0x021D4634
	.space 0x8

_021D463C: ; 0x021D463C
	.space 0x10

_021D464C: ; 0x021D464C
	.space 0x14

gCpsFree: ; 0x021D4660
	.space 0x10

	.public _021D4670
_021D4670: ; 0x021D4670
	.space 0x10

_021D4680: ; 0x021D4680
	.space 0x8

_021D4688: ; 0x021D4688
	.space 0x18

_021D46A0: ; 0x021D46A0
	.space 0xA

_021D46AA: ; 0x021D46AA
	.space 0x56

_021D4700: ; 0x021D4700
	.space 0x20

_021D4720: ; 0x021D4720
	.space 0x44

_021D4764: ; 0x021D4764
	.space 0x64

_021D47C8: ; 0x021D47C8
	.space 0xC0

_021D4888: ; 0x021D4888
	.space 0xC0

_021D4948: ; 0x021D4948
	.space 0x1C0

_021D4B08: ; 0x021D4B08
	.space 0x2A

_021D4B32: ; 0x021D4B32
	.space 0x1A

_021D4B4C: ; 0x021D4B4C
	.space 0x2A

_021D4B76: ; 0x021D4B76
	.space 0x156

_021D4CCC: ; 0x021D4CCC
	.space 0x994

_021D5660: ; 0x021D5660
	.space 0x800

_021D5E60: ; 0x021D5E60
	.space 0x8

_021D5E68: ; 0x021D5E68
	.space 0x170

_021D5FD8: ; 0x021D5FD8
	.space 0x13

_021D5FEB: ; 0x021D5FEB
	.space 0x1

gWcmWork: ; 0x021D5FEC
	.space 0x4

gWcmCpsif: ; 0x021D5FF0
	.space 0x4

gWcmCpsifSendQueue: ; 0x021D5FF4
	.space 0x8

gWcmCpsifMutex: ; 0x021D5FFC
	.space 0x20

gWcmRssiCount: ; 0x021D601C
	.space 0x4

gWcmRssi: ; 0x021D6020
	.space 0x20

