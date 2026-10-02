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
	.public NVRAMm_ExecuteCommand
	.public readNvram
	.extern Callback_NVRAM

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
