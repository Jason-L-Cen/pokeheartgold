// SPDX-License-Identifier: GPL-3.0-or-later
#include "native_summary_ui.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define CHECK(value) do { if (!(value)) return __LINE__; } while (0)

uint32_t CheckOriginalSummaryUI(void) {
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
    return 0;
}

#ifndef HEARTGOLD_ABI32
int main(void) {
    uint32_t line=CheckOriginalSummaryUI();
    if (line) { fprintf(stderr,"Summary UI check line %u\n",line); return 1; }
    puts("PASS: Summary input revisions, original key/touch routes, zero-PP details, disabled actions, bounds and view-only move selection.");
    return 0;
}
#endif
