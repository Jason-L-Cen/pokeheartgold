	.extern SOCL_Accept
	.extern SOCL_Bind
	.extern SOCL_Cleanup
	.extern SOCL_Close
	.extern SOCL_Connect
	.extern SOCL_CreateSocket
	.extern SOCL_GetHostID
	.extern SOCL_GetStatus
	.extern SOCL_InetAtoH
	.extern SOCL_Listen
	.extern SOCL_ReadFrom
	.extern SOCL_Resolve
	.extern SOCL_SetResolver
	.extern SOCL_Shutdown
	.extern SOCL_SocketIsInvalid
	.extern SOCL_Startup
	.extern SOCL_WriteTo
	.extern SOCLi_CreateCommandPacket
	.extern SOCLi_ExecCommandPacketInRecvPipe
	.extern SOCLi_SocketGetNextPtr
	.extern _02108FB8
	.extern _02108FC0
	.extern _02108FC8
	.extern _02110630
	.extern _02110648
	.extern _02110678
	.extern _02110684
	.extern _02110688
	.extern _02110690
	.extern _0211069C
	.extern _021106B0
	.extern _021106B8
	.extern _021106FC
	.extern _02110700
	.extern _02110704
	.extern _02110708
	.extern _02110710
	.extern _02110718
	.extern _02110758
	.extern _02110798
	.extern _02110898
	.extern _02110899
	.extern _021D4490
	.extern _021D4494
	.extern _021D44AC
	.extern _021D44B4
	.extern _021D44D4
	.extern _021D450C
	.extern _021D451C
	.extern _021D4620
	.extern _021D4634
	.extern _021D4660
	.extern _021D4680
	.extern _021D4688
	.extern _021D46A0
	.extern _021D46AA
	.extern _021D4700
	.extern _021D4720
	.extern _021D4764
	.extern _021D47C8
	.extern _021D4888
	.extern _021D4948
	.extern _021D4B08
	.extern _021D4B32
	.extern _021D4B4C
	.extern _021D4B76
	.extern _021D4CCC
	.extern _021D5660
	.extern _021D5E60
	.extern _021D5E68
	.extern _021D5FD8
	.extern _021D5FEB
	.extern _021D5FEC
	.extern _021D5FF0
	.extern _021D5FF4
	.extern _021D5FFC
	.extern _021D601C
	.extern _021D6020
	.include "asm/macros.inc"
	.include "wifi.inc"
	.include "global.inc"
	.public CPSi_big_div
	.public CPSi_big_modinv
	.public CPSi_big_power
	.extern CPSi_big_add
	.extern CPSi_big_compare
	.extern CPSi_big_mult
	.extern CPSi_big_mult_small
	.extern CPSi_big_sign
	.extern CPSi_big_sqr
	.extern CPSi_big_sub
	.extern count_digits
	.extern get48bits_1

.text
	arm_func_start get48bits_2
get48bits_2: ; 0x020ACF7C
	ldrh r1, [r0]
	ldrh r0, [r0, #-2]
	mov r0, r0, lsl #0x10
	bx lr
	arm_func_end get48bits_2

	arm_func_start get48bits_3
get48bits_3: ; 0x020ACF8C
	ldrh r1, [r0]
	ldrh r2, [r0, #-2]
	ldrh r3, [r0, #-4]
	orr r0, r3, r2, lsl #16
	bx lr
	arm_func_end get48bits_3

	arm_func_start get64bits
get64bits: ; 0x020ACFA0
	ldrh r2, [r0]
	ldrh r3, [r0, #-2]
	orr r1, r3, r2, lsl #16
	ldrh r2, [r0, #-4]
	ldrh r3, [r0, #-6]
	orr r0, r3, r2, lsl #16
	bx lr
	arm_func_end get64bits

	arm_func_start CPSi_big_div
CPSi_big_div: ; 0x020ACFBC
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x20
	ldr sb, [sp, #0x48]
	ldr r8, [sp, #0x4c]
	str r1, [sp, #4]
	add r6, r8, sb, lsl #1
	str r0, [sp]
	mov sl, r2
	mov r0, r6
	mov r2, sb, lsl #2
	mov r1, #0
	str r3, [sp, #8]
	add r7, r6, sb, lsl #1
	bl MI_CpuFill8
	ldr r0, [sp, #4]
	mov r1, sb
	bl count_digits
	mov fp, r0
	mov r0, sl
	mov r1, sb
	bl count_digits
	mov r5, r0
	cmp fp, #0
	cmpgt r5, #0
	ble _020AD154
	sub r0, sb, fp
	add r0, r5, r0
	sub r4, r0, #1
	cmp r4, sb
	blt _020AD048
	ldr r0, [sp, #4]
	mov r1, r7
	mov r2, sb, lsl #1
	bl MI_CpuCopy8
	b _020AD154
_020AD048:
	ldr r0, [sp, #4]
	add r1, r6, r4, lsl #1
	mov r2, fp, lsl #1
	bl MI_CpuCopy8
	cmp r5, #2
	ble _020AD07C
	add r0, sl, r5, lsl #1
	sub r0, r0, #2
	mov fp, r5, lsl #1
	bl get48bits_3
	str r0, [sp, #0x18]
	str r1, [sp, #0x10]
	b _020AD0B0
_020AD07C:
	add r0, sl, r5, lsl #1
	cmp r5, #1
	sub r0, r0, #2
	ble _020AD0A0
	mov fp, r5, lsl #1
	bl get48bits_2
	str r0, [sp, #0x18]
	str r1, [sp, #0x10]
	b _020AD0B0
_020AD0A0:
	mov fp, r5, lsl #1
	bl get48bits_1
	str r0, [sp, #0x18]
	str r1, [sp, #0x10]
_020AD0B0:
	cmp r4, sb
	bge _020AD154
	mov r0, sb, lsl #1
	sub r0, r0, #1
	mov r0, r0, lsl #1
	str r0, [sp, #0x14]
_020AD0C8:
	ldr r2, [sp, #0x14]
	mov r1, r6
	add r0, r6, #2
	bl memmove
	add r0, r7, fp
	bl get64bits
	ldr r2, [sp, #0x18]
	ldr r3, [sp, #0x10]
	bl _ll_udiv
	mov r5, r0
	ldr r0, _020AD198 ; =0x0000FFFF
	cmp r5, r0
	movhi r5, r0
_020AD0FC:
	mov r2, r5, lsl #0x10
	mov r0, r8
	mov r1, sl
	mov r2, r2, lsr #0x10
	mov r3, sb
	bl CPSi_big_mult_small
	mov r0, r7
	mov r1, r8
	mov r2, sb
	bl CPSi_big_compare
	cmp r0, #0
	sublt r5, r5, #1
	blt _020AD0FC
	mov r0, r7
	mov r1, r7
	mov r2, r8
	mov r3, sb
	bl CPSi_big_sub
	strh r5, [r6]
	add r4, r4, #1
	cmp r4, sb
	blt _020AD0C8
_020AD154:
	ldr r0, [sp]
	cmp r0, #0
	beq _020AD170
	ldr r1, [sp]
	mov r0, r6
	mov r2, sb, lsl #1
	bl MI_CpuCopy8
_020AD170:
	ldr r0, [sp, #8]
	cmp r0, #0
	addeq sp, sp, #0x20
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldr r1, [sp, #8]
	mov r0, r7
	mov r2, sb, lsl #1
	bl MI_CpuCopy8
	add sp, sp, #0x20
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_020AD198: .word 0x0000FFFF
	arm_func_end CPSi_big_div

	arm_func_start CPSi_big_power
CPSi_big_power: ; 0x020AD19C
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0xc
	ldr r4, _020AD328 ; =_021D4634
	mov r8, r3
	ldr r3, [r4]
	mov sl, r0
	mov r0, r8, lsl #3
	ldr r7, [sp, #0x30]
	str r1, [sp, #8]
	mov sb, r2
	blx r3
	movs r4, r0
	addeq sp, sp, #0xc
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	sub r1, r8, #1
	add r0, sl, #2
	mov r2, r1, lsl #1
	mov r1, #0
	add r5, r4, r8, lsl #1
	bl MI_CpuFill8
	mov r2, #1
	mov r0, sb
	mov r1, r8
	strh r2, [sl]
	bl count_digits
	sub r0, r8, r0
	mov r6, r0, lsl #4
	cmp r6, r8, lsl #4
	bhs _020AD250
	mov r2, #0x8000
_020AD214:
	sub r0, r8, r6, asr #4
	add r0, sb, r0, lsl #1
	ldrh r0, [r0, #-2]
	and r1, r6, #0xf
	tst r0, r2, lsr r1
	beq _020AD244
	ldr r0, [sp, #8]
	mov r1, sl
	mov r2, r8, lsl #1
	bl MI_CpuCopy8
	add r6, r6, #1
	b _020AD250
_020AD244:
	add r6, r6, #1
	cmp r6, r8, lsl #4
	blo _020AD214
_020AD250:
	cmp r6, r8, lsl #4
	bhs _020AD310
	mov fp, r8, lsl #1
_020AD25C:
	mov r0, r4
	mov r1, sl
	mov r2, r8
	bl CPSi_big_sqr
	mov r0, r4
	mov r1, sl
	mov r2, fp
	bl MI_CpuCopy8
	cmp r7, #0
	beq _020AD2A0
	str r8, [sp]
	mov r0, #0
	mov r1, sl
	mov r2, r7
	mov r3, sl
	str r5, [sp, #4]
	bl CPSi_big_div
_020AD2A0:
	sub r0, r8, r6, asr #4
	add r0, sb, r0, lsl #1
	ldrh r1, [r0, #-2]
	and r2, r6, #0xf
	mov r0, #0x8000
	tst r1, r0, lsr r2
	beq _020AD304
	ldr r2, [sp, #8]
	mov r0, r4
	mov r1, sl
	mov r3, r8
	bl CPSi_big_mult
	mov r0, r4
	mov r1, sl
	mov r2, fp
	bl MI_CpuCopy8
	cmp r7, #0
	beq _020AD304
	str r8, [sp]
	mov r0, #0
	mov r1, sl
	mov r2, r7
	mov r3, sl
	str r5, [sp, #4]
	bl CPSi_big_div
_020AD304:
	add r6, r6, #1
	cmp r6, r8, lsl #4
	blo _020AD25C
_020AD310:
	ldr r1, _020AD32C ; =_021D4660
	mov r0, r4
	ldr r1, [r1]
	blx r1
	add sp, sp, #0xc
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_020AD328: .word _021D4634
_020AD32C: .word _021D4660
	arm_func_end CPSi_big_power

	arm_func_start CPSi_big_modinv
CPSi_big_modinv: ; 0x020AD330
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x14
	ldr sb, [sp, #0x38]
	mov sl, r3
	add fp, sb, sl, lsl #1
	add r4, fp, sl, lsl #1
	add r5, r4, sl, lsl #1
	add r6, r5, sl, lsl #1
	add r7, r6, sl, lsl #1
	str r0, [sp, #8]
	mov r0, r1
	add r1, r7, sl, lsl #1
	str r1, [sp, #0x10]
	mov r8, sl, lsl #1
	str r2, [sp, #0xc]
	mov r1, sb
	mov r2, r8
	bl MI_CpuCopy8
	ldr r0, [sp, #0xc]
	mov r1, r4
	mov r2, r8
	bl MI_CpuCopy8
	mov r0, #1
	strh r0, [r4, r8]
	mov r0, sb
	mov r1, sl
	bl CPSi_big_sign
	cmp r0, #0
	ble _020AD440
_020AD3A4:
	ldr r3, [sp, #0x10]
	str sl, [sp]
	str r3, [sp, #4]
	mov r0, fp
	mov r1, r4
	mov r2, sb
	mov r3, r7
	bl CPSi_big_div
	mov r0, sb
	mov r1, r4
	mov r2, r8
	bl MI_CpuCopy8
	mov r0, r7
	mov r1, sb
	mov r2, r8
	bl MI_CpuCopy8
	mov r0, r7
	mov r1, fp
	mov r2, r5
	mov r3, sl
	bl CPSi_big_mult
	mov r0, r7
	mov r1, r6
	mov r2, r7
	mov r3, sl
	bl CPSi_big_sub
	mov r0, r5
	mov r1, r6
	mov r2, r8
	bl MI_CpuCopy8
	mov r0, r7
	mov r1, r5
	mov r2, r8
	bl MI_CpuCopy8
	mov r0, sb
	mov r1, sl
	bl CPSi_big_sign
	cmp r0, #0
	bgt _020AD3A4
_020AD440:
	ldr r2, [sp, #0xc]
	mov r0, r6
	mov r1, r6
	mov r3, sl
	bl CPSi_big_add
	ldr r2, [sp, #0xc]
	ldr r3, [sp, #8]
	ldr r4, [sp, #0x10]
	str sl, [sp]
	mov r1, r6
	mov r0, #0
	str r4, [sp, #4]
	bl CPSi_big_div
	add sp, sp, #0x14
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	arm_func_end CPSi_big_modinv
