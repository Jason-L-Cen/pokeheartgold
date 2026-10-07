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
extern "C" Str *ov45_022339F4(Str *,Str *,Str *);
// @fn ov45_022339F4
extern "C" Str *ov45_022339F4(Str *out,Str *left,Str *right) {
 u32 i,n1,n2;char *p1,*p2;RangeDispatch first,second;
 for(i=0;i<3;i++)out->words[i]=0;
 if(left->l.flag){n1=left->l.size;p1=left->l.data;}else{n1=left->s.size;p1=left->s.data;}
 if(right->l.flag){p2=right->l.data;n2=right->l.size;}else{p2=right->s.data;n2=right->s.size;}
 ov45_02237BFC(out,n1+n2);
 out=ov45_02238104(out,0,0,p1,p1+n1,first);
 return ov45_02238104(out,n1,0,p2,p2+n2,second);
}
