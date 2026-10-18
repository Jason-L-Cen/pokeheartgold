// SPDX-License-Identifier: GPL-3.0-or-later
#include "NativeBackup.h"
#include "assert.h"
#include <dwc/dwc_bm_internal.h>

void DWC_Auth_GetId(u32 *identifier) {
    u64 console = HGNativeBackupConsoleId();
    GF_ASSERT(identifier && console);
    identifier[0] = identifier[1] = 0;
    identifier[2] = (u32)console; identifier[3] = (u32)(console >> 32);
    identifier[4] = FALSE; // A local pseudo ID does not claim registration with Nintendo's authentication service.
}
