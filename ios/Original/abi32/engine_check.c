// SPDX-License-Identifier: GPL-3.0-or-later
#include "hgengine.h"
#include "hgengine-impl.h"
#include "NativeImports.h"
#include "NativeWASIImports.h"
#include "../NativeFile.h"
#include "../NativeBackup.h"
#include "../NativeEvents.h"
#include "../NativeDMA.h"
#include "wasm-rt-impl.h"
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

WASM_RT_THREAD_LOCAL wasm_rt_jmp_buf g_wasm_rt_jmp_buf;
typedef struct Capture {
    w2c_hgengine *game;
    struct w2c_env *env;
    const char *folder;
    u32 next,buffer;
    u32 inputStep,inputFrame;
} Capture;
static void describeGeometry(Capture *capture,u32 offset) {
    if (!w2c_hgengine_HGNativeGeometryFrame(capture->game,1,offset)) return;
    u32 frame[8]; memcpy(frame,capture->env->memory->data+offset,sizeof(frame));
    fprintf(stderr,"Native geometry: vertices=%u polygons=%u serial=%u swap=%x clear=%x depth=%x control=%x alpha=%u\n",
        frame[0],frame[1],frame[2],frame[3],frame[4],frame[5],frame[6],frame[7]);
    u32 attributes=0,modes=0;
    for (u32 i=0;i<frame[1];i++) {
        if (!w2c_hgengine_HGNativeGeometryReadPolygon(capture->game,1,i,offset)) break;
        u32 polygon[7]; memcpy(polygon,capture->env->memory->data+offset,sizeof(polygon));
        attributes|=polygon[3]; modes|=1u<<((polygon[3]>>4)&3);
        if (i<3)
            fprintf(stderr,"Native polygon %u: count=%u attributes=%x texture=%x palette=%x viewport=%x\n",
                i,polygon[2],polygon[3],polygon[4],polygon[5],polygon[6]);
    }
    fprintf(stderr,"Native geometry attribute union=%x polygon modes=%x\n",attributes,modes);
}
static void present(void *argument) {
    Capture *capture=argument; u32 frame=OS_GetVBlankCount();
    static const u32 targets[]={1,60,180,360,720,1080,1440,1800,1980,2040,2340,2400};
    // Preserve the intro/title, then confirm the original title and new-game menu.
    static const struct { u32 frame,keys,x,y; BOOL pressed; } inputs[]={
        {450,8,0,0,FALSE},{780,1,0,0,FALSE},{1320,1,0,0,FALSE},{1500,0,128,154,TRUE},
        {1860,1,0,0,FALSE},{1980,1,0,0,FALSE},{2100,1,0,0,FALSE},{2220,1,0,0,FALSE}
    };
    if (capture->inputStep<sizeof(inputs)/sizeof(inputs[0])) {
        if (!capture->inputFrame && frame>=inputs[capture->inputStep].frame) {
            if (!HGNativeSetInput(capture->env,inputs[capture->inputStep].keys,FALSE,inputs[capture->inputStep].x,inputs[capture->inputStep].y,inputs[capture->inputStep].pressed)) _Exit(1);
            capture->inputFrame=frame;
            fprintf(stderr,"Native test input: step=%u frame=%u keys=%x touch=%u,%u/%u\n",capture->inputStep,frame,inputs[capture->inputStep].keys,inputs[capture->inputStep].x,inputs[capture->inputStep].y,inputs[capture->inputStep].pressed);
        } else if (capture->inputFrame && frame>=capture->inputFrame+8) {
            if (!HGNativeSetInput(capture->env,0,FALSE,0,0,FALSE)) _Exit(1);
            capture->inputFrame=0; capture->inputStep++;
        }
    }
    if (capture->next>=sizeof(targets)/sizeof(targets[0]) || frame<targets[capture->next]) return;
    capture->next++; HGNativeDrainDMA();
    if (!capture->buffer) capture->buffer=w2c_hgengine_HGNativeFrameBuffer(capture->game);
    u32 offset=capture->buffer;
    if (!offset || offset>capture->env->memory->size || 256*192*4>capture->env->memory->size-offset) {
        fputs("Native frame output span is invalid\n",stderr); _Exit(1);
    }
    for (u32 engine=0;engine<2;engine++) {
        if (!w2c_hgengine_HGNativeRender2D(capture->game,engine,offset,256*192*4)) {
            fprintf(stderr,"Native frame %u engine %u: unsupported render state\n",frame,engine);
            if (!engine) describeGeometry(capture,offset);
            continue;
        }
        const u8 *pixels=capture->env->memory->data+offset; BOOL varied=FALSE;
        for (u32 i=1;i<256*192;i++) if (memcmp(pixels,pixels+i*4,4)) { varied=TRUE; break; }
        fprintf(stderr,"Native frame %u engine %u: rendered, nonuniform=%u\n",frame,engine,varied);
        if (capture->folder) {
            char name[4096]; int size=snprintf(name,sizeof(name),"%s/frame-%06u-engine-%u.ppm",capture->folder,frame,engine);
            if (size<0 || size>=sizeof(name)) _Exit(1);
            FILE *output=fopen(name,"wb"); if (!output) _Exit(1);
            if (fprintf(output,"P6\n256 192\n255\n")<0) _Exit(1);
            u8 row[256*3];
            for (u32 y=0;y<192;y++) {
                for (u32 x=0;x<256;x++) memcpy(row+x*3,pixels+(y*256+x)*4,3);
                if (fwrite(row,1,sizeof(row),output)!=sizeof(row)) _Exit(1);
            }
            if (fclose(output)) _Exit(1);
        }
    }
}

void HGEngineTrap(wasm_rt_trap_t code) {
    void *frames[48];
    fprintf(stderr,"Original engine native trap: %s\n",wasm_rt_strerror(code));
    backtrace_symbols_fd(frames,backtrace(frames,48),STDERR_FILENO);
    _Exit(1);
}

int main(int argc,char **argv) {
    if ((argc!=4 && argc!=5) || !HGNativeFSSetRoot(argv[1]) || !HGNativeBackupSetPath(argv[3])) {
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
    if (!HGNativeSoundRealtime(env,TRUE)) return 2;
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
    uint32_t displayStatus=w2c_hgengine_HGNativeRegisterAddress(game,0x04000004,2);
    if (!displayStatus || displayStatus%2 || displayStatus>env->memory->size || 2>env->memory->size-displayStatus ||
        !HGNativeStartDisplayClock(env,(const vu16 *)(env->memory->data+displayStatus))) return 2;
    Capture capture={game,env,argc==5 ? argv[4] : NULL,0,0,0,0};
    if (!HGNativeSetDisplayObserver(env,present,&capture)) return 2;
    fprintf(stderr,"Native engine: entering original NitroMain\n");
    w2c_hgengine_NitroMain(game);
    fprintf(stderr,"Original NitroMain unexpectedly returned\n");
    return 1;
}
