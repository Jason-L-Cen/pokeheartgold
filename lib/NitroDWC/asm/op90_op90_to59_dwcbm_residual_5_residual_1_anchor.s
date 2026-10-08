	.include "asm/macros.inc"
	.include "dwcbm.inc"
	.include "global.inc"
	.extern DWCi_BM_GetWiFiInfo
	.extern DWCi_util_wifiid_exctable
	.extern DWCi_util_wifiid_ttable
	.public DWCi_Util_WiFiId_scrambleUid

.text
