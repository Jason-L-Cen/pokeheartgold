	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"
	.public LoadWaveArcTable
	.extern NNS_SndArcGetCurrent
	.extern NNS_SndArcGetFileAddress
	.extern NNS_SndArcReadFile
	.extern NNS_SndArcSetFileAddress
	.extern NNS_SndHeapAlloc
	.extern WaveArcTableDisposeCallback
	.extern _021E0914

.text
	arm_func_start LoadWaveArcTable
LoadWaveArcTable: ; 0x020CA470
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, lr}
	mov sb, r0
	mov r8, r1
	mov r7, r2
	bl NNS_SndArcGetFileAddress
	movs r5, r0
	bne _020CA564
	ldr r1, _020CA56C ; =_021E0914
	mov r0, sb
	mov r2, #0x3c
	mov r3, #0
	bl NNS_SndArcReadFile
	cmp r0, #0x3c
	movne r0, #0
	ldmneia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	ldr r0, _020CA570 ; =_021E0914
	cmp r8, #0
	ldr r0, [r0, #0x38]
	mov r6, r0, lsl #2
	mov r4, r6, lsl #1
	moveq r0, #0
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	cmp r7, #0
	moveq r3, #0
	beq _020CA4DC
	bl NNS_SndArcGetCurrent
	mov r3, r0
_020CA4DC:
	ldr r2, _020CA574 ; =WaveArcTableDisposeCallback
	mov r0, r8
	add r1, r4, #0x5c
	str sb, [sp]
	bl NNS_SndHeapAlloc
	movs r5, r0
	moveq r0, #0
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	mov r0, sb
	mov r1, r5
	add r2, r6, #0x3c
	mov r3, #0
	bl NNS_SndArcReadFile
	add r1, r6, #0x3c
	cmp r0, r1
	movne r0, #0
	ldmneia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	ldr r1, [r5, #0x38]
	add r0, r5, #0x3c
	mov r2, r6
	add r1, r0, r1, lsl #2
	bl MI_CpuCopy8
	mov r2, r6
	add r0, r5, #0x3c
	mov r1, #0
	bl MI_CpuFill8
	mov r0, r5
	add r1, r4, #0x3c
	bl DC_StoreRange
	cmp r7, #0
	beq _020CA564
	mov r0, sb
	mov r1, r5
	bl NNS_SndArcSetFileAddress
_020CA564:
	mov r0, r5
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	.align 2, 0
_020CA56C: .word _021E0914
_020CA570: .word _021E0914
_020CA574: .word WaveArcTableDisposeCallback
	arm_func_end LoadWaveArcTable
