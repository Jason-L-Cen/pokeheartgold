#ifndef POKEHEARTGOLD_FRONTIER_FRONTIER_SCRCMD_H
#define POKEHEARTGOLD_FRONTIER_FRONTIER_SCRCMD_H

#include "frontier/frontier_script_context.h"

extern const FrontierScrCmdFunc gFrontierCommandTable[];
extern const u32 gFrontierCommandTableSize;

BOOL FrtCmd_Nop(FrontierScriptContext *ctx);
BOOL FrtCmd_End(FrontierScriptContext *ctx);
BOOL FrtCmd_Exit(FrontierScriptContext *ctx);
BOOL FrtCmd_ChangeMode(FrontierScriptContext *ctx);
BOOL FrtCmd_ChangeModeWithTask(FrontierScriptContext *ctx);
BOOL FrtCmd_Wait(FrontierScriptContext *ctx);
BOOL FrtCmd_SetVar(FrontierScriptContext *ctx);
BOOL FrtCmd_CopyVar(FrontierScriptContext *ctx);
BOOL FrtCmd_AddVar(FrontierScriptContext *ctx);
BOOL FrtCmd_SubVar(FrontierScriptContext *ctx);
BOOL FrtCmd_GoTo(FrontierScriptContext *ctx);
BOOL FrtCmd_GoToIf(FrontierScriptContext *ctx);
BOOL FrtCmd_Call(FrontierScriptContext *ctx);
BOOL FrtCmd_Return(FrontierScriptContext *ctx);
BOOL FrtCmd_CallIf(FrontierScriptContext *ctx);
BOOL FrtCmd_PrintMessageInstant(FrontierScriptContext *ctx);
BOOL FrtCmd_PrintMessage(FrontierScriptContext *ctx);
BOOL FrtCmd_PrintMessageCanSpeedUp(FrontierScriptContext *ctx);
BOOL FrtCmd_CloseMessage(FrontierScriptContext *ctx);
BOOL FrtCmd_FadeScreen(FrontierScriptContext *ctx);
BOOL FrtCmd_WaitFade(FrontierScriptContext *ctx);
BOOL FrtCmd_MenuInitStdGmm(FrontierScriptContext *ctx);
BOOL FrtCmd_MenuInit(FrontierScriptContext *ctx);
BOOL FrtCmd_MenuItemAdd(FrontierScriptContext *ctx);
BOOL FrtCmd_MenuItemAddWithDescription(FrontierScriptContext *ctx);
BOOL FrtCmd_MenuExec(FrontierScriptContext *ctx);
BOOL FrtCmd_ListMenuInitStdGmm(FrontierScriptContext *ctx);
BOOL FrtCmd_ListMenuInit(FrontierScriptContext *ctx);
BOOL FrtCmd_ListMenuItemAdd(FrontierScriptContext *ctx);
BOOL FrtCmd_ListMenuExec(FrontierScriptContext *ctx);
BOOL FrtCmd_ListMenuCancel(FrontierScriptContext *ctx);
BOOL FrtCmd_YesNo(FrontierScriptContext *ctx);
BOOL FrtCmd_CompareVarToValue(FrontierScriptContext *ctx);
BOOL FrtCmd_CompareVarToVar(FrontierScriptContext *ctx);
BOOL FrtCmd_LoadObjectGfx(FrontierScriptContext *ctx);
BOOL FrtCmd_RemoveObjectGfx(FrontierScriptContext *ctx);
BOOL FrtCmd_CreateObjects(FrontierScriptContext *ctx);
BOOL FrtCmd_DeleteObject(FrontierScriptContext *ctx);
BOOL FrtCmd_SetObjectVisible(FrontierScriptContext *ctx);
BOOL FrtCmd_Dummy039(FrontierScriptContext *ctx);
BOOL FrtCmd_ApplyMovement(FrontierScriptContext *ctx);
BOOL FrtCmd_WaitMovement(FrontierScriptContext *ctx);
BOOL FrtCmd_LoadSpriteResources(FrontierScriptContext *ctx);
BOOL FrtCmd_UnloadSpriteResources(FrontierScriptContext *ctx);

BOOL FrtCmd_ShakeMap(FrontierScriptContext *ctx);
BOOL FrtCmd_SetWindowDelayed(FrontierScriptContext *ctx);
BOOL FrtCmd_SaveBattleRecord(FrontierScriptContext *ctx);
BOOL FrtCmd_LinkSyncOrAbort(FrontierScriptContext *ctx);
BOOL FrtCmd_MenuSetAlignRight(FrontierScriptContext *ctx);

BOOL FrtCmd_CreateSprites(FrontierScriptContext *ctx);

BOOL FrtCmd_LinkLobbyAlloc(FrontierScriptContext *ctx);
BOOL FrtCmd_LinkLobbyFree(FrontierScriptContext *ctx);
BOOL FrtCmd_LinkLobbySend(FrontierScriptContext *ctx);
BOOL FrtCmd_LinkLobbyWaitReceive(FrontierScriptContext *ctx);
BOOL FrtCmd_LinkLobbyCheckPartnerFacility(FrontierScriptContext *ctx);
BOOL FrtCmd_LinkLobbyRunPartyApp(FrontierScriptContext *ctx);
BOOL FrtCmd_LinkLobbyGetPartyMenuResult(FrontierScriptContext *ctx);
BOOL FrtCmd_LinkLobbyGetPartnerChoice(FrontierScriptContext *ctx);
BOOL FrtCmd_LinkLobbyAction(FrontierScriptContext *ctx);
BOOL FrtCmd_LinkLobbyStartTower(FrontierScriptContext *ctx);
BOOL FrtCmd_TowerLinkSendTrainers(FrontierScriptContext *ctx);
BOOL FrtCmd_TowerLinkWaitReceive(FrontierScriptContext *ctx);
BOOL FrtCmd_TowerFree(FrontierScriptContext *ctx);
BOOL FrtCmd_LinkLobbyWaitReceiveOrAbort(FrontierScriptContext *ctx);

BOOL FrtCmd_StopBGM(FrontierScriptContext *ctx);
BOOL FrtCmd_TowerAction(FrontierScriptContext *ctx);
BOOL FrtCmd_TowerPrintTrainerMessage(FrontierScriptContext *ctx);
BOOL FrtCmd_TowerFreeData(FrontierScriptContext *ctx);
BOOL FrtCmd_TowerStartBattle(FrontierScriptContext *ctx);
BOOL FrtCmd_TowerEndBattle(FrontierScriptContext *ctx);
BOOL FrtCmd_TowerSendLinkMessage(FrontierScriptContext *ctx);
BOOL FrtCmd_TowerWaitLink(FrontierScriptContext *ctx);

BOOL FrtCmd_ArcadeAlloc(FrontierScriptContext *ctx);
BOOL FrtCmd_ArcadeInit(FrontierScriptContext *ctx);
BOOL FrtCmd_ArcadeFree(FrontierScriptContext *ctx);
BOOL FrtCmd_ArcadeEndBattle(FrontierScriptContext *ctx);
BOOL FrtCmd_ArcadeStartBattle(FrontierScriptContext *ctx);
BOOL FrtCmd_ArcadeSetPartyBeforeBattle(FrontierScriptContext *ctx);
BOOL FrtCmd_ArcadeSetPartyAfterBattle(FrontierScriptContext *ctx);
BOOL FrtCmd_ArcadeAction(FrontierScriptContext *ctx);
BOOL FrtCmd_ArcadeGetBattleResult(FrontierScriptContext *ctx);
BOOL FrtCmd_ArcadeSendBuffer(FrontierScriptContext *ctx);
BOOL FrtCmd_ArcadeReceiveBuffer(FrontierScriptContext *ctx);
BOOL FrtCmd_LaunchGameBoard(FrontierScriptContext *ctx);
BOOL FrtCmd_ArcadePrintMsg(FrontierScriptContext *ctx);
BOOL FrtCmd_ArcadeSetEvent(FrontierScriptContext *ctx);
BOOL FrtCmd_ArcadeSetItemIconVisible(FrontierScriptContext *ctx);

#endif
