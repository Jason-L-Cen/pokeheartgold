	.extern _start_AutoloadDoneCallback
	.include "asm/macros.inc"
	.include "mb.inc"
	.include "global.inc"
	.public MBi_CommParentCallback
	.extern MB_SendGameInfoBeacon
	.extern MB_UpdateGameInfoMember
	.extern MBi_BlockHeaderEnd
	.extern MBi_ClearParentPieceBuffer
	.extern MBi_CommCallParentError
	.extern MBi_CommChangeParentState
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
	.extern mbc

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

	arm_func_start MBi_CommParentRecvData
MBi_CommParentRecvData: ; 0x0221C648
	stmdb sp!, {r4, r5, r6, lr}
	mov ip, #0
	ldr r2, _0221C6E4 ; =mbc
	ldr r1, _0221C6E8 ; =0x000005D4
	mov r4, r0
	mov r3, ip
_0221C660:
	ldr r0, [r2]
	mla r5, ip, r1, r0
	add r0, r5, #0x1000
	ldrb r0, [r0, #0xd52]
	cmp r0, #0
	addne r0, r5, #0x1d00
	strneh r3, [r0, #0x4a]
	add r0, ip, #1
	mov r0, r0, lsl #0x10
	mov ip, r0, lsr #0x10
	cmp ip, #0x10
	blo _0221C660
	ldr r0, _0221C6EC ; =_02247480
	mov r1, #0
	ldr r5, _0221C6F0 ; =0x0000FFFF
	str r1, [r0]
	mov r6, #1
_0221C6A4:
	mov r0, r4
	mov r1, r6
	bl WM_ReadMPData
	cmp r0, #0
	ldrneh r1, [r0]
	cmpne r1, r5
	cmpne r1, #0
	beq _0221C6CC
	mov r1, r6
	bl MBi_CommParentRecvDataPerChild
_0221C6CC:
	add r0, r6, #1
	mov r0, r0, lsl #0x10
	mov r6, r0, lsr #0x10
	cmp r6, #0xf
	bls _0221C6A4
	ldmia sp!, {r4, r5, r6, pc}
	.balign 4, 0
_0221C6E4: .word mbc
_0221C6E8: .word 0x000005D4
_0221C6EC: .word _02247480
_0221C6F0: .word 0x0000FFFF
	arm_func_end MBi_CommParentRecvData

	arm_func_start MBi_CommParentSendMsg
MBi_CommParentSendMsg: ; 0x0221C6F4
	stmdb sp!, {r4, lr}
	sub sp, sp, #8
	ldr r2, _0221C730 ; =mbc
	mov r4, r1
	ldr r1, [r2]
	strb r0, [sp]
	add r0, sp, #0
	bl MBi_MakeParentSendBuffer
	ldr r0, _0221C730 ; =mbc
	mov r1, r4
	ldr r2, [r0]
	mov r0, #6
	bl MBi_BlockHeaderEnd
	add sp, sp, #8
	ldmia sp!, {r4, pc}
	.balign 4, 0
_0221C730: .word mbc
	arm_func_end MBi_CommParentSendMsg

	arm_func_start MBi_CommParentSendDLFileInfo
MBi_CommParentSendDLFileInfo: ; 0x0221C734
	stmdb sp!, {r3, r4, r5, lr}
	sub sp, sp, #0x18
	mov r5, #0
	add r0, sp, #6
	mov r1, r5
	mov r2, #0x10
	mvn r4, #0
	bl MI_CpuFill8
	ldr r1, _0221C8C0 ; =mbc
	mov r0, #1
	ldr r1, [r1]
	add ip, sp, #6
_0221C764:
	add r2, r1, r0, lsl #2
	add r2, r2, #0x1000
	ldr r2, [r2, #0x4e4]
	cmp r2, #5
	bne _0221C790
	add r2, r1, r0
	add r2, r2, #0x1500
	ldrsb r3, [r2, #0x25]
	ldrb r2, [ip, r3]
	add r2, r2, #1
	strb r2, [ip, r3]
_0221C790:
	add r0, r0, #1
	mov r0, r0, lsl #0x10
	mov r0, r0, lsr #0x10
	cmp r0, #0xf
	bls _0221C764
	ldr r2, _0221C8C4 ; =_02245980
	ldr ip, _0221C8C8 ; =0x000005D4
	ldrb r3, [r2]
	mov r0, #0
	add lr, sp, #6
_0221C7B8:
	add r2, r3, #1
	mov r3, r2, lsr #0x1f
	rsb r2, r3, r2, lsl #28
	add r2, r3, r2, ror #28
	and r3, r2, #0xff
	mla r2, r3, ip, r1
	add r2, r2, #0x1000
	ldrb r2, [r2, #0xd52]
	cmp r2, #0
	ldrneb r2, [lr, r3]
	cmpne r2, #0
	movne r0, r3, lsl #0x18
	movne r4, r0, asr #0x18
	bne _0221C800
	add r0, r0, #1
	and r0, r0, #0xff
	cmp r0, #0x10
	blo _0221C7B8
_0221C800:
	mvn r0, #0
	cmp r4, r0
	addeq sp, sp, #0x18
	moveq r0, #0x15
	ldmeqia sp!, {r3, r4, r5, pc}
	ldr r2, _0221C8C4 ; =_02245980
	mov r0, #1
	strb r4, [r2]
	mov r3, r0
_0221C824:
	add r2, r1, r0, lsl #2
	add r2, r2, #0x1000
	ldr r2, [r2, #0x4e4]
	cmp r2, #5
	bne _0221C854
	add r2, r1, r0
	add r2, r2, #0x1500
	ldrsb r2, [r2, #0x25]
	cmp r4, r2
	orreq r2, r5, r3, lsl r0
	moveq r2, r2, lsl #0x10
	moveq r5, r2, lsr #0x10
_0221C854:
	add r0, r0, #1
	mov r0, r0, lsl #0x10
	mov r0, r0, lsr #0x10
	cmp r0, #0xf
	bls _0221C824
	mov r2, #3
	add r0, sp, #0
	strb r2, [sp]
	strh r4, [sp, #2]
	bl MBi_MakeParentSendBuffer
	movs r1, r0
	beq _0221C8A4
	ldr r0, _0221C8C0 ; =mbc
	ldr r3, _0221C8C8 ; =0x000005D4
	ldr r0, [r0]
	mov r2, #0xe4
	add r0, r0, #0x388
	add r0, r0, #0x1400
	mla r0, r4, r3, r0
	bl MI_CpuCopy8
_0221C8A4:
	ldr r0, _0221C8C0 ; =mbc
	mov r1, r5
	ldr r2, [r0]
	mov r0, #0xea
	bl MBi_BlockHeaderEnd
	add sp, sp, #0x18
	ldmia sp!, {r3, r4, r5, pc}
	.balign 4, 0
_0221C8C0: .word mbc
_0221C8C4: .word _02245980
_0221C8C8: .word 0x000005D4
	arm_func_end MBi_CommParentSendDLFileInfo

	arm_func_start MBi_ReloadCache
MBi_ReloadCache: ; 0x0221C8CC
	stmdb sp!, {r3, r4, r5, r6, lr}
	sub sp, sp, #0x4c
	ldr r5, [r0, #0x14]
	ldr r4, [r0, #0x10]
	ldr r1, [r5, #0x14]
	add r0, r5, #0x10
	bl FS_FindArchive
	movs r6, r0
	add r0, sp, #4
	ldreq r6, [r5, #0x18]
	bl FS_InitFile
	ldr r2, [r4]
	mvn r0, #0
	str r0, [sp]
	ldr r3, [r4, #4]
	add r0, sp, #4
	mov r1, r6
	add r3, r2, r3
	bl FS_OpenFileDirect
	cmp r0, #0
	beq _0221C948
	ldr r1, [r4, #8]
	ldr r2, [r4, #4]
	add r0, sp, #4
	bl FS_ReadFile
	ldr r1, [r4, #4]
	cmp r1, r0
	moveq r0, #2
	streq r0, [r4, #0xc]
	add r0, sp, #4
	bl FS_CloseFile
_0221C948:
	ldr r0, [r4, #0xc]
	cmp r0, #2
	addeq sp, sp, #0x4c
	ldmeqia sp!, {r3, r4, r5, r6, pc}
	mov r0, #0
	str r0, [r4]
	mov r0, #2
	str r0, [r4, #0xc]
	add sp, sp, #0x4c
	ldmia sp!, {r3, r4, r5, r6, pc}
	arm_func_end MBi_ReloadCache

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

	arm_func_start MBi_CommParentSendData
MBi_CommParentSendData: ; 0x0221CC0C
	stmdb sp!, {r4, r5, lr}
	sub sp, sp, #0xc
	add r1, sp, #0
	mov r0, #0
	mov r2, #0xa
	bl MIi_CpuClear16
	mov r3, #1
	ldr r0, _0221CD84 ; =mbc
	mov r4, r3
	ldr ip, [r0]
	mov r5, r3
	mov r0, r3
	mov r1, r3
	mov r2, r3
_0221CC44:
	add lr, ip, r3, lsl #2
	add lr, lr, #0x1000
	ldr lr, [lr, #0x4e4]
	cmp lr, #0xb
	addls pc, pc, lr, lsl #2
	b _0221CCD8
_0221CC5C: ; jump table
	b _0221CCD8 ; case 0
	b _0221CCD8 ; case 1
	b _0221CC8C ; case 2
	b _0221CCD8 ; case 3
	b _0221CCAC ; case 4
	b _0221CC9C ; case 5
	b _0221CCD8 ; case 6
	b _0221CCD8 ; case 7
	b _0221CCBC ; case 8
	b _0221CCD8 ; case 9
	b _0221CCD8 ; case 10
	b _0221CCCC ; case 11
_0221CC8C:
	ldrh lr, [sp]
	orr lr, lr, r2, lsl r3
	strh lr, [sp]
	b _0221CCD8
_0221CC9C:
	ldrh lr, [sp, #2]
	orr lr, lr, r1, lsl r3
	strh lr, [sp, #2]
	b _0221CCD8
_0221CCAC:
	ldrh lr, [sp, #4]
	orr lr, lr, r0, lsl r3
	strh lr, [sp, #4]
	b _0221CCD8
_0221CCBC:
	ldrh lr, [sp, #6]
	orr lr, lr, r5, lsl r3
	strh lr, [sp, #6]
	b _0221CCD8
_0221CCCC:
	ldrh lr, [sp, #8]
	orr lr, lr, r4, lsl r3
	strh lr, [sp, #8]
_0221CCD8:
	add r3, r3, #1
	mov r3, r3, lsl #0x10
	mov r3, r3, lsr #0x10
	cmp r3, #0xf
	bls _0221CC44
	ldrh r1, [sp, #6]
	cmp r1, #0
	beq _0221CD04
	mov r0, #5
	bl MBi_CommParentSendMsg
	b _0221CD64
_0221CD04:
	ldrh r1, [sp]
	cmp r1, #0
	beq _0221CD1C
	mov r0, #1
	bl MBi_CommParentSendMsg
	b _0221CD64
_0221CD1C:
	ldrh r1, [sp, #8]
	cmp r1, #0
	beq _0221CD34
	mov r0, #6
	bl MBi_CommParentSendMsg
	b _0221CD64
_0221CD34:
	ldrh r1, [sp, #4]
	cmp r1, #0
	beq _0221CD4C
	mov r0, #2
	bl MBi_CommParentSendMsg
	b _0221CD64
_0221CD4C:
	ldrh r0, [sp, #2]
	cmp r0, #0
	beq _0221CD60
	bl MBi_CommParentSendDLFileInfo
	b _0221CD64
_0221CD60:
	bl MBi_CommParentSendBlock
_0221CD64:
	cmp r0, #0x15
	addne sp, sp, #0xc
	ldmneia sp!, {r4, r5, pc}
	ldr r1, _0221CD88 ; =0x0000FFFF
	mov r0, #0
	bl MBi_CommParentSendMsg
	add sp, sp, #0xc
	ldmia sp!, {r4, r5, pc}
	.balign 4, 0
_0221CD84: .word mbc
_0221CD88: .word 0x0000FFFF
	arm_func_end MBi_CommParentSendData

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
