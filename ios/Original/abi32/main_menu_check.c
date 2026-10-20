// SPDX-License-Identifier: GPL-3.0-or-later
#include "application/main_menu/main_menu_util.h"
#include "application/main_menu/main_menu_shared.h"
#include "screen_fade.h"
#include <stddef.h>
#include <string.h>
#define CHECK(v) do { if (!(v)) return __LINE__; } while (0)
_Static_assert(sizeof(MainMenuUtilWork)==0xEA8,"Original main-menu work layout");
_Static_assert(offsetof(MainMenuUtilWork,vblankCallback)==0xEA4,"Original callback offset");
enum HeapID sMainMenuUtilCryptoHeapId;
static BOOL finished;
static u32 fadeCalls;
static struct { enum FadeMode mode; enum FadeType main,sub; u16 color; int steps,frames; enum HeapID heap; } fade;
// Probe-only fade completion controls; the full engine links the original fade implementation.
void BeginNormalPaletteFade(enum FadeMode mode,enum FadeType main,enum FadeType sub,u16 color,int steps,int frames,enum HeapID heap) {
    fadeCalls++; fade.mode=mode; fade.main=main; fade.sub=sub; fade.color=color; fade.steps=steps; fade.frames=frames; fade.heap=heap;
}
BOOL IsPaletteFadeFinished(void) { return finished; }
u32 CheckNativeMainMenuWork(void) {
    MainMenuUtilWork expected; memset(&expected,0,sizeof(expected)); expected.unk_004=1; expected.heapId=HEAP_ID_MAIN_MENU;
    memset(&gMainMenuUtilWork,0xA5,sizeof(gMainMenuUtilWork)); sMainMenuUtilCryptoHeapId=HEAP_ID_3;
    MainMenuUtil_InitWork(HEAP_ID_MAIN_MENU);
    CHECK(!memcmp(&gMainMenuUtilWork,&expected,sizeof(expected)) && sMainMenuUtilCryptoHeapId==HEAP_ID_3);
    int state=0; fadeCalls=0; finished=FALSE;
    MainMenuUtil_StartFade(1,7,&state,8);
    CHECK(state==8 && gMainMenuUtilWork.nextState==7 && fadeCalls==1);
    CHECK(fade.mode==FADE_BOTH_SCREENS && fade.main==1 && fade.sub==1 && !fade.color && fade.steps==6 && fade.frames==1 && fade.heap==HEAP_ID_MAIN_MENU);
    MainMenuUtil_WaitFade(&state); CHECK(state==8);
    finished=TRUE; MainMenuUtil_WaitFade(&state); CHECK(state==7);
    MainMenuUtil_SetFadeWhite(TRUE); MainMenuUtil_StartFade(0,9,NULL,8);
    CHECK(state==7 && gMainMenuUtilWork.nextState==9 && fade.color==0x7FFF && fadeCalls==2);
    CHECK(&sMainMenuUtilSprites==&gMainMenuUtilWork.sprites && &sMainMenuUtilRenderer==&gMainMenuUtilWork.sprites.renderer);
    CHECK((u8 *)ov74_0223D654==(u8 *)&gMainMenuUtilWork+0x200 && (u8 *)ov74_0223E2D4==(u8 *)&gMainMenuUtilWork+0xE80);
    CHECK(!MainMenuUtil_SpritesActive()); gMainMenuUtilWork.sprites.spriteList=(SpriteList *)0x10;
    CHECK(MainMenuUtil_SpritesActive() && MainMenuUtil_GetSpriteList()==&gMainMenuUtilWork.sprites);
    MainMenuUtil_StartSave((SaveData *)0x1234);
    CHECK(gMainMenuUtilWork.saveData==(SaveData *)0x1234 && MainMenuUtil_GetSaveState()==SAVE_STATE_START);
    gMainMenuUtilWork.saveState=SAVE_STATE_PAUSED; MainMenuUtil_ResumeSave();
    CHECK(MainMenuUtil_GetSaveState()==SAVE_STATE_WRITING);
    gMainMenuUtilWork.saveState=SAVE_STATE_DONE; MainMenuUtil_ResumeSave(); CHECK(MainMenuUtil_GetSaveState()==SAVE_STATE_DONE);
    MainMenuUtil_InitWork(HEAP_ID_MAIN_MENU); CHECK(!memcmp(&gMainMenuUtilWork,&expected,sizeof(expected)));
    return 0;
}
