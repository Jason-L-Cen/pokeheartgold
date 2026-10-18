#pragma force_active on
#include <nitro/types.h>

// The code below is compiled as ARM (32-bit) code.
#include <nitro/code32.h>

union Str {
    struct {
        u32 flag : 1;
        u32 capacity : 31;
        u32 size;
        char *data;
    } l;

    struct {
        u8 flag : 1;
        u8 size : 7;
        char data[11];
    } s;

    u32 words[3];

    u32 Size() const {
        return !l.flag ? s.size : l.size;
    }

    const char *Data() const {
        return !l.flag ? s.data : l.data;
    }
};

extern "C" int LobbyString_FindFirstOfChars(const Str *self, const char *data, u32 pos, u32 n);
extern "C" int LobbyString_FindFirstOf(const Str *self, const Str *str, u32 pos);

extern "C" int LobbyString_FindFirstOf(const Str *self, const Str *str, u32 pos) {
    return LobbyString_FindFirstOfChars(self, str->Data(), pos, str->Size());
}
