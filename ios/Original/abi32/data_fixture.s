.rodata
.balign 4,0
.public HgAsmBytes
.public HgAsmTarget
.public HgAsmRng
.public HgAsmAlias
.public HgAsmOverlay
HgAsmBytes:
.byte 1,2,3,4
HgAsmTarget:
.word HgAsmBytes+2
HgAsmRng:
.word LCRandom
HgAsmOverlay:
.word SDK_OVERLAY_OVY_7_ID
.set HgAsmAlias,HgAsmBytes+1
.bss
.public HgAsmZero
HgAsmZero:
.space 16
.section .sdk_fixture,16,1,3
.public HgSdkData
.type HgSdkData,%object
HgSdkData:
.word 0x01020304
.short 0x1234
.byte 0x55,0x66
.size HgSdkData,8
.section .sdk_fixture_bss,8,8,3
.public HgSdkZero
.type HgSdkZero,%object
HgSdkZero:
.space 8
.size HgSdkZero,8
.data
.balign 4,0
.public HgAsmMutable
.type HgAsmMutable,%object
HgAsmMutable:
.word HgAsmBytes+2
.word LCRandom
.word SDK_OVERLAY_OVY_7_ID
.size HgAsmMutable,12
.section .sinit,4,1,3
.word HGAsmOverlayConstructor
