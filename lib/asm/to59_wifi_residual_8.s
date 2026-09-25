	.include "asm/macros.inc"
	.include "wifi.inc"
	.include "global.inc"
	.public SOCL_Cleanup
	.public SOCL_CloseAll
	.extern CPS_Cleanup
	.extern CPS_SetScavengerCallback
	.extern SOCL_CalmDown
	.extern SOCL_Close
	.extern SOCLi_CleanupCommandPacketQueue
	.extern _02110628
	.extern _021D4424
	.extern _021D442C
	.extern _021D4430
	.extern _021D448C
	.extern _021D4490
	.extern _021D4670

.text
	arm_func_start SOCL_Cleanup
SOCL_Cleanup: ; 0x020A2918
	stmdb sp!, {r3, r4, r5, lr}
	ldr r1, _020A29AC ; =_021D4424
	ldr r0, [r1]
	cmp r0, #0
	ldreq r0, _020A29B0 ; =_021D4670
	ldreq r0, [r0]
	streq r0, [r1]
	bl SOCL_CalmDown
	mvn r4, #0x19
	cmp r0, r4
	bne _020A295C
	mov r5, #0x64
_020A2948:
	mov r0, r5
	bl OS_Sleep
	bl SOCL_CalmDown
	cmp r0, r4
	beq _020A2948
_020A295C:
	bl SOCLi_CleanupCommandPacketQueue
	movs r4, r0
	bmi _020A29A4
	bl CPS_Cleanup
	mov r0, #0
	bl CPS_SetScavengerCallback
	ldr r0, _020A29B4 ; =_02110628
	ldr r1, [r0]
	ldr r0, [r1, #0x28]
	cmp r0, #0
	bne _020A2998
	ldr r0, _020A29B8 ; =_021D4430
	ldr r1, [r1, #0x1c]
	ldr r0, [r0, #0x1c]
	blx r1
_020A2998:
	ldr r0, _020A29B4 ; =_02110628
	mov r1, #0
	str r1, [r0]
_020A29A4:
	mov r0, r4
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_020A29AC: .word _021D4424
_020A29B0: .word _021D4670
_020A29B4: .word _02110628
_020A29B8: .word _021D4430
	arm_func_end SOCL_Cleanup

	arm_func_start SOCL_CloseAll
SOCL_CloseAll: ; 0x020A29BC
	stmdb sp!, {r4, r5, r6, lr}
	ldr r4, _020A2A58 ; =_021D442C
	ldr r5, _020A2A5C ; =_021D448C
_020A29C8:
	bl OS_DisableInterrupts
	ldr r6, [r5]
	cmp r6, #0
	beq _020A29FC
	ldr r2, [r4]
_020A29DC:
	cmp r6, r2
	beq _020A29F0
	ldrsh r1, [r6, #0x70]
	tst r1, #0x10
	beq _020A29FC
_020A29F0:
	ldr r6, [r6, #0x7c]
	cmp r6, #0
	bne _020A29DC
_020A29FC:
	bl OS_RestoreInterrupts
	cmp r6, #0
	beq _020A2A14
	mov r0, r6
	bl SOCL_Close
	b _020A29C8
_020A2A14:
	ldr r0, _020A2A5C ; =_021D448C
	ldr r1, [r0]
	cmp r1, #0
	beq _020A2A3C
	ldr r0, _020A2A58 ; =_021D442C
	ldr r0, [r0]
	cmp r1, r0
	ldreq r0, [r1, #0x7c]
	cmpeq r0, #0
	bne _020A2A50
_020A2A3C:
	ldr r0, _020A2A60 ; =_021D4490
	ldr r0, [r0]
	cmp r0, #0
	moveq r0, #0
	ldmeqia sp!, {r4, r5, r6, pc}
_020A2A50:
	mvn r0, #0x19
	ldmia sp!, {r4, r5, r6, pc}
	.align 2, 0
_020A2A58: .word _021D442C
_020A2A5C: .word _021D448C
_020A2A60: .word _021D4490
	arm_func_end SOCL_CloseAll
