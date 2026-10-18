#ifndef POKEHEARTGOLD_FRIEND_ROSTER_INTERNAL_H
#define POKEHEARTGOLD_FRIEND_ROSTER_INTERNAL_H
#include "friend_roster.h"
#include "to90_login_callback_private.h"

typedef struct FriendEntry {
    u16 name[8], name2[8];
    u32 id;
    u16 value24, value26, value28, value2A;
    u8 value2C, value2D, value2E, value2F;
    u16 value30, value32, value34, value36;
} FriendEntry;
typedef struct FriendEntryArray { FriendEntry entries[32]; } FriendEntryArray;
struct FriendRoster {
    W41287Account header;
    u32 friendData[32][3];
    FriendEntryArray roster;
};
#endif
