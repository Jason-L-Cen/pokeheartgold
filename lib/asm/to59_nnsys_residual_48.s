	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"
	.extern NNS_FndGetNextListObject
	.extern NNSi_SndPlayerPause
	.extern _021DFDC4

.text
	arm_func_start NNS_SndPlayerPauseByPlayerNo
NNS_SndPlayerPauseByPlayerNo: ; 0x020C816C
	stmdb sp!, {r4, r5, r6, r7, r8, lr}
	mov r2, #0x24
	mul r6, r0, r2
	ldr r5, _020C81C0 ; =_021DFDC4
	mov r8, r1
	add r0, r5, r6
	mov r1, #0
	bl NNS_FndGetNextListObject
	movs r7, r0
	ldmeqia sp!, {r4, r5, r6, r7, r8, pc}
_020C8194:
	mov r1, r7
	add r0, r5, r6
	bl NNS_FndGetNextListObject
	mov r4, r0
	mov r0, r7
	mov r1, r8
	bl NNSi_SndPlayerPause
	mov r7, r4
	cmp r4, #0
	bne _020C8194
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	.align 2, 0
_020C81C0: .word _021DFDC4
	arm_func_end NNS_SndPlayerPauseByPlayerNo
