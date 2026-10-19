// SPDX-License-Identifier: GPL-3.0-or-later
#include "hgengine.h"
#include "NativeImports.h"
#include "NativeWASIImports.h"
#include "../NativeFile.h"
#include "../NativeBackup.h"
#include "wasm-rt-impl.h"
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

WASM_RT_THREAD_LOCAL wasm_rt_jmp_buf g_wasm_rt_jmp_buf;

void HGEngineTrap(wasm_rt_trap_t code) {
    void *frames[48];
    fprintf(stderr,"Original engine native trap: %s\n",wasm_rt_strerror(code));
    backtrace_symbols_fd(frames,backtrace(frames,48),STDERR_FILENO);
    _Exit(1);
}

int main(int argc,char **argv) {
    if (argc!=4 || !HGNativeFSSetRoot(argv[1]) || !HGNativeBackupSetPath(argv[3])) {
        fprintf(stderr,"Expected extracted NitroFS directory, validated boot header and native save path\n"); return 2;
    }
    unsigned char header[0x160];
    FILE *input=fopen(argv[2],"rb");
    if (!input) return 2;
    int valid=fread(header,1,sizeof(header),input)==sizeof(header) && fgetc(input)==EOF && !ferror(input);
    fclose(input);
    if (!valid) { fprintf(stderr,"Invalid boot-header size\n"); return 2; }
    wasm_rt_init();
    w2c_hgengine *game=calloc(1,sizeof(*game));
    struct w2c_env *env=calloc(1,sizeof(*env));
    struct w2c_wasi__snapshot__preview1 wasi={NULL,{dup(0),dup(1),dup(2)}};
    if (!game || !env || wasi.fds[0]<0 || wasi.fds[1]<0 || wasi.fds[2]<0) return 2;
    wasm_rt_trap_t trap=wasm_rt_impl_try();
    if (trap) {
        fprintf(stderr,"Original NitroMain trapped: %s; WASI exit=%u/%u\n",wasm_rt_strerror(trap),wasi.exited,wasi.exitCode);
        // A trap can leave native transfers pending; this isolated process owns their lifetime.
        return 1;
    }
    wasm2c_hgengine_instantiate(game,env,&wasi);
    env->memory=wasi.memory=w2c_hgengine_memory(game);
    env->irqTable=&game->w2c_T0; env->stackPointer=&game->w2c_0x5F_stack_pointer;
    env->rootStackHigh=*env->stackPointer; *env->stackPointer-=16;
    env->irqType0=wasm2c_hgengine_get_func_type(0,0);
    env->irqType1=wasm2c_hgengine_get_func_type(1,0,WASM_RT_I32);
    env->rtcType=wasm2c_hgengine_get_func_type(2,0,WASM_RT_I32,WASM_RT_I32);
    env->irqGuardType=wasm2c_hgengine_get_func_type(4,0,WASM_RT_I32,WASM_RT_I32,WASM_RT_I32,WASM_RT_I32);
    uint32_t work=w2c_hgengine_HGNativeSystemWorkAddress(game);
    if (!work || work>env->memory->size || 0x360>env->memory->size-work) return 2;
    memcpy(env->memory->data+work+0x200,header,sizeof(header));
    fprintf(stderr,"Native engine: initializing original static data\n");
    w2c_hgengine_0x5Finitialize(game);
    fprintf(stderr,"Native engine: entering original NitroMain\n");
    w2c_hgengine_NitroMain(game);
    fprintf(stderr,"Original NitroMain unexpectedly returned\n");
    return 1;
}
