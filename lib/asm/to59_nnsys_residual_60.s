	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"
	.public NNS_SndHeapClear
	.extern EraseSync
	.extern NNS_FndFreeToFrmHeap
	.extern NNS_FndGetPrevListObject
	.extern NNS_FndRemoveListObject
	.extern NewSection

.text
	arm_func_start NNS_SndHeapClear
NNS_SndHeapClear: ; 0x020C9A94
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, lr}
	mov r8, r0
	mov r6, #0
	mov r1, r6
	add r0, r8, #4
	bl NNS_FndGetPrevListObject
	movs r5, r0
	beq _020C9B2C
	mov sl, #1
	mov sb, r6
	mov r4, r6
_020C9AC0:
	mov r0, r5
	mov r1, r4
	bl NNS_FndGetPrevListObject
	movs r7, r0
	beq _020C9B0C
_020C9AD4:
	ldr ip, [r7, #0xc]
	cmp ip, #0
	beq _020C9AF8
	ldr r1, [r7, #8]
	ldr r2, [r7, #0x10]
	ldr r3, [r7, #0x14]
	add r0, r7, #0x20
	blx ip
	mov r6, sl
_020C9AF8:
	mov r0, r5
	mov r1, r7
	bl NNS_FndGetPrevListObject
	movs r7, r0
	bne _020C9AD4
_020C9B0C:
	mov r1, r5
	add r0, r8, #4
	bl NNS_FndRemoveListObject
	mov r1, sb
	add r0, r8, #4
	bl NNS_FndGetPrevListObject
	movs r5, r0
	bne _020C9AC0
_020C9B2C:
	ldr r0, [r8]
	mov r1, #3
	bl NNS_FndFreeToFrmHeap
	cmp r6, #0
	beq _020C9B44
	bl EraseSync
_020C9B44:
	mov r0, r8
	bl NewSection
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
	arm_func_end NNS_SndHeapClear
