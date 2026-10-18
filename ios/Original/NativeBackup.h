// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef HEARTGOLD_NATIVE_BACKUP_H
#define HEARTGOLD_NATIVE_BACKUP_H
#include <nitro/types.h>
#ifdef __cplusplus
extern "C" {
#endif
// Configure before the game locks the backup. NULL closes it; existing images must be exactly 512 KiB.
BOOL HGNativeBackupSetPath(const char *path);
#ifdef __cplusplus
}
#endif
#endif
