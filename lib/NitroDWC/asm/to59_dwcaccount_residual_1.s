	.include "asm/macros.inc"
	.include "dwcaccount.inc"
	.include "global.inc"
	.extern DWCi_Acc_GetFlag_DataType
	.extern DWCi_Acc_GetFlags
	.extern DWCi_Acc_SetFlags

.text
	arm_func_start DWCi_SetBuddyFriendData
DWCi_SetBuddyFriendData: ; 0x0209FC9C
	stmdb sp!, {r4, lr}
	mov r4, r0
	bl DWCi_Acc_GetFlag_DataType
	cmp r0, #3
	ldmneia sp!, {r4, pc}
	mov r0, r4
	bl DWCi_Acc_GetFlags
	orr r1, r0, #4
	mov r0, r4
	bl DWCi_Acc_SetFlags
	ldmia sp!, {r4, pc}
	arm_func_end DWCi_SetBuddyFriendData
