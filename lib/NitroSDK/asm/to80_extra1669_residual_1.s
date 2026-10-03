	.extern MB_SendGameInfoBeacon
	.extern MB_UpdateGameInfoMember
	.extern MBi_BlockHeaderEnd
	.extern MBi_ClearParentPieceBuffer
	.extern MBi_CommCallParentError
	.extern MBi_CommChangeParentState
	.extern MBi_CommParentSendData
	.extern MBi_GetAttribute
	.extern MBi_GetGgid
	.extern MBi_GetTgid
	.extern MBi_IsTaskBusy
	.extern MBi_MakeParentSendBuffer
	.extern MBi_ReadFromCache
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
	.public MBi_CommParentRecvDataPerChild
	.extern MBi_CommParentRecvData

.text
	arm_func_start MBi_CommParentCallback
MBi_CommParentCallback: ; 0x0221BCC4
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, lr}
	mov r5, r0
	mov r4, r1
	cmp r5, #0x19
	bgt _0221BD1C
	bge _0221BF60
	cmp r5, #0x11
	bgt _0221BD10
	bge _0221C114
	cmp r5, #3
	bgt _0221C114
	cmp r5, #0
	blt _0221C114
	beq _0221BD54
	cmp r5, #1
	beq _0221BD78
	cmp r5, #3
	beq _0221BF54
	b _0221C114
_0221BD10:
	cmp r5, #0x15
	beq _0221BD40
	b _0221C114
_0221BD1C:
	cmp r5, #0xff
	bgt _0221BD34
	bge _0221C000
	cmp r5, #0x1c
	beq _0221BF68
	b _0221C114
_0221BD34:
	cmp r5, #0x100
	beq _0221C070
	b _0221C114
_0221BD40:
	mov r2, r4
	mov r0, #0
	mov r1, #1
	bl MBi_CommChangeParentState
	b _0221C114
_0221BD54:
	ldrh r0, [r4, #0x10]
	cmp r0, #0
	beq _0221C114
	cmp r0, #0x10
	bhs _0221C114
	mov r2, r4
	mov r1, #2
	bl MBi_CommChangeParentState
	b _0221C114
_0221BD78:
	ldrh r0, [r4, #0x10]
	cmp r0, #0
	beq _0221C114
	cmp r0, #0x10
	bhs _0221C114
	ldr r2, _0221C15C ; =mbc
	sub r0, r0, #1
	ldr r3, [r2]
	mov r1, #0
	add r0, r3, r0, lsl #1
	add r0, r0, #0x1400
	strh r1, [r0, #0x8a]
	ldr r0, [r2]
	ldrh r2, [r4, #0x10]
	add r0, r0, #0xa8
	add r3, r0, #0x1400
	sub r0, r2, #1
	add r0, r3, r0, lsl #2
	mov r2, #4
	bl MI_CpuFill8
	ldr r0, _0221C15C ; =mbc
	ldrh r1, [r4, #0x10]
	ldr r0, [r0]
	mov r2, #0x16
	add r0, r0, #0x1340
	sub r1, r1, #1
	mla r0, r1, r2, r0
	mov r1, #0
	bl MI_CpuFill8
	ldrh r0, [r4, #0x10]
	bl MBi_ClearParentPieceBuffer
	ldrh r1, [r4, #0x10]
	ldr r0, _0221C15C ; =mbc
	mov r3, #0
	ldr r2, [r0]
	sub r1, r1, #1
	add r1, r2, r1, lsl #1
	add r1, r1, #0x1700
	strh r3, [r1, #0x54]
	ldrh r7, [r4, #0x10]
	ldr r8, [r0]
	sub r2, r3, #1
	sub r6, r7, #1
	add r1, r8, r6
	add r1, r1, #0x1500
	ldrsb r3, [r1, #0x26]
	cmp r3, r2
	beq _0221BEA4
	ldr r1, _0221C160 ; =0x000005D4
	and r2, r3, #0xff
	mul r1, r2, r1
	add r2, r8, r1
	add r8, r2, #0x1d00
	mov r3, #1
	ldrh sb, [r8, #0x4e]
	mvn r2, r3, lsl r7
	and sb, sb, r2
	strh sb, [r8, #0x4e]
	ldr r8, [r0]
	sub sb, r3, #2
	add r8, r8, r1
	add r8, r8, #0x1d00
	ldrh ip, [r8, #0x50]
	orr r3, ip, r3, lsl r7
	strh r3, [r8, #0x50]
	ldr r3, [r0]
	add r3, r3, r6
	add r3, r3, #0x1000
	strb sb, [r3, #0x526]
	ldr r0, [r0]
	add r0, r0, r1
	add r0, r0, #0x1d00
	ldrh r1, [r0, #0x4c]
	and r1, r1, r2
	strh r1, [r0, #0x4c]
_0221BEA4:
	ldr r1, _0221C15C ; =mbc
	ldrh r2, [r4, #0x10]
	ldr r6, [r1]
	mov r3, #1
	add r0, r6, #0x1500
	ldrh r0, [r0, #0x36]
	tst r0, r3, lsl r2
	beq _0221BEF0
	add r0, r6, #0x1000
	ldrb r2, [r0, #0x535]
	sub r2, r2, #1
	strb r2, [r0, #0x535]
	ldr r0, [r1]
	ldrh r1, [r4, #0x10]
	add r0, r0, #0x1500
	ldrh r2, [r0, #0x36]
	mvn r1, r3, lsl r1
	and r1, r2, r1
	strh r1, [r0, #0x36]
_0221BEF0:
	ldrh r0, [r4, #0x10]
	ldr r1, _0221C15C ; =mbc
	ldr r2, [r1]
	sub r1, r0, #1
	add r1, r2, r1, lsl #2
	add r1, r1, #0x1000
	ldr r1, [r1, #0x4e8]
	cmp r1, #8
	bne _0221BF20
	mov r1, #9
	mov r2, #0
	bl MBi_CommChangeParentState
_0221BF20:
	ldrh r0, [r4, #0x10]
	mov r2, r4
	mov r1, #3
	bl MBi_CommChangeParentState
	ldrh r1, [r4, #0x10]
	ldr r0, _0221C15C ; =mbc
	mov r3, #0
	ldr r2, [r0]
	sub r0, r1, #1
	add r0, r2, r0, lsl #2
	add r0, r0, #0x1000
	str r3, [r0, #0x4e8]
	b _0221C114
_0221BF54:
	mov r0, r4
	bl MBi_CommParentRecvData
	b _0221C114
_0221BF60:
	bl MBi_CommParentSendData
	b _0221C114
_0221BF68:
	mov r8, #0
	ldr r6, _0221C15C ; =mbc
	ldr r4, _0221C160 ; =0x000005D4
	mov r7, r8
_0221BF78:
	mul sb, r8, r4
	ldr r1, [r6]
	add r2, r1, sb
	add r0, r2, #0x1000
	ldrb r0, [r0, #0xd52]
	cmp r0, #0
	addne r2, r2, #0x1d00
	ldrneh r3, [r2, #0x50]
	cmpne r3, #0
	beq _0221BFC8
	add r0, r1, #0x6c
	add r0, r0, #0x1800
	ldrh r2, [r2, #0x4e]
	add r0, r0, sb
	add r1, r1, #0x1340
	bl MB_UpdateGameInfoMember
	ldr r0, [r6]
	add r0, r0, sb
	add r0, r0, #0x1d00
	strh r7, [r0, #0x50]
_0221BFC8:
	add r0, r8, #1
	and r8, r0, #0xff
	cmp r8, #0x10
	blo _0221BF78
	bl MBi_GetGgid
	mov r6, r0
	bl MBi_GetTgid
	mov r4, r0
	bl MBi_GetAttribute
	mov r2, r0
	mov r0, r6
	mov r1, r4
	bl MB_SendGameInfoBeacon
	b _0221C114
_0221C000:
	ldrh r0, [r4, #2]
	cmp r0, #0xf
	addls pc, pc, r0, lsl #2
	b _0221C060
_0221C010: ; jump table
	b _0221C060 ; case 0
	b _0221C050 ; case 1
	b _0221C060 ; case 2
	b _0221C060 ; case 3
	b _0221C050 ; case 4
	b _0221C050 ; case 5
	b _0221C050 ; case 6
	b _0221C060 ; case 7
	b _0221C050 ; case 8
	b _0221C050 ; case 9
	b _0221C060 ; case 10
	b _0221C060 ; case 11
	b _0221C060 ; case 12
	b _0221C060 ; case 13
	b _0221C060 ; case 14
	b _0221C060 ; case 15
_0221C050:
	mov r0, #0
	mov r1, #9
	bl MBi_CommCallParentError
	b _0221C114
_0221C060:
	mov r0, #0
	mov r1, #8
	bl MBi_CommCallParentError
	b _0221C114
_0221C070:
	ldrh r0, [r4]
	cmp r0, #0x1d
	addls pc, pc, r0, lsl #2
	b _0221C108
_0221C080: ; jump table
	b _0221C0F8 ; case 0
	b _0221C108 ; case 1
	b _0221C108 ; case 2
	b _0221C108 ; case 3
	b _0221C108 ; case 4
	b _0221C108 ; case 5
	b _0221C108 ; case 6
	b _0221C0F8 ; case 7
	b _0221C0F8 ; case 8
	b _0221C108 ; case 9
	b _0221C108 ; case 10
	b _0221C108 ; case 11
	b _0221C108 ; case 12
	b _0221C0F8 ; case 13
	b _0221C0F8 ; case 14
	b _0221C0F8 ; case 15
	b _0221C108 ; case 16
	b _0221C0F8 ; case 17
	b _0221C0F8 ; case 18
	b _0221C108 ; case 19
	b _0221C108 ; case 20
	b _0221C0F8 ; case 21
	b _0221C108 ; case 22
	b _0221C108 ; case 23
	b _0221C108 ; case 24
	b _0221C0F8 ; case 25
	b _0221C108 ; case 26
	b _0221C108 ; case 27
	b _0221C108 ; case 28
	b _0221C0F8 ; case 29
_0221C0F8:
	mov r0, #0
	mov r1, #9
	bl MBi_CommCallParentError
	b _0221C114
_0221C108:
	mov r0, #0
	mov r1, #8
	bl MBi_CommCallParentError
_0221C114:
	cmp r5, #0x11
	ldmneia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	ldr r1, _0221C15C ; =mbc
	mov r0, #0
	ldr r1, [r1]
	mov r2, #0x7d00
	add r3, r1, #0x1000
	ldr r4, [r3, #0x4e4]
	bl MIi_CpuClearFast
	ldr r1, _0221C15C ; =mbc
	mov r0, #0
	str r0, [r1]
	cmp r4, #0
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	mov r2, r0
	mov r1, #0xc
	blx r4
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	.balign 4, 0
_0221C15C: .word mbc
_0221C160: .word 0x000005D4
	arm_func_end MBi_CommParentCallback

	arm_func_start MBi_CommParentRecvDataPerChild
MBi_CommParentRecvDataPerChild: ; 0x0221C164
	stmdb sp!, {r3, r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0x34
	movs r6, r1
	addeq sp, sp, #0x34
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, pc}
	cmp r6, #0xf
	addhi sp, sp, #0x34
	ldmhiia sp!, {r3, r4, r5, r6, r7, r8, pc}
	add r1, sp, #0x20
	mov r2, r6
	add r0, r0, #0xa
	bl MBi_SetRecvBufferFromChild
	ldr r1, _0221C63C ; =mbc
	ldrb r7, [sp, #0x20]
	ldr r2, [r1]
	sub r3, r6, #1
	add r4, r2, r3, lsl #2
	add r4, r4, #0x1000
	ldr r4, [r4, #0x4e8]
	mov r5, r0
	cmp r7, #0xb
	addls pc, pc, r7, lsl #2
	b _0221C634
_0221C1C0: ; jump table
	b _0221C634 ; case 0
	b _0221C634 ; case 1
	b _0221C634 ; case 2
	b _0221C634 ; case 3
	b _0221C634 ; case 4
	b _0221C634 ; case 5
	b _0221C634 ; case 6
	b _0221C1F0 ; case 7
	b _0221C488 ; case 8
	b _0221C530 ; case 9
	b _0221C5A0 ; case 10
	b _0221C634 ; case 11
_0221C1F0:
	cmp r4, #2
	bne _0221C298
	cmp r5, #0
	addeq sp, sp, #0x34
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, pc}
	add r1, sp, #0
	mov r2, #0x1d
	bl MI_CpuCopy8
	ldr r2, _0221C63C ; =mbc
	sub r0, r6, #1
	ldr r1, [r2]
	ldr r3, [sp]
	add r1, r1, r0, lsl #2
	add r1, r1, #0x1000
	str r3, [r1, #0x4a8]
	ldr r1, [r2]
	ldrh r3, [sp, #0x1a]
	add r1, r1, r0, lsl #1
	add r1, r1, #0x1400
	strh r3, [r1, #0x8a]
	ldr r1, [r2]
	mov r2, #0x16
	add r1, r1, #0x1340
	mla r1, r0, r2, r1
	add r0, sp, #4
	bl MI_CpuCopy8
	ldr r0, _0221C63C ; =mbc
	sub r2, r6, #1
	ldr r3, [r0]
	mov r0, #0x16
	mul r7, r2, r0
	add r8, r3, #0x1340
	and r1, r6, #0xff
	ldrb r3, [r8, r7]
	mov r0, r1, lsl #0x1c
	add r2, sp, #4
	bic r1, r3, #0xf0
	orr r1, r1, r0, lsr #24
	strb r1, [r8, r7]
	mov r0, r6
	mov r1, #0xa
	bl MBi_CommChangeParentState
_0221C298:
	cmp r4, #0xa
	addne sp, sp, #0x34
	ldmneia sp!, {r3, r4, r5, r6, r7, r8, pc}
	ldrb lr, [r5, #0x1c]
	mov r0, #0
	cmp lr, #0x10
	bhs _0221C2F4
	ldr r2, _0221C63C ; =mbc
	ldr r1, _0221C640 ; =0x000005D4
	ldr r3, [r2]
	mla r4, lr, r1, r3
	add r1, r4, #0x1000
	ldrb r2, [r1, #0xd52]
	cmp r2, #0
	beq _0221C2F4
	sub r2, r6, #1
	add r3, r3, r2, lsl #2
	ldr r2, [r1, #0xd40]
	add r1, r3, #0x1000
	ldr r3, [r1, #0x4a8]
	ldr r1, [r2, #0x14]
	cmp r3, r1
	beq _0221C314
_0221C2F4:
	ldr r0, _0221C63C ; =mbc
	sub r1, r6, #1
	ldr r0, [r0]
	mov r2, #4
	add r0, r0, r1, lsl #1
	add r0, r0, #0x1700
	strh r2, [r0, #0x54]
	b _0221C390
_0221C314:
	mov r5, r0
	add r1, r4, #0x1d00
	mov r3, #1
	b _0221C33C
_0221C324:
	ldrh r2, [r1, #0x4e]
	tst r2, r3, lsl r5
	addne r0, r0, #1
	add r2, r5, #1
	andne r0, r0, #0xff
	and r5, r2, #0xff
_0221C33C:
	cmp r5, #0x10
	blo _0221C324
	ldr r2, _0221C63C ; =mbc
	ldr r1, _0221C640 ; =0x000005D4
	ldr r2, [r2]
	mla r1, lr, r1, r2
	add r1, r1, #0x1000
	ldr r1, [r1, #0xd40]
	ldrb r1, [r1, #0x18]
	cmp r0, r1
	blo _0221C390
	sub r0, r6, #1
	add r1, r2, r0, lsl #1
	add r3, r1, #0x1700
	mov r2, #0
	mov r0, r6
	mov r1, #0xb
	strh r2, [r3, #0x54]
	bl MBi_CommChangeParentState
	add sp, sp, #0x34
	ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
_0221C390:
	ldr r3, _0221C63C ; =mbc
	sub ip, r6, #1
	ldr r1, [r3]
	add r0, r1, ip, lsl #1
	add r4, r0, #0x1700
	ldrh r0, [r4, #0x54]
	cmp r0, #3
	beq _0221C3C0
	cmp r0, #4
	beq _0221C46C
	add sp, sp, #0x34
	ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
_0221C3C0:
	add r0, r1, #0x1500
	ldrh r0, [r0, #0x36]
	mov r5, #1
	tst r0, r5, lsl r6
	addne sp, sp, #0x34
	ldmneia sp!, {r3, r4, r5, r6, r7, r8, pc}
	add r1, r1, #0x1000
	ldrb r4, [r1, #0x535]
	ldr r2, _0221C640 ; =0x000005D4
	mov r0, r6
	add r4, r4, #1
	strb r4, [r1, #0x535]
	ldr r1, [r3]
	mul r4, lr, r2
	add r7, r1, #0x1500
	ldrh r8, [r7, #0x36]
	mov r2, #0
	mov r1, #5
	orr r8, r8, r5, lsl r6
	strh r8, [r7, #0x36]
	ldr r7, [r3]
	add r7, r7, ip
	add r7, r7, #0x1000
	strb lr, [r7, #0x526]
	ldr r7, [r3]
	add r7, r7, #0x4e
	add lr, r7, #0x1d00
	ldrh r7, [lr, r4]
	orr r7, r7, r5, lsl r6
	strh r7, [lr, r4]
	ldr r7, [r3]
	add r7, r7, #0xd50
	add lr, r7, #0x1000
	ldrh r7, [lr, r4]
	orr r5, r7, r5, lsl r6
	strh r5, [lr, r4]
	ldr r3, [r3]
	add r3, r3, ip, lsl #1
	add r3, r3, #0x1700
	strh r2, [r3, #0x54]
	bl MBi_CommChangeParentState
	add sp, sp, #0x34
	ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
_0221C46C:
	mov r2, #0
	mov r0, r6
	mov r1, #4
	strh r2, [r4, #0x54]
	bl MBi_CommChangeParentState
	add sp, sp, #0x34
	ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
_0221C488:
	cmp r4, #5
	bne _0221C4A8
	mov r0, r6
	mov r1, #0xe
	mov r2, #0
	bl MBi_CommChangeParentState
	add sp, sp, #0x34
	ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
_0221C4A8:
	cmp r4, #0xe
	addne sp, sp, #0x34
	ldmneia sp!, {r3, r4, r5, r6, r7, r8, pc}
	add r0, r2, r3, lsl #1
	add r0, r0, #0x1700
	ldrh r0, [r0, #0x54]
	cmp r0, #2
	addne sp, sp, #0x34
	ldmneia sp!, {r3, r4, r5, r6, r7, r8, pc}
	add r0, r2, r3
	add r0, r0, #0x1000
	ldrb r7, [r0, #0x526]
	ldr r4, _0221C640 ; =0x000005D4
	add r0, r2, #0x14c
	mul r5, r7, r4
	add r7, r0, #0x1c00
	ldrh r4, [r7, r5]
	mov r0, #1
	mov r2, #0
	orr r0, r4, r0, lsl r6
	strh r0, [r7, r5]
	ldr r4, [r1]
	mov r0, r6
	add r4, r4, r5
	add r4, r4, #0x1d00
	strh r2, [r4, #0x48]
	ldr r4, [r1]
	mov r1, #6
	add r3, r4, r3, lsl #1
	add r3, r3, #0x1700
	strh r2, [r3, #0x54]
	bl MBi_CommChangeParentState
	add sp, sp, #0x34
	ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
_0221C530:
	cmp r4, #6
	addne sp, sp, #0x34
	ldmneia sp!, {r3, r4, r5, r6, r7, r8, pc}
	add r0, r2, r3
	add r0, r0, #0x1000
	ldrb r4, [r0, #0x526]
	cmp r4, #0xff
	addeq sp, sp, #0x34
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, pc}
	ldr r0, _0221C640 ; =0x000005D4
	ldrh r1, [sp, #0x22]
	mla r0, r4, r0, r2
	add r0, r0, #0x1d00
	ldrh r0, [r0, #0x4a]
	bl MBi_calc_nextsendblock
	ldr r2, _0221C63C ; =mbc
	ldr r1, _0221C640 ; =0x000005D4
	ldr r3, [r2]
	ldr r2, _0221C644 ; =_02247480
	mla r1, r4, r1, r3
	add r1, r1, #0x1d00
	strh r0, [r1, #0x4a]
	ldr r1, [r2]
	mov r0, #1
	orr r0, r1, r0, lsl r4
	str r0, [r2]
	add sp, sp, #0x34
	ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
_0221C5A0:
	cmp r4, #6
	bne _0221C5FC
	add r0, r2, r3
	add r0, r0, #0x1000
	ldrb r3, [r0, #0x526]
	cmp r3, #0xff
	addeq sp, sp, #0x34
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, pc}
	ldr r1, _0221C640 ; =0x000005D4
	add r0, r2, #0x14c
	mul r4, r3, r1
	add r5, r0, #0x1c00
	mov r0, #1
	ldrh r1, [r5, r4]
	mvn r0, r0, lsl r6
	and r3, r1, r0
	mov r0, r6
	mov r1, #7
	mov r2, #0
	strh r3, [r5, r4]
	bl MBi_CommChangeParentState
	add sp, sp, #0x34
	ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
_0221C5FC:
	cmp r4, #7
	addne sp, sp, #0x34
	ldmneia sp!, {r3, r4, r5, r6, r7, r8, pc}
	add r0, r2, r3, lsl #1
	add r3, r0, #0x1700
	ldrh r0, [r3, #0x54]
	cmp r0, #5
	addne sp, sp, #0x34
	ldmneia sp!, {r3, r4, r5, r6, r7, r8, pc}
	mov r2, #0
	mov r0, r6
	mov r1, #8
	strh r2, [r3, #0x54]
	bl MBi_CommChangeParentState
_0221C634:
	add sp, sp, #0x34
	ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
	.balign 4, 0
_0221C63C: .word mbc
_0221C640: .word 0x000005D4
_0221C644: .word _02247480
	arm_func_end MBi_CommParentRecvDataPerChild
