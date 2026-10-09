	.include "asm/macros.inc"
	.include "msl.inc"
	.include "global.inc"
	.extern _Z14__ThrowHandlerP12ThrowContext
	.extern sys_exit

.text
	arm_func_start _Z17__TransferControlP12ThrowContextP13ExceptionInfoPc
_Z17__TransferControlP12ThrowContextP13ExceptionInfoPc: ; 0x020F4EB4
	ldr r4, [r0, #0x2c]
	ldr r5, [r0, #0x30]
	ldr r6, [r0, #0x34]
	ldr r7, [r0, #0x38]
	ldr r8, [r0, #0x3c]
	ldr sb, [r0, #0x40]
	ldr sl, [r0, #0x44]
	ldr fp, [r0, #0x48]
	ldr sp, [r0, #0x5c]
	ldr ip, [r0, #0x64]
	sub sp, sp, ip
	bx r2
	arm_func_end _Z17__TransferControlP12ThrowContextP13ExceptionInfoPc
