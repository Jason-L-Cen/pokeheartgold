// SPDX-License-Identifier: GPL-3.0-or-later
#define _POSIX_C_SOURCE 200809L
#include "hg32.h"
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <time.h>
#include <unistd.h>

#define CHECK(v) do { if (!(v)) { fprintf(stderr,"ABI32 check line %d: %s\n",__LINE__,#v); abort(); } } while (0)
static u32 read32(wasm_rt_memory_t *memory, u32 offset) {
    CHECK(offset <= memory->size && 4 <= memory->size-offset);
    u32 value; memcpy(&value,memory->data+offset,4); return value;
}

// Host-check bindings only: each instance owns copies of its standard descriptors.
struct w2c_wasi__snapshot__preview1 { wasm_rt_memory_t *memory; int fds[3]; };
enum { WASI_AGAIN=6,WASI_BADF=8,WASI_FAULT=21,WASI_INTR=27,WASI_INVAL=28,WASI_IO=29,WASI_OVERFLOW=61,WASI_SPIPE=70 };
static u32 wasiError(void) {
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
static u8 *wasiSpan(struct w2c_wasi__snapshot__preview1 *env,u32 offset,u64 size) {
    if (!env->memory || offset>env->memory->size || size>env->memory->size-offset) return NULL;
    return env->memory->data+offset;
}
static int wasiFd(struct w2c_wasi__snapshot__preview1 *env,u32 fd) { return fd<3 ? env->fds[fd] : -1; }
u32 w2c_wasi__snapshot__preview1_clock_time_get(struct w2c_wasi__snapshot__preview1 *env,u32 id,u64 precision,u32 output) {
    (void)precision;
    const clockid_t clocks[]={CLOCK_REALTIME,CLOCK_MONOTONIC,CLOCK_PROCESS_CPUTIME_ID,CLOCK_THREAD_CPUTIME_ID};
    if (id>=4) return WASI_INVAL;
    u8 *dest=wasiSpan(env,output,8);
    if (!dest) return WASI_FAULT;
    struct timespec value;
    if (clock_gettime(clocks[id],&value)) return wasiError();
    if (value.tv_sec<0 || (u64)value.tv_sec>(UINT64_MAX-(u64)value.tv_nsec)/1000000000) return WASI_OVERFLOW;
    u64 nanos=(u64)value.tv_sec*1000000000+(u64)value.tv_nsec;
    memcpy(dest,&nanos,8);
    return 0;
}
u32 w2c_wasi__snapshot__preview1_fd_close(struct w2c_wasi__snapshot__preview1 *env,u32 fd) {
    int host=wasiFd(env,fd);
    if (host<0) return WASI_BADF;
    env->fds[fd]=-1;
    return close(host) ? wasiError() : 0;
}
u32 w2c_wasi__snapshot__preview1_fd_fdstat_get(struct w2c_wasi__snapshot__preview1 *env,u32 fd,u32 output) {
    int host=wasiFd(env,fd);
    if (host<0) return WASI_BADF;
    u8 *dest=wasiSpan(env,output,24);
    if (!dest) return WASI_FAULT;
    struct stat info;
    if (fstat(host,&info)) return wasiError();
    int flags=fcntl(host,F_GETFL);
    if (flags<0) return wasiError();
    u16 fdflags=((flags&O_APPEND) ? 1 : 0)|((flags&O_NONBLOCK) ? 4 : 0)|((flags&O_SYNC) ? 16 : 0);
    u64 rights=0;
    if ((flags&O_ACCMODE)!=O_WRONLY) rights|=1ULL<<1;
    if ((flags&O_ACCMODE)!=O_RDONLY) rights|=1ULL<<6;
    if (S_ISREG(info.st_mode)) rights|=(1ULL<<2)|(1ULL<<5);
    memset(dest,0,24);
    dest[0]=S_ISREG(info.st_mode) ? 4 : S_ISCHR(info.st_mode) ? 2 : S_ISDIR(info.st_mode) ? 3 : 0;
    memcpy(dest+2,&fdflags,2); memcpy(dest+8,&rights,8);
    return 0;
}
u32 w2c_wasi__snapshot__preview1_fd_seek(struct w2c_wasi__snapshot__preview1 *env,u32 fd,u64 offset,u32 whence,u32 output) {
    int host=wasiFd(env,fd);
    if (host<0) return WASI_BADF;
    u8 *dest=wasiSpan(env,output,8);
    if (!dest) return WASI_FAULT;
    if (whence>2) return WASI_INVAL;
    int64_t signedOffset; memcpy(&signedOffset,&offset,8);
    const int origins[]={SEEK_SET,SEEK_CUR,SEEK_END};
    off_t result=lseek(host,(off_t)signedOffset,origins[whence]);
    if (result<0) return wasiError();
    u64 position=(u64)result; memcpy(dest,&position,8);
    return 0;
}
u32 w2c_wasi__snapshot__preview1_fd_write(struct w2c_wasi__snapshot__preview1 *env,u32 fd,u32 vectors,u32 count,u32 output) {
    int host=wasiFd(env,fd);
    if (host<0) return WASI_BADF;
    u8 *dest=wasiSpan(env,output,4), *table=wasiSpan(env,vectors,(u64)count*8);
    if (!dest || !table) return WASI_FAULT;
    if (count>IOV_MAX) return WASI_INVAL;
    struct iovec entries[IOV_MAX];
    u64 total=0;
    for (u32 i=0;i<count;i++) {
        u32 address,length; memcpy(&address,table+i*8,4); memcpy(&length,table+i*8+4,4);
        entries[i].iov_base=wasiSpan(env,address,length); entries[i].iov_len=length;
        if (!entries[i].iov_base) return WASI_FAULT;
        total+=length;
    }
    if (total>UINT32_MAX) return WASI_OVERFLOW;
    ssize_t result=writev(host,entries,(int)count);
    if (result<0) return wasiError();
    u32 written=(u32)result; memcpy(dest,&written,4);
    return 0;
}
static void CheckWasiHost(struct w2c_wasi__snapshot__preview1 *env,u32 offset) {
    wasm_rt_memory_t *memory=env->memory;
    char path[]="ios/build-original-abi32/io-check-XXXXXX";
    int file=mkstemp(path), saved=env->fds[1]; CHECK(file>=0 && !unlink(path)); env->fds[1]=file;
    u32 vectors[]={offset+96,2,offset+112,2}; memcpy(memory->data+offset,vectors,sizeof(vectors));
    memcpy(memory->data+offset+96,"ab",2); memcpy(memory->data+offset+112,"cd",2);
    CHECK(!w2c_wasi__snapshot__preview1_fd_write(env,1,offset,2,offset+40) && read32(memory,offset+40)==4);
    CHECK(!w2c_wasi__snapshot__preview1_fd_seek(env,1,0,0,offset+32));
    char bytes[4]; CHECK(read(file,bytes,4)==4 && !memcmp(bytes,"abcd",4));
    CHECK(!w2c_wasi__snapshot__preview1_fd_fdstat_get(env,1,offset+48) && memory->data[offset+48]==4);
    CHECK(!w2c_wasi__snapshot__preview1_clock_time_get(env,0,1,offset+80));
    u64 time; memcpy(&time,memory->data+offset+80,8); CHECK(time>0);
    CHECK(w2c_wasi__snapshot__preview1_clock_time_get(env,99,1,offset+80)==WASI_INVAL);
    CHECK(w2c_wasi__snapshot__preview1_clock_time_get(env,0,1,memory->size-3)==WASI_FAULT);
    CHECK(w2c_wasi__snapshot__preview1_fd_fdstat_get(env,1,memory->size-3)==WASI_FAULT);
    CHECK(w2c_wasi__snapshot__preview1_fd_write(env,1,memory->size-3,2,offset+40)==WASI_FAULT);
    CHECK(w2c_wasi__snapshot__preview1_fd_write(env,1,offset,UINT32_MAX,offset+40)==WASI_FAULT);
    vectors[2]=memory->size-1; memcpy(memory->data+offset,vectors,sizeof(vectors));
    CHECK(w2c_wasi__snapshot__preview1_fd_write(env,1,offset,2,offset+40)==WASI_FAULT && lseek(file,0,SEEK_CUR)==4);
    CHECK(read32(memory,offset+40)==4);
    CHECK(w2c_wasi__snapshot__preview1_fd_write(env,3,offset,0,offset+40)==WASI_BADF);
    CHECK(w2c_wasi__snapshot__preview1_fd_seek(env,1,0,3,offset+32)==WASI_INVAL);
    CHECK(w2c_wasi__snapshot__preview1_fd_seek(env,1,UINT64_MAX,0,offset+32)==WASI_INVAL);
    CHECK(!w2c_wasi__snapshot__preview1_fd_close(env,1));
    CHECK(w2c_wasi__snapshot__preview1_fd_close(env,1)==WASI_BADF);
    env->fds[1]=saved;
}
int main(void) {
    wasm_rt_init(); w2c_hg32 first, second;
    struct w2c_wasi__snapshot__preview1 firstEnv={NULL,{dup(0),dup(1),dup(2)}}, secondEnv={NULL,{dup(0),dup(1),dup(2)}};
    wasm2c_hg32_instantiate(&first,&firstEnv); wasm2c_hg32_instantiate(&second,&secondEnv);
    firstEnv.memory=w2c_hg32_memory(&first); secondEnv.memory=w2c_hg32_memory(&second);
    w2c_hg32_0x5Finitialize(&first); w2c_hg32_0x5Finitialize(&second);
    const u32 sizes[] = {4,136,236,12,4,8,0x48,0xE4,0x10,0x108,16,2,0x18,0x1C,0x14,0x28,20};
    for (u32 i=0;i<sizeof(sizes)/sizeof(sizes[0]);i++) {
        u32 actual = w2c_hg32_HGAotLayout(&first,i);
        if (actual != sizes[i]) fprintf(stderr,"Layout field %u: actual %u, expected %u\n",i,actual,sizes[i]);
        CHECK(actual==sizes[i]);
    }
    wasm_rt_memory_t *memory=w2c_hg32_memory(&first), *other=w2c_hg32_memory(&second);
    CHECK(memory->data != other->data);
    u32 offset=w2c_hg32_HGAotData(&first);
    CHECK(offset <= memory->size && 256 <= memory->size-offset);
    CHECK(w2c_hg32_HGAotPointerCheck(&first,offset,17));
    CHECK(!w2c_hg32_HGAotPointerCheck(&first,offset,33));
    CHECK(!w2c_hg32_HGAotPointerCheck(&first,offset+256,1));
    CheckWasiHost(&firstEnv,offset);
    u32 string=w2c_hg32_HGAotString(&first), iterator=w2c_hg32_HGAotIterator(&first);
    CHECK(read32(memory,string+4)==17 && read32(memory,string+8)==offset && read32(memory,iterator)==string);
    const u16 expected[] = {0,0xE97E,0x5271,0x31B0,0x8E42,0xE2CC,0xAFC5,0x67DB};
    w2c_hg32_HGAotSeed(&first,0); w2c_hg32_HGAotSeed(&second,0);
    for (u32 i=0;i<8;i++) CHECK(w2c_hg32_HGAotRandom(&first)==expected[i]);
    CHECK(w2c_hg32_HGAotRandom(&second)==expected[0]);
    w2c_hg32_HGAotMTSeed(&first,5489);
    CHECK(w2c_hg32_HGAotMTRandom(&first)==3499211612u);
    CHECK(w2c_hg32_HGAotMTRandom(&first)==581869302u);
    CHECK(w2c_hg32_HGAotMTRandom(&first)==3890346734u);
    memset(memory->data+offset,0,256); CHECK(w2c_hg32_HGAotEncrypt(&first,16,0));
    CHECK(memcmp(memory->data+offset,expected,16)==0);
    CHECK(w2c_hg32_HGAotDecrypt(&first,16,0));
    for (u32 i=0;i<16;i++) CHECK(memory->data[offset+i]==0);
    u8 original[256]; for (u32 i=0;i<256;i++) original[i]=(u8)(i*7+11);
    for (u32 length=0;length<=256;length+=2) {
        memcpy(memory->data+offset,original,256);
        u32 checksum=w2c_hg32_HGAotChecksum(&first,length);
        CHECK(w2c_hg32_HGAotEncrypt(&first,length,0x12345678^length));
        CHECK(w2c_hg32_HGAotDecrypt(&first,length,0x12345678^length));
        CHECK(memcmp(memory->data+offset,original,256)==0);
        CHECK(w2c_hg32_HGAotChecksum(&first,length)==checksum);
    }
    CHECK(!w2c_hg32_HGAotEncrypt(&first,258,0) && !w2c_hg32_HGAotDecrypt(&first,1,0));
    CHECK(w2c_hg32_HGAotChecksum(&first,258)==UINT32_MAX && memcmp(memory->data+offset,original,256)==0);
    memcpy(memory->data+offset,"alpha beta beta",15); memcpy(memory->data+offset+128,"beta",4);
    CHECK(w2c_hg32_HGAotLobbyFind(&first,15,4,0,0)==6);
    CHECK(w2c_hg32_HGAotLobbyFind(&first,15,4,7,0)==11);
    CHECK(w2c_hg32_HGAotLobbyFind(&first,15,4,12,0)==UINT32_MAX);
    CHECK(w2c_hg32_HGAotLobbyFind(&first,15,0,15,0)==15);
    CHECK(w2c_hg32_HGAotLobbyFind(&first,15,0,16,0)==UINT32_MAX);
    memcpy(memory->data+offset,"abcabc",6); memcpy(memory->data+offset+128,"bc",2);
    CHECK(w2c_hg32_HGAotLobbyFind(&first,6,2,2,1)==4);
    CHECK(w2c_hg32_HGAotLobbyFind(&first,12,2,0,1)==UINT32_MAX);
    CHECK(w2c_hg32_HGAotDequeCopy(&first));
    CHECK(w2c_hg32_HGAotWifiStatus(&first,0));
    CHECK(w2c_hg32_HGAotWifiStatus(&first,1));
    CHECK(w2c_hg32_HGAotBillboardFrames(&first));
    CHECK(w2c_hg32_HGAotBottomScreenCallbacks(&first,0));
    CHECK(w2c_hg32_HGAotBottomScreenCallbacks(&first,1));
    CHECK(!w2c_hg32_HGAotBottomScreenCallbacks(&first,2));
    CHECK(w2c_hg32_HGAotSequenceIndex(&first,0)==0xA010);
    CHECK(w2c_hg32_HGAotSequenceIndex(&first,4)==0xA010);
    CHECK(w2c_hg32_HGAotSequenceIndex(&first,5)==0xB020);
    CHECK(w2c_hg32_HGAotSequenceIndex(&first,14)==0xB020);
    CHECK(w2c_hg32_HGAotSequenceIndex(&first,15)==0xC030);
    CHECK(w2c_hg32_HGAotSequenceIndex(&first,65535)==0xC030);
    CHECK(w2c_hg32_HGAotSequenceIndex(&first,65536)==UINT32_MAX);
    CHECK(w2c_hg32_HGAotMovementAllowed(&first,0)==0);
    CHECK(w2c_hg32_HGAotMovementAllowed(&first,1)==1);
    CHECK(w2c_hg32_HGAotMovementAllowed(&first,0xA5)==0xA5);
    CHECK(w2c_hg32_HGAotMovementAllowed(&first,256)==UINT32_MAX);
    CHECK(w2c_hg32_HGAotAsmPointers(&first));
    CHECK(w2c_hg32_HGAotFactoryFlags(&first));
    CHECK(w2c_hg32_HGAotSDKMath(&first));
    CHECK(w2c_hg32_HGAotCommunicationModes(&first));
    CHECK(w2c_hg32_HGAotCommandDispatch(&first));
    CHECK(w2c_hg32_HGAotClubDisconnect(&first));
    CHECK(w2c_hg32_HGAotLobbyStringSearch(&first));
    CHECK(w2c_hg32_HGAotSendBufferSpace(&first));
    CHECK(w2c_hg32_HGAotMatchErrorRouting(&first));
    CHECK(w2c_hg32_HGAotSocketAdapters(&first));
    CHECK(w2c_hg32_HGAotTransportCallbacks(&first));
    CHECK(w2c_hg32_HGAotCourseStateDispatch(&first));
    CHECK(w2c_hg32_HGAotCompressedResource(&first,0,10,9));
    CHECK(w2c_hg32_HGAotCompressedResource(&first,1,8,4));
    for (u32 size=0;size<10;size++) CHECK(!w2c_hg32_HGAotCompressedResource(&first,0,size,9));
    for (u32 size=0;size<8;size++) CHECK(!w2c_hg32_HGAotCompressedResource(&first,1,size,4));
    CHECK(!w2c_hg32_HGAotCompressedResource(&first,0,10,8));
    CHECK(!w2c_hg32_HGAotCompressedResource(&first,2,7,3));
    u32 memoryLine=w2c_hg32_CheckNativeMemory(&first);
    if (memoryLine) fprintf(stderr,"Memory check line %u\n",memoryLine);
    CHECK(!memoryLine);
    u32 rc4Line=w2c_hg32_CheckOriginalRC4(&first);
    if (rc4Line) fprintf(stderr,"RC4 check line %u\n",rc4Line);
    CHECK(!rc4Line);
    u32 digestLine=w2c_hg32_CheckOriginalDigests(&first);
    if (digestLine) fprintf(stderr,"Digest check line %u\n",digestLine);
    CHECK(!digestLine);
    u32 sortLine=w2c_hg32_CheckOriginalQSort(&first);
    if (sortLine) fprintf(stderr,"Sort check line %u\n",sortLine);
    CHECK(!sortLine);
    u32 stringLine=w2c_hg32_CheckOriginalLobbyStrings(&first);
    if (stringLine) fprintf(stderr,"Lobby string check line %u\n",stringLine);
    CHECK(!stringLine);
    u32 callbackLine=w2c_hg32_CheckOriginalChatDispatch(&first);
    if (callbackLine) fprintf(stderr,"Chat dispatch check line %u\n",callbackLine);
    CHECK(!callbackLine);
    stringLine=w2c_hg32_CheckOriginalLobbyComparison(&first);
    if (stringLine) fprintf(stderr,"Lobby comparison check line %u\n",stringLine);
    CHECK(!stringLine);
    stringLine=w2c_hg32_CheckOriginalLobbyCleanup(&first);
    if (stringLine) fprintf(stderr,"Lobby cleanup check line %u\n",stringLine);
    CHECK(!stringLine);
    CHECK(w2c_hg32_HGAotEventLogSort(&first));
    CHECK(w2c_hg32_HGAotParticleCreation(&first));
    const struct { u32 count; s32 values[16]; } matrices[] = {
        {9,{4096,0,0,0,4096,0,0,0,4096}},
        {9,{4096,0,-2048,0,4096,0,2048,0,4096}},
        {9,{4096,2048,0,-2048,4096,0,0,0,4096}},
        {12,{4096,0,0,0,4096,0,0,0,4096,0,0,0}},
        {12,{2048,0,0,0,4096,0,0,0,1234,0,0,0}},
        {12,{4096,0,0,0,4096,2048,0,-2048,4096,0,0,0}},
        {12,{4096,0,-2048,0,4096,0,2048,0,4096,0,0,0}},
        {16,{4096,0,0,0,0,4096,0,0,0,0,4096,0,0,0,0,4096}},
        {16,{4096,0,0,0,0,4096,2048,0,0,-2048,4096,0,0,0,0,4096}},
        {16,{4096,0,-2048,0,0,4096,0,0,2048,0,4096,0,0,0,0,4096}},
        {16,{4096,2048,0,0,-2048,4096,0,0,0,0,4096,0,0,0,0,4096}},
        {16,{1,2,3,0,4,5,6,0,7,8,9,0,10,11,12,4096}},
        {12,{1,2,3,5,6,7,9,10,11,13,14,15}},
        {12,{1,2,3,5,6,7,9,10,11,13,14,15}},
    };
    for (u32 operation=0;operation<sizeof(matrices)/sizeof(matrices[0]);operation++) {
        u32 result=w2c_hg32_HGAotSDKMatrix(&first,operation,2048,4096);
        CHECK(result);
        for (u32 i=0;i<matrices[operation].count;i++) CHECK(read32(memory,result+4*i)==(u32)matrices[operation].values[i]);
        if (operation<11) {
            result=w2c_hg32_HGAotSDKMatrix(&first,operation,0x80000000u,4096);
            for (u32 i=0;i<matrices[operation].count;i++) {
                s32 expected=matrices[operation].values[i];
                CHECK(read32(memory,result+4*i)==(expected==2048 || expected==-2048 ? 0x80000000u : (u32)expected));
            }
        }
    }
    CHECK(!w2c_hg32_HGAotSDKMatrix(&first,14,0,0));
    const u32 grid[]={0,1,2,3,8,4,5,6,7,9,10,11,12,13,14};
    for (u32 y=0;y<3;y++) for (u32 x=0;x<5;x++) CHECK(w2c_hg32_HGAotSDKCharIndex(&first,5,3,x,y)==grid[y*5+x]);
    for (u32 h=1;h<=16;h++) for (u32 w=1;w<=16;w++) {
        u8 seen[256]={0};
        for (u32 y=0;y<h;y++) for (u32 x=0;x<w;x++) {
            u32 index=w2c_hg32_HGAotSDKCharIndex(&first,w,h,x,y);
            CHECK(index<w*h && !seen[index]); seen[index]=1;
        }
    }
    CHECK(w2c_hg32_HGAotSDKCharIndex(&first,0,1,0,0)==UINT32_MAX);
    w2c_hg32_HGAotSeed(&first,0);
    for (u32 i=0;i<8;i++) CHECK(w2c_hg32_HGAotAsmRandom(&first)==expected[i]);
    CHECK(w2c_hg32_HGAotSpawnWarp(&first,1,0,0)==63);
    CHECK(w2c_hg32_HGAotSpawnWarp(&first,1,0,2)==6 && w2c_hg32_HGAotSpawnWarp(&first,1,0,3)==8);
    CHECK(w2c_hg32_HGAotSpawnWarp(&first,1,1,0)==60);
    CHECK(w2c_hg32_HGAotSpawnWarp(&first,1,1,2)==695 && w2c_hg32_HGAotSpawnWarp(&first,1,1,3)==397);
    for (u32 id=1;id<=30;id++) for (u32 kind=0;kind<3;kind++) {
        CHECK(w2c_hg32_HGAotSpawnWarp(&first,id,kind,1)==UINT32_MAX);
        CHECK(w2c_hg32_HGAotSpawnWarp(&first,id,kind,4)==(kind==0 ? 0 : 1));
    }
    wasm2c_hg32_free(&first); wasm2c_hg32_free(&second); wasm_rt_free();
    for (u32 i=0;i<3;i++) { if (firstEnv.fds[i]>=0) close(firstEnv.fds[i]); if (secondEnv.fds[i]>=0) close(secondEnv.fds[i]); }
    puts("PASS: native generated C preserves original layouts, RNG/crypto, lobby ABI, Wi-Fi returns, animation sequences and field callbacks.");
    return 0;
}
