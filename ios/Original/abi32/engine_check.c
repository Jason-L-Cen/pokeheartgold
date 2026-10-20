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
    BOOL noInput,resume,walkOut,starter;
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
    static const u32 targets[]={1,60,180,360,720,1080,1440,1800,1980,2040,2340,2400,2700,3000,3600,4200,4800,5400,6000,6600,7200,7800,7980,8100,8220,8340,8460,9000,9600,10800,12000,12240,12480,12960,13560,14400,15000};
    // Drive original intro choices and a test name through the original field-start path.
    struct TestInput { u32 frame,keys,x,y; BOOL pressed; };
    static const struct TestInput newGameInputs[]={
        {450,8,0,0,FALSE},{780,1,0,0,FALSE},{1320,1,0,0,FALSE},{1500,0,128,154,TRUE},
        {1860,1,0,0,FALSE},{1980,1,0,0,FALSE},{2100,1,0,0,FALSE},{2220,1,0,0,FALSE},
        {2400,1,0,0,FALSE},{2520,1,0,0,FALSE},{2640,1,0,0,FALSE},{2760,1,0,0,FALSE},
        {2880,1,0,0,FALSE},{3000,1,0,0,FALSE},{3120,1,0,0,FALSE},{3240,1,0,0,FALSE},
        {3360,1,0,0,FALSE},{3480,1,0,0,FALSE},{3600,1,0,0,FALSE},{3720,1,0,0,FALSE},
        {3840,1,0,0,FALSE},{3960,1,0,0,FALSE},{4080,1,0,0,FALSE},{4200,1,0,0,FALSE},
        {4320,1,0,0,FALSE},{4440,1,0,0,FALSE},{4560,1,0,0,FALSE},{4680,1,0,0,FALSE},
        {4800,1,0,0,FALSE},{4920,1,0,0,FALSE},{5040,1,0,0,FALSE},{5160,1,0,0,FALSE},
        {5280,1,0,0,FALSE},{5520,0,216,70,TRUE},
        {5640,1,0,0,FALSE},{5760,1,0,0,FALSE},{5880,1,0,0,FALSE},{6000,1,0,0,FALSE},
        {6120,1,0,0,FALSE},{6240,1,0,0,FALSE},{6360,1,0,0,FALSE},{6480,1,0,0,FALSE},
        {6600,1,0,0,FALSE},{6720,1,0,0,FALSE},{6840,1,0,0,FALSE},{6960,1,0,0,FALSE},
        {7080,1,0,0,FALSE},{7200,1,0,0,FALSE},{7320,1,0,0,FALSE},{7440,1,0,0,FALSE},
        {7560,1,0,0,FALSE},{7920,0x20,0,0,FALSE},{8040,0x20,0,0,FALSE},{8160,0x20,0,0,FALSE},
        {8280,0x40,0,0,FALSE},{8400,0x40,0,0,FALSE},{8520,0x20,0,0,FALSE},
        {9000,1,0,0,FALSE},{9120,1,0,0,FALSE},{9240,1,0,0,FALSE},{9360,1,0,0,FALSE},
        {9480,1,0,0,FALSE},{9600,1,0,0,FALSE},{9720,1,0,0,FALSE},{9840,1,0,0,FALSE},
        {9960,1,0,0,FALSE},{10080,1,0,0,FALSE},{10200,1,0,0,FALSE},{10320,1,0,0,FALSE},
        {10440,1,0,0,FALSE},{10560,1,0,0,FALSE},{10680,1,0,0,FALSE},{10800,1,0,0,FALSE},
        {10920,1,0,0,FALSE},{11040,1,0,0,FALSE},{11160,1,0,0,FALSE},{11280,1,0,0,FALSE},
        {11400,1,0,0,FALSE},{11520,1,0,0,FALSE},{11640,1,0,0,FALSE},{11760,1,0,0,FALSE},
        {11880,1,0,0,FALSE},{12000,1,0,0,FALSE},{12120,0x400,0,0,FALSE},
        {12360,0,124,80,TRUE},{12600,1,0,0,FALSE},{12720,1,0,0,FALSE}
    };
    static const struct TestInput continueInputs[]={
        {450,8,0,0,FALSE},{780,1,0,0,FALSE},{1320,1,0,0,FALSE},
        {1800,0x80,0,0,FALSE},{1920,0x80,0,0,FALSE},{2040,0x80,0,0,FALSE},
        {2160,0x80,0,0,FALSE},{2280,0x80,0,0,FALSE},{2400,0x80,0,0,FALSE},{2520,0x80,0,0,FALSE},
        {2640,0x80,0,0,FALSE},{2760,0x80,0,0,FALSE}
    };
    // Continue the saved home route to the original lab, without changing story flags or position.
    static const struct TestInput starterInputs[]={
        {3120,0x80,0,0,FALSE},
        {4440,0x20,0,0,FALSE},
        {4560,0x20,0,0,FALSE},
        {4680,0x20,0,0,FALSE},
        {4800,0x20,0,0,FALSE},
        {4920,0x20,0,0,FALSE},
        {5040,0x20,0,0,FALSE},
        {5160,0x20,0,0,FALSE},
        {5280,0x20,0,0,FALSE},
        {5400,0x20,0,0,FALSE},
        {5520,0x20,0,0,FALSE},
        {5640,0x20,0,0,FALSE},
        {5760,0x40,0,0,FALSE},
        {5880,0x40,0,0,FALSE},
        {6000,0x40,0,0,FALSE},
        {6120,0x40,0,0,FALSE},
        {6240,0x40,0,0,FALSE},
        {6360,0x40,0,0,FALSE},
        {6480,0x40,0,0,FALSE},
        {6600,0x40,0,0,FALSE},
        {6720,0x40,0,0,FALSE},
        {6840,0x40,0,0,FALSE},
        {6960,0x40,0,0,FALSE},
        {7080,0x40,0,0,FALSE},
        {7200,0x1,0,0,FALSE},
        {7320,0x1,0,0,FALSE},
        {7440,0x1,0,0,FALSE},
        {7560,0x1,0,0,FALSE},
        {7680,0x1,0,0,FALSE},
        {7800,0x1,0,0,FALSE},
        {7920,0x1,0,0,FALSE},
        {8040,0x1,0,0,FALSE},
        {8160,0x1,0,0,FALSE},
        {8280,0x1,0,0,FALSE},
        {8400,0x1,0,0,FALSE},
        {8520,0x1,0,0,FALSE},
        {8640,0x1,0,0,FALSE},
        {8760,0x1,0,0,FALSE},
        {8880,0x1,0,0,FALSE},
        {9000,0x1,0,0,FALSE},
        {9120,0x1,0,0,FALSE},
        {9240,0x1,0,0,FALSE},
        {9360,0x1,0,0,FALSE},
        {9480,0x1,0,0,FALSE},
        {9600,0x1,0,0,FALSE},
        {9720,0x1,0,0,FALSE},
        {9840,0x1,0,0,FALSE},
        {9960,0x1,0,0,FALSE},
        {10080,0x1,0,0,FALSE},
        {10200,0x1,0,0,FALSE},
        {10320,0x1,0,0,FALSE},
        {10440,0x1,0,0,FALSE},
        {10560,0x1,0,0,FALSE},
        {10680,0x1,0,0,FALSE},
        {10800,0x1,0,0,FALSE},
        {10920,0x1,0,0,FALSE},
        {11040,0x1,0,0,FALSE},
        {11160,0x1,0,0,FALSE},
        {11280,0x1,0,0,FALSE},
        {11400,0x1,0,0,FALSE},
        {11520,0x1,0,0,FALSE},
        {11640,0x1,0,0,FALSE},
        {11760,0x1,0,0,FALSE},
        {11880,0x1,0,0,FALSE},
        {12000,0x1,0,0,FALSE},
        {12120,0x1,0,0,FALSE},
        {12240,0x1,0,0,FALSE},
        {12360,0x1,0,0,FALSE},
        {12480,0x1,0,0,FALSE},
        {12600,0x10,0,0,FALSE},
        {12720,0x10,0,0,FALSE},
        {12840,0x40,0,0,FALSE},
        {12960,0x10,0,0,FALSE},
        {13080,0x10,0,0,FALSE},
        {13200,0x40,0,0,FALSE},
        {13320,0x1,0,0,FALSE},
        {13440,0x1,0,0,FALSE},
        {13560,0x1,0,0,FALSE},
        {13680,0x1,0,0,FALSE},
        {13800,0x1,0,0,FALSE},
        {13920,0x1,0,0,FALSE},
        {14040,0x1,0,0,FALSE},
        {14160,0x1,0,0,FALSE},
        {14280,0x1,0,0,FALSE},
        {14400,0x1,0,0,FALSE}
    };
    u32 continueCount=sizeof(continueInputs)/sizeof(continueInputs[0]);
    const struct TestInput *inputs=capture->resume ? continueInputs : newGameInputs;
    u32 inputCount=capture->resume ? (capture->walkOut ? continueCount : 3) : sizeof(newGameInputs)/sizeof(newGameInputs[0]);
    u32 step=capture->inputStep;
    if (capture->starter && step>=continueCount) { inputs=starterInputs; step-=continueCount; inputCount=sizeof(starterInputs)/sizeof(starterInputs[0]); }
    if (!capture->noInput && step<inputCount) {
        if (!capture->inputFrame && frame>=inputs[step].frame) {
            if (!HGNativeSetInput(capture->env,inputs[step].keys,FALSE,inputs[step].x,inputs[step].y,inputs[step].pressed)) _Exit(1);
            capture->inputFrame=frame;
            fprintf(stderr,"Native test input: step=%u frame=%u keys=%x touch=%u,%u/%u\n",capture->inputStep,frame,inputs[step].keys,inputs[step].x,inputs[step].y,inputs[step].pressed);
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
    if ((argc!=4 && argc!=5 && argc!=6) || (argc==6 && strcmp(argv[5],"--no-input") && strcmp(argv[5],"--continue") && strcmp(argv[5],"--continue-walk-out") && strcmp(argv[5],"--continue-starter")) || !HGNativeFSSetRoot(argv[1]) || !HGNativeBackupSetPath(argv[3])) {
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
    BOOL starter=argc==6 && !strcmp(argv[5],"--continue-starter");
    Capture capture={game,env,argc>=5 ? argv[4] : NULL,0,0,0,0,argc==6 && !strcmp(argv[5],"--no-input"),argc==6 && strcmp(argv[5],"--no-input"),starter || (argc==6 && !strcmp(argv[5],"--continue-walk-out")),starter};
    if (!HGNativeSetDisplayObserver(env,present,&capture)) return 2;
    fprintf(stderr,"Native engine: entering original NitroMain\n");
    w2c_hgengine_NitroMain(game);
    fprintf(stderr,"Original NitroMain unexpectedly returned\n");
    return 1;
}
