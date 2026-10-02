	.extern CPS_SocBind
	.extern CPS_SocGetEport
	.extern CPS_SocWrite
	.extern CPS_TcpClose
	.extern SOCL_SocketIsInvalid
	.extern SOCLi_CreateCommandPacket
	.extern SOCLi_ExecCommandPacketInSendPipe
	.extern SOCLi_GetOptimumSendBufLen
	.extern SOCLi_MemCpy
	.extern _02110628
	.extern _021D4484
	.include "asm/macros.inc"
	.include "wifi.inc"
	.include "global.inc"

.text
	arm_func_start SOCLi_ReadUdpBuffer
SOCLi_ReadUdpBuffer: ; 0x020A1BCC
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0xc
	mov sl, r0
	ldr r6, [sl, #0x64]
	str r1, [sp]
	ldr r0, [r6, #0x104]
	str r2, [sp, #4]
	mov fp, r3
	bl OS_DisableInterrupts
	ldr r7, [r6, #0x104]
	str r0, [sp, #8]
	cmp r7, #0
	bne _020A1C68
	ldr r0, [sp, #0x34]
	mov r4, #1
	and sb, r0, #1
	mov r5, #0
_020A1C10:
	cmp sb, #0
	mvneq r8, #5
	beq _020A1C68
	add r0, r6, #0x10c
	bl OS_SleepThread
	mov r0, sl
	bl SOCL_SocketIsInvalid
	cmp r0, #0
	bne _020A1C54
	mov r1, r5
	cmp sl, #0
	beq _020A1C4C
	ldrsh r0, [sl, #0x70]
	tst r0, #1
	movne r1, r4
_020A1C4C:
	cmp r1, #0
	bne _020A1C5C
_020A1C54:
	mvn r8, #0x37
	b _020A1C68
_020A1C5C:
	ldr r7, [r6, #0x104]
	cmp r7, #0
	beq _020A1C10
_020A1C68:
	cmp r7, #0
	beq _020A1CF8
	ldrh r1, [r7, #4]
	ldr r0, [sp, #4]
	cmp r0, r1
	strgt r1, [sp, #4]
	ldr r1, [sp]
	ldr r2, [sp, #4]
	add r0, r7, #0xc
	bl MI_CpuCopy8
	cmp fp, #0
	ldrneh r0, [r7, #6]
	ldr r1, [sp, #0x30]
	strneh r0, [fp]
	cmp r1, #0
	ldrne r0, [r7, #8]
	strne r0, [r1]
	ldrsb r0, [r6, #0xfe]
	ldrh r8, [r7, #4]
	cmp r0, #0
	bne _020A1CF8
	ldr r0, [r7]
	ldr r1, _020A1D0C ; =_02110628
	str r0, [r6, #0x104]
	ldr r0, [r7]
	cmp r0, #0
	moveq r0, #0
	streq r0, [r6, #0x100]
	ldr r1, [r1]
	mov r0, r7
	ldr r1, [r1, #0x1c]
	blx r1
	add r0, r6, #0x100
	ldrh r1, [r0, #8]
	sub r1, r1, r8
	strh r1, [r0, #8]
_020A1CF8:
	ldr r0, [sp, #8]
	bl OS_RestoreInterrupts
	mov r0, r8
	add sp, sp, #0xc
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_020A1D0C: .word _02110628
	arm_func_end SOCLi_ReadUdpBuffer
