#include <nitro/types.h>
#include <nitro/code32.h>
#include <stdarg.h>
#include <size_t.h>
inline void *operator new(std::size_t,void *storage){return storage;}
using std::va_list;
extern "C" {
#include <nitro/os/printf.h>
}
#include "overlay45_format_buffer_private.h"
using namespace DwcFormatted216Native;
namespace DwcFormatted216 {
struct Vector {
 Vec repr;
 Vector(){repr.ptr=0;repr.size=0;repr.cap=0;}
 ~Vector(){ov45_022379D4(&repr);}
 void shrink(u32 count,ResizeDispatch){repr.size-=count;}
 void resize(u32 count){
  if(count>repr.size)ov45_0223795C(&repr,count-repr.size,ResizeDispatch());
  else if(count<repr.size)shrink(repr.size-count,ResizeDispatch());
 }
 char *text(){return ov45_02232B9C(&repr);}
};
typedef char StringSize[sizeof(Str)==12?1:-1];
typedef char VectorSize[sizeof(Vector)==12?1:-1];
typedef char DispatchSize[sizeof(ResizeDispatch)==1?1:-1];
}
using namespace DwcFormatted216;
extern "C" void ov45_02233BC8(Str *,const char *,...);
#pragma force_active on
// @fn ov45_02233BC8
extern "C" void ov45_02233BC8(Str *output,const char *format,...){
 Vector vector;va_list args;u32 count;
 va_start(args,format);
 count=OS_VSNPrintf(0,0,format,args)+1;
 vector.resize(count);
 OS_VSNPrintf(vector.text(),count,format,args);
 vector.repr.ptr[count-1]=0;
 va_end(args);
 new (output) Str;
 ov45_02237B58(output,vector.text());
}
#pragma force_active off
