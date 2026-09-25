	.include "asm/macros.inc"
	.include "dwcbm.inc"
	.include "global.inc"
	.public DWCi_AUTH_GetNewWiFiInfo
	.public DWCi_Util_WiFiId_scrambleUid
	.extern DWCi_BM_GetWiFiInfo
	.extern DWCi_util_wifiid_exctable
	.extern DWCi_util_wifiid_ttable

.text
	arm_func_start DWCi_Util_WiFiId_scrambleUid
DWCi_Util_WiFiId_scrambleUid: ; 0x0209F4AC
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	sub sp, sp, #0x10
	ldr r4, _0209F624 ; =0x0000FFFF
	mov lr, #0
	and ip, lr, #0
	add r5, r4, #0xff0000
	and r2, r2, #1
	mov r6, ip, lsl #2
	and r3, r3, #3
	and r5, r1, r5
	orr r1, r3, r2, lsl #2
	orr r6, r6, r2, lsr #30
	mov r7, ip, lsl #3
	and r2, r0, r4
	orr r0, r1, r5, lsl #3
	orr r1, r0, r2, lsl #27
	mov r3, ip, lsl #0x1b
	orr r3, r3, r2, lsr #5
	orr r7, r7, r5, lsr #29
	orr r0, ip, r6
	orr r0, r7, r0
	orr r0, r3, r0
	add r2, sp, #8
	str r1, [sp, #8]
	str r0, [sp, #0xc]
_0209F510:
	ldrb r0, [r2]
	add lr, lr, #1
	cmp lr, #6
	eor r0, r0, #0xd6
	strb r0, [r2], #1
	blt _0209F510
	ldr r2, _0209F628 ; =DWCi_util_wifiid_ttable
	add r4, sp, #8
	mov r5, #0
_0209F534:
	ldrb r3, [r4]
	add r5, r5, #1
	cmp r5, #5
	mov r0, r3, asr #4
	and r1, r0, #0xf
	and r0, r3, #0xf
	ldrb r1, [r2, r1]
	ldrb r0, [r2, r0]
	orr r0, r0, r1, lsl #4
	strb r0, [r4], #1
	blt _0209F534
	add r0, sp, #8
	add r1, sp, #0
	mov r2, #8
	bl MI_CpuCopy8
	ldr r4, _0209F62C ; =DWCi_util_wifiid_exctable
	add r3, sp, #0
	mov r5, #0
	add r0, sp, #8
_0209F580:
	ldrb r2, [r3], #1
	ldrb r1, [r4], #1
	add r5, r5, #1
	cmp r5, #5
	strb r2, [r0, r1]
	blt _0209F580
	ldrb r2, [sp, #0xd]
	mov r1, #0
	ldr r3, [sp, #8]
	and r2, r2, #7
	strb r1, [sp, #0xf]
	strb r1, [sp, #0xe]
	strb r2, [sp, #0xd]
	ldr r2, [sp, #0xc]
	mov r4, r3, lsl #1
	mov r2, r2, lsl #1
	orr r2, r2, r3, lsr #31
	str r2, [sp, #0xc]
	ldrb r2, [sp, #0xd]
	str r4, [sp, #8]
	ldrb r3, [sp, #8]
	mov r2, r2, asr #3
	and r2, r2, #1
	orr r2, r3, r2
	strb r2, [sp, #8]
_0209F5E4:
	ldrb r2, [r0]
	add r1, r1, #1
	cmp r1, #6
	eor r2, r2, #0x67
	strb r2, [r0], #1
	blt _0209F5E4
	ldrb r0, [sp, #0xd]
	mov r1, #0
	strb r1, [sp, #0xf]
	and r0, r0, #7
	strb r1, [sp, #0xe]
	strb r0, [sp, #0xd]
	ldr r0, [sp, #8]
	ldr r1, [sp, #0xc]
	add sp, sp, #0x10
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.align 2, 0
_0209F624: .word 0x0000FFFF
_0209F628: .word DWCi_util_wifiid_ttable
_0209F62C: .word DWCi_util_wifiid_exctable
	arm_func_end DWCi_Util_WiFiId_scrambleUid

	arm_func_start DWCi_AUTH_GetNewWiFiInfo
DWCi_AUTH_GetNewWiFiInfo: ; 0x0209F630
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x24
	mov sl, r0
	bl DWCi_BM_GetWiFiInfo
	bl RTC_Init
	add r0, sp, #0x14
	bl RTC_GetDate
	cmp r0, #0
	addne sp, sp, #0x24
	movne r0, #0
	ldmneia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	add r0, sp, #8
	bl RTC_GetTime
	cmp r0, #0
	addne sp, sp, #0x24
	movne r0, #0
	ldmneia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	add r0, sp, #0x14
	add r1, sp, #8
	bl RTC_ConvertDateTimeToSecond
	mov r4, r0
	mov r0, #0
	subs r2, r4, r0
	sbcs r2, r1, r0
	addlt sp, sp, #0x24
	ldmltia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	bl OS_IsTickAvailable
	cmp r0, #0
	beq _0209F6AC
	ldr r0, _0209F7FC ; =OS_GetTick
	adds r4, r4, r0
_0209F6AC:
	add r0, sp, #0
	bl OS_GetMacAddress
	ldrb r0, [sp, #1]
	ldr r5, _0209F800 ; =0x5D588B65
	ldr r6, _0209F804 ; =0x00269EC3
	ldrb r1, [sp]
	mla sb, r4, r5, r6
	mov r0, r0, lsl #8
	orr r1, r0, r1, lsl #16
	ldrb r2, [sp, #2]
	ldr r0, _0209F808 ; =0x000009BF
	ldrb r7, [sp, #4]
	orr r1, r2, r1
	cmp r1, r0
	mov r2, sb, lsr #0x10
	mov r0, #0x3e8
	mul r3, r2, r0
	mov r2, r3, lsr #0x10
	movne r1, #1
	ldrb r0, [sp, #5]
	ldrb r4, [sp, #3]
	mov r3, r7, lsl #8
	moveq r1, #0
	orr r4, r3, r4, lsl #16
	strh r2, [sl, #0x10]
	mov r3, #0
	str r3, [sl]
	str r3, [sl, #4]
	ldrh r2, [sl, #0x12]
	and r7, r1, #0xff
	orr r8, r0, r4
	str r3, [sl, #8]
	cmp r2, #0
	str r3, [sl, #0xc]
	bne _0209F79C
	ldr r0, [sl, #8]
	cmp r3, r3
	cmpeq r0, r3
	bne _0209F7F0
	mov fp, r3
	mov r4, r3
_0209F750:
	mul r0, sb, r5
	adds sb, r0, r6
	bne _0209F768
_0209F75C:
	mul r0, sb, r5
	adds sb, r0, r6
	beq _0209F75C
_0209F768:
	strh sb, [sl, #0x12]
	ldrh r0, [sl, #0x12]
	mov r1, r8
	mov r2, r7
	mov r3, #0
	bl DWCi_Util_WiFiId_scrambleUid
	str r0, [sl, #8]
	str r1, [sl, #0xc]
	ldr r0, [sl, #8]
	cmp r1, fp
	cmpeq r0, r4
	beq _0209F750
	b _0209F7F0
_0209F79C:
	ldr r0, [sl, #8]
	cmp r3, r3
	cmpeq r0, r3
	bne _0209F7F0
	mov r6, r3
	mov r5, r3
	mov r4, r3
_0209F7B8:
	ldrh r0, [sl, #0x12]
	mov r1, r8
	mov r2, r7
	add r0, r0, #1
	strh r0, [sl, #0x12]
	ldrh r0, [sl, #0x12]
	mov r3, r6
	bl DWCi_Util_WiFiId_scrambleUid
	str r0, [sl, #8]
	str r1, [sl, #0xc]
	ldr r0, [sl, #8]
	cmp r1, r5
	cmpeq r0, r4
	beq _0209F7B8
_0209F7F0:
	mov r0, #1
	add sp, sp, #0x24
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_0209F7FC: .word OS_GetTick
_0209F800: .word 0x5D588B65
_0209F804: .word 0x00269EC3
_0209F808: .word 0x000009BF
	arm_func_end DWCi_AUTH_GetNewWiFiInfo
