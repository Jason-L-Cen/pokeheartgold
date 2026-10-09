	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"
	.public LoadWaveArcTable
	.extern NNS_SndArcGetCurrent
	.extern NNS_SndArcGetFileAddress
	.extern NNS_SndArcReadFile
	.extern NNS_SndArcSetFileAddress
	.extern NNS_SndHeapAlloc
	.extern WaveArcTableDisposeCallback
	.extern _021E0914

.text
