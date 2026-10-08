	.extern _start_AutoloadDoneCallback
	.include "asm/macros.inc"
	.include "mb.inc"
	.include "global.inc"
	.public MBi_SendFixedBeacon
	.extern MBi_calc_cksum
	.extern bsendBuff
	.extern ov13_022474A0

.text
