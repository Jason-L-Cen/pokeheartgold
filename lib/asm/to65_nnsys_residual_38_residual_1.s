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
	.public removeLink_

.text
	arm_func_start removeLink_
removeLink_: ; 0x020BE314
	ldr r3, [r0]
	cmp r3, #0
	moveq r0, #0
	bxeq lr
	cmp r3, r1
	bne _020BE344
	ldr r3, [r3, #0x10]
	mov r2, #0
	str r3, [r0]
	str r2, [r1, #0x10]
	mov r0, #1
	bx lr
_020BE344:
	ldr r2, [r3, #0x10]
	cmp r2, #0
	beq _020BE380
_020BE350:
	cmp r2, r1
	bne _020BE370
	ldr r1, [r2, #0x10]
	mov r0, #0
	str r1, [r3, #0x10]
	str r0, [r2, #0x10]
	mov r0, #1
	bx lr
_020BE370:
	mov r3, r2
	ldr r2, [r2, #0x10]
	cmp r2, #0
	bne _020BE350
_020BE380:
	mov r0, #0
	bx lr
	arm_func_end removeLink_
