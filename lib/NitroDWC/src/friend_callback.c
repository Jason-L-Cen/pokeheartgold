#pragma force_active on
#include <string.h>

#include "global.h"

#include "dwc/friend_internal.h"

// The code below is compiled as ARM (32-bit) code.
#include <nitro/code32.h>

// GameSpy callbacks keeping the friend list in sync with the buddy list: profile search results,
// buddy requests and profile information are matched against the list by login id user name or
// GameSpy profile id.

extern FriendGlobal ov00_0221A918;

extern const char ov00_02217A4C[]; // "%c%c%c%c"

extern void ov00_021F0550(int index);
extern DwcUserData *ov00_021EF3A4(void);
extern void gpAuthBuddyRequest(GPConnection *connection, int profileId);
extern void gpDenyBuddyRequest(GPConnection *connection, int profileId);
extern int gpGetBuddyIndex(GPConnection *connection, int profileId, int *index);

// Profile search callback for the friend at index param.
void ov00_021F0CBC(GPConnection *connection, void *arg, void *param) {
    FriendSearchResult *result = arg;
    int index = (int)(u32)param;
    int i, buddyIndex;
    if (result->result == 0 && result->count != 0 && DWC_GetFriendDataType(&ov00_0221A918.work->list[index]) != ACCOUNT_TYPE_NONE) {
        if (ov00_0221A918.work->state != 1) {
            return;
        }
        for (i = 0; i < result->count; i++) {
            if (ov00_021F09A4(ov00_0221A918.work->list, index, result->matches[i].profileId)) {
                ov00_0221A918.work->updateCount++;
                ov00_0221A918.work->updated = 1;
                result->operation = 0x601;
                return;
            }
        }
        for (i = 0; i < result->count; i++) {
            ov00_021F0C48(gpGetBuddyIndex(connection, result->matches[i].profileId, &buddyIndex));
            if (buddyIndex == -1) {
                ov00_021F0B58(result->matches[i].profileId);
            } else {
                DWC_SetGsProfileId(&ov00_0221A918.work->list[index], result->matches[0].profileId);
                DWCi_SetBuddyFriendData(&ov00_0221A918.work->list[index]);
                ov00_021F0550(index);
                ov00_0221A918.work->updateCount++;
                ov00_0221A918.work->updated = 1;
                result->operation = 0x601;
                ov00_0221A918.work->changed = 1;
                return;
            }
        }
        if (result->operation == 0x600) {
            return;
        }
        ov00_0221A918.work->updateCount++;
        ov00_0221A918.work->updated = 1;
    } else {
        if (result->result != 0) {
            if (ov00_021F0C48(result->result)) {
                return;
            }
        } else {
            if (ov00_0221A918.work->state != 1 && DWC_GetFriendDataType(&ov00_0221A918.work->list[index]) != ACCOUNT_TYPE_NONE) {
                return;
            }
            ov00_0221A918.work->updateCount++;
            ov00_0221A918.work->updated = 1;
        }
    }
}

// Buddy request callback: accept the request if the sender is on the friend list.
void ov00_021F0EF8(GPConnection *connection, FriendProfileInfo *request) {
    char userName[21];
    char gameCode[5];
    int i;
    BOOL found = FALSE;
    if (request->result != 0) {
        return;
    }
    for (i = 0; i < ov00_0221A918.work->count; i++) {
        if (DWC_GetFriendDataType(&ov00_0221A918.work->list[i]) == ACCOUNT_TYPE_LOGIN_ID) {
            DWC_LoginIdToUserName(ov00_021EF3A4(), &ov00_0221A918.work->list[i], userName);
            if (strcmp(userName, request->userName) == 0) {
                gpAuthBuddyRequest(connection, request->profileId);
                DWC_SetGsProfileId(&ov00_0221A918.work->list[i], request->profileId);
                found = TRUE;
            }
        } else if (DWC_GetFriendDataType(&ov00_0221A918.work->list[i]) == ACCOUNT_TYPE_GS_PROFILE || DWC_GetFriendDataType(&ov00_0221A918.work->list[i]) == ACCOUNT_TYPE_FRIEND_KEY) {
            u32 code = ov00_021EF3A4()->gameCode;
            OS_SNPrintf(gameCode, 5, ov00_02217A4C, (code >> 24) & 0xff, (code >> 16) & 0xff, (code >> 8) & 0xff, code & 0xff);
            if (request->profileId == DWC_GetGsProfileId(ov00_021EF3A4(), &ov00_0221A918.work->list[i]) && memcmp(gameCode, request->gameCode, 4) == 0) {
                gpAuthBuddyRequest(connection, request->profileId);
                found = TRUE;
            }
        }
    }
    if (found) {
        ov00_021F0B58(request->profileId);
    } else {
        gpDenyBuddyRequest(connection, request->profileId);
    }
}

// Profile information callback: mark the matching friend entries as buddies.
void ov00_021F10A8(GPConnection *connection, FriendProfileInfo *info) {
    int i, updated = 0, buddy = 0;
    char userName[24];
    if (info->result) {
        return;
    }
    for (i = 0; i < ov00_0221A918.work->count; i++) {
        if (DWC_GetFriendDataType(&ov00_0221A918.work->list[i]) == ACCOUNT_TYPE_LOGIN_ID) {
            DWC_LoginIdToUserName(ov00_021EF3A4(), &ov00_0221A918.work->list[i], userName);
            if (strcmp(userName, info->userName) == 0) {
                DWC_SetGsProfileId(&ov00_0221A918.work->list[i], info->profileId);
                DWCi_SetBuddyFriendData(&ov00_0221A918.work->list[i]);
                updated = 1;
            }
        } else if (DWC_GetFriendDataType(&ov00_0221A918.work->list[i]) == ACCOUNT_TYPE_GS_PROFILE || DWC_GetFriendDataType(&ov00_0221A918.work->list[i]) == ACCOUNT_TYPE_FRIEND_KEY) {
            if (DWC_GetGsProfileId(ov00_021EF3A4(), &ov00_0221A918.work->list[i]) == info->profileId) {
                if (DWC_IsBuddyFriendData(&ov00_0221A918.work->list[i]) == TRUE) {
                    buddy = 1;
                } else {
                    DWC_SetGsProfileId(&ov00_0221A918.work->list[i], info->profileId);
                    DWCi_SetBuddyFriendData(&ov00_0221A918.work->list[i]);
                    updated = 1;
                }
            }
        }
    }
    if (updated) {
        int index = ov00_021F0A50(ov00_0221A918.work->list, ov00_0221A918.work->count, info->profileId);
        if (!buddy) {
            ov00_021F0550(index);
        }
        ov00_0221A918.work->changed = 1;
    }
}
