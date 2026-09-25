	.include "asm/macros.inc"
	.include "dwcbm.inc"
	.include "global.inc"
	.extern DWCi_BM_GetWiFiInfo
	.extern DWCi_Util_WiFiId_scrambleUid

.text
	arm_func_start DWCi_AUTH_RemakeWiFiID
DWCi_AUTH_RemakeWiFiID: ; 0x0209F8A0
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x24
	add r2, sp, #0
	mov r1, #0
	mov sl, r0
	strb r1, [r2]
	strb r1, [r2, #1]
	strb r1, [r2, #2]
	strb r1, [r2, #3]
	strb r1, [r2, #4]
	strb r1, [r2, #5]
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
	beq _0209F93C
	ldr r0, _0209FA30 ; =OS_GetTick
	adds r4, r4, r0
_0209F93C:
	add r0, sp, #0
	bl OS_GetMacAddress
	ldrb r0, [sp, #1]
	ldr r5, _0209FA34 ; =0x5D588B65
	ldr r6, _0209FA38 ; =0x00269EC3
	ldrb r1, [sp]
	mov r0, r0, lsl #8
	mla sb, r4, r5, r6
	orr r1, r0, r1, lsl #16
	ldrb r2, [sp, #2]
	ldr r0, _0209FA3C ; =0x000009BF
	ldrb r7, [sp, #4]
	orr r1, r2, r1
	cmp r1, r0
	movne r1, #1
	mov r2, sb, lsr #0x10
	mov r0, #0x3e8
	mul r4, r2, r0
	mov r2, r4, lsr #0x10
	ldrb r0, [sp, #5]
	ldrb r8, [sp, #3]
	mov r4, r7, lsl #8
	moveq r1, #0
	mov r3, #0
	strh r2, [sl, #0x10]
	str r3, [sl, #8]
	str r3, [sl, #0xc]
	orr r4, r4, r8, lsl #16
	ldr r2, [sl, #8]
	cmp r3, #0
	cmpeq r2, #0
	and r7, r1, #0xff
	orr r8, r0, r4
	bne _0209FA24
	mov fp, r3
	mov r4, r3
_0209F9CC:
	mla sb, r5, sb, r6
	b _0209F9D8
_0209F9D4:
	mla sb, r5, sb, r6
_0209F9D8:
	cmp sb, #0
	beq _0209F9D4
	ldrh r2, [sl, #0x12]
	mov r0, sb, lsl #0x10
	mov r1, r0, lsr #0x10
	cmp r2, r0, lsr #16
	beq _0209F9D4
	strh r1, [sl, #0x12]
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
	beq _0209F9CC
_0209FA24:
	mov r0, #1
	add sp, sp, #0x24
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_0209FA30: .word OS_GetTick
_0209FA34: .word 0x5D588B65
_0209FA38: .word 0x00269EC3
_0209FA3C: .word 0x000009BF
	arm_func_end DWCi_AUTH_RemakeWiFiID
