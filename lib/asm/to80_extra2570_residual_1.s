	.extern AT29LV512_lib
	.extern AT29LV512_org
	.extern AgbFlash
	.extern AllocCommand
	.extern CARD_CancelBackupAsync
	.extern CARD_CheckEnabled
	.extern CARD_Enable
	.extern CARD_GetBackupSectorSize
	.extern CARD_GetResultCode
	.extern CARD_IdentifyBackup
	.extern CARD_Init
	.extern CARD_InitPulledOutCallback
	.extern CARD_IsEnabled
	.extern CARD_IsPulledOut
	.extern CARD_LockBackup
	.extern CARD_LockRom
	.extern CARD_SetCacheFlushThreshold
	.extern CARD_SetSpiReadWaitCycles
	.extern CARD_SetSpiWriteWaitCycles
	.extern CARD_SpiWaitGetStatus
	.extern CARD_SpiWaitInit
	.extern CARD_SpiWaitReadRange
	.extern CARD_SpiWaitWriteRange
	.extern CARD_TryWaitBackupAsync
	.extern CARD_UnlockBackup
	.extern CARD_UnlockRom
	.extern CARD_WaitBackupAsync
	.extern CARD_WaitRomAsync
	.extern CARDi_CheckPulledOutCore
	.extern CARDi_GetRomAccessor
	.extern CARDi_InitCommon
	.extern CARDi_LockResource
	.extern CARDi_OnFifoRecv
	.extern CARDi_OnReadCard
	.extern CARDi_PulledOutCallback
	.extern CARDi_ReadFromCache
	.extern CARDi_ReadRom
	.extern CARDi_ReadRomIDCore
	.extern CARDi_ReadRomSyncCore
	.extern CARDi_Request
	.extern CARDi_RequestStreamCommand
	.extern CARDi_SendtoPxi
	.extern CARDi_SetCardDma
	.extern CARDi_SetRomOp
	.extern CARDi_SetTask
	.extern CARDi_TaskThread
	.extern CARDi_TryWaitAsync
	.extern CARDi_UnlockResource
	.extern CARDi_WaitAsync
	.extern CP_SaveContext
	.extern CPi_RestoreContext
	.extern CTRDG_CheckEnabled
	.extern CTRDG_CpuCopy16
	.extern CTRDG_CpuCopy32
	.extern CTRDG_CpuCopy8
	.extern CTRDG_Enable
	.extern CTRDG_GetAgbGameCode
	.extern CTRDG_GetAgbMakerCode
	.extern CTRDG_IdentifyAgbBackup
	.extern CTRDG_Init
	.extern CTRDG_IsAgbCartridge
	.extern CTRDG_IsEnabled
	.extern CTRDG_IsExisting
	.extern CTRDG_IsOptionCartridge
	.extern CTRDG_IsPulledOut
	.extern CTRDG_Read32
	.extern CTRDG_ReadAgbFlash
	.extern CTRDG_TerminateForPulledOut
	.extern CTRDG_WriteAndVerifyAgbFlashAsync
	.extern CTRDGi_AccessCommon
	.extern CTRDGi_ChangeLatestAccessCycle
	.extern CTRDGi_CopyCommon
	.extern CTRDGi_EraseAgbFlashChip
	.extern CTRDGi_EraseAgbFlashChipAsync
	.extern CTRDGi_EraseAgbFlashSector
	.extern CTRDGi_EraseAgbFlashSectorAsync
	.extern CTRDGi_EraseFlash4KBAT
	.extern CTRDGi_EraseFlash4KBAsyncAT
	.extern CTRDGi_EraseFlash4KBCoreAT
	.extern CTRDGi_EraseFlashChipAT
	.extern CTRDGi_EraseFlashChipAsyncAT
	.extern CTRDGi_EraseFlashChipCoreAT
	.extern CTRDGi_EraseFlashSectorAT
	.extern CTRDGi_EraseFlashSectorAsyncAT
	.extern CTRDGi_EraseFlashSectorCoreAT
	.extern CTRDGi_GetAgbGameCodeAtInit
	.extern CTRDGi_GetAgbMakerCodeAtInit
	.extern CTRDGi_InitCommon
	.extern CTRDGi_IsAgbCartridgeAtInit
	.extern CTRDGi_LockByProcessor
	.extern CTRDGi_PollingSR
	.extern CTRDGi_RestoreAccessCycle
	.extern CTRDGi_SendtoPxi
	.extern CTRDGi_UnlockByProcessor
	.extern CTRDGi_Work
	.extern CTRDGi_WriteAgbFlashSector
	.extern CTRDGi_WriteAgbFlashSectorAsync
	.extern CTRDGi_WriteFlash4KBAT
	.extern CTRDGi_WriteFlash4KBAsyncAT
	.extern CTRDGi_WriteFlash4KBCoreAT
	.extern CTRDGi_WriteFlashSectorAT
	.extern CTRDGi_WriteFlashSectorAsyncAT
	.extern CTRDGi_WriteFlashSectorCoreAT
	.extern DC_FlushAll
	.extern DC_FlushRange
	.extern DC_InvalidateAll
	.extern DC_InvalidateRange
	.extern DC_StoreAll
	.extern DC_StoreRange
	.extern DC_WaitWriteBufferEmpty
	.extern DLAddFront
	.extern DLExtract
	.extern DLInsert
	.extern FS_ChangeDir
	.extern FS_ClearOverlayImage
	.extern FS_CloseFile
	.extern FS_ConvertPathToFileID
	.extern FS_EndOverlay
	.extern FS_FindArchive
	.extern FS_GetOverlayFileID
	.extern FS_Init
	.extern FS_InitArchive
	.extern FS_InitFile
	.extern FS_IsAvailable
	.extern FS_LoadArchive
	.extern FS_LoadArchiveTables
	.extern FS_LoadOverlay
	.extern FS_LoadOverlayImage
	.extern FS_LoadOverlayImageAsync
	.extern FS_LoadOverlayInfo
	.extern FS_NotifyArchiveAsyncEnd
	.extern FS_OpenFile
	.extern FS_OpenFileDirect
	.extern FS_OpenFileFast
	.extern FS_ReadFile
	.extern FS_ReadFileAsync
	.extern FS_RegisterArchiveName
	.extern FS_ReleaseArchiveName
	.extern FS_ResumeArchive
	.extern FS_SeekFile
	.extern FS_SetArchiveProc
	.extern FS_SetDefaultDMA
	.extern FS_StartOverlay
	.extern FS_SuspendArchive
	.extern FS_TryLoadTable
	.extern FS_UnloadArchive
	.extern FS_UnloadArchiveTables
	.extern FS_UnloadOverlay
	.extern FS_UnloadOverlayImage
	.extern FS_WaitAsync
	.extern FSi_CloseFileCommand
	.extern FSi_CompareDigest
	.extern FSi_EmptyArchiveProc
	.extern FSi_ExecuteAsyncCommand
	.extern FSi_ExecuteSyncCommand
	.extern FSi_FindPath
	.extern FSi_FindPathCommand
	.extern FSi_GetOverlayBinarySize
	.extern FSi_GetPackedName
	.extern FSi_GetPathCommand
	.extern FSi_InitRom
	.extern FSi_LoadOverlayInfoCore
	.extern FSi_NextCommand
	.extern FSi_OnRomReadDone
	.extern FSi_OpenFileDirectCommand
	.extern FSi_OpenFileFastCommand
	.extern FSi_ReadDirCommand
	.extern FSi_ReadDummyCallback
	.extern FSi_ReadFileCommand
	.extern FSi_ReadFileCore
	.extern FSi_ReadMemCallback
	.extern FSi_ReadMemoryCore
	.extern FSi_ReadRomCallback
	.extern FSi_ReadTable
	.extern FSi_ReleaseCommand
	.extern FSi_RomArchiveProc
	.extern FSi_SeekDirCommand
	.extern FSi_SeekDirDirect
	.extern FSi_StrNICmp
	.extern FSi_TranslateCommand
	.extern FSi_WriteDummyCallback
	.extern FSi_WriteFileCommand
	.extern FSi_WriteMemCallback
	.extern G2S_GetBG0ScrPtr
	.extern G2S_GetBG1ScrPtr
	.extern G2S_GetBG2ScrPtr
	.extern G2S_GetBG3ScrPtr
	.extern G2_GetBG0ScrPtr
	.extern G2_GetBG1ScrPtr
	.extern G2_GetBG2ScrPtr
	.extern G2_GetBG3ScrPtr
	.extern G3_LoadMtx44
	.extern GX_ResetBankForClearImage
	.extern GXi_DmaId
	.extern IC_InvalidateAll
	.extern IC_InvalidateRange
	.extern InitPXI
	.extern IsCommandAvailable
	.extern LE26FV10N1TS_10
	.extern LE39FW512
	.extern MATH_CalcCRC16
	.extern MATH_CalcCRC16CCITT
	.extern MATH_CalcCRC32
	.extern MATH_CalcCRC8
	.extern MATH_CalcMD5
	.extern MATH_CalcSHA1
	.extern MATH_CountPopulation
	.extern MATH_QSort
	.extern MATHi_CRC16InitTable
	.extern MATHi_CRC16InitTableRev
	.extern MATHi_CRC16Update
	.extern MATHi_CRC16UpdateRev
	.extern MATHi_CRC32InitTableRev
	.extern MATHi_CRC32UpdateRev
	.extern MATHi_CRC8InitTable
	.extern MATHi_CRC8Update
	.extern MIC_GetLastSamplingAddress
	.extern MIC_Init
	.extern MIC_StartAutoSampling
	.extern MIC_StartAutoSamplingAsync
	.extern MIC_StopAutoSampling
	.extern MI_CpuCopy8
	.extern MI_CpuFill8
	.extern MI_DmaCopy16
	.extern MI_DmaCopy32
	.extern MI_DmaFill32
	.extern MI_SetWramBank
	.extern MI_StopDma
	.extern MIi_CardDmaCopy32
	.extern MIi_CheckAnotherAutoDMA
	.extern MIi_CheckDma0SourceAddress
	.extern MIi_CpuClear16
	.extern MIi_CpuClear32
	.extern MIi_CpuClearFast
	.extern MIi_CpuCopy16
	.extern MIi_CpuCopy32
	.extern MIi_CpuCopyFast
	.extern MIi_DmaSetParams
	.extern MIi_UncompressBackward
	.extern MN63F805MNP
	.extern MX29L010
	.extern OS_CreateThread
	.extern OS_DisableInterrupts
	.extern OS_DisableIrqMask
	.extern OS_DisableProtectionUnit
	.extern OS_EnableIrqMask
	.extern OS_EnableProtectionUnit
	.extern OS_ExitThread
	.extern OS_GetArenaHi
	.extern OS_GetArenaLo
	.extern OS_GetConsoleType
	.extern OS_GetCpsrIrq
	.extern OS_GetDTCMAddress
	.extern OS_GetLockID
	.extern OS_GetMacAddress
	.extern OS_GetTick
	.extern OS_Halt
	.extern OS_InitIrqTable
	.extern OS_InitLock
	.extern OS_InitMessageQueue
	.extern OS_InitMutex
	.extern OS_InitThread
	.extern OS_IrqHandler
	.extern OS_IsRunOnEmulator
	.extern OS_IsTickAvailable
	.extern OS_JamMessage
	.extern OS_LockByWord
	.extern OS_LockCard
	.extern OS_LockCartridge
	.extern OS_LockMutex
	.extern OS_ReadOwnerOfLockWord
	.extern OS_ReceiveMessage
	.extern OS_ResetRequestIrqMask
	.extern OS_RestoreInterrupts
	.extern OS_SendMessage
	.extern OS_SetDPermissionsForProtectionRegion
	.extern OS_SetIrqFunction
	.extern OS_SetIrqMask
	.extern OS_SetIrqStackChecker
	.extern OS_SetThreadPriority
	.extern OS_SleepThread
	.extern OS_SpinWait
	.extern OS_Terminate
	.extern OS_TryLockCartridge
	.extern OS_UnLockCard
	.extern OS_UnLockCartridge
	.extern OS_UnlockByWord
	.extern OS_UnlockCard
	.extern OS_UnlockCartridge
	.extern OS_UnlockMutex
	.extern OS_WakeupThread
	.extern OS_WakeupThreadDirect
	.extern OSi_DoLockByWord
	.extern OSi_DoTryLockByWord
	.extern OSi_DoUnlockByWord
	.extern OSi_EnterDmaCallback
	.extern OSi_EnterTimerCallback
	.extern OSi_ExitThread
	.extern OSi_ExitThread_ArgSpecified
	.extern OSi_ExitThread_Destroy
	.extern OSi_GetUnusedThreadId
	.extern OSi_InsertAlarm
	.extern OSi_InsertLinkToQueue
	.extern OSi_InsertThreadToList
	.extern OSi_IrqCallback
	.extern OSi_IrqCallbackInfo
	.extern OSi_IrqCallbackInfoIndex
	.extern OSi_IrqThreadQueue
	.extern OSi_ReferSymbol
	.extern OSi_RemoveLinkFromQueue
	.extern OSi_RemoveMutexLinkFromQueue
	.extern OSi_RemoveSpecifiedLinkFromQueue
	.extern OSi_RemoveThreadFromList
	.extern OSi_RescheduleThread
	.extern OSi_SendToPxi
	.extern OSi_SetTimer
	.extern OSi_SetTimerReserved
	.extern OSi_ThreadInfo
	.extern OSi_WaitVCount0
	.extern PM_AppendPostSleepCallback
	.extern PM_DeletePostSleepCallback
	.extern PM_DeletePreSleepCallback
	.extern PM_ForceToPowerOff
	.extern PM_GetBackLight
	.extern PM_GetLEDPattern
	.extern PM_GoSleepMode
	.extern PM_Init
	.extern PM_PrependPreSleepCallback
	.extern PM_SetAmp
	.extern PM_SetAmpGain
	.extern PM_SetBackLight
	.extern PM_SetLCDPower
	.extern PMi_SendLEDPatternCommand
	.extern PXI_Init
	.extern PXI_InitFifo
	.extern PXI_IsCallbackReady
	.extern PXI_SendWordByFifo
	.extern PXI_SetFifoRecvCallback
	.extern PXIi_HandlerRecvFifoNotEmpty
	.extern PushCommand_impl
	.extern PxiFifoCallback
	.extern RTC_ConvertDateTimeToSecond
	.extern RTC_ConvertDateToDay
	.extern RTC_ConvertDayToDate
	.extern RTC_ConvertSecondToDateTime
	.extern RTC_GetDate
	.extern RTC_GetDateTime
	.extern RTC_GetDateTimeAsync
	.extern RTC_GetDayOfWeek
	.extern RTC_GetTime
	.extern RTC_Init
	.extern RTCi_ConvertSecondToTime
	.extern RTCi_ConvertTimeToSecond
	.extern RTCi_ReadRawDateAsync
	.extern RTCi_ReadRawDateTimeAsync
	.extern RTCi_ReadRawTimeAsync
	.extern RTCi_WriteRawStatus2Async
	.extern RequestCommandProc
	.extern RtcSendPxiCommand
	.extern SDK_OVERLAY_DIGEST
	.extern SDK_OVERLAY_DIGEST_END
	.extern SDK_OVERLAY_OVY_0_ID
	.extern SDK_OVERLAY_OVY_102_ID
	.extern SDK_OVERLAY_OVY_103_ID
	.extern SDK_OVERLAY_OVY_104_ID
	.extern SDK_OVERLAY_OVY_105_ID
	.extern SDK_OVERLAY_OVY_106_ID
	.extern SDK_OVERLAY_OVY_107_ID
	.extern SDK_OVERLAY_OVY_108_ID
	.extern SDK_OVERLAY_OVY_109_ID
	.extern SDK_OVERLAY_OVY_10_ID
	.extern SDK_OVERLAY_OVY_110_ID
	.extern SDK_OVERLAY_OVY_111_ID
	.extern SDK_OVERLAY_OVY_112_ID
	.extern SDK_OVERLAY_OVY_113_ID
	.extern SDK_OVERLAY_OVY_114_ID
	.extern SDK_OVERLAY_OVY_115_ID
	.extern SDK_OVERLAY_OVY_116_ID
	.extern SDK_OVERLAY_OVY_117_ID
	.extern SDK_OVERLAY_OVY_118_ID
	.extern SDK_OVERLAY_OVY_119_ID
	.extern SDK_OVERLAY_OVY_11_ID
	.extern SDK_OVERLAY_OVY_120_ID
	.extern SDK_OVERLAY_OVY_124_ID
	.extern SDK_OVERLAY_OVY_125_ID
	.extern SDK_OVERLAY_OVY_126_ID
	.extern SDK_OVERLAY_OVY_127_ID
	.extern SDK_OVERLAY_OVY_128_ID
	.extern SDK_OVERLAY_OVY_12_ID
	.extern SDK_OVERLAY_OVY_13_ID
	.extern SDK_OVERLAY_OVY_14_ID
	.extern SDK_OVERLAY_OVY_15_ID
	.extern SDK_OVERLAY_OVY_16_ID
	.extern SDK_OVERLAY_OVY_17_ID
	.extern SDK_OVERLAY_OVY_18_ID
	.extern SDK_OVERLAY_OVY_19_ID
	.extern SDK_OVERLAY_OVY_20_ID
	.extern SDK_OVERLAY_OVY_21_ID
	.extern SDK_OVERLAY_OVY_22_ID
	.extern SDK_OVERLAY_OVY_25_ID
	.extern SDK_OVERLAY_OVY_26_ID
	.extern SDK_OVERLAY_OVY_27_ID
	.extern SDK_OVERLAY_OVY_28_ID
	.extern SDK_OVERLAY_OVY_29_ID
	.extern SDK_OVERLAY_OVY_2_ID
	.extern SDK_OVERLAY_OVY_31_ID
	.extern SDK_OVERLAY_OVY_32_ID
	.extern SDK_OVERLAY_OVY_33_ID
	.extern SDK_OVERLAY_OVY_34_ID
	.extern SDK_OVERLAY_OVY_35_ID
	.extern SDK_OVERLAY_OVY_36_ID
	.extern SDK_OVERLAY_OVY_37_ID
	.extern SDK_OVERLAY_OVY_38_ID
	.extern SDK_OVERLAY_OVY_39_ID
	.extern SDK_OVERLAY_OVY_3_ID
	.extern SDK_OVERLAY_OVY_40_ID
	.extern SDK_OVERLAY_OVY_41_ID
	.extern SDK_OVERLAY_OVY_42_ID
	.extern SDK_OVERLAY_OVY_43_ID
	.extern SDK_OVERLAY_OVY_44_ID
	.extern SDK_OVERLAY_OVY_45_ID
	.extern SDK_OVERLAY_OVY_46_ID
	.extern SDK_OVERLAY_OVY_47_ID
	.extern SDK_OVERLAY_OVY_48_ID
	.extern SDK_OVERLAY_OVY_49_ID
	.extern SDK_OVERLAY_OVY_4_ID
	.extern SDK_OVERLAY_OVY_53_ID
	.extern SDK_OVERLAY_OVY_55_ID
	.extern SDK_OVERLAY_OVY_56_ID
	.extern SDK_OVERLAY_OVY_57_ID
	.extern SDK_OVERLAY_OVY_58_ID
	.extern SDK_OVERLAY_OVY_59_ID
	.extern SDK_OVERLAY_OVY_5_ID
	.extern SDK_OVERLAY_OVY_61_ID
	.extern SDK_OVERLAY_OVY_62_ID
	.extern SDK_OVERLAY_OVY_63_ID
	.extern SDK_OVERLAY_OVY_64_ID
	.extern SDK_OVERLAY_OVY_65_ID
	.extern SDK_OVERLAY_OVY_66_ID
	.extern SDK_OVERLAY_OVY_67_ID
	.extern SDK_OVERLAY_OVY_68_ID
	.extern SDK_OVERLAY_OVY_69_ID
	.extern SDK_OVERLAY_OVY_6_ID
	.extern SDK_OVERLAY_OVY_70_ID
	.extern SDK_OVERLAY_OVY_71_ID
	.extern SDK_OVERLAY_OVY_72_ID
	.extern SDK_OVERLAY_OVY_73_ID
	.extern SDK_OVERLAY_OVY_74_ID
	.extern SDK_OVERLAY_OVY_75_ID
	.extern SDK_OVERLAY_OVY_77_ID
	.extern SDK_OVERLAY_OVY_79_ID
	.extern SDK_OVERLAY_OVY_7_ID
	.extern SDK_OVERLAY_OVY_80_ID
	.extern SDK_OVERLAY_OVY_81_ID
	.extern SDK_OVERLAY_OVY_82_ID
	.extern SDK_OVERLAY_OVY_83_ID
	.extern SDK_OVERLAY_OVY_84_ID
	.extern SDK_OVERLAY_OVY_85_ID
	.extern SDK_OVERLAY_OVY_86_ID
	.extern SDK_OVERLAY_OVY_87_ID
	.extern SDK_OVERLAY_OVY_88_ID
	.extern SDK_OVERLAY_OVY_89_ID
	.extern SDK_OVERLAY_OVY_8_ID
	.extern SDK_OVERLAY_OVY_90_ID
	.extern SDK_OVERLAY_OVY_91_ID
	.extern SDK_OVERLAY_OVY_92_ID
	.extern SDK_OVERLAY_OVY_93_ID
	.extern SDK_OVERLAY_OVY_94_ID
	.extern SDK_OVERLAY_OVY_95_ID
	.extern SDK_OVERLAY_OVY_96_ID
	.extern SDK_OVERLAY_OVY_97_ID
	.extern SDK_OVERLAY_OVY_98_ID
	.extern SDK_OVERLAY_OVY_99_ID
	.extern SDK_OVERLAY_OVY_9_ID
	.extern SDK_OVERLAY_OV_101_ID
	.extern SDK_OVERLAY_bug_contest_ID
	.extern SDK_OVERLAY_certificates_app_ID
	.extern SDK_OVERLAY_credits_ID
	.extern SDK_OVERLAY_ds_protect_ID
	.extern SDK_OVERLAY_field_ID
	.extern SDK_OVERLAY_intro_title_ID
	.extern SDK_OVERLAY_npc_trade_ID
	.extern SDK_OVERLAY_options_app_ID
	.extern SDK_OVERLAY_pokegear_ID
	.extern SDK_OVERLAY_pokegear_app_ID
	.extern SDK_OVERLAY_touch_save_app_ID
	.extern SDK_OVERLAY_trainer_card_ID
	.extern SDK_OVERLAY_trainer_card_main_ID
	.extern SDK_OVERLAY_trainer_card_signature_ID
	.extern SDK_OVERLAY_view_rankings_app_ID
	.extern SDK_OVERLAY_voltorb_flip_ID
	.extern SND_AlarmInit
	.extern SND_AllocCommand
	.extern SND_AssignWaveArc
	.extern SND_CalcChannelVolume
	.extern SND_CountFreeCommand
	.extern SND_CountReservedCommand
	.extern SND_CountWaitingCommand
	.extern SND_DestroyBank
	.extern SND_DestroyWaveArc
	.extern SND_FlushCommand
	.extern SND_GetChannelStatus
	.extern SND_GetCurrentCommandTag
	.extern SND_GetFirstInstDataPos
	.extern SND_GetNextInstData
	.extern SND_GetPlayerLocalVariable
	.extern SND_GetPlayerStatus
	.extern SND_GetWaveDataAddress
	.extern SND_GetWaveDataCount
	.extern SND_Init
	.extern SND_InvalidateBankData
	.extern SND_InvalidateSeqData
	.extern SND_InvalidateWaveData
	.extern SND_IsFinishedCommandTag
	.extern SND_LockChannel
	.extern SND_PauseSeq
	.extern SND_PrepareSeq
	.extern SND_PushCommand
	.extern SND_ReadChannelInfo
	.extern SND_ReadDriverInfo
	.extern SND_ReadTrackInfo
	.extern SND_RecvCommandReply
	.extern SND_ResetMasterPan
	.extern SND_SetChannelPan
	.extern SND_SetChannelTimer
	.extern SND_SetChannelVolume
	.extern SND_SetMasterPan
	.extern SND_SetMasterVolume
	.extern SND_SetOutputSelector
	.extern SND_SetPlayerChannelPriority
	.extern SND_SetPlayerTempoRatio
	.extern SND_SetPlayerVolume
	.extern SND_SetTrackAllocatableChannel
	.extern SND_SetTrackPan
	.extern SND_SetTrackPitch
	.extern SND_SetWaveDataAddress
	.extern SND_SetupAlarm
	.extern SND_SetupChannelPcm
	.extern SND_StartPreparedSeq
	.extern SND_StartTimer
	.extern SND_StopSeq
	.extern SND_StopTimer
	.extern SND_UnlockChannel
	.extern SND_WaitForCommandProc
	.extern SNDi_CallAlarmHandler
	.extern SNDi_DecibelTable
	.extern SNDi_GetFinishedCommandTag
	.extern SNDi_IncAlarmId
	.extern SNDi_InitSharedWork
	.extern SNDi_LockMutex
	.extern SNDi_SetAlarmHandler
	.extern SNDi_SetPlayerParam
	.extern SNDi_SetTrackParam
	.extern SNDi_SharedWork
	.extern SNDi_UnlockMutex
	.extern STD_CompareNString
	.extern STD_CompareString
	.extern STD_CopyLString
	.extern STD_GetStringLength
	.extern SVC_CpuSet
	.extern SVC_WaitByLoop
	.extern TP_CheckBusy
	.extern TP_GetCalibratedPoint
	.extern TP_GetLatestIndexInAuto
	.extern TP_GetLatestRawPointInAuto
	.extern TP_GetUserInfo
	.extern TP_Init
	.extern TP_RequestAutoSamplingStartAsync
	.extern TP_RequestAutoSamplingStopAsync
	.extern TP_SetCalibrateParam
	.extern TP_WaitBusy
	.extern WM_Disable
	.extern WM_Disconnect
	.extern WM_Enable
	.extern WM_End
	.extern WM_EndDCF
	.extern WM_EndKeySharing
	.extern WM_EndMP
	.extern WM_EndParent
	.extern WM_EndScan
	.extern WM_Finish
	.extern WM_GetAllowedChannel
	.extern WM_GetDispersionBeaconPeriod
	.extern WM_GetDispersionScanPeriod
	.extern WM_GetLinkLevel
	.extern WM_GetNextTgid
	.extern WM_GetOtherElements
	.extern WM_GetSharedDataAddress
	.extern WM_Init
	.extern WM_Initialize
	.extern WM_InitializeForListening
	.extern WM_MeasureChannel
	.extern WM_PowerOff
	.extern WM_PowerOn
	.extern WM_ReadMPData
	.extern WM_ReadStatus
	.extern WM_Reset
	.extern WM_SetBeaconIndication
	.extern WM_SetDCFData
	.extern WM_SetEntry
	.extern WM_SetGameInfo
	.extern WM_SetIndCallback
	.extern WM_SetLifeTime
	.extern WM_SetMPDataToPortEx
	.extern WM_SetParentParameter
	.extern WM_SetPortCallback
	.extern WM_SetWEPKey
	.extern WM_SetWEPKeyEx
	.extern WM_StartConnectEx
	.extern WM_StartDCF
	.extern WM_StartDataSharing
	.extern WM_StartKeySharing
	.extern WM_StartMP
	.extern WM_StartMPEx
	.extern WM_StartParent
	.extern WM_StartScan
	.extern WM_StartScanEx
	.extern WM_StepDataSharing
	.extern WMi_GetStatusAddress
	.extern WMi_StartParentEx
	.extern _0210D750
	.extern _0210E188
	.extern _02110CA8
	.extern _02110CAC
	.extern _02110D6C
	.extern _02110E70
	.extern __global_destructor_chain
	.extern _ll_mod
	.extern _ll_sdiv
	.extern _ll_udiv
	.extern _version_NINTENDO_BACKUP
	.extern cardi_common
	.extern cardi_rom_base
	.extern cardi_rom_header_addr
	.extern ctrdg_flash_remainder
	.extern ctrdgi_backup_irq
	.extern ctrdgi_fl_maxtime
	.extern ctrdgi_flash_lock_id
	.extern ctrdgi_sram_lock_id
	.extern defaultFlash1M
	.extern defaultFlash512
	.extern fsi_default_command
	.extern rom_stat
	.extern sCallbackTable
	.extern sCommandArray
	.extern sDayOfYear
	.extern sFreeList
	.extern sSharedWork
	.extern sSndInitialized
	.extern sSndMutex
	.extern sVolumeTable
	.extern sWaitingCommandListQueue
	.include "asm/macros.inc"
	.include "nitro.inc"
	.include "global.inc"
	.public ProcessBlock

.text
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
