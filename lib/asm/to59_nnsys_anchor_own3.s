	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"
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
	.public _021DA700
	.public _021DA704
	.public _021DB504
	.public _021DB508
	.public _021DB50C
	.public _021DB510
	.public _021DB514
	.public _021DB518
	.public _021DBB04
	.public _021DBB44
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
	.bss
_021DA700:
	.space 0x4

	.public _021DA704
_021DA704:
	.space 0xE00

	.public _021DB504
_021DB504:
	.space 0x4

	.public _021DB508
_021DB508:
	.space 0x4

	.public _021DB50C
_021DB50C:
	.space 0x4

	.public _021DB510
_021DB510:
	.space 0x4

	.public _021DB514
_021DB514:
	.space 0x4

	.public _021DB518
_021DB518:
	.space 0x5EC

	.public _021DBB04
_021DBB04:
	.space 0x40

	.public _021DBB44
_021DBB44:
	.space 0x18C0

	.public _021DD404
_021DD404:
	.space 0x4

	.public _021DD408
_021DD408:
	.space 0x18

	.public _021DD420
_021DD420:
	.space 0x10

	.public _021DD430
_021DD430:
	.space 0xC

	.public _021DD43C
_021DD43C:
	.space 0x24

	.public _021DD460
_021DD460:
	.space 0x23C0

	.public _021DF820
_021DF820:
	.space 0xC

	.public _021DF82C
_021DF82C:
	.space 0x140

	.public _021DF96C
_021DF96C:
	.space 0xC

	.public _021DF978
_021DF978:
	.space 0xC

	.public _021DF984
_021DF984:
	.space 0x440

	.public _021DFDC4
_021DFDC4:
	.space 0x8

	.public _021DFDCC
_021DFDCC:
	.space 0x10

	.public _021DFDDC
_021DFDDC:
	.space 0x4

	.public _021DFDE0
_021DFDE0:
	.space 0x4

	.public _021DFDE4
_021DFDE4:
	.space 0x460

	.public _021E0244
_021E0244:
	.space 0x4

	.public _021E0248
_021E0248:
	.space 0xC

	.public _021E0254
_021E0254:
	.space 0x40

	.public _021E0294
_021E0294:
	.space 0x80

	.public _021E0314
_021E0314:
	.space 0x8

	.public _021E031C
_021E031C:
	.space 0x40

	.public _021E035C
_021E035C:
	.space 0x5B4

	.public _021E0910
_021E0910:
	.space 0x4

	.public _021E0914
