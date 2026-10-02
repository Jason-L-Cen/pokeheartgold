	.extern _021105F4
	.include "asm/macros.inc"
	.include "dwcaccount.inc"
	.include "global.inc"
	.public DWC_Acc_CheckFriendKey

.text
	arm_func_start DWC_Acc_CheckFriendKey
DWC_Acc_CheckFriendKey: ; 0x0209FD28
	stmdb sp!, {r4, lr}
	sub sp, sp, #0x108
	mov r4, r1
	and r1, r4, #0
	cmp r1, #0
	and r3, r0, #0x80000000
	cmpeq r3, #0
	mov r1, #0
	addne sp, sp, #0x108
	movne r0, r1
	ldmneia sp!, {r4, pc}
	str r0, [sp]
	add r0, sp, #8
	mov r1, #7
	str r2, [sp, #4]
	bl MATHi_CRC8InitTable
	add r0, sp, #8
	add r1, sp, #0
	mov r2, #8
	bl MATH_CalcCRC8
	and r1, r0, #0x7f
	mov r0, r1, asr #0x1f
	cmp r0, #0
	cmpeq r1, r4
	moveq r0, #1
	movne r0, #0
	add sp, sp, #0x108
	ldmia sp!, {r4, pc}
	arm_func_end DWC_Acc_CheckFriendKey
