// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef HEARTGOLD_NATIVE_OAK_UI_H
#define HEARTGOLD_NATIVE_OAK_UI_H
#include <stdint.h>

enum { HG_OAK_NONE, HG_OAK_DIALOG, HG_OAK_PAGE, HG_OAK_MENU, HG_OAK_GENDER, HG_OAK_YESNO };
enum { HG_OAK_TOUCH_ANY=1, HG_OAK_TOUCH_ADVANCE=2, HG_OAK_ADVANCE_VISIBLE=4 };
typedef struct HGNativeOakUIState {
    uint32_t active, ownerGeneration, revision, ready;
    uint32_t parentPhase, phase, mode, keyMask, touchMode;
    uint32_t cursor, count, gender, padMode, textKind, textX, textY, textWidth, textHeight;
} HGNativeOakUIState;

const HGNativeOakUIState *HGNativeOakUIGetState(void);
#ifdef HEARTGOLD_NATIVE
void NativeOakUIBegin(const void *owner);
void NativeOakUIPublish(const void *owner, const HGNativeOakUIState *facts);
void NativeOakUIRetire(const void *owner);
#endif

static inline uint32_t HGNativeOakUIOwnsInput(const HGNativeOakUIState *s, uint32_t namingActive) {
    return s && s->active == 1 && s->parentPhase <= 5 && !namingActive;
}
static inline uint32_t HGNativeOakUIInputAllowed(const HGNativeOakUIState *s, uint32_t generation, uint32_t revision) {
    return HGNativeOakUIOwnsInput(s, 0) && s->parentPhase == 1 && s->ready == 1
        && s->mode >= HG_OAK_DIALOG && s->mode <= HG_OAK_YESNO && s->phase <= 126
        && generation && revision && s->ownerGeneration == generation && s->revision == revision;
}
static inline uint32_t HGNativeOakUIKeyAllowed(const HGNativeOakUIState *s, uint32_t generation, uint32_t revision, uint32_t key) {
    uint32_t allowed = !s ? 0 : s->mode == HG_OAK_DIALOG || s->mode == HG_OAK_PAGE ? 3
        : s->mode == HG_OAK_GENDER ? 49 : s->mode == HG_OAK_MENU || s->mode == HG_OAK_YESNO ? 195 : 0;
    return HGNativeOakUIInputAllowed(s,generation,revision) && key && !(key & (key-1))
        && (key & allowed) && (s->keyMask & key);
}
static inline uint32_t HGNativeOakUITouchAllowed(const HGNativeOakUIState *s, uint32_t generation, uint32_t revision,
    uint32_t touchEngine, uint32_t x, uint32_t y) {
    if (!HGNativeOakUIInputAllowed(s,generation,revision) || touchEngine != 1 || x >= 256 || y >= 192) return 0;
    uint32_t mode=s->touchMode & 3;
    if (mode == HG_OAK_TOUCH_ADVANCE) return (s->mode == HG_OAK_DIALOG || s->mode == HG_OAK_PAGE) && x >= 168 && y >= 144;
    return mode == HG_OAK_TOUCH_ANY && (s->mode == HG_OAK_MENU || s->mode == HG_OAK_GENDER || s->mode == HG_OAK_YESNO);
}
typedef struct HGNativeOakRect { uint32_t x,y,width,height; } HGNativeOakRect;
// Original hitbox rectangles; native cards submit their centres to original hit testing.
static inline HGNativeOakRect HGNativeOakUIChoiceRect(const HGNativeOakUIState *s, uint32_t index) {
    HGNativeOakRect r={0};
    if (!s || index >= s->count) return r;
    if (s->phase >= 64 && s->phase <= 66 && s->count == 2) return (HGNativeOakRect){index ? 144 : 18,25,index ? 95 : 93,148};
    if (s->phase == 24 && s->count == 2) return (HGNativeOakRect){3,index ? 99 : 50,248,index ? 41 : 42};
    if (s->count == 3) return (HGNativeOakRect){50,20+56*index,163,30};
    if (s->count == 2) return (HGNativeOakRect){s->gender ? 10 : 138,index ? 108 : 26,115,index ? 56 : 57};
    return r;
}
static inline uint32_t HGNativeOakUIResolveChoice(const HGNativeOakUIState *s, uint32_t generation, uint32_t revision,
    uint32_t touchEngine, uint32_t index, uint32_t *x, uint32_t *y) {
    if (x) *x=0; if (y) *y=0;
    if (!x || !y) return 0;
    HGNativeOakRect r=HGNativeOakUIChoiceRect(s,index);
    uint32_t px=r.x+r.width/2,py=r.y+r.height/2;
    if (!r.width || !r.height || !HGNativeOakUITouchAllowed(s,generation,revision,touchEngine,px,py)) return 0;
    *x=px; *y=py; return 1;
}
#endif
