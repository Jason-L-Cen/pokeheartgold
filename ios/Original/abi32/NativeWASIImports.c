// SPDX-License-Identifier: GPL-3.0-or-later
#define _POSIX_C_SOURCE 200809L
#define _DARWIN_C_SOURCE 1
#include "NativeWASIImports.h"
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <time.h>
#include <unistd.h>

static uint32_t wasiError(void) {
    switch (errno) {
    case EAGAIN: return WASI_AGAIN;
    case EBADF: return WASI_BADF;
    case EFAULT: return WASI_FAULT;
    case EINTR: return WASI_INTR;
    case EINVAL: return WASI_INVAL;
    case EOVERFLOW: return WASI_OVERFLOW;
    case ESPIPE: return WASI_SPIPE;
    default: return WASI_IO;
    }
}
static uint8_t *wasiSpan(struct w2c_wasi__snapshot__preview1 *env,uint32_t offset,uint64_t size) {
    if (!env || !env->memory || !env->memory->data || offset>env->memory->size || size>env->memory->size-offset) return NULL;
    return env->memory->data+offset;
}
static int wasiFd(struct w2c_wasi__snapshot__preview1 *env,uint32_t fd) { return env && fd<3 ? env->fds[fd] : -1; }
// Only stdio copies are exposed; original game files use NativeFS, not WASI preopened directories.
uint32_t w2c_wasi__snapshot__preview1_fd_prestat_get(struct w2c_wasi__snapshot__preview1 *env,uint32_t fd,uint32_t output) {
    (void)env; (void)fd; (void)output; return WASI_BADF;
}
uint32_t w2c_wasi__snapshot__preview1_fd_prestat_dir_name(struct w2c_wasi__snapshot__preview1 *env,uint32_t fd,uint32_t output,uint32_t bytes) {
    (void)env; (void)fd; (void)output; (void)bytes; return WASI_BADF;
}
uint32_t w2c_wasi__snapshot__preview1_environ_sizes_get(struct w2c_wasi__snapshot__preview1 *env,uint32_t count,uint32_t size) {
    uint8_t *entries=wasiSpan(env,count,4),*bytes=wasiSpan(env,size,4);
    if (!entries || !bytes) return WASI_FAULT;
    // Native game instances have an empty environment; host process secrets are not imported.
    uint32_t zero=0; memcpy(entries,&zero,4); memcpy(bytes,&zero,4); return 0;
}
uint32_t w2c_wasi__snapshot__preview1_environ_get(struct w2c_wasi__snapshot__preview1 *env,uint32_t entries,uint32_t bytes) {
    return wasiSpan(env,entries,0) && wasiSpan(env,bytes,0) ? 0 : WASI_FAULT;
}
void w2c_wasi__snapshot__preview1_proc_exit(struct w2c_wasi__snapshot__preview1 *env,uint32_t status) {
    if (!env) wasm_rt_trap(WASM_RT_TRAP_OOB);
    env->exited=1; env->exitCode=status;
    wasm_rt_trap(WASM_RT_TRAP_UNREACHABLE);
}
uint32_t w2c_wasi__snapshot__preview1_clock_time_get(struct w2c_wasi__snapshot__preview1 *env,uint32_t id,uint64_t precision,uint32_t output) {
    (void)precision;
    const clockid_t clocks[]={CLOCK_REALTIME,CLOCK_MONOTONIC,CLOCK_PROCESS_CPUTIME_ID,CLOCK_THREAD_CPUTIME_ID};
    if (id>=4) return WASI_INVAL;
    uint8_t *dest=wasiSpan(env,output,8);
    if (!dest) return WASI_FAULT;
    struct timespec value;
    if (clock_gettime(clocks[id],&value)) return wasiError();
    if (value.tv_sec<0 || (uint64_t)value.tv_sec>(UINT64_MAX-(uint64_t)value.tv_nsec)/1000000000) return WASI_OVERFLOW;
    uint64_t nanos=(uint64_t)value.tv_sec*1000000000+(uint64_t)value.tv_nsec;
    memcpy(dest,&nanos,8);
    return 0;
}
uint32_t w2c_wasi__snapshot__preview1_fd_close(struct w2c_wasi__snapshot__preview1 *env,uint32_t fd) {
    int host=wasiFd(env,fd);
    if (host<0) return WASI_BADF;
    env->fds[fd]=-1;
    return close(host) ? wasiError() : 0;
}
uint32_t w2c_wasi__snapshot__preview1_fd_fdstat_get(struct w2c_wasi__snapshot__preview1 *env,uint32_t fd,uint32_t output) {
    int host=wasiFd(env,fd);
    if (host<0) return WASI_BADF;
    uint8_t *dest=wasiSpan(env,output,24);
    if (!dest) return WASI_FAULT;
    struct stat info;
    if (fstat(host,&info)) return wasiError();
    int flags=fcntl(host,F_GETFL);
    if (flags<0) return wasiError();
    uint16_t fdflags=((flags&O_APPEND) ? 1 : 0)|((flags&O_NONBLOCK) ? 4 : 0)|((flags&O_SYNC) ? 16 : 0);
    uint64_t rights=0;
    if ((flags&O_ACCMODE)!=O_WRONLY) rights|=1ULL<<1;
    if ((flags&O_ACCMODE)!=O_RDONLY) rights|=1ULL<<6;
    if (S_ISREG(info.st_mode)) rights|=(1ULL<<2)|(1ULL<<5);
    memset(dest,0,24);
    dest[0]=S_ISREG(info.st_mode) ? 4 : S_ISCHR(info.st_mode) ? 2 : S_ISDIR(info.st_mode) ? 3 : 0;
    memcpy(dest+2,&fdflags,2); memcpy(dest+8,&rights,8);
    return 0;
}
uint32_t w2c_wasi__snapshot__preview1_fd_seek(struct w2c_wasi__snapshot__preview1 *env,uint32_t fd,uint64_t offset,uint32_t whence,uint32_t output) {
    int host=wasiFd(env,fd);
    if (host<0) return WASI_BADF;
    uint8_t *dest=wasiSpan(env,output,8);
    if (!dest) return WASI_FAULT;
    if (whence>2) return WASI_INVAL;
    int64_t signedOffset; memcpy(&signedOffset,&offset,8);
    const int origins[]={SEEK_SET,SEEK_CUR,SEEK_END};
    off_t result=lseek(host,(off_t)signedOffset,origins[whence]);
    if (result<0) return wasiError();
    uint64_t position=(uint64_t)result; memcpy(dest,&position,8);
    return 0;
}
static uint32_t wasiTransfer(struct w2c_wasi__snapshot__preview1 *env,uint32_t fd,uint32_t vectors,uint32_t count,uint32_t output,int writing) {
    int host=wasiFd(env,fd);
    if (host<0) return WASI_BADF;
    uint8_t *dest=wasiSpan(env,output,4),*table=wasiSpan(env,vectors,(uint64_t)count*8);
    if (!dest || !table) return WASI_FAULT;
    if (count>IOV_MAX) return WASI_INVAL;
    struct iovec entries[IOV_MAX];
    uint64_t total=0;
    for (uint32_t i=0;i<count;i++) {
        uint32_t address,length; memcpy(&address,table+i*8,4); memcpy(&length,table+i*8+4,4);
        entries[i].iov_base=wasiSpan(env,address,length); entries[i].iov_len=length;
        if (!entries[i].iov_base) return WASI_FAULT;
        total+=length;
    }
    if (total>UINT32_MAX) return WASI_OVERFLOW;
    ssize_t result=writing ? writev(host,entries,(int)count) : readv(host,entries,(int)count);
    if (result<0) return wasiError();
    uint32_t written=(uint32_t)result; memcpy(dest,&written,4);
    return 0;
}
uint32_t w2c_wasi__snapshot__preview1_fd_write(struct w2c_wasi__snapshot__preview1 *env,uint32_t fd,uint32_t vectors,uint32_t count,uint32_t output) {
    return wasiTransfer(env,fd,vectors,count,output,1);
}
uint32_t w2c_wasi__snapshot__preview1_fd_read(struct w2c_wasi__snapshot__preview1 *env,uint32_t fd,uint32_t vectors,uint32_t count,uint32_t output) {
    return wasiTransfer(env,fd,vectors,count,output,0);
}
