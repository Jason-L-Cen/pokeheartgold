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
	.extern SOCLi_AllocWriteBuffer
	.extern SOCLi_ExecWriteCommand

.text
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
