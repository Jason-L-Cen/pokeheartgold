	.extern CTRDG_Enable
	.extern CTRDG_IsExisting
	.extern CTRDGi_ChangeLatestAccessCycle
	.extern CTRDGi_InitCommon
	.extern CTRDGi_InitTaskThread
	.extern CTRDGi_LockByProcessor
	.extern CTRDGi_RestoreAccessCycle
	.extern CTRDGi_SendtoPxi
	.extern CTRDGi_UnlockByProcessor
	.extern CTRDGi_Work
	.extern DC_FlushAll
	.extern DC_InvalidateRange
	.extern PXI_Init
	.extern PXI_IsCallbackReady
	.extern PXI_SetFifoRecvCallback
	.extern _021E4D60
	.extern _021E4D80
	.extern _021E4D84
	.extern _021E4E40
	.include "asm/macros.inc"
	.include "nitro.inc"
	.include "global.inc"
	.public CTRDGi_InitModuleInfo
.text
	arm_func_start CTRDGi_InitModuleInfo
CTRDGi_InitModuleInfo: ; 0x020E1798
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	sub sp, sp, #0x10
	ldr r0, _020E1960 ; =_021E4D60
	ldr r1, [r0]
	cmp r1, #0
	addne sp, sp, #0x10
	ldmneia sp!, {r3, r4, r5, r6, r7, pc}
	ldr r1, _020E1964 ; =0x04000300
	mov r2, #1
	ldrh r1, [r1]
	str r2, [r0]
	tst r1, #1
	addeq sp, sp, #0x10
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	mov r0, #0x40000
	bl OS_SetIrqMask
	ldr r3, _020E1968 ; =0x04000208
	mov r2, #1
	ldrh r5, [r3]
	ldr r1, _020E196C ; =CTRDGi_Work
	mov r4, r0
	strh r2, [r3]
	ldrh r0, [r1, #2]
	add r1, sp, #8
	bl CTRDGi_LockByProcessor
	ldr r1, _020E1970 ; =0x04000204
	add r0, sp, #0
	ldrh r1, [r1]
	and r1, r1, #0x8000
	mov r6, r1, asr #0xf
	bl CTRDGi_ChangeLatestAccessCycle
	ldr r3, _020E1970 ; =0x04000204
	ldr r0, _020E1974 ; =_021E4D80
	ldrh r2, [r3]
	add r0, r0, #0x80
	mov r1, #0x40
	bic r2, r2, #0x8000
	strh r2, [r3]
	bl DC_InvalidateRange
	ldr r2, _020E1974 ; =_021E4D80
	ldr r1, _020E1978 ; =0x08000080
	mov r0, #1
	mov r3, #0x40
	add r2, r2, #0x80
	bl MI_DmaCopy16
	ldr r2, _020E1970 ; =0x04000204
	add r0, sp, #0
	ldrh r1, [r2]
	bic r1, r1, #0x8000
	orr r1, r1, r6, lsl #15
	strh r1, [r2]
	bl CTRDGi_RestoreAccessCycle
	ldr r0, _020E196C ; =CTRDGi_Work
	add r1, sp, #8
	ldrh r0, [r0, #2]
	bl CTRDGi_UnlockByProcessor
	ldr r1, _020E197C ; =0x027FFF9B
	ldrb r0, [r1]
	cmp r0, #0
	bne _020E1894
	ldrb r0, [r1, #-1]
	cmp r0, #0
	bne _020E18F8
_020E1894:
	ldr r3, _020E1974 ; =_021E4D80
	ldr r6, _020E1980 ; =0x027FFC30
	ldrh r0, [r3, #0xbe]
	mov r2, #0
	strh r0, [r6]
	b _020E18C0
_020E18AC:
	add r0, r3, r2
	ldrb r1, [r0, #0xb5]
	add r0, r6, r2
	add r2, r2, #1
	strb r1, [r0, #2]
_020E18C0:
	cmp r2, #3
	blt _020E18AC
	ldrh r0, [r3, #0xb0]
	strh r0, [r6, #6]
	ldr r0, [r3, #0xac]
	str r0, [r6, #8]
	bl CTRDG_IsExisting
	cmp r0, #0
	movne r2, #1
	ldr r1, _020E197C ; =0x027FFF9B
	moveq r2, #0
	mov r0, #1
	strb r2, [r1]
	strb r0, [r1, #-1]
_020E18F8:
	ldr r0, _020E1984 ; =0xFFFF0020
	ldr r1, _020E1988 ; =_021E4D84
	mov r2, #0x9c
	bl MIi_CpuCopy32
	bl DC_FlushAll
	ldr r0, _020E1974 ; =_021E4D80
	add r0, r0, #0xfe000000
	mov r0, r0, lsr #5
	mov r0, r0, lsl #6
	orr r0, r0, #1
	bl CTRDGi_SendtoPxi
	mov r7, #1
	ldr r6, _020E196C ; =CTRDGi_Work
	b _020E1938
_020E1930:
	mov r0, r7
	bl SVC_WaitByLoop
_020E1938:
	ldrh r0, [r6]
	cmp r0, #1
	bne _020E1930
	ldr r2, _020E1968 ; =0x04000208
	mov r0, r4
	ldrh r1, [r2]
	strh r5, [r2]
	bl OS_SetIrqMask
	add sp, sp, #0x10
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.align 2, 0
_020E1960: .word _021E4D60
_020E1964: .word 0x04000300
_020E1968: .word 0x04000208
_020E196C: .word CTRDGi_Work
_020E1970: .word 0x04000204
_020E1974: .word _021E4D80
_020E1978: .word 0x08000080
_020E197C: .word 0x027FFF9B
_020E1980: .word 0x027FFC30
_020E1984: .word 0xFFFF0020
_020E1988: .word _021E4D84
	arm_func_end CTRDGi_InitModuleInfo
