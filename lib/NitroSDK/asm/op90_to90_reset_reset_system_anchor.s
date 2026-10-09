	.include "asm/macros.inc"
	.public OS_ResetSystem
	.extern CARD_LockRom
	.extern MI_StopDma
	.extern OS_GetLockID
	.extern OS_ResetRequestIrqMask
	.extern OS_SetIrqMask
	.extern OS_Terminate
	.extern OSi_DoResetSystem
	.extern OSi_SendToPxi
	.extern SDK_AUTOLOAD_DTCM_START
	.extern SDK_IRQ_STACKSIZE

.text
