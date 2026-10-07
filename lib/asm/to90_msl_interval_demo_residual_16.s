	.include "asm/macros.inc"
	.include "msl.inc"
	.include "global.inc"
	.public _Z16HandleUnexpectedP12ThrowContextP13ExceptionInfoP16ex_specificationPc
	.extern _Z11UnwindStackP12ThrowContextP13ExceptionInfoPc
	.extern _Z17__TransferControlP12ThrowContextP13ExceptionInfoPc
	.type _Z16HandleUnexpectedP12ThrowContextP13ExceptionInfoP16ex_specificationPc, @function

.text

	arm_func_start _Z16HandleUnexpectedP12ThrowContextP13ExceptionInfoP16ex_specificationPc
_Z16HandleUnexpectedP12ThrowContextP13ExceptionInfoP16ex_specificationPc: ; 0x020F4508
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	mov r4, r3
	mov r5, r2
	mov r2, r4
	mov r7, r0
	mov r6, r1
	bl _Z11UnwindStackP12ThrowContextP13ExceptionInfoPc
	ldr r2, [r7, #0x18]
	ldr r1, [r5, #8]
	ldr r0, [r7, #4]
	add r3, r2, r1
	str r0, [r2, r1]
	ldr r1, [r7]
	mov r0, r7
	str r1, [r3, #4]
	ldr r2, [r7, #8]
	mov r1, r6
	str r2, [r3, #8]
	str r4, [r3, #0x14]
	ldr r3, [r6]
	ldr r2, [r5, #4]
	add r2, r3, r2
	bl _Z17__TransferControlP12ThrowContextP13ExceptionInfoPc
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	arm_func_end _Z16HandleUnexpectedP12ThrowContextP13ExceptionInfoP16ex_specificationPc
