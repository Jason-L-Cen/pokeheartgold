#include <nitro/types.h>
#include <nitro/code32.h>
#include "overlay45_format_buffer_private.h"
using DwcStringConcat200::RangeDispatch;
using namespace DwcFormatted216Native;
extern "C" void ov00_021EC2EC(int,void *,u32);
extern "C" Str *ov45_02237A24(Str *,Str *);
namespace DwcFormatted216Native {
struct CharacterAllocator {
 void release(char *storage){if(storage)ov00_021EC2EC(0,storage,0);}
};
}
#pragma force_active on
// @fn ov45_022379D4
extern "C" Vec *ov45_022379D4(Vec *vector){
 if(vector->ptr){
  u32 count=vector->size;
  CharacterAllocator allocator=CharacterAllocator();
  vector->size-=count;
  allocator.release(vector->ptr);
 }
 return vector;
}
// @fn ov45_02237A24
extern "C" Str *ov45_02237A24(Str *output,Str *input){
 if(!input->l.flag)*output=*input;
 else{
  u32 index;RangeDispatch dispatch;
  for(index=0;index<3;index++)output->words[index]=0;
  ov45_02237BFC(output,input->l.size);
  ov45_02238104(output,0,0,input->l.data,input->l.data+input->l.size,dispatch);
 }
 return output;
}
#pragma force_active off
