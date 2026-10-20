#ifndef OV58_PRIVATE_H
#define OV58_PRIVATE_H

#include "global.h"

#include "constants/pokemon.h"

#include "apricorn_box.h"
#include "apricorn_internal.h"
#include "bag.h"
#include "gf_gfx_planes.h"
#include "heap.h"
#include "mail.h"
#include "menu_input_state.h"
#include "overlay_58.h"
#include "overlay_manager.h"
#include "party.h"
#include "party_menu.h"
#include "player_data.h"
#include "pokemon_mood.h"
#include "save.h"
#include "screen_fade.h"
#include "system.h"

// ApricornBoxApp.mode, the Apricorn Box screen's mode
enum ApricornBoxMode {
    APRICORN_BOX_MODE_VIEW,
    APRICORN_BOX_MODE_BLENDER,
    APRICORN_BOX_MODE_KURT,
    APRICORN_BOX_MODE_USE_JUICE,
};

// ApricornBoxArgs.mode, how the Apricorn Box was opened
enum ApricornBoxLaunchMode {
    APRICORN_BOX_LAUNCH_VIEW,
    APRICORN_BOX_LAUNCH_BLENDER,
    APRICORN_BOX_LAUNCH_KURT,
    APRICORN_BOX_LAUNCH_JUICE_STAND,
};

typedef struct ApricornBoxApp {
    enum HeapID heapId;
    ApricornBoxArgs *args;
    OverlayManager *child;
    u32 unk_0C;
    PartyMenuArgs *partyMenuArgs;
    u32 juiceIndex;
    u8 mode;
    u8 unk_19;
    u8 lastRows[2];
    u8 partySlot;
} ApricornBoxApp;

void SetMasterBrightnessByColor(PMLCDTarget screen, u16 color);

void ApricornBox_FeedAprijuice(ApricornJuice *record, Party *party, int slot);

#endif // OV58_PRIVATE_H
