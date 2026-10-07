	.include "asm/macros.inc"
	.include "msl.inc"
	.include "global.inc"
	.public _ExitProcess
	.public _Z17__TransferControlP12ThrowContextP13ExceptionInfoPc
	.public __rethrow
	.public __throw
	.extern _Z14__ThrowHandlerP12ThrowContext
	.extern sys_exit
	.type _Z17__TransferControlP12ThrowContextP13ExceptionInfoPc, @function
	.type __throw, @function
	.type __rethrow, @function
	.type _ExitProcess, @function

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

	arm_func_start __throw
__throw: ; 0x020F4EE4
	mov ip, sp
	sub sp, sp, #0x70
	str r4, [sp, #0x2c]
	str r5, [sp, #0x30]
	str r6, [sp, #0x34]
	str r7, [sp, #0x38]
	str r8, [sp, #0x3c]
	str sb, [sp, #0x40]
	str sl, [sp, #0x44]
	str fp, [sp, #0x48]
	str ip, [sp, #0x14]
	str ip, [sp, #0x5c]
	str lr, [sp, #0x10]
	str r0, [sp]
	str r1, [sp, #4]
	str r2, [sp, #8]
	mov r0, sp
	b _Z14__ThrowHandlerP12ThrowContext
	arm_func_end __throw

	arm_func_start __rethrow
__rethrow: ; 0x020F4F2C
	mov ip, sp
	sub sp, sp, #0x70
	str r4, [sp, #0x2c]
	str r5, [sp, #0x30]
	str r6, [sp, #0x34]
	str r7, [sp, #0x38]
	str r8, [sp, #0x3c]
	str sb, [sp, #0x40]
	str sl, [sp, #0x44]
	str fp, [sp, #0x48]
	str ip, [sp, #0x14]
	str ip, [sp, #0x5c]
	str lr, [sp, #0x10]
	mov ip, #0
	str ip, [sp]
	str ip, [sp, #4]
	str ip, [sp, #8]
	mov r0, sp
	b _Z14__ThrowHandlerP12ThrowContext
	arm_func_end __rethrow
