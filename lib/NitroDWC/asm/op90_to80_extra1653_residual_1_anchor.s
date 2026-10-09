	.extern Callback_NVRAM
	.extern DWC_BACKUPlCheckAddress
	.extern DWC_BACKUPlCheckIp
	.extern DWC_BACKUPlCheckSsid
	.extern DWCi_AUTH_GetNewWiFiInfo
	.extern DWCi_BACKUPlConvMaskAddr
	.extern DWCi_BACKUPlInit
	.extern DWCi_BACKUPlRead
	.extern DWCi_BACKUPlSetWiFi
	.extern DWCi_BACKUPlWriteAll
	.extern DWCi_SETTING_NONE
	.extern Wifi
	.extern Work
	.extern _021D43C8
	.extern initPage
	.include "asm/macros.inc"
	.include "dwcbm.inc"
	.include "global.inc"
	.public NVRAMm_ExecuteCommand

.text
