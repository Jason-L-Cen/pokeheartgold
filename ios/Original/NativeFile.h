// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef HEARTGOLD_NATIVE_FILE_H
#define HEARTGOLD_NATIVE_FILE_H
#include <nitro/types.h>

#ifdef __cplusplus
extern "C" {
#endif
// Configure before starting game/worker threads; open handles prohibit reconfiguration.
BOOL HGNativeFSSetRoot(const char *path);
#ifdef __cplusplus
}
#endif
#endif
