	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"
	.public SetMCDataToMCInstanceImpl_
	.extern NNS_G2dGetAnimSequenceByIdx
	.extern NNS_G2dSetCellAnimationSequence
	.extern NNS_G2dTickCellAnimation
	.extern NNSi_G2dIsAnimCtrlLoopAnim
	.extern NNSi_G2dSrtcSetTrans

.text
