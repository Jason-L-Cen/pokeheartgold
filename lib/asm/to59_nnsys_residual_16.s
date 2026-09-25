	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"
	.public NNS_G2dUnpackNMC
	.extern UnpackExtendedData_

.text
	arm_func_start NNS_G2dUnpackNMC
NNS_G2dUnpackNMC: ; 0x020B7014
	stmdb sp!, {r4, lr}
	ldr r1, [r0, #4]
	mov lr, #0
	add r1, r1, r0
	str r1, [r0, #4]
	ldr r1, [r0, #8]
	add r1, r1, r0
	str r1, [r0, #8]
	ldrh r1, [r0]
	ldr r4, [r0, #4]
	cmp r1, #0
	bls _020B7070
_020B7044:
	add r3, r4, lr, lsl #3
	ldr ip, [r0, #8]
	ldr r2, [r3, #4]
	add r1, lr, #1
	add r2, ip, r2
	str r2, [r3, #4]
	ldrh r2, [r0]
	mov r1, r1, lsl #0x10
	mov lr, r1, lsr #0x10
	cmp r2, r1, lsr #16
	bhi _020B7044
_020B7070:
	ldr r1, [r0, #0x10]
	cmp r1, #0
	ldmeqia sp!, {r4, pc}
	add r1, r1, r0
	str r1, [r0, #0x10]
	mov r0, r1
	bl UnpackExtendedData_
	ldmia sp!, {r4, pc}
	arm_func_end NNS_G2dUnpackNMC
