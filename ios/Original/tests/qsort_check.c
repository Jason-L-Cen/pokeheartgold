// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro/math/qsort.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(value) do { if (!(value)) return __LINE__; } while (0)

static int CompareByte(const void *a, const void *b) {
    return (*(const u8 *)a>*(const u8 *)b)-(*(const u8 *)a<*(const u8 *)b);
}

static int CompareRecord(const void *a, const void *b) {
    const u8 *first=a, *second=b;
    int order=CompareByte(a,b);
    return order ? order : (first[1]>second[1])-(first[1]<second[1]);
}

u32 CheckOriginalQSort(void) {
    const u32 counts[]={0,1,2,3,9,12,24,32,33,64,65,127,256};
    const u32 widths[]={1,2,3,4,8,12,17};
    u8 expected[256*17+8], withWork[sizeof(expected)], withoutWork[sizeof(expected)];
    u32 work[20];
    MATH_QSort(NULL,0,0,NULL,NULL);
    MATH_QSort(NULL,1,4,NULL,NULL);
    MATH_QSort(NULL,2,0,NULL,NULL);
    MATH_QSort(NULL,0x80000000,4,NULL,NULL);
    MATH_QSort(NULL,0xFFFFFFFF,4,NULL,NULL);
    for (u32 c=0;c<sizeof(counts)/sizeof(counts[0]);c++) for (u32 w=0;w<sizeof(widths)/sizeof(widths[0]);w++) {
        u32 count=counts[c], width=widths[w], size=count*width;
        u32 workWords=count>1 ? 2*(32-__builtin_clz(count)) : 0;
        MATHQSortComparator compare=width==1 ? CompareByte : CompareRecord;
        for (u32 alignment=0;alignment<4;alignment++) for (u32 pattern=0;pattern<3;pattern++) {
            u32 offset=alignment+1;
            memset(expected,0xA5,sizeof(expected));
            for (u32 i=0;i<count;i++) {
                u8 *record=expected+offset+i*width;
                record[0]=pattern==0 ? i : pattern==1 ? count-i-1 : (i*37+11)%17;
                if (width>1) record[1]=i;
                for (u32 byte=2;byte<width;byte++) record[byte]=(u8)(i^(byte*41));
            }
            memcpy(withWork,expected,sizeof(expected));
            memcpy(withoutWork,expected,sizeof(expected));
            memset(work,0xA5,sizeof(work));
            qsort(expected+offset,count,width,compare);
            MATH_QSort(withWork+offset,count,width,compare,work+1);
            MATH_QSort(withoutWork+offset,count,width,compare,NULL);
            CHECK(!memcmp(expected,withWork,sizeof(expected)) && !memcmp(expected,withoutWork,sizeof(expected)));
            CHECK(work[0]==0xA5A5A5A5 && work[1+workWords]==0xA5A5A5A5);
            CHECK(withWork[offset-1]==0xA5 && withWork[offset+size]==0xA5);
        }
    }
    // Equal-key permutations from the original SDK pivot/partition instruction sequence.
    const struct { u32 count,width; u8 ids[5]; } ties[]={
        {3,4,{2,1,0}},{4,4,{2,3,1,0}},{5,4,{3,4,2,0,1}},{4,3,{0,3,2,1}}
    };
    for (u32 t=0;t<sizeof(ties)/sizeof(ties[0]);t++) {
        u8 records[20]={0};
        for (u32 i=0;i<ties[t].count;i++) records[i*ties[t].width+1]=i;
        MATH_QSort(records,ties[t].count,ties[t].width,CompareByte,NULL);
        for (u32 i=0;i<ties[t].count;i++) CHECK(records[i*ties[t].width+1]==ties[t].ids[i]);
    }
    return 0;
}
