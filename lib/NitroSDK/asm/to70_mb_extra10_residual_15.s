	.extern _start_AutoloadDoneCallback
	.include "asm/macros.inc"
	.include "mb.inc"
	.include "global.inc"
	.public MBi_AttachCacheBuffer
	.public MBi_InitTaskThread
	.public MBi_ReadFromCache
	.extern MBi_InitTaskInfo
	.extern ov13_0224CF80

.text
	arm_func_start MBi_AttachCacheBuffer
MBi_AttachCacheBuffer: ; 0x02220148
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, lr}
	mov r4, r0
	mov sb, r1
	mov r8, r2
	mov r7, r3
	bl OS_DisableInterrupts
	add r6, r4, #0x30
	mov r5, r0
	add r4, r4, #0x70
_0222016C:
	cmp r6, r4
	blo _02220178
	bl OS_Terminate
_02220178:
	ldr r0, [r6, #0xc]
	cmp r0, #0
	bne _0222019C
	str sb, [r6]
	str r8, [r6, #4]
	ldr r0, [sp, #0x20]
	str r7, [r6, #8]
	str r0, [r6, #0xc]
	b _022201A4
_0222019C:
	add r6, r6, #0x10
	b _0222016C
_022201A4:
	mov r0, r5
	bl OS_RestoreInterrupts
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	arm_func_end MBi_AttachCacheBuffer

	arm_func_start MBi_ReadFromCache
MBi_ReadFromCache: ; 0x022201B0
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, lr}
	mov sb, r0
	mov r8, r1
	mov r7, r2
	mov r6, r3
	mov r4, #0
	bl OS_DisableInterrupts
	add r2, sb, #0x30
	add r1, sb, #0x70
	mov r5, r0
	cmp r2, r1
	bhs _02220238
_022201E0:
	ldr r0, [r2, #0xc]
	cmp r0, #2
	blo _0222022C
	ldr r0, [r2]
	subs r0, r8, r0
	bmi _0222022C
	ldr r3, [r2, #4]
	add ip, r0, r6
	cmp ip, r3
	bhi _0222022C
	ldr r3, [r2, #8]
	mov r1, r7
	mov r2, r6
	add r0, r3, r0
	bl MI_CpuCopy8
	mov r0, #0
	str r0, [sb]
	mov r4, #1
	b _02220238
_0222022C:
	add r2, r2, #0x10
	cmp r2, r1
	blo _022201E0
_02220238:
	mov r0, r5
	bl OS_RestoreInterrupts
	mov r0, r4
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	arm_func_end MBi_ReadFromCache

	arm_func_start MBi_TaskThread
MBi_TaskThread: ; 0x02220248
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	mov r7, r0
_02220250:
	bl OS_DisableInterrupts
	ldr r1, [r7, #0xc0]
	mov r5, r0
	cmp r1, #0
	bne _0222028C
	mov r6, #0
	mov r4, r6
_0222026C:
	mov r0, r7
	mov r1, r6
	bl OS_SetThreadPriority
	mov r0, r4
	bl OS_SleepThread
	ldr r0, [r7, #0xc0]
	cmp r0, #0
	beq _0222026C
_0222028C:
	ldr r4, [r7, #0xc0]
	ldr r1, [r7, #0xc0]
	mov r0, r7
	ldr r1, [r1]
	str r1, [r7, #0xc0]
	ldr r1, [r4, #4]
	mov r1, r1, lsr #1
	bl OS_SetThreadPriority
	mov r0, r5
	bl OS_RestoreInterrupts
	ldr r1, [r4, #8]
	cmp r1, #0
	beq _022202C8
	mov r0, r4
	blx r1
_022202C8:
	bl OS_DisableInterrupts
	mov r5, r0
	mov r0, r7
	ldr r6, [r4, #0xc]
	bl OS_GetThreadPriority
	ldr r1, [r7, #0xc0]
	cmp r1, #0
	moveq r1, #0
	beq _0222030C
	ldr r1, [r7, #0xc0]
	ldr r1, [r1, #4]
	cmp r0, r1, lsr #1
	movhs r1, r0
	bhs _0222030C
	ldr r1, [r7, #0xc0]
	ldr r1, [r1, #4]
	mov r1, r1, lsr #1
_0222030C:
	cmp r1, r0
	beq _0222031C
	mov r0, r7
	bl OS_SetThreadPriority
_0222031C:
	mov r0, #0
	str r0, [r4]
	ldr r0, [r4, #4]
	cmp r6, #0
	bic r0, r0, #1
	str r0, [r4, #4]
	beq _02220340
	mov r0, r4
	blx r6
_02220340:
	add r0, r7, #0xc4
	cmp r4, r0
	beq _02220358
	mov r0, r5
	bl OS_RestoreInterrupts
	b _02220250
_02220358:
	bl OS_ExitThread
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	arm_func_end MBi_TaskThread

	arm_func_start MBi_InitTaskThread
MBi_InitTaskThread: ; 0x02220360
	stmdb sp!, {r4, r5, r6, lr}
	sub sp, sp, #8
	mov r5, r0
	mov r6, r1
	bl OS_DisableInterrupts
	ldr r1, _022203D8 ; =ov13_0224CF80
	mov r4, r0
	ldr r0, [r1]
	cmp r0, #0
	bne _022203C8
	add r0, r5, #0xc4
	str r5, [r1]
	bl MBi_InitTaskInfo
	sub r0, r6, #0xe4
	mov lr, #0
	bic ip, r0, #3
	add r3, r5, #0xe4
	str lr, [r5, #0xc0]
	ldr r1, _022203DC ; =MBi_TaskThread
	mov r0, r5
	mov r2, r5
	add r3, r3, ip
	stmia sp, {ip, lr}
	bl OS_CreateThread
	mov r0, r5
	bl OS_WakeupThreadDirect
_022203C8:
	mov r0, r4
	bl OS_RestoreInterrupts
	add sp, sp, #8
	ldmia sp!, {r4, r5, r6, pc}
	.balign 4, 0
_022203D8: .word ov13_0224CF80
_022203DC: .word MBi_TaskThread
	arm_func_end MBi_InitTaskThread
