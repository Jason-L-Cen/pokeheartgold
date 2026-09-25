	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"
	.extern _02110A00
	.extern _02110A24
	.extern _02110A28

.text
	arm_func_start NNS_G3dAnmObjInit
NNS_G3dAnmObjInit: ; 0x020BE008
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	mov ip, #0
	str ip, [r0]
	str r1, [r0, #8]
	str ip, [r0, #0x10]
	mov r4, #0x7f
	strb r4, [r0, #0x18]
	mov r4, #0x1000
	str r4, [r0, #4]
	str r3, [r0, #0x14]
	strb ip, [r0, #0x19]
	ldr r3, _020BE09C ; =_02110A00
	str ip, [r0, #0xc]
	ldr r7, [r3]
	cmp r7, #0
	ldmlsia sp!, {r3, r4, r5, r6, r7, pc}
	ldrb r6, [r1]
	ldr r4, _020BE0A0 ; =_02110A24
_020BE050:
	ldrb r3, [r4, ip, lsl #3]
	mov r5, ip, lsl #3
	cmp r6, r3
	bne _020BE08C
	add r3, r4, r5
	ldrh lr, [r1, #2]
	ldrh r3, [r3, #2]
	cmp lr, r3
	bne _020BE08C
	ldr r3, _020BE0A4 ; =_02110A28
	ldr r3, [r3, r5]
	cmp r3, #0
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	blx r3
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_020BE08C:
	add ip, ip, #1
	cmp ip, r7
	blo _020BE050
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.align 2, 0
_020BE09C: .word _02110A00
_020BE0A0: .word _02110A24
_020BE0A4: .word _02110A28
	arm_func_end NNS_G3dAnmObjInit
