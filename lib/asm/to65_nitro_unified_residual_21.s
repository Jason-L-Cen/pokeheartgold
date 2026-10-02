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
	.extern _0210E188
	.extern _02110CA8
	.extern _02110CAC
	.extern _02110D6C
	.extern _02110E70
	.public DGT_Hash1GetDigest_R
	.public DGT_Hash1Reset
	.public DGT_Hash1SetSource
	.public DGT_Hash2CalcHmac
	.public DGT_Hash2GetDigest
	.public DGT_Hash2Reset
	.public DGT_Hash2SetSource
	.public DGTi_hash2_arm4_small
	.public HmacCalc
	.public ProcessBlock
	.public _020D8E30
	.public _020D8E34
	.public _020D8E38
	.public _020D8E3C
	.public _020D8E94
	.public _020D8EC8
	.public _020D8EF0
	.public _020D8F6C
	.public _020D8F88
	.public _020D8FBC
	.public _020D8FD8
	.public _020D90B4
	.public _020D919C
	.public _020D9264
	.public _020D936C
	.public _020D9370
	.public _020D93B0
	.public _020D93B4
	.public _020D93B8
	.public _020D93BC
	.public _020D93C0
	.public _020D945C
	.public _020D947C
	.public _020D94B4
	.public _020D94C0
	.public _020D94F4
	.public _020D9514
	.public _020D9560
	.public _020D9570
	.public _020D9590
	.public _020D95A0
	.public _020D95BC
	.public _020D95C8
	.public _020D95D8
	.public _020D9710
	.public _020D9798
	.public _020D979C
	.public _020D97A0
	.public _020D97A4
	.public _020D9828
	.public _020D9838
	.public _020D9850
	.public _020D9868
	.public _020D987C
	.public _020D98D0
	.public _020D98E8
	.public _020D9900
	.public _020D9914
	.public _020D9960
	.public _020D9964
	.public _020D9968
	.public _020D996C
	.public _020D9970
	.public _020D9984
	.public _020D9994
	.public _020D99F4
	.public _020D9A64
	.public _020D9AD8
	.public _020D9B54
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
