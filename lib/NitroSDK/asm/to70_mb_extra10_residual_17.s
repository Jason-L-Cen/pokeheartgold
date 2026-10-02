	.extern _start_AutoloadDoneCallback
	.include "asm/macros.inc"
	.include "mb.inc"
	.include "global.inc"
	.public MBi_MakeParentSendBuffer
	.public MBi_SetRecvBufferFromChild
	.extern ov13_0224CF84

.text
	arm_func_start MBi_MakeParentSendBuffer
MBi_MakeParentSendBuffer: ; 0x02220668
	ldrb r2, [r0]
	mov r3, r1
	add r3, r3, #1
	strb r2, [r1]
	ldrb r1, [r0]
	cmp r1, #6
	addls pc, pc, r1, lsl #2
	b _022206DC
_02220688: ; jump table
	b _022206DC ; case 0
	b _022206E4 ; case 1
	b _022206E4 ; case 2
	b _022206E4 ; case 3
	b _022206A4 ; case 4
	b _022206E4 ; case 5
	b _022206E4 ; case 6
_022206A4:
	ldrh r1, [r0, #2]
	strb r1, [r3]
	ldrh r1, [r0, #2]
	and r1, r1, #0xff00
	mov r1, r1, asr #8
	strb r1, [r3, #1]
	ldrh r1, [r0, #4]
	strb r1, [r3, #2]
	ldrh r0, [r0, #4]
	and r0, r0, #0xff00
	mov r0, r0, asr #8
	strb r0, [r3, #3]
	add r3, r3, #4
	b _022206E4
_022206DC:
	mov r0, #0
	bx lr
_022206E4:
	mov r0, r3
	bx lr
	arm_func_end MBi_MakeParentSendBuffer

	arm_func_start MBi_SetRecvBufferFromChild
MBi_SetRecvBufferFromChild: ; 0x022206EC
	stmdb sp!, {r4, r5, r6, lr}
	mov r6, r0
	ldrb r0, [r6]
	mov r5, r1
	mov r4, r2
	strb r0, [r5]
	cmp r0, #7
	beq _02220720
	cmp r0, #8
	beq _02220784
	cmp r0, #9
	beq _022207AC
	b _022207F4
_02220720:
	mov r0, r4
	bl IsGetAllRequestData
	cmp r0, #0
	beq _02220744
	ldr r0, _02220804 ; =ov13_0224CF84
	sub r1, r4, #1
	ldr r0, [r0, #4]
	add r0, r0, r1, lsl #5
	ldmia sp!, {r4, r5, r6, pc}
_02220744:
	ldrb r2, [r6, #1]
	ldr r0, _02220804 ; =ov13_0224CF84
	strb r2, [r5, #2]
	ldr r1, [r0, #0xc]
	cmp r2, r1
	movgt r0, #0
	ldmgtia sp!, {r4, r5, r6, pc}
	ldr r2, [r0, #8]
	add r0, r6, #2
	add r1, r5, #3
	bl MI_CpuCopy8
	mov r0, r5
	mov r1, r4
	bl MBi_ReceiveRequestDataPiece
	mov r4, r0
	b _022207FC
_02220784:
	ldrb r0, [r6, #1]
	add r4, r6, #3
	strh r0, [r5, #2]
	ldrb r0, [r6, #2]
	ldrh r1, [r5, #2]
	mov r0, r0, lsl #8
	and r0, r0, #0xff00
	orr r0, r1, r0
	strh r0, [r5, #2]
	b _022207FC
_022207AC:
	ldrb r0, [r6, #1]
	add r4, r6, #3
	ldr r1, _02220804 ; =ov13_0224CF84
	strh r0, [r5, #2]
	ldrb r2, [r6, #2]
	ldrh r3, [r5, #2]
	mov r0, r4
	mov r2, r2, lsl #8
	and r2, r2, #0xff00
	orr r2, r3, r2
	strh r2, [r5, #2]
	ldr r2, [r1, #8]
	add r1, r5, #4
	bl MI_CpuCopy8
	ldr r0, _02220804 ; =ov13_0224CF84
	ldr r0, [r0, #8]
	add r4, r4, r0
	b _022207FC
_022207F4:
	mov r0, #0
	ldmia sp!, {r4, r5, r6, pc}
_022207FC:
	mov r0, r4
	ldmia sp!, {r4, r5, r6, pc}
	.balign 4, 0
_02220804: .word ov13_0224CF84
	arm_func_end MBi_SetRecvBufferFromChild

	arm_func_start MBi_ReceiveRequestDataPiece
MBi_ReceiveRequestDataPiece: ; 0x02220808
	stmdb sp!, {r4, r5, r6, lr}
	ldr r2, _02220890 ; =ov13_0224CF84
	mov r6, r1
	ldr r3, [r2, #4]
	cmp r3, #0
	moveq r0, #0
	ldmeqia sp!, {r4, r5, r6, pc}
	ldrb r5, [r0, #2]
	ldr r1, [r2, #0xc]
	cmp r5, r1
	movgt r0, #0
	ldmgtia sp!, {r4, r5, r6, pc}
	sub r4, r6, #1
	ldr r2, [r2, #8]
	add r1, r3, r4, lsl #5
	mla r1, r5, r2, r1
	add r0, r0, #3
	bl MI_CpuCopy8
	ldr r0, _02220890 ; =ov13_0224CF84
	mov r1, #1
	ldr r2, [r0, #4]
	mov r0, r6
	add r3, r2, #0x1e0
	ldr r2, [r3, r4, lsl #2]
	orr r1, r2, r1, lsl r5
	str r1, [r3, r4, lsl #2]
	bl IsGetAllRequestData
	cmp r0, #0
	moveq r0, #0
	ldmeqia sp!, {r4, r5, r6, pc}
	ldr r0, _02220890 ; =ov13_0224CF84
	ldr r0, [r0, #4]
	add r0, r0, r4, lsl #5
	ldmia sp!, {r4, r5, r6, pc}
	.balign 4, 0
_02220890: .word ov13_0224CF84
	arm_func_end MBi_ReceiveRequestDataPiece

	arm_func_start IsGetAllRequestData
IsGetAllRequestData: ; 0x02220894
	ldr r1, _022208E4 ; =ov13_0224CF84
	mov r2, #0
	ldr ip, [r1, #0xc]
	cmp ip, #0
	ble _022208DC
	ldr r1, [r1, #4]
	sub r0, r0, #1
	add r0, r1, r0, lsl #2
	ldr r3, [r0, #0x1e0]
	mov r1, #1
_022208BC:
	tst r3, r1, lsl r2
	moveq r0, #0
	bxeq lr
	add r0, r2, #1
	mov r0, r0, lsl #0x10
	cmp ip, r0, lsr #16
	mov r2, r0, lsr #0x10
	bgt _022208BC
_022208DC:
	mov r0, #1
	bx lr
	.balign 4, 0
_022208E4: .word ov13_0224CF84
	arm_func_end IsGetAllRequestData
