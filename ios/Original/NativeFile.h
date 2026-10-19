// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef HEARTGOLD_NATIVE_FILE_H
#define HEARTGOLD_NATIVE_FILE_H
#include <nitro/types.h>

#ifdef __cplusplus
extern "C" {
#endif
// Configure before starting game/worker threads; open handles prohibit reconfiguration.
BOOL HGNativeFSSetRoot(const char *path);
// Resolve a checked file/directory inside the configured root to its relative canonical path.
BOOL HGNativeFSNormalizePath(const char *path, BOOL directory, char *relative, u32 capacity);
typedef struct { BOOL directory; char name[128]; } HGNativeDirEntry;
// Release sorted snapshots with HGNativeFSFreeDirectory, including empty directories.
BOOL HGNativeFSReadDirectory(const char *path, HGNativeDirEntry **entries, u32 *count);
void HGNativeFSFreeDirectory(HGNativeDirEntry *entries);
#ifdef __cplusplus
}
#endif
#endif
