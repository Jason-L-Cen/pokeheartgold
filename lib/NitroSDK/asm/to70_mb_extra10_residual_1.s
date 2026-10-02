	.extern _start_AutoloadDoneCallback
	.include "asm/macros.inc"
	.include "mb.inc"
	.include "global.inc"
	.extern IsChildAidValid
	.extern mbc

.text
	arm_func_start MB_CommResponseRequest
MB_CommResponseRequest: ; 0x0221BB70
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	mov r4, r1
	mov r7, r0
	bl OS_DisableInterrupts
	mov r6, r0
	cmp r4, #3
	addls pc, pc, r4, lsl #2
	b _0221BBD0
_0221BB90: ; jump table
	b _0221BBA0 ; case 0
	b _0221BBAC ; case 1
	b _0221BBB8 ; case 2
	b _0221BBC4 ; case 3
_0221BBA0:
	mov r5, #0xa
	mov r4, #4
	b _0221BBE0
_0221BBAC:
	mov r5, #0xa
	mov r4, #3
	b _0221BBE0
_0221BBB8:
	mov r5, #0xe
	mov r4, #2
	b _0221BBE0
_0221BBC4:
	mov r5, #7
	mov r4, #5
	b _0221BBE0
_0221BBD0:
	mov r0, r6
	bl OS_RestoreInterrupts
	mov r0, #0
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_0221BBE0:
	ldr r0, _0221BC4C ; =mbc
	ldr r0, [r0]
	cmp r0, #0
	beq _0221BC3C
	mov r0, r7
	bl IsChildAidValid
	cmp r0, #0
	beq _0221BC3C
	ldr r0, _0221BC4C ; =mbc
	sub r2, r7, #1
	ldr r1, [r0]
	add r0, r1, r2, lsl #2
	add r0, r0, #0x1000
	ldr r0, [r0, #0x4e8]
	cmp r5, r0
	bne _0221BC3C
	add r0, r1, r2, lsl #1
	add r1, r0, #0x1700
	mov r0, r6
	strh r4, [r1, #0x54]
	bl OS_RestoreInterrupts
	mov r0, #1
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_0221BC3C:
	mov r0, r6
	bl OS_RestoreInterrupts
	mov r0, #0
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.balign 4, 0
_0221BC4C: .word mbc
	arm_func_end MB_CommResponseRequest
