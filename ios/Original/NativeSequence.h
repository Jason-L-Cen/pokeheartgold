// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef HEARTGOLD_NATIVE_SEQUENCE_H
#define HEARTGOLD_NATIVE_SEQUENCE_H
#include <stdint.h>
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
enum { HG_SEQUENCE_PLAYERS=16, HG_SEQUENCE_TRACKS=32, HG_SEQUENCE_NO_TRACK=255 };
enum { HG_SEQUENCE_CLOCK=33513982, HG_SEQUENCE_TICK_CLOCKS=64*2728 };
enum { HG_SEQUENCE_NOTE,HG_SEQUENCE_STEP,HG_SEQUENCE_RELEASE,HG_SEQUENCE_KILL };
enum { HG_SEQUENCE_OK,HG_SEQUENCE_BOUNDS,HG_SEQUENCE_COMMAND,HG_SEQUENCE_STACK,HG_SEQUENCE_VARIABLE,HG_SEQUENCE_LIMIT,HG_SEQUENCE_CALLBACK };
typedef struct HGSequenceTrack {
    uint32_t position,stack[3];
    int32_t wait;
    uint16_t program,channels,modDelay;
    int16_t extFader,extPitch,sweepPitch;
    uint8_t allocated,active,ended,player,number,depth,stackType[3],loops[3];
    uint8_t noteWait,tie,muted,comparison,portamento,portaKey,portaTime;
    uint8_t volume,expression,bendRange,priority,attack,decay,sustain,release;
    uint8_t modType,modSpeed,modDepth,modRange,panRange;
    int8_t pan,extPan,pitchBend,transpose;
} HGSequenceTrack;
typedef struct HGSequencePlayer {
    const uint8_t *data;
    size_t bytes;
    uint32_t bank,ticks;
    uint16_t tempo,ratio,counter,trackMask;
    int16_t extFader,variables[16];
    uint8_t active,prepared,paused,priority,volume,tracks[16];
} HGSequencePlayer;
typedef struct HGSequenceEvent {
    uint8_t kind,track,key,velocity,tie;
    int32_t duration;
} HGSequenceEvent;
typedef int (*HGSequenceCallback)(void *,const HGSequenceEvent *);
typedef struct HGNativeSequence {
    HGSequencePlayer players[HG_SEQUENCE_PLAYERS];
    HGSequenceTrack tracks[HG_SEQUENCE_TRACKS];
    int16_t globals[16];
    uint32_t random;
    struct { uint32_t code,player,track,position,command; } error;
    HGSequenceCallback callback;
    void *context;
} HGNativeSequence;
void HGNativeSequenceInit(HGNativeSequence *,HGSequenceCallback,void *);
// Owner thread only. A callback is required; it may inspect state but must not reenter/mutate the engine.
// Borrows the sequence command bytes (after the SSEQ/SSAR DATA header) until Stop/replacement.
int HGNativeSequencePrepare(HGNativeSequence *,unsigned player,const void *,size_t bytes,uint32_t start,uint32_t bank);
int HGNativeSequenceStart(HGNativeSequence *,unsigned player);
int HGNativeSequencePause(HGNativeSequence *,unsigned player,int paused);
int HGNativeSequenceStop(HGNativeSequence *,unsigned player);
// One original sound-driver clock (64*2728 system clocks). This advances score events, not PCM.
// A failed tick must be followed by Stop/reset before further score execution.
int HGNativeSequenceTick(HGNativeSequence *);
// Score commands ended; voices can still have pending note/envelope tails.
int HGNativeSequenceEnded(const HGNativeSequence *,unsigned player);
#ifdef __cplusplus
}
#endif
#endif
