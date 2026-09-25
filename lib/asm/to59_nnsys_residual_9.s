	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"
	.extern ResetTaskQueue_
	.extern _021D84D0

.text
	arm_func_start NNS_GfdInitVramTransferManager
NNS_GfdInitVramTransferManager: ; 0x020B62C8
	ldr r2, _020B62E0 ; =_021D84D0
	ldr ip, _020B62E4 ; =ResetTaskQueue_
	str r0, [r2]
	ldr r0, _020B62E8 ; =_021D84D0
	str r1, [r2, #4]
	bx ip
	.align 2, 0
_020B62E0: .word _021D84D0
_020B62E4: .word ResetTaskQueue_
_020B62E8: .word _021D84D0
	arm_func_end NNS_GfdInitVramTransferManager
