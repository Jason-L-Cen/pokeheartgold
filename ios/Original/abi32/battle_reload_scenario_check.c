// SPDX-License-Identifier: GPL-3.0-or-later
#include <stdint.h>
enum { RELOAD_ROUNDS=2, RELOAD_SLOTS=6, RELOAD_ATTRIBUTES=26 };
typedef struct BattleReloadQAState {
    uint32_t requested,started,returned,fieldReady,error,loads,unloads,launches;
    uint32_t loadBefore[2],loadAfter[2],exitRevision[2],loadFrame[2],unloadFrame[2],launchFrame[2],returnFrame[2];
    uint32_t outcomes[2],battleResult[2],battleType[2],battleSpecial[2];
    uint32_t playerHp[2],playerStatus[2],playerSpeed[2],enemySpecies[2],enemyLevel[2],enemySpeed[2],enemyAbility[2];
    uint32_t partyCount[3],money[3],starter[3],gotDex[3],dexEnabled[3],dexSeen[3],dexCaught[3],wildEncounters[3],semanticsUnchanged[3];
    uint32_t mons[3][RELOAD_SLOTS][RELOAD_ATTRIBUTES];
} BattleReloadQAState;

#ifdef HG_BATTLE_RELOAD_QA_FIELD
// Included only in the private field_system.c copy; production objects stay unchanged.
#include "field_system.h"
#include "encounter.h"
#include "battle/battle_setup.h"
#include "field/draw_map_name.h"
#include "launch_application.h"
#include "native_battle_ui.h"
#include "poke_overlay.h"
#include "pokemon.h"
#include "screen_fade.h"
#include "sys_flags.h"
#include "system.h"
#include "constants/abilities.h"
#include "constants/pokemon.h"
#include "constants/species.h"
#include "constants/vars.h"

static BattleReloadQAState sReloadQA;
static Bag sReloadBag;
static const u32 sReloadAttributes[RELOAD_ATTRIBUTES]={
    MON_DATA_PERSONALITY,MON_DATA_OT_ID,MON_DATA_SPECIES,MON_DATA_FORM,MON_DATA_IS_EGG,MON_DATA_HELD_ITEM,
    MON_DATA_HP,MON_DATA_MAX_HP,MON_DATA_LEVEL,MON_DATA_STATUS,MON_DATA_MOVE1,MON_DATA_MOVE2,MON_DATA_MOVE3,MON_DATA_MOVE4,
    MON_DATA_MOVE1_PP,MON_DATA_MOVE2_PP,MON_DATA_MOVE3_PP,MON_DATA_MOVE4_PP,
    MON_DATA_MOVE1_PP_UPS,MON_DATA_MOVE2_PP_UPS,MON_DATA_MOVE3_PP_UPS,MON_DATA_MOVE4_PP_UPS,
    MON_DATA_MOVE1_MAX_PP,MON_DATA_MOVE2_MAX_PP,MON_DATA_MOVE3_MAX_PP,MON_DATA_MOVE4_MAX_PP};
#define RELOAD_REQUIRE(value) do { if (!(value) && !sReloadQA.error) sReloadQA.error=__LINE__; } while (0)

static void BattleReloadQASemantics(FieldSystem *field,u32 step) {
    BattleReloadQAState *qa=&sReloadQA;
    SaveData *save=field->saveData;
    Party *party=SaveArray_Party_Get(save);
    Pokedex *dex=Save_Pokedex_Get(save);
    SaveVarsFlags *flags=Save_VarsFlags_Get(save);
    qa->partyCount[step]=Party_GetCount(party);
    RELOAD_REQUIRE(qa->partyCount[step] && qa->partyCount[step]<=RELOAD_SLOTS);
    for (u32 i=0;i<qa->partyCount[step] && i<RELOAD_SLOTS;i++) {
        Pokemon *mon=Party_GetMonByIndex(party,i);
        for (u32 j=0;j<RELOAD_ATTRIBUTES;j++) qa->mons[step][i][j]=GetMonData(mon,sReloadAttributes[j],NULL);
    }
    qa->money[step]=PlayerProfile_GetMoney(Save_PlayerData_GetProfile(save));
    qa->starter[step]=CheckGotStarter(flags); qa->gotDex[step]=CheckGotPokedex(flags);
    qa->dexEnabled[step]=Pokedex_IsEnabled(dex); qa->dexSeen[step]=Pokedex_CountNationalDexSeen(dex);
    qa->dexCaught[step]=Pokedex_CountNationalDexOwned(dex);
    qa->wildEncounters[step]=GameStats_GetCapped(Save_GameStats_Get(save),GAME_STAT_WILD_ENCOUNTERS);
    if (!step) {
        Save_Bag_Copy(Save_Bag_Get(save),&sReloadBag);
        qa->semanticsUnchanged[step]=TRUE;
        RELOAD_REQUIRE(qa->starter[0] && qa->partyCount[0]==1);
    } else {
        qa->semanticsUnchanged[step]=qa->partyCount[step]==qa->partyCount[0] &&
            !memcmp(qa->mons[0],qa->mons[step],sizeof(qa->mons[0])) &&
            !memcmp(&sReloadBag,Save_Bag_Get(save),sizeof(sReloadBag)) &&
            qa->money[step]==qa->money[0] && qa->starter[step]==qa->starter[0] &&
            qa->gotDex[step]==qa->gotDex[0] && qa->dexEnabled[step]==qa->dexEnabled[0] &&
            qa->dexCaught[step]==qa->dexCaught[0];
        RELOAD_REQUIRE(qa->semanticsUnchanged[step] && qa->wildEncounters[step]==qa->wildEncounters[0]+step);
    }
}

const BattleReloadQAState *HGNativeBattleReloadQAGetState(void) { return &sReloadQA; }
u32 HGNativeBattleReloadQARequest(void) {
    if (sReloadQA.requested>=RELOAD_ROUNDS || sReloadQA.returned!=sReloadQA.requested || !sReloadQA.fieldReady) return FALSE;
    return ++sReloadQA.requested;
}
BOOL __real_HandleLoadOverlay(FSOverlayID,PMOverlayLoadType);
BOOL __wrap_HandleLoadOverlay(FSOverlayID id,PMOverlayLoadType type) {
    u32 index=sReloadQA.loads;
    if (id==12 && sReloadQA.started) {
        RELOAD_REQUIRE(index<RELOAD_ROUNDS && sReloadQA.loads==sReloadQA.unloads);
        if (index<RELOAD_ROUNDS) sReloadQA.loadBefore[index]=HGNativeBattleUIGetState()->revision;
    }
    BOOL result=__real_HandleLoadOverlay(id,type);
    if (id==12 && sReloadQA.started) {
        RELOAD_REQUIRE(result);
        if (result) {
            if (index<RELOAD_ROUNDS) { sReloadQA.loadAfter[index]=HGNativeBattleUIGetState()->revision; sReloadQA.loadFrame[index]=OS_GetVBlankCount(); }
            ++sReloadQA.loads;
        }
    }
    return result;
}
void __real_UnloadOverlayByID(FSOverlayID);
void __wrap_UnloadOverlayByID(FSOverlayID id) {
    if (id==12 && sReloadQA.started) {
        u32 index=sReloadQA.unloads;
        const HGNativeBattleUIState *ui=HGNativeBattleUIGetState();
        // Capture after the real BattleInput_Free invalidates its owner, before OVY12 is retired.
        RELOAD_REQUIRE(index<RELOAD_ROUNDS && sReloadQA.loads==index+1 && !ui->active && !ui->ready);
        if (index<RELOAD_ROUNDS) { sReloadQA.exitRevision[index]=ui->revision; sReloadQA.unloadFrame[index]=OS_GetVBlankCount(); }
        ++sReloadQA.unloads;
    }
    __real_UnloadOverlayByID(id);
}
void __real_Battle_LaunchApp(FieldSystem *,BattleSetup *);
void __wrap_Battle_LaunchApp(FieldSystem *field,BattleSetup *setup) {
    u32 index=sReloadQA.launches++;
    RELOAD_REQUIRE(index<RELOAD_ROUNDS && sReloadQA.started==index+1);
    if (index<RELOAD_ROUNDS) {
        RELOAD_REQUIRE(Party_GetCount(setup->party[BATTLER_PLAYER])==1 && Party_GetCount(setup->party[BATTLER_ENEMY])==1);
        Pokemon *player=Party_GetMonByIndex(setup->party[BATTLER_PLAYER],0),*enemy=Party_GetMonByIndex(setup->party[BATTLER_ENEMY],0);
        sReloadQA.battleType[index]=setup->battleType; sReloadQA.battleSpecial[index]=setup->battleSpecial;
        sReloadQA.playerHp[index]=GetMonData(player,MON_DATA_HP,NULL); sReloadQA.playerStatus[index]=GetMonData(player,MON_DATA_STATUS,NULL);
        sReloadQA.playerSpeed[index]=GetMonData(player,MON_DATA_SPEED,NULL); sReloadQA.enemySpeed[index]=GetMonData(enemy,MON_DATA_SPEED,NULL);
        sReloadQA.enemySpecies[index]=GetMonData(enemy,MON_DATA_SPECIES,NULL); sReloadQA.enemyLevel[index]=GetMonData(enemy,MON_DATA_LEVEL,NULL);
        sReloadQA.enemyAbility[index]=GetMonData(enemy,MON_DATA_ABILITY,NULL); sReloadQA.launchFrame[index]=OS_GetVBlankCount();
        // With no prior turn, this original speed branch guarantees Run without changing rolls or Pokémon.
        RELOAD_REQUIRE(!setup->battleType && !setup->battleSpecial && sReloadQA.playerHp[index] && !sReloadQA.playerStatus[index] &&
            sReloadQA.playerSpeed[index]>=sReloadQA.enemySpeed[index] && sReloadQA.enemySpecies[index]==SPECIES_CATERPIE &&
            sReloadQA.enemyLevel[index]==1 && sReloadQA.enemyAbility[index]==ABILITY_SHIELD_DUST);
    }
    __real_Battle_LaunchApp(field,setup);
}
static BOOL BattleReloadQATask(TaskManager *task) {
    u32 *state=TaskManager_GetStatePtr(task);
    if (!*state) {
        u32 round=sReloadQA.started++;
        RELOAD_REQUIRE(round<RELOAD_ROUNDS);
        if (round>=RELOAD_ROUNDS) return TRUE;
        *state=1;
        SetupAndStartWildBattle(task,SPECIES_CATERPIE,1,&sReloadQA.outcomes[round],FALSE,FALSE);
        return FALSE;
    }
    u32 round=sReloadQA.returned++;
    RELOAD_REQUIRE(round<RELOAD_ROUNDS);
    if (round<RELOAD_ROUNDS) {
        FieldSystem *field=TaskManager_GetFieldSystem(task);
        sReloadQA.battleResult[round]=FieldSystem_VarGet(field,VAR_BATTLE_RESULT);
        sReloadQA.returnFrame[round]=OS_GetVBlankCount();
        RELOAD_REQUIRE(sReloadQA.outcomes[round]==BATTLE_OUTCOME_PLAYER_FLED && sReloadQA.battleResult[round]==BATTLE_OUTCOME_PLAYER_FLED);
        BattleReloadQASemantics(field,round+1);
    }
    return TRUE;
}
static void BattleReloadQAFieldTick(FieldSystem *field) {
    sReloadQA.fieldReady=!field->taskman && FieldSystem_IsFieldMapRunning(field) &&
        !FieldSystem_IsChildAppRunning(field) && IsPaletteFadeFinished() && field->location->mapId==33;
    if (!sReloadQA.fieldReady || sReloadQA.requested<=sReloadQA.started) return;
    if (!sReloadQA.started) BattleReloadQASemantics(field,0);
    FieldDrawMapName_Reset(field->mapSystems->drawMapNameInfo);
    FieldSystem_CreateTask(field,BattleReloadQATask,NULL);
}

#else
#define main ReloadBootstrapMain
#define HGEngineTrap ReloadBootstrapTrap
#include "engine_check.c"
#undef HGEngineTrap
#undef main
#include "native_battle_ui.h"
#include <nitro/os/native_reset.h>
#include <setjmp.h>

#define QA_CHECK(value) do { if (!(value)) { fprintf(stderr,"Battle reload failure line %d: %s\n",__LINE__,#value); _Exit(1); } } while (0)
typedef struct BattleUIHead { uint32_t active,revision,mode,ready,enabledMask,cancelRun; } BattleUIHead;
_Static_assert(sizeof(BattleUIHead)==offsetof(HGNativeBattleUIState,move),"Battle menu prefix changed");
static jmp_buf resetLanding;
static struct w2c_env *reloadEnv;
static BOOL resetExpected,passed;
static u32 stage,stageFrame,readyRevision[2],exitRevision[2],exitStaleAccepted,commandStaleAccepted,duplicateRejected,revisionRollback;
static BattleReloadQAState finalQA;
void HGEngineTrap(wasm_rt_trap_t code) {
    if (code==WASM_RT_TRAP_UNREACHABLE && resetExpected && reloadEnv && reloadEnv->resetRequested) longjmp(resetLanding,1);
    ReloadBootstrapTrap(code);
}
static const void *guest(Capture *capture,u32 address,size_t size) {
    QA_CHECK(address && address<=capture->env->memory->size && size<=capture->env->memory->size-address);
    return capture->env->memory->data+address;
}
static BattleReloadQAState readQA(Capture *capture) {
    u32 address=w2c_hgengine_HGNativeBattleReloadQAGetState(capture->game);
    BattleReloadQAState qa; memcpy(&qa,guest(capture,address,sizeof(qa)),sizeof(qa));
    if (qa.error) fprintf(stderr,"Battle reload guest assertion line %u\n",qa.error);
    QA_CHECK(!qa.error); return qa;
}
static BattleUIHead readUI(Capture *capture) {
    u32 address=w2c_hgengine_HGNativeBattleUIGetState(capture->game);
    BattleUIHead ui; memcpy(&ui,guest(capture,address,sizeof(ui)),sizeof(ui));
    QA_CHECK(w2c_hgengine_HGNativeBattleUIGetState(capture->game)==address && !memcmp(&ui,guest(capture,address,sizeof(ui)),sizeof(ui)));
    return ui;
}
static BOOL snapshot(Capture *capture,const char *role,u32 round) {
    if (!capture->buffer) capture->buffer=w2c_hgengine_HGNativeFrameBuffer(capture->game);
    guest(capture,capture->buffer,256*192*4); HGNativeDrainDMA();
    for (u32 engine=0;engine<2;engine++) {
        if (!w2c_hgengine_HGNativeRender2D(capture->game,engine,capture->buffer,256*192*4)) return FALSE;
        const u8 *pixels=guest(capture,capture->buffer,256*192*4); BOOL varied=FALSE;
        for (u32 i=1;i<256*192;i++) if (memcmp(pixels,pixels+i*4,4)) { varied=TRUE; break; }
        if (!varied) return FALSE;
        char path[4096]; QA_CHECK(snprintf(path,sizeof(path),"%s/round-%u-%s-engine-%u.ppm",capture->folder,round,role,engine)<sizeof(path));
        FILE *file=fopen(path,"wb"); QA_CHECK(file && fprintf(file,"P6\n256 192\n255\n")>0);
        for (u32 i=0;i<256*192;i++) QA_CHECK(fwrite(pixels+i*4,1,3,file)==3);
        QA_CHECK(!fclose(file));
    }
    printf("Battle reload frame evidence: {\"round\":%u,\"role\":\"%s\",\"frame\":%u,\"engines\":2}\n",round,role,OS_GetVBlankCount());
    return TRUE;
}
static void nextStage(void) { ++stage; stageFrame=OS_GetVBlankCount(); }
static void ReloadPresent(void *argument) {
    Capture *capture=argument;
    if (!stage) {
        present(capture);
        BattleReloadQAState qa=readQA(capture);
        if (!qa.fieldReady || !snapshot(capture,"field-before",0)) return;
        capture->noInput=TRUE; QA_CHECK(HGNativeSetInput(capture->env,0,FALSE,0,0,FALSE));
        QA_CHECK(w2c_hgengine_HGNativeBattleReloadQARequest(capture->game)==1); nextStage(); return;
    }
    QA_CHECK(HGNativeSoundRealtime(capture->env,TRUE)); // Discard sink: this command never opens an audio device.
    QA_CHECK(OS_GetVBlankCount()-stageFrame<7200);
    BattleReloadQAState qa=readQA(capture); BattleUIHead ui=readUI(capture);
    if ((stage==1 || stage==3) && ui.active && ui.ready && ui.mode==HG_BATTLE_UI_COMMAND) {
        u32 round=stage==1 ? 0 : 1;
        QA_CHECK(qa.started==round+1 && qa.launches==round+1 && qa.loads==round+1 && qa.unloads==round && (ui.enabledMask&8));
        if (!snapshot(capture,"command",round)) return;
        readyRevision[round]=ui.revision;
        BOOL selected=FALSE;
        if (round) {
            revisionRollback=ui.revision<=exitRevision[0];
            exitStaleAccepted=w2c_hgengine_HGNativeBattleUIChoose(capture->game,exitRevision[0],3);
            selected=exitStaleAccepted;
            if (!selected) { commandStaleAccepted=w2c_hgengine_HGNativeBattleUIChoose(capture->game,readyRevision[0],3); selected=commandStaleAccepted; }
        } else {
            QA_CHECK(!w2c_hgengine_HGNativeBattleUIChoose(capture->game,ui.revision,6));
            QA_CHECK(!w2c_hgengine_HGNativeBattleUIChoose(capture->game,ui.revision-1,3));
        }
        if (!selected) QA_CHECK(w2c_hgengine_HGNativeBattleUIChoose(capture->game,ui.revision,3));
        QA_CHECK(!w2c_hgengine_HGNativeBattleUIChoose(capture->game,ui.revision,3)); ++duplicateRejected;
        printf("Battle reload command evidence: {\"round\":%u,\"revision\":%u,\"priorReadyRevision\":%u,\"priorExitRevision\":%u,\"exitStaleAccepted\":%u,\"commandStaleAccepted\":%u,\"revisionRollback\":%u,\"index\":3}\n",
            round,ui.revision,round ? readyRevision[0] : 0,round ? exitRevision[0] : 0,exitStaleAccepted,commandStaleAccepted,revisionRollback);
        nextStage(); return;
    }
    if ((stage==2 || stage==4) && qa.returned==(stage==2 ? 1u : 2u) && qa.fieldReady && !ui.active && !ui.ready) {
        u32 round=stage==2 ? 0 : 1;
        QA_CHECK(qa.loads==round+1 && qa.unloads==round+1 && qa.launches==round+1 && qa.outcomes[round]==5 && qa.battleResult[round]==5 && qa.semanticsUnchanged[round+1]);
        QA_CHECK(qa.launchFrame[round]<=qa.loadFrame[round] && qa.loadFrame[round]<qa.unloadFrame[round] && qa.unloadFrame[round]<=qa.returnFrame[round]);
        exitRevision[round]=qa.exitRevision[round];
        QA_CHECK(exitRevision[round]>readyRevision[round]);
        if (round) QA_CHECK(qa.returnFrame[0]<=qa.launchFrame[1] && qa.unloadFrame[0]<qa.loadFrame[1]);
        if (!snapshot(capture,"field-return",round)) return;
        if (!round) {
            QA_CHECK(w2c_hgengine_HGNativeBattleReloadQARequest(capture->game)==2); nextStage(); return;
        }
        QA_CHECK(duplicateRejected==2); finalQA=qa;
        passed=!revisionRollback && !exitStaleAccepted && !commandStaleAccepted;
        resetExpected=TRUE; w2c_env_HGNativeResetControl(capture->env,HG_RESET_REQUEST,0); QA_CHECK(FALSE);
    }
}
int main(int argc,char **argv) {
    QA_CHECK(!setvbuf(stdout,NULL,_IOLBF,0));
    if (argc!=5 || !HGNativeFSSetRoot(argv[1]) || !HGNativeBackupSetPath(argv[3])) return 2;
    u8 header[0x160]; FILE *input=fopen(argv[2],"rb"); QA_CHECK(input);
    QA_CHECK(fread(header,1,sizeof(header),input)==sizeof(header) && fgetc(input)==EOF && !ferror(input)); QA_CHECK(!fclose(input));
    wasm_rt_init(); w2c_hgengine *game=calloc(1,sizeof(*game)); struct w2c_env *env=calloc(1,sizeof(*env));
    struct w2c_wasi__snapshot__preview1 *wasi=calloc(1,sizeof(*wasi)); Capture *capture=calloc(1,sizeof(*capture));
    QA_CHECK(game && env && wasi && capture); for (u32 i=0;i<3;i++) { wasi->fds[i]=dup(i); QA_CHECK(wasi->fds[i]>=0); } reloadEnv=env;
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
        QA_CHECK(HGNativeSetDisplayObserver(env,ReloadPresent,capture)); w2c_hgengine_NitroMain(game); QA_CHECK(FALSE);
    }
    QA_CHECK(resetExpected && env->resetRequested); HGNativeFinishReset(env);
    QA_CHECK(env->resetFinished && !env->pendingDMA && !env->dmaAffineWrites && !env->pendingRTC && !env->pendingMIC && !env->pendingCARD && !env->pendingPOWER && !env->pendingINPUT && !env->irqTable && !env->threads && !env->rasterFunction);
    const char *status=passed ? "passed_original_two_wild_reload" : "expected_negative_revision_reuse";
    printf("Battle reload evidence: {\"status\":\"%s\",\"loads\":%u,\"unloads\":%u,\"launches\":%u,\"returns\":%u,\"readyRevision\":[%u,%u],\"exitRevision\":[%u,%u],\"loadBefore\":[%u,%u],\"loadAfter\":[%u,%u],\"outcomes\":[%u,%u],\"battleResult\":[%u,%u],\"wildEncounters\":[%u,%u,%u],\"dexSeen\":[%u,%u,%u],\"revisionRollback\":%u,\"exitStaleAccepted\":%u,\"commandStaleAccepted\":%u,\"duplicateRejected\":%u,\"partyBagMoneyUnlockUnchanged\":true,\"resetFinished\":true,\"pendingDMA\":0}\n",
        status,finalQA.loads,finalQA.unloads,finalQA.launches,finalQA.returned,readyRevision[0],readyRevision[1],exitRevision[0],exitRevision[1],
        finalQA.loadBefore[0],finalQA.loadBefore[1],finalQA.loadAfter[0],finalQA.loadAfter[1],finalQA.outcomes[0],finalQA.outcomes[1],finalQA.battleResult[0],finalQA.battleResult[1],
        finalQA.wildEncounters[0],finalQA.wildEncounters[1],finalQA.wildEncounters[2],finalQA.dexSeen[0],finalQA.dexSeen[1],finalQA.dexSeen[2],revisionRollback,exitStaleAccepted,commandStaleAccepted,duplicateRejected);
    for (u32 round=0;round<RELOAD_ROUNDS;round++) printf("Battle reload prerequisite evidence: {\"round\":%u,\"hp\":%u,\"status\":%u,\"playerSpeed\":%u,\"enemySpeed\":%u,\"enemySpecies\":%u,\"enemyLevel\":%u,\"enemyAbility\":%u,\"battleType\":%u,\"battleSpecial\":%u,\"launchFrame\":%u,\"loadFrame\":%u,\"unloadFrame\":%u,\"returnFrame\":%u}\n",
        round,finalQA.playerHp[round],finalQA.playerStatus[round],finalQA.playerSpeed[round],finalQA.enemySpeed[round],finalQA.enemySpecies[round],finalQA.enemyLevel[round],finalQA.enemyAbility[round],finalQA.battleType[round],finalQA.battleSpecial[round],finalQA.launchFrame[round],finalQA.loadFrame[round],finalQA.unloadFrame[round],finalQA.returnFrame[round]);
    char path[4096]; QA_CHECK(snprintf(path,sizeof(path),"%s/semantic-state.bin",capture->folder)<sizeof(path));
    FILE *file=fopen(path,"wb"); QA_CHECK(file && fwrite(&finalQA,1,sizeof(finalQA),file)==sizeof(finalQA) && !fclose(file));
    wasm2c_hgengine_free(game); for (u32 i=0;i<3;i++) close(wasi->fds[i]); free(game); free(env); free(wasi); free(capture); reloadEnv=NULL; wasm_rt_free();
    puts(passed ? "PASS: two real original wild battles and settlements, monotonic menu revisions and stale rejection." : "EXPECTED NEGATIVE: both original wild settlements finished; overlay reload reused menu revisions.");
    return passed ? 0 : 3;
}
#endif
