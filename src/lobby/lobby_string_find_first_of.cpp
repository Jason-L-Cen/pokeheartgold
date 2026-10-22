#include <nitro/types.h>

#include "lobby_string.h"

#include <nitro/code32.h>

extern "C" int LobbyString_FindFirstOf(const Str *self, const Str *str, u32 pos);

extern "C" int LobbyString_FindFirstOf(const Str *self, const Str *str, u32 pos) {
    return LobbyString_FindFirstOfChars(self, str->c_str(), pos, str->size());
}
