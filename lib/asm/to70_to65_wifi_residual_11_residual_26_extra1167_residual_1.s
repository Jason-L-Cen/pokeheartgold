	.extern CPSi_big_from_char
	.extern CPSi_big_power
	.extern CPSi_char_from_big
	.extern SOCL_Accept
	.extern SOCL_Bind
	.extern SOCL_Cleanup
	.extern SOCL_Close
	.extern SOCL_Connect
	.extern SOCL_CreateSocket
	.extern SOCL_GetHostID
	.extern SOCL_GetStatus
	.extern SOCL_InetAtoH
	.extern SOCL_Listen
	.extern SOCL_ReadFrom
	.extern SOCL_Resolve
	.extern SOCL_SetResolver
	.extern SOCL_Shutdown
	.extern SOCL_SocketIsInvalid
	.extern SOCL_Startup
	.extern SOCL_WriteTo
	.extern SOCLi_CreateCommandPacket
	.extern SOCLi_ExecCommandPacketInRecvPipe
	.extern SOCLi_SocketGetNextPtr
	.extern _02108FB8
	.extern _02108FC0
	.extern _02108FC8
	.extern _02110630
	.extern _02110648
	.extern _02110678
	.extern _02110684
	.extern _02110688
	.extern _02110690
	.extern _0211069C
	.extern _021106B0
	.extern _021106B8
	.extern _021106FC
	.extern _02110700
	.extern _02110704
	.extern _02110708
	.extern _02110710
	.extern _02110718
	.extern _02110758
	.extern _02110798
	.extern _02110898
	.extern _02110899
	.extern _021D4490
	.extern _021D4494
	.extern _021D44AC
	.extern _021D44B4
	.extern _021D44D4
	.extern _021D450C
	.extern _021D451C
	.extern _021D4620
	.extern _021D4634
	.extern _021D4660
	.extern _021D4680
	.extern _021D4688
	.extern _021D46A0
	.extern _021D46AA
	.extern _021D4700
	.extern _021D4720
	.extern _021D4764
	.extern _021D47C8
	.extern _021D4888
	.extern _021D4948
	.extern _021D4B08
	.extern _021D4B32
	.extern _021D4B4C
	.extern _021D4B76
	.extern _021D4CCC
	.extern _021D5660
	.extern _021D5E60
	.extern _021D5E68
	.extern _021D5FD8
	.extern _021D5FEB
	.extern _021D5FEC
	.extern _021D5FF0
	.extern _021D5FF4
	.extern _021D5FFC
	.extern _021D601C
	.extern _021D6020
	.extern cert_item_len
	.extern enter_computebound
	.extern exit_computebound
	.extern make_dn
	.include "asm/macros.inc"
	.include "wifi.inc"
	.include "global.inc"
	.extern parse_time

.text
	arm_func_start cert_item
cert_item: ; 0x020A8A98
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #8
	mov fp, r1
	ldr r1, [fp]
	mov sb, r0
	add r0, r1, #1
	str r0, [sp, #4]
	add r0, sp, #4
	mov r7, r2
	mov r6, r3
	ldr r8, [sp, #0x30]
	ldrb r5, [r1]
	bl cert_item_len
	movs r4, r0
	bmi _020A8ADC
	cmp r4, #0x7d0
	ble _020A8AE8
_020A8ADC:
	add sp, sp, #8
	mov r0, #1
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020A8AE8:
	and r1, r5, #0x1f
	cmp r1, #0x18
	addls pc, pc, r1, lsl #2
	b _020A8F0C
_020A8AF8: ; jump table
	b _020A8F0C ; case 0
	b _020A8F0C ; case 1
	b _020A8B5C ; case 2
	b _020A8C54 ; case 3
	b _020A8F0C ; case 4
	b _020A8F0C ; case 5
	b _020A8CD4 ; case 6
	b _020A8F0C ; case 7
	b _020A8F0C ; case 8
	b _020A8F0C ; case 9
	b _020A8F0C ; case 10
	b _020A8F0C ; case 11
	b _020A8D6C ; case 12
	b _020A8F0C ; case 13
	b _020A8F0C ; case 14
	b _020A8F0C ; case 15
	b _020A8E3C ; case 16
	b _020A8EBC ; case 17
	b _020A8F0C ; case 18
	b _020A8D6C ; case 19
	b _020A8D6C ; case 20
	b _020A8F0C ; case 21
	b _020A8D6C ; case 22
	b _020A8DEC ; case 23
	b _020A8DEC ; case 24
_020A8B5C:
	ldrb r0, [sb, #0x5ad]
	cmp r0, #0
	beq _020A8C44
	cmp r6, #0
	bne _020A8BDC
	ldr r0, [sp, #4]
	ldrb r1, [r0]
	cmp r1, #0
	bne _020A8B98
_020A8B80:
	add r0, r0, #1
	str r0, [sp, #4]
	ldrb r1, [r0]
	sub r4, r4, #1
	cmp r1, #0
	beq _020A8B80
_020A8B98:
	cmp r8, #0
	beq _020A8BAC
	cmp r8, #2
	beq _020A8BCC
	b _020A8C44
_020A8BAC:
	cmp r4, #0x100
	bgt _020A8C44
	add r1, sb, #0x94
	mov r2, r4
	add r1, r1, #0x400
	bl MI_CpuCopy8
	str r4, [sb, #0x594]
	b _020A8C44
_020A8BCC:
	str r4, [sb, #0x484]
	ldr r0, [sp, #4]
	str r0, [sb, #0x488]
	b _020A8C44
_020A8BDC:
	cmp r6, #1
	bne _020A8C44
	ldr r0, [sp, #4]
	ldrb r1, [r0]
	cmp r1, #0
	bne _020A8C0C
_020A8BF4:
	add r0, r0, #1
	str r0, [sp, #4]
	ldrb r1, [r0]
	sub r4, r4, #1
	cmp r1, #0
	beq _020A8BF4
_020A8C0C:
	cmp r8, #0
	beq _020A8C28
	cmp r8, #2
	streq r4, [sb, #0x48c]
	ldreq r0, [sp, #4]
	streq r0, [sb, #0x490]
	b _020A8C44
_020A8C28:
	cmp r4, #8
	bgt _020A8C44
	add r1, sb, #0x198
	mov r2, r4
	add r1, r1, #0x400
	bl MI_CpuCopy8
	str r4, [sb, #0x5a0]
_020A8C44:
	ldr r0, [sp, #4]
	add r0, r0, r4
	str r0, [sp, #4]
	b _020A8F70
_020A8C54:
	cmp r7, #1
	bne _020A8C78
	cmp r8, #2
	beq _020A8C78
	ldr r1, [sp, #4]
	sub r0, r4, #1
	add r1, r1, #1
	str r1, [sb, #0x5a4]
	str r0, [sb, #0x5a8]
_020A8C78:
	ldrb r0, [sb, #0x5ad]
	cmp r0, #0
	beq _020A8CC4
	ldr r0, [sp, #4]
	add r1, sp, #4
	add r0, r0, #1
	str r0, [sp, #4]
	mov r0, sb
	mov r2, r7
	mov r3, #0
	str r8, [sp]
	bl cert_item
	cmp r0, #0
	addne sp, sp, #8
	movne r0, #1
	ldmneia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	mov r0, #0
	strb r0, [sb, #0x5ad]
	b _020A8F70
_020A8CC4:
	ldr r0, [sp, #4]
	add r0, r0, r4
	str r0, [sp, #4]
	b _020A8F70
_020A8CD4:
	ldr r6, [sp, #4]
	ldr sl, _020A8F84 ; =_021106B8
	mov r5, #0
_020A8CE0:
	ldr r7, [sl, r5, lsl #2]
	mov r0, r7
	bl strlen
	mov r2, r0
	mov r0, r6
	mov r1, r7
	bl memcmp
	cmp r0, #0
	bne _020A8D50
	cmp r5, #5
	addls pc, pc, r5, lsl #2
	b _020A8D5C
_020A8D10: ; jump table
	b _020A8D5C ; case 0
	b _020A8D28 ; case 1
	b _020A8D28 ; case 2
	b _020A8D38 ; case 3
	b _020A8D38 ; case 4
	b _020A8D44 ; case 5
_020A8D28:
	cmp r8, #0
	streq r5, [sb, #0x45c]
	strb r5, [sb, #0x5ad]
	b _020A8D5C
_020A8D38:
	cmp r8, #2
	strne r5, [sb, #0x458]
	b _020A8D5C
_020A8D44:
	cmp r8, #2
	strneb r5, [sb, #0x5ae]
	b _020A8D5C
_020A8D50:
	add r5, r5, #1
	cmp r5, #6
	blt _020A8CE0
_020A8D5C:
	ldr r0, [sp, #4]
	add r0, r0, r4
	str r0, [sp, #4]
	b _020A8F70
_020A8D6C:
	cmp r8, #2
	beq _020A8DD4
	ldrb r0, [sb, #0x5ac]
	cmp r0, #0
	beq _020A8DC4
	ldr r1, [sp, #4]
	mov r2, r4
	add r0, sb, #0x6b0
	bl make_dn
	ldrb r0, [sb, #0x5ae]
	cmp r0, #5
	bne _020A8DD4
	cmp r4, #0x4f
	bgt _020A8DD4
	ldr r0, [sp, #4]
	mov r2, r4
	add r1, sb, #0x7b0
	bl MI_CpuCopy8
	add r0, sb, r4
	mov r1, #0
	strb r1, [r0, #0x7b0]
	b _020A8DD4
_020A8DC4:
	ldr r1, [sp, #4]
	mov r2, r4
	add r0, sb, #0x5b0
	bl make_dn
_020A8DD4:
	mov r0, #0
	strb r0, [sb, #0x5ae]
	ldr r0, [sp, #4]
	add r0, r0, r4
	str r0, [sp, #4]
	b _020A8F70
_020A8DEC:
	cmp r8, #2
	beq _020A8E24
	ldr r0, [sp, #4]
	bl parse_time
	cmp r6, #0
	ldr r1, [sb, #0x80c]
	bne _020A8E18
	cmp r1, r0
	movhs r0, #1
	strhsb r0, [sb, #0x5af]
	b _020A8E24
_020A8E18:
	cmp r1, r0
	movhi r0, #0
	strhib r0, [sb, #0x5af]
_020A8E24:
	ldr r1, [sp, #4]
	mov r0, #1
	add r1, r1, r4
	str r1, [sp, #4]
	strb r0, [sb, #0x5ac]
	b _020A8F70
_020A8E3C:
	cmp r7, #0
	cmpeq r6, #0
	bne _020A8E54
	cmp r8, #2
	ldrne r0, [sp, #4]
	strne r0, [sb, #0x460]
_020A8E54:
	ldr r0, [sp, #4]
	mov r5, #0
	add sl, r0, r4
	cmp r0, sl
	bhs _020A8EA4
	add r4, sp, #4
_020A8E6C:
	mov r0, sb
	mov r1, r4
	mov r3, r5
	add r2, r7, #1
	str r8, [sp]
	bl cert_item
	cmp r0, #0
	add r5, r5, #1
	addne sp, sp, #8
	movne r0, #1
	ldmneia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldr r0, [sp, #4]
	cmp r0, sl
	blo _020A8E6C
_020A8EA4:
	cmp r7, #1
	cmpeq r6, #0
	bne _020A8F70
	cmp r8, #2
	strne r0, [sb, #0x464]
	b _020A8F70
_020A8EBC:
	ldr r0, [sp, #4]
	add r6, r0, r4
	cmp r0, r6
	bhs _020A8F70
	add r5, sp, #4
	mov r4, #0
_020A8ED4:
	mov r0, sb
	mov r1, r5
	mov r3, r4
	add r2, r7, #1
	str r8, [sp]
	bl cert_item
	cmp r0, #0
	addne sp, sp, #8
	movne r0, #1
	ldmneia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldr r0, [sp, #4]
	cmp r0, r6
	blo _020A8ED4
	b _020A8F70
_020A8F0C:
	cmp r5, #0xa0
	bne _020A8F64
	ldr r0, [sp, #4]
	add r6, r0, r4
	cmp r0, r6
	bhs _020A8F70
	add r5, sp, #4
	mov r4, #0
_020A8F2C:
	mov r0, sb
	mov r1, r5
	mov r3, r4
	add r2, r7, #1
	str r8, [sp]
	bl cert_item
	cmp r0, #0
	addne sp, sp, #8
	movne r0, #1
	ldmneia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldr r0, [sp, #4]
	cmp r0, r6
	blo _020A8F2C
	b _020A8F70
_020A8F64:
	ldr r0, [sp, #4]
	add r0, r0, r4
	str r0, [sp, #4]
_020A8F70:
	ldr r1, [sp, #4]
	mov r0, #0
	str r1, [fp]
	add sp, sp, #8
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_020A8F84: .word _021106B8
	arm_func_end cert_item
