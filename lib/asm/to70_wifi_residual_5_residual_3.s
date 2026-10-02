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
	.public SOCLi_ExecWriteCommand
	.public SOCLi_GetWriteBufferFreeSize

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

	arm_func_start SOCLi_ExecWriteCommand
SOCLi_ExecWriteCommand: ; 0x020A20E4
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, lr}
	mov sb, r0
	ldr r4, [sb, #0x68]
	mov r6, r1
	mov r8, r2
	ldr r1, [r4, #0x10c]
	ldr r2, [sp, #0x28]
	ldr r0, _020A2254 ; =SOCLi_WriteCallBack
	mov r7, r3
	bl SOCLi_CreateCommandPacket
	movs r5, r0
	mvneq r0, #0x20
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	ldr r0, [sp, #0x28]
	add r1, r7, r8
	tst r0, #1
	ldrnesb r0, [sb, #0x73]
	cmpne r0, #1
	movne r0, #3
	strneb r0, [r5, #0xd]
	moveq r0, #0
	streqb r0, [r5, #0xd]
	ldr r3, [r4, #0xfc]
	ldr r2, [r4, #0xf8]
	add r0, r3, r7
	cmp r1, r2
	str r0, [r5, #0x10]
	bge _020A216C
	str r8, [r5, #0x14]
	mov r0, #0
	str r0, [r5, #0x18]
	mov r7, r1
	str r0, [r5, #0x1c]
	b _020A2198
_020A216C:
	sub r0, r2, r7
	str r0, [r5, #0x14]
	str r3, [r5, #0x18]
	ldr r0, [r5, #0x14]
	sub r7, r8, r0
	str r7, [r5, #0x1c]
	ldr r0, [r5, #0x14]
	ldr r1, [r5, #0x18]
	mov r2, r7
	add r0, r6, r0
	bl MI_CpuCopy8
_020A2198:
	ldr r1, [r5, #0x10]
	ldr r2, [r5, #0x14]
	mov r0, r6
	bl MI_CpuCopy8
	add r0, r4, #0x100
	ldrh r6, [r0]
	strh r7, [r5, #0x20]
	ldrh r1, [r5, #0x20]
	strh r1, [r0]
	ldrsb r0, [sb, #0x73]
	cmp r0, #1
	bne _020A2228
	ldrh r0, [sb, #0x74]
	cmp r0, #0
	bne _020A21E4
	bl CPS_SocGetEport
	strh r0, [sb, #0x74]
	ldrh r0, [sb, #0x74]
	strh r0, [sb, #0xa]
_020A21E4:
	ldrh r0, [sb, #0x74]
	strh r0, [r5, #0x24]
	ldr r1, [sb, #0x78]
	cmp r1, #0
	beq _020A2204
	ldr r0, [sp, #0x24]
	cmp r0, #0
	beq _020A2218
_020A2204:
	ldr r1, [sp, #0x24]
	ldrh r0, [sp, #0x20]
	str r1, [r5, #0x28]
	strh r0, [r5, #0x26]
	b _020A2230
_020A2218:
	str r1, [r5, #0x28]
	ldrh r0, [sb, #0x76]
	strh r0, [r5, #0x26]
	b _020A2230
_020A2228:
	mov r0, #0
	str r0, [r5, #0x28]
_020A2230:
	ldr r0, [r4, #0x10c]
	mov r1, r5
	bl SOCLi_ExecCommandPacketInSendPipe
	cmp r0, #0
	addne r0, r4, #0x100
	movne r8, #0
	strneh r6, [r0]
	mov r0, r8
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	.align 2, 0
_020A2254: .word SOCLi_WriteCallBack
	arm_func_end SOCLi_ExecWriteCommand

	arm_func_start SOCLi_WriteCallBack
SOCLi_WriteCallBack: ; 0x020A2258
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, lr}
	mov sb, r0
	ldr r5, [sb, #4]
	mov r7, #0
	ldrsb r0, [r5, #0x73]
	mov r1, #1
	ldr r6, [r5, #0x68]
	cmp r0, #0
	cmpne r0, #4
	movne r1, r7
	cmp r1, #0
	beq _020A2294
	ldrsh r0, [r5, #0x70]
	tst r0, #4
	beq _020A2368
_020A2294:
	ldr r2, [sb, #0x28]
	cmp r2, #0
	beq _020A22AC
	ldrh r0, [sb, #0x24]
	ldrh r1, [sb, #0x26]
	bl CPS_SocBind
_020A22AC:
	ldrsb r1, [r5, #0x73]
	mov r0, #1
	ldr r4, [r5, #0x4c]
	cmp r1, #0
	cmpne r1, #4
	movne r0, #0
	cmp r0, #0
	movne r8, #0x36
	moveq r8, #0x2a
	cmp r1, #0
	cmpne r1, #4
	ldrne r0, [r5, #0x48]
	subne sl, r0, r8
	bne _020A22F4
	ldr r0, [r5, #0x48]
	sub r0, r0, r8
	bl SOCLi_GetOptimumSendBufLen
	mov sl, r0
_020A22F4:
	mov r1, sl
	mov r2, sb
	add r0, r4, r8
	bl SOCLi_MemCpy
	mov r1, r0
	cmp r1, #0
	ble _020A236C
	add r0, r4, r8
	bl CPS_SocWrite
	cmp r0, #0
	bgt _020A2360
	ldrsb r0, [r5, #0x73]
	mov r1, #1
	cmp r0, #0
	cmpne r0, #4
	movne r1, #0
	cmp r1, #0
	beq _020A2358
	ldrsh r0, [r5, #0x70]
	bic r0, r0, #0xe
	strh r0, [r5, #0x70]
	ldrsh r0, [r5, #0x70]
	orr r0, r0, #0x80
	strh r0, [r5, #0x70]
	bl CPS_TcpClose
_020A2358:
	mvn r7, #0x4b
	b _020A236C
_020A2360:
	add r7, r7, r0
	b _020A22F4
_020A2368:
	mvn r7, #0x4b
_020A236C:
	ldrh r2, [sb, #0x20]
	add r1, r6, #0x100
	add r0, r6, #0x104
	strh r2, [r1, #2]
	bl OS_WakeupThread
	mov r0, r7
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
	arm_func_end SOCLi_WriteCallBack
