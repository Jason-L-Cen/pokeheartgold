#ifndef GUARD_DGT_H
#define GUARD_DGT_H

#include "nitro/types.h"

struct DGTHash1Context {
    union {
        struct {
            u32 a, b, c, d;
        };
        u32 state[4];
    };
    u64 length;
    union {
        u32 buffer32[16];
        u8 buffer8[64];
    };
};

struct DGTHash2Context {
    u32 Intermediate_Hash[5];
    u32 Length_Low;
    u32 Length_High;
    int Message_Block_Index;
    union {
        u8 Message_Block[64];
        u32 Message_Block_Words[16];
    };
    int Computed;
    int Corrupted;
};

void DGT_Hash1Reset(struct DGTHash1Context *context);
void DGT_Hash1SetSource(struct DGTHash1Context *context, u8 *input, u32 length);
void DGT_Hash1GetDigest_R(u8 *digest, struct DGTHash1Context *context);
void DGT_Hash2Reset(struct DGTHash2Context *context);
void DGT_Hash2SetSource(struct DGTHash2Context *context, u8 *input, u32 length);
void DGT_Hash2GetDigest(struct DGTHash2Context *context, u8 *digest);
void DGTi_hash2_arm4_small(struct DGTHash2Context *context, const void *input, u32 length);

#endif
