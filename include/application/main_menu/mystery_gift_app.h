#ifndef POKEHEARTGOLD_APPLICATION_MAIN_MENU_MYSTERY_GIFT_APP_H
#define POKEHEARTGOLD_APPLICATION_MAIN_MENU_MYSTERY_GIFT_APP_H

#include "bg_window.h"
#include "gf_gfx_planes.h"
#include "mystery_gift.h"
#include "overlay_manager.h"
#include "sprite.h"
#include "sys_task.h"

typedef struct MysteryGiftAppData MysteryGiftAppData;
typedef struct MysteryGiftAnimData MysteryGiftAnimData;
typedef struct MysteryGiftParticle MysteryGiftParticle;

typedef int (*MysteryGiftMenuFunc)(OverlayManager *manager);

typedef struct MysteryGiftMenuItem {
    u32 msgId;
    MysteryGiftMenuFunc func;
} MysteryGiftMenuItem;

void thunk_MysteryGiftApp_SetWaitingIconVisible(MysteryGiftAppData *data, BOOL visible);
void MysteryGiftApp_RunWfcDownload(OverlayManager *manager, int *state);
void MysteryGiftApp_ShowWfcError(MysteryGiftAppData *data);
OverlayManager *MysteryGiftApp_GetOverlayManager(void);

#endif // POKEHEARTGOLD_APPLICATION_MAIN_MENU_MYSTERY_GIFT_APP_H
