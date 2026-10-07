// clang-format off
#include <nitro/types.h>
#include <nitro/code32.h>
// clang-format on
#include "overlay45_string_search.h"
#pragma force_active on
inline StringSearchRange StringSearch::GetRange()const{if(raw.longForm.flag)return StringSearchRange(raw.longForm.data,raw.longForm.size);return StringSearchRange(raw.shortForm.data,raw.shortForm.size);}
inline int StringSearch::Find(const StringSearch&other,u32 pos)const{StringSearchRange needle=other.GetRange();return (int)ov45_02237EE4(this,needle.data,pos,needle.size);}
// @fn ov45_02237EB0
extern "C" int ov45_02237EB0(const StringSearch*a,const StringSearch*b,u32 pos){return a->Find(*b,pos);}
