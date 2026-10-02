	.extern _start_AutoloadDoneCallback
	.include "asm/macros.inc"
	.include "mb.inc"
	.include "global.inc"
	.public MBi_ChildCallback
	.public MBi_ParentCallback
	.extern MBi_CheckWmErrcode
	.extern MBi_CommEnd
	.extern MBi_EndCommon
	.extern MBi_IsSendEnabled
	.extern MBi_OnInitializeDone
	.extern changeScanChannel
	.extern ov13_0224598C
	.extern ov13_0224CF40
	.extern ov13_0224CF60

.text
	arm_func_start MBi_ParentCallback
MBi_ParentCallback: ; 0x0221E554
	stmdb sp!, {r3, r4, lr}
	sub sp, sp, #0x1c
	mov r4, r0
	ldrh r1, [r4]
	cmp r1, #0x19
	bgt _0221E5BC
	bge _0221E698
	cmp r1, #0xf
	addls pc, pc, r1, lsl #2
	b _0221ED7C
_0221E57C: ; jump table
	b _0221E5D8 ; case 0
	b _0221EB4C ; case 1
	b _0221EC3C ; case 2
	b _0221ED7C ; case 3
	b _0221ED7C ; case 4
	b _0221ED7C ; case 5
	b _0221ED7C ; case 6
	b _0221E660 ; case 7
	b _0221E734 ; case 8
	b _0221ED7C ; case 9
	b _0221ED7C ; case 10
	b _0221ED7C ; case 11
	b _0221ED7C ; case 12
	b _0221EC84 ; case 13
	b _0221E988 ; case 14
	b _0221EA28 ; case 15
_0221E5BC:
	cmp r1, #0x1d
	bgt _0221E5CC
	beq _0221E610
	b _0221ED7C
_0221E5CC:
	cmp r1, #0x80
	beq _0221ECBC
	b _0221ED7C
_0221E5D8:
	ldrh r0, [r4, #2]
	cmp r0, #0
	beq _0221E604
	ldr r0, _0221ED9C ; =ov13_0224CF40
	mov r1, r4
	ldr r2, [r0, #8]
	mov r0, #0x100
	ldr r2, [r2, #0x51c]
	blx r2
	add sp, sp, #0x1c
	ldmia sp!, {r3, r4, pc}
_0221E604:
	bl MBi_OnInitializeDone
	add sp, sp, #0x1c
	ldmia sp!, {r3, r4, pc}
_0221E610:
	ldrh r0, [r4, #2]
	cmp r0, #0
	beq _0221E63C
	ldr r0, _0221ED9C ; =ov13_0224CF40
	mov r1, r4
	ldr r2, [r0, #8]
	mov r0, #0x100
	ldr r2, [r2, #0x51c]
	blx r2
	add sp, sp, #0x1c
	ldmia sp!, {r3, r4, pc}
_0221E63C:
	ldr r1, _0221ED9C ; =ov13_0224CF40
	ldr r0, _0221EDA0 ; =MBi_ParentCallback
	ldr r1, [r1, #8]
	bl WM_SetParentParameter
	mov r1, r0
	mov r0, #7
	bl MBi_CheckWmErrcode
	add sp, sp, #0x1c
	ldmia sp!, {r3, r4, pc}
_0221E660:
	ldr r0, _0221ED9C ; =ov13_0224CF40
	mov r1, r4
	ldr r2, [r0, #8]
	mov r0, #0x15
	ldr r2, [r2, #0x51c]
	blx r2
	ldr r0, _0221EDA0 ; =MBi_ParentCallback
	mov r1, #1
	bl WM_SetBeaconIndication
	mov r1, r0
	mov r0, #0x19
	bl MBi_CheckWmErrcode
	add sp, sp, #0x1c
	ldmia sp!, {r3, r4, pc}
_0221E698:
	ldr r2, _0221ED9C ; =ov13_0224CF40
	ldr ip, [r2, #8]
	add r1, ip, #0x500
	ldrh r3, [r1, #0x26]
	cmp r3, #0
	bne _0221E6F8
	ldrh r0, [r4, #2]
	cmp r0, #0
	beq _0221E6D4
	ldr r2, [ip, #0x51c]
	mov r1, r4
	mov r0, #0x100
	blx r2
	add sp, sp, #0x1c
	ldmia sp!, {r3, r4, pc}
_0221E6D4:
	ldr r1, _0221EDA4 ; =ov13_0224598C
	ldr r0, _0221EDA0 ; =MBi_ParentCallback
	ldr r1, [r1, #0xc]
	bl WMi_StartParentEx
	mov r1, r0
	mov r0, #8
	bl MBi_CheckWmErrcode
	add sp, sp, #0x1c
	ldmia sp!, {r3, r4, pc}
_0221E6F8:
	ldrh r3, [r4, #2]
	cmp r3, #0
	beq _0221E728
	mov r0, #0
	strh r0, [r1, #0x26]
	ldr r0, [r2, #8]
	mov r1, r4
	ldr r2, [r0, #0x51c]
	mov r0, #0x100
	blx r2
	add sp, sp, #0x1c
	ldmia sp!, {r3, r4, pc}
_0221E728:
	bl MBi_EndCommon
	add sp, sp, #0x1c
	ldmia sp!, {r3, r4, pc}
_0221E734:
	ldrh r0, [r4, #2]
	cmp r0, #0
	beq _0221E760
	ldr r0, _0221ED9C ; =ov13_0224CF40
	mov r1, r4
	ldr r2, [r0, #8]
	mov r0, #0x100
	ldr r2, [r2, #0x51c]
	blx r2
	add sp, sp, #0x1c
	ldmia sp!, {r3, r4, pc}
_0221E760:
	ldrh r0, [r4, #8]
	cmp r0, #7
	bgt _0221E790
	bge _0221E7D8
	cmp r0, #2
	bgt _0221E968
	cmp r0, #0
	blt _0221E968
	beq _0221E7B0
	cmp r0, #2
	beq _0221E934
	b _0221E968
_0221E790:
	cmp r0, #9
	bgt _0221E7A0
	beq _0221E8F8
	b _0221E968
_0221E7A0:
	cmp r0, #0x1a
	addeq sp, sp, #0x1c
	ldmeqia sp!, {r3, r4, pc}
	b _0221E968
_0221E7B0:
	ldr r1, _0221ED9C ; =ov13_0224CF40
	mov r2, #0
	ldr r0, [r1, #8]
	add sp, sp, #0x1c
	add r0, r0, #0x500
	strh r2, [r0, #0x2a]
	ldr r0, [r1, #8]
	add r0, r0, #0x500
	strh r2, [r0, #0x28]
	ldmia sp!, {r3, r4, pc}
_0221E7D8:
	ldr r2, _0221ED9C ; =ov13_0224CF40
	ldr r0, [r2, #8]
	add r0, r0, #0x500
	ldrh r1, [r0, #0x26]
	cmp r1, #1
	addeq sp, sp, #0x1c
	ldmeqia sp!, {r3, r4, pc}
	ldrh lr, [r0, #0x2a]
	ldrh r3, [r4, #0x10]
	mov ip, #1
	mov r1, r4
	orr r3, lr, ip, lsl r3
	strh r3, [r0, #0x2a]
	ldr r2, [r2, #8]
	mov r0, #0
	ldr r2, [r2, #0x51c]
	blx r2
	ldr r1, _0221ED9C ; =ov13_0224CF40
	ldr r0, [r1, #8]
	add r0, r0, #0x500
	ldrh r0, [r0, #0x28]
	cmp r0, #0
	bne _0221E8C8
	ldr r0, [r1, #0xc]
	add r0, r0, #0x1000
	ldr r2, [r0, #0x31c]
	cmp r2, #0
	bne _0221E8C8
	mov r2, #1
	str r2, [r0, #0x31c]
	ldr r0, [r1, #8]
	ldr r1, _0221ED9C ; =ov13_0224CF40
	add r0, r0, #0x500
	ldrh r0, [r0, #0x2c]
	ldr ip, [r1, #8]
	mov r1, #1
	cmp r0, #0
	movne r2, #0
	mov r0, r2, lsl #0x10
	mov r3, r0, lsr #0x10
	add r0, ip, #0x500
	ldrh r4, [r0, #0x18]
	mov r2, #0
	str r4, [sp]
	str r3, [sp, #4]
	str r2, [sp, #8]
	str r2, [sp, #0xc]
	str r2, [sp, #0x10]
	str r1, [sp, #0x14]
	str r1, [sp, #0x18]
	ldrh r2, [r0, #0x1a]
	ldr r1, [ip, #0x504]
	ldr r0, _0221EDA0 ; =MBi_ParentCallback
	add r3, ip, #0x40
	bl WM_StartMPEx
	mov r1, r0
	mov r0, #0xe
	bl MBi_CheckWmErrcode
	add sp, sp, #0x1c
	ldmia sp!, {r3, r4, pc}
_0221E8C8:
	bl MBi_IsSendEnabled
	cmp r0, #0
	addeq sp, sp, #0x1c
	ldmeqia sp!, {r3, r4, pc}
	ldr r1, _0221ED9C ; =ov13_0224CF40
	mov r0, #0x19
	ldr r2, [r1, #8]
	mov r1, #0
	ldr r2, [r2, #0x51c]
	blx r2
	add sp, sp, #0x1c
	ldmia sp!, {r3, r4, pc}
_0221E8F8:
	ldr r2, _0221ED9C ; =ov13_0224CF40
	ldrh r3, [r4, #0x10]
	ldr r1, [r2, #8]
	mov r0, #1
	add r1, r1, #0x500
	ldrh ip, [r1, #0x2a]
	mvn r3, r0, lsl r3
	and r3, ip, r3
	strh r3, [r1, #0x2a]
	ldr r2, [r2, #8]
	mov r1, r4
	ldr r2, [r2, #0x51c]
	blx r2
	add sp, sp, #0x1c
	ldmia sp!, {r3, r4, pc}
_0221E934:
	ldr r0, _0221ED9C ; =ov13_0224CF40
	ldr r1, [r0, #8]
	add r0, r1, #0x500
	ldrh r0, [r0, #0x26]
	cmp r0, #1
	addeq sp, sp, #0x1c
	ldmeqia sp!, {r3, r4, pc}
	ldr r2, [r1, #0x51c]
	mov r1, r4
	mov r0, #0x1c
	blx r2
	add sp, sp, #0x1c
	ldmia sp!, {r3, r4, pc}
_0221E968:
	ldr r0, _0221ED9C ; =ov13_0224CF40
	mov r1, r4
	ldr r2, [r0, #8]
	mov r0, #0x100
	ldr r2, [r2, #0x51c]
	blx r2
	add sp, sp, #0x1c
	ldmia sp!, {r3, r4, pc}
_0221E988:
	ldr r2, _0221ED9C ; =ov13_0224CF40
	mov r1, #0
	ldr r0, [r2, #0xc]
	add r0, r0, #0x1000
	str r1, [r0, #0x31c]
	ldrh r0, [r4, #4]
	cmp r0, #0xa
	beq _0221E9B4
	cmp r0, #0xb
	beq _0221E9F0
	b _0221EA0C
_0221E9B4:
	ldr r0, [r2, #8]
	mov r3, #1
	add r0, r0, #0x500
	strh r3, [r0, #0x28]
	ldr r2, [r2, #8]
	add r0, r2, #0x500
	ldrh r0, [r0, #0x26]
	cmp r0, #0
	addne sp, sp, #0x1c
	ldmneia sp!, {r3, r4, pc}
	ldr r2, [r2, #0x51c]
	mov r0, #0x19
	blx r2
	add sp, sp, #0x1c
	ldmia sp!, {r3, r4, pc}
_0221E9F0:
	ldr r0, [r2, #8]
	ldr r1, [r4, #8]
	ldr r2, [r0, #0x51c]
	mov r0, #3
	blx r2
	add sp, sp, #0x1c
	ldmia sp!, {r3, r4, pc}
_0221EA0C:
	ldr r0, [r2, #8]
	mov r1, r4
	ldr r2, [r0, #0x51c]
	mov r0, #0x100
	blx r2
	add sp, sp, #0x1c
	ldmia sp!, {r3, r4, pc}
_0221EA28:
	ldr r0, _0221ED9C ; =ov13_0224CF40
	ldr r3, [r0, #0xc]
	add r0, r3, #0x7000
	ldr r0, [r0, #0x4c8]
	cmp r0, #0
	beq _0221EA84
	mov r2, #0
	mov r1, r2
_0221EA48:
	add r0, r3, r1, lsl #2
	add r0, r0, #0x1000
	ldr r0, [r0, #0x4e8]
	cmp r0, #0
	beq _0221EA68
	add r2, r2, #1
	cmp r2, #2
	bhs _0221EA74
_0221EA68:
	add r1, r1, #1
	cmp r1, #0xf
	blo _0221EA48
_0221EA74:
	cmp r2, #1
	bne _0221EA84
	ldr r0, _0221EDA8 ; =0x000032C8
	bl OS_SpinWait
_0221EA84:
	ldr r0, _0221ED9C ; =ov13_0224CF40
	mov r2, #0
	ldr r1, [r0, #8]
	strb r2, [r1, #0x50c]
	ldrh r1, [r4, #2]
	cmp r1, #0
	bne _0221EAE8
	ldr r0, [r0, #8]
	mov r1, r4
	ldr r2, [r0, #0x51c]
	mov r0, #2
	blx r2
	ldr r0, _0221ED9C ; =ov13_0224CF40
	ldr r1, [r0, #8]
	add r0, r1, #0x500
	ldrh r0, [r0, #0x26]
	cmp r0, #0
	addne sp, sp, #0x1c
	ldmneia sp!, {r3, r4, pc}
	ldr r2, [r1, #0x51c]
	mov r0, #0x19
	mov r1, #0
	blx r2
	add sp, sp, #0x1c
	ldmia sp!, {r3, r4, pc}
_0221EAE8:
	cmp r1, #0xa
	ldr r0, [r0, #8]
	mov r1, r4
	bne _0221EB0C
	ldr r2, [r0, #0x51c]
	mov r0, #0x2a
	blx r2
	add sp, sp, #0x1c
	ldmia sp!, {r3, r4, pc}
_0221EB0C:
	ldr r2, [r0, #0x51c]
	mov r0, #0x13
	blx r2
	ldr r0, _0221ED9C ; =ov13_0224CF40
	ldr r1, [r0, #8]
	add r0, r1, #0x500
	ldrh r0, [r0, #0x26]
	cmp r0, #0
	addne sp, sp, #0x1c
	ldmneia sp!, {r3, r4, pc}
	ldr r2, [r1, #0x51c]
	mov r0, #0x19
	mov r1, #0
	blx r2
	add sp, sp, #0x1c
	ldmia sp!, {r3, r4, pc}
_0221EB4C:
	ldr r1, _0221ED9C ; =ov13_0224CF40
	ldr r0, [r1, #0xc]
	add r0, r0, #0x1000
	ldr r0, [r0, #0x320]
	cmp r0, #0
	bne _0221EBC8
	ldrh r0, [r4, #2]
	mov r2, #0
	cmp r0, #0
	ldr r0, [r1, #8]
	add r0, r0, #0x500
	beq _0221EB9C
	strh r2, [r0, #0x26]
	ldr r0, [r1, #8]
	mov r1, r4
	ldr r2, [r0, #0x51c]
	mov r0, #0x100
	blx r2
	add sp, sp, #0x1c
	ldmia sp!, {r3, r4, pc}
_0221EB9C:
	strh r2, [r0, #0x2a]
	ldr r1, [r1, #8]
	ldr r0, _0221EDA0 ; =MBi_ParentCallback
	add r1, r1, #0x500
	strh r2, [r1, #0x28]
	bl WM_End
	mov r1, r0
	mov r0, #2
	bl MBi_CheckWmErrcode
	add sp, sp, #0x1c
	ldmia sp!, {r3, r4, pc}
_0221EBC8:
	mov r1, #0
	mov r2, r1
	mov r0, #1
	bl WM_SetPortCallback
	mov r0, #0
	bl WM_SetIndCallback
	ldrh r0, [r4, #2]
	cmp r0, #0
	beq _0221EC1C
	ldr r2, _0221ED9C ; =ov13_0224CF40
	mov r3, #0
	ldr r0, [r2, #8]
	mov r1, r4
	add r0, r0, #0x500
	strh r3, [r0, #0x26]
	ldr r2, [r2, #8]
	mov r0, #0x100
	ldr r2, [r2, #0x51c]
	blx r2
	add sp, sp, #0x1c
	ldmia sp!, {r3, r4, pc}
_0221EC1C:
	ldr r0, _0221EDA0 ; =MBi_ParentCallback
	mov r1, #0
	bl WM_SetBeaconIndication
	mov r1, r0
	mov r0, #0x19
	bl MBi_CheckWmErrcode
	add sp, sp, #0x1c
	ldmia sp!, {r3, r4, pc}
_0221EC3C:
	ldrh r1, [r4, #2]
	cmp r1, #0
	beq _0221EC78
	ldr r2, _0221ED9C ; =ov13_0224CF40
	mov r3, #0
	ldr r0, [r2, #8]
	mov r1, r4
	add r0, r0, #0x500
	strh r3, [r0, #0x26]
	ldr r2, [r2, #8]
	mov r0, #0x100
	ldr r2, [r2, #0x51c]
	blx r2
	add sp, sp, #0x1c
	ldmia sp!, {r3, r4, pc}
_0221EC78:
	bl MBi_EndCommon
	add sp, sp, #0x1c
	ldmia sp!, {r3, r4, pc}
_0221EC84:
	ldrh r0, [r4, #2]
	cmp r0, #0
	addne sp, sp, #0x1c
	ldmneia sp!, {r3, r4, pc}
	ldr r0, _0221ED9C ; =ov13_0224CF40
	ldrh r1, [r4, #0xa]
	ldr r0, [r0, #8]
	add sp, sp, #0x1c
	add r0, r0, #0x500
	ldrh r2, [r0, #0x2a]
	mvn r1, r1
	and r1, r2, r1
	strh r1, [r0, #0x2a]
	ldmia sp!, {r3, r4, pc}
_0221ECBC:
	ldrh r0, [r4, #4]
	sub r0, r0, #0x10
	cmp r0, #7
	addls pc, pc, r0, lsl #2
	b _0221ED94
_0221ECD0: ; jump table
	b _0221ECF0 ; case 0
	b _0221ED10 ; case 1
	b _0221ED30 ; case 2
	b _0221ED50 ; case 3
	b _0221ED94 ; case 4
	b _0221ED94 ; case 5
	b _0221ED70 ; case 6
	b _0221ED94 ; case 7
_0221ECF0:
	ldr r0, _0221ED9C ; =ov13_0224CF40
	mov r1, r4
	ldr r2, [r0, #8]
	mov r0, #0x1d
	ldr r2, [r2, #0x51c]
	blx r2
	add sp, sp, #0x1c
	ldmia sp!, {r3, r4, pc}
_0221ED10:
	ldr r0, _0221ED9C ; =ov13_0224CF40
	mov r1, r4
	ldr r2, [r0, #8]
	mov r0, #0x1f
	ldr r2, [r2, #0x51c]
	blx r2
	add sp, sp, #0x1c
	ldmia sp!, {r3, r4, pc}
_0221ED30:
	ldr r0, _0221ED9C ; =ov13_0224CF40
	mov r1, r4
	ldr r2, [r0, #8]
	mov r0, #0x20
	ldr r2, [r2, #0x51c]
	blx r2
	add sp, sp, #0x1c
	ldmia sp!, {r3, r4, pc}
_0221ED50:
	ldr r0, _0221ED9C ; =ov13_0224CF40
	mov r1, r4
	ldr r2, [r0, #8]
	mov r0, #0x21
	ldr r2, [r2, #0x51c]
	blx r2
	add sp, sp, #0x1c
	ldmia sp!, {r3, r4, pc}
_0221ED70:
	bl OS_Terminate
	add sp, sp, #0x1c
	ldmia sp!, {r3, r4, pc}
_0221ED7C:
	ldr r0, _0221ED9C ; =ov13_0224CF40
	mov r1, r4
	ldr r2, [r0, #8]
	mov r0, #0x100
	ldr r2, [r2, #0x51c]
	blx r2
_0221ED94:
	add sp, sp, #0x1c
	ldmia sp!, {r3, r4, pc}
	.balign 4, 0
_0221ED9C: .word ov13_0224CF40
_0221EDA0: .word MBi_ParentCallback
_0221EDA4: .word ov13_0224598C
_0221EDA8: .word 0x000032C8
	arm_func_end MBi_ParentCallback

	arm_func_start MBi_ChildPortCallback
MBi_ChildPortCallback: ; 0x0221EDAC
	stmdb sp!, {r3, lr}
	mov r1, r0
	ldrh r0, [r1, #2]
	cmp r0, #0
	ldmneia sp!, {r3, pc}
	ldrh r0, [r1, #4]
	cmp r0, #0x15
	bgt _0221EDE8
	bge _0221EE00
	cmp r0, #9
	ldmgtia sp!, {r3, pc}
	cmp r0, #7
	ldmltia sp!, {r3, pc}
	cmpne r0, #9
	ldmia sp!, {r3, pc}
_0221EDE8:
	cmp r0, #0x1a
	ldmgtia sp!, {r3, pc}
	cmp r0, #0x19
	ldmltia sp!, {r3, pc}
	cmpne r0, #0x1a
	ldmia sp!, {r3, pc}
_0221EE00:
	ldr r2, _0221EE18 ; =ov13_0224CF40
	mov r0, #9
	ldr r2, [r2, #8]
	ldr r2, [r2, #0x51c]
	blx r2
	ldmia sp!, {r3, pc}
	.balign 4, 0
_0221EE18: .word ov13_0224CF40
	arm_func_end MBi_ChildPortCallback

	arm_func_start MBi_ChildCallback
MBi_ChildCallback: ; 0x0221EE1C
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x1c
	mov r6, r0
	ldrh r2, [r6]
	ldr r1, _0221F634 ; =ov13_0224CF40
	cmp r2, #0x1d
	ldr r4, [r1, #8]
	bgt _0221EEA8
	cmp r2, #0x1d
	bge _0221EF14
	cmp r2, #0x15
	addls pc, pc, r2, lsl #2
	b _0221F61C
_0221EE50: ; jump table
	b _0221EEB4 ; case 0
	b _0221F534 ; case 1
	b _0221F594 ; case 2
	b _0221F61C ; case 3
	b _0221F61C ; case 4
	b _0221F61C ; case 5
	b _0221F61C ; case 6
	b _0221F61C ; case 7
	b _0221F61C ; case 8
	b _0221F61C ; case 9
	b _0221EFB0 ; case 10
	b _0221F278 ; case 11
	b _0221F2C8 ; case 12
	b _0221F61C ; case 13
	b _0221F440 ; case 14
	b _0221F4B4 ; case 15
	b _0221F61C ; case 16
	b _0221F61C ; case 17
	b _0221F61C ; case 18
	b _0221F61C ; case 19
	b _0221F61C ; case 20
	b _0221F5D0 ; case 21
_0221EEA8:
	cmp r2, #0x80
	beq _0221F5F8
	b _0221F61C
_0221EEB4:
	ldrh r0, [r6, #2]
	ldr r2, [r4, #0x51c]
	mov r1, r6
	cmp r0, #0
	beq _0221EED8
	mov r0, #0x100
	blx r2
	add sp, sp, #0x1c
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
_0221EED8:
	mov r0, #0x15
	blx r2
	ldr r3, _0221F638 ; =ov13_0224598C
	ldr r0, _0221F63C ; =MBi_ChildCallback
	ldrh r1, [r3, #4]
	str r1, [sp]
	ldrh r1, [r3, #6]
	ldrh r2, [r3, #2]
	ldrh r3, [r3]
	bl WM_SetLifeTime
	mov r1, r0
	mov r0, #0x1d
	bl MBi_CheckWmErrcode
	add sp, sp, #0x1c
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
_0221EF14:
	ldrh r0, [r6, #2]
	cmp r0, #0
	beq _0221EF38
	ldr r2, [r4, #0x51c]
	mov r1, r6
	mov r0, #0x100
	blx r2
	add sp, sp, #0x1c
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
_0221EF38:
	add r0, r4, #0x440
	str r0, [r1, #0x20]
	ldrh r0, [r1, #0x24]
	mov r2, #1
	cmp r0, #0
	moveq r0, #1
	streqh r0, [r1, #0x24]
	ldr r0, _0221F634 ; =ov13_0224CF40
	ldrh r1, [r0, #0x26]
	cmp r1, #0
	moveq r1, #0xc8
	streqh r1, [r0, #0x26]
	ldr r0, _0221F634 ; =ov13_0224CF40
	mov r1, #0xff
	strb r1, [r0, #0x28]
	strb r1, [r0, #0x29]
	strb r1, [r0, #0x2a]
	strb r1, [r0, #0x2b]
	strb r1, [r0, #0x2c]
	strb r1, [r0, #0x2d]
	str r2, [r4, #0x5e4]
	ldr r0, _0221F63C ; =MBi_ChildCallback
	ldr r1, _0221F640 ; =ov13_0224CF60
	str r2, [r4, #0x5e8]
	bl WM_StartScan
	mov r1, r0
	mov r0, #0xa
	bl MBi_CheckWmErrcode
	add sp, sp, #0x1c
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
_0221EFB0:
	ldrh r0, [r6, #2]
	cmp r0, #0
	beq _0221EFD4
	ldr r2, [r4, #0x51c]
	mov r1, r6
	mov r0, #0x100
	blx r2
	add sp, sp, #0x1c
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
_0221EFD4:
	ldrh r0, [r6, #8]
	cmp r0, #3
	addeq sp, sp, #0x1c
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	cmp r0, #4
	beq _0221F200
	cmp r0, #5
	bne _0221F260
	add ip, r4, #0x600
	mov r5, #0
	add r0, r4, #0x500
	mov fp, #0x180
	b _0221F130
_0221F008:
	mla r2, r5, fp, ip
	ldrb r3, [r6, #0xa]
	ldrb r1, [r2, #0xca]
	mov lr, #0
	mov sl, lr
	cmp r3, r1
	ldreqb r1, [r2, #0xcb]
	ldreqb r2, [r6, #0xb]
	mov r7, lr
	mov r8, lr
	cmpeq r2, r1
	moveq sl, #1
	mov sb, lr
	cmp sl, #0
	beq _0221F05C
	mov r2, #0x180
	mla r2, r5, r2, ip
	ldrb r1, [r6, #0xc]
	ldrb r2, [r2, #0xcc]
	cmp r1, r2
	moveq sb, #1
_0221F05C:
	cmp sb, #0
	beq _0221F07C
	mov r1, #0x180
	mla r1, r5, r1, ip
	ldrb r2, [r6, #0xd]
	ldrb r1, [r1, #0xcd]
	cmp r2, r1
	moveq r8, #1
_0221F07C:
	cmp r8, #0
	beq _0221F09C
	mov r1, #0x180
	mla r1, r5, r1, ip
	ldrb r2, [r6, #0xe]
	ldrb r1, [r1, #0xce]
	cmp r2, r1
	moveq r7, #1
_0221F09C:
	cmp r7, #0
	beq _0221F0BC
	mov r1, #0x180
	mla r1, r5, r1, ip
	ldrb r2, [r6, #0xf]
	ldrb r1, [r1, #0xcf]
	cmp r2, r1
	moveq lr, #1
_0221F0BC:
	cmp lr, #0
	beq _0221F12C
	mov r0, #0x180
	mla r0, r5, r0, ip
	ldrh r1, [r6, #0x36]
	add sb, r6, #0x38
	add r8, r0, #0xf8
	strh r1, [r0, #0xf6]
	mov r7, #8
_0221F0E0:
	ldmia sb!, {r0, r1, r2, r3}
	stmia r8!, {r0, r1, r2, r3}
	subs r7, r7, #1
	bne _0221F0E0
	add r1, r4, #0x600
	mov r0, #0x180
	mla r0, r5, r0, r1
	mov r1, #0xc0
	bl DC_InvalidateRange
	ldr r1, _0221F634 ; =ov13_0224CF40
	add r2, r4, #0x600
	mov r0, #0x180
	mla r2, r5, r0, r2
	ldrh r0, [r1]
	add r1, r4, #0x440
	mov r3, #0xc0
	bl MI_DmaCopy16
	str r5, [r4, #0x5ec]
	b _0221F1A0
_0221F12C:
	add r5, r5, #1
_0221F130:
	ldrh r1, [r0, #0xe0]
	cmp r5, r1
	blt _0221F008
	cmp r5, #0x10
	bge _0221F1A0
	mov r0, #0x180
	mla r1, r5, r0, ip
	mov r0, r6
	add r7, r5, #1
	add r3, r4, #0x500
	add r1, r1, #0xc0
	mov r2, #0xb8
	strh r7, [r3, #0xe0]
	bl MIi_CpuCopy16
	add r1, r4, #0x600
	mov r0, #0x180
	mla r0, r5, r0, r1
	mov r1, #0xc0
	bl DC_InvalidateRange
	ldr r1, _0221F634 ; =ov13_0224CF40
	add r2, r4, #0x600
	mov r0, #0x180
	mla r2, r5, r0, r2
	ldrh r0, [r1]
	add r1, r4, #0x440
	mov r3, #0xc0
	bl MI_DmaCopy16
	str r5, [r4, #0x5ec]
_0221F1A0:
	ldr r2, [r4, #0x51c]
	mov r1, r6
	mov r0, #4
	blx r2
	ldr r0, [r4, #0x5e4]
	cmp r0, #0
	addeq sp, sp, #0x1c
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldr r0, [r4, #0x5e8]
	cmp r0, #0
	beq _0221F1E0
	ldr r0, _0221F640 ; =ov13_0224CF60
	bl changeScanChannel
	cmp r0, #0
	bne _0221F1E0
	bl MBi_CommEnd
_0221F1E0:
	ldr r0, _0221F63C ; =MBi_ChildCallback
	ldr r1, _0221F640 ; =ov13_0224CF60
	bl WM_StartScan
	mov r1, r0
	mov r0, #0xa
	bl MBi_CheckWmErrcode
	add sp, sp, #0x1c
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
_0221F200:
	ldr r2, [r4, #0x51c]
	mov r1, r6
	mov r0, #5
	blx r2
	ldr r0, [r4, #0x5e4]
	cmp r0, #0
	addeq sp, sp, #0x1c
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldr r0, [r4, #0x5e8]
	cmp r0, #0
	beq _0221F240
	ldr r0, _0221F640 ; =ov13_0224CF60
	bl changeScanChannel
	cmp r0, #0
	bne _0221F240
	bl MBi_CommEnd
_0221F240:
	ldr r0, _0221F63C ; =MBi_ChildCallback
	ldr r1, _0221F640 ; =ov13_0224CF60
	bl WM_StartScan
	mov r1, r0
	mov r0, #0xa
	bl MBi_CheckWmErrcode
	add sp, sp, #0x1c
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
_0221F260:
	ldr r2, [r4, #0x51c]
	mov r1, r6
	mov r0, #0x100
	blx r2
	add sp, sp, #0x1c
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
_0221F278:
	ldrh r0, [r6, #2]
	cmp r0, #0
	beq _0221F29C
	ldr r2, [r4, #0x51c]
	mov r1, r6
	mov r0, #0x100
	blx r2
	add sp, sp, #0x1c
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
_0221F29C:
	ldr r1, [r4, #0x520]
	mov r2, #0
	ldr r0, _0221F63C ; =MBi_ChildCallback
	mov r3, #1
	str r2, [sp]
	bl WM_StartConnectEx
	mov r1, r0
	mov r0, #0xc
	bl MBi_CheckWmErrcode
	add sp, sp, #0x1c
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
_0221F2C8:
	ldrh r0, [r6, #2]
	cmp r0, #0
	beq _0221F2F8
	add r0, r4, #0x500
	mov r1, #0
	strh r1, [r0, #0xe0]
	ldr r2, [r4, #0x51c]
	mov r1, r6
	mov r0, #0xb
	blx r2
	add sp, sp, #0x1c
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
_0221F2F8:
	ldrh r0, [r6, #8]
	cmp r0, #9
	bgt _0221F324
	cmp r0, #6
	blt _0221F428
	beq _0221F334
	cmp r0, #7
	beq _0221F350
	cmp r0, #9
	beq _0221F400
	b _0221F428
_0221F324:
	cmp r0, #0x1a
	addeq sp, sp, #0x1c
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	b _0221F428
_0221F334:
	add r0, r4, #0x500
	mov r1, #0
	strh r1, [r0, #0x2a]
	mov r1, #1
	strh r1, [r0, #0x28]
	add sp, sp, #0x1c
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
_0221F350:
	ldrh r2, [r6, #0xa]
	add r0, r4, #0x500
	mov r1, r6
	strh r2, [r0, #0xe2]
	ldr r2, [r4, #0x51c]
	mov r0, #6
	blx r2
	ldr r1, _0221F644 ; =MBi_ChildPortCallback
	add r3, r4, #0x500
	mov r0, #1
	mov r2, #0
	strh r0, [r3, #0x2a]
	bl WM_SetPortCallback
	cmp r0, #0
	addne sp, sp, #0x1c
	ldmneia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	add r0, r4, #0x500
	ldrh r0, [r0, #0x2c]
	add r1, r4, #0x500
	ldrh r2, [r1, #0x18]
	cmp r0, #0
	movne r0, #0
	moveq r0, #1
	mov r0, r0, lsl #0x10
	mov r0, r0, lsr #0x10
	str r2, [sp]
	str r0, [sp, #4]
	mov r0, #0
	str r0, [sp, #8]
	str r0, [sp, #0xc]
	str r0, [sp, #0x10]
	mov r0, #1
	str r0, [sp, #0x14]
	str r0, [sp, #0x18]
	ldrh r2, [r1, #0x1a]
	ldr r1, [r4, #0x504]
	ldr r0, _0221F63C ; =MBi_ChildCallback
	add r3, r4, #0x40
	bl WM_StartMPEx
	mov r1, r0
	mov r0, #0xe
	bl MBi_CheckWmErrcode
	add sp, sp, #0x1c
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
_0221F400:
	ldr r2, [r4, #0x51c]
	mov r1, r6
	mov r0, #0xa
	blx r2
	add r0, r4, #0x500
	mov r1, #0
	strh r1, [r0, #0x2a]
	strh r1, [r0, #0x28]
	add sp, sp, #0x1c
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
_0221F428:
	ldr r2, [r4, #0x51c]
	mov r1, r6
	mov r0, #0x100
	blx r2
	add sp, sp, #0x1c
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
_0221F440:
	ldrh r0, [r6, #4]
	cmp r0, #0xa
	beq _0221F468
	cmp r0, #0xc
	addeq sp, sp, #0x1c
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	cmp r0, #0xd
	addeq sp, sp, #0x1c
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	b _0221F49C
_0221F468:
	add r0, r4, #0x500
	mov r1, #1
	strh r1, [r0, #0x28]
	bl MBi_IsSendEnabled
	cmp r0, #0
	addeq sp, sp, #0x1c
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldr r2, [r4, #0x51c]
	mov r0, #0x19
	mov r1, #0
	blx r2
	add sp, sp, #0x1c
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
_0221F49C:
	ldr r2, [r4, #0x51c]
	mov r1, r6
	mov r0, #0x100
	blx r2
	add sp, sp, #0x1c
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
_0221F4B4:
	mov r0, #0
	strb r0, [r4, #0x50c]
	ldrh r0, [r6, #2]
	cmp r0, #0
	bne _0221F4DC
	ldr r2, [r4, #0x51c]
	mov r1, r6
	mov r0, #8
	blx r2
	b _0221F500
_0221F4DC:
	cmp r0, #9
	ldr r2, [r4, #0x51c]
	mov r1, r6
	bne _0221F4F8
	mov r0, #0x29
	blx r2
	b _0221F500
_0221F4F8:
	mov r0, #0x12
	blx r2
_0221F500:
	ldr r0, _0221F634 ; =ov13_0224CF40
	ldr r0, [r0, #8]
	add r0, r0, #0x500
	ldrh r0, [r0, #0x26]
	cmp r0, #0
	addne sp, sp, #0x1c
	ldmneia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldr r2, [r4, #0x51c]
	mov r0, #0x19
	mov r1, #0
	blx r2
	add sp, sp, #0x1c
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
_0221F534:
	ldrh r0, [r6, #2]
	cmp r0, #0
	add r0, r4, #0x500
	beq _0221F564
	mov r1, #0
	strh r1, [r0, #0x26]
	ldr r2, [r4, #0x51c]
	mov r1, r6
	mov r0, #0x100
	blx r2
	add sp, sp, #0x1c
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
_0221F564:
	mov r2, #0
	strh r2, [r0, #0x2a]
	ldr r1, [r1, #8]
	ldr r0, _0221F63C ; =MBi_ChildCallback
	add r1, r1, #0x500
	strh r2, [r1, #0x28]
	bl WM_End
	mov r1, r0
	mov r0, #2
	bl MBi_CheckWmErrcode
	add sp, sp, #0x1c
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
_0221F594:
	ldrh r1, [r6, #2]
	cmp r1, #0
	beq _0221F5C4
	add r0, r4, #0x500
	mov r1, #0
	strh r1, [r0, #0x26]
	ldr r2, [r4, #0x51c]
	mov r1, r6
	mov r0, #0x100
	blx r2
	add sp, sp, #0x1c
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
_0221F5C4:
	bl MBi_EndCommon
	add sp, sp, #0x1c
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
_0221F5D0:
	bl MBi_IsSendEnabled
	cmp r0, #0
	addeq sp, sp, #0x1c
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldr r2, [r4, #0x51c]
	mov r0, #0x19
	mov r1, #0
	blx r2
	add sp, sp, #0x1c
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
_0221F5F8:
	ldrh r0, [r6, #4]
	cmp r0, #0x16
	beq _0221F610
	add sp, sp, #0x1c
	cmp r0, #0x17
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
_0221F610:
	bl OS_Terminate
	add sp, sp, #0x1c
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
_0221F61C:
	ldr r2, [r4, #0x51c]
	mov r1, r6
	mov r0, #0x100
	blx r2
	add sp, sp, #0x1c
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.balign 4, 0
_0221F634: .word ov13_0224CF40
_0221F638: .word ov13_0224598C
_0221F63C: .word MBi_ChildCallback
_0221F640: .word ov13_0224CF60
_0221F644: .word MBi_ChildPortCallback
	arm_func_end MBi_ChildCallback

	arm_func_start MBi_GetBeaconPeriodDispersion
MBi_GetBeaconPeriodDispersion: ; 0x0221F648
	stmdb sp!, {r3, lr}
	sub sp, sp, #8
	add r0, sp, #0
	bl OS_GetMacAddress
	mov r1, #0
	add r2, sp, #0
	mov r3, r1
_0221F664:
	ldrb r0, [r2], #1
	add r1, r1, #1
	cmp r1, #6
	add r3, r3, r0
	blt _0221F664
	ldr r1, _0221F6A8 ; =0x027FFC3C
	ldr r0, _0221F6AC ; =0xCCCCCCCD
	ldr r1, [r1]
	mov r2, #0x14
	add r1, r3, r1
	rsb r3, r1, r1, lsl #3
	umull r1, r0, r3, r0
	mov r0, r0, lsr #4
	umull r0, r1, r2, r0
	sub r0, r3, r0
	add sp, sp, #8
	ldmia sp!, {r3, pc}
	.balign 4, 0
_0221F6A8: .word 0x027FFC3C
_0221F6AC: .word 0xCCCCCCCD
	arm_func_end MBi_GetBeaconPeriodDispersion

	arm_func_start MB_Init
MB_Init: ; 0x0221F6B0
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, lr}
	ldr r4, _0221F888 ; =ov13_0224CF40
	mov sb, r1
	ldr r1, [r4, #0xc]
	mov r8, r2
	cmp r1, #0
	addne r1, r1, #0x1300
	ldrneh r1, [r1, #0x16]
	mov r7, r3
	cmpne r1, #0
	movne r0, #2
	ldmneia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	add r0, r0, #0x1f
	bic r4, r0, #0x1f
	add r0, r4, #0x1f
	add r0, r0, #0x1e00
	cmp r7, #0x10000
	bic r5, r0, #0x1f
	bne _0221F704
	bl WM_GetNextTgid
	mov r7, r0
_0221F704:
	bl OS_DisableInterrupts
	ldr r3, _0221F88C ; =0x0000FFFF
	ldr r1, _0221F890 ; =ov13_0224598C
	mov r2, #5
	strh r3, [r1, #6]
	strh r2, [r1]
	mov r2, #0x28
	strh r2, [r1, #2]
	mov r6, r0
	strh r2, [r1, #4]
	mov r2, #1
	ldr r0, [sp, #0x20]
	ldr r3, _0221F888 ; =ov13_0224CF40
	str r2, [r1, #0xc]
	strh r0, [r3]
	str r4, [r3, #8]
	mov r1, r4
	mov r0, #0
	mov r2, #0x1e00
	str r5, [r3, #0xc]
	bl MIi_CpuClear32
	mov r1, r5
	mov r0, #0
	mov r2, #0x1340
	bl MIi_CpuClear16
	ldrb r0, [sb, #1]
	add r2, r4, #0x530
	mov r1, #0
	cmp r0, #0
	ble _0221F798
_0221F77C:
	add r0, sb, r1, lsl #1
	ldrh r0, [r0, #2]
	add r1, r1, #1
	strh r0, [r2], #2
	ldrb r0, [sb, #1]
	cmp r1, r0
	blt _0221F77C
_0221F798:
	add r0, r4, #0x138
	add r3, r0, #0x400
	ldr r0, _0221F890 ; =ov13_0224598C
	mov ip, #0
	ldr r1, [r0, #8]
_0221F7AC:
	ldrh r2, [r1]
	cmp r2, #0
	beq _0221F7D0
	add r1, r1, #2
	add ip, ip, #1
	str r1, [r0, #8]
	cmp ip, #0x10
	strh r2, [r3], #2
	blt _0221F7AC
_0221F7D0:
	mov r0, sb
	add r1, r5, #0x1300
	mov r2, #0x16
	bl MI_CpuCopy8
	ldrb r0, [sb, #1]
	cmp r0, #0xa
	bhs _0221F7FC
	add r0, r5, r0, lsl #1
	add r0, r0, #0x1300
	mov r1, #0
	strh r1, [r0, #2]
_0221F7FC:
	add r0, r4, #0x500
	mov r1, #0x100
	strh r1, [r0]
	mov r1, #8
	strh r1, [r0, #2]
	mov r2, #0
	strh r2, [r0, #0x18]
	strh r2, [r0, #0x1a]
	mov r1, #1
	strh r1, [r0, #0x2c]
	add r0, r5, #0x400
	str r0, [r4, #0x504]
	strh r2, [r4, #0xe]
	strh r2, [r4, #0x12]
	strh r1, [r4, #0x16]
	strh r2, [r4, #0x14]
	str r8, [r4, #8]
	strh r7, [r4, #0xc]
	bl MBi_GetBeaconPeriodDispersion
	add r0, r0, #0xc8
	strh r0, [r4, #0x18]
	mov r0, #0xf
	strh r0, [r4, #0x10]
	mov r3, #0
	strb r3, [r4, #0x50c]
	strb r3, [r4, #0x50d]
	add r1, r5, #0x1300
	mov r2, #1
	mov r0, r6
	strh r2, [r1, #0x16]
	add r1, r5, #0x1000
	str r3, [r1, #0x31c]
	bl OS_RestoreInterrupts
	mov r0, #0
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	.balign 4, 0
_0221F888: .word ov13_0224CF40
_0221F88C: .word 0x0000FFFF
_0221F890: .word ov13_0224598C
	arm_func_end MB_Init
