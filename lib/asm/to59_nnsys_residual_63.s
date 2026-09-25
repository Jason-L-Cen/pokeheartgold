	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"
	.public NNSi_SndArcLoadBank
	.extern LoadBank
	.extern LoadSingleWaves
	.extern NNS_SndArcGetBankInfo
	.extern NNS_SndArcGetFileAddress
	.extern NNS_SndArcGetWaveArcInfo
	.extern NNSi_SndArcLoadWaveArc

.text
	arm_func_start NNSi_SndArcLoadBank
NNSi_SndArcLoadBank: ; 0x020CA054
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #8
	mov sl, r1
	mov sb, r2
	mov fp, r3
	bl NNS_SndArcGetBankInfo
	movs r5, r0
	addeq sp, sp, #8
	moveq r0, #4
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	tst sl, #2
	ldr r0, [r5]
	beq _020CA0A8
	mov r1, sb
	mov r2, fp
	bl LoadBank
	movs r6, r0
	bne _020CA0B0
	add sp, sp, #8
	mov r0, #8
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020CA0A8:
	bl NNS_SndArcGetFileAddress
	mov r6, r0
_020CA0B0:
	and r8, sl, #4
	mov r7, #0
_020CA0B8:
	add r0, r5, r7, lsl #1
	ldrh r0, [r0, #4]
	ldr r1, _020CA190 ; =0x0000FFFF
	cmp r0, r1
	beq _020CA16C
	bl NNS_SndArcGetWaveArcInfo
	movs r4, r0
	addeq sp, sp, #8
	moveq r0, #5
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	add r0, sp, #4
	str r0, [sp]
	add r0, r5, r7, lsl #1
	ldrh r0, [r0, #4]
	mov r1, sl
	mov r2, sb
	mov r3, fp
	bl NNSi_SndArcLoadWaveArc
	cmp r0, #0
	addne sp, sp, #8
	ldmneia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldr r0, [r4]
	mov r0, r0, lsr #0x18
	tst r0, #1
	cmpne r8, #0
	beq _020CA150
	str sb, [sp]
	ldr r1, [r4]
	ldr r0, [sp, #4]
	mov r3, r1, lsl #8
	mov r1, r6
	mov r2, r7
	mov r3, r3, lsr #8
	bl LoadSingleWaves
	cmp r0, #0
	addeq sp, sp, #8
	moveq r0, #9
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020CA150:
	cmp r6, #0
	ldrne r2, [sp, #4]
	cmpne r2, #0
	beq _020CA16C
	mov r0, r6
	mov r1, r7
	bl SND_AssignWaveArc
_020CA16C:
	add r7, r7, #1
	cmp r7, #4
	blt _020CA0B8
	ldr r0, [sp, #0x30]
	cmp r0, #0
	strne r6, [r0]
	mov r0, #0
	add sp, sp, #8
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_020CA190: .word 0x0000FFFF
	arm_func_end NNSi_SndArcLoadBank
