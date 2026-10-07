#include <nitro/types.h>
#include <nitro/code32.h>
#include <string.h>
#include "overlay45_format_buffer_private.h"
using std::strlen;
using DwcStringConcat200::RangeDispatch;
#pragma force_active on
// @fn ov45_02237B58
extern "C" Str *ov45_02237B58(Str *output,char *text){
 u32 index,length;RangeDispatch dispatch;
 for(index=0;index<3;index++)output->words[index]=0;
 length=strlen(text);
 ov45_02237BFC(output,length);
 ov45_02238104(output,0,0,text,text+length,dispatch);
 return output;
}
extern "C" void ov00_021EC2EC(int,void *,u32);
// @fn ov45_02237BC8
extern "C" Str *ov45_02237BC8(Str *output){
 if(output->l.flag && output->l.data)ov00_021EC2EC(0,output->l.data,0);
 return output;
}
#pragma force_active off
