	.include "asm/macros.inc"
	.include "wifi.inc"
	.include "global.inc"
	.public SOCLi_CopyCPSBuffer
	.extern SOCLi_ReadCPSBuffer

.text
	arm_func_start SOCLi_CopyCPSBuffer
SOCLi_CopyCPSBuffer: ; 0x020A181C
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, lr}
	sub sp, sp, #0x10
	mov sb, r0
	mov r8, r1
	mov r7, r2
	mov r6, r3
	bl OS_DisableInterrupts
	add r1, sp, #8
	mov r4, r0
	str r1, [sp]
	add r1, sp, #0xc
	mov r0, sb
	add r2, sp, #6
	add r3, sp, #4
	bl SOCLi_ReadCPSBuffer
	cmp r0, #0
	beq _020A18BC
	ldr r5, [sp, #0xc]
	cmp r5, #0
	mvneq r5, #5
	beq _020A18D8
	ldrsb r1, [sb, #0x73]
	cmp r7, r5
	mov r2, #1
	movgt r7, r5
	cmp r1, #0
	cmpne r1, #4
	movne r2, #0
	cmp r2, #0
	mov r1, r8
	mov r2, r7
	movne r5, r7
	bl MI_CpuCopy8
	ldr r1, [sb, #0x64]
	ldrsb r0, [r1, #0xfe]
	cmp r0, #0
	ldreq r0, [r1, #0xf8]
	addeq r0, r0, r5
	streq r0, [r1, #0xf8]
	b _020A18D8
_020A18BC:
	ldr r0, [sp, #0xc]
	cmp r0, #0
	ldrsh r0, [sb, #0x70]
	moveq r5, #0
	mvnne r5, #0x1b
	bic r0, r0, #6
	strh r0, [sb, #0x70]
_020A18D8:
	cmp r5, #0
	blt _020A1910
	cmp r6, #0
	ldrne r1, [sp, #0x30]
	cmpne r1, #0
	beq _020A1900
	ldrh r0, [sp, #4]
	strh r0, [r6]
	ldr r0, [sp, #8]
	str r0, [r1]
_020A1900:
	ldrh r0, [sb, #0x74]
	cmp r0, #0
	ldreqh r0, [sp, #6]
	streqh r0, [sb, #0x74]
_020A1910:
	mov r0, r4
	bl OS_RestoreInterrupts
	mov r0, r5
	add sp, sp, #0x10
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	arm_func_end SOCLi_CopyCPSBuffer
