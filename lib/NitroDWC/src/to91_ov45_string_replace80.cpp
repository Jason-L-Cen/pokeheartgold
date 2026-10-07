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
extern "C" Str *ov45_02238104(Str *,u32,u32,char *,char *,RangeDispatch);
extern "C" Str *ov45_02237DB4(Str *,u32,u32,Str *);
// @fn ov45_02237DB4
extern "C" Str *ov45_02237DB4(Str *out,u32 pos,u32 remove,Str *input) {
 char *data;u32 size;RangeDispatch dispatch;
 if(input->l.flag){data=input->l.data;size=input->l.size;}else{data=input->s.data;size=input->s.size;}
 return ov45_02238104(out,pos,remove,data,data+size,dispatch);
}
