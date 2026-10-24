// SPDX-License-Identifier: GPL-3.0-or-later
#define BattleSystem_GetBattlerIdsByType TargetCheckBattlerIds
#define BattleSystem_GetBattleType BattleUICheckBattleType
#include "../../../src/battle/battle_input.c"
#include <stdio.h>
#include <stdlib.h>
#define CHECK(value) do { if (!(value)) return __LINE__; } while (0)
static const u8 targetIds[6] = {0, 1, 2, 0, 3, 1};
static u32 targetLookups;
void BattleSystem_GetBattlerIdsByType(BattleSystem *system, u8 *ids) {
    (void)system; memcpy(ids, targetIds, sizeof(targetIds)); targetLookups++;
}
u32 BattleSystem_GetBattleType(BattleSystem *system) { return system->battleType; }

u32 CheckOriginalBattleUI(void) {
    BattleInput input = {0}, other = {0};
    BattleSystem system = {0}; input.battleSystem=other.battleSystem=&system;
    NativeBattleUIBind(&input);
    const HGNativeBattleUIState *state = HGNativeBattleUIGetState();
    CHECK(sizeof(*state)==65688 && offsetof(HGNativeBattleUIState,namePixels)==152
        && HG_BATTLE_UI_NEXT_MON==4 && HG_BATTLE_UI_KEEP_FORGET==5 && HG_BATTLE_UI_GIVE_UP==6);
    CHECK(state->active && !state->mode && !state->ready);
    input.curMenuId = BATTLE_MENU_3; CHECK(NativeBattleUIMode(&input) == HG_BATTLE_UI_COMMAND);
    input.curMenuId = BATTLE_MENU_4; CHECK(NativeBattleUIMode(&input) == HG_BATTLE_UI_COMMAND);
    input.curMenuId = BATTLE_MENU_MAIN;
    input.textObj[MENUTXT_FIGHT].textObj=(void *)1;
    CHECK(NativeBattleUIPoll(&input) == -1);
    state = HGNativeBattleUIGetState();
    u32 revision = state->revision;
    CHECK(state->mode == HG_BATTLE_UI_COMMAND && state->ready && state->enabledMask == (15 | 32));
    CHECK(!HGNativeBattleUIChoose(revision + 1, 0) && !HGNativeBattleUIChoose(revision, 4));
    CHECK(!HGNativeBattleUIChoose(revision, 0xFFFFFFFF));
    CHECK(HGNativeBattleUIChoose(revision, 2) && !HGNativeBattleUIChoose(revision, 0));
    CHECK(NativeBattleUIPoll(&other) == -1 && NativeBattleUIPoll(&input) == 2);
    CHECK(!HGNativeBattleUIGetState()->ready);
    CHECK(NativeBattleUIPoll(&input) == -1);
    CHECK(HGNativeBattleUIChoose(revision, 1));
    NativeBattleUIInvalidate(&input);
    CHECK(NativeBattleUIPoll(&input) == -1 && !HGNativeBattleUIChoose(revision, 0));
    revision = HGNativeBattleUIGetState()->revision;
    CHECK(HGNativeBattleUIChoose(revision, 5));
    CHECK(!HGNativeBattleUIConsumeBallPocket(&input));
    CHECK(sTouchArrayMainMenu[NativeBattleUIPoll(&input)] == BATTLE_INPUT_BAG);
    input.textObj[MENUTXT_FIGHT].textObj=NULL; NativeBattleUIResourcesFreed(&input);
    CHECK(state->active && !state->mode && !state->ready && !state->enabledMask);
    CHECK(!HGNativeBattleUIConsumeBallPocket(&other));
    CHECK(HGNativeBattleUIConsumeBallPocket(&input) && !HGNativeBattleUIConsumeBallPocket(&input));
    input.textObj[MENUTXT_FIGHT].textObj=(void *)1; NativeBattleUIInvalidate(&input); NativeBattleUIPublish();
    CHECK(state->mode==HG_BATTLE_UI_COMMAND && !state->ready);
    NativeBattleUIPoll(&input); revision = HGNativeBattleUIGetState()->revision;
    CHECK(HGNativeBattleUIChoose(revision, 5));
    NativeBattleUIInvalidate(&input); NativeBattleUIPoll(&input);
    CHECK(!HGNativeBattleUIConsumeBallPocket(&input));
    revision = HGNativeBattleUIGetState()->revision;
    CHECK(HGNativeBattleUIChoose(revision, 5)); NativeBattleUIPoll(&input);
    NativeBattleUIBind(NULL); CHECK(!HGNativeBattleUIConsumeBallPocket(&input));
    NativeBattleUIBind(&input);

    u8 tiles[128] = {0};
    tiles[0] = 0x87; tiles[32] = 7; tiles[64] = 8;
    input.curMenuId = BATTLE_MENU_FIGHT;
    input.menu.fight = (BattleInputFightMenu){.moveNo = {33, 45, 0, 0}, .pp = {32, 0, 0, 0}, .ppMax = {35, 40, 0, 0}, .battlerType = BATTLER_TYPE_PLAYER_SIDE_SLOT_1};
    input.moveMemory[0].moveDisplay[0] = (BattleInputMoveMemory_DisplayObj){.window = {.width = 2, .height = 2, .pixelBuffer = tiles}, .fontLength = 16};
    NativeBattleUIInvalidate(&input); NativeBattleUIPoll(&input);
    state = HGNativeBattleUIGetState(); revision = state->revision;
    CHECK(state->mode == HG_BATTLE_UI_MOVES && state->enabledMask == 19);
    CHECK(!HGNativeBattleUIChoose(revision, 5));
    CHECK(state->move[0] == 33 && state->pp[0] == 32 && state->maxPP[0] == 35 && state->nameWidth[0] == 16);
    CHECK(state->namePixels[0][0] == 250 && state->namePixels[0][3] == 255 && state->namePixels[0][4] == 45);
    CHECK(state->namePixels[0][8 * 4] == 250 && state->namePixels[0][8 * HG_BATTLE_NAME_WIDTH * 4] == 45);
    CHECK(!state->namePixels[0][2 * 4 + 3] && !state->namePixels[1][3]);
    input.moveMemory[0].moveDisplay[0].window.width = 33;
    NativeBattleUIPoll(&input);
    CHECK(!HGNativeBattleUIGetState()->nameWidth[0]);
    input.moveMemory[0].moveDisplay[0].window.width = 2;
    input.moveMemory[0].moveDisplay[0].window.height = 3;
    NativeBattleUIPoll(&input);
    CHECK(!HGNativeBattleUIGetState()->nameWidth[0]);
    input.moveMemory[0].moveDisplay[0].window.height = 2;
    CHECK(!HGNativeBattleUIChoose(revision, 2));
    CHECK(HGNativeBattleUIChoose(revision, 1) && sTouchArrayFightMenu[NativeBattleUIPoll(&input)] == BATTLE_INPUT_MOVE_2); // Original rules report zero PP.
    input.menu.fight.moveNo[2] = 75; input.menu.fight.moveNo[3] = 77;
    for (u32 i = 0; i < 4; i++) {
        NativeBattleUIPoll(&input);
        CHECK(HGNativeBattleUIChoose(revision, i));
        CHECK(sTouchArrayFightMenu[NativeBattleUIPoll(&input)] == BATTLE_INPUT_MOVE_1 + i);
    }
    NativeBattleUIPoll(&input); CHECK(HGNativeBattleUIChoose(revision, 4) && sTouchArrayFightMenu[NativeBattleUIPoll(&input)] == BATTLE_INPUT_CANCEL);
    input.curMenuId = BATTLE_MENU_TARGET;
    const u16 species[4] = {152, 163, 25, 161};
    const u32 expectedInput[4] = {BATTLE_INPUT_TARGET_OPPONENT_LEFT, BATTLE_INPUT_TARGET_OPPONENT_RIGHT, BATTLE_INPUT_TARGET_PLAYER_LEFT, BATTLE_INPUT_TARGET_PLAYER_RIGHT};
    for (u32 range=0;range<NELEMS(sMoveRangeHitMons);range++) for (u32 present=0;present<16;present++) {
        input.monTargetType=range; u8 legal[4],affected[4],visible[4]; u16 labels[4]={0};
        for (u32 i=0;i<4;i++) input.menu.target.targetMons[targetIds[2+i]]=(TargetPokemon){.exists=(present>>i)&1,.hp=10+i,.hpMax=20+i};
        BattleInput_GetMonsHitArray(&input,legal,TRUE); BattleInput_GetMonsHitArray(&input,visible,FALSE);
        for (u32 i=0;i<4;i++) if (((present>>i)&1) && visible[i]) labels[i]=species[i];
        NativeBattleUITargetReset(); NativeBattleUICacheTargets(&input,targetIds,labels);
        NativeBattleUIInvalidate(&input); NativeBattleUIPoll(&input); state=HGNativeBattleUIGetState(); revision=state->revision;
        CHECK(state->mode==HG_BATTLE_UI_TARGETS && state->ready && (state->enabledMask&16) && !(state->enabledMask&32));
        for (u32 i=0;i<4;i++) {
            u32 hitbox=sNativeTargetHitboxes[i],exists=(present>>hitbox)&1;
            CHECK(((state->enabledMask>>i)&1)==(legal[hitbox]!=0));
            CHECK(state->targetSpecies[i]==(exists ? labels[hitbox] : 0));
            CHECK(state->targetHp[i]==(exists ? 10+hitbox : 0) && state->targetMaxHp[i]==(exists ? 20+hitbox : 0));
            u32 mask=0;
            if (exists) {
                BattleInput_GetMonsHitBySelectedTarget(&input,expectedInput[i],affected,TRUE);
                for (u32 j=0;j<4;j++) if (affected[sNativeTargetHitboxes[j]]) mask|=1u<<j;
            }
            CHECK(state->targetMask[i]==mask);
            if (legal[hitbox]) {
                CHECK(HGNativeBattleUIChoose(revision,i) && !HGNativeBattleUIChoose(revision,i));
                CHECK(sTouchArrayTargetMenu[NativeBattleUIPoll(&input)]==expectedInput[i]);
                NativeBattleUIPoll(&input);
            } else CHECK(!HGNativeBattleUIChoose(revision,i));
        }
        CHECK(!HGNativeBattleUIChoose(revision,5) && HGNativeBattleUIChoose(revision,4));
        CHECK(sTouchArrayTargetMenu[NativeBattleUIPoll(&input)]==BATTLE_INPUT_CANCEL);
    }
    NativeBattleUITargetReset();
    tiles[0]=0x21;
    Window targetWindow={.width=2,.height=2,.pixelBuffer=tiles};
    NativeBattleUICacheName(&input,&input.textObj[3],&targetWindow,16,MAKE_TEXT_COLOR(1,2,3));
    memset(tiles,0,sizeof(tiles)); NativeBattleUIPoll(&input); state=HGNativeBattleUIGetState();
    CHECK(state->nameWidth[0]==16 && state->namePixels[0][0]==250 && state->namePixels[0][4]==45 && !state->namePixels[0][11]);
    u32 lookups=targetLookups; CHECK(HGNativeBattleUIGetState()==state && targetLookups==lookups);
    BattleInputTargetMenu targetMenu=input.menu.target;
    const u32 twoOptionMenus[]={BATTLE_MENU_SWITCH_OR_FLEE,BATTLE_MENU_KEEP_FORGET_MOVE,BATTLE_MENU_GIVE_UP_ON_MOVE};
    const u32 twoOptionModes[]={HG_BATTLE_UI_NEXT_MON,HG_BATTLE_UI_KEEP_FORGET,HG_BATTLE_UI_GIVE_UP};
    for (u32 choiceMenu=0;choiceMenu<NELEMS(twoOptionMenus);choiceMenu++) {
        input.curMenuId=other.curMenuId=twoOptionMenus[choiceMenu]; NativeBattleUITargetReset();
        CHECK(!sNativeTargetUI.nameWidth[0] && !sNativeTargetUI.namePixels[0][3]);
        tiles[0]=0x21; NativeBattleUICacheName(&input,&input.textObj[0],&targetWindow,16,MAKE_TEXT_COLOR(1,2,3));
        tiles[0]=0xBA; NativeBattleUICacheName(&input,&input.textObj[1],&targetWindow,16,MAKE_TEXT_COLOR(10,11,12));
        NativeBattleUICacheName(&input,&input.textObj[2],&targetWindow,16,MAKE_TEXT_COLOR(1,2,3));
        memset(tiles,0,sizeof(tiles)); NativeBattleUIInvalidate(&input); NativeBattleUIPoll(&input);
        state=HGNativeBattleUIGetState(); revision=state->revision;
        CHECK(state->mode==twoOptionModes[choiceMenu] && state->ready && state->enabledMask==3 && !state->cancelRun);
        CHECK(NativeBattleUIMode(&other)==HG_BATTLE_UI_NONE && NativeBattleUIPoll(&other)==-1);
        CHECK(state->nameWidth[0]==16 && state->nameWidth[1]==16 && !state->nameWidth[2] && !state->nameWidth[3]
            && state->namePixels[0][0]==250 && state->namePixels[0][4]==45 && state->namePixels[1][0]==250 && state->namePixels[1][4]==45);
        CHECK(!state->move[0] && !state->pp[0] && !state->maxPP[0] && !state->targetSpecies[0] && !state->targetMask[0]);
        NativeBattleUICacheName(&other,&input.textObj[0],&targetWindow,8,MAKE_TEXT_COLOR(1,2,3));
        NativeBattleUIPoll(&input); CHECK(state->nameWidth[0]==16 && state->namePixels[0][0]==250);
        CHECK(!HGNativeBattleUIChoose(revision+1,0));
        for (u32 i=2;i<6;i++) CHECK(!HGNativeBattleUIChoose(revision,i));
        CHECK(HGNativeBattleUIChoose(revision,0) && !HGNativeBattleUIChoose(revision,1));
        CHECK(NativeBattleUIPoll(&other)==-1); // A foreign menu cannot consume the owner's queued choice.
        CHECK(sTouchArrayTwoOptionsMenu[NativeBattleUIPoll(&input)]==BATTLE_INPUT_YES && !state->ready);
        NativeBattleUIPoll(&input); CHECK(HGNativeBattleUIChoose(revision,1));
        CHECK(sTouchArrayTwoOptionsMenu[NativeBattleUIPoll(&input)]==BATTLE_INPUT_CANCEL && !HGNativeBattleUIConsumeBallPocket(&input));
        NativeBattleUIPoll(&input); CHECK(HGNativeBattleUIChoose(revision,0)); NativeBattleUIInvalidate(&input);
        CHECK(NativeBattleUIPoll(&input)==-1 && !HGNativeBattleUIChoose(revision,0));
        revision=state->revision;
        input.isTouchDisabled=TRUE; NativeBattleUIPoll(&input); CHECK(!state->ready && !HGNativeBattleUIChoose(revision,0));
        input.isTouchDisabled=FALSE; input.tutorial.finger=(void *)1;
        NativeBattleUICacheName(&input,&input.textObj[0],&targetWindow,8,MAKE_TEXT_COLOR(1,2,3));
        CHECK(sNativeTargetUI.nameWidth[0]==16 && sNativeTargetUI.namePixels[0][0]==250);
        NativeBattleUIPoll(&input); CHECK(NativeBattleUIMode(&input)==HG_BATTLE_UI_NONE && !state->ready);
        input.tutorial.finger=NULL; system.battleType=BATTLE_TYPE_TUTORIAL;
        NativeBattleUICacheName(&input,&input.textObj[0],&targetWindow,8,MAKE_TEXT_COLOR(1,2,3));
        CHECK(sNativeTargetUI.nameWidth[0]==16 && sNativeTargetUI.namePixels[0][0]==250);
        NativeBattleUIPoll(&input); CHECK(NativeBattleUIMode(&input)==HG_BATTLE_UI_NONE && !state->ready && !HGNativeBattleUIChoose(state->revision,0));
        system.battleType=0;
        NativeBattleUIBind(NULL); CHECK(NativeBattleUIMode(&input)==HG_BATTLE_UI_NONE && NativeBattleUIPoll(&input)==-1);
        NativeBattleUIBind(&input);
    }
    const u32 unsupportedYesNo[]={BATTLE_MENU_YES_NO,BATTLE_MENU_SWITCH_OR_KEEP,BATTLE_MENU_5,BATTLE_MENU_6};
    for (u32 i=0;i<NELEMS(unsupportedYesNo);i++) {
        input.curMenuId=unsupportedYesNo[i]; NativeBattleUIInvalidate(&input); NativeBattleUIPoll(&input);
        CHECK(!state->mode && !state->ready && !state->enabledMask && !state->nameWidth[0] && !HGNativeBattleUIChoose(state->revision,0));
    }
    input.menu.fight=(BattleInputFightMenu){.moveNo={33,45,0,0},.pp={32,0,0,0},.ppMax={35,40,0,0},.battlerType=BATTLER_TYPE_PLAYER_SIDE_SLOT_1};
    tiles[0]=0x78; input.curMenuId=BATTLE_MENU_FIGHT; NativeBattleUIInvalidate(&input); NativeBattleUIPoll(&input);
    CHECK(state->mode==HG_BATTLE_UI_MOVES && state->move[0]==33 && state->pp[0]==32 && state->maxPP[0]==35 && state->nameWidth[0]==16
        && state->namePixels[0][0]==45 && state->namePixels[0][3]==255 && state->namePixels[0][4]==250
        && memcmp(state->namePixels[0],sNativeTargetUI.namePixels[0],sizeof(state->namePixels[0])));
    input.menu.target=targetMenu; input.curMenuId=BATTLE_MENU_TARGET; NativeBattleUITargetReset(); NativeBattleUICacheTargets(&input,targetIds,species);
    NativeBattleUIInvalidate(&input); NativeBattleUIPoll(&input);
    CHECK(state->mode==HG_BATTLE_UI_TARGETS && !state->nameWidth[0] && !state->nameWidth[1] && state->targetSpecies[0]);
    const u32 resourceMenus[]={BATTLE_MENU_MAIN,BATTLE_MENU_FIGHT,BATTLE_MENU_TARGET,
        BATTLE_MENU_SWITCH_OR_FLEE,BATTLE_MENU_KEEP_FORGET_MOVE,BATTLE_MENU_GIVE_UP_ON_MOVE};
    const u32 resourceModes[]={HG_BATTLE_UI_COMMAND,HG_BATTLE_UI_MOVES,HG_BATTLE_UI_TARGETS,
        HG_BATTLE_UI_NEXT_MON,HG_BATTLE_UI_KEEP_FORGET,HG_BATTLE_UI_GIVE_UP};
    for (u32 menu=0;menu<NELEMS(resourceMenus);menu++) {
        input.curMenuId=resourceMenus[menu];
        if (menu==1) input.menu.fight=(BattleInputFightMenu){.moveNo={33,45,0,0},.pp={32,40,0,0},.ppMax={35,40,0,0},.battlerType=BATTLER_TYPE_PLAYER_SIDE_SLOT_1};
        if (menu==2) input.menu.target=targetMenu;
        for (u32 i=0;i<NELEMS(input.textObj);i++) input.textObj[i].textObj=NULL;
        u32 font=menu==1 ? MENUTXT_CANCEL : menu==2 ? MENUTXT_TARGET_CANCEL : menu>=3 ? 1 : MENUTXT_FIGHT;
        input.textObj[font].textObj=(void *)1;
        NativeBattleUIInvalidate(&input); NativeBattleUIPublish();
        CHECK(state->mode==resourceModes[menu] && !state->ready);
        NativeBattleUIPoll(&input); CHECK(state->ready);
        NativeBattleUISelected(&input); NativeBattleUIPublish();
        CHECK(state->mode==resourceModes[menu] && !state->ready);
        NativeBattleUIInvalidate(&input); NativeBattleUIPublish();
        CHECK(state->mode==resourceModes[menu] && !state->ready);
        input.isTouchDisabled=TRUE; NativeBattleUIPoll(&input);
        CHECK(state->mode==resourceModes[menu] && !state->ready);
        input.isTouchDisabled=FALSE; NativeBattleUIPoll(&input);
        revision=state->revision; u32 choice=menu==1 || menu==2 ? 4 : 0;
        CHECK(HGNativeBattleUIChoose(revision,choice));
        u32 enabled=state->enabledMask;
        NativeBattleUIResourcesFreed(&other); NativeBattleUIResourcesFreed(NULL);
        CHECK(state->active && state->mode==resourceModes[menu] && !state->ready && state->revision==revision
            && state->enabledMask==enabled && sNativeBattleChoice==(int)choice);
        for (u32 i=0;i<NELEMS(input.textObj);i++) input.textObj[i].textObj=NULL;
        NativeBattleUIResourcesFreed(&input);
        CHECK(state->active && !state->mode && !state->ready && !state->enabledMask && state->revision==revision+1 && sNativeBattleChoice==-1);
        CHECK(NativeBattleUIMode(&input)==resourceModes[menu]);
        for (u32 i=0;i<4;i++) CHECK(!state->move[i] && !state->pp[i] && !state->maxPP[i] && !state->nameWidth[i]
            && !state->targetSpecies[i] && !state->targetHp[i] && !state->targetMaxHp[i] && !state->targetMask[i]);
        u32 pixels=0; for (u32 i=0;i<sizeof(state->namePixels);i++) pixels|=((const u8 *)state->namePixels)[i]; CHECK(!pixels);
        for (u32 i=0;i<3;i++) {
            NativeBattleUIPublish(); CHECK(NativeBattleUIPoll(&input)==-1);
            CHECK(state->active && !state->mode && !state->ready && !state->enabledMask && state->revision==revision+1);
            CHECK(!HGNativeBattleUIChoose(revision,choice) && !HGNativeBattleUIChoose(state->revision,choice));
        }
        NativeBattleUIInvalidate(&input); NativeBattleUIPublish(); CHECK(!state->mode);
        input.textObj[font].textObj=(void *)1; NativeBattleUIPublish();
        CHECK(state->mode==resourceModes[menu] && !state->ready);
        NativeBattleUIPoll(&input); CHECK(state->mode==resourceModes[menu] && state->ready);
    }
    input.isTouchDisabled = TRUE; NativeBattleUIPoll(&input); CHECK(!HGNativeBattleUIGetState()->ready);
    input.isTouchDisabled = FALSE; input.tutorial.finger = (void *)1;
    NativeBattleUIPoll(&input);
    CHECK(!HGNativeBattleUIGetState()->mode && !HGNativeBattleUIChoose(revision, 0));
    input.curMenuId=BATTLE_MENU_SWITCH_OR_FLEE; NativeBattleUIPoll(&input);
    CHECK(NativeBattleUIMode(&input)==HG_BATTLE_UI_NONE && !state->ready && !HGNativeBattleUIChoose(state->revision,0));
    NativeBattleUIBind(NULL); state = HGNativeBattleUIGetState(); CHECK(!state->active && !state->ready && !state->nameWidth[0] && !sNativeTargetUI.nameWidth[0]);
    input.tutorial.finger=NULL;
    CHECK(NativeBattleUIMode(&input)==HG_BATTLE_UI_NONE && NativeBattleUIPoll(&input)==-1 && !HGNativeBattleUIChoose(state->revision,0));
    return 0;
}
#ifndef HEARTGOLD_ABI32
int main(void) { u32 line = CheckOriginalBattleUI(); if (line) { fprintf(stderr,"Battle UI check line %u\n",line); return 1; } puts("PASS: native battle choices, owned glyphs, busy/completed resource projections, stale inputs and owner/tutorial boundaries."); return 0; }
#endif
