	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"

.text
	arm_func_start NNS_SndPlayerReadVariable
NNS_SndPlayerReadVariable: ; 0x020C8374
	stmdb sp!, {r4, lr}
	ldr r3, [r0]
	mov r4, r2
	cmp r3, #0
	moveq r0, #0
	ldmeqia sp!, {r4, pc}
	ldr r2, [r0]
	ldrb r0, [r2, #0x2d]
	cmp r0, #0
	bne _020C83AC
	mvn r0, #0
	strh r0, [r4]
	mov r0, #1
	ldmia sp!, {r4, pc}
_020C83AC:
	ldrb r0, [r2, #0x3c]
	bl SND_GetPlayerLocalVariable
	strh r0, [r4]
	mov r0, #1
	ldmia sp!, {r4, pc}
	arm_func_end NNS_SndPlayerReadVariable
