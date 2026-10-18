#ifndef OVERLAY45_FORMAT_BUFFER_PRIVATE_H
#define OVERLAY45_FORMAT_BUFFER_PRIVATE_H
#include <nitro/types.h>
#ifdef __cplusplus
namespace DwcStringConcat200 {
union Str {struct {u32 flag:1;u32 capacity:31;u32 size;char *data;} l;struct {u8 flag:1;u8 size:7;char data[11];} s;u32 words[3];};
class RangeDispatch {
#ifdef HEARTGOLD_ABI32
 u8 hgAbiCarrier;
#endif
};
}
namespace DwcFormatted216Native {
struct Vec {char *ptr;u32 size,cap;};
class ResizeDispatch {
#ifdef HEARTGOLD_ABI32
 u8 hgAbiCarrier;
#endif
};
class Allocator {
public:
 u32 recommend(Vec *vector,u32 growth);
};
}
using DwcStringConcat200::Str;
using DwcFormatted216Native::Vec;
#ifndef HG_LOBBY_TYPES_ONLY
extern "C" {
#endif
#else
typedef union Str {struct {u32 flag:1;u32 capacity:31;u32 size;char *data;} l;struct {u8 flag:1;u8 size:7;char data[11];} s;u32 words[3];} Str;
typedef struct Vec {char *ptr;u32 size,cap;} Vec;
#endif
#ifndef HG_LOBBY_TYPES_ONLY
Str *LobbyString_InitFromCStr(Str *,char *);
Str *LobbyString_Destroy(Str *);
Vec *LobbyCharVector_Destroy(Vec *);
char *LobbyCharVector_Data(Vec *);
u32 LobbyCharVector_RecommendSize(Vec *,u32);
void LobbyCharVector_AppendZerosSlow(Vec *,u32);
#ifdef __cplusplus
void LobbyCharVector_AppendZeros(Vec *,u32,DwcFormatted216Native::ResizeDispatch);
#ifndef HG_LOBBY_NO_RANGE_DECL
Str *LobbyString_ReplaceRange(Str *,u32,u32,char *,char *,DwcStringConcat200::RangeDispatch);
#endif
void LobbyString_Reserve(Str *,u32);
}
inline u32 DwcFormatted216Native::Allocator::recommend(Vec *vector,u32 growth){return LobbyCharVector_RecommendSize(vector,growth);}
#endif
#endif // HG_LOBBY_TYPES_ONLY
#endif
