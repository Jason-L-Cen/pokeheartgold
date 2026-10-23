// SPDX-License-Identifier: GPL-3.0-or-later
#define main BagBootstrapMain
#define HGEngineTrap BagBootstrapTrap
#include "engine_check.c"
#undef HGEngineTrap
#undef main
#include "native_bag_ui.h"
#include "constants/pokemon.h"
#include <nitro/os/native_reset.h>
#include <setjmp.h>

#define QA_CHECK(value) do { if (!(value)) { fprintf(stderr,"Bag menu failure line %d: %s\n",__LINE__,#value); _Exit(1); } } while (0)
typedef struct BagChoice { u32 revision,phase,row,kind; } BagChoice;
static jmp_buf resetLanding;
static struct w2c_env *bagEnv;
static BOOL resetExpected,casePassed;
static const char *caseStatus="not_started";
static u32 iconCenters[7][2],stage,stageFrame,pulseFrame,pulseReleased;
static u32 availablePockets,visitedPockets,targetPocket,candidatePocket=HG_BAG_UI_POCKETS;
static u32 selectedItem,selectedQuantity,itemMenuVisited,returnCount;
static u32 starter,partyCount,gotDex,dexEnabled,dexSeen,dexCaught,seenPhases,staleRejected,snapshotStages;
static u32 saveAddress,flagsAddress,partyAddress,dexAddress,bagAddress;
enum { MON_ATTRIBUTES=26 };
static const u32 monAttributes[MON_ATTRIBUTES]={MON_DATA_PERSONALITY,MON_DATA_OT_ID,MON_DATA_SPECIES,MON_DATA_FORM,MON_DATA_IS_EGG,MON_DATA_HELD_ITEM,
    MON_DATA_HP,MON_DATA_MAX_HP,MON_DATA_LEVEL,MON_DATA_STATUS,MON_DATA_MOVE1,MON_DATA_MOVE2,MON_DATA_MOVE3,MON_DATA_MOVE4,
    MON_DATA_MOVE1_PP,MON_DATA_MOVE2_PP,MON_DATA_MOVE3_PP,MON_DATA_MOVE4_PP,MON_DATA_MOVE1_PP_UPS,MON_DATA_MOVE2_PP_UPS,MON_DATA_MOVE3_PP_UPS,MON_DATA_MOVE4_PP_UPS,
    MON_DATA_MOVE1_MAX_PP,MON_DATA_MOVE2_MAX_PP,MON_DATA_MOVE3_MAX_PP,MON_DATA_MOVE4_MAX_PP};
static u32 originalMonData[HG_PARTY_UI_SLOTS][MON_ATTRIBUTES],bagSize;
static u8 *originalBag;
static BagChoice oldInput;

void HGEngineTrap(wasm_rt_trap_t code) {
    if (code==WASM_RT_TRAP_UNREACHABLE && resetExpected && bagEnv && bagEnv->resetRequested) longjmp(resetLanding,1);
    BagBootstrapTrap(code);
}
static const void *guest(Capture *capture,u32 address,size_t size) {
    QA_CHECK(address && address<=capture->env->memory->size && size<=capture->env->memory->size-address);
    return capture->env->memory->data+address;
}
static HGNativeBagUIState readBag(Capture *capture) {
    static u32 lastRevision;
    u32 address=w2c_hgengine_HGNativeBagUIGetState(capture->game);
    HGNativeBagUIState result; memcpy(&result,guest(capture,address,sizeof(result)),sizeof(result));
    QA_CHECK(w2c_hgengine_HGNativeBagUIGetState(capture->game)==address);
    QA_CHECK(!memcmp(&result,guest(capture,address,sizeof(result)),sizeof(result)));
    QA_CHECK(result.pocketCount<=HG_BAG_UI_POCKETS && result.visibleCount<=HG_BAG_UI_ITEMS && result.actionCount<=HG_BAG_UI_ACTIONS);
    QA_CHECK(!(result.pocketMask&~255u) && !(result.itemMask&~((1u<<result.visibleCount)-1)) && !(result.actionMask&~((1u<<result.actionCount)-1)));
    QA_CHECK(result.revision>=lastRevision);
    lastRevision=result.revision;
    return result;
}
static void pulse(Capture *capture,u32 keys,u32 x,u32 y,BOOL touch) {
    QA_CHECK(!pulseFrame && x<256 && y<192 && HGNativeSetInput(capture->env,keys,FALSE,x,y,touch));
    pulseFrame=OS_GetVBlankCount(); pulseReleased=0;
}
static void choose(Capture *capture,HGNativeBagUIState state,u32 row,u32 kind) {
    HGNativeBagUIState current=readBag(capture); u32 x,y;
    QA_CHECK(HGNativeBagUIResolveTouch(&current,state.revision,state.phase,kind,row,&x,&y));
    pulse(capture,0,x,y,TRUE);
}
static void saveBytes(Capture *capture,const char *name,const void *bytes,size_t size) {
    char path[4096]; QA_CHECK(snprintf(path,sizeof(path),"%s/%s",capture->folder,name)<sizeof(path));
    FILE *file=fopen(path,"wb"); QA_CHECK(file && fwrite(bytes,1,size,file)==size && !fclose(file));
}
static void windowEvidence(Capture *capture,u32 phase,const char *role,u32 row,const HGNativePartyUIWindow *window) {
    if (!window->pixelBuffer) return;
    QA_CHECK(window->tileWidth && window->tileWidth<=32 && window->tileHeight && window->tileHeight<=6 && window->clearColor<16 && window->engine<2);
    size_t size=window->tileWidth*window->tileHeight*32;
    char name[128]; QA_CHECK(snprintf(name,sizeof(name),"stage-%02u-%s-%u.4bpp",snapshotStages,role,row)<sizeof(name));
    saveBytes(capture,name,guest(capture,window->pixelBuffer,size),size);
    printf("Bag window evidence: {\"stage\":%u,\"phase\":%u,\"role\":\"%s\",\"row\":%u,\"file\":\"%s\",\"pixelBuffer\":%u,\"tileWidth\":%u,\"tileHeight\":%u,\"clearColor\":%u,\"engine\":%u,\"palette\":[",
        snapshotStages,phase,role,row,name,window->pixelBuffer,window->tileWidth,window->tileHeight,window->clearColor,window->engine);
    for (u32 i=0;i<16;i++) printf("%s%u",i ? "," : "",window->palette[i]);
    puts("]}");
}
static BOOL renderBoth(Capture *capture,BOOL write,BOOL requireSub) {
    if (!capture->buffer) capture->buffer=w2c_hgengine_HGNativeFrameBuffer(capture->game);
    guest(capture,capture->buffer,256*192*4); HGNativeDrainDMA();
    u32 geometry[8]={0};
    if (w2c_hgengine_HGNativeGeometryFrame(capture->game,1,capture->buffer)) memcpy(geometry,guest(capture,capture->buffer,sizeof(geometry)),sizeof(geometry));
    for (u32 engine=0;engine<2;engine++) {
        if (!w2c_hgengine_HGNativeRender2D(capture->game,engine,capture->buffer,256*192*4)) return FALSE;
        const u8 *pixels=guest(capture,capture->buffer,256*192*4); BOOL varied=FALSE;
        for (u32 i=1;i<256*192;i++) if (memcmp(pixels,pixels+i*4,4)) { varied=TRUE; break; }
        if ((!engine || requireSub) && !varied) return FALSE;
        if (!write) continue;
        char path[4096]; QA_CHECK(snprintf(path,sizeof(path),"%s/stage-%02u-engine-%u.ppm",capture->folder,snapshotStages,engine)<sizeof(path));
        FILE *file=fopen(path,"wb"); QA_CHECK(file && fprintf(file,"P6\n256 192\n255\n")>0);
        for (u32 i=0;i<256*192;i++) QA_CHECK(fwrite(pixels+i*4,1,3,file)==3);
        QA_CHECK(!fclose(file));
        printf("Bag frame evidence: {\"stage\":%u,\"engine\":%u,\"frame\":%u,\"nonuniform\":%u,\"vertices\":%u,\"polygons\":%u,\"geometrySerial\":%u}\n",snapshotStages,engine,OS_GetVBlankCount(),varied,geometry[0],geometry[1],geometry[2]);
    }
    return TRUE;
}
static BOOL snapshot(Capture *capture,HGNativeBagUIState state) {
    if (!renderBoth(capture,TRUE,state.active)) return FALSE;
    char name[64]; QA_CHECK(snprintf(name,sizeof(name),"stage-%02u-state.bin",snapshotStages)<sizeof(name));
    saveBytes(capture,name,&state,sizeof(state));
    for (u32 i=0;i<HG_BAG_UI_POCKETS;i++) if (state.pocketMask&(1u<<i)) {
        const HGNativeBagUIPocket *pocket=&state.pockets[i];
        printf("Bag pocket evidence: {\"stage\":%u,\"index\":%u,\"pocketId\":%u,\"enabled\":%u,\"hitX\":%u,\"hitY\":%u,\"crop\":[%u,%u,%u,%u]}\n",snapshotStages,i,pocket->pocketId,pocket->enabled,pocket->hitX,pocket->hitY,pocket->cropX,pocket->cropY,pocket->cropWidth,pocket->cropHeight);
    }
    if (state.phase==HG_BAG_UI_ITEM_MENU) {
        HGNativePartyUIWindow cleared={0};
        for (u32 i=0;i<HG_BAG_UI_ITEMS;i++) QA_CHECK(!memcmp(&state.items[i].text,&cleared,sizeof(cleared)));
    }
    for (u32 i=0;i<state.visibleCount;i++) {
        const HGNativeBagUIItem *item=&state.items[i];
        if (state.itemMask&(1u<<i)) {
            QA_CHECK(item->enabled && item->itemId && item->quantity);
            if (state.phase==HG_BAG_UI_INPUT) QA_CHECK(item->text.pixelBuffer);
        }
        printf("Bag item evidence: {\"stage\":%u,\"row\":%u,\"itemId\":%u,\"quantity\":%u,\"absoluteIndex\":%u,\"enabled\":%u,\"hitX\":%u,\"hitY\":%u,\"crop\":[%u,%u,%u,%u]}\n",snapshotStages,i,item->itemId,item->quantity,item->absoluteIndex,item->enabled,item->hitX,item->hitY,item->cropX,item->cropY,item->cropWidth,item->cropHeight);
        windowEvidence(capture,state.phase,"item",i,&item->text);
    }
    for (u32 i=0;i<state.actionCount;i++) {
        if (state.actionMask&(1u<<i)) QA_CHECK(state.actions[i].enabled && state.actions[i].text.pixelBuffer);
        printf("Bag action evidence: {\"stage\":%u,\"phase\":%u,\"row\":%u,\"actionId\":%u,\"enabled\":%u,\"hitX\":%u,\"hitY\":%u}\n",snapshotStages,state.phase,i,state.actions[i].actionId,state.actions[i].enabled,state.actions[i].hitX,state.actions[i].hitY);
        windowEvidence(capture,state.phase,"action",i,&state.actions[i].text);
    }
    windowEvidence(capture,state.phase,"description",0,&state.description); windowEvidence(capture,state.phase,"tm-info",0,&state.tmInfo);
    windowEvidence(capture,state.phase,"prompt",0,&state.prompt); windowEvidence(capture,state.phase,"message",0,&state.message);
    windowEvidence(capture,state.phase,"page",0,&state.page); windowEvidence(capture,state.phase,"selected-entry",0,&state.selectedEntry); windowEvidence(capture,state.phase,"cancel",0,&state.cancel);
    HGNativeBagUIState after=readBag(capture); QA_CHECK(!memcmp(&state,&after,sizeof(state)));
    printf("Bag snapshot evidence: {\"stage\":%u,\"active\":%u,\"ready\":%u,\"revision\":%u,\"mode\":%u,\"phase\":%u,\"pocketIndex\":%u,\"pocketId\":%u,\"pocketMask\":%u,\"pocketCount\":%u,\"cursorButton\":%u,\"position\":%u,\"scroll\":%u,\"itemCount\":%u,\"pageIndex\":%u,\"pageCount\":%u,\"visibleCount\":%u,\"selectedItemId\":%u,\"selectedQuantity\":%u,\"menuCursor\":%u,\"actionCount\":%u,\"actionMask\":%u}\n",snapshotStages,state.active,state.ready,state.revision,state.mode,state.phase,state.pocketIndex,state.pocketId,state.pocketMask,state.pocketCount,state.cursorButton,state.position,state.scroll,state.itemCount,state.pageIndex,state.pageCount,state.visibleCount,state.selectedItemId,state.selectedQuantity,state.menuCursor,state.actionCount,state.actionMask);
    if (state.active) seenPhases|=1u<<state.phase;
    ++snapshotStages; return TRUE;
}
static void semanticEvidence(Capture *capture,BOOL baseline) {
    u32 count=w2c_hgengine_Party_GetCount(capture->game,partyAddress),current[HG_PARTY_UI_SLOTS][MON_ATTRIBUTES]={{0}};
    QA_CHECK(count<=HG_PARTY_UI_SLOTS);
    for (u32 i=0;i<count;i++) {
        u32 mon=w2c_hgengine_Party_GetMonByIndex(capture->game,partyAddress,i); guest(capture,mon,1);
        for (u32 j=0;j<MON_ATTRIBUTES;j++) current[i][j]=w2c_hgengine_GetMonData(capture->game,mon,monAttributes[j],0);
    }
    u32 currentStarter=w2c_hgengine_CheckGotStarter(capture->game,flagsAddress),currentGotDex=w2c_hgengine_CheckGotPokedex(capture->game,flagsAddress);
    u32 currentEnabled=w2c_hgengine_Pokedex_IsEnabled(capture->game,dexAddress);
    u32 seen=w2c_hgengine_Pokedex_CountNationalDexSeen(capture->game,dexAddress),caught=w2c_hgengine_Pokedex_CountNationalDexOwned(capture->game,dexAddress);
    if (baseline) {
        starter=currentStarter; partyCount=count; gotDex=currentGotDex; dexEnabled=currentEnabled; dexSeen=seen; dexCaught=caught;
        memcpy(originalMonData,current,sizeof(current));
        bagSize=w2c_hgengine_Save_Bag_sizeof(capture->game); QA_CHECK(bagSize);
        const void *bag=guest(capture,bagAddress,bagSize);
        originalBag=malloc(bagSize); QA_CHECK(originalBag); memcpy(originalBag,bag,bagSize);
    } else {
        QA_CHECK(count==partyCount && !memcmp(originalMonData,current,sizeof(current)));
        QA_CHECK(currentStarter==starter && currentGotDex==gotDex && currentEnabled==dexEnabled && seen==dexSeen && caught==dexCaught);
        QA_CHECK(bagSize==w2c_hgengine_Save_Bag_sizeof(capture->game) && !memcmp(originalBag,guest(capture,bagAddress,bagSize),bagSize));
    }
    saveBytes(capture,baseline ? "bag-before.bin" : "bag-after.bin",guest(capture,bagAddress,bagSize),bagSize);
    printf("Bag semantic evidence: {\"step\":\"%s\",\"partyCount\":%u,\"flag0x6A\":%u,\"flag0x6B\":%u,\"dexEnabled\":%u,\"dexSeen\":%u,\"dexCaught\":%u,\"bagBytes\":%u,\"attributes\":[",baseline ? "before" : "after",count,currentStarter,currentGotDex,currentEnabled,seen,caught,bagSize);
    for (u32 j=0;j<MON_ATTRIBUTES;j++) printf("%s%u",j ? "," : "",monAttributes[j]);
    fputs("],\"mons\":[",stdout);
    for (u32 i=0;i<count;i++) { printf("%s[",i ? "," : ""); for (u32 j=0;j<MON_ATTRIBUTES;j++) printf("%s%u",j ? "," : "",current[i][j]); putchar(']'); }
    puts("]}");
}
static void finish(Capture *capture,const char *status,BOOL passed) {
    QA_CHECK(HGNativeSetInput(capture->env,0,FALSE,0,0,FALSE));
    if (saveAddress) semanticEvidence(capture,FALSE);
    caseStatus=status; casePassed=passed; resetExpected=TRUE;
    w2c_env_HGNativeResetControl(capture->env,HG_RESET_REQUEST,0); QA_CHECK(FALSE);
}
static void nextStage(void) { ++stage; stageFrame=OS_GetVBlankCount(); }
static u32 firstItem(const HGNativeBagUIState *state) {
    for (u32 i=0;i<state->visibleCount;i++) if ((state->itemMask&(1u<<i)) && state->items[i].enabled && state->items[i].itemId && state->items[i].quantity) return i;
    return HG_BAG_UI_ITEMS;
}
static void visitNextPocket(Capture *capture,HGNativeBagUIState state) {
    for (u32 i=0;i<HG_BAG_UI_POCKETS;i++) if ((availablePockets&~visitedPockets)&(1u<<i)) {
        targetPocket=i; choose(capture,state,i,HG_BAG_UI_TOUCH_POCKET); stageFrame=OS_GetVBlankCount(); return;
    }
    QA_CHECK(FALSE);
}
static BOOL openBag(Capture *capture) {
    u32 icons=w2c_hgengine_HGNativeFieldMenuIcons(capture->game);
    for (u32 i=0;i<7;i++) if (((icons>>(i*4))&15)==2) { pulse(capture,0,iconCenters[i][0],iconCenters[i][1],TRUE); return TRUE; }
    return FALSE;
}
static void BagPresent(void *argument) {
    Capture *capture=argument; u32 frame=OS_GetVBlankCount();
    if (!stage) {
        present(capture);
        if (w2c_hgengine_HGNativeFieldMap(capture->game)!=33 || !renderBoth(capture,FALSE,FALSE)) return;
        capture->noInput=TRUE; QA_CHECK(HGNativeSetInput(capture->env,0,FALSE,0,0,FALSE));
        saveAddress=w2c_hgengine_SaveData_Get(capture->game); guest(capture,saveAddress,1);
        flagsAddress=w2c_hgengine_Save_VarsFlags_Get(capture->game,saveAddress); partyAddress=w2c_hgengine_SaveArray_Party_Get(capture->game,saveAddress);
        dexAddress=w2c_hgengine_Save_Pokedex_Get(capture->game,saveAddress); bagAddress=w2c_hgengine_Save_Bag_Get(capture->game,saveAddress);
        guest(capture,flagsAddress,1); guest(capture,partyAddress,1); guest(capture,dexAddress,1); guest(capture,bagAddress,1);
        semanticEvidence(capture,TRUE);
        printf("Bag prerequisites: {\"flag0x6A\":%u,\"partyCount\":%u,\"flag0x6B\":%u,\"dexEnabled\":%u}\n",starter,partyCount,gotDex,dexEnabled);
        nextStage(); return;
    }
    QA_CHECK(HGNativeSoundRealtime(capture->env,TRUE));
    if (frame-stageFrame>3600) finish(capture,stage==1 ? "scene_prerequisite_blocked" : "scene_or_input_handshake_timeout",FALSE);
    if (pulseFrame) {
        if (!pulseReleased && frame-pulseFrame>=8) { QA_CHECK(HGNativeSetInput(capture->env,0,FALSE,0,0,FALSE)); pulseReleased=frame; }
        if (!pulseReleased || frame-pulseReleased<2) return;
        pulseFrame=0;
    }
    HGNativeBagUIState state=readBag(capture); u32 ignoredX,ignoredY;
    if (stage==1 || stage==8) { if (w2c_hgengine_HGNativeFieldMap(capture->game)==33 && openBag(capture)) nextStage(); return; }
    if (stage==7 || stage==10) {
        if (state.active || w2c_hgengine_HGNativeFieldMap(capture->game)!=33) return;
        HGNativeBagUIState cleared={.revision=state.revision}; QA_CHECK(!memcmp(&state,&cleared,sizeof(state)));
        if (!renderBoth(capture,FALSE,FALSE)) return; // Original field ownership resumes before its fade-in completes.
        if (!snapshot(capture,state)) finish(capture,"renderer_unsupported",FALSE);
        ++returnCount;
        if (stage==10) {
            QA_CHECK(visitedPockets==availablePockets && returnCount==2 && staleRejected>=1);
            if (!itemMenuVisited) finish(capture,"scene_prerequisite_blocked",FALSE);
            QA_CHECK(staleRejected>=3); finish(capture,"passed_original_bag_menu",TRUE);
        }
        nextStage(); return;
    }
    if (!state.active || !state.ready) return;
    QA_CHECK(!state.mode);
    if (stage==2 && state.phase==HG_BAG_UI_INPUT) {
        availablePockets=state.pocketMask; QA_CHECK(availablePockets==255 && state.pocketCount==HG_BAG_UI_POCKETS);
        for (u32 i=0;i<HG_BAG_UI_POCKETS;i++) QA_CHECK(state.pockets[i].pocketId==i);
        oldInput=(BagChoice){state.revision,state.phase,0,HG_BAG_UI_TOUCH_CANCEL};
        if (!snapshot(capture,state)) finish(capture,"renderer_unsupported",FALSE);
        visitNextPocket(capture,state); nextStage();
    } else if (stage==3 && state.phase==HG_BAG_UI_INPUT && state.pocketIndex==targetPocket) {
        QA_CHECK(state.pocketMask==availablePockets && state.pocketId==state.pockets[targetPocket].pocketId);
        if (!snapshot(capture,state)) finish(capture,"renderer_unsupported",FALSE);
        if (state.itemCount) {
            QA_CHECK(firstItem(&state)<HG_BAG_UI_ITEMS);
            if (candidatePocket==HG_BAG_UI_POCKETS) candidatePocket=targetPocket;
        }
        visitedPockets|=1u<<targetPocket;
        if (visitedPockets!=availablePockets) { visitNextPocket(capture,state); return; }
        if (candidatePocket==HG_BAG_UI_POCKETS) {
            choose(capture,state,0,HG_BAG_UI_TOUCH_CANCEL); stage=7; stageFrame=frame;
        } else {
            targetPocket=candidatePocket; choose(capture,state,targetPocket,HG_BAG_UI_TOUCH_POCKET); nextStage();
        }
    } else if (stage==4 && state.phase==HG_BAG_UI_INPUT && state.pocketIndex==targetPocket) {
        u32 row=firstItem(&state); QA_CHECK(row<HG_BAG_UI_ITEMS);
        selectedItem=state.items[row].itemId; selectedQuantity=state.items[row].quantity;
        if (!snapshot(capture,state)) finish(capture,"renderer_unsupported",FALSE);
        choose(capture,state,row,HG_BAG_UI_TOUCH_ITEM); nextStage();
    } else if (stage==5 && state.phase==HG_BAG_UI_ITEM_MENU) {
        QA_CHECK(state.selectedItemId==selectedItem && state.selectedQuantity==selectedQuantity && state.actionCount==HG_BAG_UI_ACTIONS && state.selectedEntry.pixelBuffer);
        QA_CHECK(!HGNativeBagUIResolveTouch(&state,oldInput.revision,oldInput.phase,oldInput.kind,oldInput.row,&ignoredX,&ignoredY)); ++staleRejected;
        QA_CHECK(!HGNativeBagUIResolveTouch(&state,state.revision,state.phase,HG_BAG_UI_TOUCH_ACTION,HG_BAG_UI_ACTIONS,&ignoredX,&ignoredY)); ++staleRejected;
        if (!snapshot(capture,state)) finish(capture,"renderer_unsupported",FALSE);
        choose(capture,state,0,HG_BAG_UI_TOUCH_CANCEL); nextStage();
    } else if (stage==6 && state.phase==HG_BAG_UI_INPUT) {
        itemMenuVisited=1;
        if (!snapshot(capture,state)) finish(capture,"renderer_unsupported",FALSE);
        choose(capture,state,0,HG_BAG_UI_TOUCH_CANCEL); nextStage();
    } else if (stage==9 && state.phase==HG_BAG_UI_INPUT) {
        QA_CHECK(state.pocketMask==availablePockets && state.pocketCount==HG_BAG_UI_POCKETS);
        QA_CHECK(state.revision>oldInput.revision && !HGNativeBagUIResolveTouch(&state,oldInput.revision,oldInput.phase,oldInput.kind,oldInput.row,&ignoredX,&ignoredY)); ++staleRejected;
        if (!snapshot(capture,state)) finish(capture,"renderer_unsupported",FALSE);
        choose(capture,state,0,HG_BAG_UI_TOUCH_CANCEL); nextStage();
    }
}

int main(int argc,char **argv) {
    QA_CHECK(!setvbuf(stdout,NULL,_IOLBF,0));
    if (argc!=19 || !HGNativeFSSetRoot(argv[1]) || !HGNativeBackupSetPath(argv[3])) return 2;
    for (u32 i=0;i<14;i++) { char *end; unsigned long value=strtoul(argv[5+i],&end,10); if (!*argv[5+i] || *end || value>=(i%2 ? 192u : 256u)) return 2; iconCenters[i/2][i%2]=(u32)value; }
    u8 header[0x160]; FILE *input=fopen(argv[2],"rb"); QA_CHECK(input);
    QA_CHECK(fread(header,1,sizeof(header),input)==sizeof(header) && fgetc(input)==EOF && !ferror(input)); QA_CHECK(!fclose(input));
    wasm_rt_init(); w2c_hgengine *game=calloc(1,sizeof(*game)); struct w2c_env *env=calloc(1,sizeof(*env));
    struct w2c_wasi__snapshot__preview1 *wasi=calloc(1,sizeof(*wasi)); Capture *capture=calloc(1,sizeof(*capture));
    QA_CHECK(game && env && wasi && capture); for (u32 i=0;i<3;i++) { wasi->fds[i]=dup(i); QA_CHECK(wasi->fds[i]>=0); } bagEnv=env;
    if (!setjmp(resetLanding)) {
        wasm_rt_trap_t trap=wasm_rt_impl_try(); QA_CHECK(!trap);
        wasm2c_hgengine_instantiate(game,env,wasi); env->memory=wasi->memory=w2c_hgengine_memory(game); QA_CHECK(HGNativeSoundRealtime(env,TRUE));
        env->irqTable=&game->w2c_T0; env->stackPointer=&game->w2c_0x5F_stack_pointer; env->rootStackHigh=*env->stackPointer; *env->stackPointer-=16;
        env->irqType0=wasm2c_hgengine_get_func_type(0,0); env->irqType1=wasm2c_hgengine_get_func_type(1,0,WASM_RT_I32);
        env->rtcType=wasm2c_hgengine_get_func_type(2,0,WASM_RT_I32,WASM_RT_I32);
        env->irqGuardType=wasm2c_hgengine_get_func_type(4,0,WASM_RT_I32,WASM_RT_I32,WASM_RT_I32,WASM_RT_I32);
        u32 work=w2c_hgengine_HGNativeSystemWorkAddress(game); guest(&(Capture){.env=env},work,0x360);
        memcpy(env->memory->data+work+0x200,header,sizeof(header)); w2c_hgengine_0x5Finitialize(game);
        u32 status=w2c_hgengine_HGNativeRegisterAddress(game,0x04000004,2); QA_CHECK(!(status&1)); guest(&(Capture){.env=env},status,2);
        QA_CHECK(HGNativeStartDisplayClock(env,(const vu16 *)(env->memory->data+status)));
        *capture=(Capture){.game=game,.env=env,.folder=argv[4],.resume=TRUE};
        QA_CHECK(HGNativeSetDisplayObserver(env,BagPresent,capture)); w2c_hgengine_NitroMain(game); QA_CHECK(FALSE);
    }
    QA_CHECK(resetExpected && env->resetRequested); HGNativeFinishReset(env);
    QA_CHECK(env->resetFinished && !env->pendingDMA && !env->dmaAffineWrites && !env->pendingRTC && !env->pendingMIC && !env->pendingCARD && !env->pendingPOWER && !env->pendingINPUT && !env->irqTable && !env->threads && !env->rasterFunction);
    HGNativeBagUIState final=readBag(capture);
    if (casePassed) QA_CHECK(!final.active && !final.ready && !final.actionCount && itemMenuVisited && seenPhases==((1u<<1)|(1u<<4)));
    printf("Bag menu evidence: {\"status\":\"%s\",\"flag0x6A\":%u,\"partyCount\":%u,\"flag0x6B\":%u,\"dexEnabled\":%u,\"availablePocketMask\":%u,\"visitedPocketMask\":%u,\"candidatePocket\":%u,\"selectedItemId\":%u,\"selectedQuantity\":%u,\"itemMenuVisited\":%u,\"returns\":%u,\"seenPhases\":%u,\"staleRejected\":%u,\"snapshotStages\":%u,\"activeAfterReturn\":%u,\"resetFinished\":true,\"pendingDMA\":0,\"partyBagFlagsDexUnchanged\":true}\n",caseStatus,starter,partyCount,gotDex,dexEnabled,availablePockets,visitedPockets,candidatePocket,selectedItem,selectedQuantity,itemMenuVisited,returnCount,seenPhases,staleRejected,snapshotStages,final.active);
    wasm2c_hgengine_free(game); for (u32 i=0;i<3;i++) close(wasi->fds[i]); free(originalBag); free(game); free(env); free(wasi); free(capture); bagEnv=NULL; wasm_rt_free();
    if (casePassed) puts("PASS: original Bag pockets and item-menu Cancel through real touch input, unchanged save semantics, stale owner rejection, original windows/frames and reset.");
    return casePassed ? 0 : 3;
}
