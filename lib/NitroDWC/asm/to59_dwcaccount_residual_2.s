	.include "asm/macros.inc"
	.include "dwcaccount.inc"
	.include "global.inc"
	.extern _021105F4

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

	arm_func_start DWC_Acc_FriendKeyToGsProfileId
DWC_Acc_FriendKeyToGsProfileId: ; 0x0209FD98
	stmdb sp!, {r4, lr}
	mov r4, r0
	bl DWC_Acc_CheckFriendKey
	cmp r0, #0
	mov r0, #0
	subne r0, r0, #1
	andne r0, r4, r0
	ldmia sp!, {r4, pc}
	arm_func_end DWC_Acc_FriendKeyToGsProfileId

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
