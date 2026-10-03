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
	.extern MBi_ReadSegmentHeader

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
