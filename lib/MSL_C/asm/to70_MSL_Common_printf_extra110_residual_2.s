	.include "asm/macros.inc"
	.include "MSL_Common_printf.inc"
	.include "global.inc"
	.public printf
	.extern __FileWrite
	.extern __pformatter

.text
	arm_func_start printf
printf: ; 0x020E7D70
	stmdb sp!, {r0, r1, r2, r3}
	stmdb sp!, {r4, lr}
	ldr r0, _020E7E70 ; =__files + 0x4C
	mvn r1, #0
	bl fwide
	cmp r0, #0
	mvnge r0, #0
	ldmgeia sp!, {r4, lr}
	addge sp, sp, #0x10
	bxge lr
	ldr r0, _020E7E74 ; =__cs + 0x48
	bl OS_TryLockMutex
	cmp r0, #0
	bne _020E7DCC
	ldr r0, _020E7E78 ; =OSi_ThreadInfo
	ldr r1, _020E7E7C ; =__cs_id
	ldr r2, [r0, #4]
	ldr r0, _020E7E80 ; =__cs_ref
	ldr r3, [r2, #0x6c]
	mov r2, #1
	str r3, [r1, #0xc]
	str r2, [r0, #0xc]
	b _020E7E24
_020E7DCC:
	ldr r0, _020E7E78 ; =OSi_ThreadInfo
	ldr r1, _020E7E7C ; =__cs_id
	ldr r0, [r0, #4]
	ldr r1, [r1, #0xc]
	ldr r0, [r0, #0x6c]
	cmp r1, r0
	bne _020E7DFC
	ldr r0, _020E7E80 ; =__cs_ref
	ldr r1, [r0, #0xc]
	add r1, r1, #1
	str r1, [r0, #0xc]
	b _020E7E24
_020E7DFC:
	ldr r0, _020E7E74 ; =__cs + 0x48
	bl OS_LockMutex
	ldr r0, _020E7E78 ; =OSi_ThreadInfo
	ldr r1, _020E7E7C ; =__cs_id
	ldr r2, [r0, #4]
	ldr r0, _020E7E80 ; =__cs_ref
	ldr r3, [r2, #0x6c]
	mov r2, #1
	str r3, [r1, #0xc]
	str r2, [r0, #0xc]
_020E7E24:
	add r0, sp, #8
	bic r3, r0, #3
	ldr r2, [sp, #8]
	ldr r0, _020E7E84 ; =__FileWrite
	ldr r1, _020E7E70 ; =__files + 0x4C
	add r3, r3, #4
	bl __pformatter
	ldr r1, _020E7E80 ; =__cs_ref
	mov r4, r0
	ldr r0, [r1, #0xc]
	subs r0, r0, #1
	str r0, [r1, #0xc]
	bne _020E7E60
	ldr r0, _020E7E74 ; =__cs + 0x48
	bl OS_UnlockMutex
_020E7E60:
	mov r0, r4
	ldmia sp!, {r4, lr}
	add sp, sp, #0x10
	bx lr
	.align 2, 0
_020E7E70: .word __files + 0x4C
_020E7E74: .word __cs + 0x48
_020E7E78: .word OSi_ThreadInfo
_020E7E7C: .word __cs_id
_020E7E80: .word __cs_ref
_020E7E84: .word __FileWrite
	arm_func_end printf
