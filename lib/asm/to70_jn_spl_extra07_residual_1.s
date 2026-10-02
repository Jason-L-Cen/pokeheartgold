	.include "asm/macros.inc"
	.include "jn_spl.inc"
	.include "global.inc"
	.extern spl_calc
	.extern spl_del
	.extern spl_push_front

.text
	arm_func_start SPL_Calc
SPL_Calc: ; 0x0209829C
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, lr}
	mov r8, r0
	ldr r7, [r8, #4]
	cmp r7, #0
	beq _020983B0
	add sl, r8, #4
	add sb, r8, #0x10
	mov r4, #0
_020982BC:
	ldr r2, [r7, #0x24]
	ldr r1, [r7, #0x20]
	mov r0, r2, lsl #0x1b
	movs r0, r0, lsr #0x1f
	ldr r5, [r1]
	ldr r6, [r7]
	bne _020982F0
	ldrh r1, [r7, #0x4c]
	ldrh r0, [r5, #0x32]
	cmp r1, r0
	orrhs r0, r2, #0x10
	strhs r0, [r7, #0x24]
	strhsh r4, [r7, #0x4c]
_020982F0:
	ldr r0, [r7, #0x24]
	mov r0, r0, lsl #0x1d
	movs r0, r0, lsr #0x1f
	bne _0209832C
	ldr r0, [r7, #0x80]
	mov r0, r0, lsl #0xd
	movs r0, r0, lsr #0x1d
	beq _02098320
	ldrh r1, [r8, #0x48]
	sub r0, r0, #1
	cmp r1, r0
	bne _0209832C
_02098320:
	mov r0, r8
	mov r1, r7
	bl spl_calc
_0209832C:
	ldr r0, [r5]
	mov r0, r0, lsl #0x11
	movs r0, r0, lsr #0x1f
	beq _02098364
	ldrh r1, [r5, #0x3c]
	cmp r1, #0
	beq _02098364
	ldr r0, [r7, #0x24]
	mov r0, r0, lsl #0x1b
	movs r0, r0, lsr #0x1f
	beq _02098364
	ldrh r0, [r7, #0x4c]
	cmp r0, r1
	bhi _02098374
_02098364:
	ldr r0, [r7, #0x24]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	beq _020983A4
_02098374:
	ldr r0, [r7, #0xc]
	cmp r0, #0
	bne _020983A4
	ldr r0, [r7, #0x18]
	cmp r0, #0
	bne _020983A4
	mov r0, sl
	mov r1, r7
	bl spl_del
	mov r1, r0
	mov r0, sb
	bl spl_push_front
_020983A4:
	mov r7, r6
	cmp r6, #0
	bne _020982BC
_020983B0:
	ldrh r0, [r8, #0x48]
	add r0, r0, #1
	strh r0, [r8, #0x48]
	ldrh r0, [r8, #0x48]
	cmp r0, #1
	movhi r0, #0
	strhih r0, [r8, #0x48]
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, lr}
	bx lr
	arm_func_end SPL_Calc
