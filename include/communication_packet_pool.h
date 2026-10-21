#ifndef POKEHEARTGOLD_COMMUNICATION_PACKET_POOL_H
#define POKEHEARTGOLD_COMMUNICATION_PACKET_POOL_H

#include "global.h"

// A ring of bytes: data is written at write, made readable up to limit by ByteRing_Commit, read from read.
typedef struct ByteRing {
    u8 *data;
    s16 read;
    volatile s16 limit;
    volatile s16 write;
    s16 capacity;
} ByteRing;

typedef struct PacketNode {
    u8 *data;
    struct PacketNode *prev;
    struct PacketNode *next;
    u16 size;
    u8 command; // 0: free node

    union {
        u8 raw;

        struct {
            u8 headerWritten : 1;
            u8 inRing : 1; // the data was copied into the pool's ring
            u8 rest : 6;
        } b;
    } flags;
} PacketNode;

typedef struct PacketQueue {
    PacketNode *head;
    PacketNode *tail;
} PacketQueue;

typedef struct PacketPool {
    PacketQueue queue;
    PacketQueue queue2;
    PacketNode *current; // partly written into the last frame
    ByteRing *ring;
    PacketNode *nodes;
    int count;
} PacketPool;

typedef struct ByteWriter {
    u8 *dest;
    int remaining;
} ByteWriter;

void ByteRing_Init(ByteRing *ring, u8 *data, int capacity);
void ByteRing_Write(ByteRing *ring, const u8 *src, int count, int unused);
int ByteRing_Read(ByteRing *ring, u8 *dst, int count);
u8 ByteRing_ReadByte(ByteRing *ring);
s32 ByteRing_GetReadableSize(const ByteRing *ring);
s32 ByteRing_GetFreeSize(const ByteRing *ring);
void ByteRing_Commit(ByteRing *ring);
BOOL PacketPool_AllNodesFree(const PacketPool *pool);
BOOL PacketPool_Enqueue(PacketPool *pool, int command, u8 *data, int size, int a4, int copyToRing);
BOOL PacketPool_WriteFrame(PacketPool *pool, ByteWriter *writer, int next);
void PacketPool_Init(PacketPool *pool, int count, ByteRing *ring);
void PacketPool_Reset(PacketPool *pool);
void PacketPool_Free(PacketPool *pool);
BOOL PacketPool_HasCommand(PacketPool *pool, int command);

#endif // POKEHEARTGOLD_COMMUNICATION_PACKET_POOL_H
