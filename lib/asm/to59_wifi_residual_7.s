	.include "asm/macros.inc"
	.include "wifi.inc"
	.include "global.inc"
	.public SOCL_Close
	.public SOCLi_CleanupSocket
	.extern CPS_SocRelease
	.extern CPS_SocUnRegister
	.extern CPS_TcpClose
	.extern CPS_TcpShutdown
	.extern SOCL_SocketIsInTrash
	.extern SOCL_SocketIsInvalid
	.extern SOCLi_CreateCommandPacket
	.extern SOCLi_FreeCommandPacket
	.extern SOCLi_SendCommandPacket
	.extern SOCLi_SendCommandPacketToCtrlPipe
	.extern SOCLi_SocketRegisterTrash
	.extern SOCLi_SocketUnregister
	.extern SOCLi_SocketUnregisterTrash
	.extern _02110628

.text
	arm_func_start SOCL_Close
SOCL_Close: ; 0x020A25D4
	stmdb sp!, {r4, lr}
	mov r4, r0
	cmp r4, #0
	mvnle r0, #0x1b
	ldmleia sp!, {r4, pc}
	bl SOCL_SocketIsInTrash
	cmp r0, #0
	mvnne r0, #0x19
	ldmneia sp!, {r4, pc}
	mov r0, r4
	bl SOCL_SocketIsInvalid
	cmp r0, #0
	movne r0, #0
	ldmneia sp!, {r4, pc}
	cmp r4, #0
	mov r1, #0
	beq _020A2624
	ldrsh r0, [r4, #0x70]
	tst r0, #1
	movne r1, #1
_020A2624:
	cmp r1, #0
	mvneq r0, #0x26
	ldmeqia sp!, {r4, pc}
	ldrsh r0, [r4, #0x70]
	tst r0, #0x10
	mvnne r0, #0x19
	ldmneia sp!, {r4, pc}
	ldrsh r0, [r4, #0x70]
	mov r1, #1
	orr r0, r0, #0x18
	strh r0, [r4, #0x70]
	ldrsb r0, [r4, #0x73]
	cmp r0, #0
	cmpne r0, #4
	movne r1, #0
	cmp r1, #0
	beq _020A2674
	ldr r0, [r4, #0x68]
	mov r1, #0
	bl SOCLi_SendCommandPacket
_020A2674:
	ldr r0, _020A26A0 ; =SOCLi_CloseCallBack
	mov r1, r4
	mov r2, #1
	bl SOCLi_CreateCommandPacket
	mov r1, r0
	mov r2, #0
	mov r0, r4
	str r2, [r1, #8]
	bl SOCLi_SendCommandPacketToCtrlPipe
	mov r0, #0
	ldmia sp!, {r4, pc}
	.align 2, 0
_020A26A0: .word SOCLi_CloseCallBack
	arm_func_end SOCL_Close

	arm_func_start SOCLi_CloseCallBack
SOCLi_CloseCallBack: ; 0x020A26A4
	stmdb sp!, {r3, r4, r5, lr}
	ldr r4, [r0, #4]
	mov r1, #1
	ldrsb r0, [r4, #0x73]
	cmp r0, #0
	cmpne r0, #4
	movne r1, #0
	cmp r1, #0
	beq _020A26E0
	ldr r0, [r4, #0x68]
	add r0, r0, #0x20
	bl OS_JoinThread
	bl CPS_TcpShutdown
	bl CPS_TcpClose
	bl CPS_SocRelease
_020A26E0:
	bl CPS_SocUnRegister
	ldrsh r0, [r4, #0x70]
	mov r1, #0
	bic r0, r0, #6
	strh r0, [r4, #0x70]
	ldrsb r0, [r4, #0x73]
	cmp r0, #2
	ldreq r0, [r4, #0x68]
	ldrne r0, [r4, #0x64]
	bl SOCLi_SendCommandPacket
	bl OS_DisableInterrupts
	mov r5, r0
	mov r0, r4
	bl SOCLi_SocketUnregister
	mov r0, r4
	bl SOCLi_SocketRegisterTrash
	mov r0, r5
	bl OS_RestoreInterrupts
	ldrsh r1, [r4, #0x70]
	mov r0, #0
	orr r1, r1, #0x20
	strh r1, [r4, #0x70]
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end SOCLi_CloseCallBack

	arm_func_start SOCLi_CleanupSocket
SOCLi_CleanupSocket: ; 0x020A273C
	stmdb sp!, {r4, r5, r6, lr}
	movs r4, r0
	ldmeqia sp!, {r4, r5, r6, pc}
	mov r0, #0
	strh r0, [r4, #0x70]
	ldrsb r2, [r4, #0x73]
	mov r1, #1
	cmp r2, #0
	cmpne r2, #4
	movne r1, r0
	cmp r1, #0
	beq _020A2780
	ldr r0, [r4, #0x68]
	bl SOCLi_FreeCommandPipe
	ldr r0, [r4, #0x64]
	bl SOCLi_FreeCommandPipe
	b _020A2800
_020A2780:
	cmp r2, #1
	bne _020A27F0
	ldr r0, [r4, #0x64]
	ldr r0, [r0, #0x104]
	cmp r0, #0
	beq _020A27B8
	ldr r5, _020A2838 ; =_02110628
_020A279C:
	ldr r1, [r5]
	ldr r6, [r0]
	ldr r1, [r1, #0x1c]
	blx r1
	mov r0, r6
	cmp r6, #0
	bne _020A279C
_020A27B8:
	ldr r0, [r4, #0x64]
	mov r1, #0
	add r0, r0, #0x100
	strh r1, [r0, #8]
	ldr r0, [r4, #0x64]
	str r1, [r0, #0x100]
	ldr r0, [r4, #0x64]
	str r1, [r0, #0x104]
	ldr r0, [r4, #0x64]
	add r0, r0, #0x10c
	bl OS_WakeupThread
	ldr r0, [r4, #0x64]
	bl SOCLi_FreeCommandPipe
	b _020A2800
_020A27F0:
	cmp r2, #2
	bne _020A2800
	ldr r0, [r4, #0x68]
	bl SOCLi_FreeCommandPipe
_020A2800:
	bl OS_DisableInterrupts
	mov r5, r0
	mov r0, r4
	bl SOCLi_SocketUnregister
	mov r0, r4
	bl SOCLi_SocketUnregisterTrash
	ldr r1, _020A2838 ; =_02110628
	mov r0, r4
	ldr r1, [r1]
	ldr r1, [r1, #0x1c]
	blx r1
	mov r0, r5
	bl OS_RestoreInterrupts
	ldmia sp!, {r4, r5, r6, pc}
	.align 2, 0
_020A2838: .word _02110628
	arm_func_end SOCLi_CleanupSocket

	arm_func_start SOCLi_FreeCommandPipe
SOCLi_FreeCommandPipe: ; 0x020A283C
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, lr}
	movs sb, r0
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	add r0, sb, #0x20
	bl OS_JoinThread
	bl OS_DisableInterrupts
	mov r8, r0
	bl OS_DisableScheduler
	add r1, sp, #0
	mov r0, sb
	mov r2, #0
	bl OS_ReceiveMessage
	cmp r0, #0
	beq _020A28C8
	mov r6, #0
	mvn r7, #0xa
	add r5, sp, #0
	mov r4, r6
_020A2884:
	ldr r0, [sp]
	cmp r0, #0
	beq _020A28B0
	ldr r0, [r0, #8]
	cmp r0, #0
	beq _020A28A8
	mov r1, r7
	mov r2, r6
	bl OS_SendMessage
_020A28A8:
	ldr r0, [sp]
	bl SOCLi_FreeCommandPacket
_020A28B0:
	mov r0, sb
	mov r1, r5
	mov r2, r4
	bl OS_ReceiveMessage
	cmp r0, #0
	bne _020A2884
_020A28C8:
	bl OS_EnableScheduler
	bl OS_RescheduleThread
	mov r0, r8
	bl OS_RestoreInterrupts
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	arm_func_end SOCLi_FreeCommandPipe
