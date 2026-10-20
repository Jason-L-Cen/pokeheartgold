// SPDX-License-Identifier: GPL-3.0-or-later
#include "NativeInstrument.h"
#include <string.h>
static uint16_t u16(const uint8_t *p) { return (uint16_t)(p[0]|(uint16_t)p[1]<<8); }
static uint32_t u32(const uint8_t *p) { return u16(p)|(uint32_t)u16(p+2)<<16; }
static int contains(size_t bytes,size_t offset,size_t count) { return offset<=bytes && count<=bytes-offset; }
_Static_assert(sizeof(HGNativeInstrument)==12,"Original instrument record");
int HGNativeReadInstrument(const void *data,size_t bytes,uint32_t program,uint32_t key,HGNativeInstrument *output) {
    if (!data || !output || key>127 || bytes<60) return HG_INSTRUMENT_INVALID;
    const uint8_t *bank=data;
    uint32_t size=u32(bank+8),count=u32(bank+56);
    if (memcmp(bank,"SBNK\xFF\xFE\x00\x01",8) || u16(bank+12)!=16 || u16(bank+14)!=1 ||
        size<60 || size>bytes || memcmp(bank+16,"DATA",4) || u32(bank+20)!=size-16 ||
        count>(size-60)/4) return HG_INSTRUMENT_INVALID;
    if (program>=count) return HG_INSTRUMENT_MISSING;
    uint32_t entry=u32(bank+60+program*4),offset=entry>>8,type=entry&255;
    if (!type) return HG_INSTRUMENT_MISSING;
    if (offset<60+count*4) return HG_INSTRUMENT_INVALID;
    HGNativeInstrument result={0}; result.type=(uint8_t)type;
    if (type>=1 && type<=5) {
        if (!contains(size,offset,10)) return HG_INSTRUMENT_INVALID;
        memcpy((uint8_t *)&result+2,bank+offset,10);
    } else if (type==16) {
        if (!contains(size,offset,2)) return HG_INSTRUMENT_INVALID;
        uint32_t first=bank[offset],last=bank[offset+1];
        if (last>127 || first>last || !contains(size,offset+2,(last-first+1)*12)) return HG_INSTRUMENT_INVALID;
        if (key<first || key>last) return HG_INSTRUMENT_MISSING;
        memcpy(&result,bank+offset+2+(key-first)*12,12);
    } else if (type==17) {
        if (!contains(size,offset,8)) return HG_INSTRUMENT_INVALID;
        uint32_t ranges=0,selected=8,previous=0;
        for (;ranges<8 && bank[offset+ranges];ranges++) {
            uint32_t high=bank[offset+ranges];
            if (high>127 || (ranges && high<=previous)) return HG_INSTRUMENT_INVALID;
            if (selected==8 && key<=high) selected=ranges;
            previous=high;
            // Original banks can retain unrelated bytes after a range ending at MIDI key 127.
            if (high==127) { ranges++; break; }
        }
        if (!contains(size,offset+8,ranges*12)) return HG_INSTRUMENT_INVALID;
        if (selected==8) return HG_INSTRUMENT_MISSING;
        memcpy(&result,bank+offset+8+selected*12,12);
    } else return HG_INSTRUMENT_INVALID;
    if (!result.type) return HG_INSTRUMENT_MISSING;
    if (result.type>5) return HG_INSTRUMENT_INVALID;
    // Decode little-endian fields independently of the host alignment and byte order.
    const uint8_t *raw=(const uint8_t *)&result;
    result.wave=u16(raw+2); result.archive=u16(raw+4);
    memcpy(output,&result,sizeof(result)); return HG_INSTRUMENT_FOUND;
}
