// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef HEARTGOLD_NATIVE_INSTRUMENT_H
#define HEARTGOLD_NATIVE_INSTRUMENT_H
#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct HGNativeInstrument {
    uint8_t type,reserved;
    uint16_t wave,archive;
    uint8_t rootKey,attack,decay,sustain,release,pan;
} HGNativeInstrument;
enum { HG_INSTRUMENT_INVALID=-1, HG_INSTRUMENT_MISSING, HG_INSTRUMENT_FOUND };
// Accepts original SBNK bytes, including runtime wave-archive links. Failure/missing leave output untouched.
int HGNativeReadInstrument(const void *bank,size_t bytes,uint32_t program,uint32_t key,HGNativeInstrument *output);
#ifdef __cplusplus
}
#endif
#endif
