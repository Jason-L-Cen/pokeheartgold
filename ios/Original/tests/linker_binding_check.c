// SPDX-License-Identifier: GPL-3.0-or-later
#include "global.h"
#include "poke_overlay.h"
#include <nnsys/snd_internal.h>
#include <limits.h>
#include "../../../src/frontier_runtime.c"
#define CHECK(v) do { if (!(v)) return __LINE__; } while (0)

// Observe module requests only; these bindings do not load or run an overlay.
static u32 requests[6], requestCount;
static BOOL badRequest;
BOOL HandleLoadOverlay(FSOverlayID id, PMOverlayLoadType mode) {
    if (requestCount>=6) { badRequest=TRUE; return FALSE; }
    requests[requestCount++]=id;
    if (mode!=OVY_LOAD_ASYNC) badRequest=TRUE;
    return TRUE;
}
void UnloadOverlayByID(FSOverlayID id) {
    if (requestCount>=6) { badRequest=TRUE; return; }
    requests[requestCount++]=id|0x80000000u;
}

u32 CheckOriginalLinkerBindings(void) {
    const u32 expected[]={80,81,42,0x80000050u,0x80000051u,0x8000002Au};
    for (u32 run=0;run<2;run++) {
        requestCount=0; badRequest=FALSE;
        Frontier_LoadOverlays(); Frontier_UnloadOverlays();
        CHECK(requestCount==6 && !badRequest);
        for (u32 i=0;i<6;i++) CHECK(requests[i]==expected[i]);
    }
    _Static_assert(sizeof(SNDi_DecibelTable)==256,"Original decibel table width");
    CHECK(SNDi_DecibelTable[0]==-32768);
    for (u32 volume=1;volume<128;volume++)
        CHECK(SNDi_DecibelTable[volume]==__builtin_lround(200*__builtin_log10((double)volume/127)));
    CHECK(SND_CalcChannelVolume(INT_MIN)==0x300 && SND_CalcChannelVolume(INT_MAX)==127);
    const u32 divisors[]={1,2,4,16};
    double previous=0;
    for (int db=-723;db<=0;db++) {
        u16 packed=SND_CalcChannelVolume(db);
        u32 amplitude=packed&255, shift=packed>>8;
        CHECK(shift<4 && amplitude<=127);
        double gain=(double)amplitude/divisors[shift];
        double ideal=128*__builtin_pow(10,(double)db/200)*divisors[shift];
        if (ideal>127) ideal=127;
        CHECK(gain>=previous && __builtin_fabs(amplitude-ideal)<=1.000001);
        previous=gain;
    }
    SndFader fader;
    NNSi_SndFaderInit(&fader);
    NNSi_SndFaderSet(&fader,32*256,0);
    NNSi_SndFaderSet(&fader,96*256,4);
    for (int frame=0;frame<=4;frame++) {
        CHECK(NNSi_SndFaderGet(&fader)==(32+16*frame)*256);
        CHECK(NNSi_SndFaderIsFinished(&fader)==(frame==4));
        NNSi_SndFaderUpdate(&fader);
    }
    CHECK(fader.counter==4);
    NNSi_SndFaderSet(&fader,0,2); NNSi_SndFaderUpdate(&fader);
    CHECK(NNSi_SndFaderGet(&fader)==48*256);
    NNSi_SndFaderSet(&fader,127*256,1); NNSi_SndFaderUpdate(&fader);
    CHECK(NNSi_SndFaderGet(&fader)==127*256 && NNSi_SndFaderIsFinished(&fader));
    return 0;
}
