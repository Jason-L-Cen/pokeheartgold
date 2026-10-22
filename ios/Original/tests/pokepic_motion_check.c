// SPDX-License-Identifier: GPL-3.0-or-later
#include "../../../src/pokepic_anim.c"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(value) do { if (!(value)) return __LINE__; } while (0)

u32 CheckOriginalPokepicMotion(void) {
    for (u32 type=0;type<5;type++) {
        PokepicScriptAnim anim; memset(&anim,0,sizeof(anim));
        PokepicMotion *motion=PokepicScriptAnim_AllocMotion(&anim,type);
        CHECK(motion==&anim.motions[0] && motion->active && motion->update==sPokepicMotionTypes[type].update);
        motion->value=&motion->values[0]; motion->dest=&anim.dx;
        motion->op=0x19; motion->base=100;
        if (type<2) {
            motion->wave.waveform=0x1F; motion->wave.amplitude=16; motion->wave.duration=2;
        } else if (type==3) {
            motion->linear.total=8; motion->linear.duration=2;
        } else {
            motion->accel.velocity=3; motion->accel.acceleration=1;
            motion->accel.durationOrLimit=type==4 ? 107 : 2;
        }
        motion->update(motion,&anim);
        CHECK(motion->active && anim.dx==100+(type<2 ? 16 : type==3 ? 4 : 3));
        motion->update(motion,&anim);
        CHECK(!motion->active && anim.dx==100+(type<2 ? 16 : type==3 ? 8 : 7));
    }
    return 0;
}

#ifndef HEARTGOLD_ABI32
void HGNativeAssertFail(const char *condition,const char *file,int line) { fprintf(stderr,"%s:%d: %s\n",file,line,condition); abort(); }
int main(void) {
    u32 line=CheckOriginalPokepicMotion();
    if (line) { fprintf(stderr,"Pokepic motion check line %u\n",line); return 1; }
    puts("PASS: all five original Pokepic motion callbacks, interpolation, limits and completion.");
    return 0;
}
#endif
