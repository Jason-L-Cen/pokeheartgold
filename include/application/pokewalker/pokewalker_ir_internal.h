#ifndef POKEHEARTGOLD_APPLICATION_POKEWALKER_POKEWALKER_IR_INTERNAL_H
#define POKEHEARTGOLD_APPLICATION_POKEWALKER_POKEWALKER_IR_INTERNAL_H

#include "application/pokewalker/pokewalker_ir.h"

// IR link layer state.
typedef struct PokewalkerIrLink {
    u8 unk_00;
    u8 unk_01[3];
    u32 sessionId;
    u32 connected;
    void (*onConnect)(int result, int arg);
    u32 numDisconnects;
    void (*onClose)(int result);
    u32 closing;
    void (*onReceive)(void *data, u8 size, u8 command, u8 arg);
    u32 ownId;
    u32 handshakeStarted;
    u32 handshakeState;
    u32 active;
    u64 lastReceiveTick;
} PokewalkerIrLink;

// Header of an IR packet; the payload follows (gPokewalkerIrPayload).
typedef struct PokewalkerIrPacket {
    u8 command;
    u8 arg;
    u8 checksumLo;
    u8 checksumHi;
    u32 sessionId;
} PokewalkerIrPacket;

// IR transfer work (0x021FFAA4..). The transfer state below is addressed from 0x021FFB24.
#ifdef HEARTGOLD_ABI32
#pragma pack(push,4)
#endif
typedef struct PokewalkerIrWork {
    u16 unk_00;
    u16 unk_02;
    u32 blockIndex;
    void *walkerMon;
    void *unk_0C;
    void *uploadImage;
    int error;
    u32 unk_18;
    u32 unk_1C;
    PokewalkerIrIdentity *identity;
    PokewalkerIrIdentity *peerIdentity;
    void *unk_28;
    void *strollData;
    u8 *records;
    PokewalkerIrBlocks *blocks; // 0x34
    PokewalkerIrStatus *status; // 0x38
    u64 transferStartTick;      // 0x3C
    u8 *compressedSizes;        // 0x44
    u8 *compressBuffer;         // 0x48
    u8 *compressedImageSizes;   // 0x4C
    void *compressedImage;      // 0x50
    u8 *compressedStrollSizes;  // 0x54
    void *compressedStroll;     // 0x58
#ifdef HEARTGOLD_ABI32
    u8 *sizes2;                 // 0x5C
    void *upload2;              // 0x60
#endif
} PokewalkerIrWork;
#ifdef HEARTGOLD_ABI32
#pragma pack(pop)
#endif

typedef struct PokewalkerIrTransfer {
    u8 unk_00[0x1C];
    u8 command;
    u8 step;
    u8 unk_1E[2];
    u8 *data; // source data (send) or Pokewalker address (receive)
    u8 *dataStart;
    u32 address; // Pokewalker address (send) or destination (receive)
    u32 addressStart;
    u16 remaining;
    u16 size;
    u16 numChunks;
    u16 chunk;
    u16 totalChunks;
    u16 totalChunk;
    volatile u16 totalSize;
} PokewalkerIrTransfer;

extern u8 gPokewalkerIrLinkArgs[];
extern PokewalkerIrLink gPokewalkerIrLink;
extern PokewalkerIrPacket gPokewalkerIrPacket;
extern u8 gPokewalkerIrPayload[];
#ifdef HEARTGOLD_ABI32
// These labels address overlapping views of one original work area (0x021FFAA4..0x021FFBD8).
typedef struct PokewalkerIrNativeState {
    PokewalkerIrWork work;
    OSMutex mutex;
    union {
        OSMessageQueue queue;
        struct { u32 queueHead; PokewalkerIrTransfer transfer; u8 buffer[0x74]; } views;
    };
} PokewalkerIrNativeState;
extern PokewalkerIrNativeState gNativePokewalkerIrState;
#define gPokewalkerIrWork (gNativePokewalkerIrState.work)
#define gPokewalkerIrBuffers ((OSMessage *)&gNativePokewalkerIrState.work.unk_1C)
#define gPokewalkerIrImageSizes (&gNativePokewalkerIrState.work.compressedImageSizes)
#define gPokewalkerIrStrollSizes (&gNativePokewalkerIrState.work.compressedStrollSizes)
#define gPokewalkerIrSync (gNativePokewalkerIrState.mutex)
#define gPokewalkerIrMessageQueue (gNativePokewalkerIrState.queue)
#define gPokewalkerIrTransfer (gNativePokewalkerIrState.views.transfer)
#define gPokewalkerIrSendBuffer (gNativePokewalkerIrState.views.buffer)
#else
extern PokewalkerIrWork gPokewalkerIrWork;
extern OSMessage gPokewalkerIrBuffers[];
extern u8 *gPokewalkerIrImageSizes[];
extern u8 *gPokewalkerIrStrollSizes[];
extern OSMutex gPokewalkerIrSync;
extern OSMessageQueue gPokewalkerIrMessageQueue;
extern PokewalkerIrTransfer gPokewalkerIrTransfer;
extern u8 gPokewalkerIrSendBuffer[];
#endif
extern OSThread sPokewalkerIrThread;

#endif // POKEHEARTGOLD_APPLICATION_POKEWALKER_POKEWALKER_IR_INTERNAL_H
