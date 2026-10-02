	.extern DWC_BACKUPlCheckAddress
	.extern DWC_BACKUPlCheckIp
	.extern DWC_BACKUPlCheckSsid
	.extern DWCi_AUTH_GetNewWiFiInfo
	.extern DWCi_BACKUPlConvMaskAddr
	.extern DWCi_BACKUPlInit
	.extern DWCi_BACKUPlRead
	.extern DWCi_BACKUPlSetWiFi
	.extern DWCi_BACKUPlWriteAll
	.extern DWCi_SETTING_NONE
	.extern Wifi
	.extern Work
	.extern _021D43C8
	.extern initPage
	.include "asm/macros.inc"
	.include "dwcbm.inc"
	.include "global.inc"
	.public DWCi_BACKUPlConvWifiInfo

.text
	arm_func_start DWCi_BACKUPlConvWifiInfo
DWCi_BACKUPlConvWifiInfo: ; 0x0209EF2C
	stmdb sp!, {r4, lr}
	sub sp, sp, #8
	mov r4, r0
	ldr r3, [r4, #8]
	ldr r2, [r4, #0xc]
	ldr r1, _0209EFE0 ; =Wifi
	str r2, [sp, #4]
	str r3, [sp]
	mov r2, #5
	bl MI_CpuCopy8
	ldr r2, [sp]
	ldr r1, [sp, #4]
	mov ip, r2, lsr #5
	ldr lr, [r4, #4]
	mov r3, r1, lsr #5
	orr ip, ip, r1, lsl #27
	mov r0, #7
	and r1, r0, lr, lsr #8
	and r0, r2, #0x1f
	orr lr, r1, r0, lsl #3
	ldr r2, _0209EFE0 ; =Wifi
	ldr r1, _0209EFE4 ; =Wifi + 6
	strb lr, [r2, #5]
	add r0, sp, #0
	str ip, [sp]
	str r3, [sp, #4]
	mov r2, #4
	bl MI_CpuCopy8
	ldrh r0, [r4, #0x10]
	ldr r1, [sp, #4]
	ldr r3, _0209EFE0 ; =Wifi
	and r1, r1, #0x3f
	mov r0, r0, lsl #0x1e
	orr r0, r1, r0, lsr #24
	strb r0, [r3, #0xa]
	ldrh r2, [r4, #0x10]
	add r0, r4, #0x12
	ldr r1, _0209EFE8 ; =Wifi + 12
	mov r4, r2, asr #2
	mov r2, #2
	strb r4, [r3, #0xb]
	bl MI_CpuCopy8
	ldr r0, _0209EFE0 ; =Wifi
	add sp, sp, #8
	ldmia sp!, {r4, pc}
	.align 2, 0
_0209EFE0: .word Wifi
_0209EFE4: .word Wifi + 6
_0209EFE8: .word Wifi + 12
	arm_func_end DWCi_BACKUPlConvWifiInfo
