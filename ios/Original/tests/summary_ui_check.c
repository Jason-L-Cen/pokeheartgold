// SPDX-License-Identifier: GPL-3.0-or-later
#include "global.h"
#include "native_summary_ui.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

static void SummaryCheckClear(void *destination, u32 size);
static void SummaryCheckCopy16(const void *source, void *destination, u32 size);
#define MI_CpuClear8 SummaryCheckClear
#define MI_CpuCopy16 SummaryCheckCopy16
#define IsPaletteFadeFinished SummaryCheckFade
#define GetMainBgPlttAddr SummaryCheckPalette
#define GetSubBgPlttAddr SummaryCheckPalette
#include "../../../src/pokemon_summary.c"

#define CHECK(value) do { if (!(value)) return __LINE__; } while (0)

static u16 summaryCheckPalette[256];
static BOOL summaryCheckFadeFinished=TRUE;
static void SummaryCheckClear(void *destination, u32 size) { memset(destination,0,size); }
static void SummaryCheckCopy16(const void *source, void *destination, u32 size) { memcpy(destination,source,size); }
BOOL SummaryCheckFade(void) { return summaryCheckFadeFinished; }
u16 *SummaryCheckPalette(void) { return summaryCheckPalette; }
static void SummaryCheckWindow(Window *window, BgConfig *bg, u8 layer, u8 width, u8 height) {
    *window=(Window){.bgConfig=bg,.bgId=layer,.width=width,.height=height,.paletteNum=13,
        .colorMode=GF_BG_CLR_4BPP,.pixelBuffer=(void *)(uintptr_t)0x1000};
}

static uint32_t SummaryCheckPublisher(void) {
    BgConfig bg={0}; Window pageWindows[8]={0};
    PokemonSummaryArgs args={.monDataType=1,.partyCount=1,.pageFlags=7};
    PokemonSummary summary={.bgConfig=&bg,.pageWindows=pageWindows,.numPageWindows=8,.args=&args};
    summary.mon.species=SPECIES_CHIKORITA; summary.mon.level=5; summary.mon.hp=summary.mon.maxHp=20;
    bg.bgs[GF_BG_LYR_MAIN_1].colorMode=bg.bgs[GF_BG_LYR_SUB_0].colorMode=GX_BG_COLORMODE_16;
    SummaryCheckWindow(&summary.windows[33],&bg,GF_BG_LYR_MAIN_1,9,2);
    SummaryCheckWindow(&summary.windows[32],&bg,GF_BG_LYR_MAIN_1,6,2);
    SummaryCheckWindow(&summary.windows[31],&bg,GF_BG_LYR_MAIN_1,12,2);
    SummaryCheckWindow(&summary.windows[23],&bg,GF_BG_LYR_SUB_0,5,2);
    SummaryCheckWindow(&summary.windows[0],&bg,GF_BG_LYR_SUB_0,11,2);
    NativeSummaryUIBind(&summary); NativeSummaryUIPublish(&summary,HG_SUMMARY_UI_INPUT);
    const HGNativeSummaryUIState *state=HGNativeSummaryUIGetState();
    CHECK(state->ready && state->nickname.pixelBuffer && state->pageTitle.pixelBuffer);
    uint32_t revision=state->revision; HGNativeSummaryUIInput input;
    for (u32 bit=0;bit<8;++bit) for (u32 scroll=0;scroll<2;++scroll) {
        bg.bufferTransferScheduled=scroll ? 0 : 1u<<bit; bg.scrollScheduled=scroll ? 1u<<bit : 0;
        NativeSummaryUIBegin(&summary); NativeSummaryUIPublish(&summary,HG_SUMMARY_UI_INPUT);
        CHECK(!state->ready && state->revision==revision
            && !HGNativeSummaryUIResolveInput(state,revision,HG_SUMMARY_UI_INPUT,HG_SUMMARY_UI_INPUT_BACK,0,&input));
        bg.bufferTransferScheduled=bg.scrollScheduled=0; NativeSummaryUIPublish(&summary,HG_SUMMARY_UI_INPUT);
        CHECK(state->ready && state->revision==revision
            && HGNativeSummaryUIResolveInput(state,revision,HG_SUMMARY_UI_INPUT,HG_SUMMARY_UI_INPUT_BACK,0,&input));
    }
    bg.bufferTransferScheduled=bg.scrollScheduled=0x100; NativeSummaryUIPublish(&summary,HG_SUMMARY_UI_INPUT);
    CHECK(state->ready && state->revision==revision);
    summary.bgConfig=NULL; NativeSummaryUIPublish(&summary,HG_SUMMARY_UI_INPUT); CHECK(!state->ready);
    summary.bgConfig=&bg; bg.bufferTransferScheduled=bg.scrollScheduled=0;
    NativeSummaryUIPublish(&summary,HG_SUMMARY_UI_INPUT); CHECK(state->ready);
    NativeSummaryUIBind(NULL); CHECK(!state->active && !state->ready && !state->nickname.pixelBuffer);
    return 0;
}

static uint32_t SummaryCheckLearning(void) {
    BgConfig bg={0}; Window pageWindows[18]={0}; Party party={.core={.maxCount=6,.curCount=3}};
    PokemonSummaryArgs args={.party=&party,.mode=2,.monDataType=1,.partyCount=1,.partySlot=2,.pageFlags=2,.moveToLearn=MOVE_SWALLOW};
    PokemonSummary summary={.bgConfig=&bg,.page=1,.pageWindows=pageWindows,.numPageWindows=18,.args=&args};
    summary.mon.species=SPECIES_CARNIVINE; summary.mon.level=31; summary.mon.hp=summary.mon.maxHp=86;
    const u16 moves[4]={MOVE_VINE_WHIP,MOVE_CUT,MOVE_GROWTH,MOVE_STOCKPILE};
    for (u32 i=0;i<4;++i) {
        summary.mon.moves[i]=moves[i]; summary.mon.pp[i]=summary.mon.maxPP[i]=20;
        SummaryCheckWindow(&pageWindows[8+i],&bg,GF_BG_LYR_SUB_0,11,4);
    }
    bg.bgs[GF_BG_LYR_MAIN_1].colorMode=bg.bgs[GF_BG_LYR_SUB_0].colorMode=GX_BG_COLORMODE_16;
    SummaryCheckWindow(&summary.windows[33],&bg,GF_BG_LYR_MAIN_1,9,2);
    SummaryCheckWindow(&summary.windows[32],&bg,GF_BG_LYR_MAIN_1,6,2);
    SummaryCheckWindow(&summary.windows[31],&bg,GF_BG_LYR_MAIN_1,12,2);
    SummaryCheckWindow(&summary.windows[23],&bg,GF_BG_LYR_SUB_0,5,2);
    SummaryCheckWindow(&summary.windows[4],&bg,GF_BG_LYR_SUB_0,11,2);
    NativeSummaryUIBind(&summary); NativeSummaryUIPublish(&summary,HG_SUMMARY_UI_FORGET_MOVE);
    const HGNativeSummaryUIState *state=HGNativeSummaryUIGetState(); HGNativeSummaryUIInput input;
    CHECK(state->ready && HGNativeSummaryUIIsLearning(state) && state->partyCount==3 && state->partySlot==2
        && args.partyCount==1 && state->moveToLearn==MOVE_SWALLOW && state->moveMask==31 && state->backEnabled
        && !state->previousPageEnabled && !state->nextPageEnabled && !state->previousMonEnabled && !state->nextMonEnabled);
    u32 revision=state->revision;
    for (u32 i=0;i<4;++i) CHECK(state->moves[i].moveId==moves[i] && state->moves[i].text.pixelBuffer
        && HGNativeSummaryUIResolveInput(state,revision,8,HG_SUMMARY_UI_INPUT_MOVE,i,&input)
        && input.touchPressed && !input.keys && input.touchX==67 && input.touchY==23+32*i);
    CHECK(HGNativeSummaryUIResolveInput(state,revision,8,HG_SUMMARY_UI_INPUT_MOVE,4,&input)
        && input.touchPressed && !input.keys && input.touchX==67 && input.touchY==167);
    CHECK(!HGNativeSummaryUIResolveInput(state,revision,8,HG_SUMMARY_UI_INPUT_CONFIRM,0,&input));
    for (u32 kind=HG_SUMMARY_UI_INPUT_PREVIOUS_PAGE;kind<=HG_SUMMARY_UI_INPUT_RIBBON_DETAILS;++kind)
        CHECK(!HGNativeSummaryUIResolveInput(state,revision,8,kind,0,&input));
    CHECK(!HGNativeSummaryUIResolveInput(state,revision,8,HG_SUMMARY_UI_INPUT_MOVE,5,&input));
    summary.moveSlot=4; NativeSummaryUIPublish(&summary,8);
    CHECK(state->ready && state->revision!=revision && !HGNativeSummaryUIResolveInput(state,revision,8,HG_SUMMARY_UI_INPUT_MOVE,0,&input));
    revision=state->revision;
    CHECK(HGNativeSummaryUIResolveInput(state,revision,8,HG_SUMMARY_UI_INPUT_CONFIRM,0,&input) && input.keys==1 && !input.touchPressed);
    CHECK(HGNativeSummaryUIResolveInput(state,revision,8,HG_SUMMARY_UI_INPUT_BACK,0,&input) && input.keys==2 && !input.touchPressed);
    summary.moveSlot=0; NativeSummaryUIPublish(&summary,9); revision=state->revision;
    CHECK(state->ready && !state->moveMask && state->moves[0].moveId==MOVE_VINE_WHIP && state->moves[1].moveId==MOVE_CUT
        && HGNativeSummaryUIResolveInput(state,revision,9,HG_SUMMARY_UI_INPUT_CONFIRM,0,&input) && input.keys==1
        && HGNativeSummaryUIResolveInput(state,revision,9,HG_SUMMARY_UI_INPUT_BACK,0,&input) && input.keys==2);
    for (u32 i=0;i<5;++i) CHECK(!HGNativeSummaryUIResolveInput(state,revision,9,HG_SUMMARY_UI_INPUT_MOVE,i,&input));
    CHECK(!HGNativeSummaryUIResolveInput(state,revision,9,HG_SUMMARY_UI_INPUT_CONFIRM,1,&input));
    summary.moveSlot=4; NativeSummaryUIPublish(&summary,9); CHECK(!state->ready);
    summary.moveSlot=0; NativeSummaryUIPublish(&summary,8); revision=state->revision;
    for (u32 bit=0;bit<8;++bit) for (u32 scroll=0;scroll<2;++scroll) {
        bg.bufferTransferScheduled=scroll ? 0 : 1u<<bit; bg.scrollScheduled=scroll ? 1u<<bit : 0;
        NativeSummaryUIBegin(&summary); CHECK(!state->ready && !state->moves[0].text.pixelBuffer && !state->nickname.pixelBuffer);
        NativeSummaryUIPublish(&summary,8);
        CHECK(!state->ready && state->revision==revision && !HGNativeSummaryUIResolveInput(state,revision,8,HG_SUMMARY_UI_INPUT_MOVE,4,&input));
        bg.bufferTransferScheduled=bg.scrollScheduled=0; CHECK(!state->ready);
        NativeSummaryUIPublish(&summary,8); CHECK(state->ready && state->revision==revision);
    }
    summaryCheckFadeFinished=FALSE; NativeSummaryUIPublish(&summary,8); CHECK(!state->ready);
    summaryCheckFadeFinished=TRUE; summary.unk7BF_0=15; NativeSummaryUIPublish(&summary,8); CHECK(!state->ready);
    summary.unk7BF_0=0; summary.exitRequested=1; NativeSummaryUIPublish(&summary,8); CHECK(!state->ready);
    summary.exitRequested=0; summary.moveSlot=5; NativeSummaryUIPublish(&summary,15);
    CHECK(HGNativeSummaryUIIsLearning(state) && !state->ready && !state->backEnabled && !state->moveMask);
    summary.moveSlot=0; NativeSummaryUIPublish(&summary,8); revision=state->revision;
    NativeSummaryUIPageWindows(&summary,FALSE); CHECK(!state->ready && !state->nickname.pixelBuffer && state->revision!=revision);
    summary.pageWindows=NULL; NativeSummaryUIPublish(&summary,8); CHECK(!state->ready);
    summary.pageWindows=pageWindows; NativeSummaryUIPageWindows(&summary,TRUE); NativeSummaryUIPublish(&summary,8);
    CHECK(state->ready && !HGNativeSummaryUIResolveInput(state,revision,8,HG_SUMMARY_UI_INPUT_MOVE,0,&input));
    Pokemon rawMon={0}; args.party=(Party *)&rawMon; args.monDataType=0; args.partySlot=0; NativeSummaryUIPublish(&summary,8);
    CHECK(state->ready && state->partyCount==1 && !state->partySlot && HGNativeSummaryUIIsLearning(state));
    args.partySlot=1; NativeSummaryUIPublish(&summary,8); CHECK(!state->ready);
    args.partySlot=0; args.partyCount=2; NativeSummaryUIPublish(&summary,8); CHECK(!state->ready);
    args.partyCount=1; args.monDataType=2; NativeSummaryUIPublish(&summary,8); CHECK(!state->ready);
    args.party=&party; args.monDataType=1; args.partySlot=2; args.moveToLearn=MOVE_NONE; NativeSummaryUIPublish(&summary,8); CHECK(!state->ready && !state->moveMask);
    args.moveToLearn=NUM_MOVES+1; NativeSummaryUIPublish(&summary,8); CHECK(!state->ready);
    args.moveToLearn=MOVE_SWALLOW; party.core.curCount=2; NativeSummaryUIPublish(&summary,8); CHECK(!state->ready);
    party.core.curCount=3; summary.moveSlot=5; NativeSummaryUIPublish(&summary,8); CHECK(!state->ready);
    summary.moveSlot=0; summary.mon.isEgg=TRUE; NativeSummaryUIPublish(&summary,8); CHECK(!state->ready);
    summary.mon.isEgg=FALSE; NativeSummaryUIPublish(&summary,8); CHECK(state->ready);
    HGNativeSummaryUIState cached=*state; CHECK(!memcmp(HGNativeSummaryUIGetState(),&cached,sizeof(cached)));
    NativeSummaryUIBind(NULL); CHECK(!state->active && !state->ready && !state->nickname.pixelBuffer && !state->moves[0].text.pixelBuffer);
    return 0;
}

uint32_t CheckOriginalSummaryUI(void) {
    uint32_t publisher=SummaryCheckPublisher(); if (publisher) return publisher;
    uint32_t learning=SummaryCheckLearning(); if (learning) return learning;
    CHECK(sizeof(HGNativeSummaryUIState)==804 && sizeof(HGNativeSummaryUIMove)==92 && sizeof(HGNativeSummaryUIInput)==16);
    CHECK(offsetof(HGNativeSummaryUIState,moves)==176 && offsetof(HGNativeSummaryUIState,nickname)==544
        && offsetof(HGNativeSummaryUIState,pageTitle)==752);
    HGNativeSummaryUIState state={.active=1,.revision=9,.phase=HG_SUMMARY_UI_INPUT,.ready=1,
        .page=1,.pageMask=7,.monDataType=1,.partyCount=1,.moveMask=3,.backEnabled=1,
        .previousPageEnabled=1,.previousPage=0,.nextPageEnabled=1,.nextPage=2};
    for (uint32_t i=0;i<HG_SUMMARY_UI_MOVES;i++) {
        state.moves[i]=(HGNativeSummaryUIMove){.moveId=i<2 ? 33+i : 0,.pp=i ? 40 : 0,.maxPP=i ? 40 : 35,.enabled=i<2,
            .hitX=67,.hitY=23+32*i,.cropX=8,.cropY=8+32*i,.cropWidth=119,.cropHeight=31};
    }
    HGNativeSummaryUIInput input={0};
    CHECK(HGNativeSummaryUIResolveInput(&state,9,2,HG_SUMMARY_UI_INPUT_MOVE,0,&input)
        && !input.keys && input.touchPressed && input.touchX==67 && input.touchY==23);
    CHECK(HGNativeSummaryUIResolveInput(&state,9,2,HG_SUMMARY_UI_INPUT_BACK,0,&input) && input.keys==2 && !input.touchPressed);
    CHECK(HGNativeSummaryUIResolveInput(&state,9,2,HG_SUMMARY_UI_INPUT_PREVIOUS_PAGE,0,&input) && input.keys==32);
    CHECK(HGNativeSummaryUIResolveInput(&state,9,2,HG_SUMMARY_UI_INPUT_NEXT_PAGE,0,&input) && input.keys==16);
    CHECK(!HGNativeSummaryUIResolveInput(&state,9,2,HG_SUMMARY_UI_INPUT_PREVIOUS_MON,0,&input)
        && !HGNativeSummaryUIResolveInput(&state,9,2,HG_SUMMARY_UI_INPUT_NEXT_MON,0,&input));
    CHECK(!HGNativeSummaryUIResolveInput(NULL,9,2,0,0,&input) && !HGNativeSummaryUIResolveInput(&state,9,2,0,0,NULL));
    CHECK(!HGNativeSummaryUIResolveInput(&state,8,2,0,0,&input) && !HGNativeSummaryUIResolveInput(&state,9,5,0,0,&input));
    CHECK(!HGNativeSummaryUIResolveInput(&state,9,2,0,2,&input) && !HGNativeSummaryUIResolveInput(&state,9,2,0,UINT32_MAX,&input));
    CHECK(!HGNativeSummaryUIResolveInput(&state,9,2,HG_SUMMARY_UI_INPUT_BACK,1,&input)
        && !HGNativeSummaryUIResolveInput(&state,9,2,UINT32_MAX,0,&input));
    HGNativeSummaryUIState valid=state;
#define REJECT(member,value) do { state=valid; state.member=(value); CHECK(!HGNativeSummaryUIResolveInput(&state,9,2,HG_SUMMARY_UI_INPUT_MOVE,0,&input)); } while (0)
    REJECT(active,0); REJECT(ready,0); REJECT(mode,1); REJECT(monDataType,2); REJECT(moveToLearn,1);
    REJECT(page,3); REJECT(pageMask,1); REJECT(partyCount,0); REJECT(partyCount,7); REJECT(partySlot,1); REJECT(isEgg,1);
    REJECT(moveMask,2); REJECT(moves[0].enabled,0); REJECT(moves[0].moveId,0);
    REJECT(moves[0].cropWidth,0); REJECT(moves[0].cropWidth,257); REJECT(moves[0].cropHeight,0); REJECT(moves[0].cropHeight,193);
    REJECT(moves[0].cropX,138); REJECT(moves[0].cropY,162); REJECT(moves[0].hitX,256); REJECT(moves[0].hitY,192);
    REJECT(moves[0].hitX,68); REJECT(moves[0].hitY,24);
#undef REJECT
    state=valid; state.phase=HG_SUMMARY_UI_MOVE_SELECT;
    CHECK(HGNativeSummaryUIResolveInput(&state,9,5,HG_SUMMARY_UI_INPUT_MOVE,1,&input)
        && !input.keys && input.touchPressed && input.touchX==67 && input.touchY==55);
    CHECK(HGNativeSummaryUIResolveInput(&state,9,5,HG_SUMMARY_UI_INPUT_BACK,0,&input) && input.keys==2);
    for (uint32_t kind=HG_SUMMARY_UI_INPUT_PREVIOUS_PAGE;kind<=HG_SUMMARY_UI_INPUT_RIBBON_DETAILS;kind++) {
        HGNativeSummaryUIInput unchanged={123,456,789,321}; input=unchanged;
        CHECK(!HGNativeSummaryUIResolveInput(&state,9,5,kind,0,&input) && !memcmp(&input,&unchanged,sizeof(input)));
    }
    state.page=2; state.ribbonCount=1;
    CHECK(!HGNativeSummaryUIResolveInput(&state,9,5,HG_SUMMARY_UI_INPUT_RIBBON_DETAILS,0,&input));
    state=valid; state.page=2; state.ribbonCount=1;
    CHECK(HGNativeSummaryUIResolveInput(&state,9,2,HG_SUMMARY_UI_INPUT_RIBBON_DETAILS,0,&input) && input.keys==1 && !input.touchPressed);
    state.ribbonCount=0; CHECK(!HGNativeSummaryUIResolveInput(&state,9,2,HG_SUMMARY_UI_INPUT_RIBBON_DETAILS,0,&input));
    state.ribbonCount=81; CHECK(!HGNativeSummaryUIResolveInput(&state,9,2,HG_SUMMARY_UI_INPUT_RIBBON_DETAILS,0,&input));
    state=valid; state.nextPage=1; CHECK(!HGNativeSummaryUIResolveInput(&state,9,2,HG_SUMMARY_UI_INPUT_NEXT_PAGE,0,&input));
    state.nextPage=3; CHECK(!HGNativeSummaryUIResolveInput(&state,9,2,HG_SUMMARY_UI_INPUT_NEXT_PAGE,0,&input));
    state=valid; state.pageMask=3; CHECK(!HGNativeSummaryUIResolveInput(&state,9,2,HG_SUMMARY_UI_INPUT_NEXT_PAGE,0,&input));
    state=valid; state.nextPageEnabled=0; CHECK(!HGNativeSummaryUIResolveInput(&state,9,2,HG_SUMMARY_UI_INPUT_NEXT_PAGE,0,&input));
    state=valid; state.backEnabled=0; CHECK(!HGNativeSummaryUIResolveInput(&state,9,2,HG_SUMMARY_UI_INPUT_BACK,0,&input));
    state=valid; state.partyCount=3; state.partySlot=1; state.previousMonEnabled=state.nextMonEnabled=1; state.previousMonSlot=0; state.nextMonSlot=2;
    CHECK(HGNativeSummaryUIResolveInput(&state,9,2,HG_SUMMARY_UI_INPUT_PREVIOUS_MON,0,&input) && input.keys==64);
    CHECK(HGNativeSummaryUIResolveInput(&state,9,2,HG_SUMMARY_UI_INPUT_NEXT_MON,0,&input) && input.keys==128);
    state.previousMonSlot=2; CHECK(!HGNativeSummaryUIResolveInput(&state,9,2,HG_SUMMARY_UI_INPUT_PREVIOUS_MON,0,&input));
    state.nextMonSlot=0; CHECK(!HGNativeSummaryUIResolveInput(&state,9,2,HG_SUMMARY_UI_INPUT_NEXT_MON,0,&input));
    state=valid; state.page=0; state.isEgg=1;
    CHECK(HGNativeSummaryUIResolveInput(&state,9,2,HG_SUMMARY_UI_INPUT_BACK,0,&input) && input.keys==2);
    CHECK(!HGNativeSummaryUIResolveInput(&state,9,2,HG_SUMMARY_UI_INPUT_MOVE,0,&input)
        && !HGNativeSummaryUIResolveInput(&state,9,2,HG_SUMMARY_UI_INPUT_NEXT_PAGE,0,&input));
    state=valid; state.mode=2; state.phase=8; state.moveToLearn=MOVE_SWALLOW; state.moveMask|=16;
    valid=state;
#define LEARN_REJECT(member,value) do { state=valid; state.member=(value); HGNativeSummaryUIInput unchanged={123,456,789,321}; input=unchanged; \
    CHECK(!HGNativeSummaryUIResolveInput(&state,9,state.phase,HG_SUMMARY_UI_INPUT_MOVE,4,&input) && !memcmp(&input,&unchanged,sizeof(input))); } while (0)
    LEARN_REJECT(mode,1); LEARN_REJECT(monDataType,2); LEARN_REJECT(moveToLearn,0); LEARN_REJECT(moveToLearn,NUM_MOVES+1);
    LEARN_REJECT(page,0); LEARN_REJECT(pageMask,1); LEARN_REJECT(partyCount,0); LEARN_REJECT(partyCount,7);
    LEARN_REJECT(partySlot,1); LEARN_REJECT(isEgg,1); LEARN_REJECT(phase,5); LEARN_REJECT(moveSlot,5); LEARN_REJECT(moveMask,15);
#undef LEARN_REJECT
    state=valid; state.phase=9; state.moveSlot=4;
    CHECK(!HGNativeSummaryUIResolveInput(&state,9,9,HG_SUMMARY_UI_INPUT_CONFIRM,0,&input));
    return 0;
}

#ifndef HEARTGOLD_ABI32
int main(void) {
    uint32_t line=CheckOriginalSummaryUI();
    if (line) { fprintf(stderr,"Summary UI check line %u\n",line); return 1; }
    puts("PASS: real Summary publisher BG commit barrier, input revisions, original key/touch routes, zero-PP details, bounds and view-only move selection.");
    return 0;
}
#endif
