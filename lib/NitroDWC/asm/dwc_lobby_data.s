	.public _02254D04
	.public _022577C4
	.public ov45_02254D1C
	.public ov45_02254D3C
	.public ov45_02254D5C
	.public ov45_02254D7C
	.public ov45_02254D9C
	.public ov45_02254DBC
	.public ov45_02254DC4
	.public ov45_02254DF4
	.public ov45_02254E05
	.public ov45_022577C8
	.public ov45_022577CC
	.public ov45_022577D0
	.public ov45_022577D4
	.public ov45_022577D8
	.public ov45_022577DC
	.public ov45_022577E0
	.public ov45_022577E4
	.public ov45_022577E8
	.public ov45_022577F0
	.public ov45_022577F4
	.public ov45_022577F8
	.public ov45_022587F8
	.extern ov45_02231448
	.extern ov45_02231460
	.extern ov45_02231C38
	.extern ov45_02236A48
	.extern ov45_02236A70
	.extern ov45_02236AB8
	.extern ov45_0223E494
	.extern ov45_0223FD6C
	.extern ov45_0223FD88
	.extern ov45_022412C4
	.extern ov45_02241F80
	.extern ov45_02241FA4
	.extern ov45_02241FA8
	.extern ov45_02241FCC
	.extern ov45_02241FD0
	.extern ov45_02241FF4
	.extern ov45_02241FF8
	.extern ov45_0224201C
	.extern ov45_02242020
	.extern ov45_02242044
	.extern ov45_02242048
	.extern ov45_0224307C
	.extern ov45_0224B0FC
	.extern ov45_0224B310
	.extern ov45_0224B474
	.extern ov45_0224B5E0
	.extern ov45_0224B748
	.extern ov45_0224B794
	.extern ov45_0224B8D0
	.extern ov45_0224BB4C
	.extern ov45_0224BC8C
	.extern ov45_0224C018
	.extern ov45_0224C244
	.extern ov45_0224C2A8
	.extern ov45_0224C388
	.extern ov45_0224C594
	.extern ov45_0224C610
	.extern ov45_0224C6DC
	.extern ov45_0224C9D8
	.extern ov45_0224CBAC
	.extern ov45_0224CCC8
	.extern ov45_0224CDE0
	.extern ov45_0224CF0C
	.extern ov45_0224D128
	.extern ov45_0224D30C
	.extern ov45_0224D4BC
	.extern ov45_0224D588
	.extern ov45_0224D924
	.extern ov45_0224D9F0
	.extern ov45_0224DE70
	.extern ov45_0224DF50
	.extern ov45_0224DFC4
	.extern ov45_0224E210
	.extern ov45_0224E288
	.extern ov45_0224E408
	.extern ov45_0224E5EC
	.extern ov45_0224E734
	.extern ov45_0224E81C
	.extern ov45_0224E968
	.extern ov45_0224EA2C
	.extern ov45_0224EAF4
	.extern ov45_0224EC14
	.extern ov45_0224ECCC
	.extern ov45_0224ED80
	.extern ov45_0224EEBC
	.extern ov45_0224EF70
	.extern ov45_0224F024
	.extern ov45_0224F0D8
	.extern ov45_0224F18C
	.extern ov45_0224F240
	.extern ov45_0224F2F4
	.extern ov45_0224F2F8
	.extern ov45_0224F35C
	.extern ov45_0224F3C0
	.extern ov45_0224F428
	.extern ov45_0224F490
	.extern ov45_02254F74
	.extern ov45_02254F78
	.extern ov45_02254F7C
	.extern ov45_02254F80
	.extern ov45_02254F84
	.extern ov45_02254F90
	.extern ov45_02255368
	.extern ov45_0225536C
	.extern ov45_02255370
	.extern ov45_02255374
	.extern ov45_02255378
	.extern ov45_0225537C
	.extern ov45_02255380
	.extern ov45_02255384
	.extern ov45_02255388
	.extern ov45_0225538C
	.extern ov45_02255390
	.extern ov45_02255394
	.include "asm/macros.inc"
	.include "overlay_45_arm.inc"
	.include "global.inc"
	.public _version_NINTENDO_DWC_LOBBY

.text
	.rodata

_02254D04:
	.word ov45_02254F78
	.word ov45_02254F84
	.word ov45_02254F80
	.word ov45_02254F7C
	.word ov45_02254F74
	.word ov45_02254F90

ov45_02254D1C: ; 0x02254D1C
	.byte 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00
	.byte 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00

ov45_02254D3C: ; 0x02254D3C
	.byte 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00
	.byte 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00

ov45_02254D5C: ; 0x02254D5C
	.byte 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00
	.byte 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00

ov45_02254D7C: ; 0x02254D7C
	.byte 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00
	.byte 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00

ov45_02254D9C: ; 0x02254D9C
	.byte 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00
	.byte 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00

ov45_02254DBC: ; 0x02254DBC
	.asciz "#GSP"
	.balign 4, 0

ov45_02254DC4: ; 0x02254DC4
	.word ov45_02255368
	.word ov45_02255370
	.word ov45_02255384
	.word ov45_02255388
	.word ov45_0225538C
	.word ov45_02255390
	.word ov45_02255394
	.word ov45_0225537C
	.word ov45_02255374
	.word ov45_02255378
	.word ov45_02255380
	.word ov45_0225536C

ov45_02254DF4: ; 0x02254DF4
	.asciz "0123456789abcdef"

ov45_02254E05: ; 0x02254E05
	.asciz "0123456789abcdef"

	.data

	.bss

_022577C4:
	.space 0x4

ov45_022577C8: ; 0x022577C8
	.space 0x4

ov45_022577CC: ; 0x022577CC
	.space 0x4

ov45_022577D0: ; 0x022577D0
	.space 0x4

ov45_022577D4: ; 0x022577D4
	.space 0x4

ov45_022577D8: ; 0x022577D8
	.space 0x4

ov45_022577DC: ; 0x022577DC
	.space 0x4

ov45_022577E0: ; 0x022577E0
	.space 0x4

ov45_022577E4: ; 0x022577E4
	.space 0x4

ov45_022577E8: ; 0x022577E8
	.space 0x8

ov45_022577F0: ; 0x022577F0
	.space 0x4

ov45_022577F4: ; 0x022577F4
	.space 0x4

ov45_022577F8: ; 0x022577F8
	.space 0x1000

ov45_022587F8: ; 0x022587F8
	.space 0x8
