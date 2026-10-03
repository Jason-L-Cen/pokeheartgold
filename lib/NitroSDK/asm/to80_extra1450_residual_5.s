	.extern MB_AddGameInfo
	.extern MBi_AttachCacheBuffer
	.extern MBi_InitCache
	.extern MBi_InitTaskInfo
	.extern MBi_InitTaskThread
	.extern MBi_IsStarted
	.extern MBi_IsTaskAvailable
	.extern MBi_MakeGameInfo
	.extern MBi_defaultLoadSegList
	.extern _start_AutoloadDoneCallback
	.extern mbc
	.extern mbi_seg_header
	.extern ov13_02245988
	.extern ov13_02247484
	.include "asm/macros.inc"
	.include "mb.inc"
	.include "global.inc"
	.public IsAbleToLoad
	.extern MBi_IsAbleToRecv

.text
	arm_func_start IsAbleToLoad
IsAbleToLoad: ; 0x0221DA64
	stmdb sp!, {r3, lr}
	ldr r3, _0221DB24 ; =MBi_defaultLoadSegList
	ldr r3, [r3, r0, lsl #2]
	cmp r3, #0
	beq _0221DA88
	cmp r3, #1
	beq _0221DA90
	cmp r3, #2
	bne _0221DB14
_0221DA88:
	bl MBi_IsAbleToRecv
	ldmia sp!, {r3, pc}
_0221DA90:
	cmp r1, #0x2000000
	blo _0221DAE8
	ldr r0, _0221DB28 ; =0x023FE800
	cmp r1, r0
	bhs _0221DAE8
	cmp r1, #0x2300000
	add r1, r1, r2
	bhs _0221DABC
	cmp r1, #0x2300000
	movhi r0, #0
	ldmhiia sp!, {r3, pc}
_0221DABC:
	cmp r1, #0x2300000
	movls r0, #1
	ldmlsia sp!, {r3, pc}
	ldr r0, _0221DB28 ; =0x023FE800
	cmp r1, r0
	bhs _0221DAE0
	cmp r2, #0x40000
	movls r0, #1
	ldmlsia sp!, {r3, pc}
_0221DAE0:
	mov r0, #0
	ldmia sp!, {r3, pc}
_0221DAE8:
	ldr r3, _0221DB2C ; =0x037F8000
	cmp r1, r3
	blo _0221DB1C
	add r0, r3, #0x17000
	cmp r1, r0
	bhs _0221DB1C
	add r1, r1, r2
	cmp r1, r0
	movls r0, #1
	movhi r0, #0
	ldmia sp!, {r3, pc}
_0221DB14:
	mov r0, #0
	ldmia sp!, {r3, pc}
_0221DB1C:
	mov r0, #0
	ldmia sp!, {r3, pc}
	.balign 4, 0
_0221DB24: .word MBi_defaultLoadSegList
_0221DB28: .word 0x023FE800
_0221DB2C: .word 0x037F8000
	arm_func_end IsAbleToLoad
