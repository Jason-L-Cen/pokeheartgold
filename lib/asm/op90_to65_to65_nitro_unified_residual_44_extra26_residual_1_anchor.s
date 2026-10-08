	.extern AgbFlash
	.extern CTRDGi_PollingSR
	.extern CTRDGi_SetFlashBankMx
	.extern OS_LockCartridge
	.extern OS_UnlockCartridge
	.extern _021E4F68
	.extern _021E4F6C
	.extern _021E4F90
	.extern ctrdg_flash_remainder
	.extern ctrdgi_backup_irq
	.extern ctrdgi_flash_lock_id
	.include "asm/macros.inc"
	.include "nitro.inc"
	.include "global.inc"
	.public CTRDGi_EraseFlashChipCoreLE
	.public CTRDGi_EraseFlashSectorCoreLE
.text
