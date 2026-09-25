	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"
	.public NNS_G3dGetResDataByName
	.public NNS_G3dGetResDictIdxByName

.text
	arm_func_start NNS_G3dGetResDataByName
NNS_G3dGetResDataByName: ; 0x020C3818
	stmdb sp!, {r4, r5, r6, r7, r8, lr}
	cmp r1, #0
	moveq r0, #0
	ldmeqia sp!, {r4, r5, r6, r7, r8, pc}
	ldrb r5, [r0, #1]
	cmp r5, #0x10
	bhs _020C38E8
	ldmia r1, {r2, r3, ip, lr}
	cmp r5, #0
	mov r1, #0
	bls _020C39D0
	mov r4, r1
	mov r7, r1
_020C384C:
	cmp r0, #0
	beq _020C3878
	ldrb r6, [r0, #1]
	cmp r1, r6
	bhs _020C3878
	ldrh r6, [r0, #6]
	add r8, r0, r6
	ldrh r6, [r8, #2]
	add r6, r8, r6
	add r8, r6, r4
	b _020C387C
_020C3878:
	mov r8, r7
_020C387C:
	ldr r6, [r8]
	cmp r6, r2
	ldreq r6, [r8, #4]
	cmpeq r6, r3
	ldreq r6, [r8, #8]
	cmpeq r6, ip
	ldreq r6, [r8, #0xc]
	cmpeq r6, lr
	bne _020C38D0
	cmp r0, #0
	beq _020C38C8
	cmp r1, r5
	bhs _020C38C8
	ldrh r3, [r0, #6]
	ldrh r2, [r0, r3]
	add r0, r0, r3
	add r0, r0, #4
	mla r0, r2, r1, r0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_020C38C8:
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_020C38D0:
	ldrb r6, [r0, #1]
	add r1, r1, #1
	add r4, r4, #0x10
	cmp r1, r6
	blo _020C384C
	b _020C39D0
_020C38E8:
	add r4, r0, #8
	ldrb r3, [r4, #1]
	cmp r3, #0
	beq _020C39D0
	ldrb r7, [r4, r3, lsl #2]
	ldrb r2, [r4]
	add r6, r4, r3, lsl #2
	cmp r2, r7
	bls _020C393C
_020C390C:
	mov r2, r7, asr #5
	ldr r3, [r1, r2, lsl #2]
	and r2, r7, #0x1f
	mov r2, r3, lsr r2
	and r2, r2, #1
	add r2, r6, r2
	ldrb r3, [r2, #1]
	ldrb r2, [r6]
	ldrb r7, [r4, r3, lsl #2]
	add r6, r4, r3, lsl #2
	cmp r2, r7
	bhi _020C390C
_020C393C:
	cmp r0, #0
	ldrb r2, [r6, #3]
	beq _020C3968
	cmp r2, r5
	bhs _020C3968
	ldrh r3, [r0, #6]
	add r4, r0, r3
	ldrh r3, [r4, #2]
	add r3, r4, r3
	add r6, r3, r2, lsl #4
	b _020C396C
_020C3968:
	mov r6, #0
_020C396C:
	ldr r4, [r6]
	ldr r3, [r1]
	cmp r4, r3
	ldreq r4, [r6, #4]
	ldreq r3, [r1, #4]
	cmpeq r4, r3
	ldreq r4, [r6, #8]
	ldreq r3, [r1, #8]
	cmpeq r4, r3
	ldreq r3, [r6, #0xc]
	ldreq r1, [r1, #0xc]
	cmpeq r3, r1
	bne _020C39D0
	cmp r0, #0
	beq _020C39C8
	cmp r2, r5
	bhs _020C39C8
	ldrh r3, [r0, #6]
	ldrh r1, [r0, r3]
	add r0, r0, r3
	add r0, r0, #4
	mla r0, r1, r2, r0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_020C39C8:
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_020C39D0:
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	arm_func_end NNS_G3dGetResDataByName

	arm_func_start NNS_G3dGetResDictIdxByName
NNS_G3dGetResDictIdxByName: ; 0x020C39D8
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	cmp r1, #0
	mvneq r0, #0
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	ldrb r2, [r0, #1]
	cmp r2, #0x10
	bhs _020C3A7C
	cmp r2, #0
	ldmia r1, {r2, r3, ip, lr}
	mov r1, #0
	bls _020C3B38
	mov r4, r1
	mov r6, r1
_020C3A0C:
	cmp r0, #0
	beq _020C3A38
	ldrb r5, [r0, #1]
	cmp r1, r5
	bhs _020C3A38
	ldrh r5, [r0, #6]
	add r7, r0, r5
	ldrh r5, [r7, #2]
	add r5, r7, r5
	add r7, r5, r4
	b _020C3A3C
_020C3A38:
	mov r7, r6
_020C3A3C:
	ldr r5, [r7]
	cmp r5, r2
	ldreq r5, [r7, #4]
	cmpeq r5, r3
	ldreq r5, [r7, #8]
	cmpeq r5, ip
	ldreq r5, [r7, #0xc]
	cmpeq r5, lr
	moveq r0, r1
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	ldrb r5, [r0, #1]
	add r1, r1, #1
	add r4, r4, #0x10
	cmp r1, r5
	blo _020C3A0C
	b _020C3B38
_020C3A7C:
	add r5, r0, #8
	ldrb r4, [r5, #1]
	cmp r4, #0
	beq _020C3B38
	ldrb r7, [r5, r4, lsl #2]
	ldrb r3, [r5]
	add r6, r5, r4, lsl #2
	cmp r3, r7
	bls _020C3AD0
_020C3AA0:
	mov r3, r7, asr #5
	ldr r4, [r1, r3, lsl #2]
	and r3, r7, #0x1f
	mov r3, r4, lsr r3
	and r3, r3, #1
	add r3, r6, r3
	ldrb r4, [r3, #1]
	ldrb r3, [r6]
	ldrb r7, [r5, r4, lsl #2]
	add r6, r5, r4, lsl #2
	cmp r3, r7
	bhi _020C3AA0
_020C3AD0:
	cmp r0, #0
	ldrb r3, [r6, #3]
	beq _020C3AFC
	cmp r3, r2
	bhs _020C3AFC
	ldrh r2, [r0, #6]
	add r2, r0, r2
	ldrh r0, [r2, #2]
	add r0, r2, r0
	add r4, r0, r3, lsl #4
	b _020C3B00
_020C3AFC:
	mov r4, #0
_020C3B00:
	ldr r2, [r4]
	ldr r0, [r1]
	cmp r2, r0
	ldreq r2, [r4, #4]
	ldreq r0, [r1, #4]
	cmpeq r2, r0
	ldreq r2, [r4, #8]
	ldreq r0, [r1, #8]
	cmpeq r2, r0
	ldreq r2, [r4, #0xc]
	ldreq r0, [r1, #0xc]
	cmpeq r2, r0
	moveq r0, r3
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
_020C3B38:
	mvn r0, #0
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	arm_func_end NNS_G3dGetResDictIdxByName
