// SPDX-License-Identifier: GPL-3.0-or-later
// Private readonly observation; original Oak/Naming/field code owns all progress and names.
#include "oak_qa.h"
#include "constants/flags.h"
#include "field_system.h"
#include "oaks_speech_internal.h"
#include "party.h"
#include "player_avatar.h"
#include "player_avatar_movement.h"
#include "player_data.h"
#include "pm_string.h"
#include "save_arrays.h"
#include "save_vars_flags.h"
#include "screen_fade.h"
#include "system.h"
#include "task.h"
#include <string.h>
static HGNativeOakQAState sOakQA={.version=HG_OAK_QA_VERSION,.byteSize=sizeof(HGNativeOakQAState)};
static const void *sOakBootOwner,*sOakNamingOwner;
static uint32_t sOakLastActive;
static uint32_t OakQANext(uint32_t value){return value+1 ? value+1 : 1;}
#define OAK_REQUIRE(v) do{if(!(v) && !sOakQA.error)sOakQA.error=__LINE__;}while(0)
static void OakQACopyName(uint32_t out[8],const u16 *name){for(unsigned i=0;i<8;i++){out[i]=name ? name[i] : 0xFFFF;if(out[i]==0xFFFF){for(++i;i<8;i++)out[i]=0xFFFF;break;}}}
const HGNativeOakQAState *HGNativeOakQAGetState(void){return &sOakQA;}
void NPCYNBootPublish(const void *owner,const HGYNBootFacts *facts){
    if(!owner || !facts)return;uint32_t gen=sOakQA.boot.ownerGeneration,rev=OakQANext(sOakQA.boot.revision);
    if(owner!=sOakBootOwner || !sOakQA.boot.active)gen=OakQANext(gen);
    sOakBootOwner=owner;sOakQA.boot=*facts;sOakQA.boot.active=1;sOakQA.boot.ownerGeneration=gen;sOakQA.boot.revision=rev;sOakQA.revision=OakQANext(sOakQA.revision);
}
void NPCYNBootRetire(const void *owner){if(!owner || owner!=sOakBootOwner)return;sOakBootOwner=NULL;sOakQA.boot.active=sOakQA.boot.ready=0;sOakQA.boot.revision=OakQANext(sOakQA.boot.revision);}
void OakQAMainMenuObserved(uint32_t phase,uint32_t empty,uint32_t selected,uint32_t checkpoint){
    sOakQA.mainEmpty=empty;sOakQA.mainSelectedNewGame=selected==HG_OAK_QA_NEW_GAME;
    if(phase==3 || phase==8 || phase==7)sOakQA.mainRouteMask|=1u<<phase;
    OAK_REQUIRE(checkpoint<=HG_OAK_QA_MAIN_TERMINAL);
    if(checkpoint==HG_OAK_QA_MAIN_SELECTED)OAK_REQUIRE(phase==3 && empty && selected==HG_OAK_QA_NEW_GAME);
    if(checkpoint==HG_OAK_QA_MAIN_TERMINAL){OAK_REQUIRE(phase==7 && empty && selected==HG_OAK_QA_NEW_GAME);sOakQA.mainTerminal=1;}
}
void OakQAObserved(const void *owner,uint32_t parentPhase){
    const OakSpeechData *data=owner;if(!data)return;sOakQA.frame=OS_GetVBlankCount();sOakQA.revision=OakQANext(sOakQA.revision);
    sOakQA.ui=*HGNativeOakUIGetState();sOakQA.naming=*HGNativeNamingUIGetState();
    OAK_REQUIRE(sOakQA.ui.parentPhase==parentPhase);
    if(sOakQA.ui.active && !sOakLastActive)++sOakQA.oakEntries;sOakLastActive=sOakQA.ui.active;
}
void OakQANamingObserved(const void *owner,const HGOakNamingFacts *facts){
    if(!owner || !facts)return;if(owner!=sOakNamingOwner){OAK_REQUIRE(!sOakNamingOwner);sOakNamingOwner=owner;++sOakQA.namingEntries;}
    sOakQA.edit=*facts;sOakQA.naming=*HGNativeNamingUIGetState();sOakQA.revision=OakQANext(sOakQA.revision);sOakQA.frame=OS_GetVBlankCount();
}
void OakQANamingResultObserved(const void *owner,const u16 *name,uint32_t length,uint32_t noInput){
    OAK_REQUIRE(owner && owner==sOakNamingOwner && length==1 && !noInput);
    OakQACopyName(sOakQA.resultName,name);OAK_REQUIRE(sOakQA.resultName[1]==0xFFFF);++sOakQA.namingExits;sOakNamingOwner=NULL;
}
static void OakQAFieldObserved(FieldSystem *field){
    sOakQA.frame=OS_GetVBlankCount();sOakQA.revision=OakQANext(sOakQA.revision);sOakQA.ui=*HGNativeOakUIGetState();sOakQA.naming=*HGNativeNamingUIGetState();
    if(!sOakQA.ui.active && sOakLastActive){++sOakQA.oakExits;sOakLastActive=0;}
    sOakQA.fieldReady=field && field->saveData && FieldSystem_IsFieldMapLoaded(field) && FieldSystem_IsFieldMapRunning(field)
        && !FieldSystem_TaskIsRunning(field) && !FieldSystem_IsChildAppRunning(field) && FieldSystem_IsPlayerMovementAllowed(field) && IsPaletteFadeFinished();
    if(!field || !field->saveData || !field->location || !field->playerAvatar)return;
    sOakQA.mapId=field->location->mapId;sOakQA.x=PlayerAvatar_GetXCoord(field->playerAvatar);sOakQA.z=PlayerAvatar_GetZCoord(field->playerAvatar);
    sOakQA.facing=PlayerAvatar_GetFacingDirection(field->playerAvatar);sOakQA.betweenSteps=PlayerAvatar_IsBetweenSteps(field->playerAvatar);
    sOakQA.moveState=PlayerAvatar_GetMoveState(field->playerAvatar);sOakQA.playerMoveState=PlayerAvatar_GetPlayerMoveState(field->playerAvatar);
    sOakQA.partyCount=Party_GetCount(SaveArray_Party_Get(field->saveData));sOakQA.gotStarter=Save_VarsFlags_CheckFlagInArray(Save_VarsFlags_Get(field->saveData),FLAG_GOT_STARTER);
    PlayerProfile *profile=Save_PlayerData_GetProfile(field->saveData);sOakQA.trainerGender=PlayerProfile_GetTrainerGender(profile);OakQACopyName(sOakQA.profileName,PlayerProfile_GetNamePtr(profile));
}
