	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"
	.public NNSi_SndArcLoadGroup
	.extern NNS_SndArcGetGroupInfo
	.extern NNSi_SndArcLoadBank
	.extern NNSi_SndArcLoadSeq
	.extern NNSi_SndArcLoadSeqArc
	.extern NNSi_SndArcLoadWaveArc

.text
	arm_func_start NNSi_SndArcLoadGroup
NNSi_SndArcLoadGroup: ; 0x020C9E6C
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	mov sb, r1
	bl NNS_SndArcGetGroupInfo
	movs r6, r0
	moveq r0, #1
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldr r0, [r6]
	mov r7, #0
	cmp r0, #0
	bls _020C9F68
	add r8, r6, #4
	mov fp, r7
	mov sl, r7
	mov r4, r7
	mov r5, r7
_020C9EA8:
	ldrb r0, [r8]
	cmp r0, #3
	addls pc, pc, r0, lsl #2
	b _020C9F54
_020C9EB8: ; jump table
	b _020C9EC8 ; case 0
	b _020C9F10 ; case 1
	b _020C9F34 ; case 2
	b _020C9EEC ; case 3
_020C9EC8:
	str r5, [sp]
	ldrb r1, [r8, #1]
	ldr r0, [r8, #4]
	mov r2, sb
	mov r3, #1
	bl NNSi_SndArcLoadSeq
	cmp r0, #0
	beq _020C9F54
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020C9EEC:
	str r4, [sp]
	ldrb r1, [r8, #1]
	ldr r0, [r8, #4]
	mov r2, sb
	mov r3, #1
	bl NNSi_SndArcLoadSeqArc
	cmp r0, #0
	beq _020C9F54
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020C9F10:
	str sl, [sp]
	ldrb r1, [r8, #1]
	ldr r0, [r8, #4]
	mov r2, sb
	mov r3, #1
	bl NNSi_SndArcLoadBank
	cmp r0, #0
	beq _020C9F54
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020C9F34:
	str fp, [sp]
	ldrb r1, [r8, #1]
	ldr r0, [r8, #4]
	mov r2, sb
	mov r3, #1
	bl NNSi_SndArcLoadWaveArc
	cmp r0, #0
	ldmneia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020C9F54:
	ldr r0, [r6]
	add r7, r7, #1
	cmp r7, r0
	add r8, r8, #8
	blo _020C9EA8
_020C9F68:
	mov r0, #0
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	arm_func_end NNSi_SndArcLoadGroup
