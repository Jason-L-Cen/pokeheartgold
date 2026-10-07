	.extern CPSi_big_add
	.extern CPSi_big_compare
	.extern CPSi_big_mult
	.extern CPSi_big_mult_small
	.extern CPSi_big_sign
	.extern CPSi_big_sqr
	.extern CPSi_big_sub
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
	.extern count_digits
	.extern get48bits_1
	.include "asm/macros.inc"
	.include "wifi.inc"
	.include "global.inc"
	.public get48bits_2
	.public get48bits_3
	.public get64bits

.text
	arm_func_start get48bits_2
get48bits_2: ; 0x020ACF7C
	ldrh r1, [r0]
	ldrh r0, [r0, #-2]
	mov r0, r0, lsl #0x10
	bx lr
	arm_func_end get48bits_2

	arm_func_start get48bits_3
get48bits_3: ; 0x020ACF8C
	ldrh r1, [r0]
	ldrh r2, [r0, #-2]
	ldrh r3, [r0, #-4]
	orr r0, r3, r2, lsl #16
	bx lr
	arm_func_end get48bits_3

	arm_func_start get64bits
get64bits: ; 0x020ACFA0
	ldrh r2, [r0]
	ldrh r3, [r0, #-2]
	orr r1, r3, r2, lsl #16
	ldrh r2, [r0, #-4]
	ldrh r3, [r0, #-6]
	orr r0, r3, r2, lsl #16
	bx lr
	arm_func_end get64bits
