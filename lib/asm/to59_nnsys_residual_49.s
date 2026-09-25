	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"
	.extern NNSi_SndFaderSet

.text
	arm_func_start NNS_SndPlayerMoveVolume
NNS_SndPlayerMoveVolume: ; 0x020C8230
	stmdb sp!, {r3, lr}
	ldr r3, [r0]
	cmp r3, #0
	ldrne r3, [r0]
	ldrneb r0, [r3, #0x2c]
	cmpne r0, #2
	ldmeqia sp!, {r3, pc}
	add r0, r3, #0x1c
	mov r1, r1, lsl #8
	bl NNSi_SndFaderSet
	ldmia sp!, {r3, pc}
	arm_func_end NNS_SndPlayerMoveVolume
