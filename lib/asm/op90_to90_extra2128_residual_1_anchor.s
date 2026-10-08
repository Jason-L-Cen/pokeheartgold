	.extern CARDi_OnReadCard
	.extern CARDi_ReadFromCache
	.extern CARDi_SetCardDma
	.extern CARDi_SetRomOp
	.extern DC_FlushAll
	.extern DC_InvalidateRange
	.extern DC_StoreRange
	.extern IC_InvalidateAll
	.extern OS_GetDTCMAddress
	.extern cardi_common
	.extern cardi_rom_header_addr
	.include "asm/macros.inc"
	.include "nitro.inc"
	.include "global.inc"

.text
