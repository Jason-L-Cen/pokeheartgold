#include "global.h"
#include <nitro/code32.h>
#pragma force_active on
#pragma opt_strength_reduction off
extern u8 *ov13_0224F4BC;extern u8 ov13_02244FA4[];
void ov13_022290F0(int,int,int);void ov13_022293F0(int,int);void ov13_0222936C(int);
// @fn ov13_0222936C
void ov13_0222936C(int mode){int i;int first=0,second=0;u32 offset;
 ov13_0224F4BC[0x11d]=(u8)mode;
 for(i=0,offset=0;i<4;i++,offset+=12)ov13_022290F0(mode,i,*(u16*)(ov13_02244FA4+4*offset+2));
 if(mode==2)first=1;else if(mode==1)second=1;
 ov13_022293F0(47,first);ov13_022293F0(48,second);
}
