// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef NITRO_FS_NATIVE_FILE_H
#define NITRO_FS_NATIVE_FILE_H
#include <nitro/types.h>
enum {
    HG_FILE_AVAILABLE, HG_FILE_INIT, HG_FILE_RESOLVE, HG_FILE_OPEN, HG_FILE_OPEN_RANGE,
    HG_FILE_CLOSE, HG_FILE_READ, HG_FILE_READ_ASYNC, HG_FILE_SEEK, HG_FILE_WAIT, HG_FILE_CHANGE_DIR,
    HG_FILE_RESOLVE_AT, HG_FILE_SEEK_DIR, HG_FILE_READ_DIR, HG_FILE_GET_PATH
};
#ifdef HEARTGOLD_ABI32
u32 HGNativeFileRequest(u32 command,u32 file,u32 first,u32 second,u32 third)
    __attribute__((import_module("env"),import_name("HGNativeFileRequest")));
#endif
#endif
