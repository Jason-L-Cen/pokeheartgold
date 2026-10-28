// SPDX-License-Identifier: GPL-3.0-or-later
#include "native_oak_ui.h"
#include <stddef.h>
#include <string.h>
#define CHECK(v) do { if (!(v)) return __LINE__; } while (0)
uint32_t CheckNativeOakUI(void) {
    CHECK(sizeof(HGNativeOakUIState)==72 && offsetof(HGNativeOakUIState,textHeight)==68);
    HGNativeOakUIState s={.active=1,.ownerGeneration=7,.revision=9,.ready=1,.parentPhase=1,.phase=3,.mode=HG_OAK_MENU,.keyMask=195,.touchMode=1,.count=3};
    CHECK(HGNativeOakUIOwnsInput(&s,0) && !HGNativeOakUIOwnsInput(&s,1));
    CHECK(!HGNativeOakUIOwnsInput(NULL,0));
    const uint32_t keys[]={0,1,2,3,8,16,32,48,64,128,192,UINT32_MAX};
    for (uint32_t mode=0;mode<=6;mode++) for (uint32_t parent=0;parent<=6;parent++) for (uint32_t ready=0;ready<3;ready++)
    for (size_t i=0;i<sizeof(keys)/sizeof(*keys);i++) {
        HGNativeOakUIState q=s,before;q.mode=mode;q.parentPhase=parent;q.ready=ready;q.keyMask=243;before=q;
        uint32_t mask=mode==HG_OAK_DIALOG || mode==HG_OAK_PAGE ? 3 : mode==HG_OAK_GENDER ? 49 : mode==HG_OAK_MENU || mode==HG_OAK_YESNO ? 195 : 0;
        uint32_t key=keys[i],expected=parent==1 && ready==1 && mask && key && !(key&(key-1)) && (key&mask);
        CHECK(!!HGNativeOakUIKeyAllowed(&q,7,9,key)==!!expected && !memcmp(&q,&before,sizeof(q)));
        CHECK(!HGNativeOakUIKeyAllowed(&q,6,9,key) && !HGNativeOakUIKeyAllowed(&q,7,8,key));
    }
    uint32_t x,y;
    for (uint32_t i=0;i<3;i++) {CHECK(HGNativeOakUIResolveChoice(&s,7,9,1,i,&x,&y));CHECK(x>=50 && x<213 && y>=20+56*i && y<50+56*i);}
    CHECK(!HGNativeOakUIResolveChoice(&s,7,9,0,0,&x,&y) && !x && !y);
    CHECK(!HGNativeOakUIResolveChoice(&s,7,9,1,3,&x,&y) && !x && !y);
    s.mode=HG_OAK_GENDER;s.phase=65;s.count=2;s.keyMask=49;
    for (uint32_t i=0;i<2;i++){CHECK(HGNativeOakUIResolveChoice(&s,7,9,1,i,&x,&y));CHECK(y>=25 && y<173 && (i ? x>=144 && x<239 : x>=18 && x<111));}
    s.mode=HG_OAK_MENU;s.phase=69;s.keyMask=195;
    for(uint32_t gender=0;gender<2;gender++){s.gender=gender;for(uint32_t i=0;i<2;i++) {CHECK(HGNativeOakUIResolveChoice(&s,7,9,1,i,&x,&y));CHECK(gender ? x>=10 && x<125 : x>=138 && x<253);CHECK(i ? y>=108 && y<164 : y>=26 && y<83);}}
    s.mode=HG_OAK_YESNO;s.phase=24;
    for(uint32_t i=0;i<2;i++){CHECK(HGNativeOakUIResolveChoice(&s,7,9,1,i,&x,&y));CHECK(x>=3 && x<251 && (i ? y>=99 && y<140 : y>=50 && y<92));}
    s.mode=HG_OAK_PAGE;s.phase=11;s.count=0;s.keyMask=3;s.touchMode=HG_OAK_TOUCH_ADVANCE|HG_OAK_ADVANCE_VISIBLE;
    CHECK(HGNativeOakUITouchAllowed(&s,7,9,1,211,167) && !HGNativeOakUITouchAllowed(&s,7,9,0,211,167));
    CHECK(!HGNativeOakUITouchAllowed(&s,7,9,1,0,0) && !HGNativeOakUITouchAllowed(&s,7,9,1,256,192));
    s.parentPhase=4;CHECK(HGNativeOakUIOwnsInput(&s,0) && !HGNativeOakUIKeyAllowed(&s,7,9,1) && !HGNativeOakUITouchAllowed(&s,7,9,1,211,167));
    return 0;
}
