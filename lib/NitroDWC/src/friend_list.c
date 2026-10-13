#pragma force_active on
#include "global.h"

#include "dwc/friend_internal.h"

// The code below is compiled as ARM (32-bit) code.
#include <nitro/code32.h>

#pragma warn_uninitializedvar off

// Friend list upkeep: merging duplicate entries (same GameSpy profile id), sending buddy requests,
// looking up a friend's buddy status, and mapping GameSpy error codes.

// ov00_0221A918[2] is the friend list work (FriendGlobal.work, see dwc/friend_internal.h).
extern FriendWork *ov00_0221A918[];

extern const char ov00_02217A48[]; // ""

extern int ov00_021F03BC(int index);
extern void ov00_021F094C(DwcAccData *list, int index, int other);
extern void ov00_021F0238(int kind, int code);
extern BOOL ov00_021EFC6C(void);
extern DwcUserData *ov00_021EF3A4(void);
extern int gpSendBuddyRequestA(GPConnection *connection, int profileId, const char *message);
extern int gpGetBuddyStatus(GPConnection *connection, int index, void *status);
extern int gpGetBuddyIndex(GPConnection *connection, int profileId, int *index);

int ov00_021F09A4(DwcAccData *list, int index, int profileId) {
    int i;
    for (i = 0; i < index; i++) {
        int id = ov00_021F03BC(i);
        if (!id || id != profileId) {
            continue;
        }
        if (DWC_IsBuddyFriendData(&list[index]) && !DWC_IsBuddyFriendData(&list[i])) {
            ov00_021F094C(list, i, index);
        } else {
            ov00_021F094C(list, index, i);
        }
        ov00_0221A918[2]->changed = 1;
        return 1;
    }
    return 0;
}

int ov00_021F0A50(DwcAccData *list, int count, int profileId) {
    int result = -1, id, i;
    for (i = 0; i < count; i++) {
        id = ov00_021F03BC(i);
        if (id) {
            int j;
            if (id == profileId) {
                result = i;
            }
            for (j = i + 1; j < count; j++) {
                if (id == ov00_021F03BC(j)) {
                    if (DWC_GetFriendDataType(&list[i]) == ACCOUNT_TYPE_FRIEND_KEY && DWC_GetFriendDataType(&list[j]) == ACCOUNT_TYPE_GS_PROFILE) {
                        // called with an int profile id; the int-typed call is what the original object was built from
                        ((void (*)(DwcAccData *, int))DWC_SetGsProfileId)(&list[i], id);
                    }
                    if (DWC_IsBuddyFriendData(&list[j])) {
                        DWCi_SetBuddyFriendData(&list[i]);
                    }
                    ov00_021F094C(list, j, i);
                    ov00_0221A918[2]->changed = 1;
                }
            }
        }
    }
    return result;
}

int ov00_021F0B58(int profileId) {
    int error = gpSendBuddyRequestA(ov00_0221A918[2]->connection, profileId, ov00_02217A48);
    ov00_021F0C48(error);
    return error;
}

enum {
    FRIEND_STATUS_NG,
    FRIEND_STATUS_OK
};

// Buddy status of a friend, FALSE if it is not a buddy or not logged in.
BOOL ov00_021F0B8C(const DwcAccData *friendData, void *status) {
    int index = 0;
    int profileId;
    if (ov00_0221A918[2] == NULL || !ov00_021EFC6C()) {
        return FALSE;
    }
    profileId = DWC_GetGsProfileId(ov00_021EF3A4(), friendData);
    if (profileId > 0 && gpGetBuddyIndex(ov00_0221A918[2]->connection, profileId, &index) != 0) {
        return FALSE;
    }
    if (profileId <= 0 || index == -1) {
        return FALSE;
    }
    return gpGetBuddyStatus(ov00_0221A918[2]->connection, index, status) != 0 ? FRIEND_STATUS_NG : FRIEND_STATUS_OK;
}

// Maps a GameSpy error to a DWC error (ov00_021F0238); returns the GameSpy error.
int ov00_021F0C48(u32 error) {
    int kind, code;
    if (!error) {
        return 0;
    }
    switch (error) {
    case 1:
        kind = 9;
        code = -1;
        break;
    case 2:
        kind = 9;
        code = -2;
        break;
    case 3:
        kind = 6;
        code = -10;
        break;
    case 4:
        kind = 6;
        code = -20;
        break;
    }
    ov00_021F0238(kind, code - 71000);
    return error;
}
