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
	.extern checkAp
	.extern init

.text
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
