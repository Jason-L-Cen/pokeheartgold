	.include "asm/macros.inc"
	.include "overlay_38_arm.inc"
	.include "global.inc"
	.extern ghiAppendCharToBuffer
	.extern ghiAppendDataToBuffer
	.extern ghiAppendToChunkHeaderBuffer
	.extern ghiCallCompletedCallback
	.extern ghiCallPostCallback
	.extern ghiCallProgressCallback
	.extern ghiDoPosting
	.extern ghiDoSecuringSession
	.extern ghiFindFreeSlot
	.extern ghiFreeBuffer
	.extern ghiFreeConnection
	.extern ghiInitBuffer
	.extern ghiInitFixedBuffer
	.extern ghiParseChunkSize
	.extern ghiParseStatus
	.extern ghiParseURL
	.extern ghiPostDoPosting
	.extern ghiPostFileDiskStateDoPosting
	.extern ghiPostFileMemoryStateDoPosting
	.extern ghiPostGetContentLength
	.extern ghiPostGetContentType
	.extern ghiPostGetHasFilesContentLength
	.extern ghiPostGetNoFilesContentLength
	.extern ghiPostInitState
	.extern ghiPostStateCleanup
	.extern ghiPostStateDoPosting
	.extern ghiPostStateInit
	.extern ghiPostStringStateDoPosting
	.extern ghiProcessConnection
	.extern ghiProcessIncomingFileData
	.extern ghiReadDataFromBuffer
	.extern ghiRequestToConnection
	.extern ghiResetBuffer
	.extern ghiResizeBuffer
	.extern ghttpGetExA
	.public ov38_02226104
	.public ov38_02226138
	.public ov38_02226144
	.public ov38_0222615C
	.public ov38_022261DC
	.public ov38_022261E0
	.public ov38_022261F8
	.public ov38_02226210
	.public ov38_02226214
	.public ov38_02226218
	.public ov38_0222621C
	.public ov38_02226220
	.public ov38_02226224
	.public ov38_0222622C
	.public ov38_02226230
	.public ov38_02226234
	.public ov38_02226AF4
	.public ov38_02226AF8
	.public ov38_02226B10
	.public ov38_02226B30
	.public ov38_02226BF0
	.public ov38_02226C10
	.public ov38_02226C18
	.public ov38_02226C1C
	.public ov38_02226C58
	.public ov38_02226C61
	.bss
ov38_02226104: ; 0x02226104
	.space 0x34

ov38_02226138: ; 0x02226138
	.space 0xC

ov38_02226144: ; 0x02226144
	.space 0x18

ov38_0222615C: ; 0x0222615C
	.space 0x80

ov38_022261DC: ; 0x022261DC
	.space 0x4

ov38_022261E0: ; 0x022261E0
	.space 0x18

ov38_022261F8: ; 0x022261F8
	.space 0x18

ov38_02226210: ; 0x02226210
	.space 0x4

ov38_02226214: ; 0x02226214
	.space 0x4

ov38_02226218: ; 0x02226218
	.space 0x4

ov38_0222621C: ; 0x0222621C
	.space 0x4

ov38_02226220: ; 0x02226220
	.space 0x4

ov38_02226224: ; 0x02226224
	.space 0x8

ov38_0222622C: ; 0x0222622C
	.space 0x4

ov38_02226230: ; 0x02226230
	.space 0x4

ov38_02226234: ; 0x02226234
	.space 0x8C0

ov38_02226AF4: ; 0x02226AF4
	.space 0x4

ov38_02226AF8: ; 0x02226AF8
	.space 0x18

ov38_02226B10: ; 0x02226B10
	.space 0x20

ov38_02226B30: ; 0x02226B30
	.space 0xC0

ov38_02226BF0: ; 0x02226BF0
	.space 0x20

ov38_02226C10: ; 0x02226C10
	.space 0x8

ov38_02226C18: ; 0x02226C18
	.space 0x4

ov38_02226C1C: ; 0x02226C1C
	.space 0x3C

ov38_02226C58: ; 0x02226C58
	.space 0x9

ov38_02226C61: ; 0x02226C61
	.space 0x3ff
