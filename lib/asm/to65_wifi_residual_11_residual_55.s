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
	.public WCM_SendDCFDataEx
	.extern WCMi_GetSystemWork
	.extern WcmCpsifWmCallback

.text
	arm_func_start WCM_SendDCFDataEx
WCM_SendDCFDataEx: ; 0x020AFFE4
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, lr}
	mov r4, r0
	mov sb, r1
	mov r8, r2
	mov r6, r3
	bl OS_DisableInterrupts
	mov r5, r0
	bl WCMi_GetSystemWork
	cmp r0, #0
	bne _020B001C
	mov r0, r5
	bl OS_RestoreInterrupts
	mvn r0, #0
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
_020B001C:
	ldr r0, _020B015C ; =_021D5FFC
	bl OS_LockMutex
	bl WCMi_GetSystemWork
	movs r7, r0
	bne _020B0048
	ldr r0, _020B015C ; =_021D5FFC
	bl OS_UnlockMutex
	mov r0, r5
	bl OS_RestoreInterrupts
	mvn r0, #0
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
_020B0048:
	add r0, r7, #0x2000
	ldr r1, [r0, #0x260]
	cmp r1, #9
	bne _020B0064
	ldrb r0, [r0, #0x26b]
	cmp r0, #1
	bne _020B007C
_020B0064:
	ldr r0, _020B015C ; =_021D5FFC
	bl OS_UnlockMutex
	mov r0, r5
	bl OS_RestoreInterrupts
	mvn r0, #3
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
_020B007C:
	mov r0, sb
	mov r2, r8
	add r1, r7, #0xf00
	bl MI_CpuCopy8
	ldr r2, [sp, #0x20]
	cmp r2, #0
	ble _020B00A8
	add r1, r7, #0xf00
	mov r0, r6
	add r1, r1, r8
	bl MI_CpuCopy8
_020B00A8:
	ldr r1, [sp, #0x20]
	ldr r0, _020B0160 ; =WcmCpsifWmCallback
	add r6, r8, r1
	mov r3, r6, lsl #0x10
	mov r1, r4
	add r2, r7, #0xf00
	mov r3, r3, lsr #0x10
	bl WM_SetDCFData
	cmp r0, #8
	addls pc, pc, r0, lsl #2
	b _020B00F8
_020B00D4: ; jump table
	b _020B00F8 ; case 0
	b _020B00F8 ; case 1
	b _020B0110 ; case 2
	b _020B00F8 ; case 3
	b _020B00F8 ; case 4
	b _020B00F8 ; case 5
	b _020B00F8 ; case 6
	b _020B00F8 ; case 7
	b _020B00F8 ; case 8
_020B00F8:
	ldr r0, _020B015C ; =_021D5FFC
	bl OS_UnlockMutex
	mov r0, r5
	bl OS_RestoreInterrupts
	mvn r0, #4
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
_020B0110:
	ldr r0, _020B0164 ; =_021D5FF4
	bl OS_SleepThread
	ldr r0, _020B0168 ; =_021D5FF0
	ldr r0, [r0, #0x24]
	cmp r0, #0
	beq _020B0144
	cmp r0, #1
	ldr r0, _020B015C ; =_021D5FFC
	bl OS_UnlockMutex
	mov r0, r5
	bl OS_RestoreInterrupts
	mvn r0, #4
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
_020B0144:
	ldr r0, _020B015C ; =_021D5FFC
	bl OS_UnlockMutex
	mov r0, r5
	bl OS_RestoreInterrupts
	mov r0, r6
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	.align 2, 0
_020B015C: .word _021D5FFC
_020B0160: .word WcmCpsifWmCallback
_020B0164: .word _021D5FF4
_020B0168: .word _021D5FF0
	arm_func_end WCM_SendDCFDataEx
