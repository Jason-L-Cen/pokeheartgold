#ifndef POKEHEARTGOLD_WIFI_SETUP_SCROLL_BAR_H
#define POKEHEARTGOLD_WIFI_SETUP_SCROLL_BAR_H

#include "global.h"

#include "wifi_setup/wifi_setup_ui.h"

// Screen helpers of the access-point setup: a copy of the BG 2 screen flushed by a task when dirty, palette uploads,
// the vertical scroll bar and the access-point scan work allocation.

// scroll bar parts (WifiSetupScrollBar_GetPartRect)
#define WIFI_SETUP_SCROLL_BAR_THUMB 1
#define WIFI_SETUP_SCROLL_BAR_UP    2
#define WIFI_SETUP_SCROLL_BAR_DOWN  3
#define WIFI_SETUP_SCROLL_BAR_TRACK 4

// WifiSetupScrollBar.state
#define WIFI_SETUP_SCROLL_BAR_STATE_IDLE     0
#define WIFI_SETUP_SCROLL_BAR_STATE_DRAGGING 1
#define WIFI_SETUP_SCROLL_BAR_STATE_UP       2
#define WIFI_SETUP_SCROLL_BAR_STATE_DOWN     3

// WifiSetupScrollBar.event (WifiSetupScrollBar_GetEvent), set for one frame
#define WIFI_SETUP_SCROLL_BAR_EVENT_NONE          0
#define WIFI_SETUP_SCROLL_BAR_EVENT_GRABBED       1
#define WIFI_SETUP_SCROLL_BAR_EVENT_DRAGGED       2
#define WIFI_SETUP_SCROLL_BAR_EVENT_RELEASED      3 // also: the track was touched
#define WIFI_SETUP_SCROLL_BAR_EVENT_UP_PRESSED    4
#define WIFI_SETUP_SCROLL_BAR_EVENT_UP_RELEASED   5
#define WIFI_SETUP_SCROLL_BAR_EVENT_DOWN_PRESSED  6
#define WIFI_SETUP_SCROLL_BAR_EVENT_DOWN_RELEASED 7

// BG 2 screen copy.
typedef struct WifiSetupBgScreen {
    void *task;
    u16 data[0x300];
    u8 dirty;
} WifiSetupBgScreen;

typedef struct WifiSetupBgScreenState {
    WifiSetupBgScreen *screen;
    void *paletteSrc;
    void *paletteDst;
} WifiSetupBgScreenState;

// Vertical scroll bar.
typedef struct WifiSetupScrollBar {
    WifiSetupOamBlock *thumb;
    int unk_04;
    int unk_08;
    void *task;
    WifiSetupPoint pos;        // top of the track
    WifiSetupPoint touchStart; // touch position when the drag started
    u8 dragStart;              // value when the drag started
    u8 length;                 // track length
    u8 value;                  // thumb offset from the top of the track
    u8 kind;
    u8 state; // WIFI_SETUP_SCROLL_BAR_STATE_*
    u8 event; // WIFI_SETUP_SCROLL_BAR_EVENT_*
    u8 disabled;
} WifiSetupScrollBar;

extern WifiSetupBgScreenState gWifiSetupBgScreen;
extern WifiSetupScrollBar *gWifiSetupScrollBar;

void WifiSetupBgScreen_Create(void *src);
void WifiSetupBgScreen_Destroy(void);
void WifiSetupBgScreen_MarkDirty(void);
void WifiSetupBgScreen_CopyRect(u16 *src, int offset, int width, int rows);
void WifiSetupBgScreen_UpdateTask(WifiSetupTask *task, void *arg);
void WifiSetupPalette_LoadRow(u8 *src, int index, int palette);
void WifiSetupPalette_Load(void *src);

void WifiSetupScrollBar_Create(int kind, int length, int x, int y, int value);
void WifiSetupScrollBar_Destroy(void);
u8 WifiSetupScrollBar_GetValue(void);
u8 WifiSetupScrollBar_GetEvent(void);
void WifiSetupScrollBar_SetValue(int value);
void WifiSetupScrollBar_Enable(void);
void WifiSetupScrollBar_Disable(void);

void WifiSetupApScan_AllocWork(void);
void WifiSetupApScan_FreeWork(void);
void WifiSetupScrollBar_Task(WifiSetupTask *task, void *arg);

#endif // POKEHEARTGOLD_WIFI_SETUP_SCROLL_BAR_H
