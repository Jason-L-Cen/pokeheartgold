// SPDX-License-Identifier: GPL-3.0-or-later
#include "NativeBackup.h"
#include <nitro/card.h>
#include <dispatch/dispatch.h>
#include <pthread.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BACKUP_SIZE 0x80000u
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t changed = PTHREAD_COND_INITIALIZER;
static int imageFD = -1, lockFD = -1;
static char *imagePath;
static u64 consoleId;
static BOOL busy, cancelRequested;
static u16 owner;
static u32 references, callbackCount;
static _Thread_local u32 callbackDepth;
static CARDResult result = CARD_RESULT_NO_RESPONSE;
static dispatch_once_t queueOnce;
static dispatch_queue_t queue;

static void createQueue(void *unused) { (void)unused; queue = dispatch_queue_create("heartgold.backup", DISPATCH_QUEUE_SERIAL); }
static BOOL callbackHere(void) { return callbackDepth != 0; }
static void waitIdle(void) { while (busy || (callbackCount && !callbackHere())) pthread_cond_wait(&changed, &mutex); }
static BOOL cancelled(void) { pthread_mutex_lock(&mutex); BOOL value = cancelRequested; pthread_mutex_unlock(&mutex); return value; }

static BOOL transfer(int fd, void *buffer, u32 size, u32 offset, BOOL write) {
    u8 *bytes = buffer;
    while (size) {
        ssize_t count = write ? pwrite(fd, bytes, size, offset) : pread(fd, bytes, size, offset);
        if (count < 0 && errno == EINTR) continue;
        if (count <= 0) return FALSE;
        bytes += count; size -= (u32)count; offset += (u32)count;
    }
    return TRUE;
}
static BOOL sameVersion(const struct stat *a, const struct stat *b) {
    return S_ISREG(a->st_mode) && a->st_dev == b->st_dev && a->st_ino == b->st_ino && a->st_size == b->st_size &&
        a->st_mtimespec.tv_sec == b->st_mtimespec.tv_sec && a->st_mtimespec.tv_nsec == b->st_mtimespec.tv_nsec &&
        a->st_ctimespec.tv_sec == b->st_ctimespec.tv_sec && a->st_ctimespec.tv_nsec == b->st_ctimespec.tv_nsec;
}
static BOOL flush(int fd) {
    int status;
    do { status = fsync(fd); } while (status < 0 && errno == EINTR);
    return status == 0;
}
static BOOL syncParent(const char *path) {
    char parent[PATH_MAX]; strcpy(parent, path);
    char *slash = strrchr(parent, '/');
    if (!slash) strcpy(parent, ".");
    else if (slash == parent) parent[1] = 0;
    else *slash = 0;
    int fd = open(parent, O_RDONLY | O_DIRECTORY | O_CLOEXEC);
    if (fd < 0) return FALSE;
    BOOL ok = flush(fd); close(fd); return ok;
}
static CARDResult publish(const char *path, const u8 *image, u32 size, const struct stat *expected, int *opened) {
    char temporary[PATH_MAX]; snprintf(temporary, sizeof(temporary), "%s.tmp.XXXXXX", path);
    int fd = mkstemp(temporary); *opened = -1;
    if (fd < 0) return CARD_RESULT_ERROR;
    if (fcntl(fd, F_SETFD, FD_CLOEXEC) != 0) { close(fd); unlink(temporary); return CARD_RESULT_ERROR; }
    CARDResult status = CARD_RESULT_ERROR;
    if (transfer(fd, (void *)image, size, 0, TRUE) && flush(fd)) {
        if (expected) pthread_mutex_lock(&mutex);
        struct stat current;
        if (expected && cancelRequested) status = CARD_RESULT_CANCELED;
        else if (expected && (lstat(path, &current) != 0 || !sameVersion(&current, expected))) status = CARD_RESULT_ERROR;
        else if ((expected ? rename(temporary, path) : link(temporary, path)) == 0) status = CARD_RESULT_SUCCESS;
        if (expected) pthread_mutex_unlock(&mutex);
        if (status == CARD_RESULT_SUCCESS) {
            *opened = fd; fd = -1;
            if (!syncParent(path)) status = CARD_RESULT_ERROR;
        }
    }
    if (fd >= 0) close(fd);
    unlink(temporary);
    return status;
}
static BOOL loadConsoleId(const char *path, u64 *identifier) {
    char name[PATH_MAX]; snprintf(name, sizeof(name), "%s.console", path);
    u8 bytes[8];
    int fd = open(name, O_RDONLY | O_NOFOLLOW | O_CLOEXEC);
    if (fd < 0 && errno == ENOENT) {
        u64 value;
        do { arc4random_buf(&value, sizeof(value)); value &= (1ULL << 43) - 1; } while (!value);
        for (u32 i = 0; i < 8; i++) bytes[i] = (u8)(value >> (8 * i));
        if (publish(name, bytes, sizeof(bytes), NULL, &fd) != CARD_RESULT_SUCCESS) { if (fd >= 0) close(fd); return FALSE; }
    }
    if (fd < 0) return FALSE;
    struct stat info;
    BOOL ok = fstat(fd, &info) == 0 && S_ISREG(info.st_mode) && info.st_size == sizeof(bytes) && transfer(fd, bytes, sizeof(bytes), 0, FALSE);
    close(fd);
    u64 value = 0;
    if (ok) for (u32 i = 0; i < 8; i++) value |= (u64)bytes[i] << (8 * i);
    if (!ok || !value || value >= (1ULL << 43)) return FALSE;
    *identifier = value; return TRUE;
}
u64 HGNativeBackupConsoleId(void) { pthread_mutex_lock(&mutex); u64 value = consoleId; pthread_mutex_unlock(&mutex); return value; }

BOOL HGNativeBackupSetPath(const char *path) {
    pthread_mutex_lock(&mutex);
    if (busy || callbackCount || references) { pthread_mutex_unlock(&mutex); return FALSE; }
    if (!path) {
        if (imageFD >= 0) close(imageFD);
        if (lockFD >= 0) close(lockFD);
        imageFD = lockFD = -1; free(imagePath); imagePath = NULL; consoleId = 0; result = CARD_RESULT_NO_RESPONSE;
        pthread_mutex_unlock(&mutex); return TRUE;
    }
    size_t length = strlen(path);
    if (!length || length > PATH_MAX - 24) { pthread_mutex_unlock(&mutex); return FALSE; }
    BOOL same = imagePath && !strcmp(path, imagePath);
    char lockPath[PATH_MAX]; snprintf(lockPath, sizeof(lockPath), "%s.lock", path);
    int lock = same ? lockFD : open(lockPath, O_RDWR | O_CREAT | O_NOFOLLOW | O_CLOEXEC, 0600);
    if (lock < 0 || (!same && flock(lock, LOCK_EX | LOCK_NB) != 0)) {
        if (lock >= 0 && !same) close(lock);
        pthread_mutex_unlock(&mutex); return FALSE;
    }
    int fd = open(path, O_RDWR | O_NOFOLLOW | O_CLOEXEC);
    if (fd < 0 && errno == ENOENT) {
        u8 *blank = malloc(BACKUP_SIZE);
        if (blank) { memset(blank, 0xFF, BACKUP_SIZE); if (publish(path, blank, BACKUP_SIZE, NULL, &fd) != CARD_RESULT_SUCCESS) { if (fd >= 0) close(fd); fd = -1; } free(blank); }
    }
    struct stat info;
    u64 identifier;
    char *name = strdup(path);
    if (fd < 0 || !name || fstat(fd, &info) != 0 || !S_ISREG(info.st_mode) || info.st_size != BACKUP_SIZE || !loadConsoleId(path, &identifier)) {
        if (fd >= 0) close(fd);
        if (!same) close(lock);
        free(name); pthread_mutex_unlock(&mutex); return FALSE;
    }
    if (imageFD >= 0) close(imageFD);
    if (!same && lockFD >= 0) close(lockFD);
    free(imagePath); imagePath = name; imageFD = fd; lockFD = lock; consoleId = identifier; result = CARD_RESULT_SUCCESS;
    pthread_mutex_unlock(&mutex); return TRUE;
}

BOOL HGNativeBackupTryLock(u16 id) {
    pthread_mutex_lock(&mutex);
    BOOL allowed=id && imageFD>=0 && (!references || owner==id) && references<UINT32_MAX;
    if (allowed) { owner=id; references++; }
    pthread_mutex_unlock(&mutex); return allowed;
}
void CARD_LockBackup(u16 id) {
    if (!id) abort();
    pthread_mutex_lock(&mutex);
    while (references && owner != id) pthread_cond_wait(&changed, &mutex);
    if (references == UINT32_MAX) abort();
    owner = id; references++;
    pthread_mutex_unlock(&mutex);
}
BOOL CARD_WaitBackupAsync(void) {
    pthread_mutex_lock(&mutex); waitIdle(); BOOL ok = result == CARD_RESULT_SUCCESS;
    pthread_mutex_unlock(&mutex); return ok;
}
BOOL CARD_TryWaitBackupAsync(void) {
    pthread_mutex_lock(&mutex); BOOL done = !busy && (!callbackCount || callbackHere());
    pthread_mutex_unlock(&mutex); return done;
}
void CARD_UnlockBackup(u16 id) {
    (void)CARD_WaitBackupAsync();
    pthread_mutex_lock(&mutex);
    if (!references || owner != id) abort();
    if (!--references) { owner = 0; pthread_cond_broadcast(&changed); }
    pthread_mutex_unlock(&mutex);
}
CARDResult CARD_GetResultCode(void) { pthread_mutex_lock(&mutex); CARDResult value = result; pthread_mutex_unlock(&mutex); return value; }
void CARD_CancelBackupAsync(void) { pthread_mutex_lock(&mutex); if (busy) cancelRequested = TRUE; pthread_mutex_unlock(&mutex); }
u32 CARD_GetBackupSectorSize(void) { pthread_mutex_lock(&mutex); u32 value = imageFD >= 0 ? 0x10000 : 0; pthread_mutex_unlock(&mutex); return value; }
BOOL CARD_IdentifyBackup(CARDBackupType type) {
    pthread_mutex_lock(&mutex); waitIdle();
    result = imageFD < 0 ? CARD_RESULT_NO_RESPONSE :
        type == CARD_BACKUP_TYPE_FLASH_4MBITS || type == CARD_BACKUP_TYPE_FLASH_4MBITS_EX ? CARD_RESULT_SUCCESS : CARD_RESULT_UNSUPPORTED;
    BOOL ok = result == CARD_RESULT_SUCCESS; pthread_mutex_unlock(&mutex); return ok;
}

typedef struct BackupTask { uaddr src, dst; u32 size; MIDmaCallback callback; void *argument; CARDRequest command; CARDRequestMode mode; int retry; } BackupTask;
static CARDResult perform(const BackupTask *task) {
    pthread_mutex_lock(&mutex);
    int fd = imageFD; const char *path = imagePath; BOOL locked = references != 0;
    pthread_mutex_unlock(&mutex);
    if (fd < 0) return CARD_RESULT_NO_RESPONSE;
    if (!locked || task->retry < 1) return CARD_RESULT_INVALID_PARAM;
    BOOL read = task->command == CARD_REQ_READ_BACKUP && task->mode == CARD_REQUEST_MODE_RECV;
    BOOL write = task->command == CARD_REQ_WRITE_BACKUP && (task->mode == CARD_REQUEST_MODE_SEND || task->mode == CARD_REQUEST_MODE_SEND_VERIFY);
    if (!read && !write) return CARD_RESULT_UNSUPPORTED;
    uaddr offset = read ? task->src : task->dst, address = read ? task->dst : task->src;
    if (offset > BACKUP_SIZE || task->size > BACKUP_SIZE - offset || (task->size && !address)) return CARD_RESULT_INVALID_PARAM;
    if (cancelled()) return CARD_RESULT_CANCELED;
    if (read) return transfer(fd, (void *)address, task->size, (u32)offset, FALSE) ? CARD_RESULT_SUCCESS : CARD_RESULT_ERROR;
    if (!task->size) return CARD_RESULT_SUCCESS;
    u8 *image = malloc(BACKUP_SIZE);
    if (!image) return CARD_RESULT_ERROR;
    struct stat version;
    CARDResult status = CARD_RESULT_ERROR;
    if (fstat(fd, &version) == 0 && version.st_size == BACKUP_SIZE && transfer(fd, image, BACKUP_SIZE, 0, FALSE)) {
        memcpy(image + offset, (const void *)address, task->size);
        int replacement;
        // shortcut: each chip write publishes 512 KiB; measure save latency before adding a durable journal.
        status = publish(path, image, BACKUP_SIZE, &version, &replacement);
        if (replacement >= 0) {
            pthread_mutex_lock(&mutex); close(imageFD); imageFD = replacement; fd = replacement; pthread_mutex_unlock(&mutex);
        }
        if (status == CARD_RESULT_SUCCESS && task->mode == CARD_REQUEST_MODE_SEND_VERIFY) {
            if (!transfer(fd, image, task->size, (u32)offset, FALSE) || memcmp(image, (const void *)address, task->size)) status = CARD_RESULT_FAILURE;
        }
    }
    free(image); return status;
}
static void finish(const BackupTask *task, CARDResult status) {
    pthread_mutex_lock(&mutex); result = status; busy = FALSE;
    if (task->callback) { callbackDepth++; callbackCount++; }
    pthread_cond_broadcast(&changed); pthread_mutex_unlock(&mutex);
    if (task->callback) {
        task->callback(task->argument);
        pthread_mutex_lock(&mutex); callbackDepth--; callbackCount--; pthread_cond_broadcast(&changed); pthread_mutex_unlock(&mutex);
    }
}
static void run(void *argument) { BackupTask *task = argument; finish(task, perform(task)); free(task); }
BOOL CARDi_RequestStreamCommand(uaddr src, uaddr dst, u32 size, MIDmaCallback callback, void *argument, BOOL async,
                               CARDRequest command, int retry, CARDRequestMode mode) {
    BackupTask local = {src, dst, size, callback, argument, command, mode, retry};
    pthread_mutex_lock(&mutex); waitIdle(); busy = TRUE; cancelRequested = FALSE; pthread_mutex_unlock(&mutex);
    if (!async) { CARDResult status = perform(&local); finish(&local, status); return status == CARD_RESULT_SUCCESS; }
    dispatch_once_f(&queueOnce, NULL, createQueue);
    BackupTask *task = queue ? malloc(sizeof(*task)) : NULL;
    if (!task) { finish(&local, CARD_RESULT_ERROR); return FALSE; }
    *task = local; dispatch_async_f(queue, task, run); return TRUE;
}
