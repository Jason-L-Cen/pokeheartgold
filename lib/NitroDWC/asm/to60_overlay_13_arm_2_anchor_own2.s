	.include "asm/macros.inc"
	.include "overlay_13_arm_2.inc"
	.include "global.inc"
	.public gWifiSetupClient
	.public ov13_0224F4C8
	.public gWifiSetupApp
	.public gWifiSetupTextBanks
	.public gWifiSetupSession
	.public ov13_0224F4F4
	.public ov13_0224F4F8
	.public ov13_0224F4FC
	.public ov13_0224F504
	.public ov13_0224F508
	.public ov13_0224F514
	.public ov13_0224F518
	.public ov13_0224F51C
	.public ov13_0224F520
	.public ov13_0224F524
	.public ov13_0224F528
	.public ov13_0224F52C
	.public ov13_0224F534
	.public gWifiSetupListScreen
	.public ov13_0224F53C
	.public gWifiSetupTextEntry
	.public gWifiSetupAddressEntry
	.public gWifiSetupErrorCode
	.public gWifiSetupSettings
	.public ov13_0224F554
	.public gWifiSetupAutoConnectDone
	.public gWifiSetupAutoReceiveDone
	.public gWifiSetupAutoDoneTimer
	.public gWifiSetupAutoSearchDone
	.public gWifiSetupApScanScreen
	.public gWifiSetupConnectionList
	.public ov13_0224F570
	.public gWifiSetupMethodMenu
	.public gWifiSetupConnectionTestPassed
	.public gWifiSetupConnectionOkTimer
	.public gWifiSetupUsbConnectorDoneTimer
	.public gWifiSetupUsbConnectorIntroNext
	.public gWifiSetupUsbConnectorExit
	.bss
gWifiSetupClient: ; 0x0224F4C4
	.space 0x4

ov13_0224F4C8: ; 0x0224F4C8
	.space 0x4

gWifiSetupApp: ; 0x0224F4CC
	.space 0x20

gWifiSetupTextBanks: ; 0x0224F4EC
	.space 0x4

