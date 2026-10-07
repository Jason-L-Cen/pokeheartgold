#pragma force_active on
#include <nitro/types.h>
#include <nitro/code32.h>
namespace DwcStringConcat200 {
union Str {struct {u32 flag:1;u32 capacity:31;u32 size;char *data;} l;struct {u8 flag:1;u8 size:7;char data[11];} s;u32 words[3];};
class RangeDispatch {};
typedef char ActualString12[sizeof(Str)==12?1:-1];
typedef char ActualCarrier1[sizeof(RangeDispatch)==1?1:-1];
}
using namespace DwcStringConcat200;
extern "C" void ov45_02237BFC(Str *,u32);
extern "C" Str *ov45_02238104(Str *,u32,u32,char *,char *,RangeDispatch);
extern "C" unsigned strlen(const char*);
extern "C" Str *ov45_02233ABC(Str *,Str *,char *);
// @fn ov45_02233ABC
extern "C" Str *ov45_02233ABC(Str *out,Str *left,char *right) {
 u32 i,n1;char *p1;RangeDispatch first,second;
 for(i=0;i<3;i++)out->words[i]=0;
 const u32 n2=strlen(right);
 if(left->l.flag){p1=left->l.data;n1=left->l.size;}else{p1=left->s.data;n1=left->s.size;}
 ov45_02237BFC(out,n1+n2);
 out=ov45_02238104(out,0,0,p1,p1+n1,first);
 return ov45_02238104(out,n1,0,right,right+n2,second);
}
