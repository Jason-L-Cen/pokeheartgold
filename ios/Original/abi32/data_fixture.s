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
