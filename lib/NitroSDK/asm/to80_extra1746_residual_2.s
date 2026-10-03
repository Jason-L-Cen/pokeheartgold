	.extern MB_SendGameInfoBeacon
	.extern MB_UpdateGameInfoMember
	.extern MBi_BlockHeaderEnd
	.extern MBi_ClearParentPieceBuffer
	.extern MBi_CommCallParentError
	.extern MBi_CommChangeParentState
	.extern MBi_CommParentSendDLFileInfo
	.extern MBi_CommParentSendMsg
	.extern MBi_GetAttribute
	.extern MBi_GetGgid
	.extern MBi_GetTgid
	.extern MBi_IsTaskBusy
	.extern MBi_MakeParentSendBuffer
	.extern MBi_ReadFromCache
	.extern MBi_ReloadCache
	.extern MBi_SetRecvBufferFromChild
	.extern MBi_SetTask
	.extern MBi_calc_nextsendblock
	.extern MBi_get_blockinfo
	.extern _02245980
	.extern _02247480
	.extern _start_AutoloadDoneCallback
	.extern mbc
	.include "asm/macros.inc"
	.include "mb.inc"
	.include "global.inc"
	.public MBi_calc_sendblock

.text
	arm_func_start MBi_calc_sendblock
MBi_calc_sendblock: ; 0x0221CD8C
	ldr r1, _0221CDFC ; =_02247480
	mov r2, #1
	ldr r1, [r1]
	tst r1, r2, lsl r0
	bxeq lr
	ldr r2, _0221CE00 ; =mbc
	ldr r1, _0221CE04 ; =0x000005D4
	ldr r2, [r2]
	mla ip, r0, r1, r2
	add r0, ip, #0x1000
	ldrb r0, [r0, #0xd52]
	cmp r0, #0
	addne r0, ip, #0x1d00
	ldrneh r1, [r0, #0x4c]
	cmpne r1, #0
	bxeq lr
	ldrh r2, [r0, #0x48]
	ldrh r3, [r0, #0x4a]
	cmp r3, r2
	bhi _0221CDF0
	add r1, r3, #2
	cmp r2, r1
	addle r1, r2, #1
	strleh r1, [r0, #0x48]
	bxle lr
_0221CDF0:
	add r0, ip, #0x1d00
	strh r3, [r0, #0x48]
	bx lr
	.balign 4, 0
_0221CDFC: .word _02247480
_0221CE00: .word mbc
_0221CE04: .word 0x000005D4
	arm_func_end MBi_calc_sendblock
