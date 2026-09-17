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

	.section .bss.unified_00,4,8,3
	.space 0x4

	.section .bss.unified_01,4,8,3
	.space 0x18

	.section .bss.unified_02,4,8,3
_021E36B0:
	.space 0x1C

_021E36CC:
	.space 0x20

_021E36EC:
	.space 0x4

_021E36F0:
	.space 0x1C

_021E370C:
	.space 0xC

_021E3718:
	.space 0xC

_021E3724:
	.space 0x4

_021E3728:
	.space 0x10

_021E3738:
	.space 0x18

_021E3750:
	.space 0x4

_021E3754:
	.space 0x24

_021E3778:
	.space 0xC

_021E3784:
	.space 0x24


	.section .bss.unified_03,4,8,3
_021E4088:
	.space 0x4

_021E408C:
	.space 0x10

_021E409C:
	.space 0xBC

_021E4158:
	.space 0xC8

_021E4220:
	.space 0x8

_021E4228:
	.space 0x20

_021E4248:
	.space 0x28

_021E4270:
	.space 0x14

_021E4284:
	.space 0x10

_021E4294:
	.space 0x2C

_021E42C0:
	.space 0xA00

_021E4CC0:
	.space 0x80


	.section .bss.unified_04,4,8,3
_021E4D48:
	.space 0x18
_021E4D60:
	.space 0x20

_021E4D80:
	.space 0x4

_021E4D84:
	.space 0xBC

_021E4E40:
	.space 0xE8

ctrdg_flash_remainder:
	.space 0x4

ctrdgi_flash_lock_id:
	.space 0x4

CTRDGi_WriteAgbFlashSector:
	.space 0x4

CTRDGi_WriteAgbFlashSectorAsync:
	.space 0x4

CTRDGi_EraseAgbFlashChipAsync:
	.space 0x4

CTRDGi_EraseAgbFlashSector:
	.space 0x4

CTRDGi_EraseAgbFlashSectorAsync:
	.space 0x4

CTRDGi_EraseAgbFlashChip:
	.space 0x4

ctrdgi_backup_irq:
	.space 0x14

CTRDGi_PollingSR:
	.space 0x4

ctrdgi_sram_lock_id:
	.space 0x8

_021E4F68:
	.space 0x4

_021E4F6C:
	.space 0x24

_021E4F90:
	.space 0x400


	.section .text.unified_01,4,1,6

	arm_func_start SND_SetPlayerTempoRatio
SND_SetPlayerTempoRatio: ; 0x020D50CC
	ldr ip, _020D50E0 ; =SNDi_SetPlayerParam
	mov r2, r1
	mov r1, #0x1a
	mov r3, #2
	bx ip
	.align 2, 0
_020D50E0: .word SNDi_SetPlayerParam
	arm_func_end SND_SetPlayerTempoRatio

	.section .text.unified_03,4,1,6

	arm_func_start SND_SetMasterVolume
SND_SetMasterVolume: ; 0x020D5394
	stmdb sp!, {r3, lr}
	mov r2, #0
	mov r1, r0
	mov r3, r2
	mov r0, #0x17
	str r2, [sp]
	bl PushCommand_impl
	ldmia sp!, {r3, pc}
	arm_func_end SND_SetMasterVolume

	.section .text.unified_05,4,1,6

	arm_func_start SND_ReadDriverInfo
SND_ReadDriverInfo: ; 0x020D541C
	stmdb sp!, {r3, lr}
	mov r2, #0
	mov r1, r0
	mov r3, r2
	mov r0, #0x21
	str r2, [sp]
	bl PushCommand_impl
	ldmia sp!, {r3, pc}
	arm_func_end SND_ReadDriverInfo

	.section .text.unified_07,4,1,6

	arm_func_start SND_CommandInit
SND_CommandInit: ; 0x020D5530
	stmdb sp!, {r4, lr}
	bl InitPXI
	ldr lr, _020D55EC ; =sCommandArray
	ldr r0, _020D55F0 ; =sFreeList
	mov r4, #0
	str lr, [r0]
	mov r0, #0x18
	mov r1, r0
	b _020D5568
_020D5554:
	add r3, r4, #1
	mul r2, r4, r1
	mla ip, r3, r0, lr
	mov r4, r3
	str ip, [lr, r2]
_020D5568:
	cmp r4, #0xff
	blt _020D5554
	ldr r0, _020D55F4 ; =sCommandArray + 0xD20
	mov r3, #0
	str r3, [r0, #0xac8]
	ldr r1, _020D55F8 ; =sCommandArray + 0x17E8
	ldr r2, _020D55F0 ; =sFreeList
	mov r0, #1
	str r1, [r2, #0x10]
	str r3, [r2, #8]
	str r3, [r2, #0xc]
	str r3, [r2, #0x1c]
	str r3, [r2, #0x14]
	str r3, [r2, #0x18]
	str r0, [r2, #0x20]
	ldr r0, _020D55FC ; =sSharedWork
	ldr r1, _020D5600 ; =SNDi_SharedWork
	str r3, [r2, #4]
	str r0, [r1]
	bl SNDi_InitSharedWork
	mov r0, #1
	bl SND_AllocCommand
	cmp r0, #0
	ldmeqia sp!, {r4, pc}
	mov r2, #0x1d
	ldr r1, _020D5600 ; =SNDi_SharedWork
	str r2, [r0, #4]
	ldr r1, [r1]
	str r1, [r0, #8]
	bl SND_PushCommand
	mov r0, #1
	bl SND_FlushCommand
	ldmia sp!, {r4, pc}
	.align 2, 0
_020D55EC: .word sCommandArray
_020D55F0: .word sFreeList
_020D55F4: .word sCommandArray + 0xD20
_020D55F8: .word sCommandArray + 0x17E8
_020D55FC: .word sSharedWork
_020D5600: .word SNDi_SharedWork
	arm_func_end SND_CommandInit

	.section .text.unified_09,4,1,6

	arm_func_start SND_FlushCommand
SND_FlushCommand: ; 0x020D57D4
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	mov sl, r0
	bl OS_DisableInterrupts
	ldr r4, _020D5984 ; =sFreeList
	mov sb, r0
	ldr r1, [r4, #8]
	cmp r1, #0
	bne _020D5800
	bl OS_RestoreInterrupts
	mov r0, #1
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020D5800:
	ldr r1, [r4, #0x1c]
	cmp r1, #8
	blt _020D5858
	tst sl, #1
	bne _020D5820
	bl OS_RestoreInterrupts
	mov r0, #0
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020D5820:
	mov r5, #1
_020D5824:
	mov r0, r5
	bl SND_RecvCommandReply
	ldr r0, [r4, #0x1c]
	cmp r0, #8
	bge _020D5824
	ldr r0, _020D5984 ; =sFreeList
	ldr r0, [r0, #8]
	cmp r0, #0
	bne _020D5858
	mov r0, sb
	bl OS_RestoreInterrupts
	mov r0, #1
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020D5858:
	ldr r0, _020D5988 ; =sCommandArray
	mov r1, #0x1800
	bl DC_FlushRange
	ldr r1, _020D5984 ; =sFreeList
	mov r0, #7
	ldr r1, [r1, #8]
	mov r2, #0
	bl PXI_SendWordByFifo
	cmp r0, #0
	bge _020D5918
	tst sl, #1
	bne _020D5898
	mov r0, sb
	bl OS_RestoreInterrupts
	mov r0, #0
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020D5898:
	mov fp, #0
	mov r5, #7
	mov r8, fp
	ldr r7, _020D5988 ; =sCommandArray
	mov r6, #0x1800
	ldr r4, _020D5984 ; =sFreeList
	b _020D58F4
_020D58B4:
	mov r0, sb
	bl OS_RestoreInterrupts
	mov r0, r8
	bl SND_RecvCommandReply
	bl OS_DisableInterrupts
	mov sb, r0
	mov r0, r7
	mov r1, r6
	bl DC_FlushRange
	ldr r0, [r4, #8]
	cmp r0, #0
	bne _020D58F4
	mov r0, sb
	bl OS_RestoreInterrupts
	mov r0, #1
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020D58F4:
	ldr r0, [r4, #0x1c]
	cmp r0, #8
	bge _020D58B4
	ldr r1, [r4, #8]
	mov r0, r5
	mov r2, fp
	bl PXI_SendWordByFifo
	cmp r0, #0
	blt _020D58B4
_020D5918:
	ldr r0, _020D5984 ; =sFreeList
	ldr r2, _020D598C ; =sWaitingCommandListQueue
	ldr r3, [r0, #0x18]
	ldr r4, [r0, #8]
	add r1, r3, #1
	str r4, [r2, r3, lsl #2]
	str r1, [r0, #0x18]
	cmp r1, #8
	movgt r1, #0
	strgt r1, [r0, #0x18]
	ldr r1, _020D5984 ; =sFreeList
	mov r0, #0
	str r0, [r1, #8]
	str r0, [r1, #0xc]
	ldr r2, [r1, #0x1c]
	mov r0, sb
	add r2, r2, #1
	str r2, [r1, #0x1c]
	ldr r2, [r1, #0x20]
	add r2, r2, #1
	str r2, [r1, #0x20]
	bl OS_RestoreInterrupts
	tst sl, #2
	beq _020D597C
	bl RequestCommandProc
_020D597C:
	mov r0, #1
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_020D5984: .word sFreeList
_020D5988: .word sCommandArray
_020D598C: .word sWaitingCommandListQueue
	arm_func_end SND_FlushCommand

	.section .text.unified_11,4,1,6

	arm_func_start SND_GetPlayerLocalVariable
SND_GetPlayerLocalVariable: ; 0x020D5D5C
	stmdb sp!, {r3, r4, r5, lr}
	mov r2, #0x24
	mul r4, r0, r2
	ldr r0, _020D5DA0 ; =SNDi_SharedWork
	mov r5, r1
	ldr r0, [r0]
	mov r1, #2
	add r0, r0, #0x20
	add r0, r0, r4
	add r0, r0, r5, lsl #1
	bl DC_InvalidateRange
	ldr r0, _020D5DA0 ; =SNDi_SharedWork
	ldr r0, [r0]
	add r0, r0, r4
	add r0, r0, r5, lsl #1
	ldrsh r0, [r0, #0x20]
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_020D5DA0: .word SNDi_SharedWork
	arm_func_end SND_GetPlayerLocalVariable

	arm_func_start SND_ReadChannelInfo
SND_ReadChannelInfo: ; 0x020D5DA4
	stmdb sp!, {r3, lr}
	cmp r1, #0
	blt _020D5DB8
	cmp r1, #0xf
	ble _020D5DC0
_020D5DB8:
	mov r0, #0
	ldmia sp!, {r3, pc}
_020D5DC0:
	add r3, r0, #0x1000
	ldr r3, [r3, #0x1c4]
	mov lr, #1
	ldr ip, [r2]
	tst r3, lr, lsl r1
	moveq lr, #0
	mov r3, lr, lsl #0x1f
	bic ip, ip, #2
	orr ip, ip, r3, lsr #30
	mov r3, ip, lsl #0x1e
	str ip, [r2]
	movs r3, r3, asr #0x1f
	beq _020D5E94
	add r0, r0, r1, lsl #2
	add r0, r0, #0x1000
	ldr r0, [r0, #0x180]
	ldr r3, [r2]
	tst r0, #0x80000000
	movne lr, #1
	moveq lr, #0
	bic ip, r3, #1
	and r3, lr, #1
	orr r3, ip, r3
	str r3, [r2]
	and r1, r0, #0x7f
	strh r1, [r2, #4]
	ldrh r1, [r2, #4]
	and r3, r0, #0x300
	mov r3, r3, lsr #8
	mov r1, r1, lsl #4
	strh r1, [r2, #4]
	cmp r3, #3
	addls pc, pc, r3, lsl #2
	b _020D5E84
_020D5E48: ; jump table
	b _020D5E84 ; case 0
	b _020D5E58 ; case 1
	b _020D5E68 ; case 2
	b _020D5E78 ; case 3
_020D5E58:
	ldrh r1, [r2, #4]
	mov r1, r1, asr #1
	strh r1, [r2, #4]
	b _020D5E84
_020D5E68:
	ldrh r1, [r2, #4]
	mov r1, r1, asr #2
	strh r1, [r2, #4]
	b _020D5E84
_020D5E78:
	ldrh r1, [r2, #4]
	mov r1, r1, asr #4
	strh r1, [r2, #4]
_020D5E84:
	and r0, r0, #0x7f0000
	mov r0, r0, lsr #0x10
	strb r0, [r2, #6]
	b _020D5F2C
_020D5E94:
	mov r3, #0x54
	mla r0, r1, r3, r0
	ldrb r1, [r0, #3]
	bic r3, ip, #1
	mov r1, r1, lsl #0x1f
	mov r1, r1, lsr #0x1f
	and r1, r1, #1
	orr r1, r3, r1
	str r1, [r2]
	ldrb r1, [r0, #2]
	str r1, [r2, #8]
	ldrh r3, [r0, #0x24]
	and r1, r3, #0xff
	strh r1, [r2, #4]
	ldrh r1, [r2, #4]
	mov r3, r3, asr #8
	cmp r3, #3
	mov r1, r1, lsl #4
	strh r1, [r2, #4]
	addls pc, pc, r3, lsl #2
	b _020D5F24
_020D5EE8: ; jump table
	b _020D5F24 ; case 0
	b _020D5EF8 ; case 1
	b _020D5F08 ; case 2
	b _020D5F18 ; case 3
_020D5EF8:
	ldrh r1, [r2, #4]
	mov r1, r1, asr #1
	strh r1, [r2, #4]
	b _020D5F24
_020D5F08:
	ldrh r1, [r2, #4]
	mov r1, r1, asr #2
	strh r1, [r2, #4]
	b _020D5F24
_020D5F18:
	ldrh r1, [r2, #4]
	mov r1, r1, asr #4
	strh r1, [r2, #4]
_020D5F24:
	ldrb r0, [r0, #0x23]
	strb r0, [r2, #6]
_020D5F2C:
	mov r0, #1
	ldmia sp!, {r3, pc}
	arm_func_end SND_ReadChannelInfo

	arm_func_start SND_ReadTrackInfo
SND_ReadTrackInfo: ; 0x020D5F34
	stmdb sp!, {r4, lr}
	cmp r1, #0
	blt _020D5F48
	cmp r1, #0xf
	ble _020D5F50
_020D5F48:
	mov r0, #0
	ldmia sp!, {r4, pc}
_020D5F50:
	cmp r2, #0
	blt _020D5F60
	cmp r2, #0xf
	ble _020D5F68
_020D5F60:
	mov r0, #0
	ldmia sp!, {r4, pc}
_020D5F68:
	mov ip, #0x24
	mla ip, r1, ip, r0
	add r1, ip, r2
	ldrb r2, [r1, #0x548]
	cmp r2, #0xff
	moveq r0, #0
	ldmeqia sp!, {r4, pc}
	add r1, r0, #0x780
	add ip, r1, r2, lsl #6
	ldrh r2, [ip, #2]
	add r1, r0, #0x1000
	strh r2, [r3]
	ldrb r2, [ip, #4]
	strb r2, [r3, #2]
	ldrb r2, [ip, #5]
	strb r2, [r3, #3]
	ldrsb r2, [ip, #6]
	strb r2, [r3, #4]
	ldrb r2, [ip, #7]
	strb r2, [r3, #5]
	ldrsb r2, [ip, #8]
	add r2, r2, #0x40
	strb r2, [r3, #6]
	ldrsb r2, [ip, #0x13]
	strb r2, [r3, #7]
	ldr r2, [ip, #0x3c]
	ldr r1, [r1, #0x1c0]
	cmp r2, #0
	moveq r4, #0
	subne r1, r2, r1
	addne r4, r0, r1
	mov r1, #0
	strb r1, [r3, #9]
	cmp r4, #0
	beq _020D6038
	add r1, r0, #0x1000
	mov ip, #0
_020D5FFC:
	ldrb r2, [r3, #9]
	ldrb lr, [r4]
	add r2, r3, r2
	strb lr, [r2, #0xa]
	ldrb r2, [r3, #9]
	add r2, r2, #1
	strb r2, [r3, #9]
	ldr r2, [r4, #0x50]
	ldr lr, [r1, #0x1c0]
	cmp r2, #0
	moveq r4, ip
	subne r2, r2, lr
	addne r4, r0, r2
	cmp r4, #0
	bne _020D5FFC
_020D6038:
	mov r0, #1
	ldmia sp!, {r4, pc}
	arm_func_end SND_ReadTrackInfo

	.section .text.unified_13,4,1,6

	arm_func_start SND_DestroyBank
SND_DestroyBank: ; 0x020D6220
	stmdb sp!, {r4, r5, r6, r7, r8, lr}
	mov r7, r0
	bl SNDi_LockMutex
	add r5, r7, #0x18
	mov r6, #0
	mov r8, #8
	mov r4, #0x3c
_020D623C:
	add r1, r7, r6, lsl #3
	ldr r0, [r1, #0x18]
	cmp r0, #0
	beq _020D62A0
	ldr r3, [r0, #0x18]
	cmp r5, r3
	bne _020D626C
	ldr r2, [r1, #0x1c]
	mov r1, r4
	str r2, [r0, #0x18]
	bl DC_StoreRange
	b _020D62A0
_020D626C:
	cmp r3, #0
	beq _020D6288
_020D6274:
	ldr r0, [r3, #4]
	cmp r5, r0
	movne r3, r0
	cmpne r0, #0
	bne _020D6274
_020D6288:
	add r0, r7, r6, lsl #3
	ldr r2, [r0, #0x1c]
	mov r0, r3
	mov r1, r8
	str r2, [r3, #4]
	bl DC_StoreRange
_020D62A0:
	add r6, r6, #1
	cmp r6, #4
	add r5, r5, #8
	blt _020D623C
	bl SNDi_UnlockMutex
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	arm_func_end SND_DestroyBank

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

	.section .text.unified_19,4,1,6

	arm_func_start FSi_SendCommand
FSi_SendCommand: ; 0x020D7788
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	mov r6, r0
	ldr r4, [r6, #8]
	mov r2, #1
	str r1, [r6, #0x10]
	mov r0, #2
	str r0, [r6, #0x14]
	ldr r0, [r6, #0xc]
	mov r7, r2, lsl r1
	orr r0, r0, #1
	str r0, [r6, #0xc]
	bl OS_DisableInterrupts
	ldr r1, [r4, #0x1c]
	mov r5, r0
	tst r1, #0x80
	beq _020D77E4
	mov r0, r6
	mov r1, #3
	bl FSi_ReleaseCommand
	mov r0, r5
	bl OS_RestoreInterrupts
	mov r0, #0
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_020D77E4:
	tst r7, #0x1fc
	ldrne r0, [r6, #0xc]
	orrne r0, r0, #4
	strne r0, [r6, #0xc]
	ldr r1, [r6]
	ldr r2, [r6, #4]
	cmp r1, #0
	strne r2, [r1, #4]
	cmp r2, #0
	add r0, r4, #0x20
	strne r1, [r2]
	ldr r1, [r0, #4]
	cmp r1, #0
	beq _020D782C
_020D781C:
	mov r0, r1
	ldr r1, [r1, #4]
	cmp r1, #0
	bne _020D781C
_020D782C:
	str r6, [r0, #4]
	str r0, [r6]
	mov r1, #0
	str r1, [r6, #4]
	ldr r0, [r4, #0x1c]
	tst r0, #8
	movne r1, #1
	cmp r1, #0
	bne _020D78DC
	ldr r0, [r4, #0x1c]
	tst r0, #0x10
	movne r0, #1
	moveq r0, #0
	cmp r0, #0
	bne _020D78DC
	ldr r1, [r4, #0x1c]
	mov r0, r5
	orr r1, r1, #0x10
	str r1, [r4, #0x1c]
	bl OS_RestoreInterrupts
	ldr r0, [r4, #0x58]
	tst r0, #0x200
	beq _020D7898
	ldr r2, [r4, #0x54]
	mov r0, r6
	mov r1, #9
	blx r2
_020D7898:
	bl OS_DisableInterrupts
	ldr r1, [r6, #0xc]
	orr r1, r1, #0x40
	str r1, [r6, #0xc]
	ldr r1, [r6, #0xc]
	tst r1, #4
	movne r1, #1
	moveq r1, #0
	cmp r1, #0
	bne _020D78D4
	bl OS_RestoreInterrupts
	mov r0, r6
	bl FSi_ExecuteAsyncCommand
	mov r0, #1
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_020D78D4:
	bl OS_RestoreInterrupts
	b _020D7920
_020D78DC:
	ldr r0, [r6, #0xc]
	tst r0, #4
	movne r0, #1
	moveq r0, #0
	cmp r0, #0
	bne _020D7904
	mov r0, r5
	bl OS_RestoreInterrupts
	mov r0, #1
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_020D7904:
	add r0, r6, #0x18
	bl OS_SleepThread
	ldr r0, [r6, #0xc]
	tst r0, #0x40
	beq _020D7904
	mov r0, r5
	bl OS_RestoreInterrupts
_020D7920:
	mov r0, r6
	bl FSi_ExecuteSyncCommand
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	arm_func_end FSi_SendCommand

	.section .text.unified_21,4,1,6

	arm_func_start DGT_Hash1Reset
DGT_Hash1Reset: ; 0x020D8E00
	ldr r2, _020D8E30 ; =0x67452301
	ldr r1, _020D8E34 ; =0xEFCDAB89
	str r2, [r0]
	ldr r2, _020D8E38 ; =0x98BADCFE
	str r1, [r0, #4]
	ldr r1, _020D8E3C ; =0x10325476
	str r2, [r0, #8]
	str r1, [r0, #0xc]
	mov r1, #0
	str r1, [r0, #0x10]
	str r1, [r0, #0x14]
	bx lr
	.align 2, 0
_020D8E30: .word 0x67452301
_020D8E34: .word 0xEFCDAB89
_020D8E38: .word 0x98BADCFE
_020D8E3C: .word 0x10325476
	arm_func_end DGT_Hash1Reset

	arm_func_start DGT_Hash1SetSource
DGT_Hash1SetSource: ; 0x020D8E40
	stmdb sp!, {r4, r5, r6, r7, r8, lr}
	mov r6, r0
	ldr r3, [r6, #0x10]
	mov r4, r2
	and ip, r3, #0x3f
	ldr r0, [r6, #0x14]
	adds r3, r3, r4
	str r3, [r6, #0x10]
	adc r0, r0, #0
	rsb r7, ip, #0x40
	mov r5, r1
	str r0, [r6, #0x14]
	cmp r7, r4
	bls _020D8E94
	cmp r4, #0
	ldmeqia sp!, {r4, r5, r6, r7, r8, pc}
	add r1, r6, #0x18
	mov r0, r5
	add r1, r1, ip
	bl MI_CpuCopy8
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_020D8E94:
	add r1, r6, #0x18
	mov r0, r5
	mov r2, r7
	add r1, r1, ip
	bl MI_CpuCopy8
	mov r0, r6
	bl ProcessBlock
	sub r4, r4, r7
	mov r8, r4, lsr #6
	cmp r8, #0
	add r7, r5, r7
	ble _020D8EF0
	mov r5, #0x40
_020D8EC8:
	mov r0, r7
	mov r2, r5
	add r1, r6, #0x18
	bl MI_CpuCopy8
	mov r0, r6
	add r7, r7, #0x40
	bl ProcessBlock
	sub r8, r8, #1
	cmp r8, #0
	bgt _020D8EC8
_020D8EF0:
	ands r2, r4, #0x3f
	ldmeqia sp!, {r4, r5, r6, r7, r8, pc}
	mov r0, r7
	add r1, r6, #0x18
	bl MI_CpuCopy8
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	arm_func_end DGT_Hash1SetSource

	arm_func_start DGT_Hash1GetDigest_R
DGT_Hash1GetDigest_R: ; 0x020D8F08
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	mov r4, r1
	ldr r1, [r4, #0x14]
	ldr r3, [r4, #0x10]
	mov r5, r0
	mov r6, r1, lsl #3
	ldr r1, _020D8FBC ; =_02110CA8
	mov r0, r4
	mov r2, #1
	orr r6, r6, r3, lsr #29
	mov r7, r3, lsl #3
	bl DGT_Hash1SetSource
	ldr r0, [r4, #0x10]
	mov r1, #0
	and r3, r0, #0x3f
	rsb r2, r3, #0x40
	cmp r2, #8
	bhs _020D8F6C
	add r0, r4, #0x18
	add r0, r0, r3
	bl MI_CpuFill8
	mov r0, r4
	bl ProcessBlock
	mov r3, #0
	mov r2, #0x40
_020D8F6C:
	cmp r2, #8
	bls _020D8F88
	add r0, r4, #0x18
	add r0, r0, r3
	sub r2, r2, #8
	mov r1, #0
	bl MI_CpuFill8
_020D8F88:
	str r7, [r4, #0x50]
	mov r0, r4
	str r6, [r4, #0x54]
	bl ProcessBlock
	mov r0, r4
	mov r1, r5
	mov r2, #0x10
	bl MI_CpuCopy8
	mov r0, r4
	mov r1, #0
	mov r2, #0x58
	bl MI_CpuFill8
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.align 2, 0
_020D8FBC: .word _02110CA8
	arm_func_end DGT_Hash1GetDigest_R

	arm_func_start ProcessBlock
ProcessBlock: ; 0x020D8FC0
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	ldmia r0, {r2, r3, ip, lr}
	add r4, r0, #0x18
	ldr r5, _020D936C ; =_02110D6C
	mov r7, r4
	mov r8, #0
_020D8FD8:
	mvn r1, r3
	and r6, r3, ip
	and r1, r1, lr
	orr r1, r6, r1
	ldr r6, [r7]
	add r1, r2, r1
	ldr r2, [r5]
	add r1, r6, r1
	add r2, r2, r1
	mov r1, r2, lsr #0x19
	orr r1, r1, r2, lsl #7
	add r2, r3, r1
	mvn r1, r2
	and r6, r2, r3
	and r1, r1, ip
	orr r1, r6, r1
	ldr r6, [r7, #4]
	add r1, lr, r1
	ldr sb, [r5, #4]
	add r1, r6, r1
	add r6, sb, r1
	mov r1, r6, lsr #0x14
	orr r1, r1, r6, lsl #12
	add lr, r2, r1
	mvn r1, lr
	and r6, lr, r2
	and r1, r1, r3
	orr r1, r6, r1
	ldr sb, [r7, #8]
	add r6, ip, r1
	ldr r1, [r7, #0xc]
	add sb, sb, r6
	ldr sl, [r5, #8]
	ldr r6, [r5, #0xc]
	add sl, sl, sb
	mov sb, sl, lsr #0xf
	orr sb, sb, sl, lsl #17
	add ip, lr, sb
	add r5, r5, #0x10
	add r7, r7, #0x10
	and sl, ip, lr
	mvn sb, ip
	and sb, sb, r2
	orr sb, sl, sb
	add r3, r3, sb
	add r1, r1, r3
	add r3, r6, r1
	mov r1, r3, lsr #0xa
	orr r1, r1, r3, lsl #22
	add r3, ip, r1
	add r8, r8, #1
	cmp r8, #4
	blt _020D8FD8
	ldr r1, _020D9370 ; =_02110CAC
	mov r6, #0
_020D90B4:
	mvn r7, lr
	ldr sb, [r1]
	and r8, r3, lr
	and r7, ip, r7
	orr r7, r8, r7
	ldr r8, [r4, sb, lsl #2]
	add r2, r2, r7
	ldr r7, [r5]
	add r2, r8, r2
	add r7, r7, r2
	mov r2, r7, lsr #0x1b
	orr r2, r2, r7, lsl #5
	add r2, r3, r2
	mvn r8, ip
	ldr r7, [r1, #4]
	and sb, r2, ip
	and r8, r3, r8
	orr sl, sb, r8
	mvn r8, r3
	ldr sb, [r1, #8]
	ldr fp, [r4, r7, lsl #2]
	add r7, lr, sl
	add r7, fp, r7
	ldr sl, [r5, #4]
	ldr sb, [r4, sb, lsl #2]
	add sl, sl, r7
	ldr r7, [r1, #0xc]
	and r8, r2, r8
	ldr r7, [r4, r7, lsl #2]
	mov fp, sl, lsr #0x17
	orr sl, fp, sl, lsl #9
	add lr, r2, sl
	ldr sl, [r5, #8]
	and fp, lr, r3
	orr r8, fp, r8
	add r8, ip, r8
	add r8, sb, r8
	add sb, sl, r8
	mov r8, sb, lsr #0x12
	orr r8, r8, sb, lsl #14
	add ip, lr, r8
	mvn r8, r2
	and sb, lr, r8
	ldr r8, [r5, #0xc]
	add r5, r5, #0x10
	add r1, r1, #0x10
	and sl, ip, r2
	orr sb, sl, sb
	add r3, r3, sb
	add r3, r7, r3
	add r7, r8, r3
	mov r3, r7, lsr #0xc
	orr r3, r3, r7, lsl #20
	add r3, ip, r3
	add r6, r6, #1
	cmp r6, #4
	blt _020D90B4
	mov r6, #0
_020D919C:
	ldr r8, [r1]
	eor r7, r3, ip
	eor r7, lr, r7
	ldr r8, [r4, r8, lsl #2]
	add r2, r2, r7
	ldr r7, [r5]
	add r2, r8, r2
	add r7, r7, r2
	mov r2, r7, lsr #0x1c
	orr r2, r2, r7, lsl #4
	add r2, r3, r2
	ldr r8, [r1, #4]
	eor r7, r2, r3
	eor r7, ip, r7
	ldr r8, [r4, r8, lsl #2]
	add r7, lr, r7
	ldr sb, [r1, #8]
	ldr sl, [r5, #4]
	add r7, r8, r7
	add r8, sl, r7
	mov r7, r8, lsr #0x15
	orr r7, r7, r8, lsl #11
	add lr, r2, r7
	eor r8, lr, r2
	ldr r7, [r1, #0xc]
	ldr sb, [r4, sb, lsl #2]
	eor r8, r3, r8
	add r8, ip, r8
	add sb, sb, r8
	ldr sl, [r5, #8]
	ldr r8, [r4, r7, lsl #2]
	add sb, sl, sb
	mov r7, sb, lsr #0x10
	orr r7, r7, sb, lsl #16
	add ip, lr, r7
	eor r7, ip, lr
	eor r7, r2, r7
	add r3, r3, r7
	add r3, r8, r3
	ldr r7, [r5, #0xc]
	add r5, r5, #0x10
	add r7, r7, r3
	add r1, r1, #0x10
	mov r3, r7, lsr #9
	orr r3, r3, r7, lsl #23
	add r3, ip, r3
	add r6, r6, #1
	cmp r6, #4
	blt _020D919C
	mov r8, #0
_020D9264:
	mvn r6, lr
	ldr r7, [r1]
	orr r6, r3, r6
	eor r6, ip, r6
	ldr r7, [r4, r7, lsl #2]
	add r2, r2, r6
	ldr r6, [r5]
	add r2, r7, r2
	add r6, r6, r2
	mov r2, r6, lsr #0x1a
	orr r2, r2, r6, lsl #6
	add r2, r3, r2
	mvn r6, ip
	ldr sb, [r1, #4]
	orr r6, r2, r6
	eor r7, r3, r6
	ldr r6, [r1, #8]
	ldr sb, [r4, sb, lsl #2]
	add r7, lr, r7
	ldr sl, [r5, #4]
	add r7, sb, r7
	add sb, sl, r7
	mov r7, sb, lsr #0x16
	orr sb, r7, sb, lsl #10
	ldr r7, [r1, #0xc]
	add lr, r2, sb
	mvn sb, r3
	ldr r6, [r4, r6, lsl #2]
	orr sb, lr, sb
	eor sb, r2, sb
	add sb, ip, sb
	add sb, r6, sb
	ldr sl, [r5, #8]
	ldr r6, [r4, r7, lsl #2]
	add sb, sl, sb
	mov r7, sb, lsr #0x11
	orr sb, r7, sb, lsl #15
	ldr r7, [r5, #0xc]
	add ip, lr, sb
	add r5, r5, #0x10
	add r1, r1, #0x10
	mvn sb, r2
	orr sb, ip, sb
	eor sb, lr, sb
	add r3, r3, sb
	add r3, r6, r3
	add r6, r7, r3
	mov r3, r6, lsr #0xb
	orr r3, r3, r6, lsl #21
	add r3, ip, r3
	add r8, r8, #1
	cmp r8, #4
	blt _020D9264
	ldr r1, [r0]
	add r1, r1, r2
	str r1, [r0]
	ldr r1, [r0, #4]
	add r1, r1, r3
	str r1, [r0, #4]
	ldr r1, [r0, #8]
	add r1, r1, ip
	str r1, [r0, #8]
	ldr r1, [r0, #0xc]
	add r1, r1, lr
	str r1, [r0, #0xc]
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_020D936C: .word _02110D6C
_020D9370: .word _02110CAC
	arm_func_end ProcessBlock

	arm_func_start DGT_Hash2Reset
DGT_Hash2Reset: ; 0x020D9374
	ldr r1, _020D93B0 ; =0x67452301
	ldr r2, _020D93B4 ; =0xEFCDAB89
	str r1, [r0]
	ldr r1, _020D93B8 ; =0x98BADCFE
	str r2, [r0, #4]
	ldr r2, _020D93BC ; =0x10325476
	str r1, [r0, #8]
	ldr r1, _020D93C0 ; =0xC3D2E1F0
	str r2, [r0, #0xc]
	str r1, [r0, #0x10]
	mov r1, #0
	str r1, [r0, #0x14]
	str r1, [r0, #0x18]
	str r1, [r0, #0x1c]
	bx lr
	.align 2, 0
_020D93B0: .word 0x67452301
_020D93B4: .word 0xEFCDAB89
_020D93B8: .word 0x98BADCFE
_020D93BC: .word 0x10325476
_020D93C0: .word 0xC3D2E1F0
	arm_func_end DGT_Hash2Reset

	arm_func_start DGT_Hash2SetSource
DGT_Hash2SetSource: ; 0x020D93C4
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	mov sl, r0
	movs r8, r2
	mov sb, r1
	add r6, sl, #0x20
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldr r0, [sl, #0x14]
	add r1, r0, r8, lsl #3
	cmp r1, r0
	ldrlo r0, [sl, #0x18]
	addlo r0, r0, #1
	strlo r0, [sl, #0x18]
	ldr r0, [sl, #0x18]
	add r0, r0, r8, lsr #29
	str r0, [sl, #0x18]
	str r1, [sl, #0x14]
	ldr r1, [sl, #0x1c]
	cmp r1, #0
	beq _020D947C
	add r0, r1, r8
	cmp r0, #0x40
	blo _020D945C
	rsb r4, r1, #0x40
	mov r0, sb
	mov r2, r4
	add r1, r6, r1
	bl MI_CpuCopy8
	ldr r1, _020D9514 ; =_02110E70
	mov r0, sl
	ldr r3, [r1]
	mov r1, r6
	mov r2, #0x40
	sub r8, r8, r4
	add sb, sb, r4
	blx r3
	mov r0, #0
	str r0, [sl, #0x1c]
	b _020D947C
_020D945C:
	mov r0, sb
	mov r2, r8
	add r1, r6, r1
	bl MI_CpuCopy8
	ldr r0, [sl, #0x1c]
	add r0, r0, r8
	str r0, [sl, #0x1c]
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020D947C:
	cmp r8, #0x40
	blo _020D94F4
	bic r7, r8, #0x3f
	tst sb, #3
	sub r8, r8, r7
	bne _020D94B4
	ldr r1, _020D9514 ; =_02110E70
	mov r0, sl
	ldr r3, [r1]
	mov r1, sb
	mov r2, r7
	blx r3
	add sb, sb, r7
	b _020D94F4
_020D94B4:
	mov r5, #0x40
	ldr r4, _020D9514 ; =_02110E70
	mov fp, r5
_020D94C0:
	mov r0, sb
	mov r1, r6
	mov r2, r5
	bl MI_CpuCopy8
	ldr r3, [r4]
	mov r0, sl
	mov r1, r6
	add sb, sb, #0x40
	mov r2, fp
	blx r3
	sub r7, r7, #0x40
	cmp r7, #0
	bgt _020D94C0
_020D94F4:
	str r8, [sl, #0x1c]
	cmp r8, #0
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	mov r0, sb
	mov r1, r6
	mov r2, r8
	bl MI_CpuCopy8
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_020D9514: .word _02110E70
	arm_func_end DGT_Hash2SetSource

	arm_func_start DGT_Hash2GetDigest
DGT_Hash2GetDigest: ; 0x020D9518
	stmdb sp!, {r0, r1, r2, r3}
	stmdb sp!, {r4, r5, r6, lr}
	ldr r0, [sp, #0x10]
	mov r6, r1
	ldr r3, [r0, #0x1c]
	add r4, r0, #0x20
	tst r3, #3
	mov r2, r3, asr #2
	moveq r0, #0
	streq r0, [r4, r2, lsl #2]
	ldr r1, [sp, #0x10]
	mov r0, #0x80
	add r5, r1, #0x20
	strb r0, [r5, r3]
	add r3, r3, #1
	tst r3, #3
	beq _020D9570
	mov r0, #0
_020D9560:
	strb r0, [r5, r3]
	add r3, r3, #1
	tst r3, #3
	bne _020D9560
_020D9570:
	ldr r0, [sp, #0x10]
	add r2, r2, #1
	ldr r0, [r0, #0x1c]
	cmp r0, #0x38
	blt _020D95BC
	cmp r2, #0x10
	bge _020D95A0
	mov r0, #0
_020D9590:
	str r0, [r4, r2, lsl #2]
	add r2, r2, #1
	cmp r2, #0x10
	blt _020D9590
_020D95A0:
	ldr r1, _020D9710 ; =_02110E70
	ldr r0, [sp, #0x10]
	ldr r3, [r1]
	mov r1, r4
	mov r2, #0x40
	blx r3
	mov r2, #0
_020D95BC:
	cmp r2, #0xe
	bge _020D95D8
	mov r0, #0
_020D95C8:
	str r0, [r4, r2, lsl #2]
	add r2, r2, #1
	cmp r2, #0xe
	blt _020D95C8
_020D95D8:
	ldr r0, [sp, #0x10]
	ldr r3, _020D9710 ; =_02110E70
	ldr r2, [r0, #0x14]
	mov r1, r4
	strb r2, [r5, #0x3f]
	mov r0, r2, lsr #8
	strb r0, [r5, #0x3e]
	mov r0, r2, lsr #0x10
	strb r0, [r5, #0x3d]
	mov r0, r2, lsr #0x18
	strb r0, [r5, #0x3c]
	ldr r0, [sp, #0x10]
	mov r2, #0x40
	ldr r4, [r0, #0x18]
	strb r4, [r5, #0x3b]
	mov r0, r4, lsr #8
	strb r0, [r5, #0x3a]
	mov r0, r4, lsr #0x10
	strb r0, [r5, #0x39]
	mov r0, r4, lsr #0x18
	strb r0, [r5, #0x38]
	ldr r0, [sp, #0x10]
	ldr r3, [r3]
	blx r3
	ldr r0, [sp, #0x10]
	ldr r1, [r0]
	mov r0, r1, lsr #0x18
	strb r0, [r6]
	mov r0, r1, lsr #0x10
	strb r0, [r6, #1]
	mov r0, r1, lsr #8
	strb r0, [r6, #2]
	strb r1, [r6, #3]
	ldr r0, [sp, #0x10]
	ldr r1, [r0, #4]
	mov r0, r1, lsr #0x18
	strb r0, [r6, #4]
	mov r0, r1, lsr #0x10
	strb r0, [r6, #5]
	mov r0, r1, lsr #8
	strb r0, [r6, #6]
	strb r1, [r6, #7]
	ldr r0, [sp, #0x10]
	ldr r1, [r0, #8]
	mov r0, r1, lsr #0x18
	strb r0, [r6, #8]
	mov r0, r1, lsr #0x10
	strb r0, [r6, #9]
	mov r0, r1, lsr #8
	strb r0, [r6, #0xa]
	strb r1, [r6, #0xb]
	ldr r0, [sp, #0x10]
	ldr r1, [r0, #0xc]
	mov r0, r1, lsr #0x18
	strb r0, [r6, #0xc]
	mov r0, r1, lsr #0x10
	strb r0, [r6, #0xd]
	mov r0, r1, lsr #8
	strb r0, [r6, #0xe]
	strb r1, [r6, #0xf]
	ldr r0, [sp, #0x10]
	ldr r1, [r0, #0x10]
	mov r0, r1, lsr #0x18
	strb r0, [r6, #0x10]
	mov r0, r1, lsr #0x10
	strb r0, [r6, #0x11]
	mov r0, r1, lsr #8
	strb r0, [r6, #0x12]
	strb r1, [r6, #0x13]
	ldr r3, [sp, #0x10]
	mov r0, #0
	add r1, sp, #0x10
	mov r2, #4
	str r0, [r3, #0x1c]
	bl MIi_CpuClear32
	ldmia sp!, {r4, r5, r6, lr}
	add sp, sp, #0x10
	bx lr
	.align 2, 0
_020D9710: .word _02110E70
	arm_func_end DGT_Hash2GetDigest

	arm_func_start DGT_Hash2CalcHmac
DGT_Hash2CalcHmac: ; 0x020D9714
	stmdb sp!, {r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0xa0
	ldr lr, _020D9798 ; =_0210E188
	add ip, sp, #8
	mov r8, r0
	mov r7, r1
	mov r6, r2
	mov r5, r3
	mov r4, ip
	ldmia lr!, {r0, r1, r2, r3}
	stmia ip!, {r0, r1, r2, r3}
	ldmia lr, {r0, r1, r2}
	stmia ip, {r0, r1, r2}
	add lr, sp, #0x38
	add ip, sp, #0x24
	str lr, [sp, #0x10]
	ldr lr, _020D979C ; =DGT_Hash2Reset
	str ip, [sp, #0x14]
	ldr ip, _020D97A0 ; =DGT_Hash2SetSource
	str lr, [sp, #0x18]
	ldr lr, _020D97A4 ; =DGT_Hash2GetDigest
	str ip, [sp, #0x1c]
	ldr ip, [sp, #0xb8]
	str lr, [sp, #0x20]
	str ip, [sp]
	mov r3, r5
	mov r0, r8
	mov r1, r7
	mov r2, r6
	str r4, [sp, #4]
	bl HmacCalc
	add sp, sp, #0xa0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	.align 2, 0
_020D9798: .word _0210E188
_020D979C: .word DGT_Hash2Reset
_020D97A0: .word DGT_Hash2SetSource
_020D97A4: .word DGT_Hash2GetDigest
	arm_func_end DGT_Hash2CalcHmac

	arm_func_start HmacCalc
HmacCalc: ; 0x020D97A8
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, lr}
	sub sp, sp, #0xc0
	ldr r5, [sp, #0xe0]
	ldr r4, [sp, #0xe4]
	mov r8, r1
	movs sb, r0
	mov r7, r2
	cmpne r8, #0
	mov r6, r3
	cmpne r7, #0
	cmpne r6, #0
	cmpne r5, #0
	cmpne r4, #0
	addeq sp, sp, #0xc0
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	ldr r0, [r4, #4]
	cmp r5, r0
	ble _020D9828
	ldr r0, [r4, #8]
	ldr r1, [r4, #0x10]
	blx r1
	ldr r0, [r4, #8]
	ldr r3, [r4, #0x14]
	mov r1, r6
	mov r2, r5
	blx r3
	ldr r0, [r4, #8]
	ldr r2, [r4, #0x18]
	add r1, sp, #0x80
	blx r2
	ldr r5, [r4]
	add r6, sp, #0x80
_020D9828:
	cmp r5, #0
	mov r2, #0
	ble _020D9850
	add r1, sp, #0x40
_020D9838:
	ldrb r0, [r6, r2]
	add r2, r2, #1
	cmp r2, r5
	eor r0, r0, #0x36
	strb r0, [r1], #1
	blt _020D9838
_020D9850:
	ldr r0, [r4, #4]
	cmp r2, r0
	bge _020D987C
	add r0, sp, #0x40
	add r3, r0, r2
	mov r1, #0x36
_020D9868:
	strb r1, [r3], #1
	ldr r0, [r4, #4]
	add r2, r2, #1
	cmp r2, r0
	blt _020D9868
_020D987C:
	ldr r0, [r4, #8]
	ldr r1, [r4, #0x10]
	blx r1
	ldr r0, [r4, #8]
	ldr r2, [r4, #4]
	ldr r3, [r4, #0x14]
	add r1, sp, #0x40
	blx r3
	ldr r0, [r4, #8]
	ldr r3, [r4, #0x14]
	mov r1, r8
	mov r2, r7
	blx r3
	ldr r0, [r4, #8]
	ldr r1, [r4, #0xc]
	ldr r2, [r4, #0x18]
	blx r2
	cmp r5, #0
	mov r3, #0
	ble _020D98E8
	add r1, sp, #0
_020D98D0:
	ldrb r0, [r6, r3]
	add r3, r3, #1
	cmp r3, r5
	eor r0, r0, #0x5c
	strb r0, [r1], #1
	blt _020D98D0
_020D98E8:
	ldr r0, [r4, #4]
	cmp r3, r0
	bge _020D9914
	add r0, sp, #0
	add r2, r0, r3
	mov r1, #0x5c
_020D9900:
	strb r1, [r2], #1
	ldr r0, [r4, #4]
	add r3, r3, #1
	cmp r3, r0
	blt _020D9900
_020D9914:
	ldr r0, [r4, #8]
	ldr r1, [r4, #0x10]
	blx r1
	ldr r0, [r4, #8]
	ldr r2, [r4, #4]
	ldr r3, [r4, #0x14]
	add r1, sp, #0
	blx r3
	ldr r0, [r4, #8]
	ldr r1, [r4, #0xc]
	ldr r2, [r4]
	ldr r3, [r4, #0x14]
	blx r3
	ldr r0, [r4, #8]
	ldr r2, [r4, #0x18]
	mov r1, sb
	blx r2
	add sp, sp, #0xc0
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	.align 2, 0
	arm_func_end HmacCalc

_020D9960: .word 0x00FF00FF
_020D9964: .word 0x5A827999
_020D9968: .word 0x6ED9EBA1
_020D996C: .word 0x8F1BBCDC
_020D9970: .word 0xCA62C1D6

	arm_func_start DGTi_hash2_arm4_small
DGTi_hash2_arm4_small: ; 0x020D9974
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, fp, ip, lr}
	ldmia r0, {r3, sb, sl, fp, ip}
	sub sp, sp, #0x84
	str r2, [sp, #0x80]
_020D9984:
	ldr r8, _020D9964 ; =0x5A827999
	ldr r7, _020D9960 ; =0x00FF00FF
	mov r6, sp
	mov r5, #0
_020D9994:
	ldr r4, [r1], #4
	add r2, r8, ip
	add r2, r2, r3, ror #27
	and lr, r4, r7
	and r4, r7, r4, ror #24
	orr r4, r4, lr, ror #8
	str r4, [r6, #0x40]
	str r4, [r6], #4
	add r2, r2, r4
	eor r4, sl, fp
	and r4, r4, sb
	eor r4, r4, fp
	add r2, r2, r4
	mov sb, sb, ror #2
	mov ip, fp
	mov fp, sl
	mov sl, sb
	mov sb, r3
	mov r3, r2
	add r5, r5, #4
	cmp r5, #0x40
	blt _020D9994
	mov r7, #0
	mov r6, sp
_020D99F4:
	ldr r2, [r6]
	ldr r5, [r6, #8]
	ldr r4, [r6, #0x20]
	ldr lr, [r6, #0x34]
	eor r2, r2, r5
	eor r4, r4, lr
	eor r2, r2, r4
	mov r2, r2, ror #0x1f
	str r2, [r6, #0x40]
	str r2, [r6], #4
	add r2, r2, ip
	add r2, r2, r8
	add r2, r2, r3, ror #27
	eor r4, sl, fp
	and r4, r4, sb
	eor r4, r4, fp
	add r2, r2, r4
	mov sb, sb, ror #2
	mov ip, fp
	mov fp, sl
	mov sl, sb
	mov sb, r3
	mov r3, r2
	add r7, r7, #4
	cmp r7, #0x10
	blt _020D99F4
	ldr r8, _020D9968 ; =0x6ED9EBA1
	mov r7, #0
_020D9A64:
	ldr r2, [r6]
	ldr r4, [r6, #8]
	ldr lr, [r6, #0x20]
	ldr r5, [r6, #0x34]
	eor r2, r2, r4
	eor lr, lr, r5
	eor r2, r2, lr
	mov r2, r2, ror #0x1f
	str r2, [r6, #0x40]
	str r2, [r6], #4
	add r2, r2, ip
	add r2, r2, r8
	add r2, r2, r3, ror #27
	eor lr, sb, sl
	eor lr, lr, fp
	add r2, r2, lr
	mov sb, sb, ror #2
	mov ip, fp
	mov fp, sl
	mov sl, sb
	mov sb, r3
	mov r3, r2
	add r7, r7, #1
	cmp r7, #0xc
	moveq r6, sp
	cmp r7, #0x14
	blt _020D9A64
	ldr r8, _020D996C ; =0x8F1BBCDC
	mov r7, #0
_020D9AD8:
	ldr r2, [r6]
	ldr lr, [r6, #8]
	ldr r5, [r6, #0x20]
	ldr r4, [r6, #0x34]
	eor r2, r2, lr
	eor r5, r5, r4
	eor r2, r2, r5
	mov r2, r2, ror #0x1f
	str r2, [r6, #0x40]
	str r2, [r6], #4
	add r2, r2, ip
	add r2, r2, r8
	add r2, r2, r3, ror #27
	orr r5, sb, sl
	and r5, r5, fp
	and r4, sb, sl
	orr r5, r5, r4
	add r2, r2, r5
	mov sb, sb, ror #2
	mov ip, fp
	mov fp, sl
	mov sl, sb
	mov sb, r3
	mov r3, r2
	add r7, r7, #1
	cmp r7, #8
	moveq r6, sp
	cmp r7, #0x14
	blt _020D9AD8
	ldr r8, _020D9970 ; =0xCA62C1D6
	mov r7, #0
_020D9B54:
	ldr r2, [r6]
	ldr r5, [r6, #8]
	ldr r4, [r6, #0x20]
	ldr lr, [r6, #0x34]
	eor r2, r2, r5
	eor r4, r4, lr
	eor r2, r2, r4
	mov r2, r2, ror #0x1f
	str r2, [r6, #0x40]
	str r2, [r6], #4
	add r2, r2, ip
	add r2, r2, r8
	add r2, r2, r3, ror #27
	eor r4, sb, sl
	eor r4, r4, fp
	add r2, r2, r4
	mov sb, sb, ror #2
	mov ip, fp
	mov fp, sl
	mov sl, sb
	mov sb, r3
	mov r3, r2
	add r7, r7, #1
	cmp r7, #4
	moveq r6, sp
	cmp r7, #0x14
	blt _020D9B54
	ldmia r0, {r2, r4, r6, r7, lr}
	add r3, r3, r2
	add sb, sb, r4
	add sl, sl, r6
	add fp, fp, r7
	add ip, ip, lr
	stmia r0, {r3, sb, sl, fp, ip}
	ldr lr, [sp, #0x80]
	subs lr, lr, #0x40
	str lr, [sp, #0x80]
	bgt _020D9984
	add sp, sp, #0x84
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, ip, pc}
	arm_func_end DGTi_hash2_arm4_small

	.section .text.unified_23,4,1,6

	arm_func_start TPi_TpCallback
TPi_TpCallback: ; 0x020D9C70
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	sub sp, sp, #8
	mov r0, r1, lsl #0x10
	mov r3, r0, lsr #0x10
	and r0, r3, #0x7f00
	mov r0, r0, lsl #8
	cmp r2, #0
	mov r0, r0, lsr #0x10
	beq _020D9CCC
	ldr r1, _020D9EE8 ; =_021E36B0
	mov r2, #1
	ldrh r3, [r1, #0x38]
	orr r2, r3, r2, lsl r0
	strh r2, [r1, #0x38]
	ldr r3, [r1, #4]
	cmp r3, #0
	addeq sp, sp, #8
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	mov r1, #4
	mov r2, #0
	blx r3
	add sp, sp, #8
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_020D9CCC:
	cmp r0, #0x10
	bne _020D9D84
	ldr r1, _020D9EE8 ; =_021E36B0
	ldrh r2, [r1, #0x10]
	add r2, r2, #1
	strh r2, [r1, #0x10]
	ldrh r3, [r1, #0x10]
	ldrh r2, [r1, #0x18]
	cmp r3, r2
	movhs r2, #0
	strhsh r2, [r1, #0x10]
	ldr r2, _020D9EEC ; =0x027FFFAA
	ldr r1, _020D9EE8 ; =_021E36B0
	ldrh r3, [r2]
	ldrh r2, [r2, #2]
	ldrh lr, [r1, #0x10]
	strh r3, [sp, #4]
	strh r2, [sp, #6]
	ldr r5, [sp, #4]
	ldr r7, [r1, #0x14]
	mov r2, r5, lsl #7
	mov r3, r2, lsr #0x1f
	mov r4, r5, lsl #8
	mov r2, r5, lsl #5
	mov ip, r5, lsl #0x14
	mov r2, r2, lsr #0x1e
	mov r6, lr, lsl #3
	mov r5, ip, lsr #0x14
	strh r5, [r7, r6]
	add r5, r7, lr, lsl #3
	mov r4, r4, lsr #0x14
	strh r4, [r5, #2]
	and r3, r3, #0xff
	strh r3, [r5, #4]
	and r2, r2, #0xff
	strh r2, [r5, #6]
	ldr r3, [r1, #4]
	cmp r3, #0
	addeq sp, sp, #8
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	ldrh r2, [r1, #0x10]
	mov r1, #0
	and r2, r2, #0xff
	blx r3
	add sp, sp, #8
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_020D9D84:
	tst r1, #0x1000000
	addeq sp, sp, #8
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	and r1, r3, #0xff
	cmp r1, #4
	addls pc, pc, r1, lsl #2
	b _020D9EDC
_020D9DA0: ; jump table
	b _020D9DB4 ; case 0
	b _020D9EDC ; case 1
	b _020D9E88 ; case 2
	b _020D9E90 ; case 3
	b _020D9E80 ; case 4
_020D9DB4:
	cmp r0, #0
	beq _020D9DD8
	cmp r0, #1
	beq _020D9E38
	cmp r0, #2
	ldreq r1, _020D9EE8 ; =_021E36B0
	moveq r2, #0
	streqh r2, [r1, #0x36]
	b _020D9E44
_020D9DD8:
	ldr r2, _020D9EEC ; =0x027FFFAA
	ldr r1, _020D9EE8 ; =_021E36B0
	ldrh r4, [r2]
	ldrh r3, [r2, #2]
	mov r2, #0
	strh r4, [sp]
	strh r3, [sp, #2]
	ldr r4, [sp]
	mov r3, r4, lsl #0x14
	mov r5, r3, lsr #0x14
	mov lr, r4, lsl #8
	mov r3, r4, lsl #7
	mov ip, r3, lsr #0x1f
	mov r3, r4, lsl #5
	mov r3, r3, lsr #0x1e
	strh r5, [r1, #8]
	mov lr, lr, lsr #0x14
	strh lr, [r1, #0xa]
	and ip, ip, #0xff
	strh ip, [r1, #0xc]
	and r3, r3, #0xff
	strh r3, [r1, #0xe]
	strh r2, [r1, #0x36]
	b _020D9E44
_020D9E38:
	ldr r1, _020D9EE8 ; =_021E36B0
	mov r2, #2
	strh r2, [r1, #0x36]
_020D9E44:
	ldr r1, _020D9EE8 ; =_021E36B0
	mov r2, #1
	ldrh r3, [r1, #0x3a]
	mvn r2, r2, lsl r0
	and r2, r3, r2
	strh r2, [r1, #0x3a]
	ldr r3, [r1, #4]
	cmp r3, #0
	addeq sp, sp, #8
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	mov r1, #0
	mov r2, r1
	blx r3
	add sp, sp, #8
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_020D9E80:
	mov r4, #3
	b _020D9E94
_020D9E88:
	mov r4, #1
	b _020D9E94
_020D9E90:
	mov r4, #2
_020D9E94:
	ldr r1, _020D9EE8 ; =_021E36B0
	mov ip, #1
	ldrh r3, [r1, #0x38]
	mvn r2, ip, lsl r0
	orr r3, r3, ip, lsl r0
	strh r3, [r1, #0x38]
	ldrh r3, [r1, #0x3a]
	and r2, r3, r2
	strh r2, [r1, #0x3a]
	ldr r3, [r1, #4]
	cmp r3, #0
	addeq sp, sp, #8
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	and r1, r4, #0xff
	mov r2, #0
	blx r3
	add sp, sp, #8
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_020D9EDC:
	bl OS_Terminate
	add sp, sp, #8
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.align 2, 0
_020D9EE8: .word _021E36B0
_020D9EEC: .word 0x027FFFAA
	arm_func_end TPi_TpCallback

	arm_func_start TP_Init
TP_Init: ; 0x020D9EF0
	stmdb sp!, {r3, r4, r5, lr}
	ldr r0, _020D9F60 ; =_021E36B0
	ldrh r1, [r0]
	cmp r1, #0
	ldmneia sp!, {r3, r4, r5, pc}
	mov r1, #1
	strh r1, [r0]
	bl PXI_Init
	ldr r0, _020D9F60 ; =_021E36B0
	mov r1, #0
	strh r1, [r0, #0x10]
	str r1, [r0, #4]
	str r1, [r0, #0x14]
	strh r1, [r0, #0x36]
	strh r1, [r0, #0x34]
	strh r1, [r0, #0x3a]
	strh r1, [r0, #0x38]
	mov r5, #6
	mov r4, #1
_020D9F3C:
	mov r0, r5
	mov r1, r4
	bl PXI_IsCallbackReady
	cmp r0, #0
	beq _020D9F3C
	ldr r1, _020D9F64 ; =TPi_TpCallback
	mov r0, #6
	bl PXI_SetFifoRecvCallback
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_020D9F60: .word _021E36B0
_020D9F64: .word TPi_TpCallback
	arm_func_end TP_Init

	arm_func_start TP_GetUserInfo
TP_GetUserInfo: ; 0x020D9F68
	stmdb sp!, {r3, r4, r5, r6, lr}
	sub sp, sp, #0x14
	ldr ip, _020D9FF8 ; =0x027FFC80
	mov r4, r0
	ldrh r1, [ip, #0x58]
	ldrh r2, [ip, #0x5a]
	ldrh r0, [ip, #0x5e]
	cmp r1, #0
	ldrh lr, [ip, #0x60]
	cmpeq r0, #0
	cmpeq r2, #0
	ldrb r3, [ip, #0x5c]
	ldrb r5, [ip, #0x5d]
	ldrb r6, [ip, #0x62]
	ldrb ip, [ip, #0x63]
	cmpeq lr, #0
	beq _020D9FCC
	str r5, [sp]
	stmib sp, {r0, lr}
	str r6, [sp, #0xc]
	mov r0, r4
	str ip, [sp, #0x10]
	bl TP_CalcCalibrateParam
	cmp r0, #0
	beq _020D9FEC
_020D9FCC:
	mov r0, #0
	strh r0, [r4]
	strh r0, [r4, #2]
	strh r0, [r4, #4]
	strh r0, [r4, #6]
	add sp, sp, #0x14
	mov r0, #1
	ldmia sp!, {r3, r4, r5, r6, pc}
_020D9FEC:
	mov r0, #1
	add sp, sp, #0x14
	ldmia sp!, {r3, r4, r5, r6, pc}
	.align 2, 0
_020D9FF8: .word 0x027FFC80
	arm_func_end TP_GetUserInfo

	arm_func_start TP_SetCalibrateParam
TP_SetCalibrateParam: ; 0x020D9FFC
	stmdb sp!, {r4, lr}
	movs r4, r0
	bne _020DA018
	ldr r0, _020DA118 ; =_021E36B0
	mov r1, #0
	strh r1, [r0, #0x34]
	ldmia sp!, {r4, pc}
_020DA018:
	bl OS_DisableInterrupts
	ldrsh r2, [r4, #4]
	cmp r2, #0
	beq _020DA07C
	ldr r3, _020DA11C ; =0x04000280
	mov r1, #0
	strh r1, [r3]
	mov r1, #0x10000000
	str r1, [r3, #0x10]
	str r2, [r3, #0x18]
	mov r1, #0
	str r1, [r3, #0x1c]
	ldrsh r2, [r4]
	ldr r1, _020DA118 ; =_021E36B0
	str r2, [r1, #0x1c]
	ldrsh r2, [r4, #4]
	str r2, [r1, #0x20]
_020DA05C:
	ldrh r1, [r3]
	tst r1, #0x8000
	bne _020DA05C
	ldr r2, _020DA120 ; =0x040002A0
	ldr r1, _020DA118 ; =_021E36B0
	ldr r2, [r2]
	str r2, [r1, #0x24]
	b _020DA090
_020DA07C:
	ldr r1, _020DA118 ; =_021E36B0
	mov r2, #0
	str r2, [r1, #0x1c]
	str r2, [r1, #0x20]
	str r2, [r1, #0x24]
_020DA090:
	ldrsh r2, [r4, #6]
	cmp r2, #0
	beq _020DA0F0
	ldr r3, _020DA11C ; =0x04000280
	mov r1, #0
	strh r1, [r3]
	mov r1, #0x10000000
	str r1, [r3, #0x10]
	str r2, [r3, #0x18]
	mov r1, #0
	str r1, [r3, #0x1c]
	ldrsh r2, [r4, #2]
	ldr r1, _020DA118 ; =_021E36B0
	str r2, [r1, #0x28]
	ldrsh r2, [r4, #6]
	str r2, [r1, #0x2c]
_020DA0D0:
	ldrh r1, [r3]
	tst r1, #0x8000
	bne _020DA0D0
	ldr r2, _020DA120 ; =0x040002A0
	ldr r1, _020DA118 ; =_021E36B0
	ldr r2, [r2]
	str r2, [r1, #0x30]
	b _020DA104
_020DA0F0:
	ldr r1, _020DA118 ; =_021E36B0
	mov r2, #0
	str r2, [r1, #0x28]
	str r2, [r1, #0x2c]
	str r2, [r1, #0x30]
_020DA104:
	bl OS_RestoreInterrupts
	ldr r0, _020DA118 ; =_021E36B0
	mov r1, #1
	strh r1, [r0, #0x34]
	ldmia sp!, {r4, pc}
	.align 2, 0
_020DA118: .word _021E36B0
_020DA11C: .word 0x04000280
_020DA120: .word 0x040002A0
	arm_func_end TP_SetCalibrateParam

	arm_func_start TP_RequestSamplingAsync
TP_RequestSamplingAsync: ; 0x020DA124
	stmdb sp!, {r4, lr}
	bl OS_DisableInterrupts
	mov r4, r0
	mov r0, #6
	mov r1, #0x3000000
	mov r2, #0
	bl PXI_SendWordByFifo
	cmp r0, #0
	movge r0, #1
	movlt r0, #0
	cmp r0, #0
	bne _020DA18C
	mov r0, r4
	bl OS_RestoreInterrupts
	ldr r0, _020DA1B4 ; =_021E36B0
	ldrh r1, [r0, #0x38]
	orr r1, r1, #1
	strh r1, [r0, #0x38]
	ldr r3, [r0, #4]
	cmp r3, #0
	ldmeqia sp!, {r4, pc}
	mov r0, #0
	mov r2, r0
	mov r1, #4
	blx r3
	ldmia sp!, {r4, pc}
_020DA18C:
	ldr r1, _020DA1B4 ; =_021E36B0
	mov r0, r4
	ldrh r2, [r1, #0x3a]
	orr r2, r2, #1
	strh r2, [r1, #0x3a]
	ldrh r2, [r1, #0x38]
	bic r2, r2, #1
	strh r2, [r1, #0x38]
	bl OS_RestoreInterrupts
	ldmia sp!, {r4, pc}
	.align 2, 0
_020DA1B4: .word _021E36B0
	arm_func_end TP_RequestSamplingAsync

	arm_func_start TP_WaitRawResult
TP_WaitRawResult: ; 0x020DA1B8
	stmdb sp!, {r4, lr}
	mov r4, r0
	mov r0, #1
	bl TP_WaitBusy
	ldr r1, _020DA204 ; =_021E36B0
	ldrh r0, [r1, #0x38]
	tst r0, #1
	movne r0, #1
	ldmneia sp!, {r4, pc}
	ldrh r3, [r1, #8]
	ldrh r2, [r1, #0xa]
	mov r0, #0
	strh r3, [r4]
	strh r2, [r4, #2]
	ldrh r2, [r1, #0xc]
	ldrh r1, [r1, #0xe]
	strh r2, [r4, #4]
	strh r1, [r4, #6]
	ldmia sp!, {r4, pc}
	.align 2, 0
_020DA204: .word _021E36B0
	arm_func_end TP_WaitRawResult

	arm_func_start TP_RequestAutoSamplingStartAsync
TP_RequestAutoSamplingStartAsync: ; 0x020DA208
	stmdb sp!, {r3, r4, r5, lr}
	ldr ip, _020DA304 ; =_021E36B0
	mov lr, #0
	str r2, [ip, #0x14]
	mov r5, r1
	strh lr, [ip, #0x10]
	strh r5, [ip, #0x12]
	mov r4, r0
	strh r3, [ip, #0x18]
	cmp r3, #0
	bls _020DA250
	mov r1, lr
_020DA238:
	ldr r0, [ip, #0x14]
	add r0, r0, lr, lsl #3
	add lr, lr, #1
	strh r1, [r0, #4]
	cmp lr, r3
	blo _020DA238
_020DA250:
	bl OS_DisableInterrupts
	and r1, r5, #0xff
	orr r1, r1, #0x100
	mov r5, r0
	orr r1, r1, #0x2000000
	mov r0, #6
	mov r2, #0
	bl PXI_SendWordByFifo
	cmp r0, #0
	movlt r0, #0
	blt _020DA29C
	orr r1, r4, #0x10000
	orr r1, r1, #0x1000000
	mov r0, #6
	mov r2, #0
	bl PXI_SendWordByFifo
	cmp r0, #0
	movlt r0, #0
	movge r0, #1
_020DA29C:
	tst r0, #0xff
	bne _020DA2DC
	mov r0, r5
	bl OS_RestoreInterrupts
	ldr r0, _020DA304 ; =_021E36B0
	ldrh r1, [r0, #0x38]
	orr r1, r1, #2
	strh r1, [r0, #0x38]
	ldr r3, [r0, #4]
	cmp r3, #0
	ldmeqia sp!, {r3, r4, r5, pc}
	mov r0, #1
	mov r1, #4
	mov r2, #0
	blx r3
	ldmia sp!, {r3, r4, r5, pc}
_020DA2DC:
	ldr r1, _020DA304 ; =_021E36B0
	mov r0, r5
	ldrh r2, [r1, #0x3a]
	orr r2, r2, #2
	strh r2, [r1, #0x3a]
	ldrh r2, [r1, #0x38]
	bic r2, r2, #2
	strh r2, [r1, #0x38]
	bl OS_RestoreInterrupts
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_020DA304: .word _021E36B0
	arm_func_end TP_RequestAutoSamplingStartAsync

	arm_func_start TP_RequestAutoSamplingStopAsync
TP_RequestAutoSamplingStopAsync: ; 0x020DA308
	stmdb sp!, {r4, lr}
	bl OS_DisableInterrupts
	mov r4, r0
	ldr r1, _020DA398 ; =0x03000200
	mov r0, #6
	mov r2, #0
	bl PXI_SendWordByFifo
	cmp r0, #0
	movge r0, #1
	movlt r0, #0
	cmp r0, #0
	bne _020DA370
	mov r0, r4
	bl OS_RestoreInterrupts
	ldr r0, _020DA39C ; =_021E36B0
	ldrh r1, [r0, #0x38]
	orr r1, r1, #4
	strh r1, [r0, #0x38]
	ldr r3, [r0, #4]
	cmp r3, #0
	ldmeqia sp!, {r4, pc}
	mov r0, #2
	mov r1, #4
	mov r2, #0
	blx r3
	ldmia sp!, {r4, pc}
_020DA370:
	ldr r1, _020DA39C ; =_021E36B0
	mov r0, r4
	ldrh r2, [r1, #0x3a]
	orr r2, r2, #4
	strh r2, [r1, #0x3a]
	ldrh r2, [r1, #0x38]
	bic r2, r2, #4
	strh r2, [r1, #0x38]
	bl OS_RestoreInterrupts
	ldmia sp!, {r4, pc}
	.align 2, 0
_020DA398: .word 0x03000200
_020DA39C: .word _021E36B0
	arm_func_end TP_RequestAutoSamplingStopAsync

	arm_func_start TP_GetLatestRawPointInAuto
TP_GetLatestRawPointInAuto: ; 0x020DA3A0
	stmdb sp!, {r4, lr}
	ldr ip, _020DA4E0 ; =_021E36B0
	mov r1, #3
	strh r1, [r0, #6]
	ldrh r1, [ip, #0x12]
	ldrh r2, [ip, #0x10]
	cmp r1, #1
	ldrneh r1, [ip, #0x18]
	cmpne r1, #1
	bne _020DA3FC
	ldr r1, _020DA4E0 ; =_021E36B0
	mov ip, r2, lsl #3
	ldr r1, [r1, #0x14]
	add r3, r1, r2, lsl #3
	ldrh r2, [r1, ip]
	ldrh r1, [r3, #2]
	strh r2, [r0]
	strh r1, [r0, #2]
	ldrh r2, [r3, #4]
	ldrh r1, [r3, #6]
	strh r2, [r0, #4]
	strh r1, [r0, #6]
	ldmia sp!, {r4, pc}
_020DA3FC:
	mov r1, #0
	b _020DA4BC
_020DA404:
	subs lr, r2, r1
	ldr r3, [ip, #0x14]
	addmi lr, lr, r4
	add r3, r3, lr, lsl #3
	ldrh lr, [r3, #4]
	cmp lr, #0
	bne _020DA444
	ldrh r2, [r3]
	ldrh r1, [r3, #2]
	strh r2, [r0]
	strh r1, [r0, #2]
	ldrh r2, [r3, #4]
	ldrh r1, [r3, #6]
	strh r2, [r0, #4]
	strh r1, [r0, #6]
	ldmia sp!, {r4, pc}
_020DA444:
	ldrh lr, [r0, #6]
	tst lr, #1
	beq _020DA474
	ldrh lr, [r3, #6]
	tst lr, #1
	bne _020DA474
	ldrh lr, [r3]
	cmp r1, #0
	strh lr, [r0]
	ldrneh lr, [r0, #6]
	bicne lr, lr, #1
	strneh lr, [r0, #6]
_020DA474:
	ldrh lr, [r0, #6]
	tst lr, #2
	beq _020DA4A4
	ldrh lr, [r3, #6]
	tst lr, #2
	bne _020DA4A4
	ldrh r3, [r3, #2]
	cmp r1, #0
	strh r3, [r0, #2]
	ldrneh r3, [r0, #6]
	bicne r3, r3, #2
	strneh r3, [r0, #6]
_020DA4A4:
	ldrh r3, [r0, #6]
	cmp r3, #0
	moveq r1, #1
	streqh r1, [r0, #4]
	ldmeqia sp!, {r4, pc}
	add r1, r1, #1
_020DA4BC:
	ldrh r3, [ip, #0x12]
	cmp r1, r3
	ldrlth r4, [ip, #0x18]
	sublt r3, r4, #1
	cmplt r1, r3
	blt _020DA404
	mov r1, #1
	strh r1, [r0, #4]
	ldmia sp!, {r4, pc}
	.align 2, 0
_020DA4E0: .word _021E36B0
	arm_func_end TP_GetLatestRawPointInAuto

	arm_func_start TP_GetLatestIndexInAuto
TP_GetLatestIndexInAuto: ; 0x020DA4E4
	ldr r0, _020DA4F0 ; =_021E36B0
	ldrh r0, [r0, #0x10]
	bx lr
	.align 2, 0
_020DA4F0: .word _021E36B0
	arm_func_end TP_GetLatestIndexInAuto

	arm_func_start TP_CalcCalibrateParam
TP_CalcCalibrateParam: ; 0x020DA4F4
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	mov sb, r1
	mov r8, r2
	cmp sb, #0x1000
	cmplo r8, #0x1000
	ldrloh r4, [sp, #0x2c]
	mov r7, r3
	mov sl, r0
	cmplo r4, #0x1000
	ldrloh r3, [sp, #0x30]
	cmplo r3, #0x1000
	movhs r0, #1
	ldmhsia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	cmp r7, #0x100
	ldrloh r2, [sp, #0x34]
	cmplo r2, #0x100
	ldrloh r1, [sp, #0x28]
	cmplo r1, #0xc0
	ldrloh r0, [sp, #0x38]
	cmplo r0, #0xc0
	movhs r0, #1
	ldmhsia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	cmp r7, r2
	cmpne r1, r0
	cmpne sb, r4
	cmpne r8, r3
	moveq r0, #1
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	bl OS_DisableInterrupts
	ldrh lr, [sp, #0x2c]
	ldrh ip, [sp, #0x34]
	ldr r3, _020DA6D8 ; =0x04000280
	sub r1, sb, lr
	mov r2, #0
	ldrh r4, [sp, #0x38]
	ldrh r5, [sp, #0x28]
	ldrh r6, [sp, #0x30]
	strh r2, [r3]
	mov r1, r1, lsl #8
	str r1, [r3, #0x10]
	sub r1, r7, ip
	str r1, [r3, #0x18]
	mov r1, r2
	sub r2, r5, r4
	str r1, [r3, #0x1c]
	sub r1, r8, r6
	str r2, [sp]
_020DA5B0:
	ldrh r2, [r3]
	tst r2, #0x8000
	bne _020DA5B0
	mov fp, r1, lsl #8
	ldr r1, _020DA6DC ; =0x040002A0
	ldr r2, [r1]
	mov r1, #0
	strh r1, [r3]
	ldr r1, _020DA6DC ; =0x040002A0
	cmp r2, #0x8000
	str fp, [r1, #-0x10]
	sub r3, r1, #8
	ldr r1, [sp]
	mov fp, #0
	stmia r3, {r1, fp}
	bge _020DA600
	mov r1, fp
	sub r1, r1, #0x8000
	cmp r2, r1
	bge _020DA60C
_020DA600:
	bl OS_RestoreInterrupts
	mov r0, #1
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020DA60C:
	strh r2, [sl, #4]
	ldrsh r2, [sl, #4]
	add r1, r7, ip
	add r3, sb, lr
	mul r1, r2, r1
	rsb r1, r1, r3, lsl #8
	mov r1, r1, lsl #9
	mov r2, r1, asr #0x10
	cmp r2, #0x8000
	bge _020DA644
	mov r1, fp
	sub r1, r1, #0x8000
	cmp r2, r1
	bge _020DA650
_020DA644:
	bl OS_RestoreInterrupts
	mov r0, #1
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020DA650:
	ldr r1, _020DA6DC ; =0x040002A0
	strh r2, [sl]
	sub r2, r1, #0x20
_020DA65C:
	ldrh r1, [r2]
	tst r1, #0x8000
	bne _020DA65C
	ldr r1, _020DA6DC ; =0x040002A0
	ldr r7, [r1]
	bl OS_RestoreInterrupts
	cmp r7, #0x8000
	bge _020DA68C
	mov r1, #0x8000
	rsb r1, r1, #0
	cmp r7, r1
	bge _020DA694
_020DA68C:
	mov r0, #1
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020DA694:
	strh r7, [sl, #6]
	ldrsh r2, [sl, #6]
	add r0, r5, r4
	add r3, r8, r6
	mul r0, r2, r0
	rsb r0, r0, r3, lsl #8
	mov r0, r0, lsl #9
	mov r0, r0, asr #0x10
	cmp r0, #0x8000
	bge _020DA6C4
	cmp r0, r1
	bge _020DA6CC
_020DA6C4:
	mov r0, #1
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020DA6CC:
	strh r0, [sl, #2]
	mov r0, #0
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_020DA6D8: .word 0x04000280
_020DA6DC: .word 0x040002A0
	arm_func_end TP_CalcCalibrateParam

	arm_func_start TP_GetCalibratedPoint
TP_GetCalibratedPoint: ; 0x020DA6E0
	stmdb sp!, {r4, r5, r6, lr}
	ldr r2, _020DA7FC ; =_021E36B0
	ldrh r2, [r2, #0x34]
	cmp r2, #0
	bne _020DA718
	ldrh r3, [r1]
	ldrh r2, [r1, #2]
	strh r3, [r0]
	strh r2, [r0, #2]
	ldrh r2, [r1, #4]
	ldrh r1, [r1, #6]
	strh r2, [r0, #4]
	strh r1, [r0, #6]
	ldmia sp!, {r4, r5, r6, pc}
_020DA718:
	ldrh r3, [r1, #4]
	ldr r2, _020DA800 ; =_021E36CC
	strh r3, [r0, #4]
	ldrh r3, [r1, #6]
	strh r3, [r0, #6]
	ldrh r3, [r1, #4]
	cmp r3, #0
	bne _020DA748
	mov r1, #0
	strh r1, [r0]
	strh r1, [r0, #2]
	ldmia sp!, {r4, r5, r6, pc}
_020DA748:
	ldrh r4, [r1]
	ldr r3, [r2]
	ldr lr, [r2, #8]
	mov r4, r4, lsl #2
	subs ip, r4, r3
	mov r3, r3, asr #0x1f
	rsc r3, r3, r4, asr #31
	umull r6, r5, lr, ip
	mla r5, lr, r3, r5
	mov r3, lr, asr #0x1f
	mla r5, r3, ip, r5
	mov r3, r6, lsr #0x16
	orr r3, r3, r5, lsl #10
	strh r3, [r0]
	ldrsh r3, [r0]
	cmp r3, #0
	movlt r3, #0
	strlth r3, [r0]
	blt _020DA7A0
	cmp r3, #0xff
	movgt r3, #0xff
	strgth r3, [r0]
_020DA7A0:
	ldrh r3, [r1, #2]
	ldr r1, [r2, #0xc]
	ldr ip, [r2, #0x14]
	mov r2, r3, lsl #2
	subs r3, r2, r1
	mov r1, r1, asr #0x1f
	rsc r1, r1, r2, asr #31
	umull r4, lr, ip, r3
	mla lr, ip, r1, lr
	mov r1, ip, asr #0x1f
	mla lr, r1, r3, lr
	mov r1, r4, lsr #0x16
	orr r1, r1, lr, lsl #10
	strh r1, [r0, #2]
	ldrsh r1, [r0, #2]
	cmp r1, #0
	movlt r1, #0
	strlth r1, [r0, #2]
	ldmltia sp!, {r4, r5, r6, pc}
	cmp r1, #0xbf
	movgt r1, #0xbf
	strgth r1, [r0, #2]
	ldmia sp!, {r4, r5, r6, pc}
	.align 2, 0
_020DA7FC: .word _021E36B0
_020DA800: .word _021E36CC
	arm_func_end TP_GetCalibratedPoint

	arm_func_start TP_WaitBusy
TP_WaitBusy: ; 0x020DA804
	ldr r1, _020DA818 ; =_021E36B0
_020DA808:
	ldrh r2, [r1, #0x3a]
	tst r2, r0
	bne _020DA808
	bx lr
	.align 2, 0
_020DA818: .word _021E36B0
	arm_func_end TP_WaitBusy

	arm_func_start TP_CheckBusy
TP_CheckBusy: ; 0x020DA81C
	ldr r1, _020DA82C ; =_021E36B0
	ldrh r1, [r1, #0x38]
	and r0, r1, r0
	bx lr
	.align 2, 0
_020DA82C: .word _021E36B0
	arm_func_end TP_CheckBusy

	arm_func_start MIC_Init
MIC_Init: ; 0x020DA830
	stmdb sp!, {r3, r4, r5, lr}
	ldr r0, _020DA894 ; =_021E36EC
	ldrh r1, [r0]
	cmp r1, #0
	ldmneia sp!, {r3, r4, r5, pc}
	mov r1, #1
	strh r1, [r0]
	mov r1, #0
	str r1, [r0, #4]
	str r1, [r0, #8]
	bl PXI_Init
	mov r5, #9
	mov r4, #1
_020DA864:
	mov r0, r5
	mov r1, r4
	bl PXI_IsCallbackReady
	cmp r0, #0
	beq _020DA864
	ldr r2, _020DA898 ; =0x027FFF90
	mov r3, #0
	ldr r1, _020DA89C ; =MicCommonCallback
	mov r0, #9
	str r3, [r2]
	bl PXI_SetFifoRecvCallback
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_020DA894: .word _021E36EC
_020DA898: .word 0x027FFF90
_020DA89C: .word MicCommonCallback
	arm_func_end MIC_Init

	arm_func_start MIC_StartAutoSamplingAsync
MIC_StartAutoSamplingAsync: ; 0x020DA8A0
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	mov r7, r0
	ldr r0, [r7, #4]
	mov r6, r1
	tst r0, #0x1f
	mov r5, r2
	movne r0, #2
	ldmneia sp!, {r3, r4, r5, r6, r7, pc}
	ldr r0, [r7, #8]
	tst r0, #0x1f
	movne r0, #2
	ldmneia sp!, {r3, r4, r5, r6, r7, pc}
	cmp r0, #0
	moveq r0, #2
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	ldr r0, [r7, #0xc]
	cmp r0, #0x400
	movlo r0, #2
	ldmloia sp!, {r3, r4, r5, r6, r7, pc}
	ldr r0, [r7]
	cmp r0, #5
	addls pc, pc, r0, lsl #2
	b _020DA944
_020DA8FC: ; jump table
	b _020DA914 ; case 0
	b _020DA91C ; case 1
	b _020DA924 ; case 2
	b _020DA92C ; case 3
	b _020DA934 ; case 4
	b _020DA93C ; case 5
_020DA914:
	mov r1, #0
	b _020DA94C
_020DA91C:
	mov r1, #1
	b _020DA94C
_020DA924:
	mov r1, #2
	b _020DA94C
_020DA92C:
	mov r1, #3
	b _020DA94C
_020DA934:
	mov r1, #5
	b _020DA94C
_020DA93C:
	mov r1, #7
	b _020DA94C
_020DA944:
	mov r0, #2
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_020DA94C:
	ldr r0, [r7, #0x10]
	cmp r0, #0
	orrne r0, r1, #0x10
	andne r0, r0, #0xff
	andeq r0, r1, #0xff
	and r4, r0, #0xff
	bl OS_DisableInterrupts
	ldr r1, _020DA9C8 ; =_021E36EC
	ldr r2, [r1, #4]
	cmp r2, #0
	beq _020DA984
	bl OS_RestoreInterrupts
	mov r0, #1
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_020DA984:
	mov r2, #1
	str r2, [r1, #4]
	bl OS_RestoreInterrupts
	ldr r0, _020DA9C8 ; =_021E36EC
	mov r3, r4
	str r6, [r0, #8]
	str r5, [r0, #0xc]
	ldr r1, [r7, #0x14]
	str r1, [r0, #0x14]
	ldr r1, [r7, #0x18]
	str r1, [r0, #0x18]
	ldmib r7, {r0, r1, r2}
	bl MicStartAutoSampling
	cmp r0, #0
	movne r0, #0
	moveq r0, #3
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.align 2, 0
_020DA9C8: .word _021E36EC
	arm_func_end MIC_StartAutoSamplingAsync

	arm_func_start MIC_StartAutoSampling
MIC_StartAutoSampling: ; 0x020DA9CC
	stmdb sp!, {r3, lr}
	ldr r1, _020DA9FC ; =MicGetResultCallback
	mov r2, #0
	bl MIC_StartAutoSamplingAsync
	ldr r1, _020DAA00 ; =_021E36EC
	cmp r0, #0
	str r0, [r1, #0x10]
	bne _020DA9F0
	bl MicWaitBusy
_020DA9F0:
	ldr r0, _020DAA00 ; =_021E36EC
	ldr r0, [r0, #0x10]
	ldmia sp!, {r3, pc}
	.align 2, 0
_020DA9FC: .word MicGetResultCallback
_020DAA00: .word _021E36EC
	arm_func_end MIC_StartAutoSampling

	arm_func_start MIC_StopAutoSamplingAsync
MIC_StopAutoSamplingAsync: ; 0x020DAA04
	stmdb sp!, {r3, r4, r5, lr}
	mov r5, r0
	mov r4, r1
	bl OS_DisableInterrupts
	ldr r1, _020DAA5C ; =_021E36EC
	ldr r2, [r1, #4]
	cmp r2, #0
	beq _020DAA30
	bl OS_RestoreInterrupts
	mov r0, #1
	ldmia sp!, {r3, r4, r5, pc}
_020DAA30:
	mov r2, #1
	str r2, [r1, #4]
	bl OS_RestoreInterrupts
	ldr r0, _020DAA5C ; =_021E36EC
	str r5, [r0, #8]
	str r4, [r0, #0xc]
	bl MicStopAutoSampling
	cmp r0, #0
	movne r0, #0
	moveq r0, #3
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_020DAA5C: .word _021E36EC
	arm_func_end MIC_StopAutoSamplingAsync

	arm_func_start MIC_StopAutoSampling
MIC_StopAutoSampling: ; 0x020DAA60
	stmdb sp!, {r3, lr}
	ldr r0, _020DAA90 ; =MicGetResultCallback
	mov r1, #0
	bl MIC_StopAutoSamplingAsync
	ldr r1, _020DAA94 ; =_021E36EC
	cmp r0, #0
	str r0, [r1, #0x10]
	bne _020DAA84
	bl MicWaitBusy
_020DAA84:
	ldr r0, _020DAA94 ; =_021E36EC
	ldr r0, [r0, #0x10]
	ldmia sp!, {r3, pc}
	.align 2, 0
_020DAA90: .word MicGetResultCallback
_020DAA94: .word _021E36EC
	arm_func_end MIC_StopAutoSampling

	arm_func_start MIC_GetLastSamplingAddress
MIC_GetLastSamplingAddress: ; 0x020DAA98
	ldr r0, _020DAAA4 ; =0x027FFF90
	ldr r0, [r0]
	bx lr
	.align 2, 0
_020DAAA4: .word 0x027FFF90
	arm_func_end MIC_GetLastSamplingAddress

	arm_func_start MicCommonCallback
MicCommonCallback: ; 0x020DAAA8
	stmdb sp!, {r4, lr}
	mov r4, r1
	cmp r2, #0
	beq _020DAAF0
	ldr r0, _020DABC8 ; =_021E36EC
	ldr r1, [r0, #4]
	cmp r1, #0
	movne r1, #0
	strne r1, [r0, #4]
	ldr r0, _020DABC8 ; =_021E36EC
	ldr r2, [r0, #8]
	cmp r2, #0
	beq _020DAAF0
	mov r1, #0
	str r1, [r0, #8]
	ldr r1, [r0, #0xc]
	mov r0, #6
	blx r2
_020DAAF0:
	and r0, r4, #0x7f00
	and r1, r4, #0xff
	mov r0, r0, lsl #8
	cmp r1, #4
	mov r2, r0, lsr #0x10
	addls pc, pc, r1, lsl #2
	b _020DAB48
_020DAB0C: ; jump table
	b _020DAB20 ; case 0
	b _020DAB28 ; case 1
	b _020DAB30 ; case 2
	b _020DAB38 ; case 3
	b _020DAB40 ; case 4
_020DAB20:
	mov r0, #0
	b _020DAB4C
_020DAB28:
	mov r0, #4
	b _020DAB4C
_020DAB30:
	mov r0, #2
	b _020DAB4C
_020DAB38:
	mov r0, #5
	b _020DAB4C
_020DAB40:
	mov r0, #1
	b _020DAB4C
_020DAB48:
	mov r0, #6
_020DAB4C:
	cmp r2, #0x51
	bne _020DAB70
	ldr r1, _020DABC8 ; =_021E36EC
	ldr r2, [r1, #0x14]
	cmp r2, #0
	ldmeqia sp!, {r4, pc}
	ldr r1, [r1, #0x18]
	blx r2
	ldmia sp!, {r4, pc}
_020DAB70:
	cmp r2, #0x40
	bne _020DAB90
	ldr r1, _020DABC8 ; =_021E36EC
	ldr r2, [r1, #0x1c]
	cmp r2, #0
	ldrne r1, _020DABCC ; =0x027FFF94
	ldrneh r1, [r1]
	strneh r1, [r2]
_020DAB90:
	ldr r1, _020DABC8 ; =_021E36EC
	ldr r2, [r1, #4]
	cmp r2, #0
	movne r2, #0
	strne r2, [r1, #4]
	ldr r1, _020DABC8 ; =_021E36EC
	ldr r3, [r1, #8]
	cmp r3, #0
	ldmeqia sp!, {r4, pc}
	mov r2, #0
	str r2, [r1, #8]
	ldr r1, [r1, #0xc]
	blx r3
	ldmia sp!, {r4, pc}
	.align 2, 0
_020DABC8: .word _021E36EC
_020DABCC: .word 0x027FFF94
	arm_func_end MicCommonCallback

	arm_func_start MicStartAutoSampling
MicStartAutoSampling: ; 0x020DABD0
	stmdb sp!, {r4, r5, r6, lr}
	mov r5, r1
	orr r1, r3, #0x4100
	mov r6, r0
	mov r4, r2
	orr r1, r1, #0x2000000
	mov r0, #9
	mov r2, #0
	bl PXI_SendWordByFifo
	cmp r0, #0
	movlt r0, #0
	ldmltia sp!, {r4, r5, r6, pc}
	mov r0, r6, lsr #0x10
	orr r1, r0, #0x10000
	mov r0, #9
	mov r2, #0
	bl PXI_SendWordByFifo
	cmp r0, #0
	movlt r0, #0
	ldmltia sp!, {r4, r5, r6, pc}
	mov r0, r6, lsl #0x10
	mov r0, r0, lsr #0x10
	orr r1, r0, #0x20000
	mov r0, #9
	mov r2, #0
	bl PXI_SendWordByFifo
	cmp r0, #0
	movlt r0, #0
	ldmltia sp!, {r4, r5, r6, pc}
	mov r0, r5, lsr #0x10
	orr r1, r0, #0x30000
	mov r0, #9
	mov r2, #0
	bl PXI_SendWordByFifo
	cmp r0, #0
	movlt r0, #0
	ldmltia sp!, {r4, r5, r6, pc}
	mov r0, r5, lsl #0x10
	mov r0, r0, lsr #0x10
	orr r1, r0, #0x40000
	mov r0, #9
	mov r2, #0
	bl PXI_SendWordByFifo
	cmp r0, #0
	movlt r0, #0
	ldmltia sp!, {r4, r5, r6, pc}
	mov r0, r4, lsr #0x10
	orr r1, r0, #0x50000
	mov r0, #9
	mov r2, #0
	bl PXI_SendWordByFifo
	cmp r0, #0
	movlt r0, #0
	ldmltia sp!, {r4, r5, r6, pc}
	ldr r1, _020DACD0 ; =0x01060000
	mov r0, r4, lsl #0x10
	orr r1, r1, r0, lsr #16
	mov r0, #9
	mov r2, #0
	bl PXI_SendWordByFifo
	cmp r0, #0
	movge r0, #1
	movlt r0, #0
	ldmia sp!, {r4, r5, r6, pc}
	.align 2, 0
_020DACD0: .word 0x01060000
	arm_func_end MicStartAutoSampling

	arm_func_start MicStopAutoSampling
MicStopAutoSampling: ; 0x020DACD4
	stmdb sp!, {r3, lr}
	ldr r1, _020DACF8 ; =0x03004200
	mov r0, #9
	mov r2, #0
	bl PXI_SendWordByFifo
	cmp r0, #0
	movge r0, #1
	movlt r0, #0
	ldmia sp!, {r3, pc}
	.align 2, 0
_020DACF8: .word 0x03004200
	arm_func_end MicStopAutoSampling

	arm_func_start MicGetResultCallback
MicGetResultCallback: ; 0x020DACFC
	ldr r1, _020DAD08 ; =_021E36EC
	str r0, [r1, #0x10]
	bx lr
	.align 2, 0
_020DAD08: .word _021E36EC
	arm_func_end MicGetResultCallback

	arm_func_start MicWaitBusy
MicWaitBusy: ; 0x020DAD0C
	ldr ip, _020DAD20 ; =_021E36F0
_020DAD10:
	ldr r0, [ip]
	cmp r0, #1
	beq _020DAD10
	bx lr
	.align 2, 0
_020DAD20: .word _021E36F0
	arm_func_end MicWaitBusy

	arm_func_start PMi_Lock
PMi_Lock: ; 0x020DAD24
	stmdb sp!, {r3, lr}
	bl OS_DisableInterrupts
	ldr r1, _020DAD5C ; =_021E370C
	ldr r2, [r1, #0x1c]
	cmp r2, #0
	beq _020DAD48
	bl OS_RestoreInterrupts
	mov r0, #0
	ldmia sp!, {r3, pc}
_020DAD48:
	mov r2, #1
	str r2, [r1, #0x1c]
	bl OS_RestoreInterrupts
	mov r0, #1
	ldmia sp!, {r3, pc}
	.align 2, 0
_020DAD5C: .word _021E370C
	arm_func_end PMi_Lock

	arm_func_start PMi_WaitBusy
PMi_WaitBusy: ; 0x020DAD60
	stmdb sp!, {r4, lr}
	ldr r0, _020DAD98 ; =_021E370C
	ldr r4, _020DAD9C ; =_021E3728
	ldr r0, [r0, #0x1c]
	cmp r0, #0
	ldmeqia sp!, {r4, pc}
_020DAD78:
	bl OS_GetCpsrIrq
	cmp r0, #0x80
	bne _020DAD88
	bl PXIi_HandlerRecvFifoNotEmpty
_020DAD88:
	ldr r0, [r4]
	cmp r0, #0
	bne _020DAD78
	ldmia sp!, {r4, pc}
	.align 2, 0
_020DAD98: .word _021E370C
_020DAD9C: .word _021E3728
	arm_func_end PMi_WaitBusy

	arm_func_start PMi_DummyCallback
PMi_DummyCallback: ; 0x020DADA0
	str r0, [r1]
	bx lr
	arm_func_end PMi_DummyCallback

	arm_func_start PMi_CallCallbackAndUnlock
PMi_CallCallbackAndUnlock: ; 0x020DADA8
	stmdb sp!, {r3, lr}
	ldr r2, _020DADE4 ; =_021E370C
	ldr r1, [r2, #0x1c]
	ldr ip, [r2, #0x20]
	cmp r1, #0
	ldr r1, [r2, #0x24]
	movne r3, #0
	strne r3, [r2, #0x1c]
	cmp ip, #0
	ldmeqia sp!, {r3, pc}
	ldr r2, _020DADE4 ; =_021E370C
	mov r3, #0
	str r3, [r2, #0x20]
	blx ip
	ldmia sp!, {r3, pc}
	.align 2, 0
_020DADE4: .word _021E370C
	arm_func_end PMi_CallCallbackAndUnlock

	arm_func_start PM_Init
PM_Init: ; 0x020DADE8
	stmdb sp!, {r3, r4, r5, lr}
	ldr r0, _020DAE78 ; =_021E370C
	ldrh r1, [r0]
	cmp r1, #0
	ldmneia sp!, {r3, r4, r5, pc}
	mov r1, #1
	strh r1, [r0]
	mov r1, #0
	str r1, [r0, #0x1c]
	str r1, [r0, #0x20]
	bl PXI_Init
	mov r5, #8
	mov r4, #1
_020DAE1C:
	mov r0, r5
	mov r1, r4
	bl PXI_IsCallbackReady
	cmp r0, #0
	beq _020DAE1C
	ldr r1, _020DAE7C ; =PMi_CommonCallback
	mov r0, #8
	bl PXI_SetFifoRecvCallback
	mov r3, #0
	ldr r0, _020DAE80 ; =_021E3750
	mov r2, r3
_020DAE48:
	mov r1, r3, lsl #3
	add r3, r3, #1
	strh r2, [r0, r1]
	cmp r3, #5
	blt _020DAE48
	ldr r0, _020DAE84 ; =_021E3738
	bl OS_InitMutex
	ldr r1, _020DAE88 ; =0x027FFC3C
	ldr r0, _020DAE78 ; =_021E370C
	ldr r1, [r1]
	str r1, [r0, #0x10]
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_020DAE78: .word _021E370C
_020DAE7C: .word PMi_CommonCallback
_020DAE80: .word _021E3750
_020DAE84: .word _021E3738
_020DAE88: .word 0x027FFC3C
	arm_func_end PM_Init

	arm_func_start PMi_CommonCallback
PMi_CommonCallback: ; 0x020DAE8C
	stmdb sp!, {r3, lr}
	cmp r2, #0
	beq _020DAEA4
	mov r0, #2
	bl PMi_CallCallbackAndUnlock
	ldmia sp!, {r3, pc}
_020DAEA4:
	and r0, r1, #0x7f00
	mov r0, r0, lsl #8
	mov r2, r0, lsr #0x10
	and r0, r1, #0xff
	cmp r2, #0x70
	blo _020DAEF4
	cmp r2, #0x74
	bhi _020DAEF4
	ldr r1, _020DAF48 ; =_021E3754
	sub r2, r2, #0x70
	ldr r1, [r1, r2, lsl #3]
	and r0, r0, #0xff
	cmp r1, #0
	strneh r0, [r1]
	mov r1, r2, lsl #3
	ldr r0, _020DAF4C ; =_021E3750
	mov r2, #1
	strh r2, [r0, r1]
	mov r0, #0
	b _020DAF40
_020DAEF4:
	cmp r2, #0x60
	bne _020DAF0C
	ldr r1, _020DAF50 ; =_021E370C
	mov r2, #1
	str r2, [r1, #4]
	b _020DAF40
_020DAF0C:
	cmp r2, #0x62
	bne _020DAF24
	ldr r1, _020DAF50 ; =_021E370C
	mov r2, #1
	str r2, [r1, #8]
	b _020DAF40
_020DAF24:
	cmp r2, #0x67
	bne _020DAF40
	ldr r1, _020DAF50 ; =_021E370C
	ldr r1, [r1, #0x28]
	cmp r1, #0
	strne r0, [r1]
	mov r0, #0
_020DAF40:
	bl PMi_CallCallbackAndUnlock
	ldmia sp!, {r3, pc}
	.align 2, 0
_020DAF48: .word _021E3754
_020DAF4C: .word _021E3750
_020DAF50: .word _021E370C
	arm_func_end PMi_CommonCallback

	arm_func_start PMi_SendSleepStart
PMi_SendSleepStart: ; 0x020DAF54
	stmdb sp!, {r3, r4, r5, lr}
	mov r5, r0
	mov r4, r1
	bl PMi_Lock
	cmp r0, #0
	moveq r0, #1
	ldmeqia sp!, {r3, r4, r5, pc}
	ldr r1, _020DAFDC ; =_021E370C
	mov r2, #0
	ldr r0, _020DAFE0 ; =0x03006000
	str r2, [r1, #4]
	bl PMi_SendPxiData
	ldr r0, _020DAFDC ; =_021E370C
_020DAF88:
	ldr r1, [r0, #4]
	cmp r1, #0
	beq _020DAF88
	ldr ip, _020DAFDC ; =_021E370C
	mov r0, #0
	str r0, [ip, #4]
	mov r2, r0
	mov r1, #2
	mov r3, #1
	str r0, [ip, #8]
	bl PMi_SetLCDPower
	and r0, r5, #0xff
	orr r0, r0, #0x6100
	orr r0, r0, #0x2000000
	bl PMi_SendPxiData
	ldr r1, _020DAFE4 ; =0x01010000
	mov r0, r4, lsl #0x10
	orr r0, r1, r0, lsr #16
	bl PMi_SendPxiData
	mov r0, #0
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_020DAFDC: .word _021E370C
_020DAFE0: .word 0x03006000
_020DAFE4: .word 0x01010000
	arm_func_end PMi_SendSleepStart

	arm_func_start PM_SendUtilityCommandAsync
PM_SendUtilityCommandAsync: ; 0x020DAFE8
	stmdb sp!, {r4, r5, r6, lr}
	mov r6, r0
	mov r5, r1
	mov r4, r2
	bl PMi_Lock
	cmp r0, #0
	moveq r0, #1
	ldmeqia sp!, {r4, r5, r6, pc}
	mov r0, r6, lsr #0x10
	and r0, r0, #0xff
	ldr r1, _020DB040 ; =_021E370C
	orr r0, r0, #0x6300
	str r5, [r1, #0x20]
	orr r0, r0, #0x2000000
	str r4, [r1, #0x24]
	bl PMi_SendPxiData
	ldr r1, _020DB044 ; =0x01010000
	mov r0, r6, lsl #0x10
	orr r0, r1, r0, lsr #16
	bl PMi_SendPxiData
	mov r0, #0
	ldmia sp!, {r4, r5, r6, pc}
	.align 2, 0
_020DB040: .word _021E370C
_020DB044: .word 0x01010000
	arm_func_end PM_SendUtilityCommandAsync

	arm_func_start PMi_ReadRegisterAsync
PMi_ReadRegisterAsync: ; 0x020DB048
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	mov r5, r0
	mov r4, r1
	mov r7, r2
	mov r6, r3
	bl PMi_Lock
	cmp r0, #0
	moveq r0, #1
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	ldr r1, _020DB0A8 ; =_021E370C
	and r0, r5, #0xff
	str r7, [r1, #0x20]
	orr r0, r0, #0x6500
	ldr r2, _020DB0AC ; =_021E3750
	str r6, [r1, #0x24]
	mov r3, r5, lsl #3
	mov ip, #0
	ldr r1, _020DB0B0 ; =_021E3754
	strh ip, [r2, r3]
	orr r0, r0, #0x3000000
	str r4, [r1, r5, lsl #3]
	bl PMi_SendPxiData
	mov r0, #0
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.align 2, 0
_020DB0A8: .word _021E370C
_020DB0AC: .word _021E3750
_020DB0B0: .word _021E3754
	arm_func_end PMi_ReadRegisterAsync

	arm_func_start PMi_ReadRegister
PMi_ReadRegister: ; 0x020DB0B4
	stmdb sp!, {r3, lr}
	ldr r2, _020DB0D8 ; =PMi_DummyCallback
	add r3, sp, #0
	bl PMi_ReadRegisterAsync
	cmp r0, #0
	ldmneia sp!, {r3, pc}
	bl PMi_WaitBusy
	ldr r0, [sp]
	ldmia sp!, {r3, pc}
	.align 2, 0
_020DB0D8: .word PMi_DummyCallback
	arm_func_end PMi_ReadRegister

	arm_func_start PMi_WriteRegisterAsync
PMi_WriteRegisterAsync: ; 0x020DB0DC
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	mov r7, r0
	mov r6, r1
	mov r5, r2
	mov r4, r3
	bl PMi_Lock
	cmp r0, #0
	moveq r0, #1
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	and r0, r7, #0xff
	ldr r1, _020DB134 ; =_021E370C
	orr r0, r0, #0x6400
	str r5, [r1, #0x20]
	orr r0, r0, #0x2000000
	str r4, [r1, #0x24]
	bl PMi_SendPxiData
	ldr r1, _020DB138 ; =0x01010000
	mov r0, r6, lsl #0x10
	orr r0, r1, r0, lsr #16
	bl PMi_SendPxiData
	mov r0, #0
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.align 2, 0
_020DB134: .word _021E370C
_020DB138: .word 0x01010000
	arm_func_end PMi_WriteRegisterAsync

	arm_func_start PMi_WriteRegister
PMi_WriteRegister: ; 0x020DB13C
	stmdb sp!, {r3, lr}
	ldr r2, _020DB160 ; =PMi_DummyCallback
	add r3, sp, #0
	bl PMi_WriteRegisterAsync
	cmp r0, #0
	ldmneia sp!, {r3, pc}
	bl PMi_WaitBusy
	ldr r0, [sp]
	ldmia sp!, {r3, pc}
	.align 2, 0
_020DB160: .word PMi_DummyCallback
	arm_func_end PMi_WriteRegister

	arm_func_start PMi_SetLEDAsync
PMi_SetLEDAsync: ; 0x020DB164
	stmdb sp!, {r3, lr}
	cmp r0, #1
	beq _020DB184
	cmp r0, #2
	beq _020DB194
	cmp r0, #3
	beq _020DB18C
	b _020DB19C
_020DB184:
	mov r0, #1
	b _020DB1A0
_020DB18C:
	mov r0, #2
	b _020DB1A0
_020DB194:
	mov r0, #3
	b _020DB1A0
_020DB19C:
	mov r0, #0
_020DB1A0:
	cmp r0, #0
	ldreq r0, _020DB1B4 ; =0x0000FFFF
	ldmeqia sp!, {r3, pc}
	bl PM_SendUtilityCommandAsync
	ldmia sp!, {r3, pc}
	.align 2, 0
_020DB1B4: .word 0x0000FFFF
	arm_func_end PMi_SetLEDAsync

	arm_func_start PMi_SetLED
PMi_SetLED: ; 0x020DB1B8
	stmdb sp!, {r3, lr}
	ldr r1, _020DB1DC ; =PMi_DummyCallback
	add r2, sp, #0
	bl PMi_SetLEDAsync
	cmp r0, #0
	ldmneia sp!, {r3, pc}
	bl PMi_WaitBusy
	ldr r0, [sp]
	ldmia sp!, {r3, pc}
	.align 2, 0
_020DB1DC: .word PMi_DummyCallback
	arm_func_end PMi_SetLED

	arm_func_start PM_SetBackLightAsync
PM_SetBackLightAsync: ; 0x020DB1E0
	stmdb sp!, {r3, lr}
	cmp r0, #0
	mov ip, #0
	bne _020DB204
	cmp r1, #1
	moveq ip, #6
	cmp r1, #0
	moveq ip, #7
	b _020DB238
_020DB204:
	cmp r0, #1
	bne _020DB220
	cmp r1, #1
	moveq ip, #4
	cmp r1, #0
	moveq ip, #5
	b _020DB238
_020DB220:
	cmp r0, #2
	bne _020DB238
	cmp r1, #1
	moveq ip, #8
	cmp r1, #0
	moveq ip, #9
_020DB238:
	cmp ip, #0
	ldreq r0, _020DB258 ; =0x0000FFFF
	ldmeqia sp!, {r3, pc}
	mov r1, r2
	mov r0, ip
	mov r2, r3
	bl PM_SendUtilityCommandAsync
	ldmia sp!, {r3, pc}
	.align 2, 0
_020DB258: .word 0x0000FFFF
	arm_func_end PM_SetBackLightAsync

	arm_func_start PM_SetBackLight
PM_SetBackLight: ; 0x020DB25C
	stmdb sp!, {r3, lr}
	ldr r2, _020DB280 ; =PMi_DummyCallback
	add r3, sp, #0
	bl PM_SetBackLightAsync
	cmp r0, #0
	ldmneia sp!, {r3, pc}
	bl PMi_WaitBusy
	ldr r0, [sp]
	ldmia sp!, {r3, pc}
	.align 2, 0
_020DB280: .word PMi_DummyCallback
	arm_func_end PM_SetBackLight

	arm_func_start PM_ForceToPowerOffAsync
PM_ForceToPowerOffAsync: ; 0x020DB284
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	sub sp, sp, #8
	mov r7, r0
	ldr r0, _020DB32C ; =0x00996A00
	mov r6, r1
	bl OS_SpinWait
	bl PM_GetLCDPower
	cmp r0, #1
	beq _020DB314
	add r0, sp, #4
	add r1, sp, #0
	bl PM_GetBackLight
	ldr r0, [sp, #4]
	cmp r0, #0
	beq _020DB2CC
	mov r0, #0
	mov r1, r0
	bl PM_SetBackLight
_020DB2CC:
	ldr r0, [sp]
	cmp r0, #0
	beq _020DB2E4
	mov r0, #1
	mov r1, #0
	bl PM_SetBackLight
_020DB2E4:
	mov r0, #1
	bl PM_SetLCDPower
	cmp r0, #0
	bne _020DB314
	ldr r5, _020DB32C ; =0x00996A00
	mov r4, #1
_020DB2FC:
	mov r0, r5
	bl OS_SpinWait
	mov r0, r4
	bl PM_SetLCDPower
	cmp r0, #0
	beq _020DB2FC
_020DB314:
	mov r1, r7
	mov r2, r6
	mov r0, #0xe
	bl PM_SendUtilityCommandAsync
	add sp, sp, #8
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.align 2, 0
_020DB32C: .word 0x00996A00
	arm_func_end PM_ForceToPowerOffAsync

	arm_func_start PM_ForceToPowerOff
PM_ForceToPowerOff: ; 0x020DB330
	stmdb sp!, {r3, lr}
	ldr r0, _020DB354 ; =PMi_DummyCallback
	add r1, sp, #0
	bl PM_ForceToPowerOffAsync
	cmp r0, #0
	ldmneia sp!, {r3, pc}
	bl PMi_WaitBusy
	ldr r0, [sp]
	ldmia sp!, {r3, pc}
	.align 2, 0
_020DB354: .word PMi_DummyCallback
	arm_func_end PM_ForceToPowerOff

	arm_func_start PM_SetAmp
PM_SetAmp: ; 0x020DB358
	ldr r1, _020DB368 ; =_021E370C
	ldr ip, _020DB36C ; =PMi_SetAmp
	str r0, [r1, #0x14]
	bx ip
	.align 2, 0
_020DB368: .word _021E370C
_020DB36C: .word PMi_SetAmp
	arm_func_end PM_SetAmp

	arm_func_start PMi_SetAmp
PMi_SetAmp: ; 0x020DB370
	stmdb sp!, {r4, lr}
	mov r4, r0
	bl PM_GetLCDPower
	cmp r0, #0
	moveq r0, #0
	ldmeqia sp!, {r4, pc}
	mov r0, r4, lsl #0x10
	mov r1, r0, lsr #0x10
	mov r0, #2
	bl PMi_WriteRegister
	ldmia sp!, {r4, pc}
	arm_func_end PMi_SetAmp

	arm_func_start PM_SetAmpGain
PM_SetAmpGain: ; 0x020DB39C
	ldr ip, _020DB3B0 ; =PMi_WriteRegister
	mov r0, r0, lsl #0x10
	mov r1, r0, lsr #0x10
	mov r0, #3
	bx ip
	.align 2, 0
_020DB3B0: .word PMi_WriteRegister
	arm_func_end PM_SetAmpGain

	arm_func_start PM_GetBackLight
PM_GetBackLight: ; 0x020DB3B4
	stmdb sp!, {r3, r4, r5, lr}
	mov r4, r1
	mov r5, r0
	add r1, sp, #0
	mov r0, #0
	bl PMi_ReadRegister
	cmp r0, #0
	ldmneia sp!, {r3, r4, r5, pc}
	cmp r5, #0
	beq _020DB3F0
	ldrh r1, [sp]
	tst r1, #8
	movne r1, #1
	moveq r1, #0
	str r1, [r5]
_020DB3F0:
	cmp r4, #0
	ldmeqia sp!, {r3, r4, r5, pc}
	ldrh r1, [sp]
	tst r1, #4
	movne r1, #1
	moveq r1, #0
	str r1, [r4]
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end PM_GetBackLight

	arm_func_start PMi_SendPxiData
PMi_SendPxiData: ; 0x020DB410
	stmdb sp!, {r4, r5, r6, lr}
	mov r6, r0
	mov r5, #8
	mov r4, #0
_020DB420:
	mov r0, r5
	mov r1, r6
	mov r2, r4
	bl PXI_SendWordByFifo
	cmp r0, #0
	bne _020DB420
	ldmia sp!, {r4, r5, r6, pc}
	arm_func_end PMi_SendPxiData

	arm_func_start PM_GoSleepMode
PM_GoSleepMode: ; 0x020DB43C
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x18
	ldr r3, _020DB660 ; =_021E370C
	mov sl, r0
	ldr r0, [r3, #0xc]
	mov sb, r1
	mov fp, r2
	mov r4, #0
	bl PMi_ExecuteList
	ldr r1, _020DB664 ; =0x04000208
	mov r0, r4
	ldrh r8, [r1]
	strh r0, [r1]
	bl OS_DisableInterrupts
	str r0, [sp, #8]
	ldr r0, _020DB668 ; =0x003FFFFF
	bl OS_DisableIrqMask
	str r0, [sp, #4]
	bl OS_IsTickAvailable
	cmp r0, #0
	movne r0, #8
	moveq r0, r4
	orr r0, r0, #0x40000
	bl OS_SetIrqMask
	ldr r0, [sp, #8]
	bl OS_RestoreInterrupts
	ldr r2, _020DB664 ; =0x04000208
	mov r0, #1
	ldrh r1, [r2]
	tst sl, #8
	strh r0, [r2]
	beq _020DB4CC
	ldr r0, _020DB66C ; =0x027FFC40
	ldrh r0, [r0]
	cmp r0, #2
	biceq sl, sl, #8
_020DB4CC:
	tst sl, #0x10
	beq _020DB4E0
	bl CTRDG_IsExisting
	cmp r0, #0
	biceq sl, sl, #0x10
_020DB4E0:
	mov r1, #0x4000000
	add r0, r1, #0x1000
	ldr r5, [r1]
	ldr r6, [r0]
	bl PM_GetLCDPower
	str r0, [sp]
	add r0, sp, #0x14
	add r1, sp, #0x10
	bl PM_GetBackLight
	mov r0, #2
	mov r1, #0
	bl PM_SetBackLight
	ldr r2, _020DB670 ; =0x027FFC3C
	ldr r0, [r2]
	str r0, [sp, #0xc]
_020DB51C:
	ldr r1, [r2]
	ldr r0, [sp, #0xc]
	cmp r0, r1
	beq _020DB51C
	ldr r0, [r2]
	mov r2, #0x4000000
	str r0, [sp, #0xc]
	ldr r0, [r2]
	add r1, r2, #0x1000
	bic r0, r0, #0x30000
	str r0, [r2]
	ldr r0, [r1]
	bic r0, r0, #0x10000
	str r0, [r1]
	ldr r2, _020DB670 ; =0x027FFC3C
_020DB558:
	ldr r1, [r2]
	ldr r0, [sp, #0xc]
	cmp r0, r1
	beq _020DB558
	ldr r0, [r2]
	str r0, [sp, #0xc]
	ldr r2, _020DB670 ; =0x027FFC3C
_020DB574:
	ldr r1, [r2]
	ldr r0, [sp, #0xc]
	cmp r0, r1
	beq _020DB574
	ldr r0, [sp, #0x14]
	ldr r1, [sp, #0x10]
	orr r0, sl, r0, lsl #5
	orr r0, r0, r1, lsl #6
	mov r0, r0, lsl #0x10
	mov r7, r0, lsr #0x10
	orr r0, sb, fp
	mov r0, r0, lsl #0x10
	mov sb, r0, lsr #0x10
_020DB5A8:
	mov r0, r7
	mov r1, sb
	bl PMi_SendSleepStart
	cmp r0, #0
	bne _020DB5A8
	bl OS_Halt
	tst sl, #8
	beq _020DB5D8
	ldr r0, _020DB674 ; =0x04000214
	ldr r0, [r0]
	tst r0, #0x100000
	movne r4, #1
_020DB5D8:
	cmp r4, #0
	bne _020DB618
	ldr r0, [sp]
	cmp r0, #1
	mov r0, #1
	bne _020DB604
	mov r1, r0
	mov r2, r0
	mov r3, r0
	bl PMi_SetLCDPower
	b _020DB608
_020DB604:
	bl PMi_SetLED
_020DB608:
	mov r0, #0x4000000
	str r5, [r0]
	add r0, r0, #0x1000
	str r6, [r0]
_020DB618:
	ldr r0, _020DB678 ; =0x00708100
	bl OS_SpinWait
	bl OS_DisableInterrupts
	ldr r0, [sp, #4]
	bl OS_SetIrqMask
	ldr r0, [sp, #8]
	bl OS_RestoreInterrupts
	ldr r1, _020DB664 ; =0x04000208
	cmp r4, #0
	ldrh r0, [r1]
	strh r8, [r1]
	beq _020DB64C
	bl PM_ForceToPowerOff
_020DB64C:
	ldr r0, _020DB660 ; =_021E370C
	ldr r0, [r0, #0x18]
	bl PMi_ExecuteList
	add sp, sp, #0x18
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_020DB660: .word _021E370C
_020DB664: .word 0x04000208
_020DB668: .word 0x003FFFFF
_020DB66C: .word 0x027FFC40
_020DB670: .word 0x027FFC3C
_020DB674: .word 0x04000214
_020DB678: .word 0x00708100
	arm_func_end PM_GoSleepMode

	arm_func_start PMi_SetLCDPower
PMi_SetLCDPower: ; 0x020DB67C
	stmdb sp!, {r3, r4, r5, lr}
	mov r5, r1
	mov r4, r3
	cmp r0, #0
	beq _020DB70C
	cmp r0, #1
	bne _020DB760
	cmp r2, #0
	bne _020DB6C0
	ldr r1, _020DB768 ; =0x027FFC3C
	ldr r0, _020DB76C ; =_021E370C
	ldr r1, [r1]
	ldr r0, [r0, #0x10]
	sub r0, r1, r0
	cmp r0, #7
	movls r0, #0
	ldmlsia sp!, {r3, r4, r5, pc}
_020DB6C0:
	cmp r5, #0
	beq _020DB6EC
	cmp r4, #0
	beq _020DB6DC
	mov r0, r5
	bl PMi_SetLED
	b _020DB6EC
_020DB6DC:
	mov r1, #0
	mov r0, r5
	mov r2, r1
	bl PMi_SetLEDAsync
_020DB6EC:
	ldr r2, _020DB770 ; =0x04000304
	ldr r0, _020DB76C ; =_021E370C
	ldrh r1, [r2]
	orr r1, r1, #1
	strh r1, [r2]
	ldr r0, [r0, #0x14]
	bl PMi_SetAmp
	b _020DB760
_020DB70C:
	mov r0, #0
	bl PMi_SetAmp
	ldr r3, _020DB770 ; =0x04000304
	ldr r1, _020DB768 ; =0x027FFC3C
	ldrh r2, [r3]
	ldr r0, _020DB76C ; =_021E370C
	cmp r5, #0
	bic r2, r2, #1
	strh r2, [r3]
	ldr r1, [r1]
	str r1, [r0, #0x10]
	beq _020DB760
	cmp r4, #0
	beq _020DB750
	mov r0, r5
	bl PMi_SetLED
	b _020DB760
_020DB750:
	mov r1, #0
	mov r0, r5
	mov r2, r1
	bl PMi_SetLEDAsync
_020DB760:
	mov r0, #1
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_020DB768: .word 0x027FFC3C
_020DB76C: .word _021E370C
_020DB770: .word 0x04000304
	arm_func_end PMi_SetLCDPower

	arm_func_start PM_SetLCDPower
PM_SetLCDPower: ; 0x020DB774
	ldr ip, _020DB790 ; =PMi_SetLCDPower
	mov r1, #0
	cmp r0, #1
	movne r0, #0
	mov r2, r1
	mov r3, #1
	bx ip
	.align 2, 0
_020DB790: .word PMi_SetLCDPower
	arm_func_end PM_SetLCDPower

	arm_func_start PM_GetLCDPower
PM_GetLCDPower: ; 0x020DB794
	ldr r0, _020DB7AC ; =0x04000304
	ldrh r0, [r0]
	tst r0, #1
	movne r0, #1
	moveq r0, #0
	bx lr
	.align 2, 0
_020DB7AC: .word 0x04000304
	arm_func_end PM_GetLCDPower

	arm_func_start PMi_SendLEDPatternCommandAsync
PMi_SendLEDPatternCommandAsync: ; 0x020DB7B0
	stmdb sp!, {r4, r5, r6, lr}
	mov r6, r0
	mov r5, r1
	mov r4, r2
	bl PMi_Lock
	cmp r0, #0
	moveq r0, #1
	ldmeqia sp!, {r4, r5, r6, pc}
	and r0, r6, #0xff
	ldr r1, _020DB7F4 ; =_021E370C
	orr r0, r0, #0x6600
	str r5, [r1, #0x20]
	orr r0, r0, #0x3000000
	str r4, [r1, #0x24]
	bl PMi_SendPxiData
	mov r0, #0
	ldmia sp!, {r4, r5, r6, pc}
	.align 2, 0
_020DB7F4: .word _021E370C
	arm_func_end PMi_SendLEDPatternCommandAsync

	arm_func_start PMi_SendLEDPatternCommand
PMi_SendLEDPatternCommand: ; 0x020DB7F8
	stmdb sp!, {r3, lr}
	ldr r1, _020DB81C ; =PMi_DummyCallback
	add r2, sp, #0
	bl PMi_SendLEDPatternCommandAsync
	cmp r0, #0
	ldmneia sp!, {r3, pc}
	bl PMi_WaitBusy
	ldr r0, [sp]
	ldmia sp!, {r3, pc}
	.align 2, 0
_020DB81C: .word PMi_DummyCallback
	arm_func_end PMi_SendLEDPatternCommand

	arm_func_start PM_GetLEDPatternAsync
PM_GetLEDPatternAsync: ; 0x020DB820
	stmdb sp!, {r4, r5, r6, lr}
	mov r6, r0
	mov r5, r1
	mov r4, r2
	bl PMi_Lock
	cmp r0, #0
	moveq r0, #1
	ldmeqia sp!, {r4, r5, r6, pc}
	ldr r1, _020DB860 ; =_021E370C
	ldr r0, _020DB864 ; =0x03006700
	str r5, [r1, #0x20]
	str r4, [r1, #0x24]
	str r6, [r1, #0x28]
	bl PMi_SendPxiData
	mov r0, #0
	ldmia sp!, {r4, r5, r6, pc}
	.align 2, 0
_020DB860: .word _021E370C
_020DB864: .word 0x03006700
	arm_func_end PM_GetLEDPatternAsync

	arm_func_start PM_GetLEDPattern
PM_GetLEDPattern: ; 0x020DB868
	stmdb sp!, {r3, lr}
	ldr r1, _020DB88C ; =PMi_DummyCallback
	add r2, sp, #0
	bl PM_GetLEDPatternAsync
	cmp r0, #0
	ldmneia sp!, {r3, pc}
	bl PMi_WaitBusy
	ldr r0, [sp]
	ldmia sp!, {r3, pc}
	.align 2, 0
_020DB88C: .word PMi_DummyCallback
	arm_func_end PM_GetLEDPattern

	arm_func_start PMi_PrependList
PMi_PrependList: ; 0x020DB890
	cmp r0, #0
	ldrne r2, [r0]
	strne r2, [r1, #8]
	strne r1, [r0]
	bx lr
	arm_func_end PMi_PrependList

	arm_func_start PMi_AppendList
PMi_AppendList: ; 0x020DB8A4
	cmp r0, #0
	bxeq lr
	ldr r2, [r0]
	cmp r2, #0
	bne _020DB8C8
	mov r2, #0
	str r2, [r1, #8]
	str r1, [r0]
	bx lr
_020DB8C8:
	ldr r0, [r2, #8]
	cmp r0, #0
	beq _020DB8E4
_020DB8D4:
	mov r2, r0
	ldr r0, [r0, #8]
	cmp r0, #0
	bne _020DB8D4
_020DB8E4:
	str r0, [r1, #8]
	str r1, [r2, #8]
	bx lr
	arm_func_end PMi_AppendList

	arm_func_start PMi_DeleteList
PMi_DeleteList: ; 0x020DB8F0
	cmp r0, #0
	ldrne r2, [r0]
	movne r3, r2
	cmpne r2, #0
	bxeq lr
_020DB904:
	cmp r2, r1
	bne _020DB924
	cmp r2, r3
	ldreq r1, [r2, #8]
	streq r1, [r0]
	ldrne r0, [r2, #8]
	strne r0, [r3, #8]
	bx lr
_020DB924:
	mov r3, r2
	ldr r2, [r2, #8]
	cmp r2, #0
	bne _020DB904
	bx lr
	arm_func_end PMi_DeleteList

	arm_func_start PMi_ExecuteList
PMi_ExecuteList: ; 0x020DB938
	stmdb sp!, {r4, lr}
	movs r4, r0
	ldmeqia sp!, {r4, pc}
_020DB944:
	ldr r0, [r4, #4]
	ldr r1, [r4]
	blx r1
	ldr r4, [r4, #8]
	cmp r4, #0
	bne _020DB944
	ldmia sp!, {r4, pc}
	arm_func_end PMi_ExecuteList

	arm_func_start PM_PrependPreSleepCallback
PM_PrependPreSleepCallback: ; 0x020DB960
	ldr ip, _020DB970 ; =PMi_PrependList
	mov r1, r0
	ldr r0, _020DB974 ; =_021E3718
	bx ip
	.align 2, 0
_020DB970: .word PMi_PrependList
_020DB974: .word _021E3718
	arm_func_end PM_PrependPreSleepCallback

	arm_func_start PM_AppendPostSleepCallback
PM_AppendPostSleepCallback: ; 0x020DB978
	ldr ip, _020DB988 ; =PMi_AppendList
	mov r1, r0
	ldr r0, _020DB98C ; =_021E3724
	bx ip
	.align 2, 0
_020DB988: .word PMi_AppendList
_020DB98C: .word _021E3724
	arm_func_end PM_AppendPostSleepCallback

	arm_func_start PM_DeletePreSleepCallback
PM_DeletePreSleepCallback: ; 0x020DB990
	ldr ip, _020DB9A0 ; =PMi_DeleteList
	mov r1, r0
	ldr r0, _020DB9A4 ; =_021E3718
	bx ip
	.align 2, 0
_020DB9A0: .word PMi_DeleteList
_020DB9A4: .word _021E3718
	arm_func_end PM_DeletePreSleepCallback

	arm_func_start PM_DeletePostSleepCallback
PM_DeletePostSleepCallback: ; 0x020DB9A8
	ldr ip, _020DB9B8 ; =PMi_DeleteList
	mov r1, r0
	ldr r0, _020DB9BC ; =_021E3724
	bx ip
	.align 2, 0
_020DB9B8: .word PMi_DeleteList
_020DB9BC: .word _021E3724
	arm_func_end PM_DeletePostSleepCallback

	arm_func_start RTC_Init
RTC_Init: ; 0x020DB9C0
	stmdb sp!, {r3, r4, r5, lr}
	ldr r0, _020DBA24 ; =_021E3778
	ldrh r1, [r0]
	cmp r1, #0
	ldmneia sp!, {r3, r4, r5, pc}
	mov r1, #1
	strh r1, [r0]
	mov r1, #0
	str r1, [r0, #0xc]
	str r1, [r0, #0x10]
	str r1, [r0, #0x28]
	str r1, [r0, #0x14]
	str r1, [r0, #0x18]
	bl PXI_Init
	mov r5, #5
	mov r4, #1
_020DBA00:
	mov r0, r5
	mov r1, r4
	bl PXI_IsCallbackReady
	cmp r0, #0
	beq _020DBA00
	ldr r1, _020DBA28 ; =RtcCommonCallback
	mov r0, #5
	bl PXI_SetFifoRecvCallback
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_020DBA24: .word _021E3778
_020DBA28: .word RtcCommonCallback
	arm_func_end RTC_Init

	arm_func_start RTC_GetDateAsync
RTC_GetDateAsync: ; 0x020DBA2C
	stmdb sp!, {r4, r5, r6, lr}
	mov r6, r0
	mov r5, r1
	mov r4, r2
	bl OS_DisableInterrupts
	ldr r1, _020DBA98 ; =_021E3778
	ldr r2, [r1, #0xc]
	cmp r2, #0
	beq _020DBA5C
	bl OS_RestoreInterrupts
	mov r0, #1
	ldmia sp!, {r4, r5, r6, pc}
_020DBA5C:
	mov r2, #1
	str r2, [r1, #0xc]
	bl OS_RestoreInterrupts
	ldr r0, _020DBA98 ; =_021E3778
	mov r1, #0
	str r1, [r0, #0x20]
	str r1, [r0, #0x24]
	str r6, [r0, #0x14]
	str r5, [r0, #0x10]
	str r4, [r0, #0x1c]
	bl RTCi_ReadRawDateAsync
	cmp r0, #0
	movne r0, #0
	moveq r0, #3
	ldmia sp!, {r4, r5, r6, pc}
	.align 2, 0
_020DBA98: .word _021E3778
	arm_func_end RTC_GetDateAsync

	arm_func_start RTC_GetDate
RTC_GetDate: ; 0x020DBA9C
	stmdb sp!, {r3, lr}
	ldr r1, _020DBACC ; =RtcGetResultCallback
	mov r2, #0
	bl RTC_GetDateAsync
	ldr r1, _020DBAD0 ; =_021E3778
	cmp r0, #0
	str r0, [r1, #0x2c]
	bne _020DBAC0
	bl RtcWaitBusy
_020DBAC0:
	ldr r0, _020DBAD0 ; =_021E3778
	ldr r0, [r0, #0x2c]
	ldmia sp!, {r3, pc}
	.align 2, 0
_020DBACC: .word RtcGetResultCallback
_020DBAD0: .word _021E3778
	arm_func_end RTC_GetDate

	arm_func_start RTC_GetTimeAsync
RTC_GetTimeAsync: ; 0x020DBAD4
	stmdb sp!, {r4, r5, r6, lr}
	mov r6, r0
	mov r5, r1
	mov r4, r2
	bl OS_DisableInterrupts
	ldr r1, _020DBB44 ; =_021E3778
	ldr r2, [r1, #0xc]
	cmp r2, #0
	beq _020DBB04
	bl OS_RestoreInterrupts
	mov r0, #1
	ldmia sp!, {r4, r5, r6, pc}
_020DBB04:
	mov r2, #1
	str r2, [r1, #0xc]
	bl OS_RestoreInterrupts
	ldr r0, _020DBB44 ; =_021E3778
	mov r1, #1
	str r1, [r0, #0x20]
	mov r1, #0
	str r1, [r0, #0x24]
	str r6, [r0, #0x14]
	str r5, [r0, #0x10]
	str r4, [r0, #0x1c]
	bl RTCi_ReadRawTimeAsync
	cmp r0, #0
	movne r0, #0
	moveq r0, #3
	ldmia sp!, {r4, r5, r6, pc}
	.align 2, 0
_020DBB44: .word _021E3778
	arm_func_end RTC_GetTimeAsync

	arm_func_start RTC_GetTime
RTC_GetTime: ; 0x020DBB48
	stmdb sp!, {r3, lr}
	ldr r1, _020DBB78 ; =RtcGetResultCallback
	mov r2, #0
	bl RTC_GetTimeAsync
	ldr r1, _020DBB7C ; =_021E3778
	cmp r0, #0
	str r0, [r1, #0x2c]
	bne _020DBB6C
	bl RtcWaitBusy
_020DBB6C:
	ldr r0, _020DBB7C ; =_021E3778
	ldr r0, [r0, #0x2c]
	ldmia sp!, {r3, pc}
	.align 2, 0
_020DBB78: .word RtcGetResultCallback
_020DBB7C: .word _021E3778
	arm_func_end RTC_GetTime

	arm_func_start RTC_GetDateTimeAsync
RTC_GetDateTimeAsync: ; 0x020DBB80
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	mov r7, r0
	mov r6, r1
	mov r5, r2
	mov r4, r3
	bl OS_DisableInterrupts
	ldr r1, _020DBBF8 ; =_021E3778
	ldr r2, [r1, #0xc]
	cmp r2, #0
	beq _020DBBB4
	bl OS_RestoreInterrupts
	mov r0, #1
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_020DBBB4:
	mov r2, #1
	str r2, [r1, #0xc]
	bl OS_RestoreInterrupts
	ldr r0, _020DBBF8 ; =_021E3778
	mov r1, #2
	str r1, [r0, #0x20]
	mov r1, #0
	str r1, [r0, #0x24]
	str r7, [r0, #0x14]
	str r6, [r0, #0x18]
	str r5, [r0, #0x10]
	str r4, [r0, #0x1c]
	bl RTCi_ReadRawDateTimeAsync
	cmp r0, #0
	movne r0, #0
	moveq r0, #3
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.align 2, 0
_020DBBF8: .word _021E3778
	arm_func_end RTC_GetDateTimeAsync

	arm_func_start RTC_GetDateTime
RTC_GetDateTime: ; 0x020DBBFC
	stmdb sp!, {r3, lr}
	ldr r2, _020DBC2C ; =RtcGetResultCallback
	mov r3, #0
	bl RTC_GetDateTimeAsync
	ldr r1, _020DBC30 ; =_021E3778
	cmp r0, #0
	str r0, [r1, #0x2c]
	bne _020DBC20
	bl RtcWaitBusy
_020DBC20:
	ldr r0, _020DBC30 ; =_021E3778
	ldr r0, [r0, #0x2c]
	ldmia sp!, {r3, pc}
	.align 2, 0
_020DBC2C: .word RtcGetResultCallback
_020DBC30: .word _021E3778
	arm_func_end RTC_GetDateTime

	arm_func_start RtcCommonCallback
RtcCommonCallback: ; 0x020DBC34
	stmdb sp!, {r3, r4, r5, lr}
	cmp r2, #0
	beq _020DBC90
	ldr r0, _020DC174 ; =_021E3778
	ldr r1, [r0, #0x24]
	cmp r1, #0
	movne r1, #0
	strne r1, [r0, #0x24]
	ldr r0, _020DC174 ; =_021E3778
	ldr r1, [r0, #0xc]
	cmp r1, #0
	movne r1, #0
	strne r1, [r0, #0xc]
	ldr r0, _020DC174 ; =_021E3778
	ldr r2, [r0, #0x10]
	cmp r2, #0
	ldmeqia sp!, {r3, r4, r5, pc}
	mov r1, #0
	str r1, [r0, #0x10]
	ldr r1, [r0, #0x1c]
	mov r0, #6
	blx r2
	ldmia sp!, {r3, r4, r5, pc}
_020DBC90:
	and r0, r1, #0x7f00
	mov r0, r0, lsr #8
	and r0, r0, #0xff
	cmp r0, #0x30
	and r2, r1, #0xff
	bne _020DBCC0
	ldr r0, _020DC174 ; =_021E3778
	ldr r0, [r0, #0x28]
	cmp r0, #0
	ldmeqia sp!, {r3, r4, r5, pc}
	blx r0
	ldmia sp!, {r3, r4, r5, pc}
_020DBCC0:
	cmp r2, #0
	bne _020DC0E4
	ldr r0, _020DC174 ; =_021E3778
	mov r4, #0
	ldr r1, [r0, #0x20]
	cmp r1, #0xf
	addls pc, pc, r1, lsl #2
	b _020DC0D0
_020DBCE0: ; jump table
	b _020DBD20 ; case 0
	b _020DBD7C ; case 1
	b _020DBDCC ; case 2
	b _020DC12C ; case 3
	b _020DC12C ; case 4
	b _020DC12C ; case 5
	b _020DBE74 ; case 6
	b _020DBEA4 ; case 7
	b _020DBEC8 ; case 8
	b _020DBF70 ; case 9
	b _020DC024 ; case 10
	b _020DC12C ; case 11
	b _020DC12C ; case 12
	b _020DC12C ; case 13
	b _020DC12C ; case 14
	b _020DC12C ; case 15
_020DBD20:
	ldr r1, _020DC178 ; =0x027FFDE8
	ldr r5, [r0, #0x14]
	ldr r0, [r1]
	mov r0, r0, lsl #0x18
	mov r0, r0, lsr #0x18
	bl RtcBCD2HEX
	ldr r1, _020DC178 ; =0x027FFDE8
	str r0, [r5]
	ldr r0, [r1]
	mov r0, r0, lsl #0x13
	mov r0, r0, lsr #0x1b
	bl RtcBCD2HEX
	ldr r1, _020DC178 ; =0x027FFDE8
	str r0, [r5, #4]
	ldr r0, [r1]
	mov r0, r0, lsl #0xa
	mov r0, r0, lsr #0x1a
	bl RtcBCD2HEX
	str r0, [r5, #8]
	mov r0, r5
	bl RTC_GetDayOfWeek
	str r0, [r5, #0xc]
	b _020DC12C
_020DBD7C:
	ldr r1, _020DC17C ; =0x027FFDEC
	ldr r5, [r0, #0x14]
	ldr r0, [r1]
	mov r0, r0, lsl #0x1a
	mov r0, r0, lsr #0x1a
	bl RtcBCD2HEX
	ldr r1, _020DC17C ; =0x027FFDEC
	str r0, [r5]
	ldr r0, [r1]
	mov r0, r0, lsl #0x11
	mov r0, r0, lsr #0x19
	bl RtcBCD2HEX
	ldr r1, _020DC17C ; =0x027FFDEC
	str r0, [r5, #4]
	ldr r0, [r1]
	mov r0, r0, lsl #9
	mov r0, r0, lsr #0x19
	bl RtcBCD2HEX
	str r0, [r5, #8]
	b _020DC12C
_020DBDCC:
	ldr r1, _020DC178 ; =0x027FFDE8
	ldr r5, [r0, #0x14]
	ldr r0, [r1]
	and r0, r0, #0xff
	bl RtcBCD2HEX
	ldr r1, _020DC178 ; =0x027FFDE8
	str r0, [r5]
	ldr r0, [r1]
	mov r0, r0, lsl #0x13
	mov r0, r0, lsr #0x1b
	bl RtcBCD2HEX
	ldr r1, _020DC178 ; =0x027FFDE8
	str r0, [r5, #4]
	ldr r0, [r1]
	mov r0, r0, lsl #0xa
	mov r0, r0, lsr #0x1a
	bl RtcBCD2HEX
	str r0, [r5, #8]
	mov r0, r5
	bl RTC_GetDayOfWeek
	ldr r1, _020DC17C ; =0x027FFDEC
	str r0, [r5, #0xc]
	ldr r0, [r1]
	ldr r1, _020DC174 ; =_021E3778
	mov r0, r0, lsl #0x1a
	mov r0, r0, lsr #0x1a
	ldr r5, [r1, #0x18]
	bl RtcBCD2HEX
	ldr r1, _020DC17C ; =0x027FFDEC
	str r0, [r5]
	ldr r0, [r1]
	mov r0, r0, lsl #0x11
	mov r0, r0, lsr #0x19
	bl RtcBCD2HEX
	ldr r1, _020DC17C ; =0x027FFDEC
	str r0, [r5, #4]
	ldr r0, [r1]
	mov r0, r0, lsl #9
	mov r0, r0, lsr #0x19
	bl RtcBCD2HEX
	str r0, [r5, #8]
	b _020DC12C
_020DBE74:
	ldr r1, _020DC180 ; =0x027FFDEA
	ldr r2, [r0, #0x14]
	ldrh r0, [r1]
	mov r0, r0, lsl #0x1c
	mov r0, r0, lsr #0x1c
	cmp r0, #4
	bne _020DBE9C
	mov r0, #1
	str r0, [r2]
	b _020DC12C
_020DBE9C:
	str r4, [r2]
	b _020DC12C
_020DBEA4:
	ldr r1, _020DC180 ; =0x027FFDEA
	ldr r2, [r0, #0x14]
	ldrh r0, [r1]
	mov r0, r0, lsl #0x19
	movs r0, r0, lsr #0x1f
	movne r0, #1
	strne r0, [r2]
	streq r4, [r2]
	b _020DC12C
_020DBEC8:
	ldr r1, _020DC17C ; =0x027FFDEC
	ldr r5, [r0, #0x14]
	ldr r0, [r1]
	mov r0, r0, lsl #0x1d
	mov r0, r0, lsr #0x1d
	str r0, [r5]
	ldr r0, [r1]
	mov r0, r0, lsl #0x12
	mov r0, r0, lsr #0x1a
	bl RtcBCD2HEX
	ldr r1, _020DC17C ; =0x027FFDEC
	str r0, [r5, #4]
	ldr r0, [r1]
	mov r0, r0, lsl #9
	mov r0, r0, lsr #0x19
	bl RtcBCD2HEX
	str r0, [r5, #8]
	mov r1, r4
	ldr r0, _020DC17C ; =0x027FFDEC
	str r1, [r5, #0xc]
	ldr r0, [r0]
	mov r0, r0, lsl #0x18
	movs r0, r0, lsr #0x1f
	ldrne r0, [r5, #0xc]
	addne r0, r0, #1
	strne r0, [r5, #0xc]
	ldr r0, _020DC17C ; =0x027FFDEC
	ldr r0, [r0]
	mov r0, r0, lsl #0x10
	movs r0, r0, lsr #0x1f
	ldrne r0, [r5, #0xc]
	addne r0, r0, #2
	strne r0, [r5, #0xc]
	ldr r0, _020DC17C ; =0x027FFDEC
	ldr r0, [r0]
	mov r0, r0, lsl #8
	movs r0, r0, lsr #0x1f
	beq _020DC12C
	ldr r0, [r5, #0xc]
	add r0, r0, #4
	str r0, [r5, #0xc]
	b _020DC12C
_020DBF70:
	ldr r3, [r0, #0x24]
	cmp r3, #0
	bne _020DC01C
	ldr r1, [r0, #0x14]
	ldr r2, _020DC180 ; =0x027FFDEA
	ldr r1, [r1]
	cmp r1, #1
	ldrh r1, [r2]
	bne _020DBFDC
	mov r1, r1, lsl #0x1c
	mov r1, r1, lsr #0x1c
	cmp r1, #4
	beq _020DC12C
	add r1, r3, #1
	str r1, [r0, #0x24]
	ldrh r0, [r2]
	bic r0, r0, #0xf
	orr r0, r0, #4
	strh r0, [r2]
	bl RTCi_WriteRawStatus2Async
	cmp r0, #0
	bne _020DC12C
	ldr r0, _020DC174 ; =_021E3778
	mov r1, r4
	str r1, [r0, #0x24]
	mov r4, #3
	b _020DC12C
_020DBFDC:
	mov r1, r1, lsl #0x1c
	movs r1, r1, lsr #0x1c
	beq _020DC12C
	add r1, r3, #1
	str r1, [r0, #0x24]
	ldrh r0, [r2]
	bic r0, r0, #0xf
	strh r0, [r2]
	bl RTCi_WriteRawStatus2Async
	cmp r0, #0
	bne _020DC12C
	ldr r0, _020DC174 ; =_021E3778
	mov r1, r4
	str r1, [r0, #0x24]
	mov r4, #3
	b _020DC12C
_020DC01C:
	str r4, [r0, #0x24]
	b _020DC12C
_020DC024:
	ldr r3, [r0, #0x24]
	cmp r3, #0
	bne _020DC0C8
	ldr r1, [r0, #0x14]
	ldr r2, _020DC180 ; =0x027FFDEA
	ldr r1, [r1]
	cmp r1, #1
	ldrh r1, [r2]
	bne _020DC088
	mov r1, r1, lsl #0x19
	movs r1, r1, lsr #0x1f
	bne _020DC12C
	add r1, r3, #1
	str r1, [r0, #0x24]
	ldrh r0, [r2]
	orr r0, r0, #0x40
	strh r0, [r2]
	bl RTCi_WriteRawStatus2Async
	cmp r0, #0
	bne _020DC12C
	ldr r0, _020DC174 ; =_021E3778
	mov r1, r4
	str r1, [r0, #0x24]
	mov r4, #3
	b _020DC12C
_020DC088:
	mov r1, r1, lsl #0x19
	movs r1, r1, lsr #0x1f
	beq _020DC12C
	add r1, r3, #1
	str r1, [r0, #0x24]
	ldrh r0, [r2]
	bic r0, r0, #0x40
	strh r0, [r2]
	bl RTCi_WriteRawStatus2Async
	cmp r0, #0
	bne _020DC12C
	ldr r0, _020DC174 ; =_021E3778
	mov r1, r4
	str r1, [r0, #0x24]
	mov r4, #3
	b _020DC12C
_020DC0C8:
	str r4, [r0, #0x24]
	b _020DC12C
_020DC0D0:
	ldr r0, _020DC174 ; =_021E3778
	mov r1, #0
	str r1, [r0, #0x24]
	mov r4, #4
	b _020DC12C
_020DC0E4:
	ldr r0, _020DC174 ; =_021E3778
	mov r1, #0
	str r1, [r0, #0x24]
	cmp r2, #4
	addls pc, pc, r2, lsl #2
	b _020DC128
_020DC0FC: ; jump table
	b _020DC128 ; case 0
	b _020DC110 ; case 1
	b _020DC118 ; case 2
	b _020DC120 ; case 3
	b _020DC128 ; case 4
_020DC110:
	mov r4, #4
	b _020DC12C
_020DC118:
	mov r4, #5
	b _020DC12C
_020DC120:
	mov r4, #1
	b _020DC12C
_020DC128:
	mov r4, #6
_020DC12C:
	ldr r0, _020DC174 ; =_021E3778
	ldr r1, [r0, #0x24]
	cmp r1, #0
	ldmneia sp!, {r3, r4, r5, pc}
	ldr r1, [r0, #0xc]
	cmp r1, #0
	movne r1, #0
	strne r1, [r0, #0xc]
	ldr r0, _020DC174 ; =_021E3778
	ldr r2, [r0, #0x10]
	cmp r2, #0
	ldmeqia sp!, {r3, r4, r5, pc}
	mov r1, #0
	str r1, [r0, #0x10]
	ldr r1, [r0, #0x1c]
	mov r0, r4
	blx r2
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_020DC174: .word _021E3778
_020DC178: .word 0x027FFDE8
_020DC17C: .word 0x027FFDEC
_020DC180: .word 0x027FFDEA
	arm_func_end RtcCommonCallback

	arm_func_start RtcBCD2HEX
RtcBCD2HEX: ; 0x020DC184
	stmdb sp!, {r3, r4, r5, lr}
	mov ip, #0
	mov r2, ip
	mov r3, ip
_020DC194:
	mov r1, r0, lsr r3
	and r1, r1, #0xf
	cmp r1, #0xa
	movhs r0, #0
	ldmhsia sp!, {r3, r4, r5, pc}
	add r2, r2, #1
	cmp r2, #8
	add r3, r3, #4
	blt _020DC194
	mov r5, #0
	mov r4, r5
	mov lr, #1
	mov r2, #0xa
_020DC1C8:
	mov r1, r0, lsr r4
	and r3, r1, #0xf
	mul r1, lr, r2
	mla ip, lr, r3, ip
	add r5, r5, #1
	mov lr, r1
	cmp r5, #8
	add r4, r4, #4
	blt _020DC1C8
	mov r0, ip
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end RtcBCD2HEX

	arm_func_start RtcGetResultCallback
RtcGetResultCallback: ; 0x020DC1F4
	ldr r1, _020DC200 ; =_021E3778
	str r0, [r1, #0x2c]
	bx lr
	.align 2, 0
_020DC200: .word _021E3778
	arm_func_end RtcGetResultCallback

	arm_func_start RtcWaitBusy
RtcWaitBusy: ; 0x020DC204
	ldr ip, _020DC218 ; =_021E3784
_020DC208:
	ldr r0, [ip]
	cmp r0, #1
	beq _020DC208
	bx lr
	.align 2, 0
_020DC218: .word _021E3784
	arm_func_end RtcWaitBusy

	.section .text.unified_26,4,1,6

	arm_func_start RTC_ConvertDateTimeToSecond
RTC_ConvertDateTimeToSecond: ; 0x020DC330
	stmdb sp!, {r3, r4, r5, lr}
	mov r5, r1
	bl RTC_ConvertDateToDay
	mov r4, r0
	mvn r0, #0
	cmp r4, r0
	moveq r1, r0
	ldmeqia sp!, {r3, r4, r5, pc}
	mov r0, r5
	bl RTCi_ConvertTimeToSecond
	mvn r2, #0
	cmp r0, r2
	moveq r1, r2
	beq _020DC388
	ldr r1, _020DC390 ; =0x00015180
	mov r2, #0
	umull ip, r3, r4, r1
	mla r3, r4, r2, r3
	mov r2, r4, asr #0x1f
	mla r3, r2, r1, r3
	adds r2, r0, ip
	adc r1, r3, r0, asr #31
_020DC388:
	mov r0, r2
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_020DC390: .word 0x00015180
	arm_func_end RTC_ConvertDateTimeToSecond

	.section .text.unified_29,4,1,6

	arm_func_start CARD_SetCacheFlushThreshold
CARD_SetCacheFlushThreshold: ; 0x020DC9DC
	ldr r2, _020DC9EC ; =cardi_common
	str r0, [r2, #0x118]
	str r1, [r2, #0x11c]
	bx lr
	.align 2, 0
_020DC9EC: .word cardi_common
	arm_func_end CARD_SetCacheFlushThreshold

	arm_func_start CARDi_IdentifyBackupCore
CARDi_IdentifyBackupCore: ; 0x020DC9F0
	stmdb sp!, {r3, r4, r5, lr}
	ldr r1, _020DCD54 ; =cardi_common
	mov r5, r0
	ldr r4, [r1]
	mov r1, #0
	add r0, r4, #0x18
	mov r2, #0x48
	bl MI_CpuFill8
	ldr r0, _020DCD58 ; =0x0000203F
	str r5, [r4, #4]
	str r0, [r4, #0x58]
	cmp r5, #0
	ldmeqia sp!, {r3, r4, r5, pc}
	mov r0, r5, asr #8
	and r0, r0, #0xff
	mov r3, #1
	mov ip, r3, lsl r0
	and r2, r5, #0xff
	mov r1, r5, asr #0x10
	str ip, [r4, #0x18]
	mov r0, #0xff
	strb r0, [r4, #0x54]
	cmp r2, #1
	and r0, r1, #0xff
	bne _020DCB2C
	cmp ip, #0x2000
	bhi _020DCA6C
	bhs _020DCAA8
	cmp ip, #0x200
	beq _020DCA88
	b _020DCD34
_020DCA6C:
	cmp ip, #0x10000
	bhi _020DCA7C
	beq _020DCACC
	b _020DCD34
_020DCA7C:
	cmp ip, #0x20000
	beq _020DCAF0
	b _020DCD34
_020DCA88:
	mov r0, #0x10
	str r0, [r4, #0x24]
	str r3, [r4, #0x28]
	mov r0, #5
	str r0, [r4, #0x2c]
	mov r0, #0xf0
	strb r0, [r4, #0x54]
	b _020DCB10
_020DCAA8:
	mov r0, #0x20
	str r0, [r4, #0x24]
	mov r0, #2
	str r0, [r4, #0x28]
	mov r0, #5
	str r0, [r4, #0x2c]
	mov r0, #0
	strb r0, [r4, #0x54]
	b _020DCB10
_020DCACC:
	mov r0, #0x80
	str r0, [r4, #0x24]
	mov r0, #2
	str r0, [r4, #0x28]
	mov r0, #0xa
	str r0, [r4, #0x2c]
	mov r0, #0
	strb r0, [r4, #0x54]
	b _020DCB10
_020DCAF0:
	mov r0, #0x100
	str r0, [r4, #0x24]
	mov r0, #3
	str r0, [r4, #0x28]
	mov r0, #5
	str r0, [r4, #0x2c]
	mov r0, #0
	strb r0, [r4, #0x54]
_020DCB10:
	ldr r0, [r4, #0x24]
	str r0, [r4, #0x1c]
	ldr r0, [r4, #0x58]
	orr r0, r0, #0x40
	orr r0, r0, #0x4300
	str r0, [r4, #0x58]
	ldmia sp!, {r3, r4, r5, pc}
_020DCB2C:
	cmp r2, #2
	bne _020DCCF4
	cmp ip, #0x100000
	bhi _020DCB5C
	bhs _020DCB80
	cmp ip, #0x40000
	bhi _020DCB50
	beq _020DCB80
	b _020DCD34
_020DCB50:
	cmp ip, #0x80000
	beq _020DCB80
	b _020DCD34
_020DCB5C:
	cmp ip, #0x400000
	bhi _020DCB74
	bhs _020DCBF8
	cmp ip, #0x200000
	beq _020DCBAC
	b _020DCD34
_020DCB74:
	cmp ip, #0x800000
	beq _020DCC48
	b _020DCD34
_020DCB80:
	mov r0, #0x19
	str r0, [r4, #0x30]
	mov r1, #0x12c
	str r1, [r4, #0x34]
	ldr r0, _020DCD5C ; =0x00001388
	str r1, [r4, #0x50]
	str r0, [r4, #0x40]
	ldr r0, [r4, #0x58]
	orr r0, r0, #0x480
	str r0, [r4, #0x58]
	b _020DCCC4
_020DCBAC:
	mov r0, #0x17
	str r0, [r4, #0x30]
	mov r1, #0x12c
	str r1, [r4, #0x34]
	mov r1, #0x1f4
	ldr r0, _020DCD5C ; =0x00001388
	str r1, [r4, #0x40]
	str r0, [r4, #0x44]
	mov r1, r0, lsl #1
	ldr r0, _020DCD60 ; =0x0000EA60
	str r1, [r4, #0x38]
	str r0, [r4, #0x3c]
	mov r0, #0
	strb r0, [r4, #0x54]
	ldr r0, [r4, #0x58]
	orr r0, r0, #0x80
	orr r0, r0, #0x5400
	str r0, [r4, #0x58]
	b _020DCCC4
_020DCBF8:
	mov r0, #0x258
	str r0, [r4, #0x40]
	add r0, r0, #0x960
	str r0, [r4, #0x44]
	mov r0, #0x46
	str r0, [r4, #0x48]
	mov r0, #0x96
	str r0, [r4, #0x4c]
	ldr r1, _020DCD64 ; =0x000059D8
	ldr r0, _020DCD68 ; =0x000C3500
	str r1, [r4, #0x38]
	str r0, [r4, #0x3c]
	mov r0, #0
	strb r0, [r4, #0x54]
	mov r0, #0x1000
	str r0, [r4, #0x20]
	ldr r0, [r4, #0x58]
	orr r0, r0, #0xd000
	str r0, [r4, #0x58]
	b _020DCCC4
_020DCC48:
	cmp r0, #0
	bne _020DCC88
	mov r0, #0x3e8
	str r0, [r4, #0x40]
	add r0, r0, #0x7d0
	str r0, [r4, #0x44]
	ldr r1, _020DCD6C ; =0x000109A0
	ldr r0, _020DCD70 ; =0x00027100
	str r1, [r4, #0x38]
	str r0, [r4, #0x3c]
	mov r0, #0
	strb r0, [r4, #0x54]
	ldr r0, [r4, #0x58]
	orr r0, r0, #0x5000
	str r0, [r4, #0x58]
	b _020DCCC4
_020DCC88:
	cmp r0, #1
	bne _020DCCC4
	mov r0, #0x3e8
	str r0, [r4, #0x40]
	add r0, r0, #0x7d0
	str r0, [r4, #0x44]
	ldr r1, _020DCD6C ; =0x000109A0
	ldr r0, _020DCD70 ; =0x00027100
	str r1, [r4, #0x38]
	str r0, [r4, #0x3c]
	mov r0, #0x84
	strb r0, [r4, #0x54]
	ldr r0, [r4, #0x58]
	orr r0, r0, #0x5000
	str r0, [r4, #0x58]
_020DCCC4:
	mov r0, #0x10000
	str r0, [r4, #0x1c]
	mov r0, #0x100
	str r0, [r4, #0x24]
	mov r0, #3
	str r0, [r4, #0x28]
	mov r0, #5
	str r0, [r4, #0x2c]
	ldr r0, [r4, #0x58]
	orr r0, r0, #0xb40
	str r0, [r4, #0x58]
	ldmia sp!, {r3, r4, r5, pc}
_020DCCF4:
	cmp r2, #3
	bne _020DCD34
	cmp ip, #0x2000
	cmpne ip, #0x8000
	bne _020DCD34
	str ip, [r4, #0x24]
	str ip, [r4, #0x1c]
	mov r0, #2
	str r0, [r4, #0x28]
	mov r0, #0
	strb r0, [r4, #0x54]
	ldr r0, [r4, #0x58]
	orr r0, r0, #0x40
	orr r0, r0, #0x4300
	str r0, [r4, #0x58]
	ldmia sp!, {r3, r4, r5, pc}
_020DCD34:
	mov r1, #0
	str r1, [r4, #4]
	str r1, [r4, #0x18]
	ldr r0, _020DCD54 ; =cardi_common
	mov r1, #3
	ldr r0, [r0]
	str r1, [r0]
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_020DCD54: .word cardi_common
_020DCD58: .word 0x0000203F
_020DCD5C: .word 0x00001388
_020DCD60: .word 0x0000EA60
_020DCD64: .word 0x000059D8
_020DCD68: .word 0x000C3500
_020DCD6C: .word 0x000109A0
_020DCD70: .word 0x00027100
	arm_func_end CARDi_IdentifyBackupCore

	arm_func_start CARDi_RequestStreamCommandCore
CARDi_RequestStreamCommandCore: ; 0x020DCD74
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	mov sb, r0
	ldr r7, [sb, #0x2c]
	ldr r0, _020DCF5C ; =_version_NINTENDO_BACKUP
	ldr r4, [sb, #0x34]
	ldr sl, [sb, #0x30]
	mov r5, #0x100
	bl OSi_ReferSymbol
	cmp r7, #0xb
	bne _020DCDA8
	bl CARD_GetBackupSectorSize
	mov r5, r0
	b _020DCDB8
_020DCDA8:
	cmp r7, #0xf
	ldreq r0, _020DCF60 ; =cardi_common
	ldreq r0, [r0]
	ldreq r5, [r0, #0x20]
_020DCDB8:
	add r6, sb, #0x120
	mov fp, #9
_020DCDC0:
	ldr r8, [sb, #0x24]
	ldr r0, [sb]
	cmp r5, r8
	movlo r8, r5
	str r8, [r0, #0x14]
	ldr r0, [sb, #0x114]
	tst r0, #0x40
	beq _020DCDFC
	ldr r0, [sb, #0x114]
	mov r1, #7
	bic r0, r0, #0x40
	str r0, [sb, #0x114]
	ldr r0, [sb]
	str r1, [r0]
	b _020DCF08
_020DCDFC:
	cmp r4, #3
	addls pc, pc, r4, lsl #2
	b _020DCE8C
_020DCE08: ; jump table
	b _020DCE18 ; case 0
	b _020DCE3C ; case 1
	b _020DCE3C ; case 2
	b _020DCE74 ; case 3
_020DCE18:
	mov r0, r6
	mov r1, r8
	bl DC_InvalidateRange
	ldr r1, [sb, #0x1c]
	ldr r0, [sb]
	str r1, [r0, #0xc]
	ldr r0, [sb]
	str r6, [r0, #0x10]
	b _020DCE8C
_020DCE3C:
	ldr r0, [sb, #0x1c]
	mov r1, r6
	mov r2, r8
	bl MI_CpuCopy8
	mov r0, r6
	mov r1, r8
	bl DC_FlushRange
	bl DC_WaitWriteBufferEmpty
	ldr r0, [sb]
	str r6, [r0, #0xc]
	ldr r1, [sb, #0x20]
	ldr r0, [sb]
	str r1, [r0, #0x10]
	b _020DCE8C
_020DCE74:
	ldr r1, [sb, #0x1c]
	ldr r0, [sb]
	str r1, [r0, #0xc]
	ldr r1, [sb, #0x20]
	ldr r0, [sb]
	str r1, [r0, #0x10]
_020DCE8C:
	mov r0, sb
	mov r1, r7
	mov r2, sl
	bl CARDi_Request
	cmp r0, #0
	beq _020DCF08
	cmp r4, #2
	bne _020DCEC8
	mov r0, sb
	mov r1, fp
	mov r2, #1
	bl CARDi_Request
	cmp r0, #0
	beq _020DCF08
	b _020DCEE0
_020DCEC8:
	cmp r4, #0
	bne _020DCEE0
	ldr r1, [sb, #0x20]
	mov r0, r6
	mov r2, r8
	bl MI_CpuCopy8
_020DCEE0:
	ldr r0, [sb, #0x1c]
	add r0, r0, r8
	str r0, [sb, #0x1c]
	ldr r0, [sb, #0x20]
	add r0, r0, r8
	str r0, [sb, #0x20]
	ldr r0, [sb, #0x24]
	subs r0, r0, r8
	str r0, [sb, #0x24]
	bne _020DCDC0
_020DCF08:
	ldr r6, [sb, #0x38]
	ldr r5, [sb, #0x3c]
	bl OS_DisableInterrupts
	ldr r1, [sb, #0x114]
	mov r4, r0
	bic r0, r1, #0x4c
	str r0, [sb, #0x114]
	add r0, sb, #0x10c
	bl OS_WakeupThread
	ldr r0, [sb, #0x114]
	tst r0, #0x10
	beq _020DCF40
	add r0, sb, #0x44
	bl OS_WakeupThreadDirect
_020DCF40:
	mov r0, r4
	bl OS_RestoreInterrupts
	cmp r6, #0
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	mov r0, r5
	blx r6
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_020DCF5C: .word _version_NINTENDO_BACKUP
_020DCF60: .word cardi_common
	arm_func_end CARDi_RequestStreamCommandCore

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

	.section .text.unified_36,4,1,6

	arm_func_start CARD_TerminateForPulledOut
CARD_TerminateForPulledOut: ; 0x020DDB34
	stmdb sp!, {r3, r4, r5, lr}
	mov r0, #0
	mov r5, #1
	bl MI_StopDma
	mov r0, r5
	bl MI_StopDma
	mov r0, #2
	bl MI_StopDma
	mov r0, #3
	bl MI_StopDma
	ldr r0, _020DDBB8 ; =0x027FFFA8
	ldrh r0, [r0]
	and r0, r0, #0x8000
	movs r0, r0, asr #0xf
	beq _020DDB9C
	bl PM_ForceToPowerOff
	cmp r0, #4
	bne _020DDB94
	ldr r4, _020DDBBC ; =0x000A3A47
_020DDB80:
	mov r0, r4
	bl OS_SpinWait
	bl PM_ForceToPowerOff
	cmp r0, #4
	beq _020DDB80
_020DDB94:
	cmp r0, #0
	moveq r5, #0
_020DDB9C:
	cmp r5, #0
	beq _020DDBB0
	mov r0, #1
	mov r1, r0
	bl CARDi_SendtoPxi
_020DDBB0:
	bl OS_Terminate
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_020DDBB8: .word 0x027FFFA8
_020DDBBC: .word 0x000A3A47
	arm_func_end CARD_TerminateForPulledOut

	.section .text.unified_38,4,1,6

	; Functions presumably in libcard but not present in plat

	arm_func_start CARD_SetSpiReadWaitCycles
CARD_SetSpiReadWaitCycles: ; 0x020DDC70
	ldr r1, _020DDC7C ; =_021E4088
	strb r0, [r1, #2]
	bx lr
	.align 2, 0
_020DDC7C: .word _021E4088
	arm_func_end CARD_SetSpiReadWaitCycles

	arm_func_start CARD_SetSpiWriteWaitCycles
CARD_SetSpiWriteWaitCycles: ; 0x020DDC80
	ldr r1, _020DDC8C ; =_021E4088
	strb r0, [r1, #1]
	bx lr
	.align 2, 0
_020DDC8C: .word _021E4088
	arm_func_end CARD_SetSpiWriteWaitCycles

	arm_func_start CARDi_SpiDummyWait
CARDi_SpiDummyWait: ; 0x020DDC90
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, lr}
	bl OS_GetTick
	mov r4, #0
	ldr r5, _020DDCEC ; =0x000082EA
	ldr sl, _020DDCF0 ; =_021E4088
	mov r6, r0
	mov r7, r1
	mov sb, r4
	mov r8, #0xfa00
_020DDCB4:
	bl OS_GetTick
	subs r3, r0, r6
	sbc r2, r1, r7
	umull r0, r1, r3, r8
	mla r1, r3, sb, r1
	mla r1, r2, r8, r1
	mov r2, r5
	mov r3, r4
	bl _ll_udiv
	ldrb r2, [sl]
	cmp r1, r2, asr #31
	cmpeq r0, r2
	blo _020DDCB4
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
	.align 2, 0
_020DDCEC: .word 0x000082EA
_020DDCF0: .word _021E4088
	arm_func_end CARDi_SpiDummyWait

	arm_func_start CARD_SpiWaitInit
CARD_SpiWaitInit: ; 0x020DDCF4
	stmdb sp!, {r3, lr}
	ldr r1, _020DDD38 ; =_021E4088
	mov r0, #0x32
	strb r0, [r1, #2]
	strb r0, [r1, #1]
	strb r0, [r1]
	mov r2, #1
	ldr r0, _020DDD3C ; =_02110FBC
	str r2, [r1, #4]
	ldrh r1, [r0]
	ldr r0, _020DDD40 ; =0x0000FFFD
	cmp r1, r0
	ldmneia sp!, {r3, pc}
	bl OS_GetLockID
	ldr r1, _020DDD3C ; =_02110FBC
	strh r0, [r1]
	ldmia sp!, {r3, pc}
	.align 2, 0
_020DDD38: .word _021E4088
_020DDD3C: .word _02110FBC
_020DDD40: .word 0x0000FFFD
	arm_func_end CARD_SpiWaitInit

	arm_func_start CARDi_SpiWaitWriteByte
CARDi_SpiWaitWriteByte: ; 0x020DDD44
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	mov r6, r0
	bl OS_GetTick
	mov fp, #0
	ldr sl, _020DDDE4 ; =0x000082EA
	ldr sb, _020DDDE8 ; =_021E4088
	mov r4, r0
	mov r5, r1
	mov r8, fp
	mov r7, #0xfa00
_020DDD6C:
	bl OS_GetTick
	subs lr, r0, r4
	sbc ip, r1, r5
	mov r2, sl
	mov r3, fp
	umull r0, r1, lr, r7
	mla r1, lr, r8, r1
	mla r1, ip, r7, r1
	bl _ll_udiv
	ldrb r2, [sb, #1]
	cmp r1, r2, asr #31
	cmpeq r0, r2
	blo _020DDD6C
	ldr r0, [r6, #4]
	ldr r1, _020DDDEC ; =0x040001A2
	ldrb r0, [r0]
	strh r0, [r1]
	ldr r0, [r6, #4]
	sub r1, r1, #2
	add r0, r0, #1
	str r0, [r6, #4]
_020DDDC0:
	ldrh r0, [r1]
	tst r0, #0x80
	bne _020DDDC0
	ldr r0, _020DDDEC ; =0x040001A2
	ldrh r0, [r0]
	strh r0, [sp]
	ldrh r0, [sp]
	and r0, r0, #0xff
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_020DDDE4: .word 0x000082EA
_020DDDE8: .word _021E4088
_020DDDEC: .word 0x040001A2
	arm_func_end CARDi_SpiWaitWriteByte

	arm_func_start CARDi_SpiWaitReadByte
CARDi_SpiWaitReadByte: ; 0x020DDDF0
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	mov r6, r0
	bl OS_GetTick
	mov fp, #0
	ldr sl, _020DDE88 ; =0x000082EA
	ldr sb, _020DDE8C ; =_021E4088
	mov r4, r0
	mov r5, r1
	mov r8, fp
	mov r7, #0xfa00
_020DDE18:
	bl OS_GetTick
	subs lr, r0, r4
	sbc ip, r1, r5
	mov r2, sl
	mov r3, fp
	umull r0, r1, lr, r7
	mla r1, lr, r8, r1
	mla r1, ip, r7, r1
	bl _ll_udiv
	ldrb r2, [sb, #2]
	cmp r1, r2, asr #31
	cmpeq r0, r2
	blo _020DDE18
	ldr r0, _020DDE90 ; =0x040001A2
	mov r1, #0
	strh r1, [r0]
	sub r1, r0, #2
_020DDE5C:
	ldrh r0, [r1]
	tst r0, #0x80
	bne _020DDE5C
	ldr r1, _020DDE90 ; =0x040001A2
	ldr r0, [r6, #8]
	ldrh r1, [r1]
	strb r1, [r0]
	ldr r0, [r6, #8]
	add r0, r0, #1
	str r0, [r6, #8]
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_020DDE88: .word 0x000082EA
_020DDE8C: .word _021E4088
_020DDE90: .word 0x040001A2
	arm_func_end CARDi_SpiWaitReadByte

	arm_func_start CARD_SpiWaitReadRange
CARD_SpiWaitReadRange: ; 0x020DDE94
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, lr}
	ldr r2, _020DDFC4 ; =_021E409C
	ldr r1, _020DDFC8 ; =_021E4088
	mov r6, r0
	str r2, [r1, #8]
	str r6, [r1, #0xc]
	mov r0, #1
	strb r0, [r2]
	mov r1, #0
	ldr r0, _020DDFCC ; =_02110FBC
	strb r1, [r6]
	ldrh r0, [r0]
	bl CARD_LockBackup
	ldr r0, _020DDFCC ; =_02110FBC
	ldrh r0, [r0]
	bl OS_LockCard
	ldr r2, _020DDFD0 ; =0x040001A0
_020DDED8:
	ldrh r0, [r2]
	tst r0, #0x80
	bne _020DDED8
	ldr r1, _020DDFD4 ; =0x0000A042
	ldr r0, _020DDFD8 ; =_021E408C
	strh r1, [r2]
	bl CARDi_SpiWaitWriteByte
	bl CARDi_SpiDummyWait
	ldr r0, _020DDFD8 ; =_021E408C
	bl CARDi_SpiWaitReadByte
	bl CARDi_SpiDummyWait
	ldrb r4, [r6]
	ldr r0, _020DDFC8 ; =_021E4088
	mov r5, #0
	str r5, [r0, #8]
	cmp r4, #0xb9
	movhi r4, #0
	str r6, [r0, #0xc]
	strb r5, [r6]
	cmp r4, #0
	beq _020DDF60
	sub sl, r4, #1
	ldr sb, _020DDFDC ; =0x0000A002
	ldr r8, _020DDFD0 ; =0x040001A0
	ldr r7, _020DDFD8 ; =_021E408C
	b _020DDF54
_020DDF40:
	mov r0, r7
	cmp r5, sl
	streqh sb, [r8]
	bl CARDi_SpiWaitReadByte
	add r5, r5, #1
_020DDF54:
	cmp r5, r4
	blt _020DDF40
	b _020DDF74
_020DDF60:
	ldr r2, _020DDFDC ; =0x0000A002
	ldr r1, _020DDFD0 ; =0x040001A0
	ldr r0, _020DDFD8 ; =_021E408C
	strh r2, [r1]
	bl CARDi_SpiWaitReadByte
_020DDF74:
	mov r5, #0
	b _020DDF8C
_020DDF7C:
	ldrb r0, [r6, r5]
	eor r0, r0, #0xaa
	strb r0, [r6, r5]
	add r5, r5, #1
_020DDF8C:
	cmp r5, r4
	blt _020DDF7C
	ldr r1, _020DDFD0 ; =0x040001A0
_020DDF98:
	ldrh r0, [r1]
	tst r0, #0x80
	bne _020DDF98
	ldr r0, _020DDFCC ; =_02110FBC
	ldrh r0, [r0]
	bl OS_UnLockCard
	ldr r0, _020DDFCC ; =_02110FBC
	ldrh r0, [r0]
	bl CARD_UnlockBackup
	mov r0, r4
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
	.align 2, 0
_020DDFC4: .word _021E409C
_020DDFC8: .word _021E4088
_020DDFCC: .word _02110FBC
_020DDFD0: .word 0x040001A0
_020DDFD4: .word 0x0000A042
_020DDFD8: .word _021E408C
_020DDFDC: .word 0x0000A002
	arm_func_end CARD_SpiWaitReadRange

	arm_func_start CARD_SpiWaitWriteRange
CARD_SpiWaitWriteRange: ; 0x020DDFE0
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, lr}
	ldr r7, _020DE0AC ; =_021E4158
	ldr r3, _020DE0B0 ; =_021E4088
	mov r4, #0
	str r7, [r3, #8]
	ldr r2, _020DE0B4 ; =_02110FBC
	str r4, [r3, #0xc]
	mov r6, r0
	mov r3, #2
	ldrh r0, [r2]
	mov r5, r1
	strb r3, [r7]
	bl CARD_LockBackup
	ldr r0, _020DE0B4 ; =_02110FBC
	ldrh r0, [r0]
	bl OS_LockCard
	ldr r2, _020DE0B8 ; =0x040001A0
_020DE024:
	ldrh r0, [r2]
	tst r0, #0x80
	bne _020DE024
	ldr r1, _020DE0BC ; =0x0000A042
	ldr r0, _020DE0C0 ; =_021E408C
	strh r1, [r2]
	bl CARDi_SpiWaitWriteByte
	bl CARDi_SpiDummyWait
	ldr r0, _020DE0B0 ; =_021E4088
	mov r4, #0
	str r6, [r0, #8]
	sub sb, r5, #1
	ldr r8, _020DE0C4 ; =0x0000A002
	ldr r7, _020DE0B8 ; =0x040001A0
	ldr r6, _020DE0C0 ; =_021E408C
	b _020DE078
_020DE064:
	mov r0, r6
	cmp r4, sb
	streqh r8, [r7]
	bl CARDi_SpiWaitWriteByte
	add r4, r4, #1
_020DE078:
	cmp r4, r5
	blt _020DE064
	ldr r1, _020DE0B8 ; =0x040001A0
_020DE084:
	ldrh r0, [r1]
	tst r0, #0x80
	bne _020DE084
	ldr r0, _020DE0B4 ; =_02110FBC
	ldrh r0, [r0]
	bl OS_UnLockCard
	ldr r0, _020DE0B4 ; =_02110FBC
	ldrh r0, [r0]
	bl CARD_UnlockBackup
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	.align 2, 0
_020DE0AC: .word _021E4158
_020DE0B0: .word _021E4088
_020DE0B4: .word _02110FBC
_020DE0B8: .word 0x040001A0
_020DE0BC: .word 0x0000A042
_020DE0C0: .word _021E408C
_020DE0C4: .word 0x0000A002
	arm_func_end CARD_SpiWaitWriteRange

	arm_func_start CARD_SpiWaitGetStatus
CARD_SpiWaitGetStatus: ; 0x020DE0C8
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	ldr r3, _020DE228 ; =_021E409C
	ldr r1, _020DE22C ; =_021E4088
	add r2, sp, #0
	str r3, [r1, #8]
	str r2, [r1, #0xc]
	ldr r0, _020DE230 ; =_02110FBC
	mov r1, #8
	ldrh r0, [r0]
	strb r1, [r3]
	bl CARD_LockBackup
	ldr r0, _020DE230 ; =_02110FBC
	ldrh r0, [r0]
	bl OS_LockCard
	bl OS_GetTick
	mov fp, #0
	mov r4, r0
	mov r5, r1
	mov r6, #0x40
	mov r8, fp
	mov r7, #0x3e8
	mov sl, fp
	mov sb, #0x3c
_020DE124:
	bl OS_GetTick
	subs r0, r0, r4
	umull lr, ip, r0, r6
	mla ip, r0, fp, ip
	sbc r1, r1, r5
	mla ip, r1, r6, ip
	umull r0, r1, lr, r7
	mla r1, lr, r8, r1
	mla r1, ip, r7, r1
	ldr r2, _020DE234 ; =0x000082EA
	mov r3, #0
	bl _ll_udiv
	cmp r1, sl
	cmpeq r0, sb
	blo _020DE124
	ldr r2, _020DE238 ; =0x040001A0
_020DE164:
	ldrh r0, [r2]
	tst r0, #0x80
	bne _020DE164
	ldr r1, _020DE23C ; =0x0000A042
	ldr r0, _020DE240 ; =_021E408C
	strh r1, [r2]
	bl CARDi_SpiWaitWriteByte
	strb r0, [sp]
	bl OS_GetTick
	mov r4, r0
	mov r5, r1
	mov r6, #0x40
	mov r7, #0x3e8
	mov sb, #0
	mov r8, #0x32
_020DE1A0:
	bl OS_GetTick
	subs r0, r0, r4
	mov r3, #0
	sbc ip, r1, r5
	mov r1, r3
	umull fp, sl, r0, r6
	mla sl, r0, r1, sl
	mla sl, ip, r6, sl
	mov ip, r3
	umull r0, r1, fp, r7
	mla r1, fp, ip, r1
	mla r1, sl, r7, r1
	ldr r2, _020DE234 ; =0x000082EA
	bl _ll_udiv
	cmp r1, sb
	cmpeq r0, r8
	blo _020DE1A0
	ldr r2, _020DE244 ; =0x0000A002
	ldr r1, _020DE238 ; =0x040001A0
	ldr r0, _020DE240 ; =_021E408C
	strh r2, [r1]
	bl CARDi_SpiWaitReadByte
	ldr r1, _020DE238 ; =0x040001A0
_020DE1FC:
	ldrh r0, [r1]
	tst r0, #0x80
	bne _020DE1FC
	ldr r0, _020DE230 ; =_02110FBC
	ldrh r0, [r0]
	bl OS_UnLockCard
	ldr r0, _020DE230 ; =_02110FBC
	ldrh r0, [r0]
	bl CARD_UnlockBackup
	ldrb r0, [sp]
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_020DE228: .word _021E409C
_020DE22C: .word _021E4088
_020DE230: .word _02110FBC
_020DE234: .word 0x000082EA
_020DE238: .word 0x040001A0
_020DE23C: .word 0x0000A042
_020DE240: .word _021E408C
_020DE244: .word 0x0000A002
	arm_func_end CARD_SpiWaitGetStatus

	; libwm

	arm_func_start WM_Init
WM_Init: ; 0x020DE248
	stmdb sp!, {r3, lr}
	mov r2, #0xf00
	bl WmInitCore
	cmp r0, #0
	ldmneia sp!, {r3, pc}
	ldr r1, _020DE270 ; =_021E4220
	mov r2, #0
	ldr r1, [r1, #4]
	strh r2, [r1, #0x16]
	ldmia sp!, {r3, pc}
	.align 2, 0
_020DE270: .word _021E4220
	arm_func_end WM_Init

	arm_func_start WmInitCore
WmInitCore: ; 0x020DE274
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, lr}
	mov r6, r0
	mov r4, r1
	mov r7, r2
	bl OS_DisableInterrupts
	ldr r1, _020DE444 ; =_021E4220
	mov r5, r0
	ldrh r1, [r1]
	cmp r1, #0
	beq _020DE2A8
	bl OS_RestoreInterrupts
	mov r0, #3
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
_020DE2A8:
	cmp r6, #0
	bne _020DE2BC
	bl OS_RestoreInterrupts
	mov r0, #6
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
_020DE2BC:
	cmp r4, #3
	bls _020DE2D0
	bl OS_RestoreInterrupts
	mov r0, #6
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
_020DE2D0:
	tst r6, #0x1f
	beq _020DE2E4
	bl OS_RestoreInterrupts
	mov r0, #6
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
_020DE2E4:
	bl PXI_Init
	mov r0, #0xa
	mov r1, #1
	bl PXI_IsCallbackReady
	cmp r0, #0
	bne _020DE30C
	mov r0, r5
	bl OS_RestoreInterrupts
	mov r0, #4
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
_020DE30C:
	mov r0, r6
	mov r1, r7
	bl DC_InvalidateRange
	mov r0, r4
	mov r1, r6
	mov r3, r7
	mov r2, #0
	bl MI_DmaFill32
	ldr r0, _020DE444 ; =_021E4220
	add r1, r6, #0x200
	str r6, [r0, #4]
	str r1, [r6]
	ldr r2, [r0, #4]
	ldr r1, [r2]
	add r1, r1, #0x300
	str r1, [r2, #4]
	ldr r2, [r0, #4]
	ldr r1, [r2, #4]
	add r1, r1, #0x800
	str r1, [r2, #0xc]
	ldr r1, [r0, #4]
	ldr r0, [r1, #0xc]
	add r0, r0, #0x100
	str r0, [r1, #0x10]
	bl WmClearFifoRecvFlag
	ldr r1, _020DE444 ; =_021E4220
	mov r3, #0
	ldr r0, [r1, #4]
	mov r2, r3
	strh r4, [r0, #0x14]
	ldr r0, [r1, #4]
	str r3, [r0, #0x14c]
	ldr r0, [r1, #4]
	add r0, r0, #0x100
	strh r3, [r0, #0x50]
	b _020DE3B8
_020DE39C:
	ldr r0, [r1, #4]
	add r0, r0, r3, lsl #2
	str r2, [r0, #0xcc]
	ldr r0, [r1, #4]
	add r0, r0, r3, lsl #2
	str r2, [r0, #0x10c]
	add r3, r3, #1
_020DE3B8:
	cmp r3, #0x10
	blt _020DE39C
	ldr r0, _020DE448 ; =_021E4228
	ldr r1, _020DE44C ; =_021E4248
	mov r2, #0xa
	bl OS_InitMessageQueue
	mov r6, #0
	mov r4, #0x8000
	ldr sl, _020DE450 ; =_021E42C0
	mov sb, #2
	ldr r8, _020DE448 ; =_021E4228
	mov r7, #1
	b _020DE414
_020DE3EC:
	mov r2, r6, lsl #8
	mov r1, sb
	add r0, sl, r6, lsl #8
	strh r4, [sl, r2]
	bl DC_StoreRange
	mov r0, r8
	mov r2, r7
	add r1, sl, r6, lsl #8
	bl OS_SendMessage
	add r6, r6, #1
_020DE414:
	cmp r6, #0xa
	blt _020DE3EC
	ldr r1, _020DE454 ; =WmReceiveFifo
	mov r0, #0xa
	bl PXI_SetFifoRecvCallback
	ldr r1, _020DE444 ; =_021E4220
	mov r2, #1
	mov r0, r5
	strh r2, [r1]
	bl OS_RestoreInterrupts
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
	.align 2, 0
_020DE444: .word _021E4220
_020DE448: .word _021E4228
_020DE44C: .word _021E4248
_020DE450: .word _021E42C0
_020DE454: .word WmReceiveFifo
	arm_func_end WmInitCore

	arm_func_start WM_Finish
WM_Finish: ; 0x020DE458
	stmdb sp!, {r4, lr}
	bl OS_DisableInterrupts
	mov r4, r0
	bl WMi_CheckInitialized
	cmp r0, #0
	beq _020DE480
	mov r0, r4
	bl OS_RestoreInterrupts
	mov r0, #3
	ldmia sp!, {r4, pc}
_020DE480:
	mov r0, #1
	mov r1, #0
	bl WMi_CheckStateEx
	cmp r0, #0
	ldmneia sp!, {r4, pc}
	bl WmClearFifoRecvFlag
	mov r0, #0xa
	mov r1, #0
	bl PXI_SetFifoRecvCallback
	ldr r1, _020DE4C4 ; =_021E4220
	mov r2, #0
	str r2, [r1, #4]
	mov r0, r4
	strh r2, [r1]
	bl OS_RestoreInterrupts
	mov r0, #0
	ldmia sp!, {r4, pc}
	.align 2, 0
_020DE4C4: .word _021E4220
	arm_func_end WM_Finish

	arm_func_start WMi_SetCallbackTable
WMi_SetCallbackTable: ; 0x020DE4C8
	ldr r2, _020DE4DC ; =_021E4220
	ldr r2, [r2, #4]
	add r0, r2, r0, lsl #2
	str r1, [r0, #0x18]
	bx lr
	.align 2, 0
_020DE4DC: .word _021E4220
	arm_func_end WMi_SetCallbackTable

	arm_func_start WmGetCommandBuffer4Arm7
WmGetCommandBuffer4Arm7: ; 0x020DE4E0
	stmdb sp!, {r3, lr}
	ldr r0, _020DE534 ; =_021E4228
	add r1, sp, #0
	mov r2, #0
	bl OS_ReceiveMessage
	cmp r0, #0
	moveq r0, #0
	ldmeqia sp!, {r3, pc}
	ldr r0, [sp]
	mov r1, #2
	bl DC_InvalidateRange
	ldr r1, [sp]
	ldrh r0, [r1]
	tst r0, #0x8000
	movne r0, r1
	ldmneia sp!, {r3, pc}
	ldr r0, _020DE534 ; =_021E4228
	mov r2, #1
	bl OS_JamMessage
	mov r0, #0
	ldmia sp!, {r3, pc}
	.align 2, 0
_020DE534: .word _021E4228
	arm_func_end WmGetCommandBuffer4Arm7

	arm_func_start WMi_SendCommand
WMi_SendCommand: ; 0x020DE538
	stmdb sp!, {r0, r1, r2, r3}
	stmdb sp!, {r3, r4, r5, lr}
	mov r5, r0
	bl WmGetCommandBuffer4Arm7
	movs r4, r0
	moveq r0, #8
	ldmeqia sp!, {r3, r4, r5, lr}
	addeq sp, sp, #0x10
	bxeq lr
	strh r5, [r4]
	ldrh r5, [sp, #0x14]
	add r0, sp, #0x14
	bic r0, r0, #3
	mov r3, #0
	cmp r5, #0
	add r2, r0, #4
	ble _020DE598
_020DE57C:
	add r2, r2, #4
	ldr r1, [r2, #-4]
	add r0, r4, r3, lsl #2
	add r3, r3, #1
	str r1, [r0, #4]
	cmp r3, r5
	blt _020DE57C
_020DE598:
	mov r0, r4
	mov r1, #0x100
	bl DC_StoreRange
	mov r1, r4
	mov r0, #0xa
	mov r2, #0
	bl PXI_SendWordByFifo
	mov r5, r0
	ldr r0, _020DE5E0 ; =_021E4228
	mov r1, r4
	mov r2, #1
	bl OS_SendMessage
	cmp r5, #0
	movlt r0, #8
	movge r0, #2
	ldmia sp!, {r3, r4, r5, lr}
	add sp, sp, #0x10
	bx lr
	.align 2, 0
_020DE5E0: .word _021E4228
	arm_func_end WMi_SendCommand

	arm_func_start WMi_SendCommandDirect
WMi_SendCommandDirect: ; 0x020DE5E4
	stmdb sp!, {r4, r5, r6, lr}
	mov r6, r0
	mov r4, r1
	bl WmGetCommandBuffer4Arm7
	movs r5, r0
	moveq r0, #8
	ldmeqia sp!, {r4, r5, r6, pc}
	mov r0, r6
	mov r1, r5
	mov r2, r4
	bl MI_CpuCopy8
	mov r0, r5
	mov r1, r4
	bl DC_StoreRange
	mov r1, r5
	mov r0, #0xa
	mov r2, #0
	bl PXI_SendWordByFifo
	mov r4, r0
	ldr r0, _020DE650 ; =_021E4228
	mov r1, r5
	mov r2, #1
	bl OS_SendMessage
	cmp r4, #0
	movlt r0, #8
	movge r0, #2
	ldmia sp!, {r4, r5, r6, pc}
	.align 2, 0
_020DE650: .word _021E4228
	arm_func_end WMi_SendCommandDirect

	arm_func_start WMi_GetSystemWork
WMi_GetSystemWork: ; 0x020DE654
	ldr r0, _020DE660 ; =_021E4220
	ldr r0, [r0, #4]
	bx lr
	.align 2, 0
_020DE660: .word _021E4220
	arm_func_end WMi_GetSystemWork

	arm_func_start WMi_CheckInitialized
WMi_CheckInitialized: ; 0x020DE664
	ldr r0, _020DE67C ; =_021E4220
	ldrh r0, [r0]
	cmp r0, #0
	movne r0, #0
	moveq r0, #3
	bx lr
	.align 2, 0
_020DE67C: .word _021E4220
	arm_func_end WMi_CheckInitialized

	arm_func_start WMi_CheckIdle
WMi_CheckIdle: ; 0x020DE680
	stmdb sp!, {r3, lr}
	bl WMi_CheckInitialized
	cmp r0, #0
	ldmneia sp!, {r3, pc}
	ldr r0, _020DE6C4 ; =_021E4220
	mov r1, #2
	ldr r0, [r0, #4]
	ldr r0, [r0, #4]
	bl DC_InvalidateRange
	ldr r0, _020DE6C4 ; =_021E4220
	ldr r0, [r0, #4]
	ldr r0, [r0, #4]
	ldrh r0, [r0]
	cmp r0, #1
	movls r0, #3
	movhi r0, #0
	ldmia sp!, {r3, pc}
	.align 2, 0
_020DE6C4: .word _021E4220
	arm_func_end WMi_CheckIdle

	arm_func_start WMi_CheckStateEx
WMi_CheckStateEx: ; 0x020DE6C8
	stmdb sp!, {r0, r1, r2, r3}
	stmdb sp!, {r3, lr}
	bl WMi_CheckInitialized
	cmp r0, #0
	ldmneia sp!, {r3, lr}
	addne sp, sp, #0x10
	bxne lr
	ldr r0, _020DE758 ; =_021E4220
	mov r1, #2
	ldr r0, [r0, #4]
	ldr r0, [r0, #4]
	bl DC_InvalidateRange
	ldr r0, _020DE758 ; =_021E4220
	ldr r3, [sp, #8]
	ldr r0, [r0, #4]
	add r1, sp, #8
	ldr r2, [r0, #4]
	bic r0, r1, #3
	add ip, r0, #4
	ldrh lr, [r2]
	cmp r3, #0
	mov r0, #3
	ldmeqia sp!, {r3, lr}
	addeq sp, sp, #0x10
	bxeq lr
	mov r1, #0
_020DE730:
	add ip, ip, #4
	ldr r2, [ip, #-4]
	cmp r2, lr
	moveq r0, r1
	subs r3, r3, #1
	str r3, [sp, #8]
	bne _020DE730
	ldmia sp!, {r3, lr}
	add sp, sp, #0x10
	bx lr
	.align 2, 0
_020DE758: .word _021E4220
	arm_func_end WMi_CheckStateEx

	arm_func_start WmReceiveFifo
WmReceiveFifo: ; 0x020DE75C
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #8
	ldr r0, _020DEAF4 ; =_021E4220
	cmp r2, #0
	ldr r4, [r0, #4]
	mov sl, r1
	addne sp, sp, #8
	ldmneia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldr r0, [r4, #0x10]
	mov r1, #0x100
	bl DC_InvalidateRange
	ldrh r0, [r4, #0x16]
	cmp r0, #0
	bne _020DE7A0
	ldr r0, [r4, #4]
	mov r1, #0x800
	bl DC_InvalidateRange
_020DE7A0:
	ldr r0, [r4, #0x10]
	cmp sl, r0
	beq _020DE7B8
	mov r0, sl
	mov r1, #0x100
	bl DC_InvalidateRange
_020DE7B8:
	ldrh r0, [sl]
	cmp r0, #0x2c
	blo _020DE870
	cmp r0, #0x80
	bne _020DE7F4
	ldrh r0, [sl, #2]
	cmp r0, #0x13
	bne _020DE7DC
	bl OS_Terminate
_020DE7DC:
	ldr r1, [r4, #0xc8]
	cmp r1, #0
	beq _020DEAB4
	mov r0, sl
	blx r1
	b _020DEAB4
_020DE7F4:
	cmp r0, #0x82
	bne _020DE848
	ldrh r0, [sl, #6]
	add r1, r4, r0, lsl #2
	ldr r0, [r1, #0xcc]
	cmp r0, #0
	beq _020DEAB4
	ldr r0, [r1, #0x10c]
	str r0, [sl, #0x1c]
	ldr r0, [r4, #0x14c]
	strh r0, [sl, #0x22]
	ldr r1, [r4, #4]
	ldr r0, [sl, #8]
	ldrh r1, [r1, #0x72]
	bl DC_InvalidateRange
	ldrh r1, [sl, #6]
	mov r0, sl
	add r1, r4, r1, lsl #2
	ldr r1, [r1, #0xcc]
	blx r1
	b _020DEAB4
_020DE848:
	cmp r0, #0x81
	bne _020DEAB4
	mov r0, #0xf
	strh r0, [sl]
	ldr r1, [sl, #0x1c]
	cmp r1, #0
	beq _020DEAB4
	mov r0, sl
	blx r1
	b _020DEAB4
_020DE870:
	cmp r0, #0xe
	bne _020DE8B0
	ldrh r0, [sl, #4]
	add r0, r0, #0xf5
	add r0, r0, #0xff00
	mov r0, r0, lsl #0x10
	mov r0, r0, lsr #0x10
	cmp r0, #1
	bhi _020DE8B0
	ldrh r0, [sl, #2]
	cmp r0, #0
	bne _020DE8B0
	ldr r1, [r4, #4]
	ldr r0, [sl, #8]
	ldrh r1, [r1, #0x72]
	bl DC_InvalidateRange
_020DE8B0:
	ldrh r1, [sl]
	cmp r1, #2
	ldreqh r0, [sl, #2]
	cmpeq r0, #0
	add r0, r4, r1, lsl #2
	bne _020DE8EC
	ldr r4, [r0, #0x18]
	bl WM_Finish
	cmp r4, #0
	addeq sp, sp, #8
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	mov r0, sl
	blx r4
	add sp, sp, #8
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020DE8EC:
	ldr r1, [r0, #0x18]
	cmp r1, #0
	beq _020DE914
	mov r0, sl
	blx r1
	ldr r0, _020DEAF4 ; =_021E4220
	ldrh r0, [r0]
	cmp r0, #0
	addeq sp, sp, #8
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020DE914:
	ldrh r0, [sl]
	cmp r0, #8
	cmpne r0, #0xc
	bne _020DEAB4
	cmp r0, #8
	bne _020DE958
	add r0, sl, #0xa
	str r0, [sp]
	ldrh r5, [sl, #8]
	ldrh r6, [sl, #0x10]
	ldrh r0, [sl, #0x12]
	add fp, sl, #0x14
	mov r7, #0
	str r0, [sp, #4]
	ldrh r8, [sl, #0x2c]
	ldrh sb, [sl, #0x2e]
	b _020DE988
_020DE958:
	cmp r0, #0xc
	bne _020DE988
	ldrh r5, [sl, #8]
	ldrh r7, [sl, #0xa]
	ldrh r0, [sl, #0xc]
	mov r6, #0
	mov fp, r6
	str r0, [sp, #4]
	add r0, sl, #0x10
	ldrh r8, [sl, #0x16]
	ldrh sb, [sl, #0x18]
	str r0, [sp]
_020DE988:
	cmp r5, #7
	cmpne r5, #9
	cmpne r5, #0x1a
	bne _020DEAB4
	cmp r5, #7
	ldreq r1, [r4, #0x14c]
	mov r0, #1
	orreq r0, r1, r0, lsl r6
	mvnne r0, r0, lsl r6
	ldrne r1, [r4, #0x14c]
	add r3, r4, #0x100
	andne r0, r1, r0
	str r0, [r4, #0x14c]
	ldr r0, _020DEAF8 ; =_021E4270
	mov r1, #0
	mov r2, #0x44
	strh r7, [r3, #0x50]
	bl MI_CpuFill8
	ldr ip, _020DEAF4 ; =_021E4220
	mov r1, #0x82
	strh r1, [ip, #0x50]
	mov r1, #0
	strh r1, [ip, #0x52]
	strh r5, [ip, #0x54]
	str r1, [ip, #0x58]
	str r1, [ip, #0x5c]
	strh r1, [ip, #0x60]
	strh r6, [ip, #0x62]
	strh r7, [ip, #0x70]
	ldr r2, [r4, #0x14c]
	ldr r1, _020DEAFC ; =0x0000FFFF
	strh r2, [ip, #0x72]
	strh r1, [ip, #0x6a]
	ldr r3, [sp, #4]
	ldr r0, [sp]
	ldr r1, _020DEB00 ; =_021E4284
	mov r2, #6
	strh r3, [ip, #0x8c]
	bl MI_CpuCopy8
	cmp fp, #0
	mov r2, #0x18
	beq _020DEA40
	ldr r1, _020DEB04 ; =_021E4294
	mov r0, fp
	bl MIi_CpuCopy16
	b _020DEA4C
_020DEA40:
	ldr r1, _020DEB04 ; =_021E4294
	mov r0, #0
	bl MIi_CpuClear16
_020DEA4C:
	cmp r7, #0
	moveq r1, r8
	movne r1, sb
	cmp r7, #0
	ldr r0, _020DEAF4 ; =_021E4220
	ldr r7, _020DEAF4 ; =_021E4220
	strh r1, [r0, #0x90]
	movne sb, r8
	ldr r5, _020DEAF8 ; =_021E4270
	mov r6, #0
	strh sb, [r7, #0x92]
_020DEA78:
	strh r6, [r7, #0x56]
	add r2, r4, r6, lsl #2
	ldr r0, [r2, #0xcc]
	cmp r0, #0
	beq _020DEAA0
	ldr r1, [r2, #0x10c]
	mov r0, r5
	str r1, [r7, #0x6c]
	ldr r1, [r2, #0xcc]
	blx r1
_020DEAA0:
	add r0, r6, #1
	mov r0, r0, lsl #0x10
	mov r6, r0, lsr #0x10
	cmp r6, #0x10
	blo _020DEA78
_020DEAB4:
	ldr r0, [r4, #0x10]
	mov r1, #0x100
	bl DC_InvalidateRange
	bl WmClearFifoRecvFlag
	ldr r0, [r4, #0x10]
	cmp sl, r0
	addeq sp, sp, #8
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldrh r2, [sl]
	mov r0, sl
	mov r1, #0x100
	orr r2, r2, #0x8000
	strh r2, [sl]
	bl DC_StoreRange
	add sp, sp, #8
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_020DEAF4: .word _021E4220
_020DEAF8: .word _021E4270
_020DEAFC: .word 0x0000FFFF
_020DEB00: .word _021E4284
_020DEB04: .word _021E4294
	arm_func_end WmReceiveFifo

	arm_func_start WmClearFifoRecvFlag
WmClearFifoRecvFlag: ; 0x020DEB08
	ldr r1, _020DEB20 ; =0x027FFF96
	ldrh r0, [r1]
	tst r0, #1
	bicne r0, r0, #1
	strneh r0, [r1]
	bx lr
	.align 2, 0
_020DEB20: .word 0x027FFF96
	arm_func_end WmClearFifoRecvFlag

	arm_func_start WMi_GetStatusAddress
WMi_GetStatusAddress: ; 0x020DEB24
	stmdb sp!, {r3, lr}
	bl WMi_CheckInitialized
	cmp r0, #0
	movne r0, #0
	ldreq r0, _020DEB44 ; =_021E4220
	ldreq r0, [r0, #4]
	ldreq r0, [r0, #4]
	ldmia sp!, {r3, pc}
	.align 2, 0
_020DEB44: .word _021E4220
	arm_func_end WMi_GetStatusAddress

	arm_func_start WM_GetAID
WM_GetAID: ; 0x020DEB48
	stmdb sp!, {r4, lr}
	bl OS_DisableInterrupts
	ldr r1, _020DEB74 ; =_021E4220
	ldr r1, [r1, #4]
	cmp r1, #0
	addne r1, r1, #0x100
	ldrneh r4, [r1, #0x50]
	moveq r4, #0
	bl OS_RestoreInterrupts
	mov r0, r4
	ldmia sp!, {r4, pc}
	.align 2, 0
_020DEB74: .word _021E4220
	arm_func_end WM_GetAID

	arm_func_start WM_GetConnectedAIDs
WM_GetConnectedAIDs: ; 0x020DEB78
	stmdb sp!, {r4, lr}
	bl OS_DisableInterrupts
	ldr r1, _020DEBA4 ; =_021E4220
	ldr r1, [r1, #4]
	cmp r1, #0
	ldrne r4, [r1, #0x14c]
	moveq r4, #0
	bl OS_RestoreInterrupts
	mov r0, r4, lsl #0x10
	mov r0, r0, lsr #0x10
	ldmia sp!, {r4, pc}
	.align 2, 0
_020DEBA4: .word _021E4220
	arm_func_end WM_GetConnectedAIDs

	arm_func_start WM_SetIndCallback
WM_SetIndCallback: ; 0x020DEBA8
	stmdb sp!, {r4, r5, r6, lr}
	mov r6, r0
	bl OS_DisableInterrupts
	mov r5, r0
	bl WMi_CheckInitialized
	movs r4, r0
	beq _020DEBD4
	mov r0, r5
	bl OS_RestoreInterrupts
	mov r0, r4
	ldmia sp!, {r4, r5, r6, pc}
_020DEBD4:
	bl WMi_GetSystemWork
	str r6, [r0, #0xc8]
	mov r0, r5
	bl OS_RestoreInterrupts
	mov r0, #0
	ldmia sp!, {r4, r5, r6, pc}
	arm_func_end WM_SetIndCallback

	arm_func_start WM_SetPortCallback
WM_SetPortCallback: ; 0x020DEBEC
	stmdb sp!, {r3, r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0x44
	movs r5, r1
	mov r6, r0
	mov r4, r2
	beq _020DEC54
	add r0, sp, #0
	mov r1, #0
	mov r2, #0x44
	bl MI_CpuFill8
	mov r3, #0
	ldr r1, _020DECC0 ; =0x0000FFFF
	mov r7, #0x82
	mov r2, #0x19
	add r0, sp, #0x14
	strh r7, [sp]
	strh r3, [sp, #2]
	strh r2, [sp, #4]
	strh r6, [sp, #6]
	str r3, [sp, #8]
	str r3, [sp, #0xc]
	strh r3, [sp, #0x10]
	strh r1, [sp, #0x1a]
	str r4, [sp, #0x1c]
	strh r3, [sp, #0x12]
	bl OS_GetMacAddress
_020DEC54:
	bl OS_DisableInterrupts
	mov r8, r0
	bl WMi_CheckInitialized
	movs r7, r0
	beq _020DEC7C
	mov r0, r8
	bl OS_RestoreInterrupts
	add sp, sp, #0x44
	mov r0, r7
	ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
_020DEC7C:
	bl WMi_GetSystemWork
	add r0, r0, r6, lsl #2
	str r5, [r0, #0xcc]
	str r4, [r0, #0x10c]
	cmp r5, #0
	beq _020DECAC
	bl WM_GetConnectedAIDs
	strh r0, [sp, #0x22]
	bl WM_GetAID
	strh r0, [sp, #0x20]
	add r0, sp, #0
	blx r5
_020DECAC:
	mov r0, r8
	bl OS_RestoreInterrupts
	mov r0, #0
	add sp, sp, #0x44
	ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
	.align 2, 0
_020DECC0: .word 0x0000FFFF
	arm_func_end WM_SetPortCallback

	arm_func_start WM_ReadStatus
WM_ReadStatus: ; 0x020DECC4
	stmdb sp!, {r3, r4, r5, lr}
	mov r5, r0
	bl WMi_GetSystemWork
	mov r4, r0
	bl WMi_CheckInitialized
	cmp r0, #0
	ldmneia sp!, {r3, r4, r5, pc}
	cmp r5, #0
	moveq r0, #6
	ldmeqia sp!, {r3, r4, r5, pc}
	ldr r0, [r4, #4]
	mov r1, #0x7d0
	bl DC_InvalidateRange
	ldr r0, [r4, #4]
	mov r1, r5
	mov r2, #0x7d0
	bl MIi_CpuCopyFast
	mov r0, #0
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end WM_ReadStatus

	arm_func_start WM_GetMPSendBufferSize
WM_GetMPSendBufferSize: ; 0x020DED10
	stmdb sp!, {r4, lr}
	bl WMi_GetSystemWork
	mov r4, r0
	mov r0, #2
	mov r1, #7
	mov r2, #8
	bl WMi_CheckStateEx
	cmp r0, #0
	movne r0, #0
	ldmneia sp!, {r4, pc}
	ldr r0, [r4, #4]
	mov r1, #4
	add r0, r0, #0xc
	bl DC_InvalidateRange
	ldr r1, [r4, #4]
	ldr r0, [r1, #0xc]
	cmp r0, #1
	moveq r0, #0
	ldmeqia sp!, {r4, pc}
	add r0, r1, #0x3c
	mov r1, #4
	bl DC_InvalidateRange
	ldr r0, [r4, #4]
	ldrh r0, [r0, #0x3c]
	add r0, r0, #0x1f
	bic r0, r0, #0x1f
	ldmia sp!, {r4, pc}
	arm_func_end WM_GetMPSendBufferSize

	arm_func_start WM_GetMPReceiveBufferSize
WM_GetMPReceiveBufferSize: ; 0x020DED7C
	stmdb sp!, {r3, r4, r5, lr}
	bl WMi_GetSystemWork
	mov r4, r0
	mov r0, #2
	mov r1, #7
	mov r2, #8
	bl WMi_CheckStateEx
	cmp r0, #0
	movne r0, #0
	ldmneia sp!, {r3, r4, r5, pc}
	ldr r0, [r4, #4]
	mov r1, #4
	add r0, r0, #0xc
	bl DC_InvalidateRange
	ldr r1, [r4, #4]
	ldr r0, [r1, #0xc]
	cmp r0, #1
	moveq r0, #0
	ldmeqia sp!, {r3, r4, r5, pc}
	add r0, r1, #0x188
	mov r1, #2
	bl DC_InvalidateRange
	ldr r1, [r4, #4]
	add r0, r1, #0x100
	ldrh r0, [r0, #0x88]
	cmp r0, #0
	moveq r5, #1
	add r0, r1, #0x3e
	mov r1, #2
	movne r5, #0
	bl DC_InvalidateRange
	ldr r0, [r4, #4]
	cmp r5, #1
	ldrh r5, [r0, #0x3e]
	addne r0, r5, #0x51
	bicne r0, r0, #0x1f
	movne r0, r0, lsl #1
	ldmneia sp!, {r3, r4, r5, pc}
	add r0, r0, #0xf8
	mov r1, #2
	bl DC_InvalidateRange
	ldr r0, [r4, #4]
	add r1, r5, #0xc
	ldrh r0, [r0, #0xf8]
	mul r0, r1, r0
	add r0, r0, #0x29
	bic r0, r0, #0x1f
	mov r0, r0, lsl #1
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end WM_GetMPReceiveBufferSize

	arm_func_start WM_ReadMPData
WM_ReadMPData: ; 0x020DEE40
	stmdb sp!, {r3, r4, r5, r6, lr}
	sub sp, sp, #0x3c
	mov r5, r0
	mov r4, r1
	bl WMi_GetSystemWork
	mov r6, r0
	bl WMi_CheckInitialized
	cmp r0, #0
	addne sp, sp, #0x3c
	movne r0, #0
	ldmneia sp!, {r3, r4, r5, r6, pc}
	cmp r4, #1
	blo _020DEE7C
	cmp r4, #0xf
	bls _020DEE88
_020DEE7C:
	add sp, sp, #0x3c
	mov r0, #0
	ldmia sp!, {r3, r4, r5, r6, pc}
_020DEE88:
	ldr r0, [r6, #4]
	mov r1, #2
	add r0, r0, #0x82
	add r0, r0, #0x100
	bl DC_InvalidateRange
	ldr r0, [r6, #4]
	mov r1, #1
	add r0, r0, #0x100
	ldrh r0, [r0, #0x82]
	tst r0, r1, lsl r4
	addeq sp, sp, #0x3c
	moveq r0, #0
	ldmeqia sp!, {r3, r4, r5, r6, pc}
	ldrh r0, [r5, #4]
	cmp r0, #0
	addeq sp, sp, #0x3c
	moveq r0, #0
	ldmeqia sp!, {r3, r4, r5, r6, pc}
	add r0, r5, #0xa
	str r0, [sp]
	mov r3, #0
	add r2, sp, #0
_020DEEE0:
	ldr r0, [r2, r3, lsl #2]
	ldrh r1, [r0, #4]
	cmp r4, r1
	addeq sp, sp, #0x3c
	ldmeqia sp!, {r3, r4, r5, r6, pc}
	add r3, r3, #1
	add r0, r2, r3, lsl #2
	ldrh r1, [r5, #6]
	ldr r0, [r0, #-4]
	add r0, r1, r0
	str r0, [r2, r3, lsl #2]
	ldrh r0, [r5, #4]
	cmp r3, r0
	blt _020DEEE0
	mov r0, #0
	add sp, sp, #0x3c
	ldmia sp!, {r3, r4, r5, r6, pc}
	arm_func_end WM_ReadMPData

	arm_func_start WM_GetAllowedChannel
WM_GetAllowedChannel: ; 0x020DEF24
	stmdb sp!, {r3, lr}
	bl WMi_CheckInitialized
	cmp r0, #0
	movne r0, #0x8000
	ldreq r0, _020DEF40 ; =0x027FFCFA
	ldreqh r0, [r0]
	ldmia sp!, {r3, pc}
	.align 2, 0
_020DEF40: .word 0x027FFCFA
	arm_func_end WM_GetAllowedChannel

	arm_func_start WM_GetLinkLevel
WM_GetLinkLevel: ; 0x020DEF44
	stmdb sp!, {r4, lr}
	bl WMi_GetSystemWork
	mov r4, r0
	bl WMi_CheckInitialized
	cmp r0, #0
	movne r0, #0
	ldmneia sp!, {r4, pc}
	ldr r0, [r4, #4]
	mov r1, #2
	bl DC_InvalidateRange
	ldr r1, [r4, #4]
	ldrh r0, [r1]
	cmp r0, #9
	beq _020DEF8C
	cmp r0, #0xa
	cmpne r0, #0xb
	beq _020DEFB4
	b _020DEFCC
_020DEF8C:
	add r0, r1, #0x82
	add r0, r0, #0x100
	mov r1, #2
	bl DC_InvalidateRange
	ldr r1, [r4, #4]
	add r0, r1, #0x100
	ldrh r0, [r0, #0x82]
	cmp r0, #0
	moveq r0, #0
	ldmeqia sp!, {r4, pc}
_020DEFB4:
	add r0, r1, #0xbc
	mov r1, #2
	bl DC_InvalidateRange
	ldr r0, [r4, #4]
	ldrh r0, [r0, #0xbc]
	ldmia sp!, {r4, pc}
_020DEFCC:
	mov r0, #0
	ldmia sp!, {r4, pc}
	arm_func_end WM_GetLinkLevel

	arm_func_start WM_GetDispersionBeaconPeriod
WM_GetDispersionBeaconPeriod: ; 0x020DEFD4
	stmdb sp!, {r3, lr}
	sub sp, sp, #8
	add r0, sp, #0
	bl OS_GetMacAddress
	mov r2, #0
	add r3, sp, #0
	mov r1, r2
_020DEFF0:
	ldrb r0, [r3], #1
	add r2, r2, #1
	cmp r2, #6
	add r0, r1, r0
	mov r0, r0, lsl #0x10
	mov r1, r0, lsr #0x10
	blt _020DEFF0
	ldr r0, _020DF05C ; =0x027FFC3C
	ldr ip, _020DF060 ; =0x66666667
	ldr r0, [r0]
	mov r3, #0x14
	add r0, r1, r0
	mov r0, r0, lsl #0x10
	mov r0, r0, lsr #0x10
	rsb r0, r0, r0, lsl #3
	mov r0, r0, lsl #0x10
	mov r2, r0, lsr #0x10
	mov r1, r2, lsr #0x1f
	smull r2, lr, ip, r2
	add lr, r1, lr, asr #3
	smull r1, r2, r3, lr
	rsb lr, r1, r0, lsr #16
	add r0, lr, #0xc8
	mov r0, r0, lsl #0x10
	mov r0, r0, lsr #0x10
	add sp, sp, #8
	ldmia sp!, {r3, pc}
	.align 2, 0
_020DF05C: .word 0x027FFC3C
_020DF060: .word 0x66666667
	arm_func_end WM_GetDispersionBeaconPeriod

	arm_func_start WM_GetDispersionScanPeriod
WM_GetDispersionScanPeriod: ; 0x020DF064
	stmdb sp!, {r3, lr}
	sub sp, sp, #8
	add r0, sp, #0
	bl OS_GetMacAddress
	mov r1, #0
	add r3, sp, #0
	mov r2, r1
_020DF080:
	ldrb r0, [r3], #1
	add r1, r1, #1
	cmp r1, #6
	add r0, r2, r0
	mov r0, r0, lsl #0x10
	mov r2, r0, lsr #0x10
	blt _020DF080
	ldr r0, _020DF0F0 ; =0x027FFC3C
	mov r1, #0xd
	ldr r0, [r0]
	ldr r3, _020DF0F4 ; =0x66666667
	add r0, r2, r0
	mov r0, r0, lsl #0x10
	mov r2, r0, lsr #0x10
	mul r0, r2, r1
	mov r0, r0, lsl #0x10
	mov r2, r0, lsr #0x10
	mov r1, r2, lsr #0x1f
	smull r2, ip, r3, r2
	add ip, r1, ip, asr #2
	mov r3, #0xa
	smull r1, r2, r3, ip
	rsb ip, r1, r0, lsr #16
	add r0, ip, #0x1e
	mov r0, r0, lsl #0x10
	mov r0, r0, lsr #0x10
	add sp, sp, #8
	ldmia sp!, {r3, pc}
	.align 2, 0
_020DF0F0: .word 0x027FFC3C
_020DF0F4: .word 0x66666667
	arm_func_end WM_GetDispersionScanPeriod

	arm_func_start WM_GetOtherElements
WM_GetOtherElements: ; 0x020DF0F8
	stmdb sp!, {r3, r4, r5, r6, lr}
	sub sp, sp, #0x84
	ldrh r2, [r1, #0x3c]
	mov lr, r0
	cmp r2, #0
	beq _020DF140
	mov r0, #0
	add r5, sp, #0
	strb r0, [sp]
	mov r4, #8
_020DF120:
	ldmia r5!, {r0, r1, r2, r3}
	stmia lr!, {r0, r1, r2, r3}
	subs r4, r4, #1
	bne _020DF120
	ldr r0, [r5]
	add sp, sp, #0x84
	str r0, [lr]
	ldmia sp!, {r3, r4, r5, r6, pc}
_020DF140:
	ldrh r0, [r1, #0x3e]
	strb r0, [sp]
	ands r0, r0, #0xff
	bne _020DF178
	add r5, sp, #0
	mov r4, #8
_020DF158:
	ldmia r5!, {r0, r1, r2, r3}
	stmia lr!, {r0, r1, r2, r3}
	subs r4, r4, #1
	bne _020DF158
	ldr r0, [r5]
	add sp, sp, #0x84
	str r0, [lr]
	ldmia sp!, {r3, r4, r5, r6, pc}
_020DF178:
	cmp r0, #0x10
	movhi r0, #0x10
	strhib r0, [sp]
	ldrh r2, [r1]
	ldrb r0, [sp]
	mov r3, #0
	mov r2, r2, lsl #1
	sub r4, r2, #0x40
	cmp r0, #0
	add r0, r1, #0x40
	mov r2, r3
	and r1, r4, #0xff
	ble _020DF228
	add ip, sp, #0
_020DF1B0:
	ldrb r5, [r0]
	add r6, ip, r2, lsl #3
	add r4, r0, #2
	strb r5, [r6, #4]
	ldrb r5, [r0, #1]
	strb r5, [r6, #5]
	str r4, [r6, #8]
	ldrb r4, [r6, #5]
	add r4, r4, #2
	and r5, r4, #0xff
	add r3, r3, r5
	and r3, r3, #0xff
	cmp r3, r1
	bls _020DF214
	mov r0, #0
	strb r0, [sp]
	mov r4, #8
_020DF1F4:
	ldmia ip!, {r0, r1, r2, r3}
	stmia lr!, {r0, r1, r2, r3}
	subs r4, r4, #1
	bne _020DF1F4
	ldr r0, [ip]
	add sp, sp, #0x84
	str r0, [lr]
	ldmia sp!, {r3, r4, r5, r6, pc}
_020DF214:
	ldrb r4, [sp]
	add r2, r2, #1
	add r0, r0, r5
	cmp r2, r4
	blt _020DF1B0
_020DF228:
	add r4, sp, #0
	mov ip, #8
_020DF230:
	ldmia r4!, {r0, r1, r2, r3}
	stmia lr!, {r0, r1, r2, r3}
	subs ip, ip, #1
	bne _020DF230
	ldr r0, [r4]
	str r0, [lr]
	add sp, sp, #0x84
	ldmia sp!, {r3, r4, r5, r6, pc}
	arm_func_end WM_GetOtherElements

	arm_func_start WM_GetNextTgid
WM_GetNextTgid: ; 0x020DF250
	stmdb sp!, {lr}
	sub sp, sp, #0xc
	ldr r0, _020DF2C0 ; =_02110FC0
	ldr r0, [r0]
	cmp r0, #0x10000
	bne _020DF298
	bl RTC_Init
	add r0, sp, #0
	bl RTC_GetTime
	cmp r0, #0
	bne _020DF298
	ldr r2, [sp, #8]
	ldr r0, [sp, #4]
	ldr r1, _020DF2C0 ; =_02110FC0
	add r0, r2, r0, lsl #8
	mov r0, r0, lsl #0x10
	mov r0, r0, lsr #0x10
	str r0, [r1]
_020DF298:
	ldr r1, _020DF2C0 ; =_02110FC0
	ldr r0, [r1]
	add r0, r0, #1
	mov r0, r0, lsl #0x10
	mov r2, r0, lsr #0x10
	mov r0, r2, lsl #0x10
	str r2, [r1]
	mov r0, r0, lsr #0x10
	add sp, sp, #0xc
	ldmia sp!, {pc}
	.align 2, 0
_020DF2C0: .word _02110FC0
	arm_func_end WM_GetNextTgid

	arm_func_start WM_Enable
WM_Enable: ; 0x020DF2C4
	ldr ip, _020DF2D0 ; =WMi_EnableEx
	mov r1, #0
	bx ip
	.align 2, 0
_020DF2D0: .word WMi_EnableEx
	arm_func_end WM_Enable

	arm_func_start WMi_EnableEx
WMi_EnableEx: ; 0x020DF2D4
	stmdb sp!, {r3, r4, r5, lr}
	sub sp, sp, #8
	mov r5, r0
	mov r4, r1
	mov r0, #1
	mov r1, #0
	bl WMi_CheckStateEx
	cmp r0, #0
	addne sp, sp, #8
	ldmneia sp!, {r3, r4, r5, pc}
	mov r1, r5
	mov r0, #3
	bl WMi_SetCallbackTable
	bl WMi_GetSystemWork
	mov r3, r0
	ldr r1, [r3, #0x10]
	mov r0, #3
	stmia sp, {r1, r4}
	mov r1, #4
	ldmia r3, {r2, r3}
	bl WMi_SendCommand
	cmp r0, #0
	moveq r0, #2
	add sp, sp, #8
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end WMi_EnableEx

	arm_func_start WM_Disable
WM_Disable: ; 0x020DF338
	stmdb sp!, {r4, lr}
	mov r4, r0
	mov r0, #1
	mov r1, r0
	bl WMi_CheckStateEx
	cmp r0, #0
	ldmneia sp!, {r4, pc}
	mov r1, r4
	mov r0, #4
	bl WMi_SetCallbackTable
	mov r0, #4
	mov r1, #0
	bl WMi_SendCommand
	cmp r0, #0
	moveq r0, #2
	ldmia sp!, {r4, pc}
	arm_func_end WM_Disable

	arm_func_start WM_PowerOn
WM_PowerOn: ; 0x020DF378
	stmdb sp!, {r4, lr}
	mov r4, r0
	mov r0, #1
	mov r1, r0
	bl WMi_CheckStateEx
	cmp r0, #0
	ldmneia sp!, {r4, pc}
	mov r1, r4
	mov r0, #5
	bl WMi_SetCallbackTable
	mov r0, #5
	mov r1, #0
	bl WMi_SendCommand
	cmp r0, #0
	moveq r0, #2
	ldmia sp!, {r4, pc}
	arm_func_end WM_PowerOn

	arm_func_start WM_PowerOff
WM_PowerOff: ; 0x020DF3B8
	stmdb sp!, {r4, lr}
	mov r4, r0
	mov r0, #1
	mov r1, #2
	bl WMi_CheckStateEx
	cmp r0, #0
	ldmneia sp!, {r4, pc}
	mov r1, r4
	mov r0, #6
	bl WMi_SetCallbackTable
	mov r0, #6
	mov r1, #0
	bl WMi_SendCommand
	cmp r0, #0
	moveq r0, #2
	ldmia sp!, {r4, pc}
	arm_func_end WM_PowerOff

	arm_func_start WM_Initialize
WM_Initialize: ; 0x020DF3F8
	ldr ip, _020DF404 ; =WMi_InitializeEx
	mov r3, #0
	bx ip
	.align 2, 0
_020DF404: .word WMi_InitializeEx
	arm_func_end WM_Initialize

	arm_func_start WM_InitializeForListening
WM_InitializeForListening: ; 0x020DF408
	ldr ip, _020DF41C ; =WMi_InitializeEx
	cmp r3, #0
	mov r3, #1
	orreq r3, r3, #2
	bx ip
	.align 2, 0
_020DF41C: .word WMi_InitializeEx
	arm_func_end WM_InitializeForListening

	arm_func_start WMi_InitializeEx
WMi_InitializeEx: ; 0x020DF420
	stmdb sp!, {r3, r4, r5, lr}
	sub sp, sp, #8
	mov r5, r1
	mov r1, r2
	mov r4, r3
	bl WM_Init
	cmp r0, #0
	addne sp, sp, #8
	ldmneia sp!, {r3, r4, r5, pc}
	mov r1, r5
	mov r0, #0
	bl WMi_SetCallbackTable
	bl WMi_GetSystemWork
	mov r3, r0
	ldr r1, [r3, #0x10]
	mov r0, #0
	stmia sp, {r1, r4}
	mov r1, #4
	ldmia r3, {r2, r3}
	bl WMi_SendCommand
	cmp r0, #0
	moveq r0, #2
	add sp, sp, #8
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end WMi_InitializeEx

	arm_func_start WM_Reset
WM_Reset: ; 0x020DF480
	stmdb sp!, {r4, lr}
	mov r4, r0
	bl WMi_CheckIdle
	cmp r0, #0
	ldmneia sp!, {r4, pc}
	mov r1, r4
	mov r0, #1
	bl WMi_SetCallbackTable
	mov r0, #1
	mov r1, #0
	bl WMi_SendCommand
	cmp r0, #0
	moveq r0, #2
	ldmia sp!, {r4, pc}
	arm_func_end WM_Reset

	arm_func_start WM_End
WM_End: ; 0x020DF4B8
	stmdb sp!, {r4, lr}
	mov r4, r0
	mov r0, #1
	mov r1, #2
	bl WMi_CheckStateEx
	cmp r0, #0
	ldmneia sp!, {r4, pc}
	mov r1, r4
	mov r0, #2
	bl WMi_SetCallbackTable
	mov r0, #2
	mov r1, #0
	bl WMi_SendCommand
	cmp r0, #0
	moveq r0, #2
	ldmia sp!, {r4, pc}
	arm_func_end WM_End

	arm_func_start WM_SetParentParameter
WM_SetParentParameter: ; 0x020DF4F8
	stmdb sp!, {r3, r4, r5, lr}
	mov r5, r0
	mov r4, r1
	mov r0, #1
	mov r1, #2
	bl WMi_CheckStateEx
	cmp r0, #0
	ldmneia sp!, {r3, r4, r5, pc}
	cmp r4, #0
	moveq r0, #6
	ldmeqia sp!, {r3, r4, r5, pc}
	ldrh r0, [r4, #4]
	cmp r0, #0
	beq _020DF540
	ldr r0, [r4]
	cmp r0, #0
	moveq r0, #6
	ldmeqia sp!, {r3, r4, r5, pc}
_020DF540:
	ldrh r1, [r4, #0x14]
	ldrh r0, [r4, #0x34]
	cmp r1, #0
	movne r2, #0x2a
	moveq r2, #0
	add r0, r0, r2
	cmp r0, #0x200
	bgt _020DF57C
	ldrh r0, [r4, #0x36]
	cmp r1, #0
	movne r1, #6
	moveq r1, #0
	add r0, r0, r1
	cmp r0, #0x200
	ble _020DF584
_020DF57C:
	mov r0, #6
	ldmia sp!, {r3, r4, r5, pc}
_020DF584:
	mov r0, r4
	bl WmCheckParentParameter
	mov r1, r5
	mov r0, #7
	bl WMi_SetCallbackTable
	mov r0, r4
	mov r1, #0x40
	bl DC_StoreRange
	ldrh r1, [r4, #4]
	cmp r1, #0
	beq _020DF5B8
	ldr r0, [r4]
	bl DC_StoreRange
_020DF5B8:
	mov r2, r4
	mov r0, #7
	mov r1, #1
	bl WMi_SendCommand
	cmp r0, #0
	moveq r0, #2
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end WM_SetParentParameter

	arm_func_start WmCheckParentParameter
WmCheckParentParameter: ; 0x020DF5D4
	ldrh r1, [r0, #4]
	cmp r1, #0x70
	movhi r0, #0
	bxhi lr
	ldrh r1, [r0, #0x18]
	cmp r1, #0xa
	blo _020DF5F8
	cmp r1, #0x3e8
	bls _020DF600
_020DF5F8:
	mov r0, #0
	bx lr
_020DF600:
	ldrh r0, [r0, #0x32]
	cmp r0, #1
	blo _020DF614
	cmp r0, #0xe
	bls _020DF61C
_020DF614:
	mov r0, #0
	bx lr
_020DF61C:
	mov r0, #1
	bx lr
	arm_func_end WmCheckParentParameter

	arm_func_start WMi_StartParentEx
WMi_StartParentEx: ; 0x020DF624
	stmdb sp!, {r3, r4, r5, lr}
	mov r5, r0
	mov r4, r1
	mov r0, #1
	mov r1, #2
	bl WMi_CheckStateEx
	cmp r0, #0
	ldmneia sp!, {r3, r4, r5, pc}
	bl WMi_GetSystemWork
	add r1, r0, #0x100
	mov r2, #0
	strh r2, [r1, #0x50]
	str r2, [r0, #0x14c]
	mov r1, r5
	mov r0, #8
	bl WMi_SetCallbackTable
	mov r2, r4
	mov r0, #8
	mov r1, #1
	bl WMi_SendCommand
	cmp r0, #0
	moveq r0, #2
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end WMi_StartParentEx

	arm_func_start WM_StartParent
WM_StartParent: ; 0x020DF680
	ldr ip, _020DF68C ; =WMi_StartParentEx
	mov r1, #1
	bx ip
	.align 2, 0
_020DF68C: .word WMi_StartParentEx
	arm_func_end WM_StartParent

	arm_func_start WM_EndParent
WM_EndParent: ; 0x020DF690
	stmdb sp!, {r4, lr}
	mov r4, r0
	mov r0, #1
	mov r1, #7
	bl WMi_CheckStateEx
	cmp r0, #0
	ldmneia sp!, {r4, pc}
	mov r1, r4
	mov r0, #9
	bl WMi_SetCallbackTable
	mov r0, #9
	mov r1, #0
	bl WMi_SendCommand
	cmp r0, #0
	moveq r0, #2
	ldmia sp!, {r4, pc}
	arm_func_end WM_EndParent

	arm_func_start WM_StartScan
WM_StartScan: ; 0x020DF6D0
	stmdb sp!, {r3, r4, r5, lr}
	sub sp, sp, #0x10
	mov r5, r0
	mov r0, #3
	mov r4, r1
	mov r2, r0
	mov r1, #2
	mov r3, #5
	bl WMi_CheckStateEx
	cmp r0, #0
	addne sp, sp, #0x10
	ldmneia sp!, {r3, r4, r5, pc}
	cmp r4, #0
	addeq sp, sp, #0x10
	moveq r0, #6
	ldmeqia sp!, {r3, r4, r5, pc}
	ldr r0, [r4]
	cmp r0, #0
	addeq sp, sp, #0x10
	moveq r0, #6
	ldmeqia sp!, {r3, r4, r5, pc}
	ldrh r0, [r4, #4]
	cmp r0, #1
	blo _020DF738
	cmp r0, #0xe
	bls _020DF744
_020DF738:
	add sp, sp, #0x10
	mov r0, #6
	ldmia sp!, {r3, r4, r5, pc}
_020DF744:
	mov r1, r5
	mov r0, #0xa
	bl WMi_SetCallbackTable
	mov r0, #0xa
	strh r0, [sp]
	ldrh r2, [r4, #4]
	add r0, sp, #0
	mov r1, #0x10
	strh r2, [sp, #2]
	ldr r2, [r4]
	str r2, [sp, #4]
	ldrh r2, [r4, #6]
	strh r2, [sp, #8]
	ldrb r2, [r4, #8]
	strb r2, [sp, #0xa]
	ldrb r2, [r4, #9]
	strb r2, [sp, #0xb]
	ldrb r2, [r4, #0xa]
	strb r2, [sp, #0xc]
	ldrb r2, [r4, #0xb]
	strb r2, [sp, #0xd]
	ldrb r2, [r4, #0xc]
	strb r2, [sp, #0xe]
	ldrb r2, [r4, #0xd]
	strb r2, [sp, #0xf]
	bl WMi_SendCommandDirect
	cmp r0, #0
	moveq r0, #2
	add sp, sp, #0x10
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end WM_StartScan

	arm_func_start WM_StartScanEx
WM_StartScanEx: ; 0x020DF7BC
	stmdb sp!, {r4, r5, lr}
	sub sp, sp, #0x3c
	mov r5, r0
	mov r0, #3
	mov r4, r1
	mov r2, r0
	mov r1, #2
	mov r3, #5
	bl WMi_CheckStateEx
	cmp r0, #0
	addne sp, sp, #0x3c
	ldmneia sp!, {r4, r5, pc}
	cmp r4, #0
	addeq sp, sp, #0x3c
	moveq r0, #6
	ldmeqia sp!, {r4, r5, pc}
	ldr r0, [r4]
	cmp r0, #0
	addeq sp, sp, #0x3c
	moveq r0, #6
	ldmeqia sp!, {r4, r5, pc}
	ldrh r0, [r4, #4]
	cmp r0, #0x400
	addhi sp, sp, #0x3c
	movhi r0, #6
	ldmhiia sp!, {r4, r5, pc}
	ldrh r0, [r4, #0x12]
	cmp r0, #0x20
	addhi sp, sp, #0x3c
	movhi r0, #6
	ldmhiia sp!, {r4, r5, pc}
	ldrh r0, [r4, #0x10]
	cmp r0, #0
	cmpne r0, #1
	cmpne r0, #2
	cmpne r0, #3
	addne sp, sp, #0x3c
	movne r0, #6
	ldmneia sp!, {r4, r5, pc}
	add r0, r0, #0xfe
	add r0, r0, #0xff00
	mov r0, r0, lsl #0x10
	mov r0, r0, lsr #0x10
	cmp r0, #1
	bhi _020DF884
	ldrh r0, [r4, #0x34]
	cmp r0, #0x20
	addhi sp, sp, #0x3c
	movhi r0, #6
	ldmhiia sp!, {r4, r5, pc}
_020DF884:
	mov r1, r5
	mov r0, #0x26
	bl WMi_SetCallbackTable
	mov r0, #0x26
	strh r0, [sp]
	ldrh r2, [r4, #6]
	add r1, sp, #0xc
	add r0, r4, #0xa
	strh r2, [sp, #2]
	ldr r3, [r4]
	mov r2, #6
	str r3, [sp, #4]
	ldrh r3, [r4, #4]
	strh r3, [sp, #8]
	ldrh r3, [r4, #8]
	strh r3, [sp, #0xa]
	bl MI_CpuCopy8
	ldrh r2, [r4, #0x10]
	add r1, sp, #0x16
	add r0, r4, #0x14
	strh r2, [sp, #0x12]
	ldrh r3, [r4, #0x34]
	mov r2, #0x20
	strh r3, [sp, #0x36]
	ldrh r3, [r4, #0x12]
	strh r3, [sp, #0x14]
	bl MI_CpuCopy8
	add r0, sp, #0
	mov r1, #0x3c
	bl WMi_SendCommandDirect
	cmp r0, #0
	moveq r0, #2
	add sp, sp, #0x3c
	ldmia sp!, {r4, r5, pc}
	arm_func_end WM_StartScanEx

	arm_func_start WM_EndScan
WM_EndScan: ; 0x020DF90C
	stmdb sp!, {r4, lr}
	mov r4, r0
	mov r0, #1
	mov r1, #5
	bl WMi_CheckStateEx
	cmp r0, #0
	ldmneia sp!, {r4, pc}
	mov r1, r4
	mov r0, #0xb
	bl WMi_SetCallbackTable
	mov r0, #0xb
	mov r1, #0
	bl WMi_SendCommand
	cmp r0, #0
	moveq r0, #2
	ldmia sp!, {r4, pc}
	arm_func_end WM_EndScan

	arm_func_start WM_StartConnectEx
WM_StartConnectEx: ; 0x020DF94C
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	sub sp, sp, #0x28
	mov r7, r0
	mov r6, r1
	mov r0, #1
	mov r1, #2
	mov r5, r2
	mov r4, r3
	bl WMi_CheckStateEx
	cmp r0, #0
	addne sp, sp, #0x28
	ldmneia sp!, {r3, r4, r5, r6, r7, pc}
	cmp r6, #0
	addeq sp, sp, #0x28
	moveq r0, #6
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	ldrh r1, [r6]
	mov r0, r6
	mov r1, r1, lsl #1
	bl DC_StoreRange
	bl WMi_GetSystemWork
	add r1, r0, #0x100
	mov r2, #0
	strh r2, [r1, #0x50]
	str r2, [r0, #0x14c]
	mov r1, r7
	mov r0, #0xc
	bl WMi_SetCallbackTable
	mov r0, #0xc
	strh r0, [sp]
	str r6, [sp, #4]
	cmp r5, #0
	mov r2, #0x18
	beq _020DF9E4
	add r1, sp, #8
	mov r0, r5
	bl MI_CpuCopy8
	b _020DF9F0
_020DF9E4:
	add r0, sp, #8
	mov r1, #0
	bl MI_CpuFill8
_020DF9F0:
	ldrh r2, [sp, #0x40]
	add r0, sp, #0
	mov r1, #0x28
	str r4, [sp, #0x20]
	strh r2, [sp, #0x26]
	bl WMi_SendCommandDirect
	cmp r0, #0
	moveq r0, #2
	add sp, sp, #0x28
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	arm_func_end WM_StartConnectEx

	arm_func_start WM_Disconnect
WM_Disconnect: ; 0x020DFA18
	stmdb sp!, {r4, r5, r6, lr}
	sub sp, sp, #8
	mov r5, r0
	mov r4, r1
	bl WMi_GetSystemWork
	mov r1, #0xa
	mov r6, r0
	str r1, [sp]
	mov ip, #0xb
	mov r0, #5
	mov r1, #7
	mov r2, #9
	mov r3, #8
	str ip, [sp, #4]
	bl WMi_CheckStateEx
	cmp r0, #0
	addne sp, sp, #8
	ldmneia sp!, {r4, r5, r6, pc}
	ldr r1, [r6, #4]
	ldrh r0, [r1]
	cmp r0, #7
	cmpne r0, #9
	bne _020DFAC4
	cmp r4, #1
	blo _020DFA84
	cmp r4, #0xf
	bls _020DFA90
_020DFA84:
	add sp, sp, #8
	mov r0, #6
	ldmia sp!, {r4, r5, r6, pc}
_020DFA90:
	add r0, r1, #0x82
	add r0, r0, #0x100
	mov r1, #2
	bl DC_InvalidateRange
	ldr r0, [r6, #4]
	mov r1, #1
	add r0, r0, #0x100
	ldrh r0, [r0, #0x82]
	tst r0, r1, lsl r4
	bne _020DFAD4
	add sp, sp, #8
	mov r0, #7
	ldmia sp!, {r4, r5, r6, pc}
_020DFAC4:
	cmp r4, #0
	addne sp, sp, #8
	movne r0, #6
	ldmneia sp!, {r4, r5, r6, pc}
_020DFAD4:
	mov r1, r5
	mov r0, #0xd
	bl WMi_SetCallbackTable
	mov r1, #1
	mov r2, r1, lsl r4
	mov r0, #0xd
	bl WMi_SendCommand
	cmp r0, #0
	moveq r0, #2
	add sp, sp, #8
	ldmia sp!, {r4, r5, r6, pc}
	arm_func_end WM_Disconnect

	arm_func_start WMi_StartMP
WMi_StartMP: ; 0x020DFB00
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, lr}
	sub sp, sp, #0x40
	mov r8, r0
	mov r7, r1
	mov r6, r2
	mov r5, r3
	bl WMi_GetSystemWork
	ldr r4, [r0, #4]
	mov r0, #2
	mov r1, #7
	mov r2, #8
	bl WMi_CheckStateEx
	cmp r0, #0
	addne sp, sp, #0x40
	ldmneia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	add r0, r4, #0x188
	mov r1, #2
	bl DC_InvalidateRange
	add r0, r4, #0xc6
	mov r1, #2
	bl DC_InvalidateRange
	add r0, r4, #0x100
	ldrh r0, [r0, #0x88]
	cmp r0, #0
	ldrneh r0, [r4, #0xc6]
	cmpne r0, #1
	addne sp, sp, #0x40
	movne r0, #3
	ldmneia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	add r0, r4, #0xc
	mov r1, #4
	bl DC_InvalidateRange
	ldr r0, [r4, #0xc]
	cmp r0, #1
	addeq sp, sp, #0x40
	moveq r0, #3
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	tst r6, #0x3f
	addne sp, sp, #0x40
	movne r0, #6
	ldmneia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	ldrh sb, [sp, #0x60]
	tst sb, #0x1f
	addne sp, sp, #0x40
	movne r0, #6
	ldmneia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	add r0, r4, #0x9c
	mov r1, #2
	bl DC_InvalidateRange
	ldrh r0, [r4, #0x9c]
	cmp r0, #0
	bne _020DFBF8
	bl WM_GetMPReceiveBufferSize
	cmp r6, r0
	addlt sp, sp, #0x40
	movlt r0, #6
	ldmltia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	bl WM_GetMPSendBufferSize
	cmp sb, r0
	addlt sp, sp, #0x40
	movlt r0, #6
	ldmltia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
_020DFBF8:
	mov r1, r8
	mov r0, #0xe
	bl WMi_SetCallbackTable
	add r1, sp, #0
	mov r0, #0
	mov r2, #0x40
	bl MIi_CpuClear32
	mov r4, r6, lsr #1
	ldrh r3, [sp, #0x60]
	mov r6, #0xe
	add r1, sp, #0x14
	mov r0, #0
	mov r2, #0x1c
	strh r6, [sp]
	str r7, [sp, #4]
	str r4, [sp, #8]
	str r5, [sp, #0xc]
	str r3, [sp, #0x10]
	bl MIi_CpuClear32
	ldr r0, [sp, #0x64]
	add r1, sp, #0x30
	mov r2, #0x10
	bl MIi_CpuCopy32
	add r0, sp, #0
	mov r1, #0x40
	bl WMi_SendCommandDirect
	cmp r0, #0
	moveq r0, #2
	add sp, sp, #0x40
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	arm_func_end WMi_StartMP

	arm_func_start WM_StartMPEx
WM_StartMPEx: ; 0x020DFC70
	stmdb sp!, {r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0x18
	mov r7, r1
	mov r8, r0
	mov r6, r2
	add r1, sp, #8
	mov r0, #0
	mov r2, #0x10
	mov r5, r3
	ldr r4, [sp, #0x34]
	bl MIi_CpuClear32
	ldr r1, [sp, #0x48]
	ldrh ip, [sp, #0x38]
	ldr r0, [sp, #0x44]
	ldr lr, _020DFD10 ; =0x00001E03
	ldr r3, [sp, #0x3c]
	ldr r2, [sp, #0x40]
	strb r1, [sp, #0x16]
	cmp r0, #0
	cmpne r4, #0
	strb r3, [sp, #0x14]
	strb r2, [sp, #0x15]
	ldrh r1, [sp, #0x30]
	str lr, [sp, #8]
	orrne r0, lr, #4
	strne r0, [sp, #8]
	strh ip, [sp, #0x12]
	strh r4, [sp, #0xc]
	strh r4, [sp, #0xe]
	strneh r4, [sp, #0x10]
	str r1, [sp]
	add r4, sp, #8
	mov r0, r8
	mov r1, r7
	mov r2, r6
	mov r3, r5
	str r4, [sp, #4]
	bl WMi_StartMP
	add sp, sp, #0x18
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	.align 2, 0
_020DFD10: .word 0x00001E03
	arm_func_end WM_StartMPEx

	arm_func_start WM_StartMP
WM_StartMP: ; 0x020DFD14
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	sub sp, sp, #0x18
	mov r6, r1
	mov r7, r0
	mov r5, r2
	add r1, sp, #8
	mov r0, #0
	mov r2, #0x10
	mov r4, r3
	bl MIi_CpuClear32
	ldrh ip, [sp, #0x34]
	mov r0, #3
	str r0, [sp, #8]
	ldrh lr, [sp, #0x30]
	strh ip, [sp, #0xc]
	strh ip, [sp, #0xe]
	mov r0, r7
	mov r1, r6
	mov r2, r5
	mov r3, r4
	add ip, sp, #8
	str lr, [sp]
	str ip, [sp, #4]
	bl WMi_StartMP
	add sp, sp, #0x18
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	arm_func_end WM_StartMP

	arm_func_start WM_SetMPDataToPortEx
WM_SetMPDataToPortEx: ; 0x020DFD7C
	stmdb sp!, {r4, r5, r6, r7, r8, sb, lr}
	sub sp, sp, #0x14
	mov sb, r0
	mov r8, r1
	mov r7, r2
	mov r6, r3
	mov r4, #1
	bl WMi_GetSystemWork
	ldr r5, [r0, #4]
	mov r0, #2
	mov r1, #9
	mov r2, #0xa
	bl WMi_CheckStateEx
	cmp r0, #0
	addne sp, sp, #0x14
	ldmneia sp!, {r4, r5, r6, r7, r8, sb, pc}
	add r0, r5, #0x188
	mov r1, #2
	bl DC_InvalidateRange
	add r0, r5, #0x100
	ldrh r0, [r0, #0x88]
	cmp r0, #0
	bne _020DFDFC
	add r0, r5, #0x82
	add r0, r0, #0x100
	mov r1, #2
	bl DC_InvalidateRange
	add r2, r5, #0x100
	add r0, r5, #0x86
	mov r1, #2
	ldrh r4, [r2, #0x82]
	bl DC_InvalidateRange
_020DFDFC:
	cmp r7, #0
	addeq sp, sp, #0x14
	moveq r0, #6
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, pc}
	cmp r4, #0
	addeq sp, sp, #0x14
	moveq r0, #7
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, pc}
	add r0, r5, #0x7c
	mov r1, #2
	bl DC_InvalidateRange
	ldr r0, [r5, #0x7c]
	cmp r7, r0
	addeq sp, sp, #0x14
	moveq r0, #6
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, pc}
	cmp r6, #0x200
	addhi sp, sp, #0x14
	movhi r0, #6
	ldmhiia sp!, {r4, r5, r6, r7, r8, sb, pc}
	cmp r6, #0
	addeq sp, sp, #0x14
	moveq r0, #6
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, pc}
	mov r0, r7
	mov r1, r6
	bl DC_StoreRange
	ldrh r2, [sp, #0x30]
	ldrh r1, [sp, #0x34]
	ldrh r0, [sp, #0x38]
	str r2, [sp]
	str r1, [sp, #4]
	str r0, [sp, #8]
	str sb, [sp, #0xc]
	mov r2, r7
	mov r3, r6
	mov r0, #0xf
	mov r1, #7
	str r8, [sp, #0x10]
	bl WMi_SendCommand
	cmp r0, #0
	moveq r0, #2
	add sp, sp, #0x14
	ldmia sp!, {r4, r5, r6, r7, r8, sb, pc}
	arm_func_end WM_SetMPDataToPortEx

	arm_func_start WM_EndMP
WM_EndMP: ; 0x020DFEAC
	stmdb sp!, {r3, r4, r5, lr}
	mov r5, r0
	bl WMi_GetSystemWork
	mov r4, r0
	mov r0, #2
	mov r1, #9
	mov r2, #0xa
	bl WMi_CheckStateEx
	cmp r0, #0
	ldmneia sp!, {r3, r4, r5, pc}
	ldr r0, [r4, #4]
	mov r1, #4
	add r0, r0, #0xc
	bl DC_InvalidateRange
	ldr r0, [r4, #4]
	ldr r0, [r0, #0xc]
	cmp r0, #0
	moveq r0, #3
	ldmeqia sp!, {r3, r4, r5, pc}
	mov r1, r5
	mov r0, #0x10
	bl WMi_SetCallbackTable
	mov r0, #0x10
	mov r1, #0
	bl WMi_SendCommand
	cmp r0, #0
	moveq r0, #2
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end WM_EndMP

	arm_func_start WM_StartDCF
WM_StartDCF: ; 0x020DFF1C
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	mov r7, r0
	mov r6, r1
	mov r5, r2
	bl WMi_GetSystemWork
	mov r4, r0
	mov r0, #1
	mov r1, #8
	bl WMi_CheckStateEx
	cmp r0, #0
	ldmneia sp!, {r3, r4, r5, r6, r7, pc}
	ldr r0, [r4, #4]
	mov r1, #4
	add r0, r0, #0x10
	bl DC_InvalidateRange
	ldr r0, [r4, #4]
	ldr r0, [r0, #0x10]
	cmp r0, #1
	moveq r0, #3
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	cmp r5, #0x10
	movlo r0, #6
	ldmloia sp!, {r3, r4, r5, r6, r7, pc}
	cmp r6, #0
	moveq r0, #6
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	mov r0, r6
	mov r1, r5
	bl DC_StoreRange
	mov r1, r7
	mov r0, #0x11
	bl WMi_SetCallbackTable
	mov r2, r6
	mov r3, r5
	mov r0, #0x11
	mov r1, #2
	bl WMi_SendCommand
	cmp r0, #0
	moveq r0, #2
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	arm_func_end WM_StartDCF

	arm_func_start WM_SetDCFData
WM_SetDCFData: ; 0x020DFFBC
	stmdb sp!, {r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0x10
	mov r7, r0
	mov r6, r1
	mov r5, r2
	mov r4, r3
	bl WMi_GetSystemWork
	mov r8, r0
	mov r0, #1
	mov r1, #0xb
	bl WMi_CheckStateEx
	cmp r0, #0
	addne sp, sp, #0x10
	ldmneia sp!, {r4, r5, r6, r7, r8, pc}
	ldr r0, [r8, #4]
	mov r1, #4
	add r0, r0, #0x10
	bl DC_InvalidateRange
	ldr r0, [r8, #4]
	ldr r0, [r0, #0x10]
	cmp r0, #0
	addeq sp, sp, #0x10
	moveq r0, #3
	ldmeqia sp!, {r4, r5, r6, r7, r8, pc}
	ldr r0, _020E0084 ; =0x000005E4
	cmp r4, r0
	addhi sp, sp, #0x10
	movhi r0, #6
	ldmhiia sp!, {r4, r5, r6, r7, r8, pc}
	mov r0, r5
	mov r1, r4
	bl DC_StoreRange
	mov r1, r7
	mov r0, #0x12
	bl WMi_SetCallbackTable
	add r1, sp, #8
	mov r0, r6
	mov r2, #6
	bl MI_CpuCopy8
	str r5, [sp]
	str r4, [sp, #4]
	ldr r2, [sp, #8]
	ldr r3, [sp, #0xc]
	mov r0, #0x12
	mov r1, #4
	bl WMi_SendCommand
	cmp r0, #0
	moveq r0, #2
	add sp, sp, #0x10
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	.align 2, 0
_020E0084: .word 0x000005E4
	arm_func_end WM_SetDCFData

	arm_func_start WM_EndDCF
WM_EndDCF: ; 0x020E0088
	stmdb sp!, {r3, r4, r5, lr}
	mov r5, r0
	bl WMi_GetSystemWork
	mov r4, r0
	mov r0, #1
	mov r1, #0xb
	bl WMi_CheckStateEx
	cmp r0, #0
	ldmneia sp!, {r3, r4, r5, pc}
	ldr r0, [r4, #4]
	mov r1, #4
	add r0, r0, #0x10
	bl DC_InvalidateRange
	ldr r0, [r4, #4]
	ldr r0, [r0, #0x10]
	cmp r0, #0
	moveq r0, #3
	ldmeqia sp!, {r3, r4, r5, pc}
	mov r1, r5
	mov r0, #0x13
	bl WMi_SetCallbackTable
	mov r0, #0x13
	mov r1, #0
	bl WMi_SendCommand
	cmp r0, #0
	moveq r0, #2
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end WM_EndDCF

	arm_func_start WM_StartDataSharing
WM_StartDataSharing: ; 0x020E00F4
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0xc
	mov sl, r0
	mov r7, r1
	mov r6, r2
	mov r0, #2
	mov r1, #9
	mov r2, #0xa
	mov r5, r3
	mov r8, #1
	bl WMi_CheckStateEx
	cmp r0, #0
	addne sp, sp, #0xc
	ldmneia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	cmp sl, #0
	addeq sp, sp, #0xc
	moveq r0, #6
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	cmp r7, #0x10
	addhs sp, sp, #0xc
	movhs r0, #6
	ldmhsia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	cmp r6, #0
	addeq sp, sp, #0xc
	moveq r0, #6
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	bl WM_GetAID
	movs r4, r0
	bne _020E0170
	bl WM_GetConnectedAIDs
	mov r8, r0
_020E0170:
	mov r1, sl
	mov r0, #0
	mov r2, #0x820
	bl MIi_CpuClearFast
	add r0, sl, #0x800
	mov r2, #0
	strh r2, [r0, #8]
	strh r2, [r0, #0xa]
	strh r2, [r0, #0xc]
	strh r5, [r0, #0x10]
	strh r7, [r0, #0x16]
	strh r2, [r0, #0xe]
	mov r0, #1
	ldr r1, [sp, #0x30]
	orr r0, r6, r0, lsl r4
	cmp r1, #0
	mov r0, r0, lsl #0x10
	movne r2, #1
	add r1, sl, #0x800
	strh r2, [r1, #0x18]
	mov r0, r0, lsr #0x10
	strh r0, [r1, #0xe]
	bl MATH_CountPopulation
	add r3, sl, #0x800
	mul r1, r5, r0
	strh r0, [r3, #0x12]
	strh r1, [r3, #0x14]
	ldrh r0, [r3, #0x14]
	cmp r0, #0x1fc
	bls _020E01FC
	mov r0, #0
	strh r0, [r3, #0xe]
	add sp, sp, #0xc
	mov r0, #6
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020E01FC:
	add r0, r0, #4
	strh r0, [r3, #0x14]
	mov r0, #1
	strh r0, [r3, #0x1c]
	cmp r4, #0
	bne _020E0314
	orr r4, r8, #1
	mov r2, #0
_020E021C:
	ldrh r1, [r3, #0xe]
	mov r0, r2, lsl #9
	add r2, r2, #1
	and r1, r1, r4
	strh r1, [sl, r0]
	cmp r2, #4
	blt _020E021C
	ldr r1, _020E0338 ; =WmDataSharingReceiveCallback_Parent
	mov r0, r7
	mov r2, sl
	bl WM_SetPortCallback
	mov r7, sl
	mov sb, #0
	add r4, sl, #0x800
	mov r6, #1
	ldr fp, _020E033C ; =WmDataSharingSetDataCallback
	ldr r5, _020E0340 ; =0x0000FFFF
	b _020E02F8
_020E0264:
	ldrh r2, [r4, #8]
	mov r0, fp
	mov r1, sl
	add r2, r2, #1
	and r2, r2, #3
	strh r2, [r4, #8]
	ldrh r3, [r4, #0xe]
	mov r2, r7
	and r3, r3, r8
	mov r3, r3, lsl #0x10
	mov r3, r3, lsr #0x10
	str r3, [sp]
	ldrh r3, [r4, #0x16]
	stmib sp, {r3, r6}
	ldrh r3, [r4, #0x14]
	bl WM_SetMPDataToPortEx
	cmp r0, #7
	bne _020E02CC
	add r0, sl, sb, lsl #1
	add r0, r0, #0x800
	strh r5, [r0]
	ldrh r0, [r4, #0xa]
	add r0, r0, #1
	and r0, r0, #3
	strh r0, [r4, #0xa]
	b _020E02F0
_020E02CC:
	cmp r0, #0
	cmpne r0, #2
	beq _020E02F0
	add r0, sl, #0x800
	mov r1, #5
	strh r1, [r0, #0x1c]
	add sp, sp, #0xc
	mov r0, #1
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020E02F0:
	add r7, r7, #0x200
	add sb, sb, #1
_020E02F8:
	ldrh r0, [r4, #0x18]
	cmp r0, #1
	movne r0, #1
	moveq r0, #2
	cmp sb, r0
	blt _020E0264
	b _020E032C
_020E0314:
	ldr r1, _020E0344 ; =WmDataSharingReceiveCallback_Child
	mov r4, #3
	mov r0, r7
	mov r2, sl
	strh r4, [r3, #0xa]
	bl WM_SetPortCallback
_020E032C:
	mov r0, #0
	add sp, sp, #0xc
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_020E0338: .word WmDataSharingReceiveCallback_Parent
_020E033C: .word WmDataSharingSetDataCallback
_020E0340: .word 0x0000FFFF
_020E0344: .word WmDataSharingReceiveCallback_Child
	arm_func_end WM_StartDataSharing

	arm_func_start WM_EndDataSharing
WM_EndDataSharing: ; 0x020E0348
	stmdb sp!, {r4, lr}
	movs r4, r0
	moveq r0, #6
	ldmeqia sp!, {r4, pc}
	add r0, r4, #0x800
	ldrh r1, [r0, #0xe]
	cmp r1, #0
	moveq r0, #3
	ldmeqia sp!, {r4, pc}
	ldrh r0, [r0, #0x16]
	mov r1, #0
	mov r2, r1
	bl WM_SetPortCallback
	add r1, r4, #0x800
	mov r0, #0
	strh r0, [r1, #0xe]
	strh r0, [r1, #0x1c]
	ldmia sp!, {r4, pc}
	arm_func_end WM_EndDataSharing

	arm_func_start WM_StepDataSharing
WM_StepDataSharing: ; 0x020E0390
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0xc
	mov sl, r0
	mov sb, r1
	mov r8, r2
	mov r0, #2
	mov r1, #9
	mov r2, #0xa
	bl WMi_CheckStateEx
	cmp r0, #0
	addne sp, sp, #0xc
	ldmneia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	cmp sl, #0
	addeq sp, sp, #0xc
	moveq r0, #6
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	cmp sb, #0
	addeq sp, sp, #0xc
	moveq r0, #6
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	cmp r8, #0
	addeq sp, sp, #0xc
	moveq r0, #6
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	bl WM_GetAID
	movs r5, r0
	bne _020E0404
	bl WM_GetConnectedAIDs
	mov r4, r0
_020E0404:
	add r0, sl, #0x800
	ldrh r0, [r0, #0x1c]
	cmp r0, #5
	addeq sp, sp, #0xc
	moveq r0, #1
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	cmp r0, #1
	cmpne r0, #4
	addne sp, sp, #0xc
	movne r0, #3
	ldmneia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	cmp r5, #0
	mov r7, #5
	bne _020E05C8
	mov r5, #0
	mov fp, r5
	cmp r0, #4
	bne _020E04EC
	add ip, sl, #0x800
	mov r2, #1
	strh r2, [ip, #0x1c]
	ldrh r3, [ip, #0xe]
	ldrh r1, [ip, #8]
	ldr r0, _020E06D0 ; =WmDataSharingSetDataCallback
	and r3, r3, r4
	mov r3, r3, lsl #0x10
	mov r3, r3, lsr #0x10
	str r3, [sp]
	ldrh r3, [ip, #0x16]
	add r1, r1, #3
	and r6, r1, #3
	str r3, [sp, #4]
	str r2, [sp, #8]
	ldrh r3, [ip, #0x14]
	mov r1, sl
	add r2, sl, r6, lsl #9
	bl WM_SetMPDataToPortEx
	cmp r0, #7
	bne _020E04C8
	add r0, sl, r6, lsl #1
	ldr r1, _020E06D4 ; =0x0000FFFF
	add r0, r0, #0x800
	strh r1, [r0]
	add r0, sl, #0x800
	ldrh r1, [r0, #0xa]
	add r1, r1, #1
	and r1, r1, #3
	strh r1, [r0, #0xa]
	b _020E04EC
_020E04C8:
	cmp r0, #0
	cmpne r0, #2
	beq _020E04EC
	add r0, sl, #0x800
	mov r1, r7
	strh r1, [r0, #0x1c]
	add sp, sp, #0xc
	mov r0, #1
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020E04EC:
	add r0, sl, #0x800
	ldrh r2, [r0, #0xc]
	ldrh r1, [r0, #0xa]
	cmp r2, r1
	beq _020E0584
	mov r5, r2, lsl #9
	ldrh r3, [sl, r5]
	mov r1, r8
	mov r2, #0x200
	orr r3, r3, #1
	strh r3, [sl, r5]
	ldrh r0, [r0, #0xc]
	add r0, sl, r0, lsl #9
	bl MIi_CpuCopy16
	add r1, sl, #0x800
	ldrh r0, [r1, #0xc]
	mov r5, #1
	mov r7, #0
	add r0, sl, r0, lsl #1
	add r0, r0, #0x800
	ldrh r0, [r0]
	strh r0, [r1, #0x1a]
	ldrh r0, [r1, #0xc]
	add r0, r0, #1
	and r0, r0, #3
	strh r0, [r1, #0xc]
	ldrh r0, [r1, #0x18]
	cmp r0, #0
	bne _020E0580
	cmp r4, #0
	beq _020E0580
	ldrh r0, [r1, #8]
	mov r0, r0, lsl #9
	ldrh r0, [sl, r0]
	cmp r0, #1
	moveq fp, r5
	beq _020E0584
_020E0580:
	mov fp, #0
_020E0584:
	mov r0, sl
	mov r1, #0
	bl WmDataSharingSendDataSet
	cmp r5, #0
	beq _020E06C4
	mov r0, sl
	mov r2, sb
	mov r1, #0
	bl WmDataSharingReceiveData
	add r0, sl, #0x800
	ldrh r0, [r0, #0x18]
	cmp r0, #0
	bne _020E06C4
	mov r0, sl
	mov r1, fp
	bl WmDataSharingSendDataSet
	b _020E06C4
_020E05C8:
	cmp r0, #4
	mov r0, #0
	add r1, sl, #0x800
	moveq r0, #1
	streqh r0, [r1, #0x1c]
	beq _020E0648
	ldrh r2, [r1, #0xc]
	ldrh r1, [r1, #8]
	cmp r2, r1
	beq _020E0648
	mov r2, r2, lsl #9
	ldrh r1, [sl, r2]
	tst r1, #1
	orreq r1, r1, #1
	streqh r1, [sl, r2]
	beq _020E0648
	mov r1, r8
	add r0, sl, r2
	mov r2, #0x200
	bl MIi_CpuCopy16
	add r2, sl, #0x800
	ldrh r1, [r2, #0xc]
	mov r0, #1
	mov r7, #0
	add r1, sl, r1, lsl #1
	add r1, r1, #0x800
	ldrh r1, [r1]
	strh r1, [r2, #0x1a]
	ldrh r1, [r2, #0xc]
	add r1, r1, #1
	and r1, r1, #3
	strh r1, [r2, #0xc]
_020E0648:
	cmp r0, #0
	beq _020E06C4
	add r0, sl, #0x800
	ldrh r1, [r0, #0xa]
	ldrh r2, [r0, #0x10]
	mov r0, sb
	add r6, sl, r1, lsl #9
	add r1, r6, #0x20
	bl MIi_CpuCopy16
	add r3, sl, #0x800
	ldrh r1, [r3, #0xe]
	mov r4, #1
	ldr r0, _020E06D0 ; =WmDataSharingSetDataCallback
	str r1, [sp]
	ldrh r5, [r3, #0x16]
	mov r1, sl
	add r2, r6, #0x20
	str r5, [sp, #4]
	str r4, [sp, #8]
	ldrh r3, [r3, #0x10]
	bl WM_SetMPDataToPortEx
	add r1, sl, #0x800
	ldrh r2, [r1, #0xa]
	cmp r0, #2
	cmpne r0, #0
	add r2, r2, #1
	and r2, r2, #3
	strh r2, [r1, #0xa]
	movne r0, #5
	strneh r0, [r1, #0x1c]
	movne r7, r4
_020E06C4:
	mov r0, r7
	add sp, sp, #0xc
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_020E06D0: .word WmDataSharingSetDataCallback
_020E06D4: .word 0x0000FFFF
	arm_func_end WM_StepDataSharing

	arm_func_start WmDataSharingSetDataCallback
WmDataSharingSetDataCallback: ; 0x020E06D8
	stmdb sp!, {r3, r4, r5, lr}
	mov r5, r0
	bl WMi_GetSystemWork
	ldrh r2, [r5, #0xa]
	ldr r1, _020E07A8 ; =WmDataSharingReceiveCallback_Parent
	add r0, r0, r2, lsl #2
	ldr r2, [r0, #0xcc]
	ldr r4, [r0, #0x10c]
	cmp r2, r1
	ldrne r0, _020E07AC ; =WmDataSharingReceiveCallback_Child
	cmpne r2, r0
	ldmneia sp!, {r3, r4, r5, pc}
	cmp r4, #0
	ldmeqia sp!, {r3, r4, r5, pc}
	ldr r0, [r5, #0x20]
	cmp r4, r0
	ldmneia sp!, {r3, r4, r5, pc}
	bl WM_GetAID
	ldrh r1, [r5, #2]
	cmp r1, #0
	bne _020E0764
	cmp r0, #0
	ldmneia sp!, {r3, r4, r5, pc}
	add r1, r4, #0x800
	ldrh r0, [r1, #0xa]
	ldrh r2, [r5, #0x1a]
	add r0, r4, r0, lsl #1
	mov r2, r2, asr #1
	add r0, r0, #0x800
	strh r2, [r0]
	ldrh r0, [r1, #0xa]
	add r0, r0, #1
	and r0, r0, #3
	strh r0, [r1, #0xa]
	ldmia sp!, {r3, r4, r5, pc}
_020E0764:
	cmp r1, #0xa
	bne _020E0798
	cmp r0, #0
	beq _020E0788
	add r0, r4, #0x800
	ldrh r1, [r0, #0xa]
	add r1, r1, #3
	and r1, r1, #3
	strh r1, [r0, #0xa]
_020E0788:
	add r0, r4, #0x800
	mov r1, #4
	strh r1, [r0, #0x1c]
	ldmia sp!, {r3, r4, r5, pc}
_020E0798:
	add r0, r4, #0x800
	mov r1, #5
	strh r1, [r0, #0x1c]
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_020E07A8: .word WmDataSharingReceiveCallback_Parent
_020E07AC: .word WmDataSharingReceiveCallback_Child
	arm_func_end WmDataSharingSetDataCallback

	arm_func_start WmDataSharingReceiveCallback_Parent
WmDataSharingReceiveCallback_Parent: ; 0x020E07B0
	stmdb sp!, {r4, r5, r6, lr}
	ldr r4, [r0, #0x1c]
	cmp r4, #0
	ldmeqia sp!, {r4, r5, r6, pc}
	ldrh r1, [r0, #2]
	cmp r1, #0
	bne _020E08CC
	ldrh r1, [r0, #4]
	cmp r1, #0x15
	bgt _020E07FC
	bge _020E081C
	cmp r1, #9
	ldmgtia sp!, {r4, r5, r6, pc}
	cmp r1, #7
	ldmltia sp!, {r4, r5, r6, pc}
	beq _020E083C
	cmp r1, #9
	beq _020E084C
	ldmia sp!, {r4, r5, r6, pc}
_020E07FC:
	cmp r1, #0x1a
	ldmgtia sp!, {r4, r5, r6, pc}
	cmp r1, #0x19
	ldmltia sp!, {r4, r5, r6, pc}
	ldmeqia sp!, {r4, r5, r6, pc}
	cmp r1, #0x1a
	beq _020E084C
	ldmia sp!, {r4, r5, r6, pc}
_020E081C:
	ldrh r1, [r0, #0x12]
	ldr r2, [r0, #0xc]
	mov r0, r4
	bl WmDataSharingReceiveData
	mov r0, r4
	mov r1, #0
	bl WmDataSharingSendDataSet
	ldmia sp!, {r4, r5, r6, pc}
_020E083C:
	mov r0, r4
	mov r1, #0
	bl WmDataSharingSendDataSet
	ldmia sp!, {r4, r5, r6, pc}
_020E084C:
	ldrh r5, [r0, #0x12]
	mov r6, #1
	bl OS_DisableInterrupts
	add r1, r4, #0x800
	ldrh lr, [r1, #8]
	mvn ip, r6, lsl r5
	mov r3, lr, lsl #9
	ldrh r2, [r4, r3]
	and r2, r2, ip
	strh r2, [r4, r3]
	ldrh r1, [r1, #0x18]
	cmp r1, #1
	bne _020E089C
	add r1, lr, #1
	and r1, r1, #3
	mov r1, r1, lsl #0x10
	mov r2, r1, lsr #7
	ldrh r1, [r4, r2]
	and r1, r1, ip
	strh r1, [r4, r2]
_020E089C:
	bl OS_RestoreInterrupts
	mov r0, r4
	mov r1, #0
	bl WmDataSharingSendDataSet
	add r0, r4, #0x800
	ldrh r0, [r0, #0x18]
	cmp r0, #1
	ldmneia sp!, {r4, r5, r6, pc}
	mov r0, r4
	mov r1, #0
	bl WmDataSharingSendDataSet
	ldmia sp!, {r4, r5, r6, pc}
_020E08CC:
	add r0, r4, #0x800
	mov r1, #5
	strh r1, [r0, #0x1c]
	ldmia sp!, {r4, r5, r6, pc}
	arm_func_end WmDataSharingReceiveCallback_Parent

	arm_func_start WmDataSharingReceiveCallback_Child
WmDataSharingReceiveCallback_Child: ; 0x020E08DC
	stmdb sp!, {r4, r5, r6, r7, r8, lr}
	mov r8, r0
	ldr r4, [r8, #0x1c]
	cmp r4, #0
	ldmeqia sp!, {r4, r5, r6, r7, r8, pc}
	ldrh r0, [r8, #2]
	cmp r0, #0
	bne _020E09C0
	ldrh r0, [r8, #4]
	cmp r0, #0x15
	bgt _020E0924
	bge _020E093C
	cmp r0, #9
	ldmgtia sp!, {r4, r5, r6, r7, r8, pc}
	cmp r0, #7
	ldmltia sp!, {r4, r5, r6, r7, r8, pc}
	cmpne r0, #9
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_020E0924:
	cmp r0, #0x1a
	ldmgtia sp!, {r4, r5, r6, r7, r8, pc}
	cmp r0, #0x19
	ldmltia sp!, {r4, r5, r6, r7, r8, pc}
	cmpne r0, #0x1a
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_020E093C:
	ldr r7, [r8, #0xc]
	ldrh r5, [r8, #0x10]
	ldrh r6, [r7]
	bl WM_GetAID
	add r1, r4, #0x800
	ldrh r1, [r1, #0x14]
	cmp r5, r1
	beq _020E0964
	cmp r5, #0x200
	movhi r5, #0x200
_020E0964:
	cmp r5, #4
	ldmloia sp!, {r4, r5, r6, r7, r8, pc}
	mov r1, #1
	tst r6, r1, lsl r0
	ldmeqia sp!, {r4, r5, r6, r7, r8, pc}
	add r0, r4, #0x800
	ldrh r1, [r0, #8]
	mov r0, r7
	mov r2, r5
	add r1, r4, r1, lsl #9
	bl MIi_CpuCopy16
	add r1, r4, #0x800
	ldrh r0, [r1, #8]
	ldrh r2, [r8, #0x1a]
	add r0, r4, r0, lsl #1
	mov r2, r2, asr #1
	add r0, r0, #0x800
	strh r2, [r0]
	ldrh r0, [r1, #8]
	add r0, r0, #1
	and r0, r0, #3
	strh r0, [r1, #8]
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_020E09C0:
	add r0, r4, #0x800
	mov r1, #5
	strh r1, [r0, #0x1c]
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	arm_func_end WmDataSharingReceiveCallback_Child

	arm_func_start WmDataSharingReceiveData
WmDataSharingReceiveData: ; 0x020E09D0
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	mov r6, r0
	mov r3, r1
	mov r1, #1
	mov r4, r1, lsl r3
	add r0, r6, #0x800
	ldrh r1, [r0, #0xe]
	mov r7, r4, lsl #0x10
	mov r5, r2
	tst r1, r7, lsr #16
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	ldrh r4, [r0, #8]
	mov r2, r4, lsl #9
	ldrh r2, [r6, r2]
	tst r2, r7, lsr #16
	bne _020E0A34
	ldrh r0, [r0, #0x18]
	cmp r0, #1
	ldmneia sp!, {r3, r4, r5, r6, r7, pc}
	add r0, r4, #1
	and r4, r0, #3
	mov r0, r4, lsl #9
	ldrh r0, [r6, r0]
	tst r0, r7, lsr #16
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
_020E0A34:
	add r2, r6, r4, lsl #9
	mov r0, r6
	add r2, r2, #4
	bl WmGetSharedDataAddress
	mov r1, r0
	add r0, r6, #0x800
	cmp r5, #0
	ldrh r2, [r0, #0x10]
	beq _020E0A64
	mov r0, r5
	bl MIi_CpuCopy16
	b _020E0A6C
_020E0A64:
	mov r0, #0
	bl MIi_CpuClear16
_020E0A6C:
	bl OS_DisableInterrupts
	mov r4, r4, lsl #9
	ldrh r3, [r6, r4]
	mvn r1, r7, lsr #16
	add r2, r6, #2
	and r1, r3, r1
	strh r1, [r6, r4]
	ldrh r1, [r2, r4]
	orr r1, r1, r7, lsr #16
	strh r1, [r2, r4]
	bl OS_RestoreInterrupts
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	arm_func_end WmDataSharingReceiveData

	arm_func_start WmDataSharingSendDataSet
WmDataSharingSendDataSet: ; 0x020E0A9C
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, lr}
	sub sp, sp, #0xc
	mov sl, r0
	mov sb, r1
	bl OS_DisableInterrupts
	add r1, sl, #0x800
	ldrh r1, [r1, #8]
	mov r4, r0
	mov r1, r1, lsl #9
	ldrh r1, [sl, r1]
	cmp r1, #0
	bne _020E0BDC
	bl WM_GetConnectedAIDs
	add r1, sl, #0x800
	ldrh r6, [r1, #8]
	ldrh r1, [r1, #0x18]
	mov r7, r0
	add r0, r6, #1
	and r5, r0, #3
	cmp r1, #1
	addeq r0, r5, #1
	andeq r8, r0, #3
	movne r8, r5
	add r1, sl, r8, lsl #9
	mov r0, #0
	mov r2, #0x200
	bl MIi_CpuClear16
	add r0, sl, #0x800
	ldrh r3, [r0, #0xe]
	orr r2, r7, #1
	mov r1, r8, lsl #9
	and r2, r3, r2
	strh r2, [sl, r1]
	strh r5, [r0, #8]
	ldrh r0, [r0, #0xe]
	mov r1, r6, lsl #9
	cmp sb, #1
	strh r0, [sl, r1]
	ldreqh r0, [sl, r1]
	biceq r0, r0, #1
	streqh r0, [sl, r1]
	mov r0, r4
	bl OS_RestoreInterrupts
	add r3, sl, #0x800
	ldrh r1, [r3, #0xe]
	mov r4, #1
	ldr r0, _020E0BE8 ; =WmDataSharingSetDataCallback
	and r1, r1, r7
	mov r1, r1, lsl #0x10
	mov r1, r1, lsr #0x10
	str r1, [sp]
	ldrh r5, [r3, #0x16]
	mov r1, sl
	add r2, sl, r6, lsl #9
	str r5, [sp, #4]
	str r4, [sp, #8]
	ldrh r3, [r3, #0x14]
	bl WM_SetMPDataToPortEx
	cmp r0, #7
	bne _020E0BB8
	add r0, sl, r6, lsl #1
	ldr r1, _020E0BEC ; =0x0000FFFF
	add r0, r0, #0x800
	strh r1, [r0]
	add r0, sl, #0x800
	ldrh r1, [r0, #0xa]
	add sp, sp, #0xc
	add r1, r1, #1
	and r1, r1, #3
	strh r1, [r0, #0xa]
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, pc}
_020E0BB8:
	cmp r0, #0
	cmpne r0, #2
	addeq sp, sp, #0xc
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, pc}
	add r0, sl, #0x800
	mov r1, #5
	strh r1, [r0, #0x1c]
	add sp, sp, #0xc
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, pc}
_020E0BDC:
	bl OS_RestoreInterrupts
	add sp, sp, #0xc
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, pc}
	.align 2, 0
_020E0BE8: .word WmDataSharingSetDataCallback
_020E0BEC: .word 0x0000FFFF
	arm_func_end WmDataSharingSendDataSet

	arm_func_start WM_GetSharedDataAddress
WM_GetSharedDataAddress: ; 0x020E0BF0
	stmdb sp!, {r3, lr}
	mov lr, r1
	mov r3, r2
	cmp r0, #0
	ldrh r1, [lr]
	ldrh ip, [lr, #2]
	mov r2, #1
	moveq r0, #0
	ldmeqia sp!, {r3, pc}
	cmp lr, #0
	moveq r0, #0
	ldmeqia sp!, {r3, pc}
	tst r1, r2, lsl r3
	moveq r0, #0
	ldmeqia sp!, {r3, pc}
	tst ip, r2, lsl r3
	moveq r0, #0
	ldmeqia sp!, {r3, pc}
	add r2, lr, #4
	bl WmGetSharedDataAddress
	ldmia sp!, {r3, pc}
	arm_func_end WM_GetSharedDataAddress

	arm_func_start WmGetSharedDataAddress
WmGetSharedDataAddress: ; 0x020E0C44
	stmdb sp!, {r3, r4, r5, lr}
	mov ip, #1
	mov r3, ip, lsl r3
	sub r3, r3, #1
	mov r5, r0
	and r0, r1, r3
	mov r4, r2
	bl MATH_CountPopulation
	add r1, r5, #0x800
	ldrh r1, [r1, #0x10]
	mla r0, r1, r0, r4
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end WmGetSharedDataAddress

	arm_func_start WM_StartKeySharing
WM_StartKeySharing: ; 0x020E0C74
	stmdb sp!, {r3, lr}
	mov ip, #1
	rsb r2, ip, #0x10000
	mov r3, #2
	str ip, [sp]
	bl WM_StartDataSharing
	ldmia sp!, {r3, pc}
	arm_func_end WM_StartKeySharing

	arm_func_start WM_EndKeySharing
WM_EndKeySharing: ; 0x020E0C90
	ldr ip, _020E0C98 ; =WM_EndDataSharing
	bx ip
	.align 2, 0
_020E0C98: .word WM_EndDataSharing
	arm_func_end WM_EndKeySharing

	arm_func_start WM_SetWEPKey
WM_SetWEPKey: ; 0x020E0C9C
	stmdb sp!, {r4, r5, r6, lr}
	mov r6, r0
	mov r5, r1
	mov r4, r2
	bl WMi_CheckIdle
	cmp r0, #0
	ldmneia sp!, {r4, r5, r6, pc}
	cmp r5, #3
	movhi r0, #6
	ldmhiia sp!, {r4, r5, r6, pc}
	cmp r5, #0
	beq _020E0CE4
	cmp r4, #0
	moveq r0, #6
	ldmeqia sp!, {r4, r5, r6, pc}
	mov r0, r4
	mov r1, #0x50
	bl DC_StoreRange
_020E0CE4:
	mov r1, r6
	mov r0, #0x14
	bl WMi_SetCallbackTable
	mov r2, r5
	mov r3, r4
	mov r0, #0x14
	mov r1, #2
	bl WMi_SendCommand
	cmp r0, #0
	moveq r0, #2
	ldmia sp!, {r4, r5, r6, pc}
	arm_func_end WM_SetWEPKey

	arm_func_start WM_SetWEPKeyEx
WM_SetWEPKeyEx: ; 0x020E0D10
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	mov r7, r0
	mov r6, r1
	mov r5, r2
	mov r4, r3
	bl WMi_CheckIdle
	cmp r0, #0
	ldmneia sp!, {r3, r4, r5, r6, r7, pc}
	cmp r6, #3
	movhi r0, #6
	ldmhiia sp!, {r3, r4, r5, r6, r7, pc}
	cmp r6, #0
	beq _020E0D5C
	cmp r4, #0
	moveq r0, #6
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	mov r0, r4
	mov r1, #0x50
	bl DC_StoreRange
_020E0D5C:
	mov r1, r7
	mov r0, #0x27
	bl WMi_SetCallbackTable
	mov r2, r6
	mov r3, r4
	mov r0, #0x27
	mov r1, #3
	str r5, [sp]
	bl WMi_SendCommand
	cmp r0, #0
	moveq r0, #2
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	arm_func_end WM_SetWEPKeyEx

	arm_func_start WM_SetGameInfo
WM_SetGameInfo: ; 0x020E0D8C
	stmdb sp!, {r4, r5, r6, r7, lr}
	sub sp, sp, #0xc
	mov r7, r0
	mov r6, r1
	mov r5, r2
	mov r0, #2
	mov r1, #7
	mov r2, #9
	mov r4, r3
	bl WMi_CheckStateEx
	cmp r0, #0
	addne sp, sp, #0xc
	ldmneia sp!, {r4, r5, r6, r7, pc}
	cmp r6, #0
	addeq sp, sp, #0xc
	moveq r0, #6
	ldmeqia sp!, {r4, r5, r6, r7, pc}
	cmp r5, #0x70
	addhi sp, sp, #0xc
	movhi r0, #6
	ldmhiia sp!, {r4, r5, r6, r7, pc}
	ldr r1, _020E0E40 ; =_021E4CC0
	mov r0, r6
	mov r2, r5
	bl MIi_CpuCopy16
	ldr r0, _020E0E40 ; =_021E4CC0
	mov r1, r5
	bl DC_StoreRange
	mov r1, r7
	mov r0, #0x18
	bl WMi_SetCallbackTable
	ldrh r0, [sp, #0x20]
	str r4, [sp]
	ldrb r1, [sp, #0x24]
	str r0, [sp, #4]
	ldr r2, _020E0E40 ; =_021E4CC0
	str r1, [sp, #8]
	mov r3, r5
	mov r0, #0x18
	mov r1, #5
	bl WMi_SendCommand
	cmp r0, #0
	moveq r0, #2
	add sp, sp, #0xc
	ldmia sp!, {r4, r5, r6, r7, pc}
	.align 2, 0
_020E0E40: .word _021E4CC0
	arm_func_end WM_SetGameInfo

	arm_func_start WM_SetBeaconIndication
WM_SetBeaconIndication: ; 0x020E0E44
	stmdb sp!, {r3, r4, r5, lr}
	mov r5, r0
	mov r4, r1
	bl WMi_CheckIdle
	cmp r0, #0
	ldmneia sp!, {r3, r4, r5, pc}
	cmp r4, #0
	cmpne r4, #1
	movne r0, #6
	ldmneia sp!, {r3, r4, r5, pc}
	mov r1, r5
	mov r0, #0x19
	bl WMi_SetCallbackTable
	mov r2, r4
	mov r0, #0x19
	mov r1, #1
	bl WMi_SendCommand
	cmp r0, #0
	moveq r0, #2
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end WM_SetBeaconIndication

	arm_func_start WM_SetLifeTime
WM_SetLifeTime: ; 0x020E0E94
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	sub sp, sp, #8
	mov r7, r0
	mov r6, r1
	mov r5, r2
	mov r4, r3
	bl WMi_CheckIdle
	cmp r0, #0
	addne sp, sp, #8
	ldmneia sp!, {r3, r4, r5, r6, r7, pc}
	mov r1, r7
	mov r0, #0x1d
	bl WMi_SetCallbackTable
	ldrh ip, [sp, #0x20]
	mov r2, r6
	mov r3, r5
	mov r0, #0x1d
	mov r1, #4
	stmia sp, {r4, ip}
	bl WMi_SendCommand
	cmp r0, #0
	moveq r0, #2
	add sp, sp, #8
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	arm_func_end WM_SetLifeTime

	arm_func_start WM_MeasureChannel
WM_MeasureChannel: ; 0x020E0EF4
	stmdb sp!, {r4, r5, r6, r7, lr}
	sub sp, sp, #0xc
	mov r7, r0
	mov r6, r1
	mov r5, r2
	mov r4, r3
	bl WMi_GetSystemWork
	mov r0, #1
	mov r1, #2
	bl WMi_CheckStateEx
	cmp r0, #0
	addne sp, sp, #0xc
	ldmneia sp!, {r4, r5, r6, r7, pc}
	mov r1, r7
	mov r0, #0x1e
	bl WMi_SetCallbackTable
	ldrh r2, [sp, #0x20]
	mov r3, #0x1e
	add r0, sp, #0
	mov r1, #0xa
	strh r3, [sp]
	strh r6, [sp, #2]
	strh r5, [sp, #4]
	strh r4, [sp, #6]
	strh r2, [sp, #8]
	bl WMi_SendCommandDirect
	cmp r0, #0
	moveq r0, #2
	add sp, sp, #0xc
	ldmia sp!, {r4, r5, r6, r7, pc}
	arm_func_end WM_MeasureChannel

	arm_func_start WM_SetEntry
WM_SetEntry: ; 0x020E0F6C
	stmdb sp!, {r3, r4, r5, lr}
	mov r5, r0
	mov r4, r1
	mov r0, #2
	mov r1, #7
	mov r2, #9
	bl WMi_CheckStateEx
	cmp r0, #0
	ldmneia sp!, {r3, r4, r5, pc}
	mov r1, r5
	mov r0, #0x21
	bl WMi_SetCallbackTable
	mov r2, r4
	mov r0, #0x21
	mov r1, #1
	bl WMi_SendCommand
	cmp r0, #0
	moveq r0, #2
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end WM_SetEntry

	.section .text.unified_40,4,1,6

	arm_func_start CTRDG_Init
CTRDG_Init: ; 0x020E16E4
	stmdb sp!, {r3, r4, r5, lr}
	ldr r0, _020E1784 ; =_021E4D60
	ldr r1, [r0, #8]
	cmp r1, #0
	ldmneia sp!, {r3, r4, r5, pc}
	mov r1, #1
	str r1, [r0, #8]
	bl CTRDGi_InitCommon
	ldr r0, _020E1784 ; =_021E4D60
	mov r1, #0
	str r1, [r0, #0xc]
	bl PXI_Init
	mov r5, #0xd
	mov r4, #1
_020E171C:
	mov r0, r5
	mov r1, r4
	bl PXI_IsCallbackReady
	cmp r0, #0
	beq _020E171C
	ldr r1, _020E1788 ; =CTRDGi_CallbackForInitModuleInfo
	mov r0, #0xd
	bl PXI_SetFifoRecvCallback
	bl CTRDGi_InitModuleInfo
	mov r0, #0xd
	mov r1, #0
	bl PXI_SetFifoRecvCallback
	ldr r1, _020E178C ; =CTRDGi_PulledOutCallback
	mov r0, #0xd
	bl PXI_SetFifoRecvCallback
	ldr r1, _020E1784 ; =_021E4D60
	mov r2, #0
	ldr r0, _020E1790 ; =_021E4E40
	str r2, [r1, #0x18]
	bl CTRDGi_InitTaskThread
	ldr r1, _020E1794 ; =CTRDGi_CallbackForSetPhi
	mov r0, #0x11
	bl PXI_SetFifoRecvCallback
	mov r0, #0
	bl CTRDG_Enable
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_020E1784: .word _021E4D60
_020E1788: .word CTRDGi_CallbackForInitModuleInfo
_020E178C: .word CTRDGi_PulledOutCallback
_020E1790: .word _021E4E40
_020E1794: .word CTRDGi_CallbackForSetPhi
	arm_func_end CTRDG_Init

	arm_func_start CTRDGi_InitModuleInfo
CTRDGi_InitModuleInfo: ; 0x020E1798
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	sub sp, sp, #0x10
	ldr r0, _020E1960 ; =_021E4D60
	ldr r1, [r0]
	cmp r1, #0
	addne sp, sp, #0x10
	ldmneia sp!, {r3, r4, r5, r6, r7, pc}
	ldr r1, _020E1964 ; =0x04000300
	mov r2, #1
	ldrh r1, [r1]
	str r2, [r0]
	tst r1, #1
	addeq sp, sp, #0x10
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	mov r0, #0x40000
	bl OS_SetIrqMask
	ldr r3, _020E1968 ; =0x04000208
	mov r2, #1
	ldrh r5, [r3]
	ldr r1, _020E196C ; =CTRDGi_Work
	mov r4, r0
	strh r2, [r3]
	ldrh r0, [r1, #2]
	add r1, sp, #8
	bl CTRDGi_LockByProcessor
	ldr r1, _020E1970 ; =0x04000204
	add r0, sp, #0
	ldrh r1, [r1]
	and r1, r1, #0x8000
	mov r6, r1, asr #0xf
	bl CTRDGi_ChangeLatestAccessCycle
	ldr r3, _020E1970 ; =0x04000204
	ldr r0, _020E1974 ; =_021E4D80
	ldrh r2, [r3]
	add r0, r0, #0x80
	mov r1, #0x40
	bic r2, r2, #0x8000
	strh r2, [r3]
	bl DC_InvalidateRange
	ldr r2, _020E1974 ; =_021E4D80
	ldr r1, _020E1978 ; =0x08000080
	mov r0, #1
	mov r3, #0x40
	add r2, r2, #0x80
	bl MI_DmaCopy16
	ldr r2, _020E1970 ; =0x04000204
	add r0, sp, #0
	ldrh r1, [r2]
	bic r1, r1, #0x8000
	orr r1, r1, r6, lsl #15
	strh r1, [r2]
	bl CTRDGi_RestoreAccessCycle
	ldr r0, _020E196C ; =CTRDGi_Work
	add r1, sp, #8
	ldrh r0, [r0, #2]
	bl CTRDGi_UnlockByProcessor
	ldr r1, _020E197C ; =0x027FFF9B
	ldrb r0, [r1]
	cmp r0, #0
	bne _020E1894
	ldrb r0, [r1, #-1]
	cmp r0, #0
	bne _020E18F8
_020E1894:
	ldr r3, _020E1974 ; =_021E4D80
	ldr r6, _020E1980 ; =0x027FFC30
	ldrh r0, [r3, #0xbe]
	mov r2, #0
	strh r0, [r6]
	b _020E18C0
_020E18AC:
	add r0, r3, r2
	ldrb r1, [r0, #0xb5]
	add r0, r6, r2
	add r2, r2, #1
	strb r1, [r0, #2]
_020E18C0:
	cmp r2, #3
	blt _020E18AC
	ldrh r0, [r3, #0xb0]
	strh r0, [r6, #6]
	ldr r0, [r3, #0xac]
	str r0, [r6, #8]
	bl CTRDG_IsExisting
	cmp r0, #0
	movne r2, #1
	ldr r1, _020E197C ; =0x027FFF9B
	moveq r2, #0
	mov r0, #1
	strb r2, [r1]
	strb r0, [r1, #-1]
_020E18F8:
	ldr r0, _020E1984 ; =0xFFFF0020
	ldr r1, _020E1988 ; =_021E4D84
	mov r2, #0x9c
	bl MIi_CpuCopy32
	bl DC_FlushAll
	ldr r0, _020E1974 ; =_021E4D80
	add r0, r0, #0xfe000000
	mov r0, r0, lsr #5
	mov r0, r0, lsl #6
	orr r0, r0, #1
	bl CTRDGi_SendtoPxi
	mov r7, #1
	ldr r6, _020E196C ; =CTRDGi_Work
	b _020E1938
_020E1930:
	mov r0, r7
	bl SVC_WaitByLoop
_020E1938:
	ldrh r0, [r6]
	cmp r0, #1
	bne _020E1930
	ldr r2, _020E1968 ; =0x04000208
	mov r0, r4
	ldrh r1, [r2]
	strh r5, [r2]
	bl OS_SetIrqMask
	add sp, sp, #0x10
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.align 2, 0
_020E1960: .word _021E4D60
_020E1964: .word 0x04000300
_020E1968: .word 0x04000208
_020E196C: .word CTRDGi_Work
_020E1970: .word 0x04000204
_020E1974: .word _021E4D80
_020E1978: .word 0x08000080
_020E197C: .word 0x027FFF9B
_020E1980: .word 0x027FFC30
_020E1984: .word 0xFFFF0020
_020E1988: .word _021E4D84
	arm_func_end CTRDGi_InitModuleInfo

	arm_func_start CTRDGi_CallbackForInitModuleInfo
CTRDGi_CallbackForInitModuleInfo: ; 0x020E198C
	stmdb sp!, {r3, lr}
	and r0, r1, #0x3f
	cmp r0, #1
	bne _020E19AC
	ldr r0, _020E19B4 ; =CTRDGi_Work
	mov r1, #1
	strh r1, [r0]
	ldmia sp!, {r3, pc}
_020E19AC:
	bl OS_Terminate
	ldmia sp!, {r3, pc}
	.align 2, 0
_020E19B4: .word CTRDGi_Work
	arm_func_end CTRDGi_CallbackForInitModuleInfo

	arm_func_start CTRDGi_PulledOutCallback
CTRDGi_PulledOutCallback: ; 0x020E19B8
	stmdb sp!, {r3, lr}
	and r0, r1, #0x3f
	cmp r0, #0x11
	bne _020E1A08
	ldr r0, _020E1A10 ; =_021E4D60
	ldr r1, [r0, #0xc]
	cmp r1, #0
	ldmneia sp!, {r3, pc}
	ldr r1, [r0, #0x18]
	mov r0, #0
	cmp r1, #0
	beq _020E19EC
	blx r1
_020E19EC:
	cmp r0, #0
	beq _020E19F8
	bl CTRDG_TerminateForPulledOut
_020E19F8:
	ldr r0, _020E1A10 ; =_021E4D60
	mov r1, #1
	str r1, [r0, #0xc]
	ldmia sp!, {r3, pc}
_020E1A08:
	bl OS_Terminate
	ldmia sp!, {r3, pc}
	.align 2, 0
_020E1A10: .word _021E4D60
	arm_func_end CTRDGi_PulledOutCallback

	arm_func_start CTRDG_TerminateForPulledOut
CTRDG_TerminateForPulledOut: ; 0x020E1A14
	stmdb sp!, {r3, lr}
	mov r0, #2
	bl CTRDGi_SendtoPxi
	bl OS_Terminate
	ldmia sp!, {r3, pc}
	arm_func_end CTRDG_TerminateForPulledOut

	arm_func_start CTRDGi_CallbackForSetPhi
CTRDGi_CallbackForSetPhi: ; 0x020E1A28
	ldr r0, _020E1A38 ; =_021E4D60
	mov r1, #0
	str r1, [r0, #4]
	bx lr
	.align 2, 0
_020E1A38: .word _021E4D60
	arm_func_end CTRDGi_CallbackForSetPhi

	.section .text.unified_42,4,1,6

	arm_func_start CTRDGi_SetFlashBankMx
CTRDGi_SetFlashBankMx: ; 0x020E1BE4
	ldr r3, _020E1C10 ; =0x0A005555
	mov r2, #0xaa
	ldr r1, _020E1C14 ; =0x0A002AAA
	strb r2, [r3]
	mov r2, #0x55
	strb r2, [r1]
	mov r1, #0xb0
	strb r1, [r3]
	mov r1, #0xa000000
	strb r0, [r1]
	bx lr
	.align 2, 0
_020E1C10: .word 0x0A005555
_020E1C14: .word 0x0A002AAA
	arm_func_end CTRDGi_SetFlashBankMx

	arm_func_start CTRDGi_ReadFlashID
CTRDGi_ReadFlashID: ; 0x020E1C18
	stmdb sp!, {r3, r4, r5, lr}
	ldr r5, _020E1D10 ; =0x04000208
	mov r0, #0
	ldrh r2, [r5]
	ldr r1, _020E1D14 ; =ctrdgi_flash_lock_id
	ldr r4, _020E1D18 ; =0x0A005555
	strh r0, [r5]
	str r2, [r1, #0x1c]
	mov r3, #0xaa
	ldr r2, _020E1D1C ; =0x0A002AAA
	strb r3, [r4]
	mov r3, #0x55
	strb r3, [r2]
	mov r2, #0x90
	strb r2, [r4]
	ldrh r2, [r5]
	ldr r1, [r1, #0x1c]
	strh r1, [r5]
	bl StartFlashTimer
	ldr r4, _020E1D14 ; =ctrdgi_flash_lock_id
	ldrh r0, [r4, #2]
	cmp r0, #0
	bne _020E1C84
_020E1C74:
	bl CheckFlashTimer
	ldrh r0, [r4, #2]
	cmp r0, #0
	beq _020E1C74
_020E1C84:
	ldr r0, _020E1D20 ; =0x0A000001
	bl CTRDGi_ReadSrFunc
	mov r4, r0, lsl #0x18
	mov r0, #0xa000000
	bl CTRDGi_ReadSrFunc
	ldr ip, _020E1D10 ; =0x04000208
	orr r4, r0, r4, lsr #16
	ldrh r2, [ip]
	mov r0, #0
	ldr r1, _020E1D14 ; =ctrdgi_flash_lock_id
	strh r0, [ip]
	ldr r5, _020E1D18 ; =0x0A005555
	str r2, [r1, #0x1c]
	mov r3, #0xaa
	ldr r2, _020E1D1C ; =0x0A002AAA
	strb r3, [r5]
	mov r3, #0x55
	strb r3, [r2]
	mov r2, #0xf0
	strb r2, [r5]
	strb r2, [r5]
	ldrh r2, [ip]
	ldr r1, [r1, #0x1c]
	strh r1, [ip]
	bl StartFlashTimer
	ldr r5, _020E1D14 ; =ctrdgi_flash_lock_id
	ldrh r0, [r5, #2]
	cmp r0, #0
	bne _020E1D08
_020E1CF8:
	bl CheckFlashTimer
	ldrh r0, [r5, #2]
	cmp r0, #0
	beq _020E1CF8
_020E1D08:
	mov r0, r4
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_020E1D10: .word 0x04000208
_020E1D14: .word ctrdgi_flash_lock_id
_020E1D18: .word 0x0A005555
_020E1D1C: .word 0x0A002AAA
_020E1D20: .word 0x0A000001
	arm_func_end CTRDGi_ReadFlashID

	arm_func_start StartFlashTimer
StartFlashTimer: ; 0x020E1D24
	stmdb sp!, {r3, lr}
	ldr r1, _020E1D64 ; =AgbFlash
	ldr r2, _020E1D68 ; =ctrdgi_flash_lock_id
	mov r3, #0
	ldr r1, [r1, #4]
	strh r3, [r2, #2]
	mov r0, r0, lsl #1
	ldrh r1, [r1, r0]
	mov r0, r3
	str r1, [r2, #0x20]
	str r0, [r2, #0x24]
	bl OS_GetTick
	ldr r2, _020E1D68 ; =ctrdgi_flash_lock_id
	str r0, [r2, #0x28]
	str r1, [r2, #0x2c]
	ldmia sp!, {r3, pc}
	.align 2, 0
_020E1D64: .word AgbFlash
_020E1D68: .word ctrdgi_flash_lock_id
	arm_func_end StartFlashTimer

	arm_func_start CheckFlashTimer
CheckFlashTimer: ; 0x020E1D6C
	stmdb sp!, {r3, lr}
	bl OS_GetTick
	ldr r3, _020E1DC0 ; =ctrdgi_flash_lock_id
	ldr r2, _020E1DC4 ; =0x000082EA
	ldr ip, [r3, #0x28]
	ldr r3, [r3, #0x2c]
	subs ip, r0, ip
	sbc r0, r1, r3
	mov r1, r0, lsl #6
	orr r1, r1, ip, lsr #26
	mov r0, ip, lsl #6
	mov r3, #0
	bl _ll_udiv
	ldr r2, _020E1DC0 ; =ctrdgi_flash_lock_id
	ldr r3, [r2, #0x24]
	ldr ip, [r2, #0x20]
	cmp r3, r1
	cmpeq ip, r0
	movls r0, #1
	strlsh r0, [r2, #2]
	ldmia sp!, {r3, pc}
	.align 2, 0
_020E1DC0: .word ctrdgi_flash_lock_id
_020E1DC4: .word 0x000082EA
	arm_func_end CheckFlashTimer

	arm_func_start CTRDGi_ReadSrFunc
CTRDGi_ReadSrFunc: ; 0x020E1DC8
	ldrb r0, [r0]
	bx lr
	arm_func_end CTRDGi_ReadSrFunc

	arm_func_start CTRDGi_PollingSR512kCOMMON
CTRDGi_PollingSR512kCOMMON: ; 0x020E1DD0
	stmdb sp!, {r4, r5, r6, r7, r8, lr}
	mov r4, r0
	mov r8, r1
	mov r7, r2
	mov r6, #0
	bl StartFlashTimer
	ldr r5, _020E1E54 ; =ctrdgi_flash_lock_id
_020E1DEC:
	mov r0, r8
	bl CTRDGi_ReadSrFunc
	cmp r0, r7
	beq _020E1E40
	ldrh r0, [r5, #2]
	cmp r0, #0
	beq _020E1DEC
	mov r0, r8
	bl CTRDGi_ReadSrFunc
	cmp r7, r0
	beq _020E1E40
	ldr r0, _020E1E58 ; =AgbFlash
	ldr r0, [r0]
	ldrh r0, [r0, #0x18]
	cmp r0, #0xc2
	ldreq r0, _020E1E5C ; =0x0A005555
	moveq r1, #0xf0
	streqb r1, [r0]
	orr r0, r4, #0xc000
	mov r0, r0, lsl #0x10
	mov r6, r0, lsr #0x10
_020E1E40:
	bl CTRDG_IsExisting
	cmp r0, #0
	moveq r6, #0x1000
	mov r0, r6
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	.align 2, 0
_020E1E54: .word ctrdgi_flash_lock_id
_020E1E58: .word AgbFlash
_020E1E5C: .word 0x0A005555
	arm_func_end CTRDGi_PollingSR512kCOMMON

	arm_func_start CTRDGi_PollingSR1MCOMMON
CTRDGi_PollingSR1MCOMMON: ; 0x020E1E60
	stmdb sp!, {r4, r5, r6, r7, r8, lr}
	mov r8, r0
	mov r7, r1
	mov r6, r2
	mov r5, #0
	bl StartFlashTimer
	ldr r4, _020E1F0C ; =ctrdgi_flash_lock_id
_020E1E7C:
	bl CheckFlashTimer
	mov r0, r7
	bl CTRDGi_ReadSrFunc
	cmp r0, r6
	beq _020E1EF8
	tst r0, #0x20
	beq _020E1EC4
	mov r0, r7
	bl CTRDGi_ReadSrFunc
	cmp r6, r0
	beq _020E1EF8
	orr r0, r8, #0xa000
	mov r0, r0, lsl #0x10
	ldr r1, _020E1F10 ; =0x0A005555
	mov r2, #0xf0
	strb r2, [r1]
	mov r5, r0, lsr #0x10
	b _020E1EF8
_020E1EC4:
	ldrh r0, [r4, #2]
	cmp r0, #0
	beq _020E1E7C
	mov r0, r7
	bl CTRDGi_ReadSrFunc
	cmp r6, r0
	beq _020E1EF8
	orr r0, r8, #0xc000
	mov r0, r0, lsl #0x10
	ldr r1, _020E1F10 ; =0x0A005555
	mov r2, #0xf0
	strb r2, [r1]
	mov r5, r0, lsr #0x10
_020E1EF8:
	bl CTRDG_IsExisting
	cmp r0, #0
	moveq r5, #0x1000
	mov r0, r5
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	.align 2, 0
_020E1F0C: .word ctrdgi_flash_lock_id
_020E1F10: .word 0x0A005555
	arm_func_end CTRDGi_PollingSR1MCOMMON

	arm_func_start CTRDGi_ReadAgbFlashCore
CTRDGi_ReadAgbFlashCore: ; 0x020E1F14
	stmdb sp!, {r3, r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0x24
	mov r5, r0
	ldmia r5!, {r0, r1, r2, r3}
	add r4, sp, #0
	stmia r4!, {r0, r1, r2, r3}
	ldmia r5!, {r0, r1, r2, r3}
	stmia r4!, {r0, r1, r2, r3}
	ldr r0, _020E1FFC ; =ctrdgi_flash_lock_id
	ldr r1, [r5]
	ldrh r0, [r0]
	str r1, [r4]
	ldrh r5, [sp, #0x20]
	ldr r6, [sp, #0x14]
	ldr r7, [sp, #0x1c]
	ldr r8, [sp, #0x18]
	bl OS_LockCartridge
	ldr r3, _020E2000 ; =0x04000204
	ldr r0, _020E2004 ; =AgbFlash
	ldrh r2, [r3]
	ldrh r1, [r3]
	and r4, r2, #3
	bic r1, r1, #3
	orr r1, r1, #3
	strh r1, [r3]
	ldr r0, [r0]
	ldr r0, [r0]
	cmp r0, #0x20000
	bne _020E1F98
	mov r0, r5, lsl #0xc
	mov r0, r0, lsr #0x10
	bl CTRDGi_SetFlashBankMx
	and r5, r5, #0xf
_020E1F98:
	ldr r0, _020E2004 ; =AgbFlash
	cmp r8, #0
	ldr r0, [r0]
	sub r8, r8, #1
	ldrh r0, [r0, #8]
	mov r0, r5, lsl r0
	add r0, r0, #0xa000000
	add r1, r6, r0
	beq _020E1FD0
_020E1FBC:
	ldrb r0, [r1], #1
	cmp r8, #0
	sub r8, r8, #1
	strb r0, [r7], #1
	bne _020E1FBC
_020E1FD0:
	ldr r2, _020E2000 ; =0x04000204
	ldr r0, _020E1FFC ; =ctrdgi_flash_lock_id
	ldrh r1, [r2]
	bic r1, r1, #3
	orr r1, r1, r4
	strh r1, [r2]
	ldrh r0, [r0]
	bl OS_UnlockCartridge
	mov r0, #0
	add sp, sp, #0x24
	ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
	.align 2, 0
_020E1FFC: .word ctrdgi_flash_lock_id
_020E2000: .word 0x04000204
_020E2004: .word AgbFlash
	arm_func_end CTRDGi_ReadAgbFlashCore

	arm_func_start CTRDGi_VerifyFlashSectorCore
CTRDGi_VerifyFlashSectorCore: ; 0x020E2008
	cmp r2, #0
	sub r2, r2, #1
	beq _020E2034
_020E2014:
	ldrb ip, [r1], #1
	ldrb r3, [r0], #1
	cmp ip, r3
	subne r0, r1, #1
	bxne lr
	cmp r2, #0
	sub r2, r2, #1
	bne _020E2014
_020E2034:
	mov r0, #0
	bx lr
	arm_func_end CTRDGi_VerifyFlashSectorCore

	arm_func_start CTRDGi_VerifyAgbFlashCore
CTRDGi_VerifyAgbFlashCore: ; 0x020E203C
	stmdb sp!, {r4, r5, r6, r7, lr}
	sub sp, sp, #0x24
	mov r5, r0
	ldmia r5!, {r0, r1, r2, r3}
	add r4, sp, #0
	stmia r4!, {r0, r1, r2, r3}
	ldmia r5!, {r0, r1, r2, r3}
	stmia r4!, {r0, r1, r2, r3}
	ldr r0, _020E210C ; =ctrdgi_flash_lock_id
	ldr r1, [r5]
	ldrh r0, [r0]
	str r1, [r4]
	ldrh r5, [sp, #0x20]
	ldr r6, [sp, #0xc]
	ldr r7, [sp, #0x18]
	bl OS_LockCartridge
	ldr r3, _020E2110 ; =0x04000204
	ldr r0, _020E2114 ; =AgbFlash
	ldrh r2, [r3]
	ldrh r1, [r3]
	and r4, r2, #3
	bic r1, r1, #3
	orr r1, r1, #3
	strh r1, [r3]
	ldr r0, [r0]
	ldr r0, [r0]
	cmp r0, #0x20000
	bne _020E20BC
	mov r0, r5, lsl #0xc
	mov r0, r0, lsr #0x10
	bl CTRDGi_SetFlashBankMx
	and r5, r5, #0xf
_020E20BC:
	ldr r1, _020E2114 ; =AgbFlash
	mov r0, r6
	ldr r1, [r1]
	mov r2, r7
	ldrh r1, [r1, #8]
	mov r1, r5, lsl r1
	add r1, r1, #0xa000000
	bl CTRDGi_VerifyFlashSectorCore
	ldr r3, _020E2110 ; =0x04000204
	ldr r1, _020E210C ; =ctrdgi_flash_lock_id
	ldrh r2, [r3]
	mov r5, r0
	bic r0, r2, #3
	orr r0, r0, r4
	strh r0, [r3]
	ldrh r0, [r1]
	bl OS_UnlockCartridge
	mov r0, r5
	add sp, sp, #0x24
	ldmia sp!, {r4, r5, r6, r7, pc}
	.align 2, 0
_020E210C: .word ctrdgi_flash_lock_id
_020E2110: .word 0x04000204
_020E2114: .word AgbFlash
	arm_func_end CTRDGi_VerifyAgbFlashCore

	arm_func_start CTRDGi_WriteAndVerifyAgbFlashSectorCore
CTRDGi_WriteAndVerifyAgbFlashSectorCore: ; 0x020E2118
	stmdb sp!, {r4, r5, r6, r7, lr}
	sub sp, sp, #0x24
	mov r4, r0
	ldmia r4!, {r0, r1, r2, r3}
	add ip, sp, #0
	stmia ip!, {r0, r1, r2, r3}
	ldmia r4!, {r0, r1, r2, r3}
	stmia ip!, {r0, r1, r2, r3}
	ldr r0, [r4]
	mov r4, #0
	str r0, [ip]
	ldrh r5, [sp, #0x20]
	ldr r6, [sp, #0xc]
	ldr r7, [sp, #0x18]
_020E2150:
	mov r0, r5
	mov r1, r6
	bl CTRDG_WriteAgbFlashSector
	cmp r0, #0
	bne _020E2180
	mov r0, r5
	mov r1, r6
	mov r2, r7
	bl CTRDG_VerifyAgbFlash
	cmp r0, #0
	addeq sp, sp, #0x24
	ldmeqia sp!, {r4, r5, r6, r7, pc}
_020E2180:
	add r1, r4, #1
	mov r1, r1, lsl #0x10
	mov r4, r1, lsr #0x10
	cmp r4, #3
	blo _020E2150
	add sp, sp, #0x24
	ldmia sp!, {r4, r5, r6, r7, pc}
	arm_func_end CTRDGi_WriteAndVerifyAgbFlashSectorCore

	arm_func_start CTRDG_ReadAgbFlash
CTRDG_ReadAgbFlash: ; 0x020E219C
	stmdb sp!, {lr}
	sub sp, sp, #0x24
	strh r0, [sp, #0x20]
	add r0, sp, #0
	str r1, [sp, #0x14]
	str r2, [sp, #0x1c]
	str r3, [sp, #0x18]
	bl CTRDGi_ReadAgbFlashCore
	add sp, sp, #0x24
	ldmia sp!, {pc}
	arm_func_end CTRDG_ReadAgbFlash

	arm_func_start CTRDG_WriteAgbFlashSector
CTRDG_WriteAgbFlashSector: ; 0x020E21C4
	stmdb sp!, {r3, lr}
	ldr r2, _020E21D8 ; =ctrdgi_flash_lock_id
	ldr r2, [r2, #4]
	blx r2
	ldmia sp!, {r3, pc}
	.align 2, 0
_020E21D8: .word ctrdgi_flash_lock_id
	arm_func_end CTRDG_WriteAgbFlashSector

	arm_func_start CTRDG_VerifyAgbFlash
CTRDG_VerifyAgbFlash: ; 0x020E21DC
	stmdb sp!, {lr}
	sub sp, sp, #0x24
	strh r0, [sp, #0x20]
	add r0, sp, #0
	str r1, [sp, #0xc]
	str r2, [sp, #0x18]
	bl CTRDGi_VerifyAgbFlashCore
	add sp, sp, #0x24
	ldmia sp!, {pc}
	arm_func_end CTRDG_VerifyAgbFlash

	arm_func_start CTRDG_WriteAndVerifyAgbFlashAsync
CTRDG_WriteAndVerifyAgbFlashAsync:
	stmdb sp!, {lr}
	sub sp, sp, #0x24
	str r1, [sp, #0xc]
	strh r0, [sp, #0x20]
	str r2, [sp, #0x18]
	ldr r1, _020E222C ; =CTRDGi_WriteAndVerifyAgbFlashSectorCore
	add r0, sp, #0
	mov r2, r3
	bl CTRDGi_SetTask
	add sp, sp, #0x24
	ldmia sp!, {pc}
	.align 2, 0
_020E222C: .word CTRDGi_WriteAndVerifyAgbFlashSectorCore
	arm_func_end CTRDG_WriteAndVerifyAgbFlashAsync

	.section .text.unified_44,4,1,6

	arm_func_start CTRDGi_EraseFlashChipCoreLE
CTRDGi_EraseFlashChipCoreLE: ; 0x020E2844
	stmdb sp!, {r3, r4, r5, lr}
	ldr r0, _020E2910 ; =ctrdgi_flash_lock_id
	ldrh r0, [r0]
	bl OS_LockCartridge
	ldr r0, _020E2914 ; =0x04000204
	ldr r1, _020E2918 ; =AgbFlash
	ldrh r3, [r0]
	ldr r2, [r1]
	ldrh r1, [r0]
	ldr r4, [r2, #0x10]
	mov r2, #0
	bic r1, r1, #3
	orr r1, r1, r4
	strh r1, [r0]
	ldrh r4, [r0, #4]
	ldr r1, _020E291C ; =ctrdgi_backup_irq
	ldr lr, _020E2920 ; =0x0A005555
	strh r2, [r0, #4]
	str r4, [r1]
	mov r5, #0xaa
	ldr r4, _020E2924 ; =0x0A002AAA
	strb r5, [lr]
	mov ip, #0x55
	strb ip, [r4]
	mov r2, #0x80
	strb r2, [lr]
	strb r5, [lr]
	strb ip, [r4]
	mov r2, #0x10
	strb r2, [lr]
	ldrh r2, [r0, #4]
	ldr r2, [r1]
	ldr r1, _020E2928 ; =CTRDGi_PollingSR
	strh r2, [r0, #4]
	ldr r4, [r1]
	mov r0, #3
	mov r1, #0xa000000
	mov r2, #0xff
	and r5, r3, #3
	blx r4
	ldr r3, _020E2914 ; =0x04000204
	ldr r1, _020E2910 ; =ctrdgi_flash_lock_id
	ldrh r2, [r3]
	mov r4, r0
	bic r0, r2, #3
	orr r0, r0, r5
	strh r0, [r3]
	ldrh r0, [r1]
	bl OS_UnlockCartridge
	mov r0, r4
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_020E2910: .word ctrdgi_flash_lock_id
_020E2914: .word 0x04000204
_020E2918: .word AgbFlash
_020E291C: .word ctrdgi_backup_irq
_020E2920: .word 0x0A005555
_020E2924: .word 0x0A002AAA
_020E2928: .word CTRDGi_PollingSR
	arm_func_end CTRDGi_EraseFlashChipCoreLE

	arm_func_start CTRDGi_EraseFlashSectorCoreLE
CTRDGi_EraseFlashSectorCoreLE: ; 0x020E292C
	stmdb sp!, {r3, r4, r5, r6, lr}
	sub sp, sp, #0x24
	mov r5, r0
	ldmia r5!, {r0, r1, r2, r3}
	add r4, sp, #0
	stmia r4!, {r0, r1, r2, r3}
	ldmia r5!, {r0, r1, r2, r3}
	stmia r4!, {r0, r1, r2, r3}
	ldr r0, [r5]
	str r0, [r4]
	ldrh r4, [sp, #0x20]
	cmp r4, #0x10
	addhs sp, sp, #0x24
	ldrhs r0, _020E2A40 ; =0x000080FF
	ldmhsia sp!, {r3, r4, r5, r6, pc}
	ldr r0, _020E2A44 ; =ctrdgi_flash_lock_id
	ldrh r0, [r0]
	bl OS_LockCartridge
	ldr r0, _020E2A48 ; =0x04000204
	ldr r1, _020E2A4C ; =AgbFlash
	ldrh r3, [r0]
	ldr r5, [r1]
	ldrh r2, [r0]
	ldr r6, [r5, #0x10]
	mov r5, #0
	bic r2, r2, #3
	orr r2, r2, r6
	strh r2, [r0]
	ldr r2, [r1]
	ldrh r1, [r0, #4]
	ldrh ip, [r2, #8]
	ldr r2, _020E2A50 ; =ctrdgi_backup_irq
	ldr lr, _020E2A54 ; =0x0A005555
	strh r5, [r0, #4]
	mov r6, r4, lsl ip
	str r1, [r2]
	mov r5, #0xaa
	ldr r4, _020E2A58 ; =0x0A002AAA
	strb r5, [lr]
	mov ip, #0x55
	strb ip, [r4]
	mov r1, #0x80
	strb r1, [lr]
	strb r5, [lr]
	strb ip, [r4]
	add r1, r6, #0xa000000
	mov r4, #0x30
	strb r4, [r1]
	ldrh r4, [r0, #4]
	ldr r4, [r2]
	ldr r2, _020E2A5C ; =CTRDGi_PollingSR
	strh r4, [r0, #4]
	ldr r4, [r2]
	mov r0, #2
	mov r2, #0xff
	and r5, r3, #3
	blx r4
	ldr r3, _020E2A48 ; =0x04000204
	ldr r1, _020E2A44 ; =ctrdgi_flash_lock_id
	ldrh r2, [r3]
	mov r4, r0
	bic r0, r2, #3
	orr r0, r0, r5
	strh r0, [r3]
	ldrh r0, [r1]
	bl OS_UnlockCartridge
	mov r0, r4
	add sp, sp, #0x24
	ldmia sp!, {r3, r4, r5, r6, pc}
	.align 2, 0
_020E2A40: .word 0x000080FF
_020E2A44: .word ctrdgi_flash_lock_id
_020E2A48: .word 0x04000204
_020E2A4C: .word AgbFlash
_020E2A50: .word ctrdgi_backup_irq
_020E2A54: .word 0x0A005555
_020E2A58: .word 0x0A002AAA
_020E2A5C: .word CTRDGi_PollingSR
	arm_func_end CTRDGi_EraseFlashSectorCoreLE

	arm_func_start CTRDGi_ProgramFlashByteLE
CTRDGi_ProgramFlashByteLE: ; 0x020E2A60
	stmdb sp!, {r3, lr}
	ldr ip, _020E2AA8 ; =0x0A005555
	mov r3, #0xaa
	ldr r2, _020E2AAC ; =0x0A002AAA
	strb r3, [ip]
	mov r3, #0x55
	mov lr, r0
	strb r3, [r2]
	mov r0, #0xa0
	strb r0, [ip]
	ldrb r2, [lr]
	ldr r3, _020E2AB0 ; =CTRDGi_PollingSR
	mov r0, #1
	strb r2, [r1]
	ldrb r2, [lr]
	ldr r3, [r3]
	blx r3
	ldmia sp!, {r3, pc}
	.align 2, 0
_020E2AA8: .word 0x0A005555
_020E2AAC: .word 0x0A002AAA
_020E2AB0: .word CTRDGi_PollingSR
	arm_func_end CTRDGi_ProgramFlashByteLE

	arm_func_start CTRDGi_WriteFlashSectorCoreLE
CTRDGi_WriteFlashSectorCoreLE: ; 0x020E2AB4
	stmdb sp!, {r4, r5, r6, r7, r8, sb, lr}
	sub sp, sp, #0x24
	mov r5, r0
	ldmia r5!, {r0, r1, r2, r3}
	add r4, sp, #0
	stmia r4!, {r0, r1, r2, r3}
	ldmia r5!, {r0, r1, r2, r3}
	stmia r4!, {r0, r1, r2, r3}
	ldr r0, [r5]
	str r0, [r4]
	ldrh r8, [sp, #0x20]
	ldr r5, [sp, #0xc]
	cmp r8, #0x10
	addhs sp, sp, #0x24
	ldrhs r0, _020E2C64 ; =0x000080FF
	ldmhsia sp!, {r4, r5, r6, r7, r8, sb, pc}
	ldr r0, _020E2C68 ; =AgbFlash
	ldr r6, _020E2C6C ; =CTRDGi_VerifyFlashCoreFF
	ldr r0, [r0]
	mov sb, #0
	ldrh r0, [r0, #8]
	mov r0, r8, lsl r0
	add r7, r0, #0xa000000
_020E2B10:
	mov r0, r8
	bl CTRDGi_EraseFlashSectorLE
	movs r4, r0
	bne _020E2B38
	mov r0, r7
	mov r1, r6
	bl CTRDGi_VerifyFlashErase
	mov r0, r0, lsl #0x10
	movs r4, r0, lsr #0x10
	beq _020E2B58
_020E2B38:
	add r0, sb, #1
	mov r0, r0, lsl #0x10
	cmp sb, #0x50
	mov sb, r0, lsr #0x10
	bne _020E2B10
	add sp, sp, #0x24
	mov r0, r4
	ldmia sp!, {r4, r5, r6, r7, r8, sb, pc}
_020E2B58:
	mov r6, #1
	cmp sb, #0
	movne r6, #6
	cmp r6, #1
	mov sb, #1
	blo _020E2B8C
_020E2B70:
	mov r0, r8
	bl CTRDGi_EraseFlashSectorLE
	add r0, sb, #1
	mov r0, r0, lsl #0x10
	cmp r6, r0, lsr #16
	mov sb, r0, lsr #0x10
	bhs _020E2B70
_020E2B8C:
	ldr r0, _020E2C70 ; =ctrdgi_flash_lock_id
	ldrh r0, [r0]
	bl OS_LockCartridge
	ldr r1, _020E2C74 ; =0x04000204
	ldr r2, _020E2C68 ; =AgbFlash
	ldrh r0, [r1]
	ldr r6, [r2]
	ldrh r3, [r1]
	ldr r8, [r6, #0x10]
	ldr r6, _020E2C78 ; =ctrdg_flash_remainder
	bic r3, r3, #3
	orr r3, r3, r8
	strh r3, [r1]
	ldr r2, [r2]
	mov r3, #0
	ldr r8, [r2, #4]
	ldr r2, _020E2C7C ; =ctrdgi_backup_irq
	strh r8, [r6]
	ldrh sb, [r1, #4]
	and r8, r0, #3
	strh r3, [r1, #4]
	ldrh r0, [r6]
	str sb, [r2]
	cmp r0, #0
	beq _020E2C24
_020E2BF0:
	mov r0, r5
	mov r1, r7
	bl CTRDGi_ProgramFlashByteLE
	movs r4, r0
	bne _020E2C24
	ldrh r0, [r6]
	add r5, r5, #1
	add r7, r7, #1
	sub r0, r0, #1
	strh r0, [r6]
	ldrh r0, [r6]
	cmp r0, #0
	bne _020E2BF0
_020E2C24:
	ldr r3, _020E2C80 ; =0x04000208
	ldr r0, _020E2C7C ; =ctrdgi_backup_irq
	ldrh r1, [r3]
	ldr r0, [r0]
	sub r2, r3, #4
	strh r0, [r3]
	ldrh r1, [r2]
	ldr r0, _020E2C70 ; =ctrdgi_flash_lock_id
	bic r1, r1, #3
	orr r1, r1, r8
	strh r1, [r2]
	ldrh r0, [r0]
	bl OS_UnlockCartridge
	mov r0, r4
	add sp, sp, #0x24
	ldmia sp!, {r4, r5, r6, r7, r8, sb, pc}
	.align 2, 0
_020E2C64: .word 0x000080FF
_020E2C68: .word AgbFlash
_020E2C6C: .word CTRDGi_VerifyFlashCoreFF
_020E2C70: .word ctrdgi_flash_lock_id
_020E2C74: .word 0x04000204
_020E2C78: .word ctrdg_flash_remainder
_020E2C7C: .word ctrdgi_backup_irq
_020E2C80: .word 0x04000208
	arm_func_end CTRDGi_WriteFlashSectorCoreLE

	arm_func_start CTRDGi_VerifyFlashCoreFF
CTRDGi_VerifyFlashCoreFF: ; 0x020E2C84
	ldr r1, _020E2CB4 ; =AgbFlash
	ldr r1, [r1]
	ldr r2, [r1, #4]
	cmp r2, #0
	beq _020E2CAC
_020E2C98:
	ldrb r1, [r0], #1
	cmp r1, #0xff
	bne _020E2CAC
	subs r2, r2, #1
	bne _020E2C98
_020E2CAC:
	mov r0, r2
	bx lr
	.align 2, 0
_020E2CB4: .word AgbFlash
	arm_func_end CTRDGi_VerifyFlashCoreFF

	arm_func_start CTRDGi_VerifyFlashErase
CTRDGi_VerifyFlashErase: ; 0x020E2CB8
	stmdb sp!, {r4, r5, r6, lr}
	ldr r2, _020E2D34 ; =ctrdgi_flash_lock_id
	mov r5, r0
	ldrh r0, [r2]
	mov r4, r1
	bl OS_LockCartridge
	ldr ip, _020E2D38 ; =0x04000204
	ldr r0, _020E2D3C ; =AgbFlash
	ldrh r3, [ip]
	ldr r0, [r0]
	ldrh r1, [ip]
	ldr r2, [r0, #0x10]
	mov r0, r5
	bic r1, r1, #3
	orr r1, r1, r2
	strh r1, [ip]
	and r6, r3, #3
	mov r5, #0
	blx r4
	cmp r0, #0
	ldr r2, _020E2D38 ; =0x04000204
	ldr r0, _020E2D34 ; =ctrdgi_flash_lock_id
	ldrh r1, [r2]
	ldrne r5, _020E2D40 ; =0x00008004
	bic r1, r1, #3
	orr r1, r1, r6
	strh r1, [r2]
	ldrh r0, [r0]
	bl OS_UnlockCartridge
	mov r0, r5
	ldmia sp!, {r4, r5, r6, pc}
	.align 2, 0
_020E2D34: .word ctrdgi_flash_lock_id
_020E2D38: .word 0x04000204
_020E2D3C: .word AgbFlash
_020E2D40: .word 0x00008004
	arm_func_end CTRDGi_VerifyFlashErase

	arm_func_start CTRDGi_EraseFlashChipLE
CTRDGi_EraseFlashChipLE: ; 0x020E2D44
	stmdb sp!, {lr}
	sub sp, sp, #0x24
	add r0, sp, #0
	bl CTRDGi_EraseFlashChipCoreLE
	mov r0, r0, lsl #0x10
	mov r0, r0, lsr #0x10
	add sp, sp, #0x24
	ldmia sp!, {pc}
	arm_func_end CTRDGi_EraseFlashChipLE

	arm_func_start CTRDGi_EraseFlashSectorLE
CTRDGi_EraseFlashSectorLE: ; 0x020E2D64
	stmdb sp!, {lr}
	sub sp, sp, #0x24
	strh r0, [sp, #0x20]
	add r0, sp, #0
	bl CTRDGi_EraseFlashSectorCoreLE
	mov r0, r0, lsl #0x10
	mov r0, r0, lsr #0x10
	add sp, sp, #0x24
	ldmia sp!, {pc}
	arm_func_end CTRDGi_EraseFlashSectorLE

	arm_func_start CTRDGi_WriteFlashSectorLE
CTRDGi_WriteFlashSectorLE:
	stmdb sp!, {lr}
	sub sp, sp, #0x24
	strh r0, [sp, #0x20]
	add r0, sp, #0
	str r1, [sp, #0xc]
	bl CTRDGi_WriteFlashSectorCoreLE
	mov r0, r0, lsl #0x10
	mov r0, r0, lsr #0x10
	add sp, sp, #0x24
	ldmia sp!, {pc}
	arm_func_end CTRDGi_WriteFlashSectorLE

	arm_func_start CTRDGi_EraseFlashChipAsyncLE
CTRDGi_EraseFlashChipAsyncLE:
	stmdb sp!, {lr}
	sub sp, sp, #0x24
	ldr r1, _020E2DD0 ; =CTRDGi_EraseFlashChipCoreLE
	mov r2, r0
	add r0, sp, #0
	bl CTRDGi_SetTask
	add sp, sp, #0x24
	ldmia sp!, {pc}
	.align 2, 0
_020E2DD0: .word CTRDGi_EraseFlashChipCoreLE
	arm_func_end CTRDGi_EraseFlashChipAsyncLE

	arm_func_start CTRDGi_EraseFlashSectorAsyncLE
CTRDGi_EraseFlashSectorAsyncLE: ; 0x020E2DD4
	stmdb sp!, {lr}
	sub sp, sp, #0x24
	mov r2, r1
	strh r0, [sp, #0x20]
	ldr r1, _020E2DF8 ; =CTRDGi_EraseFlashSectorCoreLE
	add r0, sp, #0
	bl CTRDGi_SetTask
	add sp, sp, #0x24
	ldmia sp!, {pc}
	.align 2, 0
_020E2DF8: .word CTRDGi_EraseFlashSectorCoreLE
	arm_func_end CTRDGi_EraseFlashSectorAsyncLE

	arm_func_start CTRDGi_WriteFlashSectorAsyncLE
CTRDGi_WriteFlashSectorAsyncLE: ; 0x020E2DFC
	stmdb sp!, {lr}
	sub sp, sp, #0x24
	str r1, [sp, #0xc]
	strh r0, [sp, #0x20]
	ldr r1, _020E2E20 ; =CTRDGi_WriteFlashSectorCoreLE
	add r0, sp, #0
	bl CTRDGi_SetTask
	add sp, sp, #0x24
	ldmia sp!, {pc}
	.align 2, 0
_020E2E20: .word CTRDGi_WriteFlashSectorCoreLE
	arm_func_end CTRDGi_WriteFlashSectorAsyncLE

	arm_func_start CTRDGi_EraseFlashChipCoreMX
CTRDGi_EraseFlashChipCoreMX: ; 0x020E2E24
	stmdb sp!, {r3, r4, r5, lr}
	ldr r0, _020E2EF0 ; =ctrdgi_flash_lock_id
	ldrh r0, [r0]
	bl OS_LockCartridge
	ldr r0, _020E2EF4 ; =0x04000204
	ldr r1, _020E2EF8 ; =AgbFlash
	ldrh r3, [r0]
	ldr r2, [r1]
	ldrh r1, [r0]
	ldr r4, [r2, #0x10]
	mov r2, #0
	bic r1, r1, #3
	orr r1, r1, r4
	strh r1, [r0]
	ldrh r4, [r0, #4]
	ldr r1, _020E2EFC ; =ctrdgi_backup_irq
	ldr lr, _020E2F00 ; =0x0A005555
	strh r2, [r0, #4]
	str r4, [r1]
	mov r5, #0xaa
	ldr r4, _020E2F04 ; =0x0A002AAA
	strb r5, [lr]
	mov ip, #0x55
	strb ip, [r4]
	mov r2, #0x80
	strb r2, [lr]
	strb r5, [lr]
	strb ip, [r4]
	mov r2, #0x10
	strb r2, [lr]
	ldrh r2, [r0, #4]
	ldr r2, [r1]
	ldr r1, _020E2F08 ; =CTRDGi_PollingSR
	strh r2, [r0, #4]
	ldr r4, [r1]
	mov r0, #3
	mov r1, #0xa000000
	mov r2, #0xff
	and r5, r3, #3
	blx r4
	ldr r3, _020E2EF4 ; =0x04000204
	ldr r1, _020E2EF0 ; =ctrdgi_flash_lock_id
	ldrh r2, [r3]
	mov r4, r0
	bic r0, r2, #3
	orr r0, r0, r5
	strh r0, [r3]
	ldrh r0, [r1]
	bl OS_UnlockCartridge
	mov r0, r4
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_020E2EF0: .word ctrdgi_flash_lock_id
_020E2EF4: .word 0x04000204
_020E2EF8: .word AgbFlash
_020E2EFC: .word ctrdgi_backup_irq
_020E2F00: .word 0x0A005555
_020E2F04: .word 0x0A002AAA
_020E2F08: .word CTRDGi_PollingSR
	arm_func_end CTRDGi_EraseFlashChipCoreMX

	arm_func_start CTRDGi_EraseFlashSectorCoreMX
CTRDGi_EraseFlashSectorCoreMX: ; 0x020E2F0C
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x28
	mov r5, r0
	ldmia r5!, {r0, r1, r2, r3}
	add r4, sp, #4
	stmia r4!, {r0, r1, r2, r3}
	ldmia r5!, {r0, r1, r2, r3}
	stmia r4!, {r0, r1, r2, r3}
	ldr r0, _020E3074 ; =AgbFlash
	ldr r1, [r5]
	ldr r0, [r0]
	str r1, [r4]
	ldrh r4, [sp, #0x24]
	ldrh r0, [r0, #0xa]
	cmp r4, r0
	addhs sp, sp, #0x28
	ldrhs r0, _020E3078 ; =0x000080FF
	ldmhsia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldr r0, _020E307C ; =ctrdgi_flash_lock_id
	ldrh r0, [r0]
	bl OS_LockCartridge
	ldr r5, _020E3080 ; =0x04000204
	ldr r0, _020E3074 ; =AgbFlash
	ldrh r3, [r5]
	ldr r2, [r0]
	ldrh r1, [r5]
	mov r0, r4, lsl #0xc
	ldr r2, [r2, #0x10]
	bic r1, r1, #3
	orr r1, r1, r2
	strh r1, [r5]
	and r1, r3, #3
	mov r0, r0, lsr #0x10
	str r1, [sp]
	bl CTRDGi_SetFlashBankMx
	and sl, r4, #0xf
	mov r8, #0
	add r7, r5, #4
	ldr r5, _020E3084 ; =0x0A005555
	ldr r4, _020E3088 ; =ctrdgi_backup_irq
	mov fp, r8
	mov r6, #0xaa
_020E2FB4:
	ldr r0, _020E3074 ; =AgbFlash
	ldrh r3, [r7]
	ldr r0, [r0]
	mov sb, #0x55
	ldrh r1, [r0, #8]
	mov r0, #2
	mov r2, #0xff
	strh fp, [r7]
	str r3, [r4]
	ldr r3, _020E308C ; =0x0A002AAA
	strb r6, [r5]
	strb sb, [r3]
	mov r3, #0x80
	strb r3, [r5]
	mov r1, sl, lsl r1
	ldr r3, _020E308C ; =0x0A002AAA
	strb r6, [r5]
	strb sb, [r3]
	add r1, r1, #0xa000000
	mov r3, #0x30
	strb r3, [r1]
	ldrh r3, [r7]
	ldr r3, [r4]
	strh r3, [r7]
	ldr r3, _020E3090 ; =CTRDGi_PollingSR
	ldr r3, [r3]
	blx r3
	mov sb, r0
	tst sb, #0xa000
	beq _020E3044
	cmp r8, #0
	bne _020E3044
	add r0, r8, #1
	mov r0, r0, lsl #0x10
	mov r8, r0, lsr #0x10
	b _020E2FB4
_020E3044:
	ldr r2, _020E3080 ; =0x04000204
	ldr r1, _020E307C ; =ctrdgi_flash_lock_id
	ldrh r0, [r2]
	bic r3, r0, #3
	ldr r0, [sp]
	orr r0, r3, r0
	strh r0, [r2]
	ldrh r0, [r1]
	bl OS_UnlockCartridge
	mov r0, sb
	add sp, sp, #0x28
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_020E3074: .word AgbFlash
_020E3078: .word 0x000080FF
_020E307C: .word ctrdgi_flash_lock_id
_020E3080: .word 0x04000204
_020E3084: .word 0x0A005555
_020E3088: .word ctrdgi_backup_irq
_020E308C: .word 0x0A002AAA
_020E3090: .word CTRDGi_PollingSR
	arm_func_end CTRDGi_EraseFlashSectorCoreMX

	arm_func_start CTRDGi_ProgramFlashByteMX
CTRDGi_ProgramFlashByteMX: ; 0x020E3094
	stmdb sp!, {r3, lr}
	ldr ip, _020E30DC ; =0x0A005555
	mov r3, #0xaa
	ldr r2, _020E30E0 ; =0x0A002AAA
	strb r3, [ip]
	mov r3, #0x55
	mov lr, r0
	strb r3, [r2]
	mov r0, #0xa0
	strb r0, [ip]
	ldrb r2, [lr]
	ldr r3, _020E30E4 ; =CTRDGi_PollingSR
	mov r0, #1
	strb r2, [r1]
	ldrb r2, [lr]
	ldr r3, [r3]
	blx r3
	ldmia sp!, {r3, pc}
	.align 2, 0
_020E30DC: .word 0x0A005555
_020E30E0: .word 0x0A002AAA
_020E30E4: .word CTRDGi_PollingSR
	arm_func_end CTRDGi_ProgramFlashByteMX

	arm_func_start CTRDGi_WriteFlashSectorCoreMX
CTRDGi_WriteFlashSectorCoreMX: ; 0x020E30E8
	stmdb sp!, {r3, r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0x24
	mov r5, r0
	ldmia r5!, {r0, r1, r2, r3}
	add r4, sp, #0
	stmia r4!, {r0, r1, r2, r3}
	ldmia r5!, {r0, r1, r2, r3}
	stmia r4!, {r0, r1, r2, r3}
	ldr r0, _020E3244 ; =AgbFlash
	ldr r1, [r5]
	ldr r0, [r0]
	str r1, [r4]
	ldrh r4, [sp, #0x20]
	ldrh r0, [r0, #0xa]
	ldr r7, [sp, #0xc]
	cmp r4, r0
	addhs sp, sp, #0x24
	ldrhs r0, _020E3248 ; =0x000080FF
	ldmhsia sp!, {r3, r4, r5, r6, r7, r8, pc}
	mov r0, r4
	bl CTRDGi_EraseFlashSectorMX
	movs r6, r0
	addne sp, sp, #0x24
	ldmneia sp!, {r3, r4, r5, r6, r7, r8, pc}
	ldr r0, _020E324C ; =ctrdgi_flash_lock_id
	ldrh r0, [r0]
	bl OS_LockCartridge
	ldr r5, _020E3250 ; =0x04000204
	ldr r0, _020E3244 ; =AgbFlash
	ldrh r3, [r5]
	ldr r2, [r0]
	ldrh r1, [r5]
	mov r0, r4, lsl #0xc
	ldr r2, [r2, #0x10]
	bic r1, r1, #3
	orr r1, r1, r2
	strh r1, [r5]
	mov r0, r0, lsr #0x10
	and r5, r3, #3
	bl CTRDGi_SetFlashBankMx
	ldr r0, _020E3244 ; =AgbFlash
	ldr r8, _020E3254 ; =ctrdg_flash_remainder
	ldr r2, [r0]
	ldr r1, _020E3258 ; =0x04000208
	ldr r0, [r2, #4]
	and r4, r4, #0xf
	strh r0, [r8]
	ldrh r3, [r2, #8]
	ldrh r2, [r1]
	mov r0, #0
	mov r3, r4, lsl r3
	strh r0, [r1]
	ldrh r1, [r8]
	ldr r0, _020E325C ; =ctrdgi_backup_irq
	add r4, r3, #0xa000000
	str r2, [r0]
	cmp r1, #0
	beq _020E3204
_020E31D0:
	mov r0, r7
	mov r1, r4
	bl CTRDGi_ProgramFlashByteMX
	movs r6, r0
	bne _020E3204
	ldrh r0, [r8]
	add r7, r7, #1
	add r4, r4, #1
	sub r0, r0, #1
	strh r0, [r8]
	ldrh r0, [r8]
	cmp r0, #0
	bne _020E31D0
_020E3204:
	ldr r3, _020E3258 ; =0x04000208
	ldr r0, _020E325C ; =ctrdgi_backup_irq
	ldrh r1, [r3]
	ldr r0, [r0]
	sub r2, r3, #4
	strh r0, [r3]
	ldrh r1, [r2]
	ldr r0, _020E324C ; =ctrdgi_flash_lock_id
	bic r1, r1, #3
	orr r1, r1, r5
	strh r1, [r2]
	ldrh r0, [r0]
	bl OS_UnlockCartridge
	mov r0, r6
	add sp, sp, #0x24
	ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
	.align 2, 0
_020E3244: .word AgbFlash
_020E3248: .word 0x000080FF
_020E324C: .word ctrdgi_flash_lock_id
_020E3250: .word 0x04000204
_020E3254: .word ctrdg_flash_remainder
_020E3258: .word 0x04000208
_020E325C: .word ctrdgi_backup_irq
	arm_func_end CTRDGi_WriteFlashSectorCoreMX

	arm_func_start CTRDGi_EraseFlashChipMX
CTRDGi_EraseFlashChipMX: ; 0x020E3260
	stmdb sp!, {lr}
	sub sp, sp, #0x24
	add r0, sp, #0
	bl CTRDGi_EraseFlashChipCoreMX
	mov r0, r0, lsl #0x10
	mov r0, r0, lsr #0x10
	add sp, sp, #0x24
	ldmia sp!, {pc}
	arm_func_end CTRDGi_EraseFlashChipMX

	arm_func_start CTRDGi_EraseFlashSectorMX
CTRDGi_EraseFlashSectorMX: ; 0x020E3280
	stmdb sp!, {lr}
	sub sp, sp, #0x24
	strh r0, [sp, #0x20]
	add r0, sp, #0
	bl CTRDGi_EraseFlashSectorCoreMX
	mov r0, r0, lsl #0x10
	mov r0, r0, lsr #0x10
	add sp, sp, #0x24
	ldmia sp!, {pc}
	arm_func_end CTRDGi_EraseFlashSectorMX

	arm_func_start CTRDGi_WriteFlashSectorMX
CTRDGi_WriteFlashSectorMX:
	stmdb sp!, {lr}
	sub sp, sp, #0x24
	strh r0, [sp, #0x20]
	add r0, sp, #0
	str r1, [sp, #0xc]
	bl CTRDGi_WriteFlashSectorCoreMX
	mov r0, r0, lsl #0x10
	mov r0, r0, lsr #0x10
	add sp, sp, #0x24
	ldmia sp!, {pc}
	arm_func_end CTRDGi_WriteFlashSectorMX

	arm_func_start CTRDGi_EraseFlashChipAsyncMX
CTRDGi_EraseFlashChipAsyncMX:
	stmdb sp!, {lr}
	sub sp, sp, #0x24
	ldr r1, _020E32EC ; =CTRDGi_EraseFlashChipCoreMX
	mov r2, r0
	add r0, sp, #0
	bl CTRDGi_SetTask
	add sp, sp, #0x24
	ldmia sp!, {pc}
	.align 2, 0
_020E32EC: .word CTRDGi_EraseFlashChipCoreMX
	arm_func_end CTRDGi_EraseFlashChipAsyncMX

	arm_func_start CTRDGi_EraseFlashSectorAsyncMX
CTRDGi_EraseFlashSectorAsyncMX: ; 0x020E32F0
	stmdb sp!, {lr}
	sub sp, sp, #0x24
	mov r2, r1
	strh r0, [sp, #0x20]
	ldr r1, _020E3314 ; =CTRDGi_EraseFlashSectorCoreMX
	add r0, sp, #0
	bl CTRDGi_SetTask
	add sp, sp, #0x24
	ldmia sp!, {pc}
	.align 2, 0
_020E3314: .word CTRDGi_EraseFlashSectorCoreMX
	arm_func_end CTRDGi_EraseFlashSectorAsyncMX

	arm_func_start CTRDGi_WriteFlashSectorAsyncMX
CTRDGi_WriteFlashSectorAsyncMX: ; 0x020E3318
	stmdb sp!, {lr}
	sub sp, sp, #0x24
	str r1, [sp, #0xc]
	strh r0, [sp, #0x20]
	ldr r1, _020E333C ; =CTRDGi_WriteFlashSectorCoreMX
	add r0, sp, #0
	bl CTRDGi_SetTask
	add sp, sp, #0x24
	ldmia sp!, {pc}
	.align 2, 0
_020E333C: .word CTRDGi_WriteFlashSectorCoreMX
	arm_func_end CTRDGi_WriteFlashSectorAsyncMX

	arm_func_start CTRDGi_WriteFlashSectorCoreMX5
CTRDGi_WriteFlashSectorCoreMX5: ; 0x020E3340
	stmdb sp!, {r3, r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0x24
	mov r5, r0
	ldmia r5!, {r0, r1, r2, r3}
	add r4, sp, #0
	stmia r4!, {r0, r1, r2, r3}
	ldmia r5!, {r0, r1, r2, r3}
	stmia r4!, {r0, r1, r2, r3}
	ldr r0, _020E345C ; =AgbFlash
	ldr r1, [r5]
	ldr r0, [r0]
	str r1, [r4]
	ldrh r4, [sp, #0x20]
	ldrh r0, [r0, #0xa]
	ldr r7, [sp, #0xc]
	cmp r4, r0
	addhs sp, sp, #0x24
	ldrhs r0, _020E3460 ; =0x000080FF
	ldmhsia sp!, {r3, r4, r5, r6, r7, r8, pc}
	mov r0, r4
	bl CTRDGi_EraseFlashSectorLE
	movs r6, r0
	addne sp, sp, #0x24
	ldmneia sp!, {r3, r4, r5, r6, r7, r8, pc}
	ldr r0, _020E3464 ; =ctrdgi_flash_lock_id
	ldrh r0, [r0]
	bl OS_LockCartridge
	ldr r5, _020E3468 ; =0x04000204
	ldr r0, _020E345C ; =AgbFlash
	ldrh r3, [r5]
	ldr r2, [r0]
	ldrh r1, [r5]
	ldr r2, [r2, #0x10]
	ldr r8, _020E346C ; =ctrdg_flash_remainder
	bic r1, r1, #3
	orr r1, r1, r2
	strh r1, [r5]
	ldr r1, [r0]
	and r5, r3, #3
	ldr r0, [r1, #4]
	strh r0, [r8]
	ldrh r1, [r1, #8]
	ldrh r0, [r8]
	mov r1, r4, lsl r1
	cmp r0, #0
	add r4, r1, #0xa000000
	beq _020E3430
_020E33FC:
	mov r0, r7
	mov r1, r4
	bl CTRDGi_ProgramFlashByteLE
	movs r6, r0
	bne _020E3430
	ldrh r0, [r8]
	add r7, r7, #1
	add r4, r4, #1
	sub r0, r0, #1
	strh r0, [r8]
	ldrh r0, [r8]
	cmp r0, #0
	bne _020E33FC
_020E3430:
	ldr r2, _020E3468 ; =0x04000204
	ldr r0, _020E3464 ; =ctrdgi_flash_lock_id
	ldrh r1, [r2]
	bic r1, r1, #3
	orr r1, r1, r5
	strh r1, [r2]
	ldrh r0, [r0]
	bl OS_UnlockCartridge
	mov r0, r6
	add sp, sp, #0x24
	ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
	.align 2, 0
_020E345C: .word AgbFlash
_020E3460: .word 0x000080FF
_020E3464: .word ctrdgi_flash_lock_id
_020E3468: .word 0x04000204
_020E346C: .word ctrdg_flash_remainder
	arm_func_end CTRDGi_WriteFlashSectorCoreMX5

	arm_func_start CTRDGi_WriteFlashSectorMX5
CTRDGi_WriteFlashSectorMX5: ; 0x020E3470
	stmdb sp!, {lr}
	sub sp, sp, #0x24
	strh r0, [sp, #0x20]
	add r0, sp, #0
	str r1, [sp, #0xc]
	bl CTRDGi_WriteFlashSectorCoreMX5
	mov r0, r0, lsl #0x10
	mov r0, r0, lsr #0x10
	add sp, sp, #0x24
	ldmia sp!, {pc}
	arm_func_end CTRDGi_WriteFlashSectorMX5

	arm_func_start CTRDGi_WriteFlashSectorAsyncMX5
CTRDGi_WriteFlashSectorAsyncMX5:
	stmdb sp!, {lr}
	sub sp, sp, #0x24
	str r1, [sp, #0xc]
	strh r0, [sp, #0x20]
	ldr r1, _020E34BC ; =CTRDGi_WriteFlashSectorCoreMX5
	add r0, sp, #0
	bl CTRDGi_SetTask
	add sp, sp, #0x24
	ldmia sp!, {pc}
	.align 2, 0
_020E34BC: .word CTRDGi_WriteFlashSectorCoreMX5
	arm_func_end CTRDGi_WriteFlashSectorAsyncMX5

	arm_func_start CTRDGi_InitTaskThread
CTRDGi_InitTaskThread: ; 0x020E34C0
	stmdb sp!, {r3, r4, r5, lr}
	sub sp, sp, #8
	mov r5, r0
	bl OS_DisableInterrupts
	ldr r1, _020E353C ; =_021E4F68
	mov r4, r0
	ldr r0, [r1]
	cmp r0, #0
	bne _020E352C
	add r0, r5, #0xc4
	str r5, [r1]
	bl CTRDGi_InitTaskInfo
	ldr r0, _020E3540 ; =_021E4F6C
	bl CTRDGi_InitTaskInfo
	mov r0, #0
	str r0, [r5, #0xc0]
	mov r2, #0x400
	ldr r1, _020E3544 ; =CTRDGi_TaskThread
	ldr r3, _020E3548 ; =_021E4F90 + 0x400
	mov r0, r5
	str r2, [sp]
	mov r2, #0x14
	str r2, [sp, #4]
	mov r2, r5
	bl OS_CreateThread
	mov r0, r5
	bl OS_WakeupThreadDirect
_020E352C:
	mov r0, r4
	bl OS_RestoreInterrupts
	add sp, sp, #8
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_020E353C: .word _021E4F68
_020E3540: .word _021E4F6C
_020E3544: .word CTRDGi_TaskThread
_020E3548: .word _021E4F90 + 0x400
	arm_func_end CTRDGi_InitTaskThread

	arm_func_start CTRDGi_IsTaskAvailable
CTRDGi_IsTaskAvailable: ; 0x020E354C
	ldr r0, _020E3564 ; =_021E4F68
	ldr r0, [r0]
	cmp r0, #0
	movne r0, #1
	moveq r0, #0
	bx lr
	.align 2, 0
_020E3564: .word _021E4F68
	arm_func_end CTRDGi_IsTaskAvailable

	arm_func_start CTRDGi_InitTaskInfo
CTRDGi_InitTaskInfo: ; 0x020E3568
	ldr ip, _020E3578 ; =MI_CpuFill8
	mov r1, #0
	mov r2, #0x24
	bx ip
	.align 2, 0
_020E3578: .word MI_CpuFill8
	arm_func_end CTRDGi_InitTaskInfo

	arm_func_start CTRDGi_TaskThread
CTRDGi_TaskThread: ; 0x020E357C
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x24
	mov r4, #0
	mov r8, r0
	add r6, sp, #0
	mov sl, r4
	mov sb, r4
	mov r5, r4
	mov fp, #0x24
_020E35A0:
	mov r0, r6
	mov r1, r5
	mov r2, fp
	bl MI_CpuFill8
	bl OS_DisableInterrupts
	ldr r1, [r8, #0xc0]
	mov r7, r0
	cmp r1, #0
	bne _020E35D8
_020E35C4:
	mov r0, r4
	bl OS_SleepThread
	ldr r0, [r8, #0xc0]
	cmp r0, #0
	beq _020E35C4
_020E35D8:
	ldr lr, [r8, #0xc0]
	add ip, sp, #0
	ldmia lr!, {r0, r1, r2, r3}
	stmia ip!, {r0, r1, r2, r3}
	ldmia lr!, {r0, r1, r2, r3}
	stmia ip!, {r0, r1, r2, r3}
	ldr r1, [lr]
	mov r0, r7
	str r1, [ip]
	bl OS_RestoreInterrupts
	ldr r1, [sp]
	cmp r1, #0
	beq _020E3618
	mov r0, r6
	blx r1
	str r0, [sp, #8]
_020E3618:
	bl OS_DisableInterrupts
	ldr r2, [sp, #4]
	ldr r1, _020E3668 ; =_021E4F68
	mov r7, r0
	strb sl, [r1, #0x26]
	cmp r2, #0
	beq _020E363C
	mov r0, r6
	blx r2
_020E363C:
	ldr r0, _020E3668 ; =_021E4F68
	ldr r0, [r0]
	cmp r0, #0
	beq _020E365C
	mov r0, r7
	str sb, [r8, #0xc0]
	bl OS_RestoreInterrupts
	b _020E35A0
_020E365C:
	bl OS_ExitThread
	add sp, sp, #0x24
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_020E3668: .word _021E4F68
	arm_func_end CTRDGi_TaskThread

	arm_func_start CTRDGi_SetTask
CTRDGi_SetTask: ; 0x020E366C
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	ldr r3, _020E370C ; =_021E4F68
	mov r7, r0
	mov r5, r1
	mov r6, r2
	ldr r4, [r3]
	bl CTRDGi_IsTaskAvailable
	cmp r0, #0
	bne _020E3694
	bl OS_Terminate
_020E3694:
	ldr r0, _020E370C ; =_021E4F68
	ldrb r0, [r0, #0x26]
	cmp r0, #0
	beq _020E36A8
	bl OS_Terminate
_020E36A8:
	bl OS_DisableInterrupts
	mov r1, #1
	strb r1, [r7, #0x22]
	str r5, [r7]
	add r1, r4, #0xc4
	cmp r7, r1
	mov r5, r0
	ldreq r0, _020E370C ; =_021E4F68
	str r6, [r7, #4]
	moveq r1, #0
	streq r1, [r0]
	ldr ip, _020E3710 ; =_021E4F6C
	ldmia r7!, {r0, r1, r2, r3}
	mov r6, ip
	stmia ip!, {r0, r1, r2, r3}
	ldmia r7!, {r0, r1, r2, r3}
	stmia ip!, {r0, r1, r2, r3}
	ldr r1, [r7]
	mov r0, r4
	str r1, [ip]
	str r6, [r4, #0xc0]
	bl OS_WakeupThreadDirect
	mov r0, r5
	bl OS_RestoreInterrupts
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.align 2, 0
_020E370C: .word _021E4F68
_020E3710: .word _021E4F6C
	arm_func_end CTRDGi_SetTask

	.section .text.unified_46,4,1,6

	arm_func_start MATH_QSort
MATH_QSort: ; 0x020E3A84
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	cmp r1, #1
	ble _020E3D6C
	ldr r4, [sp, #0x24]
	mov fp, r3
	mov r8, r2
	cmp r4, #0
	bne _020E3ABC
	clz r2, r1
	rsb r2, r2, #0x20
	mov r2, r2, lsl #3
	sub sp, sp, r2
	mov r4, sp
	str r2, [sp, #-4]!
_020E3ABC:
	sub r1, r1, #1
	mla r1, r1, r8, r0
	mov r5, r4
	str r0, [r4], #4
	str r1, [r4], #4
	clz r2, r8
	rsb r2, r2, #0x20
	str r2, [sp, #-4]!
_020E3ADC:
	cmp r4, r5
	beq _020E3D3C
	ldr r7, [r4, #-4]
	ldr r6, [r4, #-8]!
	sub r2, r7, r6
	cmp r2, r8
	bne _020E3B50
	mov r0, r6
	mov r1, r7
	blx fp
	cmp r0, #0
	ble _020E3ADC
	mov r0, r8
	tst r0, #3
	beq _020E3B34
_020E3B18:
	ldrb r1, [r6]
	subs r0, r0, #1
	swpb r1, r1, [r7]
	add r7, r7, #1
	strb r1, [r6], #1
	bne _020E3B18
	b _020E3ADC
_020E3B34:
	ldr r1, [r6]
	subs r0, r0, #4
	swp r1, r1, [r7]
	add r7, r7, #4
	str r1, [r6], #4
	bne _020E3B34
	b _020E3ADC
_020E3B50:
	ldr r3, [sp]
	sub r2, r7, r6
	mov r2, r2, lsr r3
	mla r2, r2, r8, r6
	mov r3, r6
	mov r0, r8
	mov r2, r2
	tst r0, #3
	beq _020E3B90
_020E3B74:
	ldrb r1, [r2]
	subs r0, r0, #1
	swpb r1, r1, [r3]
	add r3, r3, #1
	strb r1, [r2], #1
	bne _020E3B74
	b _020E3BA8
_020E3B90:
	ldr r1, [r2]
	subs r0, r0, #4
	swp r1, r1, [r3]
	add r3, r3, #4
	str r1, [r2], #4
	bne _020E3B90
_020E3BA8:
	mov sb, r6
	mov sl, r7
	add sb, sb, r8
_020E3BB4:
	cmp sb, r7
	bge _020E3BDC
	mov r1, r6
	mov r0, sb
	blx fp
	cmp r0, #0
	blt _020E3BD4
	b _020E3BD8
_020E3BD4:
	add sb, sb, r8
_020E3BD8:
	blt _020E3BB4
_020E3BDC:
	mov r1, r6
	mov r0, sl
	blx fp
	cmp r0, #0
	bgt _020E3BF4
	b _020E3BF8
_020E3BF4:
	sub sl, sl, r8
_020E3BF8:
	bgt _020E3BDC
	cmp sb, sl
	bge _020E3C5C
	mov r2, sb
	mov r3, sl
	mov r0, r8
	tst r0, #3
	beq _020E3C34
_020E3C18:
	ldrb r1, [r2]
	subs r0, r0, #1
	swpb r1, r1, [r3]
	add r3, r3, #1
	strb r1, [r2], #1
	bne _020E3C18
	b _020E3C4C
_020E3C34:
	ldr r1, [r2]
	subs r0, r0, #4
	swp r1, r1, [r3]
	add r3, r3, #4
	str r1, [r2], #4
	bne _020E3C34
_020E3C4C:
	add sb, sb, r8
	sub sl, sl, r8
	cmp sb, sl
	ble _020E3BB4
_020E3C5C:
	mov r2, r6
	mov r3, sl
	mov r0, r8
	tst r0, #3
	beq _020E3C8C
_020E3C70:
	ldrb r1, [r2]
	subs r0, r0, #1
	swpb r1, r1, [r3]
	add r3, r3, #1
	strb r1, [r2], #1
	bne _020E3C70
	b _020E3CA4
_020E3C8C:
	ldr r1, [r2]
	subs r0, r0, #4
	swp r1, r1, [r3]
	add r3, r3, #4
	str r1, [r2], #4
	bne _020E3C8C
_020E3CA4:
	sub r2, sl, r6
	sub r3, r7, sl
	cmp r2, r3
	ble _020E3CF8
	sub r2, sl, r8
	cmp r6, r2
	blt _020E3CC4
	b _020E3CC8
_020E3CC4:
	str r6, [r4], #4
_020E3CC8:
	blt _020E3CD0
	b _020E3CD4
_020E3CD0:
	str r2, [r4], #4
_020E3CD4:
	add r2, sl, r8
	cmp r2, r7
	blt _020E3CE4
	b _020E3CE8
_020E3CE4:
	str r2, [r4], #4
_020E3CE8:
	blt _020E3CF0
	b _020E3CF4
_020E3CF0:
	str r7, [r4], #4
_020E3CF4:
	b _020E3ADC
_020E3CF8:
	add r2, sl, r8
	cmp r2, r7
	blt _020E3D08
	b _020E3D0C
_020E3D08:
	str r2, [r4], #4
_020E3D0C:
	blt _020E3D14
	b _020E3D18
_020E3D14:
	str r7, [r4], #4
_020E3D18:
	sub r2, sl, r8
	cmp r6, r2
	blt _020E3D28
	b _020E3D2C
_020E3D28:
	str r6, [r4], #4
_020E3D2C:
	blt _020E3D34
	b _020E3D38
_020E3D34:
	str r2, [r4], #4
_020E3D38:
	b _020E3ADC
_020E3D3C:
	add sp, sp, #4
	sub r4, r4, #4
	cmp r4, sp
	beq _020E3D50
	b _020E3D54
_020E3D50:
	ldr r0, [sp]
_020E3D54:
	beq _020E3D5C
	b _020E3D60
_020E3D5C:
	add r0, r0, #4
_020E3D60:
	beq _020E3D68
	b _020E3D6C
_020E3D68:
	add sp, sp, r0
_020E3D6C:
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	bx lr
	arm_func_end MATH_QSort

	arm_func_start STD_CopyLString
STD_CopyLString: ; 0x020E3D74
	stmdb sp!, {r4, lr}
	sub ip, r2, #1
	mov r4, r1
	cmp ip, #0
	mov lr, #0
	ble _020E3DB0
_020E3D8C:
	ldrsb r3, [r4]
	strb r3, [r0, lr]
	ldrsb r3, [r4]
	cmp r3, #0
	beq _020E3DB0
	add lr, lr, #1
	cmp lr, ip
	add r4, r4, #1
	blt _020E3D8C
_020E3DB0:
	sub r3, r2, #1
	cmp lr, r3
	blt _020E3DC8
	cmp r2, #0
	movne r2, #0
	strneb r2, [r0, lr]
_020E3DC8:
	mov r0, r1
	bl STD_GetStringLength
	ldmia sp!, {r4, pc}
	arm_func_end STD_CopyLString

	arm_func_start STD_GetStringLength
STD_GetStringLength: ; 0x020E3DD4
	ldrsb r1, [r0]
	mov r2, #0
	cmp r1, #0
	beq _020E3DF4
_020E3DE4:
	add r2, r2, #1
	ldrsb r1, [r0, r2]
	cmp r1, #0
	bne _020E3DE4
_020E3DF4:
	mov r0, r2
	bx lr
	arm_func_end STD_GetStringLength

	arm_func_start STD_CompareString
STD_CompareString: ; 0x020E3DFC
	b _020E3E08
_020E3E00:
	add r0, r0, #1
	add r1, r1, #1
_020E3E08:
	ldrsb r3, [r1]
	ldrsb r2, [r0]
	cmp r2, r3
	bne _020E3E20
	cmp r2, #0
	bne _020E3E00
_020E3E20:
	sub r0, r2, r3
	bx lr
	arm_func_end STD_CompareString

	arm_func_start STD_CompareNString
STD_CompareNString: ; 0x020E3E28
	stmdb sp!, {r3, lr}
	cmp r2, #0
	beq _020E3E60
	cmp r2, #0
	mov lr, #0
	ble _020E3E60
_020E3E40:
	ldrb ip, [r0, lr]
	ldrb r3, [r1, lr]
	cmp ip, r3
	subne r0, ip, r3
	ldmneia sp!, {r3, pc}
	add lr, lr, #1
	cmp lr, r2
	blt _020E3E40
_020E3E60:
	mov r0, #0
	ldmia sp!, {r3, pc}
	arm_func_end STD_CompareNString

	arm_func_start string_put_char
string_put_char: ; 0x020E3E68
	ldr r2, [r0]
	cmp r2, #0
	beq _020E3E88
	ldr r2, [r0, #4]
	strb r1, [r2]
	ldr r1, [r0]
	sub r1, r1, #1
	str r1, [r0]
_020E3E88:
	ldr r1, [r0, #4]
	add r1, r1, #1
	str r1, [r0, #4]
	bx lr
	arm_func_end string_put_char

	arm_func_start string_fill_char
string_fill_char: ; 0x020E3E98
	stmdb sp!, {r3, lr}
	cmp r2, #0
	ldmleia sp!, {r3, pc}
	ldr lr, [r0]
	mov ip, #0
	cmp lr, r2
	movhi lr, r2
	cmp lr, #0
	bls _020E3ED0
_020E3EBC:
	ldr r3, [r0, #4]
	strb r1, [r3, ip]
	add ip, ip, #1
	cmp ip, lr
	blo _020E3EBC
_020E3ED0:
	ldr r1, [r0]
	sub r1, r1, lr
	str r1, [r0]
	ldr r1, [r0, #4]
	add r1, r1, r2
	str r1, [r0, #4]
	ldmia sp!, {r3, pc}
	arm_func_end string_fill_char

	arm_func_start string_put_string
string_put_string: ; 0x020E3EEC
	stmdb sp!, {r4, lr}
	cmp r2, #0
	ldmleia sp!, {r4, pc}
	ldr r4, [r0]
	mov lr, #0
	cmp r4, r2
	movhi r4, r2
	cmp r4, #0
	bls _020E3F28
_020E3F10:
	ldrsb ip, [r1, lr]
	ldr r3, [r0, #4]
	strb ip, [r3, lr]
	add lr, lr, #1
	cmp lr, r4
	blo _020E3F10
_020E3F28:
	ldr r1, [r0]
	sub r1, r1, r4
	str r1, [r0]
	ldr r1, [r0, #4]
	add r1, r1, r2
	str r1, [r0, #4]
	ldmia sp!, {r4, pc}
	arm_func_end string_put_string

	arm_func_start STD_TVSNPrintf
STD_TVSNPrintf: ; 0x020E3F44
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x30
	mov sb, r2
	str r1, [sp, #0xc]
	str r0, [sp, #0x14]
	str r0, [sp, #0x10]
	ldrsb r0, [sb]
	str r1, [sp]
	mov fp, r3
	cmp r0, #0
	beq _020E4770
_020E3F70:
	ldrsb r1, [sb]
	and r0, r1, #0xff
	eor r0, r0, #0x20
	sub r0, r0, #0xa1
	cmp r0, #0x3c
	bhs _020E3FAC
	add r0, sp, #0xc
	bl string_put_char
	ldrsb r1, [sb, #1]!
	cmp r1, #0
	beq _020E4764
	add r0, sp, #0xc
	add sb, sb, #1
	bl string_put_char
	b _020E4764
_020E3FAC:
	cmp r1, #0x25
	beq _020E3FC4
	add r0, sp, #0xc
	add sb, sb, #1
	bl string_put_char
	b _020E4764
_020E3FC4:
	mov r5, #0
	mov sl, r5
	mov r2, sb
	sub r6, r5, #1
	mov r0, #0xa
	mov r3, #0x57
_020E3FDC:
	ldrsb r4, [sb, #1]!
	cmp r4, #0x20
	bgt _020E3FF0
	beq _020E402C
	b _020E4044
_020E3FF0:
	cmp r4, #0x30
	bgt _020E4044
	cmp r4, #0x2b
	blt _020E4044
	beq _020E4018
	cmp r4, #0x2d
	beq _020E4034
	cmp r4, #0x30
	beq _020E403C
	b _020E4044
_020E4018:
	ldrsb r1, [sb, #-1]
	cmp r1, #0x20
	bne _020E4044
	orr r5, r5, #2
	b _020E3FDC
_020E402C:
	orr r5, r5, #1
	b _020E3FDC
_020E4034:
	orr r5, r5, #8
	b _020E3FDC
_020E403C:
	orr r5, r5, #0x10
	b _020E3FDC
_020E4044:
	cmp r4, #0x2a
	bne _020E4068
	add fp, fp, #4
	ldr sl, [fp, #-4]
	add sb, sb, #1
	cmp sl, #0
	rsblt sl, sl, #0
	orrlt r5, r5, #8
	b _020E4090
_020E4068:
	mov r1, #0xa
	b _020E407C
_020E4070:
	ldrsb r4, [sb], #1
	mla r4, sl, r1, r4
	sub sl, r4, #0x30
_020E407C:
	ldrsb r4, [sb]
	cmp r4, #0x30
	blt _020E4090
	cmp r4, #0x39
	ble _020E4070
_020E4090:
	ldrsb r1, [sb]
	cmp r1, #0x2e
	bne _020E40EC
	ldrsb r1, [sb, #1]!
	mov r6, #0
	cmp r1, #0x2a
	bne _020E40C4
	add fp, fp, #4
	ldr r6, [fp, #-4]
	add sb, sb, #1
	cmp r6, #0
	mvnlt r6, #0
	b _020E40EC
_020E40C4:
	mov r1, #0xa
	b _020E40D8
_020E40CC:
	ldrsb r4, [sb], #1
	mla r4, r6, r1, r4
	sub r6, r4, #0x30
_020E40D8:
	ldrsb r4, [sb]
	cmp r4, #0x30
	blt _020E40EC
	cmp r4, #0x39
	ble _020E40CC
_020E40EC:
	ldrsb r1, [sb]
	cmp r1, #0x68
	beq _020E4104
	cmp r1, #0x6c
	beq _020E411C
	b _020E4130
_020E4104:
	ldrsb r1, [sb, #1]!
	cmp r1, #0x68
	orrne r5, r5, #0x40
	addeq sb, sb, #1
	orreq r5, r5, #0x100
	b _020E4130
_020E411C:
	ldrsb r1, [sb, #1]!
	cmp r1, #0x6c
	orrne r5, r5, #0x20
	addeq sb, sb, #1
	orreq r5, r5, #0x80
_020E4130:
	ldrsb r1, [sb]
	cmp r1, #0x69
	bgt _020E4174
	cmp r1, #0x63
	blt _020E4158
	beq _020E41E4
	cmp r1, #0x64
	cmpne r1, #0x69
	beq _020E439C
	b _020E4380
_020E4158:
	cmp r1, #0x25
	bgt _020E4168
	beq _020E4364
	b _020E4380
_020E4168:
	cmp r1, #0x58
	beq _020E41D0
	b _020E4380
_020E4174:
	cmp r1, #0x6e
	bgt _020E4184
	beq _020E430C
	b _020E4380
_020E4184:
	sub r1, r1, #0x6f
	cmp r1, #9
	addls pc, pc, r1, lsl #2
	b _020E4380
_020E4194: ; jump table
	b _020E41BC ; case 0
	b _020E41D8 ; case 1
	b _020E4380 ; case 2
	b _020E4380 ; case 3
	b _020E4258 ; case 4
	b _020E4380 ; case 5
	b _020E41C8 ; case 6
	b _020E4380 ; case 7
	b _020E4380 ; case 8
	b _020E4394 ; case 9
_020E41BC:
	orr r5, r5, #0x1000
	mov r0, #8
	b _020E439C
_020E41C8:
	orr r5, r5, #0x1000
	b _020E439C
_020E41D0:
	mov r3, #0x37
	b _020E4394
_020E41D8:
	orr r5, r5, #4
	mov r6, #8
	b _020E4394
_020E41E4:
	cmp r6, #0
	bge _020E4380
	add fp, fp, #4
	tst r5, #8
	ldr r4, [fp, #-4]
	beq _020E4220
	mov r1, r4, lsl #0x18
	add r0, sp, #0xc
	mov r1, r1, asr #0x18
	bl string_put_char
	add r0, sp, #0xc
	sub r2, sl, #1
	mov r1, #0x20
	bl string_fill_char
	b _020E4250
_020E4220:
	tst r5, #0x10
	movne r0, #0x30
	moveq r0, #0x20
	mov r1, r0, lsl #0x18
	add r0, sp, #0xc
	mov r1, r1, asr #0x18
	sub r2, sl, #1
	bl string_fill_char
	mov r1, r4, lsl #0x18
	add r0, sp, #0xc
	mov r1, r1, asr #0x18
	bl string_put_char
_020E4250:
	add sb, sb, #1
	b _020E4764
_020E4258:
	add fp, fp, #4
	cmp r6, #0
	ldr r7, [fp, #-4]
	mov r4, #0
	bge _020E4290
	ldrsb r0, [r7]
	cmp r0, #0
	beq _020E42A4
_020E4278:
	add r4, r4, #1
	ldrsb r0, [r7, r4]
	cmp r0, #0
	bne _020E4278
	b _020E42A4
_020E428C:
	add r4, r4, #1
_020E4290:
	cmp r4, r6
	bge _020E42A4
	ldrsb r0, [r7, r4]
	cmp r0, #0
	bne _020E428C
_020E42A4:
	tst r5, #8
	sub sl, sl, r4
	beq _020E42D4
	add r0, sp, #0xc
	mov r1, r7
	mov r2, r4
	bl string_put_string
	add r0, sp, #0xc
	mov r2, sl
	mov r1, #0x20
	bl string_fill_char
	b _020E4304
_020E42D4:
	tst r5, #0x10
	movne r0, #0x30
	moveq r0, #0x20
	mov r1, r0, lsl #0x18
	add r0, sp, #0xc
	mov r2, sl
	mov r1, r1, asr #0x18
	bl string_fill_char
	add r0, sp, #0xc
	mov r1, r7
	mov r2, r4
	bl string_put_string
_020E4304:
	add sb, sb, #1
	b _020E4764
_020E430C:
	ldr r1, [sp, #0x10]
	ldr r0, [sp, #0x14]
	tst r5, #0x100
	sub r2, r1, r0
	bne _020E435C
	tst r5, #0x40
	beq _020E4338
	add fp, fp, #4
	ldr r0, [fp, #-4]
	strh r2, [r0]
	b _020E435C
_020E4338:
	add fp, fp, #4
	tst r5, #0x80
	ldreq r0, [fp, #-4]
	streq r2, [r0]
	beq _020E435C
	ldr r0, [fp, #-4]
	mov r1, r2, asr #0x1f
	str r2, [r0]
	str r1, [r0, #4]
_020E435C:
	add sb, sb, #1
	b _020E4764
_020E4364:
	add r0, r2, #1
	cmp r0, sb
	bne _020E4380
	add r0, sp, #0xc
	add sb, sb, #1
	bl string_put_char
	b _020E4764
_020E4380:
	mov r1, r2
	add r0, sp, #0xc
	sub r2, sb, r2
	bl string_put_string
	b _020E4764
_020E4394:
	orr r5, r5, #0x1000
	mov r0, #0x10
_020E439C:
	tst r5, #8
	bicne r5, r5, #0x10
	cmp r6, #0
	bicge r5, r5, #0x10
	movlt r6, #1
	mov r4, #0
	tst r5, #0x1000
	beq _020E445C
	tst r5, #0x100
	beq _020E43D4
	add fp, fp, #4
	ldrb r7, [fp, #-4]
	mov r1, #0
	b _020E440C
_020E43D4:
	tst r5, #0x40
	beq _020E43EC
	add fp, fp, #4
	ldrh r7, [fp, #-4]
	mov r1, #0
	b _020E440C
_020E43EC:
	tst r5, #0x80
	addeq fp, fp, #4
	ldreq r7, [fp, #-4]
	moveq r1, #0
	beq _020E440C
	add fp, fp, #8
	ldr r7, [fp, #-8]
	ldr r1, [fp, #-4]
_020E440C:
	bic r5, r5, #3
	tst r5, #4
	beq _020E451C
	cmp r0, #0x10
	bne _020E4444
	cmp r1, #0
	cmpeq r7, #0
	beq _020E451C
	add r4, r3, #0x21
	mov r2, #0x30
	strb r4, [sp, #8]
	strb r2, [sp, #9]
	mov r4, #2
	b _020E451C
_020E4444:
	cmp r0, #8
	bne _020E451C
	mov r2, #0x30
	strb r2, [sp, #8]
	mov r4, #1
	b _020E451C
_020E445C:
	tst r5, #0x100
	beq _020E4474
	add fp, fp, #4
	ldrsb r7, [fp, #-4]
	mov r1, r7, asr #0x1f
	b _020E44AC
_020E4474:
	tst r5, #0x40
	beq _020E448C
	add fp, fp, #4
	ldrsh r7, [fp, #-4]
	mov r1, r7, asr #0x1f
	b _020E44AC
_020E448C:
	tst r5, #0x80
	addeq fp, fp, #4
	ldreq r7, [fp, #-4]
	moveq r1, r7, asr #0x1f
	beq _020E44AC
	add fp, fp, #8
	ldr r7, [fp, #-8]
	ldr r1, [fp, #-4]
_020E44AC:
	mov ip, #0
	and r2, ip, #0
	and r8, r1, #0x80000000
	cmp r2, #0
	cmpeq r8, #0
	beq _020E44E4
	mvn r4, r7
	mov r2, #0x2d
	mvn r1, r1
	strb r2, [sp, #8]
	adds r7, r4, #1
	adc r1, r1, ip
	mov r4, #1
	b _020E451C
_020E44E4:
	cmp r1, ip
	cmpeq r7, ip
	cmpeq r6, #0
	beq _020E451C
	tst r5, #2
	beq _020E450C
	mov r2, #0x2b
	strb r2, [sp, #8]
	mov r4, #1
	b _020E451C
_020E450C:
	tst r5, #1
	movne r2, #0x20
	strneb r2, [sp, #8]
	movne r4, #1
_020E451C:
	cmp r0, #8
	mov r8, #0
	beq _020E453C
	cmp r0, #0xa
	beq _020E4588
	cmp r0, #0x10
	beq _020E4620
	b _020E466C
_020E453C:
	cmp r1, r8
	cmpeq r7, r8
	beq _020E466C
	add r2, sp, #0x18
	mov r0, r8
	mov lr, #7
	mov ip, r8
_020E4558:
	and r3, r7, lr
	add r3, r3, #0x30
	strb r3, [r2, r8]
	mov r3, r1, lsr #3
	cmp r3, r0
	mov r7, r7, lsr #3
	orr r7, r7, r1, lsl #29
	mov r1, r3
	cmpeq r7, ip
	add r8, r8, #1
	bne _020E4558
	b _020E466C
_020E4588:
	mov r0, r8
	cmp r0, r8
	cmpeq r1, r8
	bne _020E45D4
	cmp r7, #0
	beq _020E466C
	ldr ip, _020E47BC ; =0xCCCCCCCD
	add r3, sp, #0x18
	mov r2, #0xa
_020E45AC:
	umull r1, r0, r7, ip
	movs r0, r0, lsr #3
	mul r1, r0, r2
	sub r1, r7, r1
	mov r7, r0
	add r0, r1, #0x30
	strb r0, [r3, r8]
	add r8, r8, #1
	bne _020E45AC
	b _020E466C
_020E45D4:
	cmp r1, r8
	cmpeq r7, r8
	beq _020E466C
_020E45E0:
	mov r0, r7
	mov r2, #0xa
	mov r3, #0
	bl _ll_udiv
	mov r2, #0xa
	umull r3, r2, r0, r2
	subs r2, r7, r3
	add r3, r2, #0x30
	add r2, sp, #0x18
	strb r3, [r2, r8]
	cmp r1, #0
	cmpeq r0, #0
	mov r7, r0
	add r8, r8, #1
	bne _020E45E0
	b _020E466C
_020E4620:
	cmp r1, r8
	cmpeq r7, r8
	beq _020E466C
	add ip, sp, #0x18
	mov lr, #0xf
_020E4634:
	and r2, r7, lr
	mov r7, r7, lsr #4
	mov r0, r1, lsr #4
	orr r7, r7, r1, lsl #28
	cmp r2, #0xa
	mov r1, r0
	addlt r0, r2, #0x30
	addge r0, r2, r3
	strb r0, [ip, r8]
	mov r0, #0
	cmp r1, r0
	cmpeq r7, r0
	add r8, r8, #1
	bne _020E4634
_020E466C:
	cmp r4, #0
	ble _020E4694
	ldrsb r0, [sp, #8]
	cmp r0, #0x30
	bne _020E4694
	add r0, sp, #0x18
	mov r1, #0x30
	strb r1, [r0, r8]
	add r8, r8, #1
	mov r4, #0
_020E4694:
	tst r5, #0x10
	sub r6, r6, r8
	beq _020E46B0
	sub r0, sl, r8
	sub r0, r0, r4
	cmp r6, r0
	movlt r6, r0
_020E46B0:
	cmp r6, #0
	subgt sl, sl, r6
	add r0, r4, r8
	sub sl, sl, r0
	ands r0, r5, #8
	str r0, [sp, #4]
	bne _020E46DC
	add r0, sp, #0xc
	mov r2, sl
	mov r1, #0x20
	bl string_fill_char
_020E46DC:
	cmp r4, #0
	ble _020E4708
	add r0, sp, #8
	add r5, r0, r4
	add r7, sp, #0xc
_020E46F0:
	ldrsb r1, [r5, #-1]!
	mov r0, r7
	sub r4, r4, #1
	bl string_put_char
	cmp r4, #0
	bgt _020E46F0
_020E4708:
	add r0, sp, #0xc
	mov r2, r6
	mov r1, #0x30
	bl string_fill_char
	cmp r8, #0
	ble _020E4744
	add r0, sp, #0x18
	add r5, r0, r8
	add r4, sp, #0xc
_020E472C:
	ldrsb r1, [r5, #-1]!
	mov r0, r4
	sub r8, r8, #1
	bl string_put_char
	cmp r8, #0
	bgt _020E472C
_020E4744:
	ldr r0, [sp, #4]
	cmp r0, #0
	beq _020E4760
	add r0, sp, #0xc
	mov r2, sl
	mov r1, #0x20
	bl string_fill_char
_020E4760:
	add sb, sb, #1
_020E4764:
	ldrsb r0, [sb]
	cmp r0, #0
	bne _020E3F70
_020E4770:
	ldr r0, [sp, #0xc]
	cmp r0, #0
	beq _020E478C
	ldr r0, [sp, #0x10]
	mov r1, #0
	strb r1, [r0]
	b _020E47A8
_020E478C:
	ldr r0, [sp]
	cmp r0, #0
	beq _020E47A8
	ldr r1, [sp, #0x14]
	mov r2, #0
	add r0, r1, r0
	strb r2, [r0, #-1]
_020E47A8:
	ldr r1, [sp, #0x10]
	ldr r0, [sp, #0x14]
	sub r0, r1, r0
	add sp, sp, #0x30
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_020E47BC: .word 0xCCCCCCCD
	arm_func_end STD_TVSNPrintf

	.section .rodata.unified_00,4,1,2

_0210D650:
	.byte 0x00, 0x80, 0x2E, 0xFD, 0x2F, 0xFD, 0x75, 0xFD, 0xA7, 0xFD, 0xCE, 0xFD, 0xEE, 0xFD, 0x09, 0xFE
	.byte 0x20, 0xFE, 0x34, 0xFE, 0x46, 0xFE, 0x57, 0xFE, 0x66, 0xFE, 0x74, 0xFE, 0x81, 0xFE, 0x8D, 0xFE
	.byte 0x98, 0xFE, 0xA3, 0xFE, 0xAD, 0xFE, 0xB6, 0xFE, 0xBF, 0xFE, 0xC7, 0xFE, 0xCF, 0xFE, 0xD7, 0xFE
	.byte 0xDF, 0xFE, 0xE6, 0xFE, 0xEC, 0xFE, 0xF3, 0xFE, 0xF9, 0xFE, 0xFF, 0xFE, 0x05, 0xFF, 0x0B, 0xFF
	.byte 0x11, 0xFF, 0x16, 0xFF, 0x1B, 0xFF, 0x20, 0xFF, 0x25, 0xFF, 0x2A, 0xFF, 0x2E, 0xFF, 0x33, 0xFF
	.byte 0x37, 0xFF, 0x3C, 0xFF, 0x40, 0xFF, 0x44, 0xFF, 0x48, 0xFF, 0x4C, 0xFF, 0x50, 0xFF, 0x53, 0xFF
	.byte 0x57, 0xFF, 0x5B, 0xFF, 0x5E, 0xFF, 0x62, 0xFF, 0x65, 0xFF, 0x68, 0xFF, 0x6B, 0xFF, 0x6F, 0xFF
	.byte 0x72, 0xFF, 0x75, 0xFF, 0x78, 0xFF, 0x7B, 0xFF, 0x7E, 0xFF, 0x81, 0xFF, 0x83, 0xFF, 0x86, 0xFF
	.byte 0x89, 0xFF, 0x8C, 0xFF, 0x8E, 0xFF, 0x91, 0xFF, 0x93, 0xFF, 0x96, 0xFF, 0x99, 0xFF, 0x9B, 0xFF
	.byte 0x9D, 0xFF, 0xA0, 0xFF, 0xA2, 0xFF, 0xA5, 0xFF, 0xA7, 0xFF, 0xA9, 0xFF, 0xAB, 0xFF, 0xAE, 0xFF
	.byte 0xB0, 0xFF, 0xB2, 0xFF, 0xB4, 0xFF, 0xB6, 0xFF, 0xB8, 0xFF, 0xBA, 0xFF, 0xBC, 0xFF, 0xBE, 0xFF
	.byte 0xC0, 0xFF, 0xC2, 0xFF, 0xC4, 0xFF, 0xC6, 0xFF, 0xC8, 0xFF, 0xCA, 0xFF, 0xCC, 0xFF, 0xCE, 0xFF
	.byte 0xCF, 0xFF, 0xD1, 0xFF, 0xD3, 0xFF, 0xD5, 0xFF, 0xD6, 0xFF, 0xD8, 0xFF, 0xDA, 0xFF, 0xDC, 0xFF
	.byte 0xDD, 0xFF, 0xDF, 0xFF, 0xE1, 0xFF, 0xE2, 0xFF, 0xE4, 0xFF, 0xE5, 0xFF, 0xE7, 0xFF, 0xE9, 0xFF
	.byte 0xEA, 0xFF, 0xEC, 0xFF, 0xED, 0xFF, 0xEF, 0xFF, 0xF0, 0xFF, 0xF2, 0xFF, 0xF3, 0xFF, 0xF5, 0xFF
	.byte 0xF6, 0xFF, 0xF8, 0xFF, 0xF9, 0xFF, 0xFA, 0xFF, 0xFC, 0xFF, 0xFD, 0xFF, 0xFF, 0xFF, 0x00, 0x00
	.public SNDi_DecibelTable

	.section .rodata.alias_d750,4,1,2
	.public _0210D750
_0210D750:

	.section .rodata.unified_01,4,1,2
	.byte 0x00, 0x00, 0x3B, 0x00, 0x76, 0x00, 0xB2, 0x00, 0xED, 0x00, 0x28, 0x01, 0x64, 0x01, 0x9F, 0x01
	.byte 0xDB, 0x01, 0x17, 0x02, 0x52, 0x02, 0x8E, 0x02, 0xCA, 0x02, 0x05, 0x03, 0x41, 0x03, 0x7D, 0x03
	.byte 0xB9, 0x03, 0xF5, 0x03, 0x31, 0x04, 0x6E, 0x04, 0xAA, 0x04, 0xE6, 0x04, 0x22, 0x05, 0x5F, 0x05
	.byte 0x9B, 0x05, 0xD8, 0x05, 0x14, 0x06, 0x51, 0x06, 0x8D, 0x06, 0xCA, 0x06, 0x07, 0x07, 0x43, 0x07
	.byte 0x80, 0x07, 0xBD, 0x07, 0xFA, 0x07, 0x37, 0x08, 0x74, 0x08, 0xB1, 0x08, 0xEF, 0x08, 0x2C, 0x09
	.byte 0x69, 0x09, 0xA7, 0x09, 0xE4, 0x09, 0x21, 0x0A, 0x5F, 0x0A, 0x9C, 0x0A, 0xDA, 0x0A, 0x18, 0x0B
	.byte 0x56, 0x0B, 0x93, 0x0B, 0xD1, 0x0B, 0x0F, 0x0C, 0x4D, 0x0C, 0x8B, 0x0C, 0xC9, 0x0C, 0x07, 0x0D
	.byte 0x45, 0x0D, 0x84, 0x0D, 0xC2, 0x0D, 0x00, 0x0E, 0x3F, 0x0E, 0x7D, 0x0E, 0xBC, 0x0E, 0xFA, 0x0E
	.byte 0x39, 0x0F, 0x78, 0x0F, 0xB6, 0x0F, 0xF5, 0x0F, 0x34, 0x10, 0x73, 0x10, 0xB2, 0x10, 0xF1, 0x10
	.byte 0x30, 0x11, 0x6F, 0x11, 0xAE, 0x11, 0xEE, 0x11, 0x2D, 0x12, 0x6C, 0x12, 0xAC, 0x12, 0xEB, 0x12
	.byte 0x2B, 0x13, 0x6B, 0x13, 0xAA, 0x13, 0xEA, 0x13, 0x2A, 0x14, 0x6A, 0x14, 0xA9, 0x14, 0xE9, 0x14
	.byte 0x29, 0x15, 0x69, 0x15, 0xAA, 0x15, 0xEA, 0x15, 0x2A, 0x16, 0x6A, 0x16, 0xAB, 0x16, 0xEB, 0x16
	.byte 0x2C, 0x17, 0x6C, 0x17, 0xAD, 0x17, 0xED, 0x17, 0x2E, 0x18, 0x6F, 0x18, 0xB0, 0x18, 0xF0, 0x18
	.byte 0x31, 0x19, 0x72, 0x19, 0xB3, 0x19, 0xF5, 0x19, 0x36, 0x1A, 0x77, 0x1A, 0xB8, 0x1A, 0xFA, 0x1A
	.byte 0x3B, 0x1B, 0x7D, 0x1B, 0xBE, 0x1B, 0x00, 0x1C, 0x41, 0x1C, 0x83, 0x1C, 0xC5, 0x1C, 0x07, 0x1D
	.byte 0x48, 0x1D, 0x8A, 0x1D, 0xCC, 0x1D, 0x0E, 0x1E, 0x51, 0x1E, 0x93, 0x1E, 0xD5, 0x1E, 0x17, 0x1F
	.byte 0x5A, 0x1F, 0x9C, 0x1F, 0xDF, 0x1F, 0x21, 0x20, 0x64, 0x20, 0xA6, 0x20, 0xE9, 0x20, 0x2C, 0x21
	.byte 0x6F, 0x21, 0xB2, 0x21, 0xF5, 0x21, 0x38, 0x22, 0x7B, 0x22, 0xBE, 0x22, 0x01, 0x23, 0x44, 0x23
	.byte 0x88, 0x23, 0xCB, 0x23, 0x0E, 0x24, 0x52, 0x24, 0x96, 0x24, 0xD9, 0x24, 0x1D, 0x25, 0x61, 0x25
	.byte 0xA4, 0x25, 0xE8, 0x25, 0x2C, 0x26, 0x70, 0x26, 0xB4, 0x26, 0xF8, 0x26, 0x3D, 0x27, 0x81, 0x27
	.byte 0xC5, 0x27, 0x0A, 0x28, 0x4E, 0x28, 0x92, 0x28, 0xD7, 0x28, 0x1C, 0x29, 0x60, 0x29, 0xA5, 0x29
	.byte 0xEA, 0x29, 0x2F, 0x2A, 0x74, 0x2A, 0xB9, 0x2A, 0xFE, 0x2A, 0x43, 0x2B, 0x88, 0x2B, 0xCD, 0x2B
	.byte 0x13, 0x2C, 0x58, 0x2C, 0x9D, 0x2C, 0xE3, 0x2C, 0x28, 0x2D, 0x6E, 0x2D, 0xB4, 0x2D, 0xF9, 0x2D
	.byte 0x3F, 0x2E, 0x85, 0x2E, 0xCB, 0x2E, 0x11, 0x2F, 0x57, 0x2F, 0x9D, 0x2F, 0xE3, 0x2F, 0x2A, 0x30
	.byte 0x70, 0x30, 0xB6, 0x30, 0xFD, 0x30, 0x43, 0x31, 0x8A, 0x31, 0xD0, 0x31, 0x17, 0x32, 0x5E, 0x32
	.byte 0xA5, 0x32, 0xEC, 0x32, 0x32, 0x33, 0x79, 0x33, 0xC1, 0x33, 0x08, 0x34, 0x4F, 0x34, 0x96, 0x34
	.byte 0xDD, 0x34, 0x25, 0x35, 0x6C, 0x35, 0xB4, 0x35, 0xFB, 0x35, 0x43, 0x36, 0x8B, 0x36, 0xD3, 0x36
	.byte 0x1A, 0x37, 0x62, 0x37, 0xAA, 0x37, 0xF2, 0x37, 0x3A, 0x38, 0x83, 0x38, 0xCB, 0x38, 0x13, 0x39
	.byte 0x5C, 0x39, 0xA4, 0x39, 0xED, 0x39, 0x35, 0x3A, 0x7E, 0x3A, 0xC6, 0x3A, 0x0F, 0x3B, 0x58, 0x3B
	.byte 0xA1, 0x3B, 0xEA, 0x3B, 0x33, 0x3C, 0x7C, 0x3C, 0xC5, 0x3C, 0x0E, 0x3D, 0x58, 0x3D, 0xA1, 0x3D
	.byte 0xEA, 0x3D, 0x34, 0x3E, 0x7D, 0x3E, 0xC7, 0x3E, 0x11, 0x3F, 0x5A, 0x3F, 0xA4, 0x3F, 0xEE, 0x3F
	.byte 0x38, 0x40, 0x82, 0x40, 0xCC, 0x40, 0x16, 0x41, 0x61, 0x41, 0xAB, 0x41, 0xF5, 0x41, 0x40, 0x42
	.byte 0x8A, 0x42, 0xD5, 0x42, 0x1F, 0x43, 0x6A, 0x43, 0xB5, 0x43, 0x00, 0x44, 0x4B, 0x44, 0x95, 0x44
	.byte 0xE1, 0x44, 0x2C, 0x45, 0x77, 0x45, 0xC2, 0x45, 0x0D, 0x46, 0x59, 0x46, 0xA4, 0x46, 0xF0, 0x46
	.byte 0x3B, 0x47, 0x87, 0x47, 0xD3, 0x47, 0x1E, 0x48, 0x6A, 0x48, 0xB6, 0x48, 0x02, 0x49, 0x4E, 0x49
	.byte 0x9A, 0x49, 0xE6, 0x49, 0x33, 0x4A, 0x7F, 0x4A, 0xCB, 0x4A, 0x18, 0x4B, 0x64, 0x4B, 0xB1, 0x4B
	.byte 0xFE, 0x4B, 0x4A, 0x4C, 0x97, 0x4C, 0xE4, 0x4C, 0x31, 0x4D, 0x7E, 0x4D, 0xCB, 0x4D, 0x18, 0x4E
	.byte 0x66, 0x4E, 0xB3, 0x4E, 0x00, 0x4F, 0x4E, 0x4F, 0x9B, 0x4F, 0xE9, 0x4F, 0x36, 0x50, 0x84, 0x50
	.byte 0xD2, 0x50, 0x20, 0x51, 0x6E, 0x51, 0xBC, 0x51, 0x0A, 0x52, 0x58, 0x52, 0xA6, 0x52, 0xF4, 0x52
	.byte 0x43, 0x53, 0x91, 0x53, 0xE0, 0x53, 0x2E, 0x54, 0x7D, 0x54, 0xCC, 0x54, 0x1A, 0x55, 0x69, 0x55
	.byte 0xB8, 0x55, 0x07, 0x56, 0x56, 0x56, 0xA5, 0x56, 0xF4, 0x56, 0x44, 0x57, 0x93, 0x57, 0xE2, 0x57
	.byte 0x32, 0x58, 0x82, 0x58, 0xD1, 0x58, 0x21, 0x59, 0x71, 0x59, 0xC1, 0x59, 0x10, 0x5A, 0x60, 0x5A
	.byte 0xB0, 0x5A, 0x01, 0x5B, 0x51, 0x5B, 0xA1, 0x5B, 0xF1, 0x5B, 0x42, 0x5C, 0x92, 0x5C, 0xE3, 0x5C
	.byte 0x34, 0x5D, 0x84, 0x5D, 0xD5, 0x5D, 0x26, 0x5E, 0x77, 0x5E, 0xC8, 0x5E, 0x19, 0x5F, 0x6A, 0x5F
	.byte 0xBB, 0x5F, 0x0D, 0x60, 0x5E, 0x60, 0xB0, 0x60, 0x01, 0x61, 0x53, 0x61, 0xA4, 0x61, 0xF6, 0x61
	.byte 0x48, 0x62, 0x9A, 0x62, 0xEC, 0x62, 0x3E, 0x63, 0x90, 0x63, 0xE2, 0x63, 0x34, 0x64, 0x87, 0x64
	.byte 0xD9, 0x64, 0x2C, 0x65, 0x7E, 0x65, 0xD1, 0x65, 0x24, 0x66, 0x76, 0x66, 0xC9, 0x66, 0x1C, 0x67
	.byte 0x6F, 0x67, 0xC2, 0x67, 0x15, 0x68, 0x69, 0x68, 0xBC, 0x68, 0x0F, 0x69, 0x63, 0x69, 0xB6, 0x69
	.byte 0x0A, 0x6A, 0x5E, 0x6A, 0xB1, 0x6A, 0x05, 0x6B, 0x59, 0x6B, 0xAD, 0x6B, 0x01, 0x6C, 0x55, 0x6C
	.byte 0xAA, 0x6C, 0xFE, 0x6C, 0x52, 0x6D, 0xA7, 0x6D, 0xFB, 0x6D, 0x50, 0x6E, 0xA4, 0x6E, 0xF9, 0x6E
	.byte 0x4E, 0x6F, 0xA3, 0x6F, 0xF8, 0x6F, 0x4D, 0x70, 0xA2, 0x70, 0xF7, 0x70, 0x4D, 0x71, 0xA2, 0x71
	.byte 0xF7, 0x71, 0x4D, 0x72, 0xA2, 0x72, 0xF8, 0x72, 0x4E, 0x73, 0xA4, 0x73, 0xFA, 0x73, 0x50, 0x74
	.byte 0xA6, 0x74, 0xFC, 0x74, 0x52, 0x75, 0xA8, 0x75, 0xFF, 0x75, 0x55, 0x76, 0xAC, 0x76, 0x02, 0x77
	.byte 0x59, 0x77, 0xB0, 0x77, 0x07, 0x78, 0x5E, 0x78, 0xB4, 0x78, 0x0C, 0x79, 0x63, 0x79, 0xBA, 0x79
	.byte 0x11, 0x7A, 0x69, 0x7A, 0xC0, 0x7A, 0x18, 0x7B, 0x6F, 0x7B, 0xC7, 0x7B, 0x1F, 0x7C, 0x77, 0x7C
	.byte 0xCF, 0x7C, 0x27, 0x7D, 0x7F, 0x7D, 0xD7, 0x7D, 0x2F, 0x7E, 0x88, 0x7E, 0xE0, 0x7E, 0x38, 0x7F
	.byte 0x91, 0x7F, 0xEA, 0x7F, 0x42, 0x80, 0x9B, 0x80, 0xF4, 0x80, 0x4D, 0x81, 0xA6, 0x81, 0xFF, 0x81
	.byte 0x59, 0x82, 0xB2, 0x82, 0x0B, 0x83, 0x65, 0x83, 0xBE, 0x83, 0x18, 0x84, 0x72, 0x84, 0xCB, 0x84
	.byte 0x25, 0x85, 0x7F, 0x85, 0xD9, 0x85, 0x33, 0x86, 0x8E, 0x86, 0xE8, 0x86, 0x42, 0x87, 0x9D, 0x87
	.byte 0xF7, 0x87, 0x52, 0x88, 0xAC, 0x88, 0x07, 0x89, 0x62, 0x89, 0xBD, 0x89, 0x18, 0x8A, 0x73, 0x8A
	.byte 0xCE, 0x8A, 0x2A, 0x8B, 0x85, 0x8B, 0xE0, 0x8B, 0x3C, 0x8C, 0x97, 0x8C, 0xF3, 0x8C, 0x4F, 0x8D
	.byte 0xAB, 0x8D, 0x07, 0x8E, 0x63, 0x8E, 0xBF, 0x8E, 0x1B, 0x8F, 0x77, 0x8F, 0xD4, 0x8F, 0x30, 0x90
	.byte 0x8C, 0x90, 0xE9, 0x90, 0x46, 0x91, 0xA2, 0x91, 0xFF, 0x91, 0x5C, 0x92, 0xB9, 0x92, 0x16, 0x93
	.byte 0x73, 0x93, 0xD1, 0x93, 0x2E, 0x94, 0x8C, 0x94, 0xE9, 0x94, 0x47, 0x95, 0xA4, 0x95, 0x02, 0x96
	.byte 0x60, 0x96, 0xBE, 0x96, 0x1C, 0x97, 0x7A, 0x97, 0xD8, 0x97, 0x36, 0x98, 0x95, 0x98, 0xF3, 0x98
	.byte 0x52, 0x99, 0xB0, 0x99, 0x0F, 0x9A, 0x6E, 0x9A, 0xCD, 0x9A, 0x2C, 0x9B, 0x8B, 0x9B, 0xEA, 0x9B
	.byte 0x49, 0x9C, 0xA8, 0x9C, 0x08, 0x9D, 0x67, 0x9D, 0xC7, 0x9D, 0x26, 0x9E, 0x86, 0x9E, 0xE6, 0x9E
	.byte 0x46, 0x9F, 0xA6, 0x9F, 0x06, 0xA0, 0x66, 0xA0, 0xC6, 0xA0, 0x27, 0xA1, 0x87, 0xA1, 0xE8, 0xA1
	.byte 0x48, 0xA2, 0xA9, 0xA2, 0x0A, 0xA3, 0x6B, 0xA3, 0xCC, 0xA3, 0x2D, 0xA4, 0x8E, 0xA4, 0xEF, 0xA4
	.byte 0x50, 0xA5, 0xB2, 0xA5, 0x13, 0xA6, 0x75, 0xA6, 0xD6, 0xA6, 0x38, 0xA7, 0x9A, 0xA7, 0xFC, 0xA7
	.byte 0x5E, 0xA8, 0xC0, 0xA8, 0x22, 0xA9, 0x84, 0xA9, 0xE7, 0xA9, 0x49, 0xAA, 0xAC, 0xAA, 0x0E, 0xAB
	.byte 0x71, 0xAB, 0xD4, 0xAB, 0x37, 0xAC, 0x9A, 0xAC, 0xFD, 0xAC, 0x60, 0xAD, 0xC3, 0xAD, 0x27, 0xAE
	.byte 0x8A, 0xAE, 0xED, 0xAE, 0x51, 0xAF, 0xB5, 0xAF, 0x19, 0xB0, 0x7C, 0xB0, 0xE0, 0xB0, 0x45, 0xB1
	.byte 0xA9, 0xB1, 0x0D, 0xB2, 0x71, 0xB2, 0xD6, 0xB2, 0x3A, 0xB3, 0x9F, 0xB3, 0x03, 0xB4, 0x68, 0xB4
	.byte 0xCD, 0xB4, 0x32, 0xB5, 0x97, 0xB5, 0xFC, 0xB5, 0x62, 0xB6, 0xC7, 0xB6, 0x2C, 0xB7, 0x92, 0xB7
	.byte 0xF7, 0xB7, 0x5D, 0xB8, 0xC3, 0xB8, 0x29, 0xB9, 0x8F, 0xB9, 0xF5, 0xB9, 0x5B, 0xBA, 0xC1, 0xBA
	.byte 0x28, 0xBB, 0x8E, 0xBB, 0xF5, 0xBB, 0x5B, 0xBC, 0xC2, 0xBC, 0x29, 0xBD, 0x90, 0xBD, 0xF7, 0xBD
	.byte 0x5E, 0xBE, 0xC5, 0xBE, 0x2C, 0xBF, 0x94, 0xBF, 0xFB, 0xBF, 0x63, 0xC0, 0xCA, 0xC0, 0x32, 0xC1
	.byte 0x9A, 0xC1, 0x02, 0xC2, 0x6A, 0xC2, 0xD2, 0xC2, 0x3A, 0xC3, 0xA2, 0xC3, 0x0B, 0xC4, 0x73, 0xC4
	.byte 0xDC, 0xC4, 0x44, 0xC5, 0xAD, 0xC5, 0x16, 0xC6, 0x7F, 0xC6, 0xE8, 0xC6, 0x51, 0xC7, 0xBB, 0xC7
	.byte 0x24, 0xC8, 0x8D, 0xC8, 0xF7, 0xC8, 0x60, 0xC9, 0xCA, 0xC9, 0x34, 0xCA, 0x9E, 0xCA, 0x08, 0xCB
	.byte 0x72, 0xCB, 0xDC, 0xCB, 0x47, 0xCC, 0xB1, 0xCC, 0x1B, 0xCD, 0x86, 0xCD, 0xF1, 0xCD, 0x5B, 0xCE
	.byte 0xC6, 0xCE, 0x31, 0xCF, 0x9C, 0xCF, 0x08, 0xD0, 0x73, 0xD0, 0xDE, 0xD0, 0x4A, 0xD1, 0xB5, 0xD1
	.byte 0x21, 0xD2, 0x8D, 0xD2, 0xF8, 0xD2, 0x64, 0xD3, 0xD0, 0xD3, 0x3D, 0xD4, 0xA9, 0xD4, 0x15, 0xD5
	.byte 0x82, 0xD5, 0xEE, 0xD5, 0x5B, 0xD6, 0xC7, 0xD6, 0x34, 0xD7, 0xA1, 0xD7, 0x0E, 0xD8, 0x7B, 0xD8
	.byte 0xE9, 0xD8, 0x56, 0xD9, 0xC3, 0xD9, 0x31, 0xDA, 0x9E, 0xDA, 0x0C, 0xDB, 0x7A, 0xDB, 0xE8, 0xDB
	.byte 0x56, 0xDC, 0xC4, 0xDC, 0x32, 0xDD, 0xA0, 0xDD, 0x0F, 0xDE, 0x7D, 0xDE, 0xEC, 0xDE, 0x5B, 0xDF
	.byte 0xC9, 0xDF, 0x38, 0xE0, 0xA7, 0xE0, 0x16, 0xE1, 0x86, 0xE1, 0xF5, 0xE1, 0x64, 0xE2, 0xD4, 0xE2
	.byte 0x43, 0xE3, 0xB3, 0xE3, 0x23, 0xE4, 0x93, 0xE4, 0x03, 0xE5, 0x73, 0xE5, 0xE3, 0xE5, 0x54, 0xE6
	.byte 0xC4, 0xE6, 0x35, 0xE7, 0xA5, 0xE7, 0x16, 0xE8, 0x87, 0xE8, 0xF8, 0xE8, 0x69, 0xE9, 0xDA, 0xE9
	.byte 0x4B, 0xEA, 0xBC, 0xEA, 0x2E, 0xEB, 0x9F, 0xEB, 0x11, 0xEC, 0x83, 0xEC, 0xF5, 0xEC, 0x66, 0xED
	.byte 0xD9, 0xED, 0x4B, 0xEE, 0xBD, 0xEE, 0x2F, 0xEF, 0xA2, 0xEF, 0x14, 0xF0, 0x87, 0xF0, 0xFA, 0xF0
	.byte 0x6D, 0xF1, 0xE0, 0xF1, 0x53, 0xF2, 0xC6, 0xF2, 0x39, 0xF3, 0xAD, 0xF3, 0x20, 0xF4, 0x94, 0xF4
	.byte 0x07, 0xF5, 0x7B, 0xF5, 0xEF, 0xF5, 0x63, 0xF6, 0xD7, 0xF6, 0x4C, 0xF7, 0xC0, 0xF7, 0x34, 0xF8
	.byte 0xA9, 0xF8, 0x1E, 0xF9, 0x92, 0xF9, 0x07, 0xFA, 0x7C, 0xFA, 0xF1, 0xFA, 0x66, 0xFB, 0xDC, 0xFB
	.byte 0x51, 0xFC, 0xC7, 0xFC, 0x3C, 0xFD, 0xB2, 0xFD, 0x28, 0xFE, 0x9E, 0xFE, 0x14, 0xFF, 0x8A, 0xFF


	.section .rodata.unified_02,4,1,2
_0210E188:
	.byte 0x14, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x14, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00


	.section .rodata.unified_03,4,1,2
_0210E27C:
	.byte 0x0A, 0x00, 0x0A, 0x00
	.byte 0xD0, 0x07, 0x88, 0x13

LE26FV10N1TS_10:
	.word CTRDGi_WriteFlashSectorMX
	.word CTRDGi_EraseFlashChipMX
	.word CTRDGi_EraseFlashSectorMX
	.word CTRDGi_WriteFlashSectorAsyncMX
	.word CTRDGi_EraseFlashChipAsyncMX
	.word CTRDGi_EraseFlashSectorAsyncMX
	.word CTRDGi_PollingSR1MCOMMON
	.word _0210E27C
	.byte 0x00, 0x00, 0x02, 0x00, 0x00, 0x10, 0x00, 0x00, 0x0C, 0x00, 0x20, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x62, 0x00, 0x13, 0x00

_0210E2C0:
	.byte 0x0A, 0x00, 0x0A, 0x00, 0x28, 0x00, 0xC8, 0x00

defaultFlash512:
	.word CTRDGi_WriteFlashSectorLE
	.word CTRDGi_EraseFlashChipLE
	.word CTRDGi_EraseFlashSectorLE
	.word CTRDGi_WriteFlashSectorAsyncLE
	.word CTRDGi_EraseFlashChipAsyncLE
	.word CTRDGi_EraseFlashSectorAsyncLE
	.word CTRDGi_PollingSR512kCOMMON
	.word _0210E2C0
	.byte 0x00, 0x00, 0x01, 0x00, 0x00, 0x10, 0x00, 0x00
	.byte 0x0C, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00

LE39FW512:
	.word CTRDGi_WriteFlashSectorLE
	.word CTRDGi_EraseFlashChipLE
	.word CTRDGi_EraseFlashSectorLE
	.word CTRDGi_WriteFlashSectorAsyncLE
	.word CTRDGi_EraseFlashChipAsyncLE
	.word CTRDGi_EraseFlashSectorAsyncLE
	.word CTRDGi_PollingSR512kCOMMON
	.word _0210E2C0
	.byte 0x00, 0x00, 0x01, 0x00, 0x00, 0x10, 0x00, 0x00, 0x0C, 0x00, 0x10, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0xBF, 0x00, 0xD4, 0x00

_0210E340:
	.byte 0x0A, 0x00, 0x0A, 0x00, 0xD0, 0x07, 0xD0, 0x07

MX29L010:
	.word CTRDGi_WriteFlashSectorMX
	.word CTRDGi_EraseFlashChipMX
	.word CTRDGi_EraseFlashSectorMX
	.word CTRDGi_WriteFlashSectorAsyncMX
	.word CTRDGi_EraseFlashChipAsyncMX
	.word CTRDGi_EraseFlashSectorAsyncMX
	.word CTRDGi_PollingSR1MCOMMON
	.word _0210E340
	.byte 0x00, 0x00, 0x02, 0x00, 0x00, 0x10, 0x00, 0x00
	.byte 0x0C, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00
	.byte 0xC2, 0x00, 0x09, 0x00

defaultFlash1M:
	.word CTRDGi_WriteFlashSectorMX
	.word CTRDGi_EraseFlashChipMX
	.word CTRDGi_EraseFlashSectorMX
	.word CTRDGi_WriteFlashSectorAsyncMX
	.word CTRDGi_EraseFlashChipAsyncMX
	.word CTRDGi_EraseFlashSectorAsyncMX
	.word CTRDGi_PollingSR1MCOMMON
	.word _0210E340
	.byte 0x00, 0x00, 0x02, 0x00, 0x00, 0x10, 0x00, 0x00, 0x0C, 0x00, 0x20, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00

_0210E3C0:
	.byte 0x0A, 0x00, 0x0A, 0x00, 0xF4, 0x01, 0xF4, 0x01

MN63F805MNP:
	.word CTRDGi_WriteFlashSectorMX5
	.word CTRDGi_EraseFlashChipLE
	.word CTRDGi_EraseFlashSectorLE
	.word CTRDGi_WriteFlashSectorAsyncMX5
	.word CTRDGi_EraseFlashChipAsyncLE
	.word CTRDGi_EraseFlashSectorAsyncLE
	.word CTRDGi_PollingSR512kCOMMON
	.word _0210E3C0
	.byte 0x00, 0x00, 0x01, 0x00, 0x00, 0x10, 0x00, 0x00
	.byte 0x0C, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00
	.byte 0x32, 0x00, 0x1B, 0x00


	.section .data.unified_00,4,1,3
_02110CA8:
	.byte 0x80, 0x00, 0x00, 0x00
_02110CAC:
	.byte 0x01, 0x00, 0x00, 0x00
	.byte 0x06, 0x00, 0x00, 0x00, 0x0B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00
	.byte 0x0A, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00
	.byte 0x0E, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x0D, 0x00, 0x00, 0x00
	.byte 0x02, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00
	.byte 0x08, 0x00, 0x00, 0x00, 0x0B, 0x00, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00
	.byte 0x04, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x0D, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00
	.byte 0x0C, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x07, 0x00, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x00
	.byte 0x03, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00
	.byte 0x0F, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x0D, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00
	.byte 0x0B, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00
_02110D6C:
	.byte 0x78, 0xA4, 0x6A, 0xD7
	.byte 0x56, 0xB7, 0xC7, 0xE8, 0xDB, 0x70, 0x20, 0x24, 0xEE, 0xCE, 0xBD, 0xC1, 0xAF, 0x0F, 0x7C, 0xF5
	.byte 0x2A, 0xC6, 0x87, 0x47, 0x13, 0x46, 0x30, 0xA8, 0x01, 0x95, 0x46, 0xFD, 0xD8, 0x98, 0x80, 0x69
	.byte 0xAF, 0xF7, 0x44, 0x8B, 0xB1, 0x5B, 0xFF, 0xFF, 0xBE, 0xD7, 0x5C, 0x89, 0x22, 0x11, 0x90, 0x6B
	.byte 0x93, 0x71, 0x98, 0xFD, 0x8E, 0x43, 0x79, 0xA6, 0x21, 0x08, 0xB4, 0x49, 0x62, 0x25, 0x1E, 0xF6
	.byte 0x40, 0xB3, 0x40, 0xC0, 0x51, 0x5A, 0x5E, 0x26, 0xAA, 0xC7, 0xB6, 0xE9, 0x5D, 0x10, 0x2F, 0xD6
	.byte 0x53, 0x14, 0x44, 0x02, 0x81, 0xE6, 0xA1, 0xD8, 0xC8, 0xFB, 0xD3, 0xE7, 0xE6, 0xCD, 0xE1, 0x21
	.byte 0xD6, 0x07, 0x37, 0xC3, 0x87, 0x0D, 0xD5, 0xF4, 0xED, 0x14, 0x5A, 0x45, 0x05, 0xE9, 0xE3, 0xA9
	.byte 0xF8, 0xA3, 0xEF, 0xFC, 0xD9, 0x02, 0x6F, 0x67, 0x8A, 0x4C, 0x2A, 0x8D, 0x42, 0x39, 0xFA, 0xFF
	.byte 0x81, 0xF6, 0x71, 0x87, 0x22, 0x61, 0x9D, 0x6D, 0x0C, 0x38, 0xE5, 0xFD, 0x44, 0xEA, 0xBE, 0xA4
	.byte 0xA9, 0xCF, 0xDE, 0x4B, 0x60, 0x4B, 0xBB, 0xF6, 0x70, 0xBC, 0xBF, 0xBE, 0xC6, 0x7E, 0x9B, 0x28
	.byte 0xFA, 0x27, 0xA1, 0xEA, 0x85, 0x30, 0xEF, 0xD4, 0x05, 0x1D, 0x88, 0x04, 0x39, 0xD0, 0xD4, 0xD9
	.byte 0xE5, 0x99, 0xDB, 0xE6, 0xF8, 0x7C, 0xA2, 0x1F, 0x65, 0x56, 0xAC, 0xC4, 0x44, 0x22, 0x29, 0xF4
	.byte 0x97, 0xFF, 0x2A, 0x43, 0xA7, 0x23, 0x94, 0xAB, 0x39, 0xA0, 0x93, 0xFC, 0xC3, 0x59, 0x5B, 0x65
	.byte 0x92, 0xCC, 0x0C, 0x8F, 0x7D, 0xF4, 0xEF, 0xFF, 0xD1, 0x5D, 0x84, 0x85, 0x4F, 0x7E, 0xA8, 0x6F
	.byte 0xE0, 0xE6, 0x2C, 0xFE, 0x14, 0x43, 0x01, 0xA3, 0xA1, 0x11, 0x08, 0x4E, 0x82, 0x7E, 0x53, 0xF7
	.byte 0x35, 0xF2, 0x3A, 0xBD, 0xBB, 0xD2, 0xD7, 0x2A, 0x91, 0xD3, 0x86, 0xEB
_02110E6C:
	.byte 0x61, 0x00, 0x00, 0x00
_02110E70:
	.word DGTi_hash2_arm4_small
	.word _02110ED8
    .word _02110EF0
    .word _02110EA8
    .word _02110EC0
    .word _02110EA4
    .word _02110F08
    .word _02110E6C
    .word _02110F44
	.byte 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x40, 0x42, 0x0F, 0x00
	.byte 0x0A, 0x00, 0x00, 0x00

_02110EA4:
	.byte 0x61, 0x62, 0x63, 0x00

_02110EA8:
	.byte 0x34, 0xAA, 0x97, 0x3C, 0xD4, 0xC4, 0xDA, 0xA4
	.byte 0xF6, 0x1E, 0xEB, 0x2B, 0xDB, 0xAD, 0x27, 0x31, 0x65, 0x34, 0x01, 0x6F, 0x00, 0x00, 0x00, 0x00

_02110EC0:
	.byte 0xDE, 0xA3, 0x56, 0xA2, 0xCD, 0xDD, 0x90, 0xC7, 0xA7, 0xEC, 0xED, 0xC5, 0xEB, 0xB5, 0x63, 0x93
	.byte 0x4F, 0x46, 0x04, 0x52, 0x00, 0x00, 0x00, 0x00

_02110ED8:
	.byte 0xA9, 0x99, 0x3E, 0x36, 0x47, 0x06, 0x81, 0x6A
	.byte 0xBA, 0x3E, 0x25, 0x71, 0x78, 0x50, 0xC2, 0x6C, 0x9C, 0xD0, 0xD8, 0x9D, 0x00, 0x00, 0x00, 0x00

_02110EF0:
	.byte 0x84, 0x98, 0x3E, 0x44, 0x1C, 0x3B, 0xD2, 0x6E, 0xBA, 0xAE, 0x4A, 0xA1, 0xF9, 0x51, 0x29, 0xE5
	.byte 0xE5, 0x46, 0x70, 0xF1, 0x00, 0x00, 0x00, 0x00

_02110F08:
	.byte 0x61, 0x62, 0x63, 0x64, 0x62, 0x63, 0x64, 0x65
	.byte 0x63, 0x64, 0x65, 0x66, 0x64, 0x65, 0x66, 0x67, 0x65, 0x66, 0x67, 0x68, 0x66, 0x67, 0x68, 0x69
	.byte 0x67, 0x68, 0x69, 0x6A, 0x68, 0x69, 0x6A, 0x6B, 0x69, 0x6A, 0x6B, 0x6C, 0x6A, 0x6B, 0x6C, 0x6D
	.byte 0x6B, 0x6C, 0x6D, 0x6E, 0x6C, 0x6D, 0x6E, 0x6F, 0x6D, 0x6E, 0x6F, 0x70, 0x6E, 0x6F, 0x70, 0x71
	.byte 0x00, 0x00, 0x00, 0x00

_02110F44:
	.byte 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x30, 0x31, 0x32, 0x33
	.byte 0x34, 0x35, 0x36, 0x37, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x30, 0x31, 0x32, 0x33
	.byte 0x34, 0x35, 0x36, 0x37, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x30, 0x31, 0x32, 0x33
	.byte 0x34, 0x35, 0x36, 0x37, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x30, 0x31, 0x32, 0x33
	.byte 0x34, 0x35, 0x36, 0x37, 0x00, 0x00, 0x00, 0x00


	.section .data.unified_01,4,1,3
_02110FBC:
	.byte 0xFD, 0xFF, 0x00, 0x00
_02110FC0:
	.byte 0x00, 0x00, 0x01, 0x00
AgbFlash:
	.byte 0x00, 0x00, 0x00, 0x00
ctrdgi_fl_maxtime:
	.byte 0x00, 0x00, 0x00, 0x00
