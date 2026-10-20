// SPDX-License-Identifier: GPL-3.0-or-later
#include <SND_bank.h>
#include <nitro/snd/native_sound.h>
#include <stddef.h>
#include <string.h>
#define CHECK(v) do { if (!(v)) return __LINE__; } while (0)
_Static_assert(offsetof(struct SNDBankData,waveArcLinks)==24 && offsetof(struct SNDBankData,instCount)==56,"Original bank links");
_Static_assert(offsetof(struct SNDWaveArc,waveOffsets)==60 && sizeof(struct SNDInstData)==12,"Original wave/patch layouts");
static struct { struct SNDWaveArc header; u32 offsets[2]; struct SNDWaveData wave; u32 pcm[2]; } archive;
static struct { struct SNDWaveData header; u32 pcm[2]; } separate;
static struct SNDBankData banks[2];
u32 CheckNativeSoundBank(void) {
    memset(&archive,0,sizeof(archive)); memset(&separate,0,sizeof(separate)); memset(banks,0,sizeof(banks));
    struct SNDWaveArc *war=&archive.header;
    war->waveCount=2; archive.offsets[0]=(u32)((u8 *)&archive.wave-(u8 *)war);
    CHECK((u32)&separate<0x02000000u);
    CHECK(SND_GetWaveDataCount(war)==2 && SND_GetWaveDataAddress(war,0)==&archive.wave && !SND_GetWaveDataAddress(war,1));
    SND_SetWaveDataAddress(war,1,&separate.header);
    CHECK(archive.offsets[1]==((u32)&separate.header|HG_SOUND_WAVE_ADDRESS_FLAG));
    CHECK(SND_GetWaveDataAddress(war,1)==&separate.header && SND_GetWaveDataAddress(war,0)==&archive.wave);
    SND_AssignWaveArc(&banks[0],0,war); SND_AssignWaveArc(&banks[1],3,war);
    CHECK(war->waveArcLLHead==&banks[1].waveArcLinks[3]);
    CHECK(banks[1].waveArcLinks[3].waveArcLLnext==&banks[0].waveArcLinks[0]);
    SND_AssignWaveArc(&banks[1],3,war); CHECK(war->waveArcLLHead==&banks[1].waveArcLinks[3]);
    SND_DestroyBank(&banks[1]); CHECK(war->waveArcLLHead==&banks[0].waveArcLinks[0]);
    memset(&banks[1],0,sizeof(banks[1]));
    SND_AssignWaveArc(&banks[1],1,war);
    SND_DestroyWaveArc(war);
    CHECK(!banks[0].waveArcLinks[0].waveArc && !banks[1].waveArcLinks[1].waveArc);
    SND_SetWaveDataAddress(war,1,NULL); CHECK(!archive.offsets[1] && !SND_GetWaveDataAddress(war,1));
    return 0;
}
