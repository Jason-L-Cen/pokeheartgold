	.include "asm/macros.inc"
	.include "wifi.inc"
	.include "global.inc"
	.public SOCL_Startup
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

.text
	arm_func_start SOCL_Startup
SOCL_Startup: ; 0x020A0804
	stmdb sp!, {r4, lr}
	mov r4, r0
	ldr r0, _020A0838 ; =_version_NINTENDO_WiFi
	bl OSi_ReferSymbol
	ldr r0, _020A083C ; =_02110628
	ldr r1, [r0]
	cmp r1, #0
	movne r0, #0
	ldmneia sp!, {r4, pc}
	str r4, [r0]
	bl SOCLi_StartupCPS
	bl SOCLi_StartupSOCL
	ldmia sp!, {r4, pc}
	.align 2, 0
_020A0838: .word _version_NINTENDO_WiFi
_020A083C: .word _02110628
	arm_func_end SOCL_Startup

	arm_func_start SOCLi_StartupSOCL
SOCLi_StartupSOCL: ; 0x020A0840
	stmdb sp!, {r4, lr}
	ldr r0, _020A0874 ; =_02110628
	ldr r0, [r0]
	ldr r0, [r0, #0x20]
	bl SOCLi_StartupCommandPacketQueue
	movs r4, r0
	bmi _020A086C
	ldr r0, _020A0878 ; =_02110660
	bl SOCL_CreateSocket
	ldr r1, _020A087C ; =_021D4420
	str r0, [r1, #0xc]
_020A086C:
	mov r0, r4
	ldmia sp!, {r4, pc}
	.align 2, 0
_020A0874: .word _02110628
_020A0878: .word _02110660
_020A087C: .word _021D4420
	arm_func_end SOCLi_StartupSOCL

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

	arm_func_start SOCLi_SetMyIP
SOCLi_SetMyIP: ; 0x020A09DC
	ldr r0, _020A0A30 ; =_02110628
	ldr r1, _020A0A34 ; =_021D4670
	ldr ip, [r0]
	ldr r0, _020A0A38 ; =_021D463C
	ldr r3, [ip, #4]
	ldr r2, _020A0A3C ; =_021D464C
	str r3, [r1]
	ldr r3, [ip, #8]
	ldr r1, _020A0A40 ; =_021D4680
	str r3, [r0]
	ldr r3, [ip, #0xc]
	ldr r0, _020A0A44 ; =_021D4420
	str r3, [r2]
	ldr r2, [ip, #0x10]
	str r2, [r1]
	ldr r2, [ip, #0x14]
	str r2, [r1, #4]
	ldr r1, [r0, #8]
	orr r1, r1, #2
	str r1, [r0, #8]
	bx lr
	.align 2, 0
_020A0A30: .word _02110628
_020A0A34: .word _021D4670
_020A0A38: .word _021D463C
_020A0A3C: .word _021D464C
_020A0A40: .word _021D4680
_020A0A44: .word _021D4420
	arm_func_end SOCLi_SetMyIP

	arm_func_start SOCLi_DhcpTimeout
SOCLi_DhcpTimeout: ; 0x020A0A48
	ldr r0, _020A0A5C ; =_021D4420
	ldr r1, [r0, #8]
	orr r1, r1, #2
	str r1, [r0, #8]
	bx lr
	.align 2, 0
_020A0A5C: .word _021D4420
	arm_func_end SOCLi_DhcpTimeout

	arm_func_start SOCL_LinkIsOn
SOCL_LinkIsOn: ; 0x020A0A60
	stmdb sp!, {r3, lr}
	bl WCM_GetApMacAddress
	cmp r0, #0
	movne r0, #1
	moveq r0, #0
	ldmia sp!, {r3, pc}
	arm_func_end SOCL_LinkIsOn
