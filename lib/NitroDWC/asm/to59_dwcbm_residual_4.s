	.include "asm/macros.inc"
	.include "dwcbm.inc"
	.include "global.inc"
	.public DWCi_BACKUPlConvWifiInfo
	.public readNvram
	.public verify
	.public writeDisable
	.public writeNvram
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

.text
	arm_func_start NVRAMm_ExecuteCommand
NVRAMm_ExecuteCommand: ; 0x0209E8CC
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x3c
	ldr sl, _0209ED20 ; =Work
	mov r8, r1, lsr #0x10
	mov r4, sl, lsl #8
	mov sb, r4
	and r4, r4, #0xff00
	mov r1, r1, lsl #0x10
	orr r4, r4, #0x20000
	mov r5, r3, lsr #0x10
	mov r7, r0
	ldr ip, _0209ED24 ; =0x01020000
	mov r1, r1, lsr #0x10
	str r4, [sp, #0x30]
	orr r4, r1, #0x10000
	mov r3, r3, lsl #0x10
	add r1, ip, #0x30000
	orr r1, r1, r3, lsr #16
	str r1, [sp, #0x10]
	mov r1, r3, lsr #0x10
	orr r1, r1, #0x1040000
	ldr r0, _0209ED28 ; =0x02002200
	str r1, [sp, #0x24]
	orr r1, r0, sl, lsr #24
	str r1, [sp, #0x28]
	ldr r1, [sp, #0x30]
	mov sb, sb, lsr #0x10
	orr r1, r1, #0x1000000
	str r1, [sp, #0x30]
	add r1, r0, #0x300
	and r8, r8, #0xff
	add r0, r0, #0x100
	orr r0, r8, r0
	orr r1, r8, r1
	str r0, [sp]
	orr r0, r2, #0x20000
	str r0, [sp, #0x1c]
	orr r0, r5, #0x30000
	str r0, [sp, #0x20]
	orr r0, r5, #0x40000
	str r0, [sp, #0xc]
	ldr r0, _0209ED2C ; =0x03002100
	mov r6, #0
	sub r0, r0, #0x100
	str r0, [sp, #0x14]
	mov r0, r2, lsr #0x10
	orr r0, r0, #0x20000
	str r0, [sp, #4]
	mov r0, r2, lsl #0x10
	mov r0, r0, lsr #0x10
	orr sb, sb, #0x10000
	orr r0, r0, #0x30000
	str sb, [sp, #0x2c]
	str r1, [sp, #0x18]
	str r0, [sp, #8]
	mov r5, r6
	mov r8, #0xfa0
_0209E9B0:
	cmp r6, #0
	bne _0209EBF0
	ldr r0, _0209ED30 ; =_021D43C8
	cmp r7, #7
	str r5, [r0, #4]
	addls pc, pc, r7, lsl #2
	b _0209E9B0
_0209E9CC: ; jump table
	b _0209E9B0 ; case 0
	b _0209E9EC ; case 1
	b _0209EA98 ; case 2
	b _0209EAB8 ; case 3
	b _0209EB54 ; case 4
	b _0209EB54 ; case 5
	b _0209EBAC ; case 6
	b _0209EBD0 ; case 7
_0209E9EC:
	ldr r1, [sp]
	mov r0, #4
	mov r2, r5
	bl PXI_SendWordByFifo
	cmp r0, #0
	movlt r6, #0
	blt _0209E9B0
	mov r0, #4
	mov r1, r4
	mov r2, #0
	bl PXI_SendWordByFifo
	cmp r0, #0
	movlt r6, #0
	blt _0209E9B0
	ldr r1, [sp, #4]
	mov r0, #4
	mov r2, #0
	bl PXI_SendWordByFifo
	cmp r0, #0
	movlt r6, #0
	blt _0209E9B0
	ldr r1, [sp, #8]
	mov r0, #4
	mov r2, #0
	bl PXI_SendWordByFifo
	cmp r0, #0
	movlt r6, #0
	blt _0209E9B0
	ldr r1, [sp, #0xc]
	mov r0, #4
	mov r2, #0
	bl PXI_SendWordByFifo
	cmp r0, #0
	movlt r6, #0
	blt _0209E9B0
	ldr r1, [sp, #0x10]
	mov r0, #4
	mov r2, #0
	bl PXI_SendWordByFifo
	cmp r0, #0
	movlt r6, #0
	movge r6, #1
	b _0209E9B0
_0209EA98:
	ldr r1, [sp, #0x14]
	mov r0, #4
	mov r2, r5
	bl PXI_SendWordByFifo
	cmp r0, #0
	movlt r6, #0
	movge r6, #1
	b _0209E9B0
_0209EAB8:
	ldr r1, [sp, #0x18]
	mov r0, #4
	mov r2, r5
	bl PXI_SendWordByFifo
	cmp r0, #0
	movlt r6, #0
	blt _0209EB44
	mov r0, #4
	mov r1, r4
	mov r2, #0
	bl PXI_SendWordByFifo
	cmp r0, #0
	movlt r6, #0
	blt _0209EB44
	ldr r1, [sp, #0x1c]
	mov r0, #4
	mov r2, #0
	bl PXI_SendWordByFifo
	cmp r0, #0
	movlt r6, #0
	blt _0209EB44
	ldr r1, [sp, #0x20]
	mov r0, #4
	mov r2, #0
	bl PXI_SendWordByFifo
	cmp r0, #0
	movlt r6, #0
	blt _0209EB44
	ldr r1, [sp, #0x24]
	mov r0, #4
	mov r2, #0
	bl PXI_SendWordByFifo
	cmp r0, #0
	movlt r6, #0
	movge r6, #1
_0209EB44:
	bl OS_GetTick
	str r0, [sp, #0x34]
	mov fp, r1
	b _0209E9B0
_0209EB54:
	ldr r1, [sp, #0x28]
	mov r0, #4
	mov r2, r5
	bl PXI_SendWordByFifo
	cmp r0, #0
	movlt r6, #0
	blt _0209E9B0
	ldr r1, [sp, #0x2c]
	mov r0, #4
	mov r2, #0
	bl PXI_SendWordByFifo
	cmp r0, #0
	movlt r6, #0
	blt _0209E9B0
	ldr r1, [sp, #0x30]
	mov r0, #4
	mov r2, #0
	bl PXI_SendWordByFifo
	cmp r0, #0
	movlt r6, #0
	movge r6, #1
	b _0209E9B0
_0209EBAC:
	ldr r1, _0209ED2C ; =0x03002100
	mov r0, #4
	add r1, r1, #0xc00
	mov r2, r5
	bl PXI_SendWordByFifo
	cmp r0, #0
	movlt r6, #0
	movge r6, #1
	b _0209E9B0
_0209EBD0:
	ldr r1, _0209ED2C ; =0x03002100
	mov r0, #4
	mov r2, r5
	bl PXI_SendWordByFifo
	cmp r0, #0
	movlt r6, #0
	movge r6, #1
	b _0209E9B0
_0209EBF0:
	ldr r0, _0209ED30 ; =_021D43C8
	ldr r0, [r0, #4]
	cmp r0, #1
	bne _0209E9B0
	ldr r0, _0209ED30 ; =_021D43C8
	mov r6, #0
	ldrh r0, [r0]
	cmp r0, #0
	bne _0209ED14
	cmp r7, #7
	addls pc, pc, r7, lsl #2
	b _0209E9B0
_0209EC20: ; jump table
	b _0209E9B0 ; case 0
	b _0209EC40 ; case 1
	b _0209EC4C ; case 2
	b _0209EC54 ; case 3
	b _0209EC5C ; case 4
	b _0209EC5C ; case 5
	b _0209ECFC ; case 6
	b _0209ED08 ; case 7
_0209EC40:
	add sp, sp, #0x3c
	mov r0, #1
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
_0209EC4C:
	mov r7, #4
	b _0209E9B0
_0209EC54:
	mov r7, #5
	b _0209E9B0
_0209EC5C:
	mov r0, sl
	mov r1, #1
	bl DC_InvalidateRange
	cmp r7, #4
	ldrb r0, [sl]
	bne _0209EC8C
	tst r0, #2
	movne r7, #3
	bne _0209E9B0
	add sp, sp, #0x3c
	mov r0, r6
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
_0209EC8C:
	tst r0, #1
	addeq sp, sp, #0x3c
	moveq r0, #1
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	tst r0, #0x20
	bne _0209ECE8
	bl OS_GetTick
	ldr r2, [sp, #0x34]
	mov ip, r6
	subs sb, r0, r2
	mov r0, #0x40
	sbc lr, r1, fp
	umull r0, r1, sb, r0
	mla r1, sb, ip, r1
	mov sb, #0x40
	mla r1, lr, sb, r1
	ldr r2, _0209ED34 ; =0x000082EA
	mov r3, r6
	bl _ll_udiv
	mov r2, r6
	cmp r1, r2
	cmpeq r0, r8
	bls _0209ECF0
_0209ECE8:
	mov r7, #6
	b _0209E9B0
_0209ECF0:
	mov r0, #0x4000
	bl SVC_WaitByLoop
	b _0209E9B0
_0209ECFC:
	add sp, sp, #0x3c
	mov r0, r6
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
_0209ED08:
	add sp, sp, #0x3c
	mov r0, #1
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
_0209ED14:
	mov r0, r6
	add sp, sp, #0x3c
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_0209ED20: .word Work
_0209ED24: .word 0x01020000
_0209ED28: .word 0x02002200
_0209ED2C: .word 0x03002100
_0209ED30: .word _021D43C8
_0209ED34: .word 0x000082EA
	arm_func_end NVRAMm_ExecuteCommand

	arm_func_start readNvram
readNvram: ; 0x0209ED38
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, lr}
	mov r4, r2
	mov r6, r0
	mov r0, r4
	mov r5, r1
	bl DC_InvalidateRange
	mov r8, #4
	mov r7, #1
_0209ED58:
	mov r0, r8
	mov r1, r7
	bl PXI_IsCallbackReady
	cmp r0, #0
	beq _0209ED58
	ldr r1, _0209EDC4 ; =Callback_NVRAM
	mov r0, #4
	bl PXI_SetFifoRecvCallback
	mov r0, r5, lsl #0x10
	mov r8, r0, lsr #0x10
	mov r7, #0x40000
	mov sb, #1
_0209ED88:
	mov r0, sb
	mov r1, r6
	mov r2, r8
	mov r3, r4
	bl NVRAMm_ExecuteCommand
	cmp r0, #1
	beq _0209EDB0
	mov r0, r7
	bl SVC_WaitByLoop
	b _0209ED88
_0209EDB0:
	mov r0, r4
	mov r1, r5
	bl DC_InvalidateRange
	mov r0, #1
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	.align 2, 0
_0209EDC4: .word Callback_NVRAM
	arm_func_end readNvram

	arm_func_start writeNvram
writeNvram: ; 0x0209EDC8
	stmdb sp!, {r4, r5, r6, r7, r8, lr}
	mov r8, r0
	mov r7, r1
	mov r6, r2
	mov r5, #4
	mov r4, #1
_0209EDE0:
	mov r0, r5
	mov r1, r4
	bl PXI_IsCallbackReady
	cmp r0, #0
	beq _0209EDE0
	ldr r1, _0209EE40 ; =Callback_NVRAM
	mov r0, #4
	bl PXI_SetFifoRecvCallback
	mov r0, r6
	mov r1, r7
	bl DC_StoreRange
	mov r4, #0x40000
	mov r5, #2
_0209EE14:
	mov r0, r5
	mov r1, r8
	mov r2, r7
	mov r3, r6
	bl NVRAMm_ExecuteCommand
	cmp r0, #1
	ldmeqia sp!, {r4, r5, r6, r7, r8, pc}
	mov r0, r4
	bl SVC_WaitByLoop
	b _0209EE14
_0209EE3C: ; 0x0209EE3C
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	.align 2, 0
_0209EE40: .word Callback_NVRAM
	arm_func_end writeNvram

	arm_func_start verify
verify: ; 0x0209EE44
	stmdb sp!, {r4, r5, r6, lr}
	mov r5, r2
	mov r4, r3
	mov r6, r0
	mov r0, r1
	mov r1, r5
	mov r2, r4
	bl readNvram
	cmp r0, #0
	moveq r0, #0
	ldmeqia sp!, {r4, r5, r6, pc}
	mov r0, r6
	mov r1, r4
	mov r2, r5
	bl memcmp
	cmp r0, #0
	moveq r0, #1
	movne r0, #0
	ldmia sp!, {r4, r5, r6, pc}
	arm_func_end verify

	arm_func_start writeDisable
writeDisable: ; 0x0209EE90
	stmdb sp!, {r4, r5, r6, lr}
	mov r5, #4
	mov r4, #1
_0209EE9C:
	mov r0, r5
	mov r1, r4
	bl PXI_IsCallbackReady
	cmp r0, #0
	beq _0209EE9C
	ldr r1, _0209EEF8 ; =Callback_NVRAM
	mov r0, #4
	bl PXI_SetFifoRecvCallback
	mov r4, #0x40000
	mov r6, #7
	mov r5, #0
_0209EEC8:
	mov r0, r6
	mov r1, r5
	mov r2, r5
	mov r3, r5
	bl NVRAMm_ExecuteCommand
	cmp r0, #1
	beq _0209EEF0
	mov r0, r4
	bl SVC_WaitByLoop
	b _0209EEC8
_0209EEF0:
	mov r0, #1
	ldmia sp!, {r4, r5, r6, pc}
	.align 2, 0
_0209EEF8: .word Callback_NVRAM
	arm_func_end writeDisable

	arm_func_start Callback_NVRAM
Callback_NVRAM: ; 0x0209EEFC
	ldr r0, _0209EF28 ; =_021D43C8
	and r1, r1, #0xff
	strh r1, [r0]
	mov r1, #1
	str r1, [r0, #4]
	cmp r2, #0
	movne r1, #0xff
	strneh r1, [r0]
	ldr r0, _0209EF28 ; =_021D43C8
	ldrh r0, [r0]
	bx lr
	.align 2, 0
_0209EF28: .word _021D43C8
	arm_func_end Callback_NVRAM

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

	arm_func_start DWCi_BACKUPlGetWifi
DWCi_BACKUPlGetWifi: ; 0x0209EFEC
	ldr r0, _0209EFF4 ; =Wifi
	bx lr
	.align 2, 0
_0209EFF4: .word Wifi
	arm_func_end DWCi_BACKUPlGetWifi

	arm_func_start DWC_BM_Init
DWC_BM_Init: ; 0x0209EFF8
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, lr}
	sub sp, sp, #0x10
	mov r8, r0
	mov r1, #0
	mov r2, #0x700
	bl MI_CpuFill8
	mov r0, r8
	bl DWCi_BACKUPlInit
	cmp r0, #0
	addeq sp, sp, #0x10
	ldreq r0, _0209F2EC ; =0xFFFFD8EF
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
	ldr r1, _0209F2F0 ; =0x0000A001
	add r0, r8, #0x500
	bl MATHi_CRC16InitTableRev
	mov r0, r8
	bl DWCi_BACKUPlRead
	cmp r0, #0
	addeq sp, sp, #0x10
	ldreq r0, _0209F2EC ; =0xFFFFD8EF
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
	add r0, sp, #0
	mov r1, #0
	mov r2, #0x10
	bl MI_CpuFill8
	mov r6, r8
	mov r7, r8
	mov r5, #0
	mov sl, #1
	add sb, sp, #0
	mov r4, #0xfe
_0209F074:
	mov r1, r6
	mov r2, r4
	add r0, r8, #0x500
	bl MATH_CalcCRC16
	add r1, r8, r5, lsl #8
	ldrh r1, [r1, #0xfe]
	cmp r0, r1
	bne _0209F0A4
	mov r0, r7
	bl checkAp
	cmp r0, #0
	strne sl, [sb, r5, lsl #2]
_0209F0A4:
	add r5, r5, #1
	cmp r5, #3
	add r6, r6, #0x100
	add r7, r7, #0x100
	blt _0209F074
	add r0, r8, #0x500
	add r1, r8, #0x300
	mov r2, #0xfe
	bl MATH_CalcCRC16
	add r1, r8, #0x300
	ldrh r1, [r1, #0xfe]
	cmp r0, r1
	moveq r0, #1
	ldr r1, [sp]
	streq r0, [sp, #0xc]
	cmp r1, #0
	ldrne r0, [sp, #4]
	cmpne r0, #0
	ldrne r0, [sp, #8]
	cmpne r0, #0
	ldrne r0, [sp, #0xc]
	cmpne r0, #0
	beq _0209F114
	add r0, r8, #0xf0
	bl DWCi_BACKUPlSetWiFi
	add sp, sp, #0x10
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
_0209F114:
	cmp r1, #0
	ldreq r0, [sp, #4]
	cmpeq r0, #0
	ldreq r0, [sp, #8]
	cmpeq r0, #0
	ldreq r0, [sp, #0xc]
	cmpeq r0, #0
	bne _0209F158
	mov r0, r8
	bl init
	mov r0, r8
	bl DWCi_BACKUPlWriteAll
	cmp r0, #0
	movne r0, #0
	add sp, sp, #0x10
	ldreq r0, _0209F2F4 ; =0xFFFFD8F0
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
_0209F158:
	cmp r1, #0
	ldrne r0, [sp, #4]
	cmpne r0, #0
	bne _0209F1A0
	ldr r0, [sp, #8]
	cmp r0, #0
	ldrne r0, [sp, #0xc]
	cmpne r0, #0
	bne _0209F1A0
	mov r0, r8
	bl init
	mov r0, r8
	bl DWCi_BACKUPlWriteAll
	cmp r0, #0
	movne r0, #0
	add sp, sp, #0x10
	ldreq r0, _0209F2F4 ; =0xFFFFD8F0
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
_0209F1A0:
	cmp r1, #0
	ldreq r0, [sp, #4]
	cmpeq r0, #0
	bne _0209F1D4
	mov r0, r8
	bl init
	mov r0, r8
	bl DWCi_BACKUPlWriteAll
	cmp r0, #0
	ldrne r0, _0209F2F8 ; =0xFFFFD8ED
	add sp, sp, #0x10
	ldreq r0, _0209F2F4 ; =0xFFFFD8F0
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
_0209F1D4:
	cmp r1, #0
	bne _0209F204
	mov r0, r8
	mov r1, #0
	bl initPage
	add r0, r8, #0x1f0
	add r1, r8, #0xf0
	mov r2, #0xe
	bl MI_CpuCopy8
	ldrb r0, [r8, #0x1ef]
	strb r0, [r8, #0xef]
	b _0209F234
_0209F204:
	ldr r0, [sp, #4]
	cmp r0, #0
	bne _0209F234
	mov r0, r8
	mov r1, #1
	bl initPage
	add r0, r8, #0xf0
	add r1, r8, #0x1f0
	mov r2, #0xe
	bl MI_CpuCopy8
	ldrb r0, [r8, #0xef]
	strb r0, [r8, #0x1ef]
_0209F234:
	add r0, r8, #0xf0
	bl DWCi_BACKUPlSetWiFi
	ldr r0, [sp, #8]
	cmp r0, #0
	bne _0209F254
	mov r0, r8
	mov r1, #2
	bl initPage
_0209F254:
	ldr r0, [sp, #0xc]
	cmp r0, #0
	bne _0209F270
	add r1, r8, #0x300
	mov r0, #0
	mov r2, #0x100
	bl MIi_CpuClear16
_0209F270:
	mov r4, #0
	mov r5, r4
	mov r2, #1
	add r3, sp, #0
_0209F280:
	ldr r0, [r3, r5, lsl #2]
	cmp r0, #0
	bne _0209F2B4
	ldrb r0, [r8, #0xef]
	tst r0, r2, lsl r5
	beq _0209F2B4
	mov r1, r0
	mvn r0, r2, lsl r5
	and r0, r1, r0
	strb r0, [r8, #0xef]
	and r0, r0, #0xff
	mov r4, r2
	strb r0, [r8, #0x1ef]
_0209F2B4:
	add r5, r5, #1
	cmp r5, #3
	blt _0209F280
	mov r0, r8
	bl DWCi_BACKUPlWriteAll
	cmp r0, #0
	addeq sp, sp, #0x10
	ldreq r0, _0209F2F4 ; =0xFFFFD8F0
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
	cmp r4, #0
	ldrne r0, _0209F2FC ; =0xFFFFD8EE
	moveq r0, #0
	add sp, sp, #0x10
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
	.align 2, 0
_0209F2EC: .word 0xFFFFD8EF
_0209F2F0: .word 0x0000A001
_0209F2F4: .word 0xFFFFD8F0
_0209F2F8: .word 0xFFFFD8ED
_0209F2FC: .word 0xFFFFD8EE
	arm_func_end DWC_BM_Init

	arm_func_start checkAp
checkAp: ; 0x0209F300
	stmdb sp!, {r3, r4, lr}
	sub sp, sp, #4
	mov r4, r0
	ldrb r0, [r4, #0xe7]
	cmp r0, #0xff
	addeq sp, sp, #4
	moveq r0, #1
	ldmeqia sp!, {r3, r4, pc}
	cmp r0, #2
	addhi sp, sp, #4
	movhi r0, #0
	ldmhiia sp!, {r3, r4, pc}
	add r0, r4, #0x40
	bl DWC_BACKUPlCheckSsid
	cmp r0, #0
	addeq sp, sp, #4
	moveq r0, #0
	ldmeqia sp!, {r3, r4, pc}
	ldr r1, _0209F3FC ; =DWCi_SETTING_NONE
	add r0, r4, #0xc0
	mov r2, #4
	bl memcmp
	cmp r0, #0
	beq _0209F3B0
	add r0, r4, #0xc4
	bl DWC_BACKUPlCheckAddress
	cmp r0, #0
	addeq sp, sp, #4
	moveq r0, #0
	ldmeqia sp!, {r3, r4, pc}
	ldrb r0, [r4, #0xd0]
	cmp r0, #0x20
	addhi sp, sp, #4
	movhi r0, #0
	ldmhiia sp!, {r3, r4, pc}
	add r1, sp, #0
	bl DWCi_BACKUPlConvMaskAddr
	add r1, sp, #0
	add r0, r4, #0xc0
	bl DWC_BACKUPlCheckIp
	cmp r0, #0
	addeq sp, sp, #4
	moveq r0, #0
	ldmeqia sp!, {r3, r4, pc}
_0209F3B0:
	ldr r1, _0209F3FC ; =DWCi_SETTING_NONE
	add r0, r4, #0xc8
	mov r2, #4
	bl memcmp
	cmp r0, #0
	beq _0209F3F0
	add r0, r4, #0xc8
	bl DWC_BACKUPlCheckAddress
	cmp r0, #0
	bne _0209F3F0
	add r0, r4, #0xcc
	bl DWC_BACKUPlCheckAddress
	cmp r0, #0
	addeq sp, sp, #4
	moveq r0, #0
	ldmeqia sp!, {r3, r4, pc}
_0209F3F0:
	mov r0, #1
	add sp, sp, #4
	ldmia sp!, {r3, r4, pc}
	.align 2, 0
_0209F3FC: .word DWCi_SETTING_NONE
	arm_func_end checkAp

	arm_func_start init
init: ; 0x0209F400
	stmdb sp!, {r4, r5, r6, r7, lr}
	sub sp, sp, #0x14
	mov r7, r0
	mov r1, r7
	mov r0, #0
	mov r2, #0x400
	bl MIi_CpuClear16
	mov r2, #0
	mov r1, #0xff
_0209F424:
	add r0, r7, r2, lsl #8
	add r2, r2, #1
	strb r1, [r0, #0xe7]
	cmp r2, #3
	blt _0209F424
	add r0, sp, #0
	bl DWCi_AUTH_GetNewWiFiInfo
	add r0, sp, #0
	bl DWCi_BACKUPlConvWifiInfo
	mov r5, r0
	mov r6, #0
	mov r4, #0xe
_0209F454:
	mov r0, r5
	mov r2, r4
	add r1, r7, #0xf0
	bl MI_CpuCopy8
	add r6, r6, #1
	cmp r6, #2
	add r7, r7, #0x100
	blt _0209F454
	mov r0, #0
	add sp, sp, #0x14
	ldmia sp!, {r4, r5, r6, r7, pc}
	arm_func_end init
