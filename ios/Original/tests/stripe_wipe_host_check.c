// SPDX-License-Identifier: GPL-3.0-or-later
#include "stripe.h"
#include "NativeImports.h"
#include "NativeWASIImports.h"
#include "NativeEvents.h"
#include "NativeDMA.h"
#include "wasm-rt-impl.h"
#include "wasm-rt-exceptions.h"
#include <nitro/os/system.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct StripeCapture {
    w2c_stripe *game;
    struct w2c_env *env;
    u32 phase,line,frames;
    u32 kind,mode,masked;
    int done;
} StripeCapture;

static void present(void *argument) {
    StripeCapture *capture=argument;
    capture->frames++;
    if (capture->masked) {
        // The first observer is deferred until CPSR.I is restored, after original Stop/Delete.
        if (!capture->phase++) return;
        capture->line=w2c_stripe_HGScanlineCheckStopped(capture->game);
        capture->done=1; return;
    }
    if (capture->phase<2) {
        capture->line=capture->kind ? w2c_stripe_HGScanlineCheckFrame(capture->game,capture->phase) : w2c_stripe_HGStripeCheckFrame(capture->game,capture->phase);
        if (capture->line) { capture->done=1; return; }
        capture->line=capture->kind ? (capture->phase ? w2c_stripe_HGScanlineStop(capture->game) : w2c_stripe_HGScanlineAdvance(capture->game)) :
            (capture->phase ? w2c_stripe_HGStripeStop(capture->game) : w2c_stripe_HGStripeAdvance(capture->game));
        capture->phase++;
    } else {
        capture->line=capture->kind ? w2c_stripe_HGScanlineCheckStopped(capture->game) : w2c_stripe_HGStripeCheckStopped(capture->game);
        capture->done=1;
    }
    if (capture->line) capture->done=1;
}

static int runCase(s32 direction,u32 kind,u32 mode,u32 masked) {
    w2c_stripe game;
    struct w2c_env native={0};
    struct w2c_wasi__snapshot__preview1 wasi={NULL,{dup(0),dup(1),dup(2)}};
    StripeCapture capture={.game=&game,.env=&native,.kind=kind,.mode=mode,.masked=masked};
    wasm2c_stripe_instantiate(&game,&native,&wasi);
    native.memory=wasi.memory=w2c_stripe_memory(&game);
    native.irqTable=&game.w2c_T0; native.stackPointer=&game.w2c_0x5F_stack_pointer;
    native.rootStackHigh=*native.stackPointer; *native.stackPointer-=16;
    native.irqType0=wasm2c_stripe_get_func_type(0,0);
    native.irqType1=wasm2c_stripe_get_func_type(1,0,WASM_RT_I32);
    native.rtcType=wasm2c_stripe_get_func_type(2,0,WASM_RT_I32,WASM_RT_I32);
    native.irqGuardType=wasm2c_stripe_get_func_type(4,0,WASM_RT_I32,WASM_RT_I32,WASM_RT_I32,WASM_RT_I32);
    wasm_rt_trap_t trap=wasm_rt_impl_try();
    int started=0;
    if (!trap) {
        w2c_stripe_0x5Finitialize(&game);
        capture.line=w2c_stripe_InitializeOriginalGameHeap(&game);
        if (!capture.line) {
            w2c_stripe_HGIRQInit(&game); // Original client automatically registers the real kind3 bridge.
            capture.line=kind ? w2c_stripe_HGScanlineSetup(&game,kind-1,mode) : w2c_stripe_HGStripeSetup(&game,(u32)direction); started=!capture.line;
        }
        if (!capture.line) {
            u32 status=w2c_stripe_HGStripeDisplayStatus(&game);
            if (!status || status>native.memory->size || 2>native.memory->size-status ||
                !HGNativeStartDisplayClock(&native,(const vu16 *)(native.memory->data+status)) ||
                !HGNativeSetDisplayObserver(&native,present,&capture)) capture.line=__LINE__;
            else {
                if (masked) {
                    OSIntrMode previous=OS_DisableInterrupts();
                    for (u32 i=0;i<1000 && !w2c_stripe_HGScanlineEpochs(&game);i++) HGNativeWaitNanoseconds(1000000);
                    if (capture.frames || w2c_stripe_HGScanlineEpochs(&game)!=1) capture.line=__LINE__;
                    if (!capture.line) capture.line=w2c_stripe_HGScanlineCheckMasked(&game);
                    if (!capture.line) capture.line=w2c_stripe_HGScanlineStop(&game);
                    if (__atomic_load_n(&native.pendingDMA,__ATOMIC_ACQUIRE)) capture.line=__LINE__;
                    OS_RestoreInterrupts(previous);
                    if (capture.frames!=1 || capture.phase!=1) capture.line=__LINE__;
                }
                while (!capture.done && !capture.line) HGNativeHalt();
            }
        }
    }
    if (trap) { fprintf(stderr,"Stripe native trap: %s\n",wasm_rt_strerror(trap)); _Exit(23); }
    HGNativeStopDisplayClock(&native);
    if (capture.line) {
        u32 address=w2c_stripe_HGStripeFailure(&game),failure[4]={0};
        if (address && address<=native.memory->size && sizeof(failure)<=native.memory->size-address)
            memcpy(failure,native.memory->data+address,sizeof(failure));
        if (!kind) fprintf(stderr,"Stripe fixture line %u: sign=%d phase=%u pixel=%u,%u expected=%x actualRGBA=%x\n",
            capture.line,direction,capture.phase,failure[0],failure[1],failure[2],failure[3]);
        else fprintf(stderr,"HDMA fixture line %u: kind=%u mode=%u masked=%u phase=%u pixel=%u,%u expected=%x actualRGBA=%x\n",
            capture.line,kind,mode,masked,capture.phase,failure[0],failure[1],failure[2],failure[3]);
    }
    if (started && !trap) {
        u32 cleanup=kind ? w2c_stripe_HGScanlineFinish(&game) : w2c_stripe_HGStripeFinish(&game);
        if (cleanup && !capture.line) capture.line=cleanup;
    }
    if (__atomic_load_n(&native.pendingDMA,__ATOMIC_ACQUIRE)) capture.line=__LINE__;
    HGNativeDrainDMA(); HGNativeReleaseIrqCallbacks(&native);
    wasm2c_stripe_free(&game);
    for (u32 i=0;i<3;i++) if (wasi.fds[i]>=0) close(wasi.fds[i]);
    if (!trap && !capture.line) {
        if (!kind) printf("PASS: original StripeWipe sign=%d, two Task_Update frames, full row9/17 pixels, disabled canaries, frames=%u.\n",direction,capture.frames);
        else printf("PASS: original HDMA kind=%u mode=%u masked=%u, exact pixels, real queues, Stop/Delete, pendingDMA=0, frames=%u.\n",kind,mode,masked,capture.frames);
    }
    return trap || capture.line ? 1 : 0;
}

int main(void) {
    wasm_rt_init();
    int result=runCase(1,0,0,0);
    if (!result) result=runCase(-1,0,0,0);
    for (u32 mode=0;mode<3 && !result;mode++) result=runCase(0,1,mode,0);
    for (u32 mode=0;mode<4 && !result;mode++) result=runCase(0,2,mode,0);
    if (!result) result=runCase(0,3,0,0);
    if (!result) result=runCase(0,1,0,1);
    wasm_rt_free(); return result;
}
