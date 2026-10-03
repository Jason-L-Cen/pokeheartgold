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
	.extern MBi_MakeBlockInfoTable
	.extern MBi_MakeDownloadFileInfo

.text
	arm_func_start MB_RegisterFile
MB_RegisterFile: ; 0x0221D3BC
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, lr}
	mov r8, r0
	mov r7, r1
	mov r4, #0xff
	bl OS_DisableInterrupts
	mov r5, r0
	bl MBi_IsStarted
	cmp r0, #0
	bne _0221D3F0
	mov r0, r5
	bl OS_RestoreInterrupts
	mov r0, #0
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
_0221D3F0:
	ldr r0, _0221D640 ; =mbc
	ldr r0, [r0]
	add r1, r0, #0x1000
	ldrb r1, [r1, #0x524]
	add r1, r1, #1
	cmp r1, #0x10
	ble _0221D41C
	mov r0, r5
	bl OS_RestoreInterrupts
	mov r0, #0
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
_0221D41C:
	ldr r1, _0221D644 ; =0x000005D4
	mov r6, #0
_0221D424:
	mla r2, r6, r1, r0
	add r2, r2, #0x1000
	ldr r3, [r2, #0xd40]
	cmp r3, r8
	bne _0221D448
	mov r0, r5
	bl OS_RestoreInterrupts
	mov r0, #0
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
_0221D448:
	ldrb r2, [r2, #0xd52]
	cmp r2, #0
	moveq r4, r6
	beq _0221D468
	add r2, r6, #1
	and r6, r2, #0xff
	cmp r6, #0x10
	blo _0221D424
_0221D468:
	cmp r6, #0x10
	bne _0221D480
	mov r0, r5
	bl OS_RestoreInterrupts
	mov r0, #0
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
_0221D480:
	ldr r1, _0221D644 ; =0x000005D4
	ldr r2, _0221D640 ; =mbc
	mul r6, r4, r1
	add r0, r0, r6
	add r0, r0, #0x1000
	str r8, [r0, #0xd40]
	ldr r0, [r2]
	mov r1, r7
	add r0, r0, #0x388
	add r0, r0, #0x1400
	add sb, r0, r6
	mov r0, sb
	bl MBi_MakeDownloadFileInfo
	add r0, r8, #0x1c
	add r1, sb, #0xc4
	mov r2, #0x20
	bl MI_CpuCopy8
	ldr r0, _0221D640 ; =mbc
	mov r1, sb
	ldr r0, [r0]
	add r0, r0, #0x12c
	add r0, r0, #0x1c00
	add r0, r0, r6
	bl MBi_MakeBlockInfoTable
	cmp r0, #0
	bne _0221D4F8
	mov r0, r5
	bl OS_RestoreInterrupts
	mov r0, #0
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
_0221D4F8:
	ldr r0, _0221D640 ; =mbc
	mov r1, r8
	ldr r2, [r0]
	add r0, r2, #0x6c
	add r0, r0, #0x1800
	add r0, r0, r6
	add r2, r2, #0x1300
	bl MBi_MakeGameInfo
	ldr r1, _0221D640 ; =mbc
	ldr r0, [r1]
	add r0, r0, r6
	add r0, r0, #0x1000
	strb r4, [r0, #0xd21]
	ldr r0, [r1]
	add r0, r0, #0x6c
	add r0, r0, #0x1800
	add r0, r0, r6
	bl MB_AddGameInfo
	ldr r2, _0221D640 ; =mbc
	ldr r3, _0221D648 ; =ov13_02247484
	ldr r1, [r2]
	ldrb r0, [r3]
	add r1, r1, r6
	add r1, r1, #0x1000
	strb r0, [r1, #0xd1f]
	ldr r1, [r2]
	mov r4, #1
	add r1, r1, r6
	add r1, r1, #0x1d00
	strh r4, [r1, #0x4e]
	ldr r1, [r2]
	add r8, r7, #0x1e8
	add r1, r1, r6
	add r1, r1, #0x1000
	str r7, [r1, #0xd44]
	ldr r1, [r2]
	add r4, r7, #0x258
	add r1, r1, r6
	add r1, r1, #0x1000
	str r8, [r1, #0xd54]
	ldr r1, [r2]
	add r7, r0, #1
	add r0, r1, r6
	add r0, r0, #0x1000
	str r4, [r0, #0xd58]
	ldr r0, [r2]
	strb r7, [r3]
	add r0, r0, r6
	add r0, r0, #0x1000
	ldr r0, [r0, #0xd54]
	ldr r0, [r0, #0x6c]
	cmp r0, #0
	beq _0221D604
	bl MBi_IsTaskAvailable
	cmp r0, #0
	bne _0221D604
	ldr r0, _0221D640 ; =mbc
	ldr r0, [r0]
	add r0, r0, #0xce0
	add r0, r0, #0x7000
	bl MBi_InitTaskInfo
	ldr r0, _0221D640 ; =mbc
	mov r1, #0x800
	ldr r0, [r0]
	add r0, r0, #0x4e0
	add r0, r0, #0x7000
	bl MBi_InitTaskThread
_0221D604:
	ldr r2, _0221D640 ; =mbc
	mov r3, #1
	ldr r1, [r2]
	mov r0, r5
	add r1, r1, r6
	add r1, r1, #0x1000
	strb r3, [r1, #0xd52]
	ldr r1, [r2]
	add r1, r1, #0x1000
	ldrb r2, [r1, #0x524]
	add r2, r2, #1
	strb r2, [r1, #0x524]
	bl OS_RestoreInterrupts
	mov r0, #1
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	.balign 4, 0
_0221D640: .word mbc
_0221D644: .word 0x000005D4
_0221D648: .word ov13_02247484
	arm_func_end MB_RegisterFile
