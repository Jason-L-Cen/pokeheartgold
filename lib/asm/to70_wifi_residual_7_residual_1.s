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
	.include "asm/macros.inc"
	.include "wifi.inc"
	.include "global.inc"
	.extern SOCLi_FreeCommandPipe

.text
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
