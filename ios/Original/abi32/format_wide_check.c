// SPDX-License-Identifier: GPL-3.0-or-later
#define swprintf HGMSLSwprintf
#define vswprintf HGMSLVswprintf
#include "../../../lib/MSL_C/src/msl_wprintf_formatter.c"
#define CHECK(v) do { if (!(v)) return __LINE__; } while (0)
static void ToWide(u16 *out,const char *text) { do { *out++=(u8)*text; } while (*text++); }
static BOOL WideEquals(const u16 *wide,const char *text) {
    do { if (*wide++!=(u8)*text) return FALSE; } while (*text++);
    return TRUE;
}
u32 CheckOriginalWideFormat(void) {
    struct { u32 before; u16 text[128]; u32 after; } out={0x12345678,{0},0x87654321};
    u16 format[96],word[]={'H','G',0};
    ToWide(format,"%d/%llu/%d/%ls/%Lf/%d");
    const char *expected="11/1311768467463790320/-7/HG/1.250000/23";
    int count=HGMSLSwprintf(out.text,128,format,11,0x123456789ABCDEF0ULL,-7,word,1.25L,23);
    CHECK(count==(int)strlen(expected) && WideEquals(out.text,expected));
    ToWide(format,"%*.*lld|%*ls|%d");
    word[0]='A'; word[1]=0;
    expected="      -01234|A    |17";
    count=HGMSLSwprintf(out.text,128,format,12,5,-1234LL,-5,word,17);
    CHECK(count==(int)strlen(expected) && WideEquals(out.text,expected));
    ToWide(format,"%d:%llu%n:%d%lln");
    int written=-1; s64 wideWritten=-1;
    count=HGMSLSwprintf(out.text,128,format,5,0x100000001ULL,&written,7,&wideWritten);
    CHECK(WideEquals(out.text,"5:4294967297:7") && written==12 && wideWritten==14 && count==14);
    ToWide(format,"%d:%llu");
    count=HGMSLSwprintf(out.text,5,format,7,1234567890123ULL);
    CHECK(count==-1 && WideEquals(out.text,"7:12"));
    ToWide(format,"%d|%lld|%llu|%d");
    count=HGMSLSwprintf(out.text,128,format,1,(s64)-4294967297LL,(u64)4294967297ULL,2);
    CHECK(count==26 && WideEquals(out.text,"1|-4294967297|4294967297|2"));
    CHECK(out.before==0x12345678 && out.after==0x87654321);
    return 0;
}
