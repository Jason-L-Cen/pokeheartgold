	.include "asm/macros.inc"
	.include "dwcbm.inc"
	.include "global.inc"
	.public DWCi_BM_GetWiFiInfo
	.extern DWCi_BACKUPlConvWifiInfo
	.extern Wifi
	.extern _021D43C8
	.extern readNvram
	.extern verify
	.extern writeDisable
	.extern writeNvram

.text
	arm_func_start DWCi_BM_GetWiFiInfo
DWCi_BM_GetWiFiInfo: ; 0x0209E430
	stmdb sp!, {r4, lr}
	mov r4, r0
	ldr r0, _0209E4F0 ; =Wifi
	mov r1, r4
	mov r2, #6
	bl MI_CpuCopy8
	ldr r1, _0209E4F4 ; =0x000007FF
	ldr r3, [r4]
	sub r0, r1, #0x800
	ldr r2, [r4, #4]
	and r0, r3, r0
	str r0, [r4]
	and r1, r2, r1
	str r1, [r4, #4]
	ldr r0, _0209E4F8 ; =Wifi + 5
	add r1, r4, #8
	mov r2, #6
	bl MI_CpuCopy8
	ldr r1, [r4, #8]
	ldr r0, [r4, #0xc]
	mov r1, r1, lsr #3
	orr r1, r1, r0, lsl #29
	str r1, [r4, #8]
	mov r3, r0, lsr #3
	str r3, [r4, #0xc]
	ldr r1, _0209E4F4 ; =0x000007FF
	ldr r2, [r4, #8]
	sub r0, r1, #0x800
	and r0, r2, r0
	str r0, [r4, #8]
	and r1, r1, r3
	str r1, [r4, #0xc]
	ldr r0, _0209E4FC ; =Wifi + 10
	add r1, r4, #0x10
	mov r2, #2
	bl MI_CpuCopy8
	ldrh r1, [r4, #0x10]
	ldr r3, _0209E500 ; =0x000003FF
	ldr r0, _0209E504 ; =Wifi + 12
	mov r1, r1, asr #6
	strh r1, [r4, #0x10]
	ldrh ip, [r4, #0x10]
	add r1, r4, #0x12
	mov r2, #2
	and r3, ip, r3
	strh r3, [r4, #0x10]
	bl MI_CpuCopy8
	ldmia sp!, {r4, pc}
	.align 2, 0
_0209E4F0: .word Wifi
_0209E4F4: .word 0x000007FF
_0209E4F8: .word Wifi + 5
_0209E4FC: .word Wifi + 10
_0209E500: .word 0x000003FF
_0209E504: .word Wifi + 12
	arm_func_end DWCi_BM_GetWiFiInfo

	arm_func_start DWCi_BM_SetWiFiInfo
DWCi_BM_SetWiFiInfo: ; 0x0209E508
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	ldr r2, _0209E5D8 ; =_021D43C8
	mov sl, r1
	ldr sb, [r2, #8]
	bl DWCi_BACKUPlConvWifiInfo
	ldr r1, _0209E5DC ; =0x0000A001
	add r0, sl, #0x200
	bl MATHi_CRC16InitTableRev
	mov r5, #0x100
	ldr r6, _0209E5E0 ; =Wifi
	mov r8, #0
	mov r4, r5
	mov fp, #0xe
	mov r7, r5
_0209E540:
	mov r0, sb
	mov r1, r7
	mov r2, sl
	bl readNvram
	cmp r0, #0
	bne _0209E564
	bl OS_Terminate
	mov r0, #0
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_0209E564:
	mov r0, r6
	mov r2, fp
	add r1, sl, #0xf0
	bl MI_CpuCopy8
	mov r1, sl
	mov r2, #0xfe
	add r0, sl, #0x200
	bl MATH_CalcCRC16
	strh r0, [sl, #0xfe]
_0209E588:
	mov r0, sb
	mov r1, r5
	mov r2, sl
	bl writeNvram
	mov r0, sl
	mov r1, sb
	mov r2, r4
	add r3, sl, #0x100
	bl verify
	cmp r0, #0
	beq _0209E588
	add r8, r8, #1
	cmp r8, #2
	add sb, sb, #0x100
	blt _0209E540
	bl writeDisable
	cmp r0, #0
	movne r0, #1
	moveq r0, #0
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_0209E5D8: .word _021D43C8
_0209E5DC: .word 0x0000A001
_0209E5E0: .word Wifi
	arm_func_end DWCi_BM_SetWiFiInfo
