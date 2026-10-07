#include <nitro/types.h>
#include <nitro/code32.h>
#include <string.h>
using std::memset;
#include "overlay45_format_buffer_private.h"
using namespace DwcFormatted216Native;
#pragma force_active on
// @fn ov45_0223795C
extern "C" void ov45_0223795C(Vec *vector,u32 count,ResizeDispatch){
 u32 capacity=vector->cap;
 if(count>capacity || vector->size>capacity-count){
  u32 additional=vector->size+count-capacity;
  Allocator allocator=Allocator();
  allocator.recommend(vector,additional);
 }else{
  memset(vector->ptr+vector->size,0,count);
  vector->size+=count;return;
 }
 ov45_02238820(vector,count);
}
#pragma force_active off
