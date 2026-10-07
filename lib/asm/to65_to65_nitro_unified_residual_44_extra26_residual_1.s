	.extern AgbFlash
	.extern CTRDGi_PollingSR
	.extern CTRDGi_SetFlashBankMx
	.extern OS_LockCartridge
	.extern OS_UnlockCartridge
	.extern _021E4F68
	.extern _021E4F6C
	.extern _021E4F90
	.extern ctrdg_flash_remainder
	.extern ctrdgi_backup_irq
	.extern ctrdgi_flash_lock_id
	.include "asm/macros.inc"
	.include "nitro.inc"
	.include "global.inc"
	.public CTRDGi_EraseFlashChipCoreLE
	.public CTRDGi_EraseFlashSectorCoreLE
.text
	arm_func_start CTRDGi_EraseFlashChipCoreLE
CTRDGi_EraseFlashChipCoreLE: ; 0x020E2844
	stmdb sp!, {r3, r4, r5, lr}
	ldr r0, _020E2910 ; =ctrdgi_flash_lock_id
	ldrh r0, [r0]
	bl OS_LockCartridge
	ldr r0, _020E2914 ; =0x04000204
	ldr r1, _020E2918 ; =AgbFlash
	ldrh r3, [r0]
	ldr r2, [r1]
	ldrh r1, [r0]
	ldr r4, [r2, #0x10]
	mov r2, #0
	bic r1, r1, #3
	orr r1, r1, r4
	strh r1, [r0]
	ldrh r4, [r0, #4]
	ldr r1, _020E291C ; =ctrdgi_backup_irq
	ldr lr, _020E2920 ; =0x0A005555
	strh r2, [r0, #4]
	str r4, [r1]
	mov r5, #0xaa
	ldr r4, _020E2924 ; =0x0A002AAA
	strb r5, [lr]
	mov ip, #0x55
	strb ip, [r4]
	mov r2, #0x80
	strb r2, [lr]
	strb r5, [lr]
	strb ip, [r4]
	mov r2, #0x10
	strb r2, [lr]
	ldrh r2, [r0, #4]
	ldr r2, [r1]
	ldr r1, _020E2928 ; =CTRDGi_PollingSR
	strh r2, [r0, #4]
	ldr r4, [r1]
	mov r0, #3
	mov r1, #0xa000000
	mov r2, #0xff
	and r5, r3, #3
	blx r4
	ldr r3, _020E2914 ; =0x04000204
	ldr r1, _020E2910 ; =ctrdgi_flash_lock_id
	ldrh r2, [r3]
	mov r4, r0
	bic r0, r2, #3
	orr r0, r0, r5
	strh r0, [r3]
	ldrh r0, [r1]
	bl OS_UnlockCartridge
	mov r0, r4
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_020E2910: .word ctrdgi_flash_lock_id
_020E2914: .word 0x04000204
_020E2918: .word AgbFlash
_020E291C: .word ctrdgi_backup_irq
_020E2920: .word 0x0A005555
_020E2924: .word 0x0A002AAA
_020E2928: .word CTRDGi_PollingSR
	arm_func_end CTRDGi_EraseFlashChipCoreLE

	arm_func_start CTRDGi_EraseFlashSectorCoreLE
CTRDGi_EraseFlashSectorCoreLE: ; 0x020E292C
	stmdb sp!, {r3, r4, r5, r6, lr}
	sub sp, sp, #0x24
	mov r5, r0
	ldmia r5!, {r0, r1, r2, r3}
	add r4, sp, #0
	stmia r4!, {r0, r1, r2, r3}
	ldmia r5!, {r0, r1, r2, r3}
	stmia r4!, {r0, r1, r2, r3}
	ldr r0, [r5]
	str r0, [r4]
	ldrh r4, [sp, #0x20]
	cmp r4, #0x10
	addhs sp, sp, #0x24
	ldrhs r0, _020E2A40 ; =0x000080FF
	ldmhsia sp!, {r3, r4, r5, r6, pc}
	ldr r0, _020E2A44 ; =ctrdgi_flash_lock_id
	ldrh r0, [r0]
	bl OS_LockCartridge
	ldr r0, _020E2A48 ; =0x04000204
	ldr r1, _020E2A4C ; =AgbFlash
	ldrh r3, [r0]
	ldr r5, [r1]
	ldrh r2, [r0]
	ldr r6, [r5, #0x10]
	mov r5, #0
	bic r2, r2, #3
	orr r2, r2, r6
	strh r2, [r0]
	ldr r2, [r1]
	ldrh r1, [r0, #4]
	ldrh ip, [r2, #8]
	ldr r2, _020E2A50 ; =ctrdgi_backup_irq
	ldr lr, _020E2A54 ; =0x0A005555
	strh r5, [r0, #4]
	mov r6, r4, lsl ip
	str r1, [r2]
	mov r5, #0xaa
	ldr r4, _020E2A58 ; =0x0A002AAA
	strb r5, [lr]
	mov ip, #0x55
	strb ip, [r4]
	mov r1, #0x80
	strb r1, [lr]
	strb r5, [lr]
	strb ip, [r4]
	add r1, r6, #0xa000000
	mov r4, #0x30
	strb r4, [r1]
	ldrh r4, [r0, #4]
	ldr r4, [r2]
	ldr r2, _020E2A5C ; =CTRDGi_PollingSR
	strh r4, [r0, #4]
	ldr r4, [r2]
	mov r0, #2
	mov r2, #0xff
	and r5, r3, #3
	blx r4
	ldr r3, _020E2A48 ; =0x04000204
	ldr r1, _020E2A44 ; =ctrdgi_flash_lock_id
	ldrh r2, [r3]
	mov r4, r0
	bic r0, r2, #3
	orr r0, r0, r5
	strh r0, [r3]
	ldrh r0, [r1]
	bl OS_UnlockCartridge
	mov r0, r4
	add sp, sp, #0x24
	ldmia sp!, {r3, r4, r5, r6, pc}
	.align 2, 0
_020E2A40: .word 0x000080FF
_020E2A44: .word ctrdgi_flash_lock_id
_020E2A48: .word 0x04000204
_020E2A4C: .word AgbFlash
_020E2A50: .word ctrdgi_backup_irq
_020E2A54: .word 0x0A005555
_020E2A58: .word 0x0A002AAA
_020E2A5C: .word CTRDGi_PollingSR
	arm_func_end CTRDGi_EraseFlashSectorCoreLE
