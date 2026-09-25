	.include "asm/macros.inc"
	.include "wifi.inc"
	.include "global.inc"
	.public SOCL_ReadFrom
	.extern SOCL_SocketIsInvalid
	.extern SOCLi_ReadAndConsumeBuffer

.text
	arm_func_start SOCL_ReadFrom
SOCL_ReadFrom: ; 0x020A15C4
	stmdb sp!, {r4, r5, r6, r7, r8, sb, lr}
	sub sp, sp, #0xc
	mov sb, r0
	mov r8, r1
	mov r7, r2
	mov r6, r3
	bl SOCL_SocketIsInvalid
	cmp r0, #0
	addne sp, sp, #0xc
	mvnne r0, #0x1b
	ldmneia sp!, {r4, r5, r6, r7, r8, sb, pc}
	ldr r0, [sp, #0x2c]
	tst r0, #4
	bne _020A1608
	ldrsb r0, [sb, #0x72]
	cmp r0, #0
	bne _020A1624
_020A1608:
	ldrsb r0, [sb, #0x73]
	cmp r0, #4
	addeq sp, sp, #0xc
	mvneq r0, #0x1b
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, pc}
	mov r5, #0
	b _020A163C
_020A1624:
	bl OS_GetProcMode
	cmp r0, #0x12
	addeq sp, sp, #0xc
	mvneq r0, #0x1b
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, pc}
	mov r5, #1
_020A163C:
	cmp sb, #0
	mov r1, #0
	beq _020A1654
	ldrsh r0, [sb, #0x70]
	tst r0, #1
	movne r1, #1
_020A1654:
	cmp r1, #0
	addeq sp, sp, #0xc
	mvneq r0, #0x26
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, pc}
	ldrsb r0, [sb, #0x73]
	mov r1, #1
	cmp r0, #0
	cmpne r0, #4
	movne r1, #0
	cmp r1, #0
	beq _020A16A4
	ldrsh r0, [sb, #0x70]
	tst r0, #4
	beq _020A1698
	ldrsh r0, [sb, #0x70]
	tst r0, #8
	beq _020A16A4
_020A1698:
	add sp, sp, #0xc
	mvn r0, #0x37
	ldmia sp!, {r4, r5, r6, r7, r8, sb, pc}
_020A16A4:
	ldr r4, [sb, #0x64]
	tst r5, #1
	add r0, r4, #0xe0
	bne _020A16CC
	bl OS_TryLockMutex
	cmp r0, #0
	bne _020A16D0
	add sp, sp, #0xc
	mvn r0, #5
	ldmia sp!, {r4, r5, r6, r7, r8, sb, pc}
_020A16CC:
	bl OS_LockMutex
_020A16D0:
	ldr r0, [sp, #0x28]
	ldr ip, [sp, #0x2c]
	str r0, [sp]
	mov r0, sb
	mov r1, r8
	mov r2, r7
	mov r3, r6
	stmib sp, {r5, ip}
	bl SOCLi_ReadAndConsumeBuffer
	mov r5, r0
	add r0, r4, #0xe0
	bl OS_UnlockMutex
	mov r0, r5
	add sp, sp, #0xc
	ldmia sp!, {r4, r5, r6, r7, r8, sb, pc}
	arm_func_end SOCL_ReadFrom
