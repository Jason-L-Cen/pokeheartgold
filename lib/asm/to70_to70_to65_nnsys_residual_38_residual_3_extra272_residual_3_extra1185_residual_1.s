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
