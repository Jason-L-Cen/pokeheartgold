// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef HEARTGOLD_NATIVE_BACKUP_H
#define HEARTGOLD_NATIVE_BACKUP_H
#include <nitro/types.h>
#ifdef __cplusplus
extern "C" {
#endif
// Configure before the game locks the backup. NULL closes it; existing images must be exactly 512 KiB.
BOOL HGNativeBackupSetPath(const char *path);
u64 HGNativeBackupConsoleId(void);
// Original DWC's four settings pages, stored beside the selected save (.wifi).
BOOL HGNativeBackupSettings(u32 offset, void *buffer, u32 size, BOOL write);
// Nonblocking lease for the native game kernel; keeps the selected image fixed through its lifetime.
BOOL HGNativeBackupTryLock(u16 id);
#ifdef __cplusplus
}
#endif
#endif
