#ifndef POKEHEARTGOLD_APPLICATION_POKEWALKER_POKEWALKER_EVENT_LOG_H
#define POKEHEARTGOLD_APPLICATION_POKEWALKER_POKEWALKER_EVENT_LOG_H

#include "application/pokewalker/pokewalker_modes2.h"

#include "bg_window.h"
#include "message_format.h"
#include "msgdata.h"
#include "overlay_manager.h"
#include "sprite_system.h"
#include "string_util.h"

// Event types of the Pokewalker event log (PokewalkerIrRecord eventType) as the event log app shows them.
// PokewalkerEventLog_BuildEntryList turns 12 into 11, 14 into 13, 28 into 12 and 29 into 14 first.
enum PokewalkerEventType {
    POKEWALKER_EVENT_NONE,
    POKEWALKER_EVENT_PEER_FIRST, // 1-10: met the Pokemon of another game's Pokewalker (5 kinds, odd/even outcome)
    POKEWALKER_EVENT_PEER_LAST = 10,
    POKEWALKER_EVENT_DOWSING_ITEM,   // found an item with the Dowsing Machine
    POKEWALKER_EVENT_GIFT_ITEM,      // an item was dropped here (28 as read from the Pokewalker)
    POKEWALKER_EVENT_RADAR_CAUGHT,   // caught a Pokemon found with the Poke Radar
    POKEWALKER_EVENT_GIFT_MON,       // met a Pokemon (29 as read from the Pokewalker)
    POKEWALKER_EVENT_RADAR_FLED,     // the Pokemon found with the Poke Radar ran away
    POKEWALKER_EVENT_RADAR_LOST,     // lost against the Pokemon found with the Poke Radar
    POKEWALKER_EVENT_MON_FOUND_ITEM, // the walking Pokemon found an item
    POKEWALKER_EVENT_HAPPY_FIRST,    // 18-22: the walking Pokemon is happy
    POKEWALKER_EVENT_HAPPY_LAST = 22,
    POKEWALKER_EVENT_MON_JOINED,    // the walking Pokemon joined the player
    POKEWALKER_EVENT_MON_WENT_HOME, // the walking Pokemon went home, too tired
    POKEWALKER_EVENT_STROLL_START,  // set out on a Stroll
    POKEWALKER_EVENT_STROLL_END,    // returned from the Stroll (added by PokewalkerEventLog_InitStrollEndRecord)
    POKEWALKER_EVENT_STEPS,         // steps walked in an hour, or the Pokemon took a nap
    POKEWALKER_EVENT_RAW_GIFT_ITEM,
    POKEWALKER_EVENT_RAW_GIFT_MON,
};

// Sprites of the event log app (PokewalkerEventLog sprites).
enum PokewalkerEventLogSprite {
    EVENT_LOG_SPRITE_PREV_PAGE,
    EVENT_LOG_SPRITE_NEXT_PAGE,
    EVENT_LOG_SPRITE_EXIT,
    EVENT_LOG_SPRITE_COURSE, // animation = course of the entry
    EVENT_LOG_SPRITE_LEFT_ICON,
    EVENT_LOG_SPRITE_RIGHT_ICON,
    EVENT_LOG_SPRITE_LEFT_EFFECT,
    EVENT_LOG_SPRITE_RIGHT_EFFECT,
    EVENT_LOG_SPRITE_CENTER_EFFECT,
    EVENT_LOG_SPRITE_MON_EFFECT,
    EVENT_LOG_SPRITE_MAX,
};

// A Pokemon walking on the event log's main screen: overworld picture loaded from the overworld models.
enum PokewalkerEventLogMonSpriteSize {
    MON_SPRITE_SMALL, // 32x32
    MON_SPRITE_LARGE, // 64x64
};

typedef struct PokewalkerEventLogMonSprite {
    BOOL isLarge;
    int species;
    ManagedSprite *sprites[2]; // small, large
    ManagedSprite *shadowSprite;
} PokewalkerEventLogMonSprite;

// Hop-and-run-away animation of the right-hand Pokemon (Poke Radar Pokemon that fled).
typedef struct PokewalkerEventLogFleeAnim {
    u32 active : 1;
    u32 phase : 8;
    u32 hops : 7;
    u32 frame : 16;
} PokewalkerEventLogFleeAnim;

// Poke Ball animation of a caught Poke Radar Pokemon.
typedef struct PokewalkerEventLogCatchAnim {
    u32 active : 1;
    u32 state : 7;
    u32 timer : 24;
} PokewalkerEventLogCatchAnim;

// Work of the Pokewalker event log app (heap HEAP_ID_155).
typedef struct PokewalkerEventLog {
    PokewalkerEventLogArgs *args;
    enum HeapID heapId;
    const u16 *playerName;
    u32 playerGender;
    NARC *narc;
    BgConfig *bgConfig;
    Window windows[4]; // date, time, message, quit button
    MessageFormat *msgFormat;
    MsgData *msgData;
    MsgData *speciesNames;
    MsgData *itemNames;
    SpriteSystem *spriteSystem;
    SpriteManager *spriteManager;
    ManagedSprite *sprites[EVENT_LOG_SPRITE_MAX];
    PokewalkerEventLogMonSprite monSprites[2]; // left, right
    PokewalkerIrRecord *entries[24];           // sorted by time
    RTCDate date;                              // of the shown entry
    RTCTime time;
    u8 loadState;
    u8 page;
    u8 count;
    u8 dowsingStreakPage; // page of the 4th Dowsing Machine item, 0 = none
    u8 radarStreakPage;   // page of the 4th Poke Radar catch, 0 = none
    u8 dowsingCount;
    u8 radarCount;
    u8 peerCount;
    u32 course : 8; // course of the shown entry (0xFF = none yet)
    u32 hasGiftMon : 1;
    u32 hasGiftItem : 1;
    u32 pageTurnActive : 1;
    u32 pageTurnState : 4;
    u32 unused : 17;
    PokewalkerEventLogFleeAnim fleeAnim;
    PokewalkerEventLogCatchAnim catchAnim;
} PokewalkerEventLog;

typedef struct PokewalkerEventLogSlot {
    PokewalkerIrRecord *record;
    u32 index;
} PokewalkerEventLogSlot;

// Data sent to the Pokewalker: 16 blocks of the upload with offset, size and byte sum.
typedef struct PokewalkerUploadBlock {
    u16 offset;
    u8 size;
    u8 checksum;
} PokewalkerUploadBlock;

void PokewalkerApp_WriteUploadBlocks(PokewalkerUploadBlock *blocks);
BOOL PokewalkerEventLog_Init(OverlayManager *manager, int *state);
BOOL PokewalkerEventLog_Exit(OverlayManager *manager, int *state);
BOOL PokewalkerEventLog_Main(OverlayManager *manager, int *state);
void PokewalkerEventLog_InitStrollEndRecord(PokewalkerIrRecord *record, PokewalkerEventLogParams *params);
void PokewalkerEventLog_SetGiftMon(PokewalkerIrRecord *records, PokewalkerEventLogMon *mon);
BOOL PokewalkerEventLog_HasGiftMon(PokewalkerIrRecord *records);

#endif // POKEHEARTGOLD_APPLICATION_POKEWALKER_POKEWALKER_EVENT_LOG_H
