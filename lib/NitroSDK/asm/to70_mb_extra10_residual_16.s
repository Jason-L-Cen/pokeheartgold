	.extern _start_AutoloadDoneCallback
	.include "asm/macros.inc"
	.include "mb.inc"
	.include "global.inc"
	.public MBi_IsTaskBusy
	.public MBi_SetTask
	.extern MBi_IsTaskAvailable
	.extern ov13_0224CF80

.text
	arm_func_start MBi_IsTaskBusy
MBi_IsTaskBusy: ; 0x02220410
	ldr r0, [r0, #4]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	movne r0, #1
	moveq r0, #0
	bx lr
	arm_func_end MBi_IsTaskBusy

	arm_func_start MBi_SetTask
MBi_SetTask: ; 0x02220428
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, lr}
	ldr r4, _02220588 ; =ov13_0224CF80
	mov sb, r0
	ldr r4, [r4]
	mov r8, r1
	mov r7, r2
	mov r6, r3
	bl MBi_IsTaskAvailable
	cmp r0, #0
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	ldr r0, [sb, #4]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	ldmneia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	cmp r6, #0x1f
	bls _022204AC
	mov r0, r4
	bl OS_GetThreadPriority
	cmp r6, #0x20
	bne _02220488
	cmp r0, #0
	subne r6, r0, #1
	moveq r6, #0
	b _022204AC
_02220488:
	cmp r6, #0x21
	bne _022204A0
	cmp r0, #0x1f
	addlo r6, r0, #1
	movhs r6, #0x1f
	b _022204AC
_022204A0:
	cmp r6, #0x22
	moveq r6, r0
	movne r6, #0x1f
_022204AC:
	bl OS_DisableInterrupts
	ldr r1, [sb, #4]
	mov r5, r0
	bic r0, r1, #1
	orr r1, r0, #1
	and r0, r1, #1
	str r1, [sb, #4]
	orr r0, r0, r6, lsl #1
	stmib sb, {r0, r8}
	str r7, [sb, #0xc]
	ldr r0, [r4, #0xc0]
	cmp r0, #0
	add r0, r4, #0xc4
	bne _02220504
	cmp sb, r0
	ldreq r0, _02220588 ; =ov13_0224CF80
	moveq r1, #0
	streq r1, [r0]
	mov r0, r4
	str sb, [r4, #0xc0]
	bl OS_WakeupThreadDirect
	b _0222057C
_02220504:
	cmp sb, r0
	ldr r1, [r4, #0xc0]
	bne _02220540
	ldr r0, [r1]
	cmp r0, #0
	beq _0222052C
_0222051C:
	mov r1, r0
	ldr r0, [r0]
	cmp r0, #0
	bne _0222051C
_0222052C:
	ldr r0, _02220588 ; =ov13_0224CF80
	str sb, [r1]
	mov r1, #0
	str r1, [r0]
	b _0222057C
_02220540:
	ldr r0, [r1, #4]
	cmp r6, r0, lsr #1
	strlo sb, [r4, #0xc0]
	strlo r1, [sb]
	blo _0222057C
	b _0222055C
_02220558:
	mov r1, r2
_0222055C:
	ldr r2, [r1]
	cmp r2, #0
	beq _02220574
	ldr r0, [r2, #4]
	cmp r6, r0, lsr #1
	bhs _02220558
_02220574:
	str r2, [sb]
	str sb, [r1]
_0222057C:
	mov r0, r5
	bl OS_RestoreInterrupts
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	.balign 4, 0
_02220588: .word ov13_0224CF80
	arm_func_end MBi_SetTask
