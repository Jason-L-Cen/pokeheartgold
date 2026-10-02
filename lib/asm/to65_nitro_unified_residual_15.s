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
	.public SND_GetNextInstData
	.public _020D6338
	.public _020D6368
	.public _020D6380
	.public _020D638C
	.public _020D63CC
	.public _020D63D4
	.public _020D6420
	.public _020D6440
	.public _020D6448
	.public _020D64A0
	.public _020D64AC
	.public _020D64C8
	.section .text.unified_15,4,1,6
	arm_func_start SND_GetNextInstData
SND_GetNextInstData: ; 0x020D6320
	stmdb sp!, {r3, r4, r5, lr}
	ldr r4, [r2]
	ldr r3, [r0, #0x38]
	cmp r4, r3
	bhs _020D64C8
	mov ip, #0
_020D6338:
	add r3, r0, r4, lsl #2
	ldr r4, [r3, #0x3c]
	and r3, r4, #0xff
	strb r4, [r1]
	cmp r3, #0x10
	mov r4, r4, lsr #8
	bgt _020D6380
	cmp r3, #0x10
	bge _020D63CC
	cmp r3, #5
	addls pc, pc, r3, lsl #2
	b _020D64AC
_020D6368: ; jump table
	b _020D64AC ; case 0
	b _020D638C ; case 1
	b _020D638C ; case 2
	b _020D638C ; case 3
	b _020D638C ; case 4
	b _020D638C ; case 5
_020D6380:
	cmp r3, #0x11
	beq _020D6440
	b _020D64AC
_020D638C:
	add r5, r0, r4
	ldrh r4, [r0, r4]
	ldrh r3, [r5, #2]
	mov r0, #1
	strh r4, [r1, #2]
	strh r3, [r1, #4]
	ldrh r4, [r5, #4]
	ldrh r3, [r5, #6]
	strh r4, [r1, #6]
	strh r3, [r1, #8]
	ldrh r3, [r5, #8]
	strh r3, [r1, #0xa]
	ldr r1, [r2]
	add r1, r1, #1
	str r1, [r2]
	ldmia sp!, {r3, r4, r5, pc}
_020D63CC:
	add r5, r0, r4
	b _020D6420
_020D63D4:
	mov r0, #0xc
	mla ip, lr, r0, r5
	ldrh r4, [ip, #2]
	ldrh r3, [ip, #4]
	mov r0, #1
	strh r4, [r1]
	strh r3, [r1, #2]
	ldrh r4, [ip, #6]
	ldrh r3, [ip, #8]
	strh r4, [r1, #4]
	strh r3, [r1, #6]
	ldrh r4, [ip, #0xa]
	ldrh r3, [ip, #0xc]
	strh r4, [r1, #8]
	strh r3, [r1, #0xa]
	ldr r1, [r2, #4]
	add r1, r1, #1
	str r1, [r2, #4]
	ldmia sp!, {r3, r4, r5, pc}
_020D6420:
	ldrb r4, [r5, #1]
	ldrb r3, [r5]
	ldr lr, [r2, #4]
	sub r3, r4, r3
	add r3, r3, #1
	cmp lr, r3
	blo _020D63D4
	b _020D64AC
_020D6440:
	add r4, r0, r4
	b _020D64A0
_020D6448:
	ldrb lr, [r4, r3]
	cmp lr, #0
	beq _020D64AC
	mov r0, #0xc
	mla lr, r3, r0, r4
	ldrh ip, [lr, #8]
	ldrh r3, [lr, #0xa]
	mov r0, #1
	strh ip, [r1]
	strh r3, [r1, #2]
	ldrh ip, [lr, #0xc]
	ldrh r3, [lr, #0xe]
	strh ip, [r1, #4]
	strh r3, [r1, #6]
	ldrh ip, [lr, #0x10]
	ldrh r3, [lr, #0x12]
	strh ip, [r1, #8]
	strh r3, [r1, #0xa]
	ldr r1, [r2, #4]
	add r1, r1, #1
	str r1, [r2, #4]
	ldmia sp!, {r3, r4, r5, pc}
_020D64A0:
	ldr r3, [r2, #4]
	cmp r3, #8
	blo _020D6448
_020D64AC:
	ldr r3, [r2]
	add r3, r3, #1
	stmia r2, {r3, ip}
	ldr r4, [r2]
	ldr r3, [r0, #0x38]
	cmp r4, r3
	blo _020D6338
_020D64C8:
	mov r0, #0
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end SND_GetNextInstData
