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
	.public MBi_CommParentSendBlock
	.extern MBi_calc_sendblock

.text
	arm_func_start MBi_CommParentSendBlock
MBi_CommParentSendBlock: ; 0x0221C970
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	sub sp, sp, #0x18
	ldr r5, _0221CC00 ; =mbc
	ldr r0, [r5]
	add r0, r0, #0x1000
	ldrb r0, [r0, #0x524]
	cmp r0, #0
	addeq sp, sp, #0x18
	moveq r0, #0x15
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	ldr r4, _0221CC04 ; =0x000005D4
	mov r1, #0
_0221C9A0:
	ldr r0, [r5]
	add r3, r0, #0x1000
	ldrb r0, [r3, #0x525]
	add r0, r0, #1
	mov r2, r0, lsr #0x1f
	rsb r0, r2, r0, lsl #28
	add r0, r2, r0, ror #28
	strb r0, [r3, #0x525]
	ldr r2, [r5]
	add r0, r2, #0x1000
	ldrb r0, [r0, #0x525]
	mla r3, r0, r4, r2
	add r2, r3, #0x1000
	ldrb r2, [r2, #0xd52]
	cmp r2, #0
	addne r2, r3, #0x1d00
	ldrneh r2, [r2, #0x4c]
	cmpne r2, #0
	bne _0221C9FC
	add r1, r1, #1
	and r1, r1, #0xff
	cmp r1, #0x10
	blo _0221C9A0
_0221C9FC:
	cmp r1, #0x10
	addeq sp, sp, #0x18
	moveq r0, #0x15
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	bl MBi_calc_sendblock
	ldr r0, _0221CC00 ; =mbc
	ldr r2, _0221CC04 ; =0x000005D4
	ldr r6, [r0]
	add r0, sp, #8
	add r1, r6, #0x1000
	ldrb r4, [r1, #0x525]
	add r1, r6, #0x12c
	add r3, r6, #0x388
	mul r5, r4, r2
	add r2, r6, r5
	add r2, r2, #0x1d00
	add r1, r1, #0x1c00
	add r3, r3, #0x1400
	ldrh r2, [r2, #0x48]
	add r1, r1, r5
	add r3, r3, r5
	bl MBi_get_blockinfo
	cmp r0, #0
	addeq sp, sp, #0x18
	moveq r0, #0x15
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	ldr r0, _0221CC00 ; =mbc
	mov r3, #4
	ldr r1, [r0]
	strb r3, [sp]
	add r2, r1, #0x1000
	ldrb r4, [r2, #0x525]
	add r3, r3, #0x5d0
	add r0, sp, #0
	strh r4, [sp, #2]
	ldrb r4, [r2, #0x525]
	mla r2, r4, r3, r1
	add r2, r2, #0x1d00
	ldrh r2, [r2, #0x48]
	strh r2, [sp, #4]
	bl MBi_MakeParentSendBuffer
	ldr r1, _0221CC00 ; =mbc
	ldr r4, _0221CC04 ; =0x000005D4
	ldr r5, [r1]
	ldrb r1, [sp, #0x14]
	add r2, r5, #0x1000
	ldrb r2, [r2, #0x525]
	ldr r6, [sp, #0x10]
	ldr r3, [sp, #0xc]
	mla r4, r2, r4, r5
	add r2, r4, r1, lsl #2
	add r5, r4, #0x1000
	ldr r4, [r5, #0xd58]
	add r2, r2, #0x1000
	ldr r2, [r2, #0xd2c]
	ldr r1, [r4, r1, lsl #2]
	sub r2, r6, r2
	add r4, r2, r1
	ldr r5, [r5, #0xd54]
	mov r2, r0
	mov r0, r5
	mov r1, r4
	bl MBi_ReadFromCache
	cmp r0, #0
	bne _0221CBCC
	ldr r0, _0221CC00 ; =mbc
	ldr r0, [r0]
	add r0, r0, #0xce0
	add r6, r0, #0x7000
	mov r0, r6
	bl MBi_IsTaskBusy
	cmp r0, #0
	bne _0221CBC0
	ldr r0, [r5]
	cmp r0, #0
	subne r0, r0, #1
	strne r0, [r5]
	bne _0221CBC0
	add r2, r5, #0x30
	mov r7, #0
	mov r3, r7
	mov ip, r2
_0221CB44:
	add r0, r2, r3, lsl #4
	ldr r0, [r0, #0xc]
	cmp r0, #2
	bne _0221CB70
	cmp r7, #0
	beq _0221CB6C
	ldr r1, [r7]
	ldr r0, [r2, r3, lsl #4]
	cmp r1, r0
	bls _0221CB70
_0221CB6C:
	mov r7, ip
_0221CB70:
	add r3, r3, #1
	cmp r3, #4
	add ip, ip, #0x10
	blt _0221CB44
	cmp r7, #0
	bne _0221CB8C
	bl OS_Terminate
_0221CB8C:
	mov r0, #2
	str r0, [r5]
	mov r0, #1
	str r0, [r7, #0xc]
	bic r0, r4, #0x1f
	str r0, [r7]
	str r7, [r6, #0x10]
	ldr r1, _0221CC08 ; =MBi_ReloadCache
	mov r0, r6
	mov r2, #0
	mov r3, #4
	str r5, [r6, #0x14]
	bl MBi_SetTask
_0221CBC0:
	add sp, sp, #0x18
	mov r0, #0x15
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_0221CBCC:
	ldr r0, _0221CC00 ; =mbc
	ldr r4, [sp, #0xc]
	ldr r2, [r0]
	ldr r1, _0221CC04 ; =0x000005D4
	add r0, r2, #0x1000
	ldrb r3, [r0, #0x525]
	add r0, r4, #6
	mla r1, r3, r1, r2
	add r1, r1, #0x1d00
	ldrh r1, [r1, #0x4c]
	bl MBi_BlockHeaderEnd
	add sp, sp, #0x18
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.balign 4, 0
_0221CC00: .word mbc
_0221CC04: .word 0x000005D4
_0221CC08: .word MBi_ReloadCache
	arm_func_end MBi_CommParentSendBlock
