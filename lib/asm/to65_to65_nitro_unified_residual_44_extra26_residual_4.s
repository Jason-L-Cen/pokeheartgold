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
	.public CTRDGi_EraseFlashChipCoreMX
	.public CTRDGi_EraseFlashSectorCoreMX
.text
	arm_func_start CTRDGi_EraseFlashChipCoreMX
CTRDGi_EraseFlashChipCoreMX: ; 0x020E2E24
	stmdb sp!, {r3, r4, r5, lr}
	ldr r0, _020E2EF0 ; =ctrdgi_flash_lock_id
	ldrh r0, [r0]
	bl OS_LockCartridge
	ldr r0, _020E2EF4 ; =0x04000204
	ldr r1, _020E2EF8 ; =AgbFlash
	ldrh r3, [r0]
	ldr r2, [r1]
	ldrh r1, [r0]
	ldr r4, [r2, #0x10]
	mov r2, #0
	bic r1, r1, #3
	orr r1, r1, r4
	strh r1, [r0]
	ldrh r4, [r0, #4]
	ldr r1, _020E2EFC ; =ctrdgi_backup_irq
	ldr lr, _020E2F00 ; =0x0A005555
	strh r2, [r0, #4]
	str r4, [r1]
	mov r5, #0xaa
	ldr r4, _020E2F04 ; =0x0A002AAA
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
	ldr r1, _020E2F08 ; =CTRDGi_PollingSR
	strh r2, [r0, #4]
	ldr r4, [r1]
	mov r0, #3
	mov r1, #0xa000000
	mov r2, #0xff
	and r5, r3, #3
	blx r4
	ldr r3, _020E2EF4 ; =0x04000204
	ldr r1, _020E2EF0 ; =ctrdgi_flash_lock_id
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
_020E2EF0: .word ctrdgi_flash_lock_id
_020E2EF4: .word 0x04000204
_020E2EF8: .word AgbFlash
_020E2EFC: .word ctrdgi_backup_irq
_020E2F00: .word 0x0A005555
_020E2F04: .word 0x0A002AAA
_020E2F08: .word CTRDGi_PollingSR
	arm_func_end CTRDGi_EraseFlashChipCoreMX

	arm_func_start CTRDGi_EraseFlashSectorCoreMX
CTRDGi_EraseFlashSectorCoreMX: ; 0x020E2F0C
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x28
	mov r5, r0
	ldmia r5!, {r0, r1, r2, r3}
	add r4, sp, #4
	stmia r4!, {r0, r1, r2, r3}
	ldmia r5!, {r0, r1, r2, r3}
	stmia r4!, {r0, r1, r2, r3}
	ldr r0, _020E3074 ; =AgbFlash
	ldr r1, [r5]
	ldr r0, [r0]
	str r1, [r4]
	ldrh r4, [sp, #0x24]
	ldrh r0, [r0, #0xa]
	cmp r4, r0
	addhs sp, sp, #0x28
	ldrhs r0, _020E3078 ; =0x000080FF
	ldmhsia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldr r0, _020E307C ; =ctrdgi_flash_lock_id
	ldrh r0, [r0]
	bl OS_LockCartridge
	ldr r5, _020E3080 ; =0x04000204
	ldr r0, _020E3074 ; =AgbFlash
	ldrh r3, [r5]
	ldr r2, [r0]
	ldrh r1, [r5]
	mov r0, r4, lsl #0xc
	ldr r2, [r2, #0x10]
	bic r1, r1, #3
	orr r1, r1, r2
	strh r1, [r5]
	and r1, r3, #3
	mov r0, r0, lsr #0x10
	str r1, [sp]
	bl CTRDGi_SetFlashBankMx
	and sl, r4, #0xf
	mov r8, #0
	add r7, r5, #4
	ldr r5, _020E3084 ; =0x0A005555
	ldr r4, _020E3088 ; =ctrdgi_backup_irq
	mov fp, r8
	mov r6, #0xaa
_020E2FB4:
	ldr r0, _020E3074 ; =AgbFlash
	ldrh r3, [r7]
	ldr r0, [r0]
	mov sb, #0x55
	ldrh r1, [r0, #8]
	mov r0, #2
	mov r2, #0xff
	strh fp, [r7]
	str r3, [r4]
	ldr r3, _020E308C ; =0x0A002AAA
	strb r6, [r5]
	strb sb, [r3]
	mov r3, #0x80
	strb r3, [r5]
	mov r1, sl, lsl r1
	ldr r3, _020E308C ; =0x0A002AAA
	strb r6, [r5]
	strb sb, [r3]
	add r1, r1, #0xa000000
	mov r3, #0x30
	strb r3, [r1]
	ldrh r3, [r7]
	ldr r3, [r4]
	strh r3, [r7]
	ldr r3, _020E3090 ; =CTRDGi_PollingSR
	ldr r3, [r3]
	blx r3
	mov sb, r0
	tst sb, #0xa000
	beq _020E3044
	cmp r8, #0
	bne _020E3044
	add r0, r8, #1
	mov r0, r0, lsl #0x10
	mov r8, r0, lsr #0x10
	b _020E2FB4
_020E3044:
	ldr r2, _020E3080 ; =0x04000204
	ldr r1, _020E307C ; =ctrdgi_flash_lock_id
	ldrh r0, [r2]
	bic r3, r0, #3
	ldr r0, [sp]
	orr r0, r3, r0
	strh r0, [r2]
	ldrh r0, [r1]
	bl OS_UnlockCartridge
	mov r0, sb
	add sp, sp, #0x28
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_020E3074: .word AgbFlash
_020E3078: .word 0x000080FF
_020E307C: .word ctrdgi_flash_lock_id
_020E3080: .word 0x04000204
_020E3084: .word 0x0A005555
_020E3088: .word ctrdgi_backup_irq
_020E308C: .word 0x0A002AAA
_020E3090: .word CTRDGi_PollingSR
	arm_func_end CTRDGi_EraseFlashSectorCoreMX
