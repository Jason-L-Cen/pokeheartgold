	.include "asm/macros.inc"
#include <nitro/fs/overlay.h>
	.include "global.inc"

	.data

strtok_string: ; 0x02111358
	.asciz ""
	.balign 4

strtok_ns: ; 0x0211135C
	.word strtok_string
	.word strtok_string

	.text

	arm_func_start strtok
strtok: ; 0x020E9978
	stmdb sp!, {r4, lr}
	sub sp, sp, #0x20
	add ip, sp, #0
	mov r3, #8
	mov r2, #0
_020E998C:
	strb r2, [ip]
	strb r2, [ip, #1]
	strb r2, [ip, #2]
	strb r2, [ip, #3]
	add ip, ip, #4
	subs r3, r3, #1
	bne _020E998C
	cmp r0, #0
	ldrne r2, _020E9ACC ; =strtok_ns
	add lr, r1, #1
	strne r0, [r2, #4]
	ldrb r0, [r1]
	cmp r0, #0
	beq _020E99F4
	add ip, sp, #0
	mov r1, #1
_020E99CC:
	and r3, r0, #0xff
	and r0, r0, #7
	mov r0, r1, lsl r0
	ldrb r2, [ip, r3, asr #3]
	and r0, r0, #0xff
	orr r0, r2, r0
	strb r0, [ip, r3, asr #3]
	ldrb r0, [lr], #1
	cmp r0, #0
	bne _020E99CC
_020E99F4:
	ldr r0, _020E9ACC ; =strtok_ns
	ldr r0, [r0, #4]
	ldrb r4, [r0]
	add r1, r0, #1
	cmp r4, #0
	beq _020E9A38
	add ip, sp, #0
	mov r2, #1
_020E9A14:
	and r3, r4, #0xff
	and r0, r4, #7
	mov r0, r2, lsl r0
	ldrb r3, [ip, r3, asr #3]
	and r0, r0, #0xff
	tst r3, r0
	ldrneb r4, [r1], #1
	cmpne r4, #0
	bne _020E9A14
_020E9A38:
	cmp r4, #0
	bne _020E9A58
	ldr r1, _020E9ACC ; =strtok_ns
	add sp, sp, #0x20
	ldr r2, [r1]
	mov r0, #0
	str r2, [r1, #4]
	ldmia sp!, {r4, pc}
_020E9A58:
	sub r0, r1, #1
	ldrb r4, [r1], #1
	cmp r4, #0
	beq _020E9A98
	add lr, sp, #0
	mov r3, #1
_020E9A70:
	and ip, r4, #0xff
	and r2, r4, #7
	mov r2, r3, lsl r2
	ldrb ip, [lr, ip, asr #3]
	and r2, r2, #0xff
	tst ip, r2
	bne _020E9A98
	ldrb r4, [r1], #1
	cmp r4, #0
	bne _020E9A70
_020E9A98:
	cmp r4, #0
	bne _020E9AB4
	ldr r1, _020E9ACC ; =strtok_ns
	add sp, sp, #0x20
	ldr r2, [r1]
	str r2, [r1, #4]
	ldmia sp!, {r4, pc}
_020E9AB4:
	ldr r2, _020E9ACC ; =strtok_ns
	mov r3, #0
	str r1, [r2, #4]
	strb r3, [r1, #-1]
	add sp, sp, #0x20
	ldmia sp!, {r4, pc}
	.align 2, 0
_020E9ACC: .word strtok_ns
	arm_func_end strtok
