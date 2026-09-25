	.include "asm/macros.inc"
	.include "wifi.inc"
	.include "global.inc"
	.public SOCLi_ReadCallBack
	.extern CPS_SocConsume
	.extern CPS_SocRead
	.extern SOCLi_ConsumeCPSBuffer
	.extern SOCLi_CopyCPSBuffer

.text
	arm_func_start SOCLi_ReadCallBack
SOCLi_ReadCallBack: ; 0x020A19E4
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x14
	ldr r7, [r0, #4]
	ldr r1, [r0, #0x10]
	ldr r8, [r7, #0x64]
	str r1, [sp, #0xc]
	ldr r1, [r0, #0x18]
	ldr sl, [r0, #0x14]
	ldr r0, [r0, #0x1c]
	ldr sb, [r8, #0xf8]
	str r1, [sp, #8]
	str r0, [sp, #4]
	mov r4, #0
	mov r5, #1
	mov fp, #0xa
	add r6, sp, #0x10
_020A1A24:
	mov r0, r6
	bl CPS_SocRead
	cmp r0, #0
	beq _020A1A78
	ldr r1, [sp, #0x10]
	sub r1, r1, sb
	cmp r1, #0
	bgt _020A1A78
	ldrsb r0, [r7, #0x73]
	mov r1, r5
	cmp r0, #0
	cmpne r0, #4
	movne r1, r4
	cmp r1, #0
	ldrneb r0, [r7, #8]
	cmpne r0, #4
	movne r0, #0
	bne _020A1A78
	mov r0, fp
	bl OS_Sleep
	b _020A1A24
_020A1A78:
	ldrsh r1, [r7, #0x70]
	tst r1, #0x80
	addne sp, sp, #0x14
	mvnne r0, #0xe
	ldmneia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldrsb r1, [r7, #0x73]
	cmp r1, #4
	bne _020A1AD4
	cmp r0, #0
	addeq sp, sp, #0x14
	moveq r0, #0
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldr r1, [sp, #0x10]
	cmp sl, r1
	movhi sl, r1
	ldr r1, [sp, #0xc]
	mov r2, sl
	bl MI_CpuCopy8
	mov r0, sl
	bl CPS_SocConsume
	add sp, sp, #0x14
	mov r0, sl
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020A1AD4:
	cmp r0, #0
	moveq r4, #0
	beq _020A1B00
	ldr r4, [sp, #4]
	ldr r1, [sp, #0xc]
	ldr r3, [sp, #8]
	mov r0, r7
	mov r2, sl
	str r4, [sp]
	bl SOCLi_CopyCPSBuffer
	mov r4, r0
_020A1B00:
	cmp r4, #0
	addle sp, sp, #0x14
	movle r0, r4
	ldmleia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldrh r0, [r8, #0xfc]
	ldr r1, [r8, #0xf8]
	cmp r1, r0
	blt _020A1B28
	mov r0, r7
	bl SOCLi_ConsumeCPSBuffer
_020A1B28:
	mov r0, r4
	add sp, sp, #0x14
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	arm_func_end SOCLi_ReadCallBack
