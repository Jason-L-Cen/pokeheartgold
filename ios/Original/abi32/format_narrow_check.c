// SPDX-License-Identifier: GPL-3.0-or-later
// Keep the probe's WASI libc separate from the actual MSL formatter under test.
#define printf HGMSLPrintf
#define snprintf HGMSLSnprintf
#define vsnprintf HGMSLVsnprintf
#define sprintf HGMSLSprintf
#define vsprintf HGMSLVsprintf
#include "../../../lib/MSL_C/src/msl_printf_formatter.c"
#define CHECK(v) do { if (!(v)) return __LINE__; } while (0)
u32 CheckOriginalNarrowFormat(void) {
    struct { u32 before; char text[128]; u32 after; } out={0x12345678,{0},0x87654321};
    const char *expected="11/1311768467463790320/-7/HG/1.250000/23";
    int count=HGMSLSnprintf(out.text,sizeof(out.text),"%d/%llu/%d/%s/%Lf/%d",11,0x123456789ABCDEF0ULL,-7,"HG",1.25L,23);
    CHECK(count==(int)strlen(expected) && !strcmp(out.text,expected));
    expected="      -01234|A    |17";
    count=HGMSLSnprintf(out.text,sizeof(out.text),"%*.*lld|%*s|%d",12,5,-1234LL,-5,"A",17);
    CHECK(count==(int)strlen(expected) && !strcmp(out.text,expected));
    int written=-1; s64 wideWritten=-1;
    count=HGMSLSnprintf(out.text,sizeof(out.text),"%d:%llu%n:%d%lln",5,0x100000001ULL,&written,7,&wideWritten);
    CHECK(!strcmp(out.text,"5:4294967297:7") && written==12 && wideWritten==14 && count==14);
    count=HGMSLSnprintf(out.text,5,"%d:%llu",7,1234567890123ULL);
    CHECK(count==15 && !strcmp(out.text,"7:12"));
    count=HGMSLSnprintf(out.text,sizeof(out.text),"%d|%.2f|%llu|%d",3,2.5,0x100000001ULL,9);
    CHECK(count==19 && !strcmp(out.text,"3|2.50|4294967297|9"));
    count=HGMSLSnprintf(out.text,sizeof(out.text),"%d|%lld|%llu|%d",1,(s64)-4294967297LL,(u64)4294967297ULL,2);
    CHECK(count==26 && !strcmp(out.text,"1|-4294967297|4294967297|2"));
    CHECK(out.before==0x12345678 && out.after==0x87654321);
    return 0;
}
