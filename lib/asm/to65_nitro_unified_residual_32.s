	.include "asm/macros.inc"
	.include "nitro.inc"
	.include "global.inc"

	.public GXi_DmaId
	.public GX_ResetBankForClearImage
	.public G2_GetBG0ScrPtr
	.public G2S_GetBG0ScrPtr
	.public G2_GetBG1ScrPtr
	.public G2S_GetBG1ScrPtr
	.public G2_GetBG2ScrPtr
	.public G2S_GetBG2ScrPtr
	.public G2_GetBG3ScrPtr
	.public G2S_GetBG3ScrPtr
	.public G3_LoadMtx44
	.public OSi_GetUnusedThreadId
	.public OSi_InsertLinkToQueue
	.public OSi_ThreadInfo
	.public OSi_IrqThreadQueue
	.public OSi_IrqCallback
	.public OSi_IrqCallbackInfoIndex
	.public OSi_IrqCallbackInfo
	.public OSi_EnterDmaCallback
	.public OSi_EnterTimerCallback
	.public OS_SetIrqStackChecker
	.public OS_InitLock
	.public OSi_DoLockByWord
	.public OS_LockByWord
	.public OSi_DoUnlockByWord
	.public OS_UnlockByWord
	.public OSi_DoTryLockByWord
	.public OS_LockCartridge
	.public OS_UnlockCartridge
	.public OS_UnLockCartridge
	.public OS_TryLockCartridge
	.public OS_LockCard
	.public OS_UnlockCard
	.public OS_UnLockCard
	.public OS_ReadOwnerOfLockWord
	.public OS_InitThread
	.public OSi_ExitThread
	.public OSi_ExitThread_ArgSpecified
	.public OSi_InsertThreadToList
	.public OSi_RemoveLinkFromQueue
	.public OSi_RemoveMutexLinkFromQueue
	.public OSi_RemoveSpecifiedLinkFromQueue
	.public OSi_RemoveThreadFromList
	.public OSi_RescheduleThread
	.public OSi_ExitThread_Destroy
	.public OS_IsRunOnEmulator
	.public OS_GetConsoleType
	.public OS_JamMessage
	.public DC_InvalidateAll
	.public DC_StoreAll
	.public DC_FlushAll
	.public DC_InvalidateRange
	.public DC_StoreRange
	.public DC_FlushRange
	.public IC_InvalidateAll
	.public MI_SetWramBank
	.public MIi_CheckAnotherAutoDMA
	.public MIi_CheckDma0SourceAddress
	.public MIi_DmaSetParams
	.public MIi_CardDmaCopy32
	.public OS_InitIrqTable
	.public OSi_WaitVCount0
	.public OS_GetArenaHi
	.public OS_GetArenaLo
	.public DLAddFront
	.public DLExtract
	.public DLInsert
	.public OS_GetDTCMAddress
	.public OS_EnableProtectionUnit
	.public OS_DisableProtectionUnit
	.public OS_SetDPermissionsForProtectionRegion
	.public OSi_SetTimerReserved
	.public OSi_SetTimer
	.public OSi_InsertAlarm
	.public OS_GetCpsrIrq
	.public OSi_SendToPxi
	.public SNDi_DecibelTable
	.public SND_StopSeq
	.public SND_PrepareSeq
	.public SND_StartPreparedSeq
	.public SND_PauseSeq
	.public SND_SetPlayerVolume
	.public SND_SetPlayerChannelPriority
	.public SND_SetTrackPitch
	.public SND_SetTrackPan
	.public SND_SetTrackAllocatableChannel
	.public SND_StartTimer
	.public SND_StopTimer
	.public SND_SetupAlarm
	.public SND_LockChannel
	.public SND_UnlockChannel
	.public SND_SetChannelTimer
	.public SND_SetChannelVolume
	.public SND_SetChannelPan
	.public SND_SetupChannelPcm
	.public SND_InvalidateSeqData
	.public SND_InvalidateBankData
	.public SND_InvalidateWaveData
	.public SND_SetOutputSelector
	.public SND_SetMasterPan
	.public SND_ResetMasterPan
	.public SNDi_SetPlayerParam
	.public SNDi_SetTrackParam
	.public PushCommand_impl
	.public SND_Init
	.public SNDi_LockMutex
	.public SNDi_UnlockMutex
	.public SND_RecvCommandReply
	.public SND_AllocCommand
	.public SND_PushCommand
	.public SND_WaitForCommandProc
	.public SND_GetCurrentCommandTag
	.public SND_IsFinishedCommandTag
	.public SND_CountFreeCommand
	.public SND_CountReservedCommand
	.public SND_CountWaitingCommand
	.public PxiFifoCallback
	.public InitPXI
	.public RequestCommandProc
	.public AllocCommand
	.public IsCommandAvailable
	.public SND_AlarmInit
	.public SNDi_IncAlarmId
	.public SNDi_SetAlarmHandler
	.public SNDi_CallAlarmHandler
	.public SND_GetPlayerStatus
	.public SND_GetChannelStatus
	.public SNDi_GetFinishedCommandTag
	.public SNDi_InitSharedWork
	.public SND_CalcChannelVolume
	.public SND_AssignWaveArc
	.public SND_DestroyWaveArc
	.public SND_GetFirstInstDataPos
	.public SND_GetWaveDataCount
	.public SND_SetWaveDataAddress
	.public SND_GetWaveDataAddress
	.public PXI_Init
	.public PXI_InitFifo
	.public PXI_SetFifoRecvCallback
	.public PXI_IsCallbackReady
	.public PXI_SendWordByFifo
	.public PXIi_HandlerRecvFifoNotEmpty
	.public FSi_ReleaseCommand
	.public FSi_TranslateCommand
	.public FSi_StrNICmp
	.public FSi_ReadTable
	.public FSi_SeekDirDirect
	.public FSi_ReadFileCommand
	.public FSi_WriteFileCommand
	.public FSi_SeekDirCommand
	.public FSi_ReadDirCommand
	.public FSi_FindPathCommand
	.public FSi_GetPathCommand
	.public FSi_OpenFileFastCommand
	.public FSi_OpenFileDirectCommand
	.public FSi_CloseFileCommand
	.public FSi_GetPackedName
	.public FSi_ReadMemCallback
	.public FSi_WriteMemCallback
	.public FSi_ReadMemoryCore
	.public FSi_NextCommand
	.public FSi_ExecuteAsyncCommand
	.public FSi_ExecuteSyncCommand
	.public FS_InitArchive
	.public FS_FindArchive
	.public FS_RegisterArchiveName
	.public FS_ReleaseArchiveName
	.public FS_LoadArchive
	.public FS_UnloadArchive
	.public FS_LoadArchiveTables
	.public FS_UnloadArchiveTables
	.public FS_SuspendArchive
	.public FS_ResumeArchive
	.public FS_SetArchiveProc
	.public FS_NotifyArchiveAsyncEnd
	.public FS_Init
	.public FS_IsAvailable
	.public FS_InitFile
	.public FSi_FindPath
	.public FSi_ReadFileCore
	.public FS_ConvertPathToFileID
	.public FS_OpenFileDirect
	.public FS_OpenFileFast
	.public FS_OpenFile
	.public FS_CloseFile
	.public FS_WaitAsync
	.public FS_ReadFileAsync
	.public FS_ReadFile
	.public FS_SeekFile
	.public FS_ChangeDir
	.public FSi_OnRomReadDone
	.public FSi_ReadRomCallback
	.public FSi_WriteDummyCallback
	.public FSi_RomArchiveProc
	.public FSi_ReadDummyCallback
	.public FSi_EmptyArchiveProc
	.public FSi_InitRom
	.public FS_SetDefaultDMA
	.public FS_TryLoadTable
	.public FSi_GetOverlayBinarySize
	.public FS_ClearOverlayImage
	.public FS_GetOverlayFileID
	.public FSi_LoadOverlayInfoCore
	.public FS_LoadOverlayInfo
	.public FS_LoadOverlayImageAsync
	.public FS_LoadOverlayImage
	.public FSi_CompareDigest
	.public FS_StartOverlay
	.public FS_EndOverlay
	.public FS_UnloadOverlayImage
	.public FS_LoadOverlay
	.public FS_UnloadOverlay
	.public CP_SaveContext
	.public CPi_RestoreContext
	.public RTCi_ReadRawDateTimeAsync
	.public RTCi_ReadRawDateAsync
	.public RTCi_ReadRawTimeAsync
	.public RTCi_WriteRawStatus2Async
	.public RtcSendPxiCommand
	.public RTC_ConvertDateToDay
	.public RTCi_ConvertTimeToSecond
	.public RTC_ConvertDayToDate
	.public RTCi_ConvertSecondToTime
	.public RTC_ConvertSecondToDateTime
	.public RTC_GetDayOfWeek
	.public CARDi_SetTask
	.public CARDi_LockResource
	.public CARDi_UnlockResource
	.public CARDi_InitCommon
	.public CARD_IsEnabled
	.public CARD_CheckEnabled
	.public CARD_Enable
	.public CARDi_WaitAsync
	.public CARDi_TryWaitAsync
	.public CARD_GetResultCode
	.public CARD_LockRom
	.public CARD_UnlockRom
	.public CARD_LockBackup
	.public CARD_UnlockBackup
	.public CARDi_RequestStreamCommand
	.public CARD_GetBackupSectorSize
	.public CARD_IdentifyBackup
	.public CARD_WaitBackupAsync
	.public CARD_TryWaitBackupAsync
	.public CARD_CancelBackupAsync
	.public CARDi_ReadFromCache
	.public CARDi_SetRomOp
	.public CARDi_SetCardDma
	.public CARDi_OnReadCard
	.public CARDi_ReadRomIDCore
	.public CARDi_ReadRomSyncCore
	.public CARDi_ReadRom
	.public CARD_Init
	.public CARD_WaitRomAsync
	.public CARDi_GetRomAccessor
	.public CARDi_OnFifoRecv
	.public CARDi_TaskThread
	.public CARDi_Request
	.public CARD_InitPulledOutCallback
	.public CARDi_PulledOutCallback
	.public CARD_IsPulledOut
	.public CARDi_CheckPulledOutCore
	.public CARDi_SendtoPxi
	.public CTRDGi_InitCommon
	.public CTRDG_IsAgbCartridge
	.public CTRDG_IsOptionCartridge
	.public CTRDGi_IsAgbCartridgeAtInit
	.public CTRDG_GetAgbGameCode
	.public CTRDGi_GetAgbGameCodeAtInit
	.public CTRDG_GetAgbMakerCode
	.public CTRDGi_GetAgbMakerCodeAtInit
	.public CTRDG_IsPulledOut
	.public CTRDG_IsExisting
	.public CTRDGi_ChangeLatestAccessCycle
	.public CTRDGi_RestoreAccessCycle
	.public CTRDGi_LockByProcessor
	.public CTRDGi_UnlockByProcessor
	.public CTRDGi_SendtoPxi
	.public CTRDG_CpuCopy8
	.public CTRDG_CpuCopy16
	.public CTRDG_CpuCopy32
	.public CTRDGi_CopyCommon
	.public CTRDG_Read32
	.public CTRDGi_AccessCommon
	.public CTRDG_IsEnabled
	.public CTRDG_Enable
	.public CTRDG_CheckEnabled
	.public CTRDG_IdentifyAgbBackup
	.public CTRDGi_EraseFlashChipCoreAT
	.public CTRDGi_EraseFlashSectorCoreAT
	.public CTRDGi_EraseFlash4KBCoreAT
	.public CTRDGi_WriteFlashSectorCoreAT
	.public CTRDGi_WriteFlash4KBCoreAT
	.public CTRDGi_EraseFlashChipAT
	.public CTRDGi_EraseFlash4KBAT
	.public CTRDGi_WriteFlash4KBAT
	.public CTRDGi_EraseFlashChipAsyncAT
	.public CTRDGi_EraseFlash4KBAsyncAT
	.public CTRDGi_WriteFlash4KBAsyncAT
	.public CTRDGi_EraseFlashSectorAT
	.public CTRDGi_WriteFlashSectorAT
	.public CTRDGi_EraseFlashSectorAsyncAT
	.public CTRDGi_WriteFlashSectorAsyncAT
	.public MATH_CountPopulation
	.public MATH_CalcMD5
	.public MATH_CalcSHA1
	.public MATHi_CRC8InitTable
	.public MATHi_CRC8Update
	.public MATHi_CRC16InitTable
	.public MATHi_CRC16InitTableRev
	.public MATHi_CRC16Update
	.public MATHi_CRC16UpdateRev
	.public MATHi_CRC32InitTableRev
	.public MATHi_CRC32UpdateRev
	.public MATH_CalcCRC8
	.public MATH_CalcCRC16
	.public MATH_CalcCRC16CCITT
	.public MATH_CalcCRC32
	.public sSndInitialized
	.public sSndMutex
	.public sFreeList
	.public sWaitingCommandListQueue
	.public sSharedWork
	.public sCommandArray
	.public sCallbackTable
	.public SNDi_SharedWork
	.public SNDi_DecibelTable
	.public sVolumeTable
	.public fsi_default_command
	.public sDayOfYear
	.public cardi_common
	.public cardi_rom_header_addr
	.public rom_stat
	.public cardi_rom_base
	.public CTRDGi_Work
	.public AT29LV512_lib
	.public AT29LV512_org
	.public ctrdg_flash_remainder
	.public ctrdgi_flash_lock_id
	.public CTRDGi_WriteAgbFlashSector
	.public CTRDGi_WriteAgbFlashSectorAsync
	.public CTRDGi_EraseAgbFlashChipAsync
	.public CTRDGi_EraseAgbFlashSector
	.public CTRDGi_EraseAgbFlashSectorAsync
	.public CTRDGi_EraseAgbFlashChip
	.public ctrdgi_backup_irq
	.public CTRDGi_PollingSR
	.public ctrdgi_sram_lock_id
	.public AgbFlash
	.public ctrdgi_fl_maxtime
	.public LE26FV10N1TS_10
	.public defaultFlash512
	.public LE39FW512
	.public MX29L010
	.public defaultFlash1M
	.public MN63F805MNP
	.public CARDi_ReadCard
	.public CARDi_TryReadCardDma
	.public _020DD41C
	.public _020DD448
	.public _020DD464
	.public _020DD468
	.public _020DD470
	.public _020DD48C
	.public _020DD4DC
	.public _020DD4E0
	.public _020DD514
	.public _020DD528
	.public _020DD52C
	.public _020DD554
	.public _020DD55C
	.public _020DD560
	.public _020DD564
	.public _020DD568
	.public _020DD56C
	.public _020DD57C
	.public _020DD5AC
	.public _020DD5B4
	.public _020DD5DC
	.public _020DD5FC
	.public _020DD640
	.public _020DD654
	.public _020DD658
	.public _020DD65C
	.section .text.unified_32,4,1,6
	arm_func_start CARDi_TryReadCardDma
CARDi_TryReadCardDma: ; 0x020DD3E4
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	ldr r4, _020DD55C ; =cardi_common
	mov r6, #0
	ldr sb, [r4, #0x20]
	mov fp, r0
	mov r7, r6
	mov r8, r6
	mov r1, r6
	ands sl, sb, #0x1f
	ldr r5, [r4, #0x24]
	bne _020DD41C
	ldr r0, [r4, #0x28]
	cmp r0, #3
	movls r1, #1
_020DD41C:
	cmp r1, #0
	beq _020DD470
	bl OS_GetDTCMAddress
	ldr r1, _020DD560 ; =OS_IrqHandler
	add r3, sb, r5
	cmp r3, r1
	mov r1, #1
	mov r2, #0
	bls _020DD448
	cmp sb, #0x2000000
	movlo r2, r1
_020DD448:
	cmp r2, #0
	bne _020DD468
	cmp r0, r3
	bhs _020DD464
	add r0, r0, #0x4000
	cmp r0, sb
	bhi _020DD468
_020DD464:
	mov r1, #0
_020DD468:
	cmp r1, #0
	moveq r8, #1
_020DD470:
	cmp r8, #0
	beq _020DD48C
	ldr r1, [r4, #0x1c]
	ldr r0, _020DD564 ; =0x000001FF
	orr r1, r1, r5
	tst r1, r0
	moveq r7, #1
_020DD48C:
	ldr r0, _020DD568 ; =cardi_rom_header_addr
	cmp r7, #0
	ldr r0, [r0]
	cmpne r5, #0
	ldr r0, [r0, #0x60]
	movne r6, #1
	bic r0, r0, #0x7000000
	orr r0, r0, #0xa1000000
	cmp r6, #0
	str r0, [fp, #4]
	beq _020DD554
	bl OS_DisableInterrupts
	ldr r1, [r4, #0x118]
	mov r7, r0
	cmp r5, r1
	bhs _020DD4DC
	mov r0, sb
	mov r1, r5
	bl IC_InvalidateRange
	b _020DD4E0
_020DD4DC:
	bl IC_InvalidateAll
_020DD4E0:
	ldr r0, [r4, #0x11c]
	cmp r5, r0
	bhs _020DD528
	cmp sl, #0
	beq _020DD514
	sub sb, sb, sl
	mov r0, sb
	mov r1, #0x20
	bl DC_StoreRange
	add r0, sb, r5
	mov r1, #0x20
	bl DC_StoreRange
	add r5, r5, #0x20
_020DD514:
	mov r0, sb
	mov r1, r5
	bl DC_InvalidateRange
	bl DC_WaitWriteBufferEmpty
	b _020DD52C
_020DD528:
	bl DC_FlushAll
_020DD52C:
	ldr r1, _020DD56C ; =CARDi_OnReadCard
	mov r0, #0x80000
	bl OS_SetIrqFunction
	mov r0, #0x80000
	bl OS_ResetRequestIrqMask
	mov r0, #0x80000
	bl OS_EnableIrqMask
	mov r0, r7
	bl OS_RestoreInterrupts
	bl CARDi_SetCardDma
_020DD554:
	mov r0, r6
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_020DD55C: .word cardi_common
_020DD560: .word OS_IrqHandler
_020DD564: .word 0x000001FF
_020DD568: .word cardi_rom_header_addr
_020DD56C: .word CARDi_OnReadCard
	arm_func_end CARDi_TryReadCardDma

	arm_func_start CARDi_ReadCard
CARDi_ReadCard: ; 0x020DD570
	stmdb sp!, {r4, r5, r6, lr}
	ldr r4, _020DD654 ; =cardi_common
	mov r6, r0
_020DD57C:
	mov r0, #0x200
	ldr r1, [r4, #0x1c]
	rsb r0, r0, #0
	and r2, r1, r0
	cmp r2, r1
	bne _020DD5AC
	ldr r5, [r4, #0x20]
	tst r5, #3
	bne _020DD5AC
	ldr r0, [r4, #0x24]
	cmp r0, #0x200
	bhs _020DD5B4
_020DD5AC:
	str r2, [r6, #8]
	add r5, r6, #0x20
_020DD5B4:
	mov r0, r2, lsr #8
	orr r0, r0, #0xb7000000
	mov r1, r2, lsl #0x18
	bl CARDi_SetRomOp
	mov ip, #0
	ldr r0, [r6, #4]
	ldr r2, _020DD658 ; =0x040001A4
	mov lr, ip
	str r0, [r2]
	ldr r1, _020DD65C ; =0x04100010
_020DD5DC:
	ldr r3, [r2]
	tst r3, #0x800000
	beq _020DD5FC
	ldr r0, [r1]
	cmp lr, #0x200
	strlo r0, [r5, ip, lsl #2]
	addlo lr, lr, #4
	addlo ip, ip, #1
_020DD5FC:
	tst r3, #0x80000000
	bne _020DD5DC
	ldr r0, [r4, #0x20]
	cmp r5, r0
	bne _020DD640
	ldr r0, _020DD654 ; =cardi_common
	ldr r1, [r0, #0x1c]
	add r1, r1, #0x200
	str r1, [r0, #0x1c]
	ldr r1, [r0, #0x20]
	add r1, r1, #0x200
	str r1, [r0, #0x20]
	ldr r1, [r0, #0x24]
	subs r1, r1, #0x200
	str r1, [r0, #0x24]
	ldmeqia sp!, {r4, r5, r6, pc}
	b _020DD57C
_020DD640:
	mov r0, r6
	bl CARDi_ReadFromCache
	cmp r0, #0
	bne _020DD57C
	ldmia sp!, {r4, r5, r6, pc}
	.align 2, 0
_020DD654: .word cardi_common
_020DD658: .word 0x040001A4
_020DD65C: .word 0x04100010
	arm_func_end CARDi_ReadCard
