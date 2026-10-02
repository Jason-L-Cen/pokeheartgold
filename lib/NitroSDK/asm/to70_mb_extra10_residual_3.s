	.extern _start_AutoloadDoneCallback
	.include "asm/macros.inc"
	.include "mb.inc"
	.include "global.inc"
	.public MBi_get_blockinfo
	.extern MB_AddGameInfo
	.extern MBi_AttachCacheBuffer
	.extern MBi_InitCache
	.extern MBi_InitTaskInfo
	.extern MBi_InitTaskThread
	.extern MBi_IsStarted
	.extern MBi_IsTaskAvailable
	.extern MBi_MakeGameInfo
	.extern MBi_defaultLoadSegList
	.extern mbc
	.extern mbi_seg_header
	.extern ov13_02245988
	.extern ov13_02247484

.text
	arm_func_start MB_ReadSegment
MB_ReadSegment: ; 0x0221CECC
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x6c
	str r2, [sp, #4]
	cmp r2, #0x164
	mov r2, #0
	mov sl, r0
	mov sb, r1
	str r2, [sp, #0x10]
	blo _0221D324
	ldr r4, [sp, #4]
	mov r5, sb
	str r2, [sp, #0xc]
	mov r6, r2
	mov fp, r2
	cmp sl, #0
	add r5, r5, #0x160
	sub r4, r4, #0x160
	beq _0221CF44
	ldr r7, [sl, #0x2c]
	ldr r3, [sl, #0x24]
	mov r2, #0x160
	sub r3, r7, r3
	str r3, [sp, #8]
	bl FS_ReadFile
	cmp r0, #0x160
	ldr r7, [sb, #0x80]
	movlt r4, fp
	cmp r7, #0
	moveq r7, #0x1000000
	b _0221CFC0
_0221CF44:
	ldr r0, _0221D330 ; =0x027FFE00
	mov r1, #1
	ldr r7, [r0, #0x80]
	add r0, sp, #0x24
	cmp r7, #0
	moveq r7, #0x1000000
	str r1, [sp, #0xc]
	bl FS_InitFile
	ldr r0, _0221D334 ; =ov13_02245988
	mov r1, #3
	bl FS_FindArchive
	mov r1, r0
	mvn r0, #0
	str r0, [sp]
	add r0, sp, #0x24
	mov r2, #0
	add r3, r7, #0x88
	bl FS_OpenFileDirect
	ldr r2, [sp, #0x50]
	ldr r1, [sp, #0x48]
	ldr r0, _0221D330 ; =0x027FFE00
	sub r1, r2, r1
	str r1, [sp, #8]
	mov r1, sb
	mov r2, #0x160
	add sl, sp, #0x24
	bl MI_CpuCopy8
	ldr r0, [sb, #0x60]
	orr r0, r0, #0x6000
	orr r0, r0, #0x400000
	str r0, [sb, #0x60]
_0221CFC0:
	cmp r4, #0x88
	movlo r4, #0
	blo _0221CFF8
	ldr r1, [sp, #8]
	mov r0, sl
	add r1, r1, r7
	mov r2, #0
	bl FS_SeekFile
	mov r0, sl
	mov r1, r5
	mov r2, #0x88
	bl FS_ReadFile
	add r5, r5, #0x88
	sub r4, r4, #0x88
_0221CFF8:
	cmp r4, #0x70
	blo _0221D070
	mov r0, r5
	mov r6, r5
	bl MBi_InitCache
	mov r0, #3
	str r0, [sp]
	mov r0, r6
	mov r1, #0
	mov r2, #0x160
	mov r3, sb
	add r5, r5, #0x70
	sub r4, r4, #0x70
	bl MBi_AttachCacheBuffer
	ldr r8, [sl, #8]
	mov r7, #0
	b _0221D040
_0221D03C:
	add r7, r7, #1
_0221D040:
	cmp r7, #3
	bge _0221D054
	ldrsb r0, [r8, r7]
	cmp r0, #0
	bne _0221D03C
_0221D054:
	mov r0, r8
	mov r2, r7
	add r1, r6, #0x10
	bl MI_CpuCopy8
	str r7, [r6, #0x14]
	str r8, [r6, #0x18]
	b _0221D074
_0221D070:
	mov r4, #0
_0221D074:
	cmp r4, #0x10
	movlo r4, #0
	blo _0221D0C4
	mov r0, #0
	str r0, [r5]
	ldr r1, [sl, #0x24]
	ldr r0, [sp, #8]
	ldr r2, [sb, #0x20]
	add r0, r0, r1
	add r0, r2, r0
	str r0, [r5, #4]
	ldr r1, [sl, #0x24]
	ldr r0, [sp, #8]
	ldr r2, [sb, #0x30]
	add r0, r0, r1
	add r0, r2, r0
	mov fp, r5
	str r0, [r5, #8]
	add r5, r5, #0x10
	sub r4, r4, #0x10
_0221D0C4:
	ldr r1, [sb, #0x2c]
	ldr r0, [sb, #0x3c]
	add r0, r1, r0
	cmp r4, r0
	blo _0221D16C
	ldr r7, [sl, #0x24]
	ldr r1, [fp, #4]
	mov r0, sl
	sub r1, r1, r7
	mov r2, #0
	bl FS_SeekFile
	ldr r2, [sb, #0x2c]
	mov r0, sl
	mov r1, r5
	bl FS_ReadFile
	mov r0, #3
	str r0, [sp]
	ldr r1, [fp, #4]
	ldr r2, [sb, #0x2c]
	mov r0, r6
	mov r3, r5
	bl MBi_AttachCacheBuffer
	ldr r1, [fp, #8]
	ldr r4, [sb, #0x2c]
	mov r0, sl
	mov r2, #0
	sub r1, r1, r7
	bl FS_SeekFile
	ldr r2, [sb, #0x3c]
	mov r0, sl
	add r1, r5, r4
	bl FS_ReadFile
	mov r0, #3
	str r0, [sp]
	ldr r1, [fp, #8]
	ldr r2, [sb, #0x3c]
	add r3, r5, r4
	mov r0, r6
	bl MBi_AttachCacheBuffer
	mov r0, #1
	str r0, [sp, #0x10]
	b _0221D240
_0221D16C:
	cmp r4, #0xcc00
	blo _0221D240
	ldr r7, [sl, #0x24]
	ldr r4, [fp, #4]
	mov r0, sl
	sub r1, r4, r7
	mov r2, #0
	bl FS_SeekFile
	mov r0, sl
	mov r1, r5
	mov r2, #0x4400
	bl FS_ReadFile
	mov r0, #3
	str r0, [sp]
	mov r0, r6
	mov r1, r4
	mov r2, #0x4400
	mov r3, r5
	bl MBi_AttachCacheBuffer
	add r1, r4, #0x4400
	mov r0, sl
	sub r1, r1, r7
	mov r2, #0
	bl FS_SeekFile
	mov r0, sl
	add r1, r5, #0x4400
	mov r2, #0x4400
	bl FS_ReadFile
	mov r0, #2
	str r0, [sp]
	mov r0, r6
	add r1, r4, #0x4400
	mov r2, #0x4400
	add r3, r5, #0x4400
	bl MBi_AttachCacheBuffer
	add r1, r4, #0x8800
	mov r0, sl
	sub r1, r1, r7
	mov r2, #0
	bl FS_SeekFile
	mov r0, sl
	add r1, r5, #0x8800
	mov r2, #0x4400
	bl FS_ReadFile
	mov r0, #2
	str r0, [sp]
	add r1, r4, #0x8800
	add r3, r5, #0x8800
	mov r0, r6
	mov r2, #0x4400
	bl MBi_AttachCacheBuffer
	mov r0, #1
	str r0, [sp, #0x10]
_0221D240:
	ldr r1, [sp, #8]
	mov r0, sl
	mov r2, #0
	bl FS_SeekFile
	ldr r0, [sp, #0xc]
	cmp r0, #0
	beq _0221D30C
	add r0, sp, #0x24
	bl FS_CloseFile
	ldr r0, [sp, #0x10]
	cmp r0, #0
	beq _0221D30C
	ldr r1, [sb, #0x20]
	ldr r0, _0221D338 ; =mbi_seg_header
	str r1, [sp, #0x14]
	ldr r2, [sb, #0x28]
	ldr r1, [sb, #0x20]
	ldr r4, [r0]
	sub r0, r2, r1
	str r0, [sp, #0x18]
	ldr r2, [r6, #0x48]
	ldr r1, [sb, #0x20]
	add r0, sp, #0x14
	sub r1, r2, r1
	str r1, [sp, #0x1c]
	ldr r1, [sp, #4]
	mov r2, #0x8000
	str r1, [sp, #0x20]
	mov r1, #0x4000
	mov r3, #1
	bl MBi_ReadSegmentHeader
	ldr r0, [r4, #4]
	cmp r0, #0
	beq _0221D2F4
	add r7, sp, #0x14
	mov r5, #0
_0221D2D0:
	ldmia r4, {r1, r2}
	mov r0, r7
	mov r3, r5
	add r2, r1, r2
	bl MBi_ReadSegmentHeader
	add r4, r4, #8
	ldr r0, [r4, #4]
	cmp r0, #0
	bne _0221D2D0
_0221D2F4:
	ldr r1, [sb, #0x28]
	ldr r2, _0221D33C ; =_start_AutoloadDoneCallback
	ldr r3, [r6, #0x48]
	ldr r0, _0221D340 ; =0xE12FFF1E
	sub r1, r2, r1
	str r0, [r3, r1]
_0221D30C:
	ldr r0, [sp, #0x10]
	cmp r0, #0
	beq _0221D324
	ldr r1, [sp, #4]
	mov r0, sb
	bl DC_FlushRange
_0221D324:
	ldr r0, [sp, #0x10]
	add sp, sp, #0x6c
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.balign 4, 0
_0221D330: .word 0x027FFE00
_0221D334: .word ov13_02245988
_0221D338: .word mbi_seg_header
_0221D33C: .word _start_AutoloadDoneCallback
_0221D340: .word 0xE12FFF1E
	arm_func_end MB_ReadSegment

	arm_func_start MBi_ReadSegmentHeader
MBi_ReadSegmentHeader: ; 0x0221D344
	stmdb sp!, {r3, lr}
	mov lr, r1
	ldr ip, [r0]
	ldr r1, [r0, #0xc]
	cmp lr, #0x4000
	movlo lr, #0x4000
	cmp r2, #0x8000
	movhi r2, #0x8000
	cmp lr, ip
	add r1, ip, r1
	movlo lr, ip
	cmp r2, r1
	movhi r2, r1
	cmp lr, r2
	ldmhsia sp!, {r3, pc}
	cmp r3, #0
	beq _0221D3A0
	ldr r0, [r0, #8]
	sub r2, r2, lr
	add r0, r0, lr
	mov r1, #0
	bl MI_CpuFill8
	ldmia sp!, {r3, pc}
_0221D3A0:
	ldr r3, [r0, #4]
	ldr r1, [r0, #8]
	add r0, r3, lr
	add r1, r1, lr
	sub r2, r2, lr
	bl MI_CpuCopy8
	ldmia sp!, {r3, pc}
	arm_func_end MBi_ReadSegmentHeader

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

	arm_func_start MBi_MakeDownloadFileInfo
MBi_MakeDownloadFileInfo: ; 0x0221D64C
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, lr}
	mov r2, #0x22c0000
	mov r4, r1
	mov r5, r0
	str r2, [sp]
	ldr r0, [r4, #0x24]
	ldr r7, _0221D6BC ; =MBi_defaultLoadSegList
	str r0, [r5]
	ldr r0, [r4, #0x34]
	add r8, r5, #0xc
	str r0, [r5, #4]
	mov sb, #0
	add r6, sp, #0
_0221D680:
	mov r0, r4
	mov r1, r7
	mov r2, r8
	mov r3, r6
	bl MBi_SetSegmentInfo
	add sb, sb, #1
	cmp sb, #3
	add r8, r8, #0x10
	add r7, r7, #4
	blt _0221D680
	add r0, r4, #0x160
	add r1, r5, #0x3c
	mov r2, #0x88
	bl MI_CpuCopy8
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	.balign 4, 0
_0221D6BC: .word MBi_defaultLoadSegList
	arm_func_end MBi_MakeDownloadFileInfo

	arm_func_start MBi_SetSegmentInfo
MBi_SetSegmentInfo: ; 0x0221D6C0
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	ldr r1, [r1]
	mov r7, r0
	mov r6, r2
	mov r5, r3
	cmp r1, #0
	beq _0221D6F0
	cmp r1, #1
	beq _0221D73C
	cmp r1, #2
	beq _0221D80C
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_0221D6F0:
	ldr r0, [r7, #0x28]
	cmp r0, #0x2000000
	blo _0221D734
	cmp r0, #0x22c0000
	bhs _0221D734
	ldr r1, [r7, #0x2c]
	add r0, r0, r1
	cmp r0, #0x22c0000
	bhi _0221D734
	str r1, [r6, #8]
	ldr r0, [r7, #0x28]
	str r0, [r6, #4]
	str r0, [r6]
	ldr r0, [r6, #0xc]
	bic r0, r0, #1
	str r0, [r6, #0xc]
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_0221D734:
	bl OS_Terminate
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_0221D73C:
	ldr ip, [r7, #0x38]
	ldr r1, [r7, #0x3c]
	mov r2, #0
	mov r4, r2
	cmp ip, #0x2000000
	add r3, ip, r1
	blo _0221D788
	ldr r0, _0221D82C ; =0x023FE800
	cmp ip, r0
	bhs _0221D788
	cmp r3, #0x2300000
	bls _0221D7B4
	cmp r3, r0
	bhs _0221D780
	cmp r1, #0x40000
	movls r4, #1
	bls _0221D7B4
_0221D780:
	mov r2, #1
	b _0221D7B4
_0221D788:
	ldr r1, _0221D830 ; =0x037F8000
	cmp ip, r1
	blo _0221D7B0
	add r0, r1, #0x17000
	cmp ip, r0
	bhs _0221D7B0
	cmp r3, r0
	movls r4, #1
	movhi r2, #1
	b _0221D7B4
_0221D7B0:
	mov r2, #1
_0221D7B4:
	cmp r2, #1
	bne _0221D7C0
	bl OS_Terminate
_0221D7C0:
	ldr r0, [r7, #0x3c]
	cmp r4, #0
	str r0, [r6, #8]
	ldr r0, [r7, #0x38]
	str r0, [r6, #4]
	ldreq r0, [r6, #4]
	streq r0, [r6]
	beq _0221D7F8
	ldr r0, [r5]
	str r0, [r6]
	ldr r1, [r5]
	ldr r0, [r6, #8]
	add r0, r1, r0
	str r0, [r5]
_0221D7F8:
	ldr r0, [r6, #0xc]
	bic r0, r0, #1
	orr r0, r0, #1
	str r0, [r6, #0xc]
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_0221D80C:
	ldr r0, _0221D834 ; =0x027FFE00
	mov r1, #0x160
	stmib r6, {r0, r1}
	str r0, [r6]
	ldr r0, [r6, #0xc]
	bic r0, r0, #1
	str r0, [r6, #0xc]
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.balign 4, 0
_0221D82C: .word 0x023FE800
_0221D830: .word 0x037F8000
_0221D834: .word 0x027FFE00
	arm_func_end MBi_SetSegmentInfo

	arm_func_start MBi_MakeBlockInfoTable
MBi_MakeBlockInfoTable: ; 0x0221D838
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, lr}
	mov sl, r0
	add r6, sl, #0xc
	cmp r1, #0
	mov r0, #0
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
	mov r2, r0
_0221D854:
	str r0, [sl, r2, lsl #2]
	add r3, r1, r2, lsl #4
	add r2, r2, #1
	ldr r3, [r3, #0x14]
	and r2, r2, #0xff
	cmp r2, #3
	add r0, r0, r3
	blo _0221D854
	mov sb, #0
	ldr r4, _0221D900 ; =mbc
	strh sb, [r6]
	add r5, r1, #0xc
_0221D884:
	ldr r0, [r4]
	add r7, r5, sb, lsl #4
	add r0, r0, #0x1000
	ldr r1, [r0, #0x318]
	ldr r8, [r7, #8]
	add r0, r8, r1
	sub r0, r0, #1
	bl _u32_div_f
	mov r1, sb, lsl #1
	ldrh r3, [r6, r1]
	mov r2, r0, lsl #0x10
	ldr r1, [r7, #4]
	add r2, r3, r2, lsr #16
	mov r3, r2, lsl #0x10
	mov r0, sb
	mov r2, r8
	mov r7, r3, lsr #0x10
	bl IsAbleToLoad
	cmp r0, #0
	moveq r0, #0
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
	cmp sb, #2
	addlo r0, r6, sb, lsl #1
	strloh r7, [r0, #2]
	add r0, sb, #1
	and sb, r0, #0xff
	strhsh r7, [sl, #0x12]
	cmp sb, #3
	blo _0221D884
	mov r0, #1
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
	.balign 4, 0
_0221D900: .word mbc
	arm_func_end MBi_MakeBlockInfoTable

	arm_func_start MBi_get_blockinfo
MBi_get_blockinfo: ; 0x0221D904
	stmdb sp!, {r3, r4, r5, lr}
	ldrh r4, [r1, #0x12]
	cmp r2, r4
	movhs r0, #0
	ldmhsia sp!, {r3, r4, r5, pc}
	mov ip, #2
_0221D91C:
	add r4, r1, ip, lsl #1
	ldrh r4, [r4, #0xc]
	cmp r2, r4
	bhs _0221D93C
	sub r4, ip, #1
	mov ip, r4, lsl #0x18
	movs ip, ip, asr #0x18
	bpl _0221D91C
_0221D93C:
	cmp ip, #0
	movlt r0, #0
	ldmltia sp!, {r3, r4, r5, pc}
	ldr r4, _0221D9B4 ; =mbc
	add r5, r1, ip, lsl #1
	ldr lr, [r4]
	ldrh r5, [r5, #0xc]
	add lr, lr, #0x1000
	ldr lr, [lr, #0x318]
	sub r5, r2, r5
	mul r2, r5, lr
	add r3, r3, #0xc
	add r5, r3, ip, lsl #4
	ldr r3, [r5, #8]
	sub lr, r3, r2
	str lr, [r0, #4]
	ldr r3, [r4]
	add r3, r3, #0x1000
	ldr r3, [r3, #0x318]
	cmp lr, r3
	strhi r3, [r0, #4]
	ldr r1, [r1, ip, lsl #2]
	add r1, r2, r1
	str r1, [r0, #8]
	ldr r1, [r5]
	add r1, r2, r1
	str r1, [r0]
	strb ip, [r0, #0xc]
	mov r0, #1
	ldmia sp!, {r3, r4, r5, pc}
	.balign 4, 0
_0221D9B4: .word mbc
	arm_func_end MBi_get_blockinfo

	arm_func_start MBi_IsAbleToRecv
MBi_IsAbleToRecv: ; 0x0221D9B8
	ldr r3, _0221DA5C ; =MBi_defaultLoadSegList
	ldr r0, [r3, r0, lsl #2]
	cmp r0, #0
	beq _0221D9FC
	cmp r0, #1
	beq _0221DA18
	cmp r0, #2
	bne _0221DA4C
	ldr r0, _0221DA60 ; =0x027FFE00
	cmp r1, r0
	blo _0221DA54
	add r1, r1, r2
	add r0, r0, #0x160
	cmp r1, r0
	bhi _0221DA54
	mov r0, #1
	bx lr
_0221D9FC:
	cmp r1, #0x2000000
	blo _0221DA54
	add r0, r1, r2
	cmp r0, #0x22c0000
	bhi _0221DA54
	mov r0, #1
	bx lr
_0221DA18:
	cmp r1, #0x22c0000
	blo _0221DA30
	add r0, r1, r2
	cmp r0, #0x2300000
	movls r0, #1
	bxls lr
_0221DA30:
	cmp r1, #0x2000000
	blo _0221DA54
	add r0, r1, r2
	cmp r0, #0x2300000
	bhi _0221DA54
	mov r0, #1
	bx lr
_0221DA4C:
	mov r0, #0
	bx lr
_0221DA54:
	mov r0, #0
	bx lr
	.balign 4, 0
_0221DA5C: .word MBi_defaultLoadSegList
_0221DA60: .word 0x027FFE00
	arm_func_end MBi_IsAbleToRecv

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
