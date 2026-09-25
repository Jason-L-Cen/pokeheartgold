	.include "asm/macros.inc"
	.include "dwcbm.inc"
	.include "global.inc"
	.extern DWCi_BM_GetWiFiInfo

.text
	arm_func_start DWC_Auth_CheckPseudoWiFiID
DWC_Auth_CheckPseudoWiFiID: ; 0x0209FA94
	stmdb sp!, {lr}
	sub sp, sp, #0x14
	add r0, sp, #0
	bl DWCi_BM_GetWiFiInfo
	ldr r0, [sp, #0xc]
	ldr r1, [sp, #8]
	cmp r0, #0
	cmpeq r1, #0
	mov r0, #0
	bne _0209FAD8
	ldr r1, [sp, #4]
	ldr r2, [sp]
	cmp r1, r0
	cmpeq r2, r0
	addeq sp, sp, #0x14
	moveq r0, #1
	ldmeqia sp!, {pc}
_0209FAD8:
	mov r0, #0
	add sp, sp, #0x14
	ldmia sp!, {pc}
	arm_func_end DWC_Auth_CheckPseudoWiFiID
