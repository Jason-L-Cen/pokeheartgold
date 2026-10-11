	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"

	.public sIsDispOn
	.public GXi_DmaId

	.bss

	.public gFndRootHeapListInitialized
gFndRootHeapListInitialized:
	.space 0x4

	.public gFndRootHeapList
gFndRootHeapList:
	.space 0xC

	.public gFrmTexVramSlotCount
gFrmTexVramSlotCount:
	.space 0x4

	.public gFrmPlttVramManager
gFrmPlttVramManager:
	.space 0xC

	.public gVramTransferTaskQueue
	.public gLnkTexVramFreeList
	.public gLnkTexVramFreeList4x4
	.public gLnkTexVramWork
	.public gLnkPlttVramManager
	.public _021D8504
	.public gOamManagerNextId
	.public gOamBuffers
	.public gOamBufferMainOam
	.public gOamBufferMainAffineOwners
	.public gOamBufferSubOamOwners
	.public gOamBufferSubOam
	.public gOamBufferSubAffineOwners
	.public gOamBufferSoftOamOwners
	.public gOamBufferSoftOam
	.public gOamBufferSoftAffineOwners
	.public gRendererWork
	.public gRendererZStack
	.public gRendererMtxCacheRecords
	.public _021D956C
	.public gRendererMtxStack
	.public gRendererMtxStackTransX
	.public gRendererMtxStackTransY
	.public gRendererAffineMtxStack
	.public gRendererMtxCaches
	.public gRendererWorkB00
	.public gRendererCellAnimMtxCaches
	.public gRendererCellAnimMtxCacheTable
	.public gCellTransferStateManager
	.public gOamSoftEmuState
	.public gCurrentRndCore
	.public NNS_G3dGlb
	.public gCurrentRenderState
	.public gMatAnmResultBuffer
	.public _021DB504
	.public _021DB508
	.public _021DB50C
	.public _021DB510
	.public _021DB514
	.public _021DB518
	.public gEvpMtxCache
	.public gEvpMtxCacheFirstDirMtx
	.public _021DD404
	.public _021DD408
	.public _021DD420
	.public _021DD430
	.public _021DD43C
	.public _021DD460
	.public _021DF820
	.public _021DF82C
	.public _021DF96C
	.public _021DF978
	.public _021DF984
	.public _021DFDC4
	.public _021DFDCC
	.public _021DFDDC
	.public _021DFDE0
	.public _021DFDE4
	.public _021E0244
	.public _021E0248
	.public _021E0254
	.public _021E0294
	.public _021E0314
	.public _021E031C
	.public _021E035C
	.public _021E0910
	.public _021E0914
	.public _021E0950
	.public _021E095C
	.public _021E0FC8
	.public _021E0FE0
	.public _021E0FEC
	.public NNS_G3dGlb
	.public NNS_GfdDefaultFuncAllocPlttVram
	.public NNS_GfdDefaultFuncAllocTexVram
	.public NNS_GfdDefaultFuncFreePlttVram
	.public NNS_GfdDefaultFuncFreeTexVram
	.public gFndExpHeapAllocatorFunc
	.public gVramTransferFuncTable
	.public gLnkTexVramSlotTable
	.public gG2dSoftSpriteCameraUp
	.public gG2dSoftSpriteCameraTarget
	.public gObjHeights
	.public gObjWidths
	.public gCellTransferDstTypes
	.public gObjColorModeTexFmts
	.public gTexSizeInChars
	.public gObjTexSizeS
	.public gObjTexSizeT
	.public gRndCoreIdentityMtx
	.public gCharCanvasVTableBG
	.public gCharCanvasVTableObj1D
	.public gCharShiftTable
	.public gObjShapeSizeAttrs
	.public gMatColorMasks
	.public gPivotMtxIndices
	.public _021094B8
	.public gFrmTexVramSearch4x4
	.public gFrmTexVramSearchNormal
	.public gFrmTexVramRegions
	.public gFrmTexVramRegion1
	.public gFrmTexVramRegion2
	.public gOamLoadFuncs
	.public gRendererContext
	.public gRendererTranslationMtx
	.public gOamSoftEmuAutoZOffsetStep
	.public gAnmObjInitFuncCount
	.public _02110A04
	.public _02110A08
	.public _02110A0C
	.public _02110A10
	.public _02110A14
	.public gAnmBlendVisDefault
	.public gAnmBlendJntDefault
	.public gAnmBlendMatDefault
	.public gAnmObjInitFuncs
	.public gAnmObjInitFuncsFirstFunc
	.public gSendJointSRTFuncs
	.public gGetJointScaleFuncs
	.public gSendTexSRTFuncs
	.public gTexGenTexImageCmds
	.public gPrjMapTexImageParam
	.public gEnvMapTexImageParam
	.public gSbcMatFuncs
	.public gSbcShpFuncs
	.public gPrjMapTexMtx
	.public gBillboardCmd
	.public gBillboardCmdParams
	.public gBillboardMtx
	.public gBillboardTrans
	.public gBillboardScale
	.public gBillboardYCmd
	.public gBillboardYCmdParams
	.public gBillboardYMtx
	.public gBillboardYTrans
	.public gBillboardYScale
	.public gSbcFuncs
	.public _02110C1C
	.public _02110C3C
	.public _02110C5C
	.public gFndRootHeapListInitialized
	.public gFndRootHeapList
	.public gFrmTexVramSlotCount
	.public gFrmPlttVramManager
	.public gVramTransferTaskQueue
	.public gLnkTexVramFreeList
	.public gLnkTexVramFreeList4x4
	.public gLnkTexVramWork
	.public gLnkPlttVramManager
	.public _021D8504
	.public gOamManagerNextId
	.public gOamBuffers
	.public gOamBufferMainOam
	.public gOamBufferMainAffineOwners
	.public gOamBufferSubOamOwners
	.public gOamBufferSubOam
	.public gOamBufferSubAffineOwners
	.public gOamBufferSoftOamOwners
	.public gOamBufferSoftOam
	.public gOamBufferSoftAffineOwners
	.public gRendererWork
	.public gRendererZStack
	.public gRendererMtxCacheRecords
	.public _021D956C
	.public gRendererMtxStack
	.public gRendererMtxStackTransX
	.public gRendererMtxStackTransY
	.public gRendererAffineMtxStack
	.public gRendererMtxCaches
	.public gRendererWorkB00
	.public gRendererCellAnimMtxCaches
	.public gRendererCellAnimMtxCacheTable
	.public gCellTransferStateManager
	.public gOamSoftEmuState
	.public gCurrentRndCore
	.public gCurrentRenderState
	.public gMatAnmResultBuffer
	.public _021DB504
	.public _021DB508
	.public _021DB50C
	.public _021DB510
	.public _021DB514
	.public _021DB518
	.public gEvpMtxCache
	.public gEvpMtxCacheFirstDirMtx
	.public _021DD404
	.public _021DD408
	.public _021DD420
	.public _021DD430
	.public _021DD43C
	.public _021DD460
	.public _021DF820
	.public _021DF82C
	.public _021DF96C
	.public _021DF978
	.public _021DF984
	.public _021DFDC4
	.public _021DFDCC
	.public _021DFDDC
	.public _021DFDE0
	.public _021DFDE4
	.public _021E0244
	.public _021E0248
	.public _021E0254
	.public _021E0294
	.public _021E0314
	.public _021E031C
	.public _021E035C
	.public _021E0910
	.public _021E0914
	.public _021E0950
	.public _021E095C
	.public _021E0FC8
	.public _021E0FE0
	.public _021E0FEC
	.extern AllocPlttVram_
	.extern AllocTexVram_
	.extern AllocatorAllocForExpHeap
	.extern AllocatorAllocForSDKHeap
	.extern AllocatorFreeForExpHeap
	.extern AllocatorFreeForSDKHeap
	.extern ClearArea1D
	.extern ClearAreaLine
	.extern ClearContinuous
	.extern ClearLine
	.extern CpuLoadOAMMain_
	.extern CpuLoadOAMSub_
	.extern DoTransfer2dBG0CharMain
	.extern DoTransfer2dBG0CharSub
	.extern DoTransfer2dBG0ScrMain
	.extern DoTransfer2dBG0ScrSub
	.extern DoTransfer2dBG1CharMain
	.extern DoTransfer2dBG1CharSub
	.extern DoTransfer2dBG1ScrMain
	.extern DoTransfer2dBG1ScrSub
	.extern DoTransfer2dBG2BmpMain
	.extern DoTransfer2dBG2BmpSub
	.extern DoTransfer2dBG2CharMain
	.extern DoTransfer2dBG2CharSub
	.extern DoTransfer2dBG2ScrMain
	.extern DoTransfer2dBG2ScrSub
	.extern DoTransfer2dBG3BmpMain
	.extern DoTransfer2dBG3BmpSub
	.extern DoTransfer2dBG3CharMain
	.extern DoTransfer2dBG3CharSub
	.extern DoTransfer2dBG3ScrMain
	.extern DoTransfer2dBG3ScrSub
	.extern DoTransfer2dBGExtPlttMain
	.extern DoTransfer2dBGExtPlttSub
	.extern DoTransfer2dBGPlttMain
	.extern DoTransfer2dBGPlttSub
	.extern DoTransfer2dObjCharMain
	.extern DoTransfer2dObjCharSub
	.extern DoTransfer2dObjExtPlttMain
	.extern DoTransfer2dObjExtPlttSub
	.extern DoTransfer2dObjOamMain
	.extern DoTransfer2dObjOamSub
	.extern DoTransfer2dObjPlttMain
	.extern DoTransfer2dObjPlttSub
	.extern DoTransfer3dClearImageColor
	.extern DoTransfer3dClearImageDepth
	.extern DoTransfer3dTex
	.extern DoTransfer3dTexPltt
	.extern DrawGlyph1D
	.extern DrawGlyphLine
	.extern FreePlttVram_
	.extern FreeTexVram_
	.extern NNSi_G3dAnmBlendJnt
	.extern NNSi_G3dAnmBlendMat
	.extern NNSi_G3dAnmBlendVis
	.extern NNSi_G3dAnmCalcNsBca
	.extern NNSi_G3dAnmCalcNsBma
	.extern NNSi_G3dAnmCalcNsBta
	.extern NNSi_G3dAnmCalcNsBtp
	.extern NNSi_G3dAnmCalcNsBva
	.extern NNSi_G3dAnmObjInitNsBca
	.extern NNSi_G3dAnmObjInitNsBma
	.extern NNSi_G3dAnmObjInitNsBta
	.extern NNSi_G3dAnmObjInitNsBtp
	.extern NNSi_G3dAnmObjInitNsBva
	.extern NNSi_G3dFuncSbc_BB
	.extern NNSi_G3dFuncSbc_BBY
	.extern NNSi_G3dFuncSbc_CALLDL
	.extern NNSi_G3dFuncSbc_ENVMAP
	.extern NNSi_G3dFuncSbc_MAT
	.extern NNSi_G3dFuncSbc_MAT_InternalDefault
	.extern NNSi_G3dFuncSbc_MTX
	.extern NNSi_G3dFuncSbc_NODE
	.extern NNSi_G3dFuncSbc_NODEDESC
	.extern NNSi_G3dFuncSbc_NODEMIX
	.extern NNSi_G3dFuncSbc_NOP
	.extern NNSi_G3dFuncSbc_POSSCALE
	.extern NNSi_G3dFuncSbc_PRJMAP
	.extern NNSi_G3dFuncSbc_RET
	.extern NNSi_G3dFuncSbc_SHP
	.extern NNSi_G3dFuncSbc_SHP_InternalDefault
	.extern NNSi_G3dGetJointScaleBasic
	.extern NNSi_G3dGetJointScaleMaya
	.extern NNSi_G3dGetJointScaleSi3d
	.extern NNSi_G3dSendJointSRTBasic
	.extern NNSi_G3dSendJointSRTMaya
	.extern NNSi_G3dSendJointSRTSi3d
	.extern NNSi_G3dSendTexSRT3dsMax
	.extern NNSi_G3dSendTexSRTMaya
	.extern NNSi_G3dSendTexSRTSi3d
	.extern NNSi_G3dSendTexSRTXsi
	.extern texmtxCalc_flagRS_
	.extern texmtxCalc_flagRS___3dsmax
	.extern texmtxCalc_flagRS___xsi
	.extern texmtxCalc_flagR_
	.extern texmtxCalc_flagR___3dsmax
	.extern texmtxCalc_flagR___xsi
	.extern texmtxCalc_flagS_
	.extern texmtxCalc_flagS___3dsmax
	.extern texmtxCalc_flagS___xsi
	.extern texmtxCalc_flagTRS_
	.extern texmtxCalc_flagTRS___3dsmax
	.extern texmtxCalc_flagTRS___xsi
	.extern texmtxCalc_flagTR_
	.extern texmtxCalc_flagTR___3dsmax
	.extern texmtxCalc_flagTR___xsi
	.extern texmtxCalc_flagTS_
	.extern texmtxCalc_flagTS___3dsmax
	.extern texmtxCalc_flagTS___xsi
	.extern texmtxCalc_flagT_
	.extern texmtxCalc_flagT___3dsmax
	.extern texmtxCalc_flagT___xsi
	.extern texmtxCalc_flag_
	.extern texmtxCalc_flag___3dsmax
	.extern texmtxCalc_flag___xsi
.text
	.rodata

gFndExpHeapAllocatorFunc:
	.word AllocatorAllocForExpHeap
	.word AllocatorFreeForExpHeap
	.word AllocatorAllocForSDKHeap
	.word AllocatorFreeForSDKHeap
gVramTransferFuncTable:
	.word DoTransfer3dTex
	.word DoTransfer3dTexPltt
	.word DoTransfer3dClearImageColor
	.word DoTransfer3dClearImageDepth
	.word DoTransfer2dBG0CharMain
	.word DoTransfer2dBG1CharMain
	.word DoTransfer2dBG2CharMain
	.word DoTransfer2dBG3CharMain
	.word DoTransfer2dBG0ScrMain
	.word DoTransfer2dBG1ScrMain
	.word DoTransfer2dBG2ScrMain
	.word DoTransfer2dBG3ScrMain
	.word DoTransfer2dBG2BmpMain
	.word DoTransfer2dBG3BmpMain
	.word DoTransfer2dObjPlttMain
	.word DoTransfer2dBGPlttMain
	.word DoTransfer2dObjExtPlttMain
	.word DoTransfer2dBGExtPlttMain
	.word DoTransfer2dObjOamMain
	.word DoTransfer2dObjCharMain
	.word DoTransfer2dBG0CharSub
	.word DoTransfer2dBG1CharSub
	.word DoTransfer2dBG2CharSub
	.word DoTransfer2dBG3CharSub
	.word DoTransfer2dBG0ScrSub
	.word DoTransfer2dBG1ScrSub
	.word DoTransfer2dBG2ScrSub
	.word DoTransfer2dBG3ScrSub
	.word DoTransfer2dBG2BmpSub
	.word DoTransfer2dBG3BmpSub
	.word DoTransfer2dObjPlttSub
	.word DoTransfer2dBGPlttSub
	.word DoTransfer2dObjExtPlttSub
	.word DoTransfer2dBGExtPlttSub
	.word DoTransfer2dObjOamSub
	.word DoTransfer2dObjCharSub
gLnkTexVramSlotTable:
	.byte 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x08, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00
gG2dSoftSpriteCameraUp:
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
gG2dSoftSpriteCameraTarget:
	.byte 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xFF, 0xFF
gObjHeights:
	.byte 0x08, 0x00, 0x10, 0x00, 0x20, 0x00, 0x40, 0x00
	.byte 0x08, 0x00, 0x08, 0x00, 0x10, 0x00, 0x20, 0x00, 0x10, 0x00, 0x20, 0x00, 0x20, 0x00, 0x40, 0x00
gObjWidths:
	.byte 0x08, 0x00, 0x10, 0x00, 0x20, 0x00, 0x40, 0x00, 0x10, 0x00, 0x20, 0x00, 0x20, 0x00, 0x40, 0x00
	.byte 0x08, 0x00, 0x08, 0x00, 0x10, 0x00, 0x20, 0x00
gCellTransferDstTypes:
	.byte 0x00, 0x00, 0x00, 0x00, 0x13, 0x00, 0x00, 0x00
	.byte 0x23, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00
gObjColorModeTexFmts:
	.byte 0x03, 0x00, 0x00, 0x00
	.byte 0x04, 0x00, 0x00, 0x00
gTexSizeInChars:
	.byte 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00
	.byte 0x08, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00
	.byte 0x80, 0x00, 0x00, 0x00
gObjTexSizeS:
	.byte 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00
	.byte 0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00
	.byte 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00
	.byte 0x02, 0x00, 0x00, 0x00
gObjTexSizeT:
	.byte 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00
	.byte 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00
	.byte 0x02, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00
	.byte 0x03, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
gRndCoreIdentityMtx:
	.byte 0x00, 0x10, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
_021093F0:
	.word DrawGlyphLine, ClearLine, ClearAreaLine
gCharCanvasVTableBG:
	.word DrawGlyphLine, ClearContinuous, ClearAreaLine
gCharCanvasVTableObj1D:
	.word DrawGlyph1D, ClearContinuous, ClearArea1D
gCharShiftTable:
	.byte 0x00, 0x00, 0x01, 0x00, 0x02, 0x00, 0x02, 0x00, 0x00, 0x01, 0x01, 0x01
	.byte 0x02, 0x01, 0x02, 0x01, 0x00, 0x02, 0x01, 0x02, 0x02, 0x02, 0x03, 0x02, 0x00, 0x02, 0x01, 0x02
	.byte 0x02, 0x03, 0x03, 0x03
gObjShapeSizeAttrs:
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x40, 0x00, 0x40
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x40, 0x00, 0x80
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x40, 0x00, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80
	.byte 0x00, 0x40, 0x00, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0xC0
	.byte 0x00, 0x00, 0x00, 0xC0
gMatColorMasks:
	.byte 0x00, 0x00, 0x00, 0x00, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x7F
	.byte 0xFF, 0x7F, 0xFF, 0x7F, 0x00, 0x80, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x80, 0xFF, 0x7F
	.byte 0xFF, 0xFF, 0xFF, 0x7F
gPivotMtxIndices:
	.byte 0x04, 0x05, 0x07, 0x08, 0x03, 0x05, 0x06, 0x08, 0x03, 0x04, 0x06, 0x07
	.byte 0x01, 0x02, 0x07, 0x08, 0x00, 0x02, 0x06, 0x08, 0x00, 0x01, 0x06, 0x07, 0x01, 0x02, 0x04, 0x05
	.byte 0x00, 0x02, 0x03, 0x05, 0x00, 0x01, 0x03, 0x04
_021094B8:
	.byte 0x04, 0x05, 0x07, 0x08, 0x03, 0x05, 0x06, 0x08
	.byte 0x03, 0x04, 0x06, 0x07, 0x01, 0x02, 0x07, 0x08, 0x00, 0x02, 0x06, 0x08, 0x00, 0x01, 0x06, 0x07
	.byte 0x01, 0x02, 0x04, 0x05, 0x00, 0x02, 0x03, 0x05, 0x00, 0x01, 0x03, 0x04

	.data

	.public NNS_GfdDefaultFuncAllocTexVram
NNS_GfdDefaultFuncAllocTexVram:
	.word AllocTexVram_
	.public NNS_GfdDefaultFuncFreeTexVram
NNS_GfdDefaultFuncFreeTexVram:
	.word FreeTexVram_
	.public NNS_GfdDefaultFuncAllocPlttVram
NNS_GfdDefaultFuncAllocPlttVram:
	.word AllocPlttVram_
	.public NNS_GfdDefaultFuncFreePlttVram
NNS_GfdDefaultFuncFreePlttVram:
	.word FreePlttVram_
gFrmTexVramSearch4x4:
	.word gFrmTexVramRegions
	.word _02110998
gFrmTexVramSearchNormal:
	.word _021109B0
	.word _02110998
	.word gFrmTexVramRegions
	.word gFrmTexVramRegion2
	.word gFrmTexVramRegion1
gFrmTexVramRegions:
	.byte 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00
gFrmTexVramRegion1:
	.byte 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
	.byte 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x02, 0x00
gFrmTexVramRegion2:
	.byte 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00
	.byte 0x02, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x03, 0x00
_02110998:
	.byte 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x04, 0x00
_021109B0:
	.byte 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x04, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x06, 0x00
gOamLoadFuncs:
	.word CpuLoadOAMMain_
	.word CpuLoadOAMSub_
	.word 0
	.word 0
	.word 0
gRendererContext:
	.byte 0xFE, 0xFF, 0x00, 0x00
	.byte 0xFF, 0xFF, 0xFF, 0xFF
gRendererTranslationMtx:
	.byte 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
gOamSoftEmuAutoZOffsetStep:
	.byte 0x00, 0xF0, 0xFF, 0xFF
gAnmObjInitFuncCount:
	.byte 0x05, 0x00, 0x00, 0x00
_02110A04:
	.word NNSi_G3dAnmCalcNsBva
_02110A08:
	.word NNSi_G3dAnmCalcNsBca
	.public _02110A0C
_02110A0C:
	.word NNSi_G3dAnmCalcNsBta
_02110A10:
	.word NNSi_G3dAnmCalcNsBtp
_02110A14:
	.word NNSi_G3dAnmCalcNsBma
gAnmBlendVisDefault:
	.word NNSi_G3dAnmBlendVis
gAnmBlendJntDefault:
	.word NNSi_G3dAnmBlendJnt
gAnmBlendMatDefault:
	.word NNSi_G3dAnmBlendMat
gAnmObjInitFuncs:
	.byte 0x4D, 0x00, 0x41, 0x4D
gAnmObjInitFuncsFirstFunc:
	.word NNSi_G3dAnmObjInitNsBma
	.byte 0x4D, 0x00, 0x50, 0x54
	.word NNSi_G3dAnmObjInitNsBtp
	.byte 0x4D, 0x00, 0x41, 0x54
	.word NNSi_G3dAnmObjInitNsBta
	.byte 0x56, 0x00, 0x41, 0x56
	.word NNSi_G3dAnmObjInitNsBva
	.byte 0x4A, 0x00, 0x41, 0x43
	.word NNSi_G3dAnmObjInitNsBca
	.byte 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00
gSendJointSRTFuncs:
	.word NNSi_G3dSendJointSRTBasic
	.word NNSi_G3dSendJointSRTMaya
	.word NNSi_G3dSendJointSRTSi3d
gGetJointScaleFuncs:
	.word NNSi_G3dGetJointScaleBasic
	.word NNSi_G3dGetJointScaleMaya
	.word NNSi_G3dGetJointScaleSi3d
gSendTexSRTFuncs:
	.word NNSi_G3dSendTexSRTMaya
	.word NNSi_G3dSendTexSRTSi3d
	.word NNSi_G3dSendTexSRT3dsMax
	.word NNSi_G3dSendTexSRTXsi
gTexGenTexImageCmds:
	.byte 0x2A, 0x00, 0x00, 0x00
gPrjMapTexImageParam:
	.byte 0x00, 0x00, 0x00, 0x00, 0x2A, 0x00, 0x00, 0x00
gEnvMapTexImageParam:
	.byte 0x00, 0x00, 0x00, 0x00
gSbcMatFuncs:
	.word NNSi_G3dFuncSbc_MAT_InternalDefault
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
gSbcShpFuncs:
	.word NNSi_G3dFuncSbc_SHP_InternalDefault
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
gPrjMapTexMtx:
	.byte 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00
gBillboardCmd:
	.byte 0x12, 0x10, 0x17, 0x1B
gBillboardCmdParams:
	.byte 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00
gBillboardMtx:
	.byte 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00
gBillboardTrans:
	.byte 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
gBillboardScale:
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x00, 0x00, 0x00
gBillboardYCmd:
	.byte 0x12, 0x10, 0x17, 0x1B
gBillboardYCmdParams:
	.byte 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00
gBillboardYMtx:
	.byte 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0x00, 0x10, 0x00, 0x00
gBillboardYTrans:
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
gBillboardYScale:
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
gSbcFuncs:
	.word NNSi_G3dFuncSbc_NOP
	.word NNSi_G3dFuncSbc_RET
	.word NNSi_G3dFuncSbc_NODE
	.word NNSi_G3dFuncSbc_MTX
	.word NNSi_G3dFuncSbc_MAT
	.word NNSi_G3dFuncSbc_SHP
	.word NNSi_G3dFuncSbc_NODEDESC
	.word NNSi_G3dFuncSbc_BB
	.word NNSi_G3dFuncSbc_BBY
	.word NNSi_G3dFuncSbc_NODEMIX
	.word NNSi_G3dFuncSbc_CALLDL
	.word NNSi_G3dFuncSbc_POSSCALE
	.word NNSi_G3dFuncSbc_ENVMAP
	.word NNSi_G3dFuncSbc_PRJMAP
	.word 0x00000000
	.word 0x00000000
	.word 0x00000000
	.word 0x00000000
	.word 0x00000000
	.word 0x00000000
	.word 0x00000000
	.word 0x00000000
	.word 0x00000000
	.word 0x00000000
	.word 0x00000000
	.word 0x00000000
	.word 0x00000000
	.word 0x00000000
	.word 0x00000000
	.word 0x00000000
	.word 0x00000000
	.word 0x00000000
_02110C1C:
	.word texmtxCalc_flag_
	.word texmtxCalc_flagS_
	.word texmtxCalc_flagR_
	.word texmtxCalc_flagRS_
	.word texmtxCalc_flagT_
	.word texmtxCalc_flagTS_
	.word texmtxCalc_flagTR_
	.word texmtxCalc_flagTRS_
_02110C3C:
	.word texmtxCalc_flag___3dsmax
	.word texmtxCalc_flagS___3dsmax
	.word texmtxCalc_flagR___3dsmax
	.word texmtxCalc_flagRS___3dsmax
	.word texmtxCalc_flagT___3dsmax
	.word texmtxCalc_flagTS___3dsmax
	.word texmtxCalc_flagTR___3dsmax
	.word texmtxCalc_flagTRS___3dsmax
_02110C5C:
	.word texmtxCalc_flag___xsi
	.word texmtxCalc_flagS___xsi
	.word texmtxCalc_flagR___xsi
	.word texmtxCalc_flagRS___xsi
	.word texmtxCalc_flagT___xsi
	.word texmtxCalc_flagTS___xsi
	.word texmtxCalc_flagTR___xsi
	.word texmtxCalc_flagTRS___xsi
