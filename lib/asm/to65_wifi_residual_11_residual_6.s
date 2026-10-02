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
	.public put_in_buffer

.text
	arm_func_start put_in_buffer
put_in_buffer: ; 0x020A3FC0
	stmdb sp!, {r4, r5, r6, r7, r8, lr}
	ldr r4, _020A41A8 ; =_021D4620
	mov r7, r0
	ldr ip, [r4, #0x58]
	mov r6, r2
	cmp ip, #0
	ldrne r0, [r4, #0x5c]
	mov r5, r3
	cmpne r0, #0
	ldmeqia sp!, {r4, r5, r6, r7, r8, pc}
	ldr r0, [sp, #0x1c]
	add r0, r5, r0
	cmp r0, #8
	ldmloia sp!, {r4, r5, r6, r7, r8, pc}
	ldr r2, _020A41AC ; =0x000005E4
	cmp r0, r2
	ldmhiia sp!, {r4, r5, r6, r7, r8, pc}
	ldr r2, _020A41B0 ; =_02110690
	ldrb r4, [r6]
	ldrb r3, [r2]
	cmp r4, r3
	ldreqb r4, [r6, #1]
	ldreqb r3, [r2, #1]
	cmpeq r4, r3
	ldreqb r3, [r6, #2]
	ldreqb r2, [r2, #2]
	cmpeq r3, r2
	ldreqb r2, [r6, #6]
	cmpeq r2, #8
	ldmneia sp!, {r4, r5, r6, r7, r8, pc}
	ldrb r2, [r6, #7]
	cmp r2, #0
	cmpne r2, #6
	ldmneia sp!, {r4, r5, r6, r7, r8, pc}
	ldr r2, _020A41A8 ; =_021D4620
	add r0, r0, #9
	bic r0, r0, #1
	ldr r8, [r2, #0x28]
	mov r0, r0, lsl #0x10
	ldr lr, [r2, #0x28]
	ldr r4, [r2, #0x30]
	mov r3, r0, lsr #0x10
	cmp lr, r4
	add r4, r8, r0, lsr #16
	bhs _020A4080
	ldr r0, [r2, #0x30]
	cmp r0, r4
	ldmlsia sp!, {r4, r5, r6, r7, r8, pc}
_020A4080:
	ldr r0, _020A41A8 ; =_021D4620
	ldr r2, [r0, #0x5c]
	cmp r4, r2
	bne _020A40A4
	ldr r0, [r0, #0x30]
	mov r4, #0
	cmp r0, #0
	bne _020A40C0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_020A40A4:
	ldr r2, [r0, #0x5c]
	cmp r4, r2
	bls _020A40C0
	ldr r0, [r0, #0x30]
	mov r4, r3
	cmp r0, r3
	ldmlsia sp!, {r4, r5, r6, r7, r8, pc}
_020A40C0:
	ldr r0, _020A41A8 ; =_021D4620
	ldr lr, [r0, #0x28]
	ldr r2, [r0, #0x5c]
	add lr, lr, r3
	cmp lr, r2
	bls _020A4100
	ldr lr, [r0, #0x5c]
	ldr r2, [r0, #0x28]
	sub r2, lr, r2
	cmp r2, #2
	ldrhs r0, [r0, #0x28]
	movhs r2, #0
	strhsh r2, [ip, r0]
	ldr r0, _020A41A8 ; =_021D4620
	mov r2, #0
	str r2, [r0, #0x28]
_020A4100:
	ldr ip, _020A41A8 ; =_021D4620
	mov r0, r1
	ldr lr, [ip, #0x58]
	ldr r1, [ip, #0x28]
	mov r2, #6
	strh r3, [lr, r1]
	ldr r3, [ip, #0x58]
	ldr r1, [ip, #0x28]
	add r1, r3, r1
	add r1, r1, #2
	bl MI_CpuCopy8
	ldr r1, _020A41A8 ; =_021D4620
	mov r0, r7
	ldr r3, [r1, #0x58]
	ldr r1, [r1, #0x28]
	mov r2, #6
	add r1, r3, r1
	add r1, r1, #8
	bl MI_CpuCopy8
	ldr r1, _020A41A8 ; =_021D4620
	add r0, r6, #6
	ldr r3, [r1, #0x58]
	ldr r1, [r1, #0x28]
	sub r2, r5, #6
	add r1, r3, r1
	add r1, r1, #0xe
	bl MI_CpuCopy8
	ldr r0, [sp, #0x18]
	cmp r0, #0
	ldrne r2, [sp, #0x1c]
	cmpne r2, #0
	beq _020A419C
	ldr r1, _020A41A8 ; =_021D4620
	ldr r3, [r1, #0x58]
	ldr r1, [r1, #0x28]
	add r1, r3, r1
	add r1, r1, #8
	add r1, r1, r5
	bl MI_CpuCopy8
_020A419C:
	ldr r0, _020A41A8 ; =_021D4620
	str r4, [r0, #0x28]
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	.align 2, 0
_020A41A8: .word _021D4620
_020A41AC: .word 0x000005E4
_020A41B0: .word _02110690
	arm_func_end put_in_buffer
