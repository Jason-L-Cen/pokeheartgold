	.extern NNS_G3dGeBufferOP_N
	.extern NNS_G3dGetResDataByName
	.extern NNS_G3dGlb
	.extern _02109474
	.extern _02110A74
	.extern _02110A80
	.extern _02110A8C
	.extern _02110B9C
	.extern _021DA700
	.extern _021DA704
	.extern addLink_
	.extern updateHintVec_
	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"

.text
	arm_func_start NNS_G3dGlbInit
NNS_G3dGlbInit: ; 0x020BEE44
	stmdb sp!, {r3, lr}
	ldr r2, _020BEF58 ; =0x17101610
	ldr r1, _020BEF5C ; =NNS_G3dGlb
	mov r0, #0
	str r2, [r1]
	str r0, [r1, #4]
	mov r2, #2
	ldr r0, _020BEF60 ; =0x32323232
	str r2, [r1, #0x48]
	ldr r2, _020BEF64 ; =0x60293130
	str r0, [r1, #0x7c]
	ldr r0, _020BEF68 ; =0x33333333
	str r2, [r1, #0x90]
	ldr r2, _020BEF6C ; =0x002A1B19
	str r0, [r1, #0xa4]
	ldr r0, _020BEF70 ; =NNS_G3dGlb + 0x4C
	str r2, [r1, #0xb8]
	bl MTX_Identity43_
	ldr r0, _020BEF74 ; =NNS_G3dGlb + 0x8
	bl MTX_Identity44_
	ldr ip, _020BEF78 ; =0x40000200
	ldr r3, _020BEF7C ; =0xC0080000
	ldr r2, _020BEF80 ; =0x2D8B62D8
	ldr r1, _020BEF5C ; =NNS_G3dGlb
	sub r0, ip, #0xc0000001
	str r2, [r1, #0x80]
	str ip, [r1, #0x84]
	str r0, [r1, #0x88]
	ldr r2, _020BEF84 ; =0x4210C210
	str r3, [r1, #0x8c]
	str r2, [r1, #0x94]
	ldr r0, _020BEF88 ; =0x001F008F
	str r2, [r1, #0x98]
	str r0, [r1, #0x9c]
	sub r2, r3, #0x90000
	ldr r0, _020BEF8C ; =0x00007FFF
	str r2, [r1, #0xa0]
	str r0, [r1, #0xa8]
	mov r2, #0x4000001f
	ldr r0, _020BEF90 ; =0x800003E0
	str r2, [r1, #0xac]
	str r0, [r1, #0xb0]
	rsb r0, ip, #0x7e00
	str r0, [r1, #0xb4]
	mov r2, #0
	str r2, [r1, #0xe0]
	str r2, [r1, #0xe4]
	ldr r0, _020BEF94 ; =NNS_G3dGlb + 0xBC
	str r2, [r1, #0xe8]
	bl MTX_Identity33_
	mov r1, #0
	ldr r0, _020BEF5C ; =NNS_G3dGlb
	mov r2, #0x1000
	str r2, [r0, #0xec]
	str r2, [r0, #0xf0]
	str r2, [r0, #0xf4]
	str r1, [r0, #0xf8]
	str r1, [r0, #0xfc]
	str r1, [r0, #0x248]
	str r1, [r0, #0x244]
	str r1, [r0, #0x240]
	str r1, [r0, #0x254]
	str r1, [r0, #0x24c]
	str r2, [r0, #0x250]
	str r1, [r0, #0x25c]
	str r1, [r0, #0x258]
	sub r1, r1, #0x1000
	str r1, [r0, #0x260]
	ldmia sp!, {r3, pc}
	.align 2, 0
_020BEF58: .word 0x17101610
_020BEF5C: .word NNS_G3dGlb
_020BEF60: .word 0x32323232
_020BEF64: .word 0x60293130
_020BEF68: .word 0x33333333
_020BEF6C: .word 0x002A1B19
_020BEF70: .word NNS_G3dGlb + 0x4C
_020BEF74: .word NNS_G3dGlb + 0x8
_020BEF78: .word 0x40000200
_020BEF7C: .word 0xC0080000
_020BEF80: .word 0x2D8B62D8
_020BEF84: .word 0x4210C210
_020BEF88: .word 0x001F008F
_020BEF8C: .word 0x00007FFF
_020BEF90: .word 0x800003E0
_020BEF94: .word NNS_G3dGlb + 0xBC
	arm_func_end NNS_G3dGlbInit

	arm_func_start NNS_G3dGlbFlushP
NNS_G3dGlbFlushP: ; 0x020BEF98
	stmdb sp!, {r3, lr}
	ldr r0, _020BEFCC ; =NNS_G3dGlb
	ldr r1, _020BEFD0 ; =NNS_G3dGlb
	ldr r0, [r0]
	add r1, r1, #4
	mov r2, #0x3e
	bl NNS_G3dGeBufferOP_N
	ldr r0, _020BEFCC ; =NNS_G3dGlb
	ldr r1, [r0, #0xfc]
	bic r1, r1, #1
	bic r1, r1, #2
	str r1, [r0, #0xfc]
	ldmia sp!, {r3, pc}
	.align 2, 0
_020BEFCC: .word NNS_G3dGlb
_020BEFD0: .word NNS_G3dGlb
	arm_func_end NNS_G3dGlbFlushP
