	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"
	.extern _021DA470

.text
	arm_func_start NNS_G2dGetNewCellTransferStateHandle
NNS_G2dGetNewCellTransferStateHandle: ; 0x020BAEE4
	ldr r1, _020BAF38 ; =_021DA470
	mov r0, #0
	ldr r3, [r1, #4]
	cmp r3, #0
	bls _020BAF30
	ldr ip, [r1, #8]
	mov r2, ip
_020BAF00:
	ldr r1, [r2, #0x1c]
	cmp r1, #1
	beq _020BAF20
	mov r1, #0x30
	mla r1, r0, r1, ip
	mov r2, #1
	str r2, [r1, #0x1c]
	bx lr
_020BAF20:
	add r0, r0, #1
	cmp r0, r3
	add r2, r2, #0x30
	blo _020BAF00
_020BAF30:
	mvn r0, #0
	bx lr
	.align 2, 0
_020BAF38: .word _021DA470
	arm_func_end NNS_G2dGetNewCellTransferStateHandle
