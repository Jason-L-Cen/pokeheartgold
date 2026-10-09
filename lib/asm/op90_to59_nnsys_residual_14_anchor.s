	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"
	.public NNS_GfdResetLnkPlttVramState
	.extern NNSi_GfdAddNewFreeBlock
	.extern NNSi_GfdInitLnkVramBlockPool
	.extern NNSi_GfdInitLnkVramMan
	.extern NNSi_GfdMergeAllFreeBlocks
	.extern _021D8500
	.extern _021D8504

.text
