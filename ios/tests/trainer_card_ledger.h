// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef HG_PRIVATE_TRAINER_CARD_LEDGER_H
#define HG_PRIVATE_TRAINER_CARD_LEDGER_H
#include <stdint.h>
#include <string.h>
// Independent reference; checked against the unchanged original PackCanvas in the portable C check.
static void TCQAPackCanvas(uint8_t output[1536],const uint8_t pixels[6144]) {
    memset(output,0,1536);
    for (uint32_t i=0;i<6144;++i) {
        uint32_t shift=(i%4)*2;
        output[i/4]|=((pixels[i]&15)==1)<<shift;
        output[i/4]|=((pixels[i]>>4)==1)<<(shift+1);
    }
}
static uint32_t TCQASignatureExists(const uint8_t payload[1536]) {
    uint32_t sum=0;
    for (uint32_t i=0;i<384;++i) {uint32_t word;memcpy(&word,payload+i*4,4);sum+=word;}
    return sum!=0;
}
static uint32_t TCQANext(uint32_t value) {return value==UINT32_MAX ? 1 : value+1;}
typedef struct TCQATouchPoint {uint32_t generation,revision,x,y,down,padReads;} TCQATouchPoint;
static uint32_t TCQAStrokeTraceValid(const TCQATouchPoint *points,uint32_t count) {
    if (!points || count!=6 || !points[0].generation || !points[0].revision || points[5].down)return 0;
    for(uint32_t i=0;i<count;++i) {
        if(points[i].generation!=points[0].generation || points[i].revision!=points[0].revision
            || points[i].down!=(i<5) || points[i].x>=256 || points[i].y>=192
            || (i && points[i].padReads-points[i-1].padReads<2))return 0;
    }
    return 1;
}
#endif
