// SPDX-License-Identifier: GPL-3.0-or-later
#include "stripe.h"
#include "NativeImports.h"
#include "NativeWASIImports.h"
#include "NativeEvents.h"
#include "NativeDMA.h"
#include "wasm-rt-impl.h"
#include "wasm-rt-exceptions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct StripeCapture {
    w2c_stripe *game;
    struct w2c_env *env;
    u32 phase,line,frames;
    int done;
} StripeCapture;

static void present(void *argument) {
    StripeCapture *capture=argument;
    capture->frames++;
    if (capture->phase<2) {
        capture->line=w2c_stripe_HGStripeCheckFrame(capture->game,capture->phase);
        if (capture->line) { capture->done=1; return; }
        capture->line=capture->phase ? w2c_stripe_HGStripeStop(capture->game) : w2c_stripe_HGStripeAdvance(capture->game);
        capture->phase++;
    } else {
        capture->line=w2c_stripe_HGStripeCheckStopped(capture->game);
        capture->done=1;
    }
    if (capture->line) capture->done=1;
}

static int runCase(s32 direction) {
    w2c_stripe game;
    struct w2c_env native={0};
    struct w2c_wasi__snapshot__preview1 wasi={NULL,{dup(0),dup(1),dup(2)}};
    StripeCapture capture={&game,&native,0,0,0,0};
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
            capture.line=w2c_stripe_HGStripeSetup(&game,(u32)direction); started=!capture.line;
        }
        if (!capture.line) {
            u32 status=w2c_stripe_HGStripeDisplayStatus(&game);
            if (!status || status>native.memory->size || 2>native.memory->size-status ||
                !HGNativeStartDisplayClock(&native,(const vu16 *)(native.memory->data+status)) ||
                !HGNativeSetDisplayObserver(&native,present,&capture)) capture.line=__LINE__;
            else while (!capture.done) HGNativeHalt();
        }
    }
    if (trap) { fprintf(stderr,"Stripe native trap: %s\n",wasm_rt_strerror(trap)); _Exit(23); }
    HGNativeStopDisplayClock(&native);
    if (capture.line) {
        u32 address=w2c_stripe_HGStripeFailure(&game),failure[4]={0};
        if (address && address<=native.memory->size && sizeof(failure)<=native.memory->size-address)
            memcpy(failure,native.memory->data+address,sizeof(failure));
        fprintf(stderr,"Stripe fixture line %u: sign=%d phase=%u pixel=%u,%u expected=%x actualRGBA=%x\n",
            capture.line,direction,capture.phase,failure[0],failure[1],failure[2],failure[3]);
    }
    if (started && !trap) {
        u32 cleanup=w2c_stripe_HGStripeFinish(&game);
        if (cleanup && !capture.line) capture.line=cleanup;
    }
    HGNativeDrainDMA(); HGNativeReleaseIrqCallbacks(&native);
    wasm2c_stripe_free(&game);
    for (u32 i=0;i<3;i++) if (wasi.fds[i]>=0) close(wasi.fds[i]);
    if (!trap && !capture.line) printf("PASS: original StripeWipe sign=%d, two Task_Update frames, full row9/17 pixels, disabled canaries, frames=%u.\n",direction,capture.frames);
    return trap || capture.line ? 1 : 0;
}

int main(void) {
    wasm_rt_init();
    int result=runCase(1);
    if (!result) result=runCase(-1);
    wasm_rt_free(); return result;
}
