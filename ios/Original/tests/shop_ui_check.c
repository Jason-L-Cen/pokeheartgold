// SPDX-License-Identifier: GPL-3.0-or-later
#include "native_shop_ui.h"
#include <stddef.h>
#include <string.h>
#define CHECK(value) do {if(!(value))return __LINE__;} while(0)
uint32_t CheckNativeShopUI(void) {
    CHECK(sizeof(HGNativeShopUIState)==1268 && offsetof(HGNativeShopUIState,money)==800);
    HGNativeShopUIState s={.active=1,.ownerGeneration=7,.revision=11,.ready=1,.viewActive=1,.phase=HG_SHOP_UI_LIST,.visibleCount=1};
    s.items[0]=(HGNativeShopUIItem){.itemId=17,.enabled=1,.hitX=63,.hitY=52,.cropWidth=127,.cropY=32,.cropHeight=41};
    HGNativeShopUIState before=s;uint32_t key=123,x=123,y=123;
    CHECK(HGNativeShopUIInputAllowed(&s,7,11) && !HGNativeShopUIInputAllowed(&s,6,11) && !HGNativeShopUIInputAllowed(&s,7,10));
    CHECK(HGNativeShopUIResolveTouch(&s,7,11,HG_SHOP_UI_TOUCH_ITEM,0,&x,&y) && x==63 && y==52);
    CHECK(!HGNativeShopUIResolveTouch(&s,7,10,HG_SHOP_UI_TOUCH_ITEM,0,&x,&y) && !HGNativeShopUIResolveTouch(&s,7,11,HG_SHOP_UI_TOUCH_ITEM,1,&x,&y));
    CHECK(HGNativeShopUIResolveKey(&s,7,11,1,&key) && key==1 && !HGNativeShopUIResolveKey(&s,7,11,8,&key));
    CHECK(!memcmp(&s,&before,sizeof(s)));
    s.items[0].cropWidth=0;CHECK(!HGNativeShopUIResolveTouch(&s,7,11,HG_SHOP_UI_TOUCH_ITEM,0,&x,&y));s=before;
    s.kind=1;CHECK(!HGNativeShopUIInputAllowed(&s,7,11));s.kind=0;
    s.ready=0;CHECK(!HGNativeShopUIResolveKey(&s,7,11,1,&key));s.ready=1;
    s.phase=HG_SHOP_UI_CONFIRM;
    CHECK(HGNativeShopUIResolveKey(&s,7,11,64,&key) && HGNativeShopUIResolveKey(&s,7,11,128,&key));
    CHECK(HGNativeShopUIResolveKey(&s,7,11,1,&key) && HGNativeShopUIResolveKey(&s,7,11,2,&key));
    CHECK(!HGNativeShopUIResolveKey(&s,7,11,16,&key) && !HGNativeShopUIResolveKey(&s,7,11,32,&key));
    s.phase=HG_SHOP_UI_MESSAGE;CHECK(HGNativeShopUIResolveKey(&s,7,11,1,&key) && !HGNativeShopUIResolveKey(&s,7,11,64,&key));
    s.phase=HG_SHOP_UI_PURCHASE_PRINT;
    CHECK(HGNativeShopUIInputAllowed(&s,7,11) && HGNativeShopUIResolveKey(&s,7,11,1,&key) && key==1);
    CHECK(HGNativeShopUIResolveKey(&s,7,11,2,&key) && key==2);
    CHECK(!HGNativeShopUIResolveKey(&s,7,11,16,&key) && !HGNativeShopUIResolveKey(&s,7,11,32,&key)
        && !HGNativeShopUIResolveKey(&s,7,11,64,&key) && !HGNativeShopUIResolveKey(&s,7,11,128,&key));
    s.buttonCount=1;s.buttons[0]=(HGNativeShopUIButton){.enabled=1,.hitX=15,.hitY=15,.cropWidth=31,.cropHeight=31};
    CHECK(!HGNativeShopUIResolveTouch(&s,7,11,HG_SHOP_UI_TOUCH_BUTTON,0,&x,&y));
    CHECK(!HGNativeShopUIResolveTouch(&s,7,11,HG_SHOP_UI_TOUCH_ITEM,0,&x,&y));
    CHECK(!HGNativeShopUIResolveKey(&s,7,10,1,&key) && !HGNativeShopUIResolveKey(&s,6,11,1,&key));
    s.ready=0;CHECK(!HGNativeShopUIResolveKey(&s,7,11,1,&key));s.ready=1;
    s.viewActive=0;CHECK(!HGNativeShopUIResolveKey(&s,7,11,1,&key));s.viewActive=1;
    s.phase=5;CHECK(!HGNativeShopUIInputAllowed(&s,7,11));s.phase=10;CHECK(!HGNativeShopUIInputAllowed(&s,7,11));
    s.phase=21;CHECK(!HGNativeShopUIInputAllowed(&s,7,11));
    return 0;
}
#ifdef HG_NATIVE_SHOP_UI_CHECK_MAIN
#include <stdio.h>
int main(void){uint32_t line=CheckNativeShopUI();if(line)fprintf(stderr,"Shop UI check line %u\n",line);return line ? 1 : 0;}
#endif
