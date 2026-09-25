	.include "asm/macros.inc"
	.include "wifi.inc"
	.include "global.inc"
	.public SOCL_WriteTo
	.public SOCLi_GetWriteBufferFreeSize
	.public SOCLi_ReadUdpBuffer
	.public SOCLi_UdpRecvCallback
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

	arm_func_start SOCLi_UdpRecvCallback
SOCLi_UdpRecvCallback: ; 0x020A1D10
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, lr}
	mov r7, r2
	ldr r4, [r7, #0x64]
	mov sb, r0
	mov r8, r1
	bl OS_DisableInterrupts
	add r1, r4, #0x100
	ldrh r2, [r1, #8]
	ldrh r1, [r1, #0xa]
	mov r6, r0
	add r0, r2, r8
	cmp r1, r0
	blo _020A1DD4
	ldr r1, _020A1E1C ; =_02110628
	add r0, r8, #0xc
	ldr r1, [r1]
	ldr r1, [r1, #0x18]
	blx r1
	movs r5, r0
	beq _020A1DC0
	add r1, r4, #0x100
	ldrh r3, [r1, #8]
	mov r2, #0
	mov r0, sb
	add r3, r3, r8
	strh r3, [r1, #8]
	str r2, [r5]
	strh r8, [r5, #4]
	ldrh r3, [r7, #0x18]
	mov r2, r8
	add r1, r5, #0xc
	strh r3, [r5, #6]
	ldr r3, [r7, #0x1c]
	str r3, [r5, #8]
	bl MI_CpuCopy8
	ldr r0, [r4, #0x100]
	cmp r0, #0
	ldrne r0, [r4, #0x100]
	strne r5, [r0]
	str r5, [r4, #0x100]
	ldr r0, [r4, #0x104]
	cmp r0, #0
	streq r5, [r4, #0x104]
	b _020A1DE4
_020A1DC0:
	ldr r0, _020A1E20 ; =_021D4484
	ldr r1, [r0]
	add r1, r1, #1
	str r1, [r0]
	b _020A1DE4
_020A1DD4:
	ldr r0, _020A1E20 ; =_021D4484
	ldr r1, [r0, #4]
	add r1, r1, #1
	str r1, [r0, #4]
_020A1DE4:
	ldrh r0, [r7, #0x74]
	cmp r0, #0
	ldreqh r0, [r7, #0xa]
	streqh r0, [r7, #0x74]
	ldrh r1, [r7, #0x1a]
	add r0, r4, #0x10c
	strh r1, [r7, #0x18]
	ldr r1, [r7, #0x20]
	str r1, [r7, #0x1c]
	bl OS_WakeupThread
	mov r0, r6
	bl OS_RestoreInterrupts
	mov r0, #1
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	.align 2, 0
_020A1E1C: .word _02110628
_020A1E20: .word _021D4484
	arm_func_end SOCLi_UdpRecvCallback

	arm_func_start SOCL_WriteTo
SOCL_WriteTo: ; 0x020A1E24
	stmdb sp!, {r4, r5, r6, r7, r8, lr}
	sub sp, sp, #8
	mov r8, r0
	mov r7, r1
	mov r6, r2
	mov r5, r3
	bl SOCL_SocketIsInvalid
	cmp r0, #0
	addne sp, sp, #8
	mvnne r0, #0x1b
	ldmneia sp!, {r4, r5, r6, r7, r8, pc}
	cmp r8, #0
	mov r1, #0
	beq _020A1E68
	ldrsh r0, [r8, #0x70]
	tst r0, #1
	movne r1, #1
_020A1E68:
	cmp r1, #0
	addeq sp, sp, #8
	mvneq r0, #0x26
	ldmeqia sp!, {r4, r5, r6, r7, r8, pc}
	ldrsb r0, [r8, #0x73]
	mov r1, #1
	cmp r0, #0
	cmpne r0, #4
	movne r1, #0
	cmp r1, #0
	beq _020A1EB8
	ldrsh r0, [r8, #0x70]
	tst r0, #4
	beq _020A1EAC
	ldrsh r0, [r8, #0x70]
	tst r0, #8
	beq _020A1EB8
_020A1EAC:
	add sp, sp, #8
	mvn r0, #0x37
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_020A1EB8:
	ldr r0, [sp, #0x24]
	ldr r4, [r8, #0x68]
	tst r0, #4
	bne _020A1ED4
	ldrsb r0, [r8, #0x72]
	cmp r0, #0
	bne _020A1EF4
_020A1ED4:
	add r0, r4, #0xe0
	bl OS_TryLockMutex
	cmp r0, #0
	addeq sp, sp, #8
	mvneq r0, #5
	ldmeqia sp!, {r4, r5, r6, r7, r8, pc}
	mov ip, #0
	b _020A1F00
_020A1EF4:
	add r0, r4, #0xe0
	bl OS_LockMutex
	mov ip, #1
_020A1F00:
	ldr r1, [sp, #0x20]
	mov r0, r8
	str r1, [sp]
	mov r1, r7
	mov r2, r6
	mov r3, r5
	str ip, [sp, #4]
	bl SOCLi_WriteBuffer
	mov r5, r0
	add r0, r4, #0xe0
	bl OS_UnlockMutex
	mov r0, r5
	add sp, sp, #8
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	arm_func_end SOCL_WriteTo

	arm_func_start SOCLi_WriteBuffer
SOCLi_WriteBuffer: ; 0x020A1F38
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x18
	mov sl, r0
	ldr r4, [sl, #0x68]
	ldrsb r0, [sl, #0x73]
	ldr r4, [r4, #0x10c]
	mov sb, r1
	str r3, [sp, #0xc]
	cmp r0, #1
	mov r8, r2
	ldr r7, [sp, #0x40]
	ldr r6, [sp, #0x44]
	ldr r0, [r4, #0x48]
	mov r5, #0
	bne _020A1F90
	sub r0, r0, #0x2a
	cmp r8, r0
	addgt sp, sp, #0x18
	subgt r0, r5, #0x23
	ldmgtia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	str r8, [sp, #0x10]
	b _020A1FA0
_020A1F90:
	sub r0, r0, #0x36
	str r0, [sp, #0x10]
	cmp r8, r0
	strle r8, [sp, #0x10]
_020A1FA0:
	cmp r8, #0
	ble _020A2030
	and fp, r6, #1
_020A1FAC:
	ldr r2, [sp, #0x10]
	mov r0, sl
	mov r1, r8
	add r3, sp, #0x14
	str r6, [sp]
	bl SOCLi_AllocWriteBuffer
	mov r4, r0
	cmp r4, #0
	ble _020A200C
	ldr r0, [sp, #0xc]
	mov r1, sb
	stmia sp, {r0, r7}
	str r6, [sp, #8]
	ldr r3, [sp, #0x14]
	mov r0, sl
	mov r2, r4
	bl SOCLi_ExecWriteCommand
	cmp r0, #0
	addle sp, sp, #0x18
	mvnle r0, #5
	ldmleia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	add sb, sb, r4
	sub r8, r8, r4
	add r5, r5, r4
_020A200C:
	cmp fp, #0
	bne _020A2028
	cmp r4, #0
	bgt _020A2030
	add sp, sp, #0x18
	mvn r0, #5
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020A2028:
	cmp r8, #0
	bgt _020A1FAC
_020A2030:
	mov r0, r5
	add sp, sp, #0x18
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	arm_func_end SOCLi_WriteBuffer

	arm_func_start SOCLi_AllocWriteBuffer
SOCLi_AllocWriteBuffer: ; 0x020A203C
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	mov sl, r0
	mov sb, r1
	mov r8, r2
	cmp r8, sb
	mov r7, r3
	ldr r4, [sl, #0x68]
	movgt r8, sb
	bl OS_DisableInterrupts
	ldr r1, [sp, #0x28]
	mov fp, r0
	and r6, r1, #1
_020A206C:
	mov r0, sl
	bl SOCLi_GetWriteBufferFreeSize
	mov r5, r0
	cmp r5, r8
	blt _020A2098
	add r0, r4, #0x100
	ldrh r0, [r0]
	cmp r5, sb
	movge r5, sb
	str r0, [r7]
	b _020A20B0
_020A2098:
	cmp r6, #0
	moveq r5, #0
	beq _020A20B0
	add r0, r4, #0x104
	bl OS_SleepThread
	b _020A206C
_020A20B0:
	mov r0, fp
	bl OS_RestoreInterrupts
	mov r0, r5
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	arm_func_end SOCLi_AllocWriteBuffer

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
