	.extern _021105F4
	.include "asm/macros.inc"
	.include "dwcaccount.inc"
	.include "global.inc"

.text
	arm_func_start DWCi_Acc_U64ToString32
DWCi_Acc_U64ToString32: ; 0x0209FDB8
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	ldr lr, _0209FE20 ; =0x66666667
	add r4, r2, #4
	smull ip, r2, lr, r4
	mov ip, r4, lsr #0x1f
	add r2, ip, r2, asr #1
	cmp r2, #0
	ldr r7, _0209FE24 ; =_021105F4
	mov r6, #0
	ble _0209FE14
	add r4, r3, r2
	sub r4, r4, #1
	mov ip, #0x1f
_0209FDEC:
	and r5, r0, ip
	ldrsb r5, [r7, r5]
	mov r0, r0, lsr #5
	mov lr, r1, lsr #5
	strb r5, [r4, -r6]
	add r6, r6, #1
	orr r0, r0, r1, lsl #27
	mov r1, lr
	cmp r6, r2
	blt _0209FDEC
_0209FE14:
	mov r0, #0
	strb r0, [r3, r2]
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.align 2, 0
_0209FE20: .word 0x66666667
_0209FE24: .word _021105F4
	arm_func_end DWCi_Acc_U64ToString32
