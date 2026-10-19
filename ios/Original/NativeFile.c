// SPDX-License-Identifier: GPL-3.0-or-later
#include "NativeFile.h"
#include <nitro/fs/file.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <stdatomic.h>

static char contentRoot[PATH_MAX];
static atomic_uint openFiles;

BOOL HGNativeFSSetRoot(const char *path) {
    char resolved[PATH_MAX];
    struct stat info;
    if (atomic_load(&openFiles) || !path || !realpath(path, resolved) || stat(resolved, &info) || !S_ISDIR(info.st_mode)) return FALSE;
    memcpy(contentRoot, resolved, strlen(resolved) + 1);
    return TRUE;
}
BOOL FS_IsAvailable(void) { return contentRoot[0] != 0; }
void FS_InitFile(FSFile *file) { memset(file, 0, sizeof(*file)); file->command = FS_COMMAND_INVALID; }

static BOOL resolvePath(const char *path, char resolved[PATH_MAX], struct stat *info) {
    char joined[PATH_MAX];
    if (!path || !FS_IsAvailable()) return FALSE;
    if (strncmp(path, "rom:/", 5) == 0) path += 5;
    if (*path == '/' || strchr(path, '\\') || strchr(path, ':')) return FALSE;
    for (const char *start = path; *start;) {
        const char *end = strchr(start, '/');
        size_t length = end ? (size_t)(end - start) : strlen(start);
        if (!length || (length == 1 && *start == '.') || (length == 2 && start[0] == '.' && start[1] == '.')) return FALSE;
        if (!end) break;
        start = end + 1;
        if (!*start) return FALSE;
    }
    int length = snprintf(joined, sizeof(joined), "%s/%s", contentRoot, path);
    if (length < 0 || length >= sizeof(joined) || !realpath(joined, resolved)) return FALSE;
    size_t rootLength = strlen(contentRoot);
    if (strncmp(resolved, contentRoot, rootLength) || (resolved[rootLength] && resolved[rootLength] != '/') ||
        stat(resolved, info)) return FALSE;
    return TRUE;
}
BOOL HGNativeFSNormalizePath(const char *path, BOOL directory, char *relative, u32 capacity) {
    char resolved[PATH_MAX]; struct stat info;
    if (!relative || !capacity || !resolvePath(path, resolved, &info) ||
        (directory ? !S_ISDIR(info.st_mode) : !S_ISREG(info.st_mode))) return FALSE;
    const char *start = resolved+strlen(contentRoot);
    if (*start == '/') start++;
    size_t length = strlen(start);
    if (length >= capacity) return FALSE;
    memcpy(relative, start, length+1); return TRUE;
}
BOOL FS_OpenFile(FSFile *file, const char *path) {
    char resolved[PATH_MAX]; struct stat info;
    if (!file || file->nativeHandle || !resolvePath(path, resolved, &info) ||
        !S_ISREG(info.st_mode) || info.st_size < 0 || info.st_size > UINT32_MAX) return FALSE;
    FILE *stream = fopen(resolved, "rb");
    if (!stream) return FALSE;
    file->nativeHandle = stream; file->stat = FS_FILE_STATUS_IS_FILE;
    file->error = FS_RESULT_SUCCESS; file->prop.file.top = 0;
    file->prop.file.bottom = (u32)info.st_size; file->prop.file.pos = 0;
    atomic_fetch_add(&openFiles, 1);
    return TRUE;
}
BOOL FS_CloseFile(FSFile *file) {
    if (!file || !file->nativeHandle) return FALSE;
    int result = fclose(file->nativeHandle);
    file->nativeHandle = NULL; file->stat = 0; atomic_fetch_sub(&openFiles, 1);
    file->error = result == 0 ? FS_RESULT_SUCCESS : FS_RESULT_ERROR;
    return result == 0;
}
BOOL FS_SeekFile(FSFile *file, s32 offset, FSSeekFileMode origin) {
    if (!file || !file->nativeHandle) return FALSE;
    s64 position = offset;
    switch (origin) {
    case FS_SEEK_SET: position += file->prop.file.top; break;
    case FS_SEEK_CUR: position += file->prop.file.pos; break;
    case FS_SEEK_END: position += file->prop.file.bottom; break;
    default: return FALSE;
    }
    if (position < file->prop.file.top) position = file->prop.file.top;
    if (position > file->prop.file.bottom) position = file->prop.file.bottom;
    if (fseek(file->nativeHandle, (long)position, SEEK_SET)) { file->error = FS_RESULT_ERROR; return FALSE; }
    file->prop.file.pos = (u32)position;
    return TRUE;
}
int FSi_ReadFileCore(FSFile *file, void *dst, s32 length, BOOL async) {
    if (!file || !file->nativeHandle || length < 0 || (!dst && length)) return -1;
    size_t remaining = file->prop.file.bottom - file->prop.file.pos;
    if ((size_t)length > remaining) length = (s32)remaining;
    size_t count = length ? fread(dst, 1, (size_t)length, file->nativeHandle) : 0;
    file->prop.file.pos += (u32)count;
    file->error = ferror(file->nativeHandle) ? FS_RESULT_ERROR : FS_RESULT_SUCCESS;
    file->stat = FS_FILE_STATUS_IS_FILE | (async ? FS_FILE_STATUS_ASYNC : FS_FILE_STATUS_SYNC);
    return file->error == FS_RESULT_SUCCESS ? (int)count : -1;
}
int FS_ReadFile(FSFile *file, void *dst, s32 length) { return FSi_ReadFileCore(file, dst, length, FALSE); }
// shortcut: reads finish synchronously; add native async IO if frame profiling requires background streaming.
int FS_ReadFileAsync(FSFile *file, void *dst, s32 length) { return FSi_ReadFileCore(file, dst, length, TRUE); }
BOOL FS_WaitAsync(FSFile *file) { return file && file->nativeHandle && file->error == FS_RESULT_SUCCESS; }
