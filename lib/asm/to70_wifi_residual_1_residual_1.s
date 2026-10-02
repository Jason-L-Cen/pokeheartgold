	.extern CPS_SetScavengerCallback
	.extern CPS_SetThreadPriority
	.extern CPS_Startup
	.extern CPSi_RecvCallbackFunc
	.extern SOCL_CreateSocket
	.extern SOCLi_StartupCommandPacketQueue
	.extern SOCLi_TrashSocket
	.extern WCM_GetApMacAddress
	.extern WCM_SetRecvDCFCallback
	.extern _02110628
	.extern _02110648
	.extern _02110660
	.extern _021D4420
	.extern _021D4430
	.extern _021D463C
	.extern _021D464C
	.extern _021D4670
	.extern _021D4680
	.include "asm/macros.inc"
	.include "wifi.inc"
	.include "global.inc"
	.public SOCLi_StartupCPS
	.extern SOCL_LinkIsOn
	.extern SOCLi_DhcpTimeout
	.extern SOCLi_SetMyIP

.text
	arm_func_start SOCLi_StartupCPS
SOCLi_StartupCPS: ; 0x020A0880
	stmdb sp!, {r3, r4, r5, lr}
	ldr r0, _020A09B4 ; =_02110628
	ldr r4, _020A09B8 ; =_021D4430
	ldr r5, [r0]
	mov r0, r4
	mov r1, #0
	mov r2, #0x30
	bl MI_CpuFill8
	ldr r1, [r5, #0x18]
	ldr r0, _020A09BC ; =_021D4420
	ldr r2, _020A09C0 ; =SOCL_LinkIsOn
	str r1, [r0, #0x14]
	ldr r3, [r5, #0x1c]
	mov r1, #0
	str r3, [r0, #0x18]
	str r2, [r0, #0x20]
	str r1, [r0, #0x24]
	str r1, [r0, #0x28]
	ldr r1, [r0]
	str r1, [r0, #0x3c]
	ldr r0, [r5, #0x24]
	cmp r0, #0
	moveq r0, #0x4000
	str r0, [r4, #0x20]
	ldr r0, [r5, #0x28]
	cmp r0, #0
	bne _020A0900
	ldr r1, _020A09B4 ; =_02110628
	ldr r0, [r4, #0x20]
	ldr r1, [r1]
	ldr r1, [r1, #0x18]
	blx r1
_020A0900:
	str r0, [r4, #0x1c]
	ldr r1, [r5, #0x30]
	ldr lr, [r5, #0x34]
	cmp r1, #0
	moveq r1, #0x240
	sub ip, r1, #0x28
	cmp lr, #0
	moveq lr, #0x10c0
	ldr r2, _020A09C4 ; =_02110648
	add r0, lr, lr, lsr #31
	ldr r1, _020A09C8 ; =_021D4670
	mov r3, #0
	strh lr, [r2, #2]
	mov r0, r0, asr #1
	strh r0, [r2, #4]
	str ip, [r4, #0x24]
	str r3, [r1]
	ldr r0, [r5]
	mov r2, #1
	cmp r0, #0
	beq _020A0974
	ldr r1, _020A09CC ; =SOCLi_DhcpTimeout
	ldr r0, _020A09BC ; =_021D4420
	str r3, [r4]
	str r1, [r4, #0xc]
	str r2, [r0, #8]
	ldr r0, [r0, #4]
	str r0, [r4, #0x28]
	b _020A0988
_020A0974:
	ldr r0, _020A09BC ; =_021D4420
	ldr r1, _020A09D0 ; =SOCLi_SetMyIP
	str r3, [r0, #8]
	str r2, [r4]
	str r1, [r4, #0xc]
_020A0988:
	ldr r0, [r5, #0x2c]
	cmp r0, #0
	moveq r0, #0xb
	bl CPS_SetThreadPriority
	ldr r0, _020A09D4 ; =CPSi_RecvCallbackFunc
	bl WCM_SetRecvDCFCallback
	ldr r0, _020A09D8 ; =SOCLi_TrashSocket
	bl CPS_SetScavengerCallback
	mov r0, r4
	bl CPS_Startup
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_020A09B4: .word _02110628
_020A09B8: .word _021D4430
_020A09BC: .word _021D4420
_020A09C0: .word SOCL_LinkIsOn
_020A09C4: .word _02110648
_020A09C8: .word _021D4670
_020A09CC: .word SOCLi_DhcpTimeout
_020A09D0: .word SOCLi_SetMyIP
_020A09D4: .word CPSi_RecvCallbackFunc
_020A09D8: .word SOCLi_TrashSocket
	arm_func_end SOCLi_StartupCPS
