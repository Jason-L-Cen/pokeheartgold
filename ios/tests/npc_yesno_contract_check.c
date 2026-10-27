// SPDX-License-Identifier: GPL-3.0-or-later
// Compile twice: actual save-ledger/resident publisher, then actual boot fact predicates.
#ifdef NPCYN_CHECK_LEDGER
// SPDX-License-Identifier: GPL-3.0-or-later
// Pure host check of the actual observer ledger and full-byte comparisons; no game objects link.
#include "npc_yesno_guest.inc"

static Party fixtureParty;
static PCStorage fixturePC;
static Bag fixtureBag;
static SaveVarsFlags fixtureFlags;
static SaveEasyChat fixtureEasy;
static PlayerProfile fixtureProfile;
static FieldSystem fixtureField;

Party *SaveArray_Party_Get(SaveData *save) { (void)save; return &fixtureParty; }
PCStorage *SaveArray_PCStorage_Get(SaveData *save) { (void)save; return &fixturePC; }
Bag *Save_Bag_Get(SaveData *save) { (void)save; return &fixtureBag; }
SaveVarsFlags *Save_VarsFlags_Get(SaveData *save) { (void)save; return &fixtureFlags; }
SaveEasyChat *Save_EasyChat_Get(SaveData *save) { (void)save; return &fixtureEasy; }
PlayerProfile *Save_PlayerData_GetProfile(SaveData *save) { (void)save; return &fixtureProfile; }
u32 PlayerProfile_GetMoney(PlayerProfile *profile) { return profile->money; }
u32 SaveArray_Party_sizeof(void) { return sizeof(fixtureParty); }
u32 PCStorage_sizeof(void) { return sizeof(fixturePC); }
u32 Save_Bag_sizeof(void) { return sizeof(fixtureBag); }
u32 Save_VarsFlags_sizeof(void) { return sizeof(fixtureFlags); }
u32 Save_EasyChat_sizeof(void) { return sizeof(fixtureEasy); }

#define CHECK(value) do { if (!(value)) return __LINE__; } while (0)

int main(void) {
    sNPCYNField=&fixtureField;
    fixtureProfile.money=12345;
    fixtureEasy.greetings=UINT32_C(0x11223344);
    NPCYNSnapshot(TRUE);
    CHECK(!sNPCYN.error && sNPCYN.checkpointSerial==1 && sNPCYN.conservationChecks==0);
    CHECK(sNPCYN.partyUnchanged && sNPCYN.pcUnchanged && sNPCYN.bagUnchanged && sNPCYN.moneyUnchanged
        && sNPCYN.varsFlagsUnchanged && sNPCYN.easyChatUnchanged && sNPCYN.varsFlagsLedgerMatched && sNPCYN.easyChatLedgerMatched);
    NPCYNSnapshot(FALSE);
    CHECK(!sNPCYN.error && sNPCYN.checkpointSerial==2 && sNPCYN.conservationChecks==1);

    SaveVarsFlags expectedFlags;
    SaveEasyChat expectedEasy;
    for (u32 word=0;word<32;++word) {
        CHECK(NPCYNMakeYesLedger(&sNPCYNVarsFlags,&sNPCYNEasyChat,word,&expectedFlags,&expectedEasy));
        CHECK(expectedEasy.greetings==fixtureEasy.greetings && expectedEasy.trendy==(UINT32_C(1)<<word));
        for (u32 i=0;i<sizeof(expectedFlags);++i)
            CHECK(((const u8 *)&expectedFlags)[i]==(i==offsetof(SaveVarsFlags,flags)+FLAG_UNK_ABA/8 ? 1u<<(FLAG_UNK_ABA%8) : 0));
        CHECK(!NPCYNMakeYesLedger(&sNPCYNVarsFlags,&expectedEasy,word,&expectedFlags,&expectedEasy));
    }
    CHECK(!NPCYNMakeYesLedger(&sNPCYNVarsFlags,&sNPCYNEasyChat,32,&expectedFlags,&expectedEasy));
    CHECK(!NPCYNMakeYesLedger(&sNPCYNVarsFlags,&sNPCYNEasyChat,UINT32_MAX,&expectedFlags,&expectedEasy));
    CHECK(!NPCYNMakeYesLedger(&expectedFlags,&sNPCYNEasyChat,0,&expectedFlags,&expectedEasy));
    CHECK(!memcmp(&sNPCYNVarsFlags,&fixtureFlags,sizeof(fixtureFlags)) && !memcmp(&sNPCYNEasyChat,&fixtureEasy,sizeof(fixtureEasy)));

    CHECK(NPCYNMakeYesLedger(&sNPCYNVarsFlags,&sNPCYNEasyChat,31,&sNPCYNExpectedVarsFlags,&sNPCYNExpectedEasyChat));
    sNPCYN.ledgerPrepared=1;
    fixtureFlags=sNPCYNExpectedVarsFlags; fixtureEasy=sNPCYNExpectedEasyChat;
    NPCYNSnapshot(FALSE);
    CHECK(!sNPCYN.error && sNPCYN.checkpointSerial==3 && sNPCYN.conservationChecks==2);
    CHECK(!sNPCYN.varsFlagsUnchanged && !sNPCYN.easyChatUnchanged && sNPCYN.varsFlagsLedgerMatched && sNPCYN.easyChatLedgerMatched);

    fixtureFlags.vars[0]^=1;
    NPCYNSnapshot(FALSE);
    CHECK(sNPCYN.error && !sNPCYN.varsFlagsLedgerMatched && sNPCYN.checkpointSerial==3);
    sNPCYN.error=0; fixtureFlags=sNPCYNExpectedVarsFlags; fixtureEasy.greetings^=1;
    NPCYNSnapshot(FALSE);
    CHECK(sNPCYN.error && !sNPCYN.easyChatLedgerMatched && sNPCYN.checkpointSerial==3);
    sNPCYN.error=0; fixtureEasy=sNPCYNExpectedEasyChat; fixtureEasy.trendy|=1;
    NPCYNSnapshot(FALSE);
    CHECK(sNPCYN.error && !sNPCYN.easyChatLedgerMatched && sNPCYN.checkpointSerial==3);
    sNPCYN.error=0; fixtureEasy=sNPCYNExpectedEasyChat; ((u8 *)&fixtureParty)[0]^=1;
    NPCYNSnapshot(FALSE);
    CHECK(sNPCYN.error && !sNPCYN.partyUnchanged && sNPCYN.checkpointSerial==3);
    sNPCYN.error=0; ((u8 *)&fixtureParty)[0]^=1; ((u8 *)&fixturePC)[0]^=1;
    NPCYNSnapshot(FALSE);
    CHECK(sNPCYN.error && !sNPCYN.pcUnchanged && sNPCYN.checkpointSerial==3);
    sNPCYN.error=0; ((u8 *)&fixturePC)[0]^=1; ((u8 *)&fixtureBag)[0]^=1;
    NPCYNSnapshot(FALSE);
    CHECK(sNPCYN.error && !sNPCYN.bagUnchanged && sNPCYN.checkpointSerial==3);
    sNPCYN.error=0; ((u8 *)&fixtureBag)[0]^=1; ++fixtureProfile.money;
    NPCYNSnapshot(FALSE);
    CHECK(sNPCYN.error && !sNPCYN.moneyUnchanged && sNPCYN.checkpointSerial==3);
    sNPCYN.error=0; --fixtureProfile.money;
    NPCYNSnapshot(FALSE);
    CHECK(!sNPCYN.error && sNPCYN.checkpointSerial==4 && sNPCYN.conservationChecks==3);

    int ownerA,ownerB;
    HGYNBootFacts facts={.kind=HG_YN_BOOT_TITLE,.ready=1,.phase=2,.progressFlags=15};
    CHECK(sNPCYN.version==HG_YN_QA_VERSION && sNPCYN.byteSize==sizeof(sNPCYN));
    sNPCYNBootOwner=&ownerA; sNPCYN.boot.active=1; sNPCYN.boot.ownerGeneration=UINT32_MAX;
    sNPCYN.boot.revision=UINT32_MAX; sNPCYN.revision=UINT32_MAX;
    NPCYNBootPublish(&ownerA,&facts);
    CHECK(sNPCYN.boot.ownerGeneration==UINT32_MAX && sNPCYN.boot.revision==1 && sNPCYN.revision==1);
    sNPCYN.boot.ownerGeneration=UINT32_MAX; sNPCYN.boot.revision=UINT32_MAX; sNPCYN.revision=UINT32_MAX;
    NPCYNBootPublish(&ownerB,&facts);
    CHECK(sNPCYN.boot.ownerGeneration==1 && sNPCYN.boot.revision==1 && sNPCYN.revision==1);
    NPCYNBootRetire(&ownerA);
    CHECK(sNPCYN.boot.active==1);
    sNPCYN.boot.revision=UINT32_MAX; sNPCYN.revision=UINT32_MAX;
    NPCYNBootRetire(&ownerB);
    CHECK(!sNPCYN.boot.active && !sNPCYN.boot.ready && sNPCYN.boot.ownerGeneration==1 && sNPCYN.boot.revision==1 && sNPCYN.revision==1);
    NPCYNBootRetire(&ownerB);
    CHECK(sNPCYN.boot.revision==1 && sNPCYN.revision==1);
    return 0;
}

#else
// SPDX-License-Identifier: GPL-3.0-or-later
// Actual boot facts with pure local owner records; no game or platform objects.
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef int32_t fx32;
typedef int BOOL;

#define TRUE 1
#define FALSE 0

enum {
    GF_BG_LYR_MAIN_0 = 0,
    GF_BG_LYR_MAIN_1 = 1,
    GF_BG_LYR_MAIN_2 = 2,
    GF_BG_LYR_MAIN_3 = 3,
    GF_BG_LYR_SUB_0 = 4,
    GF_BG_LYR_SUB_1 = 5,
    GF_BG_LYR_SUB_2 = 6,
    GF_BG_LYR_SUB_3 = 7,
};

enum { TITLESCREEN_MAIN_PLAY = 2 };
enum { TITLESCREEN_EXIT_UNSET = 0, TITLESCREEN_EXIT_MENU = 1 };
enum { MENU_INPUT_STATE_BUTTONS = 0, MENU_INPUT_STATE_TOUCH = 1 };
enum { APPOPTION_TITLE_SCREEN = 0, APPOPTION_CONTINUE = 1, APPOPTION_COUNT = 9 };

typedef struct Background {
    void *tilemapBuffer;
} Background;

typedef struct BgConfig {
    u16 scrollScheduled;
    u16 bufferTransferScheduled;
    Background bgs[8];
} BgConfig;

typedef struct Window {
    BgConfig *bgConfig;
    void *pixelBuffer;
    u8 width;
    u8 height;
} Window;

typedef struct TitleScreenAnimData {
    Window window;
} TitleScreenAnimData;

typedef struct TitleScreenOverlayData {
    BgConfig *bgConfig;
    TitleScreenAnimData animData;
    u32 initialDelay;
    u16 exitMode;
} TitleScreenOverlayData;

typedef struct MenuInputStateMgr {
    int state;
} MenuInputStateMgr;

typedef struct MainMenuAppData {
    BgConfig *bgConfig;
    void *saveData;
    u16 currentOption;
    int selectedApp;
    Window optionWindows[APPOPTION_COUNT];
    u32 optionIds[APPOPTION_COUNT];
    fx32 currentScreenY;
    fx32 effectiveScreenY;
    u32 noticeState;
    u32 noticeDelay;
    BOOL dontHaveSavedata;
    BOOL noticePending;
    BOOL redrawOptions;
    u32 optionChangeTimer;
    MenuInputStateMgr menuInputState;
} MainMenuAppData;

static BOOL gFade;

BOOL IsPaletteFadeFinished(void) {
    return gFade;
}

#define NPCYN_BOOT_TITLE_TU
#include "npc_yesno_boot.inc"
#undef NPCYN_BOOT_TITLE_TU

#define NPCYN_BOOT_MAIN_MENU_TU
#include "npc_yesno_boot.inc"
#undef NPCYN_BOOT_MAIN_MENU_TU

static HGYNBootFacts published;
static const void *publishedOwner;
static u32 publishCount;
void NPCYNBootPublish(const void *owner, const HGYNBootFacts *facts) {
    publishedOwner=owner; published=*facts; published.active=1; ++publishCount;
}

void NPCYNBootRetire(const void *owner) {
    assert(owner==publishedOwner); published.active=published.ready=0;
}

static void TitleLive(TitleScreenOverlayData *data, BgConfig *bg, unsigned *pixel) {
    static const int layers[] = { 1, 2, 3, 5, 6, 7 };
    memset(data, 0, sizeof(*data));
    memset(bg, 0, sizeof(*bg));
    data->bgConfig = bg;
    data->animData.window.bgConfig = bg;
    data->animData.window.pixelBuffer = pixel;
    data->animData.window.width = 32;
    data->animData.window.height = 2;
    data->initialDelay = 0;
    data->exitMode = TITLESCREEN_EXIT_UNSET;
    for (u32 i = 0; i < sizeof(layers) / sizeof(layers[0]); ++i) {
        bg->bgs[layers[i]].tilemapBuffer = pixel;
    }
}

static void MainMenuLive(MainMenuAppData *data, BgConfig *bg, unsigned *pixel) {
    memset(data, 0, sizeof(*data));
    memset(bg, 0, sizeof(*bg));
    data->bgConfig = bg;
    data->saveData = pixel;
    data->currentOption = 0;
    data->optionIds[0] = APPOPTION_CONTINUE;
    data->optionWindows[0].bgConfig = bg;
    data->optionWindows[0].pixelBuffer = pixel;
    data->optionWindows[0].width = 23;
    data->optionWindows[0].height = 10;
    data->noticeState = 15;
    data->menuInputState.state = MENU_INPUT_STATE_BUTTONS;
    for (int i = 0; i < 3; ++i) {
        bg->bgs[i].tilemapBuffer = pixel;
    }
}

static void CheckTitlePending(TitleScreenOverlayData *data, BgConfig *bg, int layer) {
    bg->scrollScheduled = (u16)(1u << layer);
    assert(!NPCYNBootTitleFacts(data, TITLESCREEN_MAIN_PLAY).ready);
    bg->scrollScheduled = 0;
    bg->bufferTransferScheduled = (u16)(1u << layer);
    assert(!NPCYNBootTitleFacts(data, TITLESCREEN_MAIN_PLAY).ready);
    bg->bufferTransferScheduled = 0;
}

static void CheckMainMenuPending(MainMenuAppData *data, BgConfig *bg, int layer) {
    bg->scrollScheduled = (u16)(1u << layer);
    assert(!NPCYNBootMainMenuFacts(data, 5).ready);
    bg->scrollScheduled = 0;
    bg->bufferTransferScheduled = (u16)(1u << layer);
    assert(!NPCYNBootMainMenuFacts(data, 5).ready);
    bg->bufferTransferScheduled = 0;
}

int main(void) {
    static const int titleLayers[] = { 1, 2, 3, 5, 6, 7 };
    unsigned pixel = 0;
    BgConfig bg;
    TitleScreenOverlayData title;
    MainMenuAppData mainMenu;

    gFade = TRUE;
    TitleLive(&title, &bg, &pixel);
    assert(NPCYNBootTitleFacts(&title, TITLESCREEN_MAIN_PLAY).ready == 1);
    for (u32 i = 0; i < sizeof(titleLayers) / sizeof(titleLayers[0]); ++i) {
        CheckTitlePending(&title, &bg, titleLayers[i]);
    }
    for (u32 i = 0; i < sizeof(titleLayers) / sizeof(titleLayers[0]); ++i) {
        int layer = titleLayers[i];
        bg.bgs[layer].tilemapBuffer = NULL;
        assert(!NPCYNBootTitleFacts(&title, TITLESCREEN_MAIN_PLAY).ready);
        bg.bgs[layer].tilemapBuffer = &pixel;
    }
    title.animData.window.pixelBuffer = NULL;
    assert(!NPCYNBootTitleFacts(&title, TITLESCREEN_MAIN_PLAY).ready);
    title.animData.window.pixelBuffer = &pixel;
    title.animData.window.width = 0;
    assert(!NPCYNBootTitleFacts(&title, TITLESCREEN_MAIN_PLAY).ready);
    title.animData.window.width = 32;
    title.animData.window.height = 0;
    assert(!NPCYNBootTitleFacts(&title, TITLESCREEN_MAIN_PLAY).ready);
    title.animData.window.height = 2;
    gFade = FALSE;
    assert(!NPCYNBootTitleFacts(&title, TITLESCREEN_MAIN_PLAY).ready);

    gFade = TRUE;
    MainMenuLive(&mainMenu, &bg, &pixel);
    assert(NPCYNBootMainMenuFacts(&mainMenu, 5).ready == 1);
    for (int layer = 0; layer < 3; ++layer) {
        CheckMainMenuPending(&mainMenu, &bg, layer);
    }
    mainMenu.optionWindows[0].pixelBuffer = NULL;
    assert(!NPCYNBootMainMenuFacts(&mainMenu, 5).ready);
    mainMenu.optionWindows[0].pixelBuffer = &pixel;
    mainMenu.optionWindows[0].height = 0;
    assert(!NPCYNBootMainMenuFacts(&mainMenu, 5).ready);
    mainMenu.optionWindows[0].height = 10;
    gFade = FALSE;
    assert(!NPCYNBootMainMenuFacts(&mainMenu, 5).ready);
    gFade = TRUE;
    mainMenu.menuInputState.state = MENU_INPUT_STATE_TOUCH;
    assert(NPCYNBootMainMenuFacts(&mainMenu, 5).ready == 1);
    mainMenu.menuInputState.state = 2;
    assert(!NPCYNBootMainMenuFacts(&mainMenu, 5).ready);
    mainMenu.menuInputState.state = MENU_INPUT_STATE_BUTTONS;
    mainMenu.optionIds[0] = APPOPTION_TITLE_SCREEN;
    assert(!NPCYNBootMainMenuFacts(&mainMenu, 5).ready);
    mainMenu.optionIds[0] = APPOPTION_CONTINUE;
    mainMenu.currentOption = APPOPTION_COUNT;
    assert(!NPCYNBootMainMenuFacts(&mainMenu, 5).ready);
    mainMenu.currentOption = 0;
    mainMenu.noticePending = TRUE;
    assert(!NPCYNBootMainMenuFacts(&mainMenu, 5).ready);

    mainMenu.noticePending=FALSE; mainMenu.selectedApp=APPOPTION_CONTINUE;
    NPCYNBootPublishMainMenu(&mainMenu,8);
    assert(published.phase==8 && published.exit==1 && !published.ready);
    published.ownerGeneration=42;
    u32 count=publishCount;
    NPCYNBootPublishMainMenu(&mainMenu,7);
    assert(publishCount==count && published.phase==8);
    NPCYNBootMainMenuTerminal(&mainMenu,7);
    assert(!published.active && !published.ready && published.phase==8 && published.exit==1 && published.ownerGeneration==42);
    NPCYNBootPublishMainMenu(&mainMenu,5);
    NPCYNBootMainMenuTerminal(&mainMenu,7);
    assert(!published.active && !published.ready && published.phase==5 && published.phase!=8);
    puts("PASS: actual Title/Main readiness, invalid BG/window/fade/mode/option; retained original phase-8 exit");
    return 0;
}

#endif
