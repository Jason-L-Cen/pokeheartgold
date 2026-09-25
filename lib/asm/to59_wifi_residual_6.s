	.include "asm/macros.inc"
	.include "wifi.inc"
	.include "global.inc"
	.public SOCL_Shutdown
	.extern SOCL_SocketIsInvalid
	.extern SOCLi_CreateCommandPacket
	.extern SOCLi_ExecCommandPacketInSendPipe
	.extern SOCLi_ShutdownCallBack

.text
	arm_func_start SOCL_Shutdown
SOCL_Shutdown: ; 0x020A24C4
	stmdb sp!, {r3, r4, r5, lr}
	mov r4, r0
	bl SOCL_SocketIsInvalid
	cmp r0, #0
	mvnne r0, #0x1b
	ldmneia sp!, {r3, r4, r5, pc}
	cmp r4, #0
	mov r1, #0
	beq _020A24F4
	ldrsh r0, [r4, #0x70]
	tst r0, #1
	movne r1, #1
_020A24F4:
	cmp r1, #0
	mvneq r0, #0x26
	ldmeqia sp!, {r3, r4, r5, pc}
	ldrsh r0, [r4, #0x70]
	tst r0, #4
	beq _020A2518
	ldrsh r0, [r4, #0x70]
	tst r0, #8
	beq _020A2520
_020A2518:
	mvn r0, #0x37
	ldmia sp!, {r3, r4, r5, pc}
_020A2520:
	ldrsh r0, [r4, #0x70]
	orr r0, r0, #8
	strh r0, [r4, #0x70]
	ldr r5, [r4, #0x68]
	cmp r5, #0
	ldrne r1, [r5, #0x10c]
	cmpne r1, #0
	beq _020A2564
	ldrsb r2, [r4, #0x72]
	ldr r0, _020A256C ; =SOCLi_ShutdownCallBack
	bl SOCLi_CreateCommandPacket
	movs r1, r0
	mvneq r0, #0x20
	ldmeqia sp!, {r3, r4, r5, pc}
	ldr r0, [r5, #0x10c]
	bl SOCLi_ExecCommandPacketInSendPipe
	ldmia sp!, {r3, r4, r5, pc}
_020A2564:
	mov r0, #0
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_020A256C: .word SOCLi_ShutdownCallBack
	arm_func_end SOCL_Shutdown
