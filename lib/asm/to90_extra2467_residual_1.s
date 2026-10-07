	.extern CPS_SocBind
	.extern CPS_SocGetEport
	.extern CPS_SocWrite
	.extern CPS_TcpClose
	.extern SOCL_SocketIsInvalid
	.extern SOCLi_CreateCommandPacket
	.extern SOCLi_ExecCommandPacketInSendPipe
	.extern SOCLi_GetOptimumSendBufLen
	.extern SOCLi_MemCpy
	.extern SOCLi_WriteCallBack
	.extern _02110628
	.extern _021D4484
	.include "asm/macros.inc"
	.include "wifi.inc"
	.include "global.inc"

.text
	arm_func_start SOCLi_GetWriteBufferFreeSize
SOCLi_GetWriteBufferFreeSize: ; 0x020A20C0
	ldr r2, [r0, #0x68]
	add r0, r2, #0x100
	ldrh r1, [r0]
	ldrh r0, [r0, #2]
	ldr r2, [r2, #0xf8]
	sub r0, r0, r1
	subs r0, r0, #1
	addmi r0, r0, r2
	bx lr
	arm_func_end SOCLi_GetWriteBufferFreeSize
