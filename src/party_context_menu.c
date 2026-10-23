#include "party_context_menu.h"

#include "global.h"

#include "msgdata/msg/msg_0300.h"

#include "font.h"
#include "party_menu_items.h"
#include "party_menu_list_items.h"
#include "render_text.h"
#include "system.h"
#include "text.h"
#include "sound_playback.h"
#include "message_printer_number.h"
#include "ui_util.h"

#ifdef HEARTGOLD_NATIVE
#include "native_party_ui.h"
#include "palette_vram_addr.h"
#include "screen_fade.h"
#include "sprite_system.h"

static void NativePartyUIContextCreated(PartyMenu *partyMenu, PartyMenuContextMenuCursor *cursor);
static void NativePartyUIContextClosed(PartyMenu *partyMenu);
static void NativePartyUISetMessage(PartyMenu *partyMenu, Window *window);
static void NativePartyUIPrinterUpdated(TextPrinterTemplate *template, u16 glyphId);
static void NativePartyUITopTextIds(PartyMenu *partyMenu, const u8 *items, u32 count);
#endif

static void PartyMenu_StartContextMenuButtonPressAnim_FromCursorObj(PartyMenu *partyMenu, PartyMenuContextMenuCursor *cursor, int followUpState);
static void addFiveWindows(BgConfig *bgConfig, Window *window, const WindowTemplate *template);
static void PartyMenu_DrawSlashOnMonHpTextWindow(PartyMenu *partyMenu, u8 partySlot);
static void PartyMenu_PrintMonNicknameOnWindow(PartyMenu *partyMenu, u8 partySlot);
static void PartyMenu_PrintMonMaxHpOnWindow(PartyMenu *partyMenu, u8 partySlot);
static u32 getStringCenterXPos(FontID fontId, String *string, u32 windowWidth);
static void PartyMenu_PrintMessageOnWindowEx(PartyMenu *partyMenu, Window *window, int msgId, BOOL drawFrame);
static BOOL TextPrinterCB_PartyMenuWin34Msg(TextPrinterTemplate *template, u16 glyphId);
static void PartyMenu_PrintEvoStoneCompatString(PartyMenu *partyMenu, u8 partySlot, u8 compatibilityParam);
static void PartyMenu_PrintTMHMCompatString(PartyMenu *partyMenu, u8 partySlot, u8 compatibilityParam);
static void PartyMenu_PrintSuperContestCompatString(PartyMenu *partyMenu, u8 partySlot, u8 isCompatible);
static void PartyMenu_PrintEntryStatusString(PartyMenu *partyMenu, u8 partySlot, u8 entryStatus);
static void PartyMenu_ShowContextMenu(PartyMenu *partyMenu, int numItems, int state);
static void PartyMenu_HideContextMenu(PartyMenu *partyMenu, int numItems, int state);
static u32 getButtonColorDepressed(int selection);
static u32 getButtonColorRaised(int selection);
static void PartyMenu_PrintContextMenuItemText(PartyMenu *partyMenu, PartyMenuContextMenu *contextMenu, int numItems, int selection, int state, BOOL depressed);
static void PartyMenu_PrintContextMenuItemsText(PartyMenu *partyMenu, PartyMenuContextMenu *contextMenu, int numItems, int selection, int state);
static void PartyMenu_DrawContextMenuButtonFrame(PartyMenu *partyMenu, int numItems, int selection, int state, int frameType);
static BOOL hitboxHasSelectionMapping(int state, int numItems, int selection);
static int hitboxToSelection(int state, int numItems, int selection);
static BOOL contextMenuButtonIsActive(int state, int numItems, int selection);
static BOOL handlePartyMenuTopLevelDpadInput(u8 *pSelection, int numItems, int direction);
static BOOL handlePartyContextMenuDpadInput(u8 *pSelection, int numItems, int direction);

static const WindowTemplate sMainWindowTemplates[] = {
    { GF_BG_LYR_MAIN_1, 0x06, 0x01, 0x09, 0x02, 0x00, 0x0048 },
    { GF_BG_LYR_MAIN_1, 0x00, 0x04, 0x06, 0x02, 0x00, 0x005A },
    { GF_BG_LYR_MAIN_1, 0x07, 0x04, 0x08, 0x02, 0x00, 0x0066 },
    { GF_BG_LYR_MAIN_1, 0x08, 0x03, 0x06, 0x01, 0x03, 0x0076 },
    { GF_BG_LYR_MAIN_1, 0x06, 0x04, 0x0A, 0x02, 0x00, 0x0066 },
    { GF_BG_LYR_MAIN_1, 0x16, 0x02, 0x09, 0x02, 0x00, 0x007C },
    { GF_BG_LYR_MAIN_1, 0x10, 0x05, 0x06, 0x02, 0x00, 0x008E },
    { GF_BG_LYR_MAIN_1, 0x17, 0x05, 0x08, 0x02, 0x00, 0x009A },
    { GF_BG_LYR_MAIN_1, 0x18, 0x04, 0x06, 0x01, 0x04, 0x00AA },
    { GF_BG_LYR_MAIN_1, 0x16, 0x05, 0x0A, 0x02, 0x00, 0x009A },
    { GF_BG_LYR_MAIN_1, 0x06, 0x07, 0x09, 0x02, 0x00, 0x00B0 },
    { GF_BG_LYR_MAIN_1, 0x00, 0x0A, 0x06, 0x02, 0x00, 0x00C2 },
    { GF_BG_LYR_MAIN_1, 0x07, 0x0A, 0x08, 0x02, 0x00, 0x00CE },
    { GF_BG_LYR_MAIN_1, 0x08, 0x09, 0x06, 0x01, 0x05, 0x00DE },
    { GF_BG_LYR_MAIN_1, 0x06, 0x0A, 0x0A, 0x02, 0x00, 0x00CE },
    { GF_BG_LYR_MAIN_1, 0x16, 0x08, 0x09, 0x02, 0x00, 0x00E4 },
    { GF_BG_LYR_MAIN_1, 0x10, 0x0B, 0x06, 0x02, 0x00, 0x00F6 },
    { GF_BG_LYR_MAIN_1, 0x17, 0x0B, 0x08, 0x02, 0x00, 0x0102 },
    { GF_BG_LYR_MAIN_1, 0x18, 0x0A, 0x06, 0x01, 0x06, 0x0112 },
    { GF_BG_LYR_MAIN_1, 0x16, 0x0B, 0x0A, 0x02, 0x00, 0x0102 },
    { GF_BG_LYR_MAIN_1, 0x06, 0x0D, 0x09, 0x02, 0x00, 0x0118 },
    { GF_BG_LYR_MAIN_1, 0x00, 0x10, 0x06, 0x02, 0x00, 0x012A },
    { GF_BG_LYR_MAIN_1, 0x07, 0x10, 0x08, 0x02, 0x00, 0x0136 },
    { GF_BG_LYR_MAIN_1, 0x08, 0x0F, 0x06, 0x01, 0x07, 0x0146 },
    { GF_BG_LYR_MAIN_1, 0x06, 0x10, 0x0A, 0x02, 0x00, 0x0136 },
    { GF_BG_LYR_MAIN_1, 0x16, 0x0E, 0x09, 0x02, 0x00, 0x014C },
    { GF_BG_LYR_MAIN_1, 0x10, 0x11, 0x06, 0x02, 0x00, 0x015E },
    { GF_BG_LYR_MAIN_1, 0x17, 0x11, 0x08, 0x02, 0x00, 0x016A },
    { GF_BG_LYR_MAIN_1, 0x18, 0x10, 0x06, 0x01, 0x08, 0x017A },
    { GF_BG_LYR_MAIN_1, 0x16, 0x11, 0x0A, 0x02, 0x00, 0x016A },
    { GF_BG_LYR_MAIN_1, 0x16, 0x01, 0x09, 0x02, 0x00, 0x007C },
    { GF_BG_LYR_MAIN_1, 0x11, 0x05, 0x05, 0x02, 0x00, 0x008E },
    { GF_BG_LYR_MAIN_1, 0x17, 0x05, 0x08, 0x02, 0x00, 0x009A },
    { GF_BG_LYR_MAIN_1, 0x18, 0x04, 0x06, 0x01, 0x04, 0x00AA },
    { GF_BG_LYR_MAIN_1, 0x18, 0x04, 0x06, 0x01, 0x04, 0x00AA },
    { GF_BG_LYR_MAIN_1, 0x16, 0x08, 0x09, 0x02, 0x00, 0x00E4 },
    { GF_BG_LYR_MAIN_1, 0x11, 0x0B, 0x05, 0x02, 0x00, 0x00F6 },
    { GF_BG_LYR_MAIN_1, 0x17, 0x0B, 0x08, 0x02, 0x00, 0x0102 },
    { GF_BG_LYR_MAIN_1, 0x18, 0x0A, 0x06, 0x01, 0x06, 0x0112 },
    { GF_BG_LYR_MAIN_1, 0x18, 0x04, 0x06, 0x01, 0x04, 0x00AA },
    { GF_BG_LYR_MAIN_1, 0x16, 0x0E, 0x09, 0x02, 0x00, 0x014C },
    { GF_BG_LYR_MAIN_1, 0x11, 0x11, 0x05, 0x02, 0x00, 0x015E },
    { GF_BG_LYR_MAIN_1, 0x17, 0x11, 0x08, 0x02, 0x00, 0x016A },
    { GF_BG_LYR_MAIN_1, 0x18, 0x10, 0x06, 0x01, 0x08, 0x017A },
    { GF_BG_LYR_MAIN_1, 0x18, 0x04, 0x06, 0x01, 0x04, 0x00AA },
    { GF_BG_LYR_SUB_2,  0x07, 0x18, 0x0A, 0x02, 0x02, 0x0001 },
    { GF_BG_LYR_SUB_2,  0x11, 0x18, 0x06, 0x02, 0x02, 0x0015 },
    { GF_BG_LYR_SUB_2,  0x11, 0x1A, 0x0C, 0x03, 0x02, 0x0021 },
};

static const WindowTemplate sAdditionalWindowTemplates[] = {
    { GF_BG_LYR_MAIN_1, 0x1A, 0x15, 0x05, 0x02, 0x00, 0x0180 },
    { GF_BG_LYR_MAIN_1, 0x19, 0x14, 0x07, 0x02, 0x00, 0x018A },
    { GF_BG_LYR_MAIN_0, 0x02, 0x15, 0x14, 0x02, 0x0D, 0x0198 },
    { GF_BG_LYR_MAIN_0, 0x02, 0x13, 0x0D, 0x04, 0x0D, 0x01C0 },
    { GF_BG_LYR_MAIN_0, 0x02, 0x13, 0x1B, 0x04, 0x0D, 0x01F4 },
    { GF_BG_LYR_MAIN_0, 0x13, 0x11, 0x0C, 0x06, 0x00, 0x0260 },
    { GF_BG_LYR_MAIN_0, 0x13, 0x0F, 0x0C, 0x08, 0x00, 0x0260 },
};

static const WindowTemplate sButtonWindowTemplates[] = {
    { GF_BG_LYR_MAIN_0, 0x11, 0x04, 0x0E, 0x02, 0x02, 0x0260 },
    { GF_BG_LYR_MAIN_0, 0x11, 0x08, 0x0E, 0x02, 0x02, 0x027C },
    { GF_BG_LYR_MAIN_0, 0x11, 0x0C, 0x0E, 0x02, 0x02, 0x0298 },
    { GF_BG_LYR_MAIN_0, 0x01, 0x03, 0x0E, 0x02, 0x02, 0x02B4 },
    { GF_BG_LYR_MAIN_0, 0x01, 0x07, 0x0E, 0x02, 0x02, 0x02D0 },
    { GF_BG_LYR_MAIN_0, 0x01, 0x0B, 0x0E, 0x02, 0x02, 0x02EC },
    { GF_BG_LYR_MAIN_0, 0x01, 0x0F, 0x0E, 0x02, 0x02, 0x0308 },
    { GF_BG_LYR_MAIN_0, 0x1A, 0x14, 0x05, 0x03, 0x02, 0x0324 },
    { GF_BG_LYR_MAIN_0, 0x11, 0x03, 0x0E, 0x02, 0x02, 0x0260 },
    { GF_BG_LYR_MAIN_0, 0x11, 0x07, 0x0E, 0x02, 0x02, 0x027C },
    { GF_BG_LYR_MAIN_0, 0x11, 0x0B, 0x0E, 0x02, 0x02, 0x0298 },
    { GF_BG_LYR_MAIN_0, 0x11, 0x0F, 0x0E, 0x02, 0x02, 0x02B4 },
};

static const u8 sButtonRects[][4] = {
    // x, y, width, height
    { 0x10, 0x03, 0x10, 0x04 },
    { 0x10, 0x07, 0x10, 0x04 },
    { 0x10, 0x0B, 0x10, 0x04 },
    { 0x00, 0x02, 0x10, 0x04 },
    { 0x00, 0x06, 0x10, 0x04 },
    { 0x00, 0x0A, 0x10, 0x04 },
    { 0x00, 0x0E, 0x10, 0x04 },
    { 0x19, 0x13, 0x07, 0x05 },
    { 0x10, 0x02, 0x10, 0x04 },
    { 0x10, 0x06, 0x10, 0x04 },
    { 0x10, 0x0A, 0x10, 0x04 },
    { 0x10, 0x0E, 0x10, 0x04 },
};

static const s8 sButtonWindowIDs[][2][8] = {
    {
     { 0, -1, -1, -1, -1, -1, -1, -1 },
     { 7, -1, -1, -1, -1, -1, -1, -1 },
     },
    {
     { 0, 7, -1, -1, -1, -1, -1, -1 },
     { 8, 7, -1, -1, -1, -1, -1, -1 },
     },
    {
     { 0, 1, 7, -1, -1, -1, -1, -1 },
     { 8, 9, 7, -1, -1, -1, -1, -1 },
     },
    {
     { 0, 1, 2, 7, -1, -1, -1, -1 },
     { 8, 9, 10, 7, -1, -1, -1, -1 },
     },
    {
     { 0, 1, 2, 7, 3, -1, -1, -1 },
     { 8, 9, 10, 11, 7, -1, -1, -1 },
     },
    {
     { 0, 1, 2, 7, 3, 4, -1, -1 },
     { 0, 1, 2, 3, 4, 8, -1, -1 },
     },
    {
     { 0, 1, 2, 7, 3, 4, 5, -1 },
     { 0, 1, 2, 3, 4, 5, 8, -1 },
     },
    {
     { 0, 1, 2, 7, 3, 4, 5, 6 },
     { 0, 1, 2, 7, 3, 4, 5, 6 },
     },
};

static const u16 sButtonFrameTileOffsets[] = {
    0, 2, 6, 8, 3, 5, 1, 7
};

// only the first column is used and they all
// reference the MessageFormat field 0
// which derives from BufferMonNickname.
// they could've just gotten MON_DATA_NICKNAME_STRING
static const u16 sMonNicknameMsgIds[][4] = {
    { msg_0300_00008, msg_0300_00002, msg_0300_00020, msg_0300_00014 },
    { msg_0300_00009, msg_0300_00003, msg_0300_00021, msg_0300_00015 },
    { msg_0300_00010, msg_0300_00004, msg_0300_00022, msg_0300_00016 },
    { msg_0300_00011, msg_0300_00005, msg_0300_00023, msg_0300_00017 },
    { msg_0300_00012, msg_0300_00006, msg_0300_00024, msg_0300_00018 },
    { msg_0300_00013, msg_0300_00007, msg_0300_00025, msg_0300_00019 },
};

static int sDpadNavParam_ContextMenu[][5][2] = {
    {
     { 1, 1 },
     { 0, 0 },
     { -1, -1 },
     { -1, -1 },
     { -1, -1 },
     },
    {
     { 2, 1 },
     { 0, 2 },
     { 1, 0 },
     { -1, -1 },
     { -1, -1 },
     },
    {
     { 3, 1 },
     { 0, 2 },
     { 1, 3 },
     { 2, 0 },
     { -1, -1 },
     },
    {
     { 4, 1 },
     { 0, 2 },
     { 1, 3 },
     { 2, 4 },
     { 3, 0 },
     },
};

static int sDpadNavParam_PartyMenu[][8][3] = {
    {
     { 1, 1, -1 },
     { 0, 0, -1 },
     { -1, -1, -1 },
     { -1, -1, -1 },
     { -1, -1, -1 },
     { -1, -1, -1 },
     { -1, -1, -1 },
     { -1, -1, -1 },
     },
    {
     { 2, 1, -1 },
     { 0, 2, -1 },
     { 1, 0, -1 },
     { -1, -1, -1 },
     { -1, -1, -1 },
     { -1, -1, -1 },
     { -1, -1, -1 },
     { 2, 0, -1 },
     },
    {
     { 3, 1, -1 },
     { 0, 2, -1 },
     { 1, 3, -1 },
     { 2, 0, -1 },
     { -1, -1, -1 },
     { -1, -1, -1 },
     { -1, -1, -1 },
     { 2, 0, -1 },
     },
    {
     { 3, 1, 4 },
     { 0, 2, 4 },
     { 1, 3, 4 },
     { 2, 0, -1 },
     { -1, -1, 0 },
     { -1, -1, -1 },
     { -1, -1, -1 },
     { 2, 0, -1 },
     },
    {
     { 3, 1, 4 },
     { 0, 2, 5 },
     { 1, 3, 5 },
     { 2, 0, -1 },
     { 5, 5, 0 },
     { 4, 4, 1 },
     { -1, -1, -1 },
     { 2, 0, -1 },
     },
    {
     { 3, 1, 4 },
     { 0, 2, 5 },
     { 1, 3, 6 },
     { 2, 0, -1 },
     { 6, 5, 0 },
     { 4, 6, 1 },
     { 5, 4, 2 },
     { 2, 0, -1 },
     },
    {
     { 3, 1, 4 },
     { 0, 2, 5 },
     { 1, 3, 6 },
     { 2, 0, -1 },
     { 7, 5, 0 },
     { 4, 6, 1 },
     { 5, 7, 2 },
     { 6, 4, 2 },
     },
};

static TouchscreenHitbox sHitboxes[] = {
    { .rect = { 0x18, 0x38, 0x80, 0x00 } },
    { .rect = { 0x38, 0x58, 0x80, 0x00 } },
    { .rect = { 0x58, 0x78, 0x80, 0x00 } },
    { .rect = { 0x10, 0x30, 0x00, 0x80 } },
    { .rect = { 0x30, 0x50, 0x00, 0x80 } },
    { .rect = { 0x50, 0x70, 0x00, 0x80 } },
    { .rect = { 0x70, 0x90, 0x00, 0x80 } },
    { .rect = { 0x98, 0xC0, 0xC8, 0x00 } },
    { { TOUCHSCREEN_RECTLIST_END } },
};

static TouchscreenHitbox sContextMenuHitboxes[] = {
    { .rect = { 0x10, 0x30, 0x80, 0x00 } },
    { .rect = { 0x30, 0x50, 0x80, 0x00 } },
    { .rect = { 0x50, 0x70, 0x80, 0x00 } },
    { .rect = { 0x70, 0x90, 0x80, 0x00 } },
    { .rect = { 0x98, 0xC0, 0xC8, 0x00 } },
    { { TOUCHSCREEN_RECTLIST_END } },
};

void PartyMenu_SetBlendBrightness_ForYesNo(void) {
    G2_SetBlendBrightness(30, 8);
}

void PartyMenu_DisableMainScreenBlend_AfterYesNo(void) {
    G2_BlendNone();
}

static void PartyMenu_StartContextMenuButtonPressAnim_FromCursorObj(PartyMenu *partyMenu, PartyMenuContextMenuCursor *cursor, int followUpState) {
    PartyMenuContextButtonAnimData *animData = &partyMenu->contextMenuButtonAnim;

    animData->autoAnimTimer = 0;
    animData->buttonAnimState = 0;
    animData->template = &cursor->menu;
    animData->numItems = cursor->numItems;
    animData->selection = cursor->selection;
    animData->state = cursor->state;
    animData->followUpState = followUpState;
    animData->active = TRUE;
}

void PartyMenu_StartContextMenuButtonAnim(PartyMenu *partyMenu, int selection, int followUpState, BOOL restartAnim) {
    PartyMenuContextButtonAnimData *animData = &partyMenu->contextMenuButtonAnim;

    animData->autoAnimTimer = 0;
    animData->buttonAnimState = 0;
    animData->template = NULL;
    animData->selection = selection;
    animData->state = Sprite_GetAnimationNumber(partyMenu->sprites[selection]) & 2;
    if (restartAnim == TRUE) {
        Sprite_SetAnimationFrame(partyMenu->sprites[selection], 0);
        Sprite_SetAnimCtrlSeq(partyMenu->sprites[selection], animData->state);
    }
    animData->followUpState = followUpState;
    animData->active = TRUE;
}

BOOL PartyMenu_AnimateContextMenuButtonPress(PartyMenu *partyMenu) {
    PartyMenuContextButtonAnimData *animData = &partyMenu->contextMenuButtonAnim;

    switch (animData->buttonAnimState) {
    case 0:
        if (animData->template != NULL) {
            PartyMenu_DrawContextMenuButtonFrame(partyMenu, animData->numItems, animData->selection, animData->state, 2);
            PartyMenu_PrintContextMenuItemText(partyMenu, animData->template, animData->numItems, animData->selection, animData->state, FALSE);
            BgCommitTilemapBufferToVram(partyMenu->bgConfig, GF_BG_LYR_MAIN_0);
        } else {
            Sprite_SetAnimationFrame(partyMenu->sprites[animData->selection], 0);
            Sprite_SetAnimCtrlSeq(partyMenu->sprites[animData->selection], animData->state);
        }
        ++animData->buttonAnimState;
        break;
    case 1:
        ++animData->autoAnimTimer;
        if (animData->autoAnimTimer == 2) {
            if (animData->template != NULL) {
                PartyMenu_DrawContextMenuButtonFrame(partyMenu, animData->numItems, animData->selection, animData->state, 1);
                PartyMenu_PrintContextMenuItemText(partyMenu, animData->template, animData->numItems, animData->selection, animData->state, TRUE);
                BgCommitTilemapBufferToVram(partyMenu->bgConfig, GF_BG_LYR_MAIN_0);
            } else {
                Sprite_SetAnimationFrame(partyMenu->sprites[animData->selection], 0);
                Sprite_SetAnimCtrlSeq(partyMenu->sprites[animData->selection], animData->state + 1);
            }
            animData->autoAnimTimer = 0;
            ++animData->buttonAnimState;
        }
        break;
    case 2:
        ++animData->autoAnimTimer;
        if (animData->autoAnimTimer == 2) {
            animData->active = FALSE;
            return FALSE;
        }
        break;
    }

    return TRUE;
}

static void addFiveWindows(BgConfig *bgConfig, Window *window, const WindowTemplate *template) {
    AddWindow(bgConfig, &window[0], &template[0]);
    AddWindow(bgConfig, &window[1], &template[1]);
    AddWindow(bgConfig, &window[2], &template[2]);
    AddWindow(bgConfig, &window[3], &template[3]);
    AddWindow(bgConfig, &window[4], &template[4]);
}

void PartyMenu_AddAllWindows(PartyMenu *partyMenu) {
    const WindowTemplate *templates = sMainWindowTemplates;
    addFiveWindows(partyMenu->bgConfig, &partyMenu->windows[PARTY_MENU_WINDOW_ID_MON1_NICKNAME], &templates[0]);
    addFiveWindows(partyMenu->bgConfig, &partyMenu->windows[PARTY_MENU_WINDOW_ID_MON3_NICKNAME], &templates[10]);
    addFiveWindows(partyMenu->bgConfig, &partyMenu->windows[PARTY_MENU_WINDOW_ID_MON5_NICKNAME], &templates[20]);
    if (partyMenu->args->panelLayout == 2) {
        addFiveWindows(partyMenu->bgConfig, &partyMenu->windows[PARTY_MENU_WINDOW_ID_MON2_NICKNAME], &templates[30]);
        addFiveWindows(partyMenu->bgConfig, &partyMenu->windows[PARTY_MENU_WINDOW_ID_MON4_NICKNAME], &templates[35]);
        addFiveWindows(partyMenu->bgConfig, &partyMenu->windows[PARTY_MENU_WINDOW_ID_MON6_NICKNAME], &templates[40]);
    } else {
        addFiveWindows(partyMenu->bgConfig, &partyMenu->windows[PARTY_MENU_WINDOW_ID_MON2_NICKNAME], &templates[5]);
        addFiveWindows(partyMenu->bgConfig, &partyMenu->windows[PARTY_MENU_WINDOW_ID_MON4_NICKNAME], &templates[15]);
        addFiveWindows(partyMenu->bgConfig, &partyMenu->windows[PARTY_MENU_WINDOW_ID_MON6_NICKNAME], &templates[25]);
    }
    AddWindow(partyMenu->bgConfig, &partyMenu->windows[PARTY_MENU_WINDOW_ID_37], &templates[45]);
    AddWindow(partyMenu->bgConfig, &partyMenu->windows[PARTY_MENU_WINDOW_ID_38], &templates[46]);
    AddWindow(partyMenu->bgConfig, &partyMenu->windows[PARTY_MENU_WINDOW_ID_39], &templates[47]);
    if (partyMenu->args->context == PARTY_MENU_CONTEXT_UNION_ROOM_BATTLE_SELECT || partyMenu->args->context == PARTY_MENU_CONTEXT_17 || partyMenu->args->context == PARTY_MENU_CONTEXT_BATTLE_HALL || partyMenu->args->context == PARTY_MENU_CONTEXT_23) {
        WindowTemplate template = sAdditionalWindowTemplates[0];
        template.top = 22;
        AddWindow(partyMenu->bgConfig, &partyMenu->windows[PARTY_MENU_WINDOW_ID_31], &template);
    } else {
        AddWindow(partyMenu->bgConfig, &partyMenu->windows[PARTY_MENU_WINDOW_ID_31], &sAdditionalWindowTemplates[0]);
    }
    AddWindow(partyMenu->bgConfig, &partyMenu->windows[PARTY_MENU_WINDOW_ID_30], &sAdditionalWindowTemplates[1]);
    AddWindow(partyMenu->bgConfig, &partyMenu->windows[PARTY_MENU_WINDOW_ID_32], &sAdditionalWindowTemplates[2]);
    AddWindow(partyMenu->bgConfig, &partyMenu->windows[PARTY_MENU_WINDOW_ID_33], &sAdditionalWindowTemplates[3]);
    AddWindow(partyMenu->bgConfig, &partyMenu->windows[PARTY_MENU_WINDOW_ID_34], &sAdditionalWindowTemplates[4]);
    AddWindow(partyMenu->bgConfig, &partyMenu->windows[PARTY_MENU_WINDOW_ID_35], &sAdditionalWindowTemplates[5]);
    AddWindow(partyMenu->bgConfig, &partyMenu->windows[PARTY_MENU_WINDOW_ID_36], &sAdditionalWindowTemplates[6]);
}

void PartyMenu_RemoveAllWindows(PartyMenu *partyMenu) {
    u16 i;
    for (i = 0; i < PARTY_MENU_WINDOW_ID_MAX; ++i) {
        RemoveWindow(&partyMenu->windows[i]);
    }

    for (i = 0; i < 1; ++i) {
        Window *win = &partyMenu->levelUpStatsWindow[i];
        if (WindowIsInUse(win) == TRUE) {
            RemoveWindow(win);
        }
    }
}

void PartyMenu_SetContextMenuStaticStrings(PartyMenu *partyMenu) {
    ReadMsgDataIntoString(partyMenu->msgData, msg_0300_00128, partyMenu->contextMenuStrings[PARTY_MON_CONTEXT_MENU_SWITCH]);
    ReadMsgDataIntoString(partyMenu->msgData, msg_0300_00129, partyMenu->contextMenuStrings[PARTY_MON_CONTEXT_MENU_SUMMARY]);
    ReadMsgDataIntoString(partyMenu->msgData, msg_0300_00130, partyMenu->contextMenuStrings[PARTY_MON_CONTEXT_MENU_ITEM]);
    ReadMsgDataIntoString(partyMenu->msgData, msg_0300_00143, partyMenu->contextMenuStrings[PARTY_MON_CONTEXT_MENU_GIVE]);
    ReadMsgDataIntoString(partyMenu->msgData, msg_0300_00144, partyMenu->contextMenuStrings[PARTY_MON_CONTEXT_MENU_TAKE]);
    ReadMsgDataIntoString(partyMenu->msgData, msg_0300_00131, partyMenu->contextMenuStrings[PARTY_MON_CONTEXT_MENU_MAIL]);
    ReadMsgDataIntoString(partyMenu->msgData, msg_0300_00132, partyMenu->contextMenuStrings[PARTY_MON_CONTEXT_MENU_READ_MAIL]);
    ReadMsgDataIntoString(partyMenu->msgData, msg_0300_00133, partyMenu->contextMenuStrings[PARTY_MON_CONTEXT_MENU_TAKE_MAIL]);
    ReadMsgDataIntoString(partyMenu->msgData, msg_0300_00134, partyMenu->contextMenuStrings[PARTY_MON_CONTEXT_MENU_STORE]);
    ReadMsgDataIntoString(partyMenu->msgData, msg_0300_00135, partyMenu->contextMenuStrings[PARTY_MON_CONTEXT_MENU_QUIT]);
    ReadMsgDataIntoString(partyMenu->msgData, msg_0300_00136, partyMenu->contextMenuStrings[PARTY_MON_CONTEXT_MENU_UNUSED]);
    ReadMsgDataIntoString(partyMenu->msgData, msg_0300_00137, partyMenu->contextMenuStrings[PARTY_MON_CONTEXT_MENU_ENTER]);
    ReadMsgDataIntoString(partyMenu->msgData, msg_0300_00138, partyMenu->contextMenuStrings[PARTY_MON_CONTEXT_MENU_NO_ENTRY]);
    ReadMsgDataIntoString(partyMenu->msgData, msg_0300_00137, partyMenu->contextMenuStrings[PARTY_MON_CONTEXT_MENU_CONTEST_ENTER]);
    ReadMsgDataIntoString(partyMenu->msgData, msg_0300_00149, partyMenu->contextMenuStrings[PARTY_MON_CONTEXT_MENU_SET]);
    ReadMsgDataIntoString(partyMenu->msgData, msg_0300_00186, partyMenu->contextMenuStrings[PARTY_MON_CONTEXT_MENU_CONFIRM]);
}

void PartyMenu_ContextMenuAddFieldMove(PartyMenu *partyMenu, u16 move, u8 index) {
    String *msg = NewString_ReadMsgData(partyMenu->msgData, msg_0300_00139 + index);
    BufferMoveName(partyMenu->msgFormat, 0, move);
    StringExpandPlaceholders(partyMenu->msgFormat, partyMenu->contextMenuStrings[PARTY_MON_CONTEXT_MENU_FIELD_MOVES_BEGIN + index], msg);
    String_Delete(msg);
}

void PartyMenu_OpenContextMenu(PartyMenu *partyMenu, u8 *items, u8 numItems) {
    PartyMenuContextMenu contextMenu;
    u16 i, numFieldMoves;

    partyMenu->listMenuItems = ListMenuItems_New(numItems, HEAP_ID_PARTY_MENU);
    numFieldMoves = 0;
    for (i = 0; i < numItems; ++i) {
        if (items[i] >= PARTY_MON_CONTEXT_MENU_FIELD_MOVES_BEGIN) {
            ListMenuItems_AddItem(partyMenu->listMenuItems, partyMenu->contextMenuStrings[PARTY_MON_CONTEXT_MENU_FIELD_MOVES_BEGIN + numFieldMoves], GetPartyMenuContextMenuActionFunc(items[i]));
            ++numFieldMoves;
        } else {
            ListMenuItems_AddItem(partyMenu->listMenuItems, partyMenu->contextMenuStrings[items[i]], GetPartyMenuContextMenuActionFunc(items[i]));
        }
    }

    contextMenu.items = partyMenu->listMenuItems;
    contextMenu.window = &partyMenu->levelUpStatsWindow[0];
    contextMenu.unk_08 = 0;
    contextMenu.unk_09 = 1;
    contextMenu.numItems = numItems;
    contextMenu.unk_0B_0 = 0;
    contextMenu.unk_0B_4 = 0;
    if (numItems >= 4) {
        contextMenu.scrollEnabled = 1;
    } else {
        contextMenu.scrollEnabled = 0;
    }
    PartyMenu_DrawContextMenuButtonFrames(partyMenu, contextMenu.numItems, 0, 0);
    partyMenu->contextMenuCursor = PartyMenu_CreateContextMenuCursor(partyMenu, &contextMenu, 0, HEAP_ID_PARTY_MENU, 0);
#ifdef HEARTGOLD_NATIVE
    NativePartyUITopTextIds(partyMenu, items, numItems);
#endif
}

void PartyMenu_BufferDoWhatWithMonMessage(PartyMenu *partyMenu) {
    if (partyMenu->args->context == PARTY_MENU_CONTEXT_ATTACH_CAPSULE) {
        ReadMsgDataIntoString(partyMenu->msgData, msg_0300_00043, partyMenu->formattedStrBuf);
    } else if (partyMenu->args->context == PARTY_MENU_CONTEXT_SPIN_TRADE && partyMenu->monsDrawState[partyMenu->partyMonIndex].isEgg == TRUE) {
        ReadMsgDataIntoString(partyMenu->msgData, msg_0300_00183, partyMenu->formattedStrBuf);
    } else {
        Pokemon *mon = Party_GetMonByIndex(partyMenu->args->party, partyMenu->partyMonIndex);
        String *msg = NewString_ReadMsgData(partyMenu->msgData, msg_0300_00038);
        BufferBoxMonNickname(partyMenu->msgFormat, 0, Mon_GetBoxMon(mon));
        StringExpandPlaceholders(partyMenu->msgFormat, partyMenu->formattedStrBuf, msg);
        String_Delete(msg);
    }
}

static void PartyMenu_DrawSlashOnMonHpTextWindow(PartyMenu *partyMenu, u8 partySlot) {
    MessagePrinter_PrintGlyph(partyMenu->msgPrinter, 0, &partyMenu->windows[partySlot * PARTY_MENU_WINDOWS_PER_MON + PARTY_MENU_WINDOW_ID_MON1_HPTEXT], 28, 2);
}

void PartyMenu_BufferMonNickname(PartyMenu *partyMenu, Pokemon *mon, u32 partySlot) {
    String *msg = NewString_ReadMsgData(partyMenu->msgData, sMonNicknameMsgIds[partySlot][0]);
    BufferBoxMonNickname(partyMenu->msgFormat, 0, Mon_GetBoxMon(mon));
    StringExpandPlaceholders(partyMenu->msgFormat, partyMenu->monsDrawState[partySlot].nickname, msg);
    String_Delete(msg);
}

static void PartyMenu_PrintMonNicknameOnWindow(PartyMenu *partyMenu, u8 partySlot) {
    Window *window = &partyMenu->windows[partySlot * PARTY_MENU_WINDOWS_PER_MON + PARTY_MENU_WINDOW_ID_MON1_NICKNAME];

    AddTextPrinterParameterizedWithColor(window, 0, partyMenu->monsDrawState[partySlot].nickname, 0, 0, TEXT_SPEED_NOTRANSFER, MAKE_TEXT_COLOR(15, 14, 0), NULL);
    if (partyMenu->monsDrawState[partySlot].isEgg == TRUE) {
        ScheduleWindowCopyToVram(window);
    } else {
        FillWindowPixelBuffer(&partyMenu->windows[PARTY_MENU_WINDOW_ID_38], 0);
        AddTextPrinterParameterizedWithColor(&partyMenu->windows[PARTY_MENU_WINDOW_ID_38], 0, partyMenu->contextMenuStrings[PARTY_MON_CONTEXT_MENU_ITEM], 2, 0, TEXT_SPEED_INSTANT, MAKE_TEXT_COLOR(15, 14, 0), NULL);
        if (!partyMenu->monsDrawState[partySlot].dontPrintGenderSymbol) {
            if (partyMenu->monsDrawState[partySlot].gender == MON_MALE) {
                ReadMsgDataIntoString(partyMenu->msgData, msg_0300_00027, partyMenu->unformattedStrBuf);
                AddTextPrinterParameterizedWithColor(window, 0, partyMenu->unformattedStrBuf, 64, 0, TEXT_SPEED_NOTRANSFER, MAKE_TEXT_COLOR(3, 4, 0), NULL);
            } else if (partyMenu->monsDrawState[partySlot].gender == MON_FEMALE) {
                ReadMsgDataIntoString(partyMenu->msgData, msg_0300_00028, partyMenu->unformattedStrBuf);
                AddTextPrinterParameterizedWithColor(window, 0, partyMenu->unformattedStrBuf, 64, 0, TEXT_SPEED_NOTRANSFER, MAKE_TEXT_COLOR(5, 6, 0), NULL);
            }
        }
        ScheduleWindowCopyToVram(window);
    }
}

void PartyMenu_PrintMonLevelOnWindow(PartyMenu *partyMenu, u8 partySlot) {
    Window *window = &partyMenu->windows[partySlot * PARTY_MENU_WINDOWS_PER_MON + PARTY_MENU_WINDOW_ID_MON1_LEVEL];

    if (partyMenu->monsDrawState[partySlot].status != PARTY_MON_STATUS_ICON_OK) {
        ScheduleWindowCopyToVram(window);
    } else {
        MessagePrinter_PrintGlyphAndUInt(partyMenu->msgPrinter, 1, partyMenu->monsDrawState[partySlot].level, 3, PRINTING_MODE_LEFT_ALIGN, window, 5, 2);
        ScheduleWindowCopyToVram(window);
    }
}

void PartyMenu_PrintMonCurHpOnWindow(PartyMenu *partyMenu, u8 partySlot) {
    Window *window = &partyMenu->windows[partySlot * PARTY_MENU_WINDOWS_PER_MON + PARTY_MENU_WINDOW_ID_MON1_HPTEXT];

    PrintUIntOnWindow(partyMenu->msgPrinter, partyMenu->monsDrawState[partySlot].hp, 3, PRINTING_MODE_RIGHT_ALIGN, window, 0, 2);
    ScheduleWindowCopyToVram(window);
}

void PartyMenu_ClearMonHpTextWindow(PartyMenu *partyMenu, u8 partySlot) {
    Window *window = &partyMenu->windows[partySlot * PARTY_MENU_WINDOWS_PER_MON + PARTY_MENU_WINDOW_ID_MON1_HPTEXT];

    FillWindowPixelRect(window, 0, 0, 0, 24, 16);
    ScheduleWindowCopyToVram(window);
}

static void PartyMenu_PrintMonMaxHpOnWindow(PartyMenu *partyMenu, u8 partySlot) {
    Window *window = &partyMenu->windows[partySlot * PARTY_MENU_WINDOWS_PER_MON + PARTY_MENU_WINDOW_ID_MON1_HPTEXT];

    PrintUIntOnWindow(partyMenu->msgPrinter, partyMenu->monsDrawState[partySlot].maxHp, 3, PRINTING_MODE_LEFT_ALIGN, window, 36, 2);
    ScheduleWindowCopyToVram(window);
}

void PartyMenu_DrawMonHpBarOnWindow(PartyMenu *partyMenu, u8 partySlot) {
    Window *window = &partyMenu->windows[partySlot * PARTY_MENU_WINDOWS_PER_MON + PARTY_MENU_WINDOW_ID_MON1_HPBAR];

    switch (CalculateHpBarColor(partyMenu->monsDrawState[partySlot].hp, partyMenu->monsDrawState[partySlot].maxHp, 48)) {
    case 0:
        ScheduleWindowCopyToVram(window);
        return;
    case 4:
    case 3:
        BG_LoadPlttData(1, &partyMenu->hpBarPalettes[9], 4, (window->paletteNum * 16 + 9) * sizeof(u16));
        break;
    case 2:
        BG_LoadPlttData(1, &partyMenu->hpBarPalettes[16 + 9], 4, (window->paletteNum * 16 + 9) * sizeof(u16));
        break;
    case 1:
        BG_LoadPlttData(1, &partyMenu->hpBarPalettes[32 + 9], 4, (window->paletteNum * 16 + 9) * sizeof(u16));
        break;
    }

    u8 barLength = CalculateHpBarPixelsLength(partyMenu->monsDrawState[partySlot].hp, partyMenu->monsDrawState[partySlot].maxHp, 48);
    FillWindowPixelRect(window, 10, 0, 2, barLength, 1);
    FillWindowPixelRect(window, 9, 0, 3, barLength, 2);
    FillWindowPixelRect(window, 10, 0, 5, barLength, 1);
    ScheduleWindowCopyToVram(window);
}

void PartyMenu_DrawPartyMonsList_Default(PartyMenu *partyMenu, u8 partySlot) {
    FillWindowPixelBuffer(&partyMenu->windows[partySlot * PARTY_MENU_WINDOWS_PER_MON + PARTY_MENU_WINDOW_ID_MON1_NICKNAME], 0);
    FillWindowPixelBuffer(&partyMenu->windows[partySlot * PARTY_MENU_WINDOWS_PER_MON + PARTY_MENU_WINDOW_ID_MON1_LEVEL], 0);
    FillWindowPixelBuffer(&partyMenu->windows[partySlot * PARTY_MENU_WINDOWS_PER_MON + PARTY_MENU_WINDOW_ID_MON1_HPTEXT], 0);
    FillWindowPixelBuffer(&partyMenu->windows[partySlot * PARTY_MENU_WINDOWS_PER_MON + PARTY_MENU_WINDOW_ID_MON1_HPBAR], 0);
    PartyMenu_PrintMonNicknameOnWindow(partyMenu, partySlot);
    if (partyMenu->monsDrawState[partySlot].isEgg == TRUE) {
        ScheduleWindowCopyToVram(&partyMenu->windows[partySlot * PARTY_MENU_WINDOWS_PER_MON + PARTY_MENU_WINDOW_ID_MON1_NICKNAME]);
        ScheduleWindowCopyToVram(&partyMenu->windows[partySlot * PARTY_MENU_WINDOWS_PER_MON + PARTY_MENU_WINDOW_ID_MON1_LEVEL]);
        ScheduleWindowCopyToVram(&partyMenu->windows[partySlot * PARTY_MENU_WINDOWS_PER_MON + PARTY_MENU_WINDOW_ID_MON1_HPTEXT]);
        ScheduleWindowCopyToVram(&partyMenu->windows[partySlot * PARTY_MENU_WINDOWS_PER_MON + PARTY_MENU_WINDOW_ID_MON1_HPBAR]);
    } else {
        PartyMenu_DrawSlashOnMonHpTextWindow(partyMenu, partySlot);
        PartyMenu_PrintMonLevelOnWindow(partyMenu, partySlot);
        PartyMenu_PrintMonCurHpOnWindow(partyMenu, partySlot);
        PartyMenu_PrintMonMaxHpOnWindow(partyMenu, partySlot);
        PartyMenu_DrawMonHpBarOnWindow(partyMenu, partySlot);
    }
}

void PartyMenu_CommitPartyMonPanelWindowsToVram_InVBlank(PartyMenu *partyMenu, u8 partySlot) {
    CopyWindowPixelsToVram_TextMode(&partyMenu->windows[partySlot * PARTY_MENU_WINDOWS_PER_MON + PARTY_MENU_WINDOW_ID_MON1_NICKNAME]);
    CopyWindowPixelsToVram_TextMode(&partyMenu->windows[partySlot * PARTY_MENU_WINDOWS_PER_MON + PARTY_MENU_WINDOW_ID_MON1_LEVEL]);
    CopyWindowPixelsToVram_TextMode(&partyMenu->windows[partySlot * PARTY_MENU_WINDOWS_PER_MON + PARTY_MENU_WINDOW_ID_MON1_HPTEXT]);
    CopyWindowPixelsToVram_TextMode(&partyMenu->windows[partySlot * PARTY_MENU_WINDOWS_PER_MON + PARTY_MENU_WINDOW_ID_MON1_HPBAR]);
}

void PartyMenu_CommitPartyMonPanelWindowsToVram_NotInVBlank(PartyMenu *partyMenu, u8 partySlot) {
    ClearWindowTilemapAndScheduleTransfer(&partyMenu->windows[partySlot * PARTY_MENU_WINDOWS_PER_MON + PARTY_MENU_WINDOW_ID_MON1_NICKNAME]);
    ClearWindowTilemapAndScheduleTransfer(&partyMenu->windows[partySlot * PARTY_MENU_WINDOWS_PER_MON + PARTY_MENU_WINDOW_ID_MON1_LEVEL]);
    ClearWindowTilemapAndScheduleTransfer(&partyMenu->windows[partySlot * PARTY_MENU_WINDOWS_PER_MON + PARTY_MENU_WINDOW_ID_MON1_HPTEXT]);
    ClearWindowTilemapAndScheduleTransfer(&partyMenu->windows[partySlot * PARTY_MENU_WINDOWS_PER_MON + PARTY_MENU_WINDOW_ID_MON1_HPBAR]);
}

void PartyMenu_DrawPartyMonsList_UseEvoStone(PartyMenu *partyMenu, u8 partySlot) {
    FillWindowPixelBuffer(&partyMenu->windows[partySlot * PARTY_MENU_WINDOWS_PER_MON + PARTY_MENU_WINDOW_ID_MON1_NICKNAME], 0);
    FillWindowPixelBuffer(&partyMenu->windows[partySlot * PARTY_MENU_WINDOWS_PER_MON + PARTY_MENU_WINDOW_ID_MON1_LEVEL], 0);
    PartyMenu_PrintMonNicknameOnWindow(partyMenu, partySlot);
    if (partyMenu->monsDrawState[partySlot].isEgg == TRUE) {
        PartyMenu_PrintEvoStoneCompatString(partyMenu, partySlot, 1);
    } else {
        PartyMenu_PrintMonLevelOnWindow(partyMenu, partySlot);
        Pokemon *mon = Party_GetMonByIndex(partyMenu->args->party, partySlot);
        if (!GetMonEvolution(NULL, mon, EVOCTX_ITEM_USE, partyMenu->args->itemId, NULL)) {
            PartyMenu_PrintEvoStoneCompatString(partyMenu, partySlot, 1);
        } else {
            PartyMenu_PrintEvoStoneCompatString(partyMenu, partySlot, 0);
        }
    }
}

void PartyMenu_DrawPartyMonsList_UseTMHM(PartyMenu *partyMenu, u8 partySlot) {
    FillWindowPixelBuffer(&partyMenu->windows[partySlot * PARTY_MENU_WINDOWS_PER_MON + PARTY_MENU_WINDOW_ID_MON1_NICKNAME], 0);
    FillWindowPixelBuffer(&partyMenu->windows[partySlot * PARTY_MENU_WINDOWS_PER_MON + PARTY_MENU_WINDOW_ID_MON1_LEVEL], 0);
    PartyMenu_PrintMonNicknameOnWindow(partyMenu, partySlot);
    if (partyMenu->monsDrawState[partySlot].isEgg == TRUE) {
        PartyMenu_PrintTMHMCompatString(partyMenu, partySlot, 1);
    } else {
        PartyMenu_PrintMonLevelOnWindow(partyMenu, partySlot);
        u8 x = PartyMenu_CheckCanLearnTMHMMove(partyMenu, Party_GetMonByIndex(partyMenu->args->party, partySlot));
        if (x == LEARN_MOVE_CHECK_INCOMPAT) {
            PartyMenu_PrintTMHMCompatString(partyMenu, partySlot, 1);
        } else if (x == LEARN_MOVE_CHECK_KNOWN) {
            PartyMenu_PrintTMHMCompatString(partyMenu, partySlot, 2);
        } else {
            PartyMenu_PrintTMHMCompatString(partyMenu, partySlot, 0);
        }
    }
}

void PartyMenu_DrawPartyMonsList_SuperContestEntry(PartyMenu *partyMenu, u8 partySlot) {
    FillWindowPixelBuffer(&partyMenu->windows[partySlot * PARTY_MENU_WINDOWS_PER_MON + PARTY_MENU_WINDOW_ID_MON1_NICKNAME], 0);
    FillWindowPixelBuffer(&partyMenu->windows[partySlot * PARTY_MENU_WINDOWS_PER_MON + PARTY_MENU_WINDOW_ID_MON1_LEVEL], 0);
    PartyMenu_PrintMonNicknameOnWindow(partyMenu, partySlot);
    if (partyMenu->monsDrawState[partySlot].isEgg != TRUE) {
        PartyMenu_PrintMonLevelOnWindow(partyMenu, partySlot);
    }
    PartyMenu_PrintSuperContestCompatString(partyMenu, partySlot, partyMenu->monsDrawState[partySlot].isContestCompatible);
}

void PartyMenu_DrawPartyMonsList_FrontierFacilityEntry(PartyMenu *partyMenu, u8 partySlot) {
    FillWindowPixelBuffer(&partyMenu->windows[partySlot * PARTY_MENU_WINDOWS_PER_MON + PARTY_MENU_WINDOW_ID_MON1_NICKNAME], 0);
    FillWindowPixelBuffer(&partyMenu->windows[partySlot * PARTY_MENU_WINDOWS_PER_MON + PARTY_MENU_WINDOW_ID_MON1_LEVEL], 0);
    PartyMenu_PrintMonNicknameOnWindow(partyMenu, partySlot);
    if (partyMenu->monsDrawState[partySlot].isEgg != TRUE) {
        PartyMenu_PrintMonLevelOnWindow(partyMenu, partySlot);
    }
}

void PartyMenu_PrintFrontierEntryStatus(PartyMenu *partyMenu, u8 partySlot) {
    for (u8 i = 0; i < partyMenu->args->maxMonsToSelect; ++i) {
        if (partyMenu->args->selectedOrder[i] == partySlot + 1) {
            PartyMenu_PrintEntryStatusString(partyMenu, partySlot, i);
            return;
        }
    }

    if (partyMenu->args->context == PARTY_MENU_CONTEXT_BATTLE_HALL) {
        if (PartyMenu_GetBattleHallEntryStatus(partyMenu, partySlot) == 0) {
            PartyMenu_PrintEntryStatusString(partyMenu, partySlot, 8);
            return;
        }
    } else if (partyMenu->args->context == PARTY_MENU_CONTEXT_23) {
        if (PartyMenu_GetArcadeCastleEntryStatus(partyMenu, partySlot) == 0) {
            PartyMenu_PrintEntryStatusString(partyMenu, partySlot, 8);
            return;
        }
    } else {
        if (PartyMenu_GetLinkBattleEntryStatus(partyMenu, partySlot) == 0) {
            PartyMenu_PrintEntryStatusString(partyMenu, partySlot, 8);
            return;
        }
    }

    PartyMenu_PrintEntryStatusString(partyMenu, partySlot, 7);
}

static u32 getStringCenterXPos(FontID fontId, String *string, u32 windowWidth) {
    return (windowWidth - FontID_String_GetWidth(fontId, string, 0)) / 2;
}

void PartyMenu_PrintConfirmCancelButtonText(PartyMenu *partyMenu, u8 flag) {
    FillWindowPixelBuffer(&partyMenu->windows[PARTY_MENU_WINDOW_ID_30], 0);
    FillWindowPixelBuffer(&partyMenu->windows[PARTY_MENU_WINDOW_ID_31], 0);
    if (flag & 1) {
        ReadMsgDataIntoString(partyMenu->msgData, msg_0300_00000, partyMenu->unformattedStrBuf);
        AddTextPrinterParameterizedWithColor(&partyMenu->windows[PARTY_MENU_WINDOW_ID_30], 0, partyMenu->unformattedStrBuf, getStringCenterXPos(0, partyMenu->unformattedStrBuf, partyMenu->windows[PARTY_MENU_WINDOW_ID_30].width * 8), 0, TEXT_SPEED_NOTRANSFER, MAKE_TEXT_COLOR(15, 14, 0), NULL);
        ScheduleWindowCopyToVram(&partyMenu->windows[PARTY_MENU_WINDOW_ID_30]);
    }
    if (flag & 2) {
        ReadMsgDataIntoString(partyMenu->msgData, msg_0300_00001, partyMenu->unformattedStrBuf);
        AddTextPrinterParameterizedWithColor(&partyMenu->windows[PARTY_MENU_WINDOW_ID_31], 0, partyMenu->unformattedStrBuf, getStringCenterXPos(0, partyMenu->unformattedStrBuf, partyMenu->windows[PARTY_MENU_WINDOW_ID_31].width * 8), 0, TEXT_SPEED_NOTRANSFER, MAKE_TEXT_COLOR(15, 14, 0), NULL);
        ScheduleWindowCopyToVram(&partyMenu->windows[PARTY_MENU_WINDOW_ID_31]);
    }
}

static void PartyMenu_PrintMessageOnWindowEx(PartyMenu *partyMenu, Window *window, int msgId, BOOL drawFrame) {
    if (drawFrame == TRUE) {
        DrawFrameAndWindow2(window, TRUE, 0x02A, 15);
    }
    FillWindowPixelBuffer(window, 15);
    if (msgId != -1) {
        ReadMsgDataIntoString(partyMenu->msgData, msgId, partyMenu->formattedStrBuf);
    }
    AddTextPrinterParameterized(window, 1, partyMenu->formattedStrBuf, 0, 0, TEXT_SPEED_NOTRANSFER, NULL);
    ScheduleWindowCopyToVram(window);
#ifdef HEARTGOLD_NATIVE
    NativePartyUISetMessage(partyMenu, window);
#endif
}

void PartyMenu_PrintMessageOnWindow32(PartyMenu *partyMenu, int msgId, BOOL drawFrame) {
    PartyMenu_PrintMessageOnWindowEx(partyMenu, &partyMenu->windows[PARTY_MENU_WINDOW_ID_32], msgId, drawFrame);
}

void PartyMenu_PrintMessageOnWindow33(PartyMenu *partyMenu, int msgId, BOOL drawFrame) {
    PartyMenu_PrintMessageOnWindowEx(partyMenu, &partyMenu->windows[PARTY_MENU_WINDOW_ID_33], msgId, drawFrame);
}

void PartyMenu_PrintMessageOnWindow34(PartyMenu *partyMenu, int msgId, BOOL drawFrame) {
    Window *window = &partyMenu->windows[PARTY_MENU_WINDOW_ID_34];
    if (drawFrame == TRUE) {
        DrawFrameAndWindow2(window, TRUE, 0x02A, 15);
    }
    FillWindowPixelBuffer(window, 15);
    if (msgId != -1) {
        ReadMsgDataIntoString(partyMenu->msgData, msgId, partyMenu->formattedStrBuf);
    }
    PartyMenu_PrintBufferedMessageOnWindow34(partyMenu);
}

void PartyMenu_PrintBufferedMessageOnWindow34(PartyMenu *partyMenu) {
    TextFlags_SetCanABSpeedUpPrint(TRUE);
    TextFlags_SetAutoScrollParam(AUTO_SCROLL_OFF);
    partyMenu->textPrinterId = AddTextPrinterParameterized(&partyMenu->windows[PARTY_MENU_WINDOW_ID_34], 1, partyMenu->formattedStrBuf, 0, 0, Options_GetTextFrameDelay(partyMenu->args->options), TextPrinterCB_PartyMenuWin34Msg);
#ifdef HEARTGOLD_NATIVE
    NativePartyUISetMessage(partyMenu, &partyMenu->windows[PARTY_MENU_WINDOW_ID_34]);
#endif
}

static BOOL TextPrinterCB_PartyMenuWin34Msg(TextPrinterTemplate *template, u16 glyphId) {
#ifdef HEARTGOLD_NATIVE
    NativePartyUIPrinterUpdated(template, glyphId);
#endif
    switch (glyphId) {
    case 0:
        break;
    case 1:
        return GF_IsAnySEPlaying();
    case 2:
        return IsFanfarePlaying();
    case 3:
        PlaySE(SEQ_SE_DP_KON);
        break;
    case 4:
        PlayFanfare(SEQ_ME_LVUP);
        break;
    case 5:
        return IsSEPlaying(SEQ_SE_DP_KON);
    }

    return FALSE;
}

void PartyMenu_CreateYesNoPrompt(PartyMenu *partyMenu) {
    YesNoPromptTemplate template;

    partyMenu->yesNoPrompt = YesNoPrompt_Create(HEAP_ID_PARTY_MENU);

    template.bgConfig = partyMenu->bgConfig;
    template.bgId = GF_BG_LYR_MAIN_0;
    template.tileStart = 0x260;
    template.plttSlot = 11;
    template.x = 25;
    template.y = 10;
    template.ignoreTouchFlag = FALSE;
    template.initialCursorPos = 0;
    template.shapeParam = 0;
    YesNoPrompt_InitFromTemplate(partyMenu->yesNoPrompt, &template);
    PartyMenu_SetBlendBrightness_ForYesNo();
}

static void PartyMenu_PrintEvoStoneCompatString(PartyMenu *partyMenu, u8 partySlot, u8 compatibilityParam) {
    Window *window = &partyMenu->windows[partySlot * PARTY_MENU_WINDOWS_PER_MON + PARTY_MENU_WINDOW_ID_MON1_COMPAT];
    String *string;

    FillWindowPixelBuffer(window, 0);
    switch (compatibilityParam) {
    case 0: // ABLE!
        string = NewString_ReadMsgData(partyMenu->msgData, msg_0300_00161);
        break;
    case 1: // UNABLE!
        string = NewString_ReadMsgData(partyMenu->msgData, msg_0300_00162);
        break;
    }
    // potential UB: in default case, string is uninitialized
    AddTextPrinterParameterizedWithColor(window, 0, string, 0, 0, TEXT_SPEED_NOTRANSFER, MAKE_TEXT_COLOR(15, 14, 0), NULL);
    String_Delete(string);
    ScheduleWindowCopyToVram(window);
}

static void PartyMenu_PrintTMHMCompatString(PartyMenu *partyMenu, u8 partySlot, u8 compatibilityParam) {
    Window *window = &partyMenu->windows[partySlot * PARTY_MENU_WINDOWS_PER_MON + PARTY_MENU_WINDOW_ID_MON1_COMPAT];
    String *string;

    FillWindowPixelBuffer(window, 0);
    switch (compatibilityParam) {
    case 0: // ABLE!
        string = NewString_ReadMsgData(partyMenu->msgData, msg_0300_00158);
        break;
    case 1: // UNABLE!
        string = NewString_ReadMsgData(partyMenu->msgData, msg_0300_00159);
        break;
    case 2: // LEARNED
        string = NewString_ReadMsgData(partyMenu->msgData, msg_0300_00160);
        break;
    }
    // potential UB: in default case, string is uninitialized
    AddTextPrinterParameterizedWithColor(window, 0, string, 0, 0, TEXT_SPEED_NOTRANSFER, MAKE_TEXT_COLOR(15, 14, 0), NULL);
    String_Delete(string);
    ScheduleWindowCopyToVram(window);
}

static void PartyMenu_PrintSuperContestCompatString(PartyMenu *partyMenu, u8 partySlot, u8 isCompatible) {
    Window *window = &partyMenu->windows[partySlot * PARTY_MENU_WINDOWS_PER_MON + PARTY_MENU_WINDOW_ID_MON1_COMPAT];
    String *string;

    FillWindowPixelBuffer(window, 0);
    if (isCompatible == FALSE) {
        // UNABLE
        string = NewString_ReadMsgData(partyMenu->msgData, msg_0300_00164);
    } else {
        // ABLE
        string = NewString_ReadMsgData(partyMenu->msgData, msg_0300_00163);
    }
    AddTextPrinterParameterizedWithColor(window, 0, string, 0, 0, TEXT_SPEED_NOTRANSFER, MAKE_TEXT_COLOR(15, 14, 0), NULL);
    String_Delete(string);
    ScheduleWindowCopyToVram(window);
}

static void PartyMenu_PrintEntryStatusString(PartyMenu *partyMenu, u8 partySlot, u8 entryStatus) {
    Window *window = &partyMenu->windows[partySlot * PARTY_MENU_WINDOWS_PER_MON + PARTY_MENU_WINDOW_ID_MON1_COMPAT];
    String *string;

    FillWindowPixelBuffer(window, 0);
    if (entryStatus < 6) {
        string = NewString_ReadMsgData(partyMenu->msgData, msg_0300_00152 + entryStatus);
    } else if (entryStatus == 7) {
        string = NewString_ReadMsgData(partyMenu->msgData, msg_0300_00150);
    } else {
        string = NewString_ReadMsgData(partyMenu->msgData, msg_0300_00151);
    }
    AddTextPrinterParameterizedWithColor(window, 0, string, 0, 0, TEXT_SPEED_NOTRANSFER, MAKE_TEXT_COLOR(15, 14, 0), NULL);
    String_Delete(string);
    ScheduleWindowCopyToVram(window);
}

void PartyMenu_LevelUpPrintStatsChange(PartyMenu *partyMenu) {
    u16 stats[NUM_STATS];
    String *str_formatInt;
    String *str_statName;
    String *str_plusSign;
    u32 i;
    Pokemon *mon;

    mon = Party_GetMonByIndex(partyMenu->args->party, partyMenu->partyMonIndex);
    stats[0] = GetMonData(mon, MON_DATA_MAX_HP, NULL);
    stats[1] = GetMonData(mon, MON_DATA_ATK, NULL);
    stats[2] = GetMonData(mon, MON_DATA_DEF, NULL);
    stats[3] = GetMonData(mon, MON_DATA_SP_ATK, NULL);
    stats[4] = GetMonData(mon, MON_DATA_SP_DEF, NULL);
    stats[5] = GetMonData(mon, MON_DATA_SPEED, NULL);
    AddWindowParameterized(partyMenu->bgConfig, &partyMenu->levelUpStatsWindow[0], GF_BG_LYR_MAIN_0, 1, 1, 14, 12, 0, 0x260);
    DrawFrameAndWindow1(&partyMenu->levelUpStatsWindow[0], TRUE, 1, 14);
    FillWindowPixelBuffer(&partyMenu->levelUpStatsWindow[0], 15);
    str_plusSign = NewString_ReadMsgData(partyMenu->msgData, msg_0300_00175);
    for (i = 0; i < NUM_STATS; ++i) {
        str_statName = NewString_ReadMsgData(partyMenu->msgData, msg_0300_00169 + i);
        AddTextPrinterParameterized(&partyMenu->levelUpStatsWindow[0], 0, str_statName, 0, i * 16, TEXT_SPEED_NOTRANSFER, NULL);
        String_Delete(str_statName);

        AddTextPrinterParameterized(&partyMenu->levelUpStatsWindow[0], 0, str_plusSign, 80, i * 16, TEXT_SPEED_NOTRANSFER, NULL);

        str_formatInt = NewString_ReadMsgData(partyMenu->msgData, msg_0300_00176);
        BufferIntegerAsString(partyMenu->msgFormat, 0, stats[i] - partyMenu->levelUpStatsTmp[i], 2, PRINTING_MODE_LEFT_ALIGN, TRUE);
        StringExpandPlaceholders(partyMenu->msgFormat, partyMenu->formattedStrBuf, str_formatInt);
        String_Delete(str_formatInt);
        AddTextPrinterParameterized(&partyMenu->levelUpStatsWindow[0], 0, partyMenu->formattedStrBuf, 94, i * 16, TEXT_SPEED_NOTRANSFER, NULL);

        partyMenu->levelUpStatsTmp[i] = stats[i];
    }
    String_Delete(str_plusSign);
    ScheduleWindowCopyToVram(&partyMenu->levelUpStatsWindow[0]);
}

void PartyMenu_LevelUpPrintNewStats(PartyMenu *partyMenu) {
    String *numberMsg;
    u32 i;

    FillWindowPixelRect(&partyMenu->levelUpStatsWindow[0], 15, 80, 0, 32, 112);
    numberMsg = NewString_ReadMsgData(partyMenu->msgData, msg_0300_00176);
    for (i = 0; i < NUM_STATS; ++i) {
        BufferIntegerAsString(partyMenu->msgFormat, 0, partyMenu->levelUpStatsTmp[i], 3, PRINTING_MODE_LEFT_ALIGN, TRUE);
        StringExpandPlaceholders(partyMenu->msgFormat, partyMenu->formattedStrBuf, numberMsg);
        AddTextPrinterParameterized(&partyMenu->levelUpStatsWindow[0], 0, partyMenu->formattedStrBuf, 104 - FontID_String_GetWidth(0, partyMenu->formattedStrBuf, 0), i * 16, TEXT_SPEED_NOTRANSFER, NULL);
    }
    String_Delete(numberMsg);
    ScheduleWindowCopyToVram(&partyMenu->levelUpStatsWindow[0]);
}

void PartyMenu_LevelUpCloseStatsWindow(PartyMenu *partyMenu) {
    EraseFrameAndWindow1(&partyMenu->levelUpStatsWindow[0], FALSE);
    RemoveWindow(&partyMenu->levelUpStatsWindow[0]);
}

void PartyMenu_PrintTopScreenMonNameAndItem(PartyMenu *partyMenu) {
    Pokemon *mon;
    String *msg;

    FillWindowPixelBuffer(&partyMenu->windows[PARTY_MENU_WINDOW_ID_37], 0);
    FillWindowPixelBuffer(&partyMenu->windows[PARTY_MENU_WINDOW_ID_39], 0);

    mon = Party_GetMonByIndex(partyMenu->args->party, partyMenu->partyMonIndex);
    msg = NewString_ReadMsgData(partyMenu->msgData, msg_0300_00008);
    BufferBoxMonNickname(partyMenu->msgFormat, 0, Mon_GetBoxMon(mon));
    StringExpandPlaceholders(partyMenu->msgFormat, partyMenu->formattedStrBuf, msg);
    AddTextPrinterParameterizedWithColor(&partyMenu->windows[PARTY_MENU_WINDOW_ID_37], 0, partyMenu->formattedStrBuf, 0, 0, TEXT_SPEED_NOTRANSFER, MAKE_TEXT_COLOR(15, 14, 0), NULL);
    String_Delete(msg);

    mon = Party_GetMonByIndex(partyMenu->args->party, partyMenu->partyMonIndex);
    msg = NewString_ReadMsgData(partyMenu->msgData, msg_0300_00182);
    BufferItemName(partyMenu->msgFormat, 1, GetMonData(mon, MON_DATA_HELD_ITEM, NULL));
    StringExpandPlaceholders(partyMenu->msgFormat, partyMenu->formattedStrBuf, msg);
    AddTextPrinterParameterizedWithColor(&partyMenu->windows[PARTY_MENU_WINDOW_ID_39], 0, partyMenu->formattedStrBuf, 2, 4, TEXT_SPEED_NOTRANSFER, MAKE_TEXT_COLOR(15, 14, 0), NULL);
    String_Delete(msg);

    ScheduleWindowCopyToVram(&partyMenu->windows[PARTY_MENU_WINDOW_ID_37]);
    ScheduleWindowCopyToVram(&partyMenu->windows[PARTY_MENU_WINDOW_ID_39]);
}

static void PartyMenu_ShowContextMenu(PartyMenu *partyMenu, int numItems, int state) {
    GF_ASSERT(numItems <= 8);
    for (int i = 0; i < numItems; ++i) {
        AddWindow(partyMenu->bgConfig, &partyMenu->contextMenuButtonWindows[sButtonWindowIDs[numItems - 1][state][i]], &sButtonWindowTemplates[sButtonWindowIDs[numItems - 1][state][i]]);
        FillWindowPixelBuffer(&partyMenu->contextMenuButtonWindows[sButtonWindowIDs[numItems - 1][state][i]], 4);
    }
}

static void PartyMenu_HideContextMenu(PartyMenu *partyMenu, int numItems, int state) {
    GF_ASSERT(numItems <= 8);
    for (int i = 0; i < numItems; ++i) {
        RemoveWindow(&partyMenu->contextMenuButtonWindows[sButtonWindowIDs[numItems - 1][state][i]]);
    }
    FillBgTilemapRect(partyMenu->bgConfig, GF_BG_LYR_MAIN_0, 0, 0, 0, 32, 18, 0);
    FillBgTilemapRect(partyMenu->bgConfig, GF_BG_LYR_MAIN_0, 0, 25, 19, 7, 5, 0);
    BgCommitTilemapBufferToVram(partyMenu->bgConfig, GF_BG_LYR_MAIN_0);
}

static u32 getButtonColorDepressed(int selection) {
    if (selection < 4) {
        return MAKE_TEXT_COLOR(14, 15, 11);
    } else {
        return MAKE_TEXT_COLOR(9, 10, 11);
    }
}

static u32 getButtonColorRaised(int selection) {
    if (selection < 4) {
        return MAKE_TEXT_COLOR(14, 15, 4);
    } else {
        return MAKE_TEXT_COLOR(9, 10, 4);
    }
}

static void PartyMenu_PrintContextMenuItemText(PartyMenu *partyMenu, PartyMenuContextMenu *contextMenu, int numItems, int selection, int state, BOOL depressed) {
    u32 color;
    u32 y;
    u32 x = 0;
    u32 fillValue;
    u8 windowId;

    windowId = sButtonWindowIDs[numItems - 1][state][selection];
    if (windowId == 7) {
        if (depressed == FALSE) {
            fillValue = 4;
            color = MAKE_TEXT_COLOR(14, 15, 4);
        } else {
            fillValue = 11;
            color = MAKE_TEXT_COLOR(14, 15, 11);
        }
        y = 4;
        x = FontID_String_GetCenterAlignmentX(4, contextMenu->items[selection].text, 0, GetWindowWidth(&partyMenu->contextMenuButtonWindows[windowId]) * 8);
    } else {
        if (depressed == FALSE) {
            fillValue = 4;
            color = getButtonColorRaised(selection);
        } else {
            fillValue = 11;
            color = getButtonColorDepressed(selection);
        }
        y = 0;
    }
    FillWindowPixelBuffer(&partyMenu->contextMenuButtonWindows[windowId], fillValue);
    AddTextPrinterParameterizedWithColor(&partyMenu->contextMenuButtonWindows[windowId], 4, contextMenu->items[selection].text, x, y, TEXT_SPEED_NOTRANSFER, color, NULL);
    ScheduleWindowCopyToVram(&partyMenu->contextMenuButtonWindows[windowId]);
}

static void PartyMenu_PrintContextMenuItemsText(PartyMenu *partyMenu, PartyMenuContextMenu *contextMenu, int numItems, int selection, int state) {
    GF_ASSERT(numItems <= 8);
    for (int i = 0; i < numItems; ++i) {
        if (selection == i) {
            PartyMenu_PrintContextMenuItemText(partyMenu, contextMenu, numItems, i, state, TRUE);
        } else {
            PartyMenu_PrintContextMenuItemText(partyMenu, contextMenu, numItems, i, state, FALSE);
        }
    }
}

static void PartyMenu_DrawContextMenuButtonFrame(PartyMenu *partyMenu, int numItems, int selection, int state, int frameType) {
    u16 i;
    u16 tiles[8];
    const u8 *rect = sButtonRects[sButtonWindowIDs[numItems - 1][state][selection]];
    u32 tileStart;
    if (frameType == 0) {
        tileStart = 0x2000;
    } else if (frameType == 1) {
        tileStart = 0x2009;
    } else {
        tileStart = 0x2012;
    }
    tileStart += 10;

    for (i = 0; i < 8; ++i) {
        tiles[i] = tileStart + sButtonFrameTileOffsets[i];
    }
    LoadRectToBgTilemapRect(partyMenu->bgConfig, GF_BG_LYR_MAIN_0, &tiles[0], rect[0], rect[1], 1, 1);
    LoadRectToBgTilemapRect(partyMenu->bgConfig, GF_BG_LYR_MAIN_0, &tiles[1], rect[0] + rect[2] - 1, rect[1], 1, 1);
    LoadRectToBgTilemapRect(partyMenu->bgConfig, GF_BG_LYR_MAIN_0, &tiles[2], rect[0], rect[1] + rect[3] - 1, 1, 1);
    LoadRectToBgTilemapRect(partyMenu->bgConfig, GF_BG_LYR_MAIN_0, &tiles[3], rect[0] + rect[2] - 1, rect[1] + rect[3] - 1, 1, 1);
    FillBgTilemapRect(partyMenu->bgConfig, GF_BG_LYR_MAIN_0, tiles[4], rect[0], rect[1] + 1, 1, rect[3] - 2, TILEMAP_FILL_OVWT_PAL);
    FillBgTilemapRect(partyMenu->bgConfig, GF_BG_LYR_MAIN_0, tiles[5], rect[0] + rect[2] - 1, rect[1] + 1, 1, rect[3] - 2, TILEMAP_FILL_OVWT_PAL);
    FillBgTilemapRect(partyMenu->bgConfig, GF_BG_LYR_MAIN_0, tiles[6], rect[0] + 1, rect[1], rect[2] - 2, 1, TILEMAP_FILL_OVWT_PAL);
    FillBgTilemapRect(partyMenu->bgConfig, GF_BG_LYR_MAIN_0, tiles[7], rect[0] + 1, rect[1] + rect[3] - 1, rect[2] - 2, 1, TILEMAP_FILL_OVWT_PAL);
}

void PartyMenu_DrawContextMenuButtonFrames(PartyMenu *partyMenu, int numItems, int selection, int state) {
    for (int i = 0; i < numItems; ++i) {
        if (selection == i) {
            PartyMenu_DrawContextMenuButtonFrame(partyMenu, numItems, i, state, 1);
        } else {
            PartyMenu_DrawContextMenuButtonFrame(partyMenu, numItems, i, state, 0);
        }
    }
}

PartyMenuContextMenuCursor *PartyMenu_CreateContextMenuCursor(PartyMenu *partyMenu, const PartyMenuContextMenu *template, int selection, enum HeapID heapID, int state) {
    PartyMenuContextMenuCursor *ret = Heap_Alloc(heapID, sizeof(PartyMenuContextMenuCursor));
    ret->menu = *template;
    ret->numItems = ret->menu.numItems;
    ret->prevSelection = selection;
    ret->selection = selection;
    ret->state = state;
    PartyMenu_ShowContextMenu(partyMenu, ret->menu.numItems, ret->state);
    PartyMenu_PrintContextMenuItemsText(partyMenu, &ret->menu, ret->menu.numItems, ret->prevSelection, ret->state);
    PartyMenu_SetBlendBrightness_ForYesNo();
    Sprite_SetDrawFlag(partyMenu->sprites[PARTY_MENU_SPRITE_ID_9], FALSE);
    if (partyMenu->args->context == PARTY_MENU_CONTEXT_UNION_ROOM_BATTLE_SELECT || partyMenu->args->context == PARTY_MENU_CONTEXT_17 || partyMenu->args->context == PARTY_MENU_CONTEXT_BATTLE_HALL || partyMenu->args->context == PARTY_MENU_CONTEXT_23) {
        Sprite_SetDrawFlag(partyMenu->sprites[PARTY_MENU_SPRITE_ID_8], FALSE);
    }
#ifdef HEARTGOLD_NATIVE
    NativePartyUIContextCreated(partyMenu, ret);
#endif
    return ret;
}

void PartyMenu_CloseContextMenu(PartyMenu *partyMenu, PartyMenuContextMenuCursor *cursor) {
#ifdef HEARTGOLD_NATIVE
    NativePartyUIContextClosed(partyMenu);
#endif
    PartyMenu_HideContextMenu(partyMenu, cursor->numItems, cursor->state);
    Heap_Free(cursor);
    if (partyMenu->args->context == PARTY_MENU_CONTEXT_4 || partyMenu->args->context == PARTY_MENU_CONTEXT_SPIN_TRADE) {
        Sprite_SetDrawFlag(partyMenu->sprites[PARTY_MENU_SPRITE_ID_9], FALSE);
    } else {
        Sprite_SetDrawFlag(partyMenu->sprites[PARTY_MENU_SPRITE_ID_9], TRUE);
    }
    if (partyMenu->args->context == PARTY_MENU_CONTEXT_UNION_ROOM_BATTLE_SELECT || partyMenu->args->context == PARTY_MENU_CONTEXT_17 || partyMenu->args->context == PARTY_MENU_CONTEXT_BATTLE_HALL || partyMenu->args->context == PARTY_MENU_CONTEXT_23) {
        Sprite_SetDrawFlag(partyMenu->sprites[PARTY_MENU_SPRITE_ID_8], TRUE);
    }
}

// NOTE: naming style reflective of that these do not operate on the central type `PartyMenuStruct *` of this TU
static BOOL hitboxHasSelectionMapping(int state, int numItems, int selection) {
    for (int i = 0; i < 8; ++i) {
        if (selection == sButtonWindowIDs[numItems - 1][state][i]) {
            return TRUE;
        }
    }

    return FALSE;
}

static int hitboxToSelection(int state, int numItems, int selection) {
    for (int i = 0; i < 8; ++i) {
        if (selection == sButtonWindowIDs[numItems - 1][state][i]) {
            return i;
        }
    }

    return LIST_NOTHING_CHOSEN;
}

static BOOL contextMenuButtonIsActive(int state, int numItems, int selection) {
    if (sButtonWindowIDs[numItems - 1][state][selection] != -1 && sButtonWindowIDs[numItems - 1][state][selection] != 7) {
        return TRUE;
    } else {
        return FALSE;
    }
}

static BOOL handlePartyMenuTopLevelDpadInput(u8 *pSelection, int numItems, int direction) {
    if (sDpadNavParam_PartyMenu[numItems - 2][*pSelection][direction] != -1) {
        *pSelection = sDpadNavParam_PartyMenu[numItems - 2][*pSelection][direction];
        return TRUE;
    } else {
        return FALSE;
    }
}

static BOOL handlePartyContextMenuDpadInput(u8 *pSelection, int numItems, int direction) {
    if (sDpadNavParam_ContextMenu[numItems - 2][*pSelection][direction] != -1) {
        *pSelection = sDpadNavParam_ContextMenu[numItems - 2][*pSelection][direction];
        return TRUE;
    } else {
        return FALSE;
    }
}

u32 PartyMenu_HandleInput_ContextMenu(PartyMenu *partyMenu, PartyMenuContextMenuCursor *cursor) {
    PartyMenuContextButtonAnimData *animData = &partyMenu->contextMenuButtonAnim;
    BOOL dpadInputValid = FALSE;

    if (animData->active == TRUE) {
        if (PartyMenu_AnimateContextMenuButtonPress(partyMenu) == FALSE) {
            return animData->followUpState;
        } else {
            return LIST_NOTHING_CHOSEN;
        }
    }

    int hitbox = TouchscreenHitbox_FindRectAtTouchNew(sHitboxes);
    if (hitbox != -1) {
        if (hitboxHasSelectionMapping(cursor->state, cursor->numItems, hitbox) == TRUE) {
            if (hitbox == 7) {
                PlaySE(SEQ_SE_GS_GEARCANCEL);
            } else {
                PlaySE(SEQ_SE_DP_SELECT);
            }
            PartyMenu_DrawContextMenuButtonFrame(partyMenu, cursor->numItems, cursor->selection, cursor->state, 0);
            PartyMenu_PrintContextMenuItemText(partyMenu, &cursor->menu, cursor->numItems, cursor->selection, cursor->state, FALSE);
            cursor->selection = hitboxToSelection(cursor->state, cursor->numItems, hitbox);
            PartyMenu_StartContextMenuButtonPressAnim_FromCursorObj(partyMenu, cursor, cursor->menu.items[cursor->selection].value);
            return LIST_NOTHING_CHOSEN;
        }
    } else {
        if (gSystem.newAndRepeatedKeys & PAD_KEY_UP) {
            if (handlePartyMenuTopLevelDpadInput(&cursor->selection, cursor->numItems, 0)) {
                ++dpadInputValid;
            }
        } else if (gSystem.newAndRepeatedKeys & PAD_KEY_DOWN) {
            if (handlePartyMenuTopLevelDpadInput(&cursor->selection, cursor->numItems, 1)) {
                ++dpadInputValid;
            }
        } else if (gSystem.newAndRepeatedKeys & PAD_KEY_LEFT || gSystem.newAndRepeatedKeys & PAD_KEY_RIGHT) {
            if (handlePartyMenuTopLevelDpadInput(&cursor->selection, cursor->numItems, 2)) {
                ++dpadInputValid;
            }
        } else if (gSystem.newKeys & PAD_BUTTON_A) {
            if (cursor->selection == hitboxToSelection(cursor->state, cursor->numItems, 7)) {
                PlaySE(SEQ_SE_GS_GEARCANCEL);
            } else {
                PlaySE(SEQ_SE_DP_SELECT);
            }
            PartyMenu_StartContextMenuButtonPressAnim_FromCursorObj(partyMenu, cursor, cursor->menu.items[cursor->selection].value);
            return LIST_NOTHING_CHOSEN;
        } else if (gSystem.newKeys & PAD_BUTTON_B) {
            PlaySE(SEQ_SE_GS_GEARCANCEL);
            PartyMenu_DrawContextMenuButtonFrame(partyMenu, cursor->numItems, cursor->selection, cursor->state, 0);
            PartyMenu_PrintContextMenuItemText(partyMenu, &cursor->menu, cursor->numItems, cursor->selection, cursor->state, FALSE);
            cursor->selection = hitboxToSelection(cursor->state, cursor->numItems, 7);
            PartyMenu_StartContextMenuButtonPressAnim_FromCursorObj(partyMenu, cursor, LIST_CANCEL);
            return LIST_NOTHING_CHOSEN;
        }
    }

    if (dpadInputValid) {
        PlaySE(SEQ_SE_DP_SELECT);
        PartyMenu_DrawContextMenuButtonFrames(partyMenu, cursor->numItems, cursor->selection, cursor->state);
        PartyMenu_PrintContextMenuItemsText(partyMenu, &cursor->menu, cursor->numItems, cursor->selection, cursor->state);
    }

    return LIST_NOTHING_CHOSEN;
}

u32 PartyMenu_HandleSubcontextMenuInput_TopLevel(PartyMenu *partyMenu, PartyMenuContextMenuCursor *cursor) {
    PartyMenuContextButtonAnimData *animData = &partyMenu->contextMenuButtonAnim;
    BOOL dpadInputValid = FALSE;

    if (animData->active == TRUE) {
        if (PartyMenu_AnimateContextMenuButtonPress(partyMenu) == FALSE) {
            return animData->followUpState;
        } else {
            return LIST_NOTHING_CHOSEN;
        }
    }

    int hitbox = TouchscreenHitbox_FindRectAtTouchNew(sContextMenuHitboxes);
    if (hitbox != -1) {
        if (hitbox == 4) {
            PlaySE(SEQ_SE_GS_GEARCANCEL);
            PartyMenu_DrawContextMenuButtonFrame(partyMenu, cursor->numItems, cursor->selection, cursor->state, 0);
            PartyMenu_PrintContextMenuItemText(partyMenu, &cursor->menu, cursor->numItems, cursor->selection, cursor->state, FALSE);
            cursor->selection = cursor->numItems - 1;
            PartyMenu_StartContextMenuButtonPressAnim_FromCursorObj(partyMenu, cursor, LIST_CANCEL);
            return LIST_NOTHING_CHOSEN;
        } else if (contextMenuButtonIsActive(cursor->state, cursor->numItems, hitbox) == TRUE) {
            PlaySE(SEQ_SE_DP_SELECT);
            PartyMenu_DrawContextMenuButtonFrame(partyMenu, cursor->numItems, cursor->selection, cursor->state, 0);
            PartyMenu_PrintContextMenuItemText(partyMenu, &cursor->menu, cursor->numItems, cursor->selection, cursor->state, FALSE);
            cursor->selection = hitbox;
            PartyMenu_StartContextMenuButtonPressAnim_FromCursorObj(partyMenu, cursor, cursor->menu.items[cursor->selection].value);
            return LIST_NOTHING_CHOSEN;
        }
    } else {
        if (gSystem.newAndRepeatedKeys & PAD_KEY_UP) {
            if (handlePartyContextMenuDpadInput(&cursor->selection, cursor->numItems, 0)) {
                ++dpadInputValid;
            }
        } else if (gSystem.newAndRepeatedKeys & PAD_KEY_DOWN) {
            if (handlePartyContextMenuDpadInput(&cursor->selection, cursor->numItems, 1)) {
                ++dpadInputValid;
            }
        } else if (gSystem.newKeys & PAD_BUTTON_A) {
            if (cursor->selection == cursor->numItems - 1) {
                PlaySE(SEQ_SE_GS_GEARCANCEL);
            } else {
                PlaySE(SEQ_SE_DP_SELECT);
            }
            PartyMenu_DrawContextMenuButtonFrame(partyMenu, cursor->numItems, cursor->selection, cursor->state, 0);
            PartyMenu_PrintContextMenuItemText(partyMenu, &cursor->menu, cursor->numItems, cursor->selection, cursor->state, FALSE);
            PartyMenu_StartContextMenuButtonPressAnim_FromCursorObj(partyMenu, cursor, cursor->menu.items[cursor->selection].value);
            return LIST_NOTHING_CHOSEN;
        } else if (gSystem.newKeys & PAD_BUTTON_B) {
            PlaySE(SEQ_SE_GS_GEARCANCEL);
            PartyMenu_DrawContextMenuButtonFrame(partyMenu, cursor->numItems, cursor->selection, cursor->state, 0);
            PartyMenu_PrintContextMenuItemText(partyMenu, &cursor->menu, cursor->numItems, cursor->selection, cursor->state, FALSE);
            cursor->selection = cursor->numItems - 1;
            PartyMenu_StartContextMenuButtonPressAnim_FromCursorObj(partyMenu, cursor, LIST_CANCEL);
            return LIST_NOTHING_CHOSEN;
        }
    }

    if (dpadInputValid) {
        PlaySE(SEQ_SE_DP_SELECT);
        PartyMenu_DrawContextMenuButtonFrames(partyMenu, cursor->numItems, cursor->selection, cursor->state);
        PartyMenu_PrintContextMenuItemsText(partyMenu, &cursor->menu, cursor->numItems, cursor->selection, cursor->state);
    }

    return LIST_NOTHING_CHOSEN;
}

#ifdef HEARTGOLD_NATIVE
static HGNativePartyUIState sNativePartyUI;
static PartyMenu *sNativePartyMenu;
static PartyMenuContextMenuCursor *sNativePartyCursor;
static Window *sNativePartyMessage;
static BOOL sNativePartyMessageWaiting;
static u32 sNativePartyTextIds[HG_PARTY_UI_ACTIONS];

static void NativePartyUIInvalidateContext(void) {
    ++sNativePartyUI.revision;
    sNativePartyUI.ready = FALSE;
    sNativePartyUI.actionCount = sNativePartyUI.actionSelected = sNativePartyUI.actionEnabledMask = 0;
    MI_CpuClear8(sNativePartyUI.actions, sizeof(sNativePartyUI.actions));
    MI_CpuClear8(sNativePartyTextIds, sizeof(sNativePartyTextIds));
}

static void NativePartyUIContextCreated(PartyMenu *partyMenu, PartyMenuContextMenuCursor *cursor) {
    if (partyMenu != sNativePartyMenu) return;
    NativePartyUIInvalidateContext();
    sNativePartyCursor = cursor;
    if (cursor->state == 1 && cursor->numItems <= HG_PARTY_UI_ACTIONS) {
        for (u32 i = 0; i < cursor->numItems; ++i) {
            for (u32 j = 0; j < PARTY_MON_CONTEXT_MENU_FIELD_MOVES_BEGIN; ++j) {
                if (cursor->menu.items[i].text != NULL && partyMenu->contextMenuStrings[j] != NULL
                    && String_Compare(cursor->menu.items[i].text, partyMenu->contextMenuStrings[j]) == FALSE) {
                    sNativePartyTextIds[i] = j + 1;
                    break;
                }
            }
        }
    }
}

static void NativePartyUITopTextIds(PartyMenu *partyMenu, const u8 *items, u32 count) {
    if (partyMenu != sNativePartyMenu || partyMenu->contextMenuCursor != sNativePartyCursor || count > HG_PARTY_UI_ACTIONS) return;
    for (u32 i = 0; i < count; ++i) {
        sNativePartyTextIds[i] = items[i] < PARTY_MON_CONTEXT_MENU_MAX ? items[i] + 1 : 0;
    }
}

static void NativePartyUIContextClosed(PartyMenu *partyMenu) {
    if (partyMenu != sNativePartyMenu) return;
    sNativePartyCursor = NULL;
    NativePartyUIInvalidateContext();
}

static void NativePartyUISetMessage(PartyMenu *partyMenu, Window *window) {
    if (partyMenu != sNativePartyMenu) return;
    sNativePartyMessage = window;
    sNativePartyMessageWaiting = FALSE;
    ++sNativePartyUI.revision;
}

static void NativePartyUIPrinterUpdated(TextPrinterTemplate *template, u16 glyphId) {
    if (sNativePartyMenu == NULL || template->window != sNativePartyMessage
        || template->window != &sNativePartyMenu->windows[PARTY_MENU_WINDOW_ID_34]) return;
    // RunTextPrinter passes the first member of its live TextPrinter after RenderFont updates it.
    const TextPrinter *printer = (const TextPrinter *)template;
    sNativePartyMessageWaiting = printer->active && printer->state == 2 && !printer->callbackResult && !printer->callbackParam && !glyphId
        && printer->id == sNativePartyMenu->textPrinterId;
}

void NativePartyUIBind(PartyMenu *partyMenu) {
    if (partyMenu == sNativePartyMenu) return;
    u32 revision = sNativePartyUI.revision + 1;
    MI_CpuClear8(&sNativePartyUI, sizeof(sNativePartyUI));
    sNativePartyUI.revision = revision;
    sNativePartyUI.active = partyMenu != NULL;
    sNativePartyMenu = partyMenu;
    sNativePartyCursor = NULL;
    sNativePartyMessage = NULL;
    sNativePartyMessageWaiting = FALSE;
    MI_CpuClear8(sNativePartyTextIds, sizeof(sNativePartyTextIds));
    if (partyMenu != NULL) {
        sNativePartyUI.context = partyMenu->args->context;
        sNativePartyUI.panelLayout = partyMenu->args->panelLayout;
        sNativePartyUI.selected = partyMenu->partyMonIndex;
        if (partyMenu->args->context == PARTY_MENU_CONTEXT_0) {
            sNativePartyMessage = &partyMenu->windows[PARTY_MENU_WINDOW_ID_32];
        }
    }
}

void NativePartyUIBegin(PartyMenu *partyMenu) {
    if (partyMenu != sNativePartyMenu) return;
    sNativePartyUI.ready = FALSE;
    for (u32 i = 0; i < HG_PARTY_UI_SLOTS; ++i) MI_CpuClear8(&sNativePartyUI.mons[i].nickname, sizeof(HGNativePartyUIWindow));
    for (u32 i = 0; i < HG_PARTY_UI_ACTIONS; ++i) MI_CpuClear8(&sNativePartyUI.actions[i].text, sizeof(HGNativePartyUIWindow));
    MI_CpuClear8(&sNativePartyUI.selectedName, sizeof(HGNativePartyUIWindow));
    MI_CpuClear8(&sNativePartyUI.selectedItem, sizeof(HGNativePartyUIWindow));
    MI_CpuClear8(&sNativePartyUI.message, sizeof(HGNativePartyUIWindow));
}

static BOOL NativePartyUIWindow(HGNativePartyUIWindow *out, PartyMenu *partyMenu, const Window *window, u32 bgId, u32 width, u32 height, u32 clearColor) {
    MI_CpuClear8(out, sizeof(*out));
    if (window == NULL || window->bgConfig == NULL || window->bgConfig != partyMenu->bgConfig || bgId >= 8 || window->bgId != bgId
        || window->colorMode != GF_BG_CLR_4BPP || window->bgConfig->bgs[bgId].colorMode != GX_BG_COLORMODE_16
        || window->width != width || window->height != height || width == 0 || width > 32 || height == 0 || height > 4
        || window->paletteNum >= 16 || clearColor >= 16 || window->pixelBuffer == NULL
        || (uintptr_t)window->pixelBuffer > UINT32_MAX - width * height * TILE_SIZE_4BPP) return FALSE;
    out->pixelBuffer = (u32)(uintptr_t)window->pixelBuffer;
    out->tileWidth = width;
    out->tileHeight = height;
    out->clearColor = clearColor;
    out->engine = bgId >= GF_BG_LYR_SUB_0;
    const u16 *palette = out->engine ? GetSubBgPlttAddr() : GetMainBgPlttAddr();
    MI_CpuCopy16(palette + window->paletteNum * 16, out->palette, sizeof(out->palette));
    return TRUE;
}

static void NativePartyUIHitCenter(const TouchscreenHitbox *hitbox, u32 *x, u32 *y) {
    *x = (hitbox->rect.left + (hitbox->rect.right ? hitbox->rect.right : 256)) / 2;
    *y = (hitbox->rect.top + hitbox->rect.bottom) / 2;
}

static BOOL NativePartyUIActions(PartyMenu *partyMenu, HGNativePartyUIState *next, BOOL ready) {
    PartyMenuContextMenuCursor *cursor = sNativePartyCursor;
    u32 state = next->phase == HG_PARTY_UI_SUBCONTEXT;
    if (cursor == NULL || partyMenu->contextMenuCursor != cursor || cursor->state != state
        || cursor->numItems == 0 || cursor->numItems > (state ? 5 : HG_PARTY_UI_ACTIONS)
        || cursor->menu.numItems != cursor->numItems || cursor->selection >= cursor->numItems
        || cursor->menu.items == NULL || cursor->menu.items != partyMenu->listMenuItems) return FALSE;
    next->actionCount = cursor->numItems;
    next->actionSelected = cursor->selection;
    for (u32 i = 0; i < cursor->numItems; ++i) {
        int windowId = sButtonWindowIDs[cursor->numItems - 1][state][i];
        if (windowId < 0 || windowId >= NELEMS(sButtonWindowTemplates)) return FALSE;
        const TouchscreenHitbox *hitbox;
        if (state) {
            hitbox = &sContextMenuHitboxes[i == cursor->numItems - 1 ? 4 : i];
        } else {
            if (windowId >= NELEMS(sHitboxes) - 1) return FALSE;
            hitbox = &sHitboxes[windowId];
        }
        HGNativePartyUIAction *action = &next->actions[i];
        action->enabled = TRUE;
        action->textId = sNativePartyTextIds[i];
        NativePartyUIHitCenter(hitbox, &action->hitX, &action->hitY);
        next->actionEnabledMask |= 1 << i;
        if (ready && !NativePartyUIWindow(&action->text, partyMenu, &partyMenu->contextMenuButtonWindows[windowId], GF_BG_LYR_MAIN_0,
                sButtonWindowTemplates[windowId].width, sButtonWindowTemplates[windowId].height, i == cursor->selection ? 11 : 4)) return FALSE;
    }
    return TRUE;
}

void NativePartyUIPublish(PartyMenu *partyMenu, u32 phase, const TouchscreenHitbox *hitboxes) {
    if (partyMenu != sNativePartyMenu) return;
    HGNativePartyUIState next;
    MI_CpuClear8(&next, sizeof(next));
    next.active = TRUE;
    next.revision = sNativePartyUI.revision;
    next.context = partyMenu->args->context;
    next.panelLayout = partyMenu->args->panelLayout;
    next.phase = phase;
    next.selected = partyMenu->partyMonIndex;
    BOOL graphicsReady = partyMenu->bgConfig != NULL && !((partyMenu->bgConfig->bufferTransferScheduled | partyMenu->bgConfig->scrollScheduled) & 0xFF);
    BOOL itemProgress = !next.panelLayout && next.selected < HG_PARTY_UI_SLOTS && partyMenu->monsDrawState[next.selected].active
        && ((next.context == PARTY_MENU_CONTEXT_0 && (phase == HG_PARTY_UI_ITEM_RESULT || phase == HG_PARTY_UI_TAKE_RESULT))
            || (next.context == HG_PARTY_UI_GIVE_CONTEXT && (phase == HG_PARTY_UI_GIVE_ITEM || phase == HG_PARTY_UI_ITEM_RESULT)));
    BOOL supported = (next.context == PARTY_MENU_CONTEXT_0 && next.panelLayout == 0) || itemProgress;
    BOOL ready = next.context == PARTY_MENU_CONTEXT_0 && supported && (phase == HG_PARTY_UI_ROSTER || phase == HG_PARTY_UI_CONTEXT || phase == HG_PARTY_UI_SUBCONTEXT)
        && !partyMenu->contextMenuButtonAnim.active && !partyMenu->swapMonsData.active && IsPaletteFadeFinished();
    BOOL valid = TRUE;
    if (supported) {
        next.cancelEnabled = !itemProgress && !partyMenu->cancelDisabled;
        NativePartyUIHitCenter(&hitboxes[6], &next.cancelX, &next.cancelY);
        for (u32 i = 0; i < HG_PARTY_UI_SLOTS; ++i) {
            const PartyMenuMonsDrawState *mon = &partyMenu->monsDrawState[i];
            HGNativePartyUIMon *out = &next.mons[i];
            out->species = mon->species;
            out->hp = mon->hp;
            out->maxHp = mon->maxHp;
            out->level = mon->level;
            out->heldItem = mon->heldItem;
            out->status = mon->status;
            out->gender = mon->gender;
            out->dontPrintGenderSymbol = mon->dontPrintGenderSymbol;
            out->isEgg = mon->isEgg;
            out->form = mon->form;
            out->active = mon->active;
            NativePartyUIHitCenter(&hitboxes[i], &out->hitX, &out->hitY);
            if (!mon->active) continue;
            ++next.count;
            next.slotMask |= 1 << i;
            if (graphicsReady && IsPaletteFadeFinished()) {
                if (mon->iconSprite == NULL) {
                    valid = FALSE;
                } else {
                    s16 x, y;
                    Sprite_GetPositionXY(mon->iconSprite, &x, &y);
                    out->iconX = x - 16;
                    out->iconY = y - 16;
                    out->iconWidth = out->iconHeight = 32;
                }
                if (!NativePartyUIWindow(&out->nickname, partyMenu, &partyMenu->windows[i * PARTY_MENU_WINDOWS_PER_MON], GF_BG_LYR_MAIN_1, 9, 2, 0)) valid = FALSE;
            }
        }
        if (phase == HG_PARTY_UI_CONTEXT || phase == HG_PARTY_UI_SUBCONTEXT) {
            if (!NativePartyUIActions(partyMenu, &next, ready)) valid = FALSE;
            if (ready) {
                if (next.selected >= HG_PARTY_UI_SLOTS || !(next.slotMask & (1 << next.selected))) valid = FALSE;
                if (!NativePartyUIWindow(&next.selectedName, partyMenu, &partyMenu->windows[PARTY_MENU_WINDOW_ID_37], GF_BG_LYR_SUB_2, 10, 2, 0)) valid = FALSE;
                if (!NativePartyUIWindow(&next.selectedItem, partyMenu, &partyMenu->windows[PARTY_MENU_WINDOW_ID_39], GF_BG_LYR_SUB_2, 12, 3, 0)) valid = FALSE;
            }
        } else if (phase == HG_PARTY_UI_ROSTER && sNativePartyCursor != NULL) {
            valid = FALSE;
        }
        u32 width = 0, height = 0;
        if (sNativePartyMessage == &partyMenu->windows[PARTY_MENU_WINDOW_ID_32]) { width = 20; height = 2; }
        else if (sNativePartyMessage == &partyMenu->windows[PARTY_MENU_WINDOW_ID_33]) { width = 13; height = 4; }
        else if (sNativePartyMessage == &partyMenu->windows[PARTY_MENU_WINDOW_ID_34]) { width = 27; height = 4; }
        if (width) NativePartyUIWindow(&next.message, partyMenu, sNativePartyMessage, GF_BG_LYR_MAIN_0, width, height, 15);
        if (itemProgress && phase != HG_PARTY_UI_GIVE_ITEM && graphicsReady && sNativePartyMessageWaiting && valid
            && sNativePartyMessage == &partyMenu->windows[PARTY_MENU_WINDOW_ID_34]
            && next.message.pixelBuffer && width == 27 && height == 4
            && sNativePartyMessage->tilemapLeft == 2 && sNativePartyMessage->tilemapTop == 19
            && partyMenu->textPrinterId < MAX_TEXT_PRINTERS && TextPrinterCheckActive(partyMenu->textPrinterId)
            && IsPaletteFadeFinished()) {
            ready = next.cancelEnabled = TRUE;
            next.cancelX = 124; next.cancelY = 168;
        }
    }
    next.ready = ready && graphicsReady && valid && next.count != 0;
    BOOL changed = memcmp(&next, &sNativePartyUI, offsetof(HGNativePartyUIState, ready)) != 0
        || memcmp(&next.selected, &sNativePartyUI.selected, offsetof(HGNativePartyUIState, mons) - offsetof(HGNativePartyUIState, selected)) != 0;
    for (u32 i = 0; i < HG_PARTY_UI_SLOTS; ++i) {
        if (memcmp(&next.mons[i], &sNativePartyUI.mons[i], offsetof(HGNativePartyUIMon, iconX)) != 0) changed = TRUE;
    }
    for (u32 i = 0; i < HG_PARTY_UI_ACTIONS; ++i) {
        if (memcmp(&next.actions[i], &sNativePartyUI.actions[i], offsetof(HGNativePartyUIAction, text)) != 0) changed = TRUE;
    }
    if (changed) ++next.revision;
    sNativePartyUI = next;
}

const HGNativePartyUIState *HGNativePartyUIGetState(void) { return &sNativePartyUI; }
#endif
