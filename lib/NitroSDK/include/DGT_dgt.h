#ifndef NITRO_DGT_DGT_H_
#define NITRO_DGT_DGT_H_

#include "nitro/types.h"

struct DGTHash2Context;

typedef struct DgtHmacParams {
    int digestSize;
    int blockSize;
    struct DGTHash2Context *context;
    u8 *digest;
    void (*reset)(struct DGTHash2Context *context);
    void (*setSource)(struct DGTHash2Context *context, u8 *input, u32 length);
    void (*getDigest)(struct DGTHash2Context *context, u8 *digest);
} DgtHmacParams;

void DGT_Hash2CalcHmac(void* digest, const void* bin_ptr, u32 bin_len, const void* key_ptr, u32 keylen);

#endif //NITRO_DGT_DGT_H_
