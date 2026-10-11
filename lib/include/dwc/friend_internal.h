#ifndef POKEHEARTGOLD_LIB_INCLUDE_DWC_FRIEND_INTERNAL_H
#define POKEHEARTGOLD_LIB_INCLUDE_DWC_FRIEND_INTERNAL_H

// Friend list state of OVY_0 (lib/NitroDWC/src/friend_update.c, match_setup.c): the list of 12-byte
// friend data entries kept in sync with the GameSpy buddy list.

#include "dwc/dwc_account_internal.h"

#include "to90_login_callback_private.h"

typedef struct FriendWork {
    int state;                // 1 while the friend list is being updated
    GPConnection *connection; // 0x04
    u32 unk_08[3];
    int count;        // 0x14, entries in list
    DwcAccData *list; // 0x18
    u8 updateCount;   // 0x1C, entries checked so far
    u8 changed;       // 0x1D, list was modified
    u8 updated;       // 0x1E
    u8 unk_1F;
    u32 unk_20; // 0x20
} FriendWork;

// ov00_0221A918. Declared by its users: friend_list.c reads it as an array of words (FriendWork *[]) and only
// matches that way, the other files use this struct.
typedef struct FriendGlobal {
    u32 unk_00[2];
    FriendWork *work; // 0x08
    u32 unk_0C;
} FriendGlobal;

// Buddy request / profile information delivered to the GameSpy callbacks.
typedef struct FriendProfileInfo {
    int result;
    int profileId; // 0x04
    u8 unk_08[0x86];
    char userName[9]; // 0x8E
    char gameCode[4]; // 0x97
} FriendProfileInfo;

// One match of a profile search, 0xAC bytes.
typedef struct FriendSearchMatch {
    int profileId;
    char unk_04[31];
    char unk_23[21];
    char unk_38[31];
    char unk_57[31];
    char unk_76[51];
} FriendSearchMatch;

typedef struct FriendSearchResult {
    int result;
    int count;                  // 0x04
    u32 operation;              // 0x08
    FriendSearchMatch *matches; // 0x0C
} FriendSearchResult;

int ov00_021F09A4(DwcAccData *list, int index, int profileId);
int ov00_021F0A50(DwcAccData *list, int count, int profileId);
int ov00_021F0B58(int profileId);
BOOL ov00_021F0B8C(const DwcAccData *friendData, void *status);
int ov00_021F0C48(u32 error);
void ov00_021F0CBC(GPConnection *connection, void *arg, void *param);
void ov00_021F0EF8(GPConnection *connection, FriendProfileInfo *request);
void ov00_021F10A8(GPConnection *connection, FriendProfileInfo *info);
u32 ov00_021F1248(void);

#endif // POKEHEARTGOLD_LIB_INCLUDE_DWC_FRIEND_INTERNAL_H
