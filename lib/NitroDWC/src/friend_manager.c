#pragma force_active on
#include <string.h>

#include "global.h"

#include "dwc/dwc_main_internal.h"
#include "dwc/net_common_internal.h"

// The code below is compiled as ARM (32-bit) code.
#include <nitro/code32.h>

#pragma warn_unusedvar off
#pragma warn_uninitializedvar off

// Friend list management: the buddy status strings ("SCM"/"SCN" matchmaking fields, base64 game data in
// the location string), the friend list update started after the log-in, the GameSpy buddy callbacks,
// and mapping between friend list indices and GameSpy profile ids.

// The part of the GameSpy presence connection read here: the status set last.
struct GPIConnection {
    u8 unk_000[0x214];
    int status;               // 0x214
    char statusString[256];   // 0x218
    char locationString[256]; // 0x318
};

extern FriendGlobal ov00_0221A918;
extern char ov00_02217A08[]; // "SCM"
extern char ov00_02217A0C[]; // "SCN"
extern char ov00_02217A10[]; // "I have authorized your request to add me to your list"

extern unsigned long strtoul(const char *s, char **end, int base);
extern void CloseStatsConnection(void);
extern int IsStatsConnected(void);
extern void PersistThink(void);
extern int gpProcess(GPConnection *connection);
extern int gpProfileSearchA(GPConnection *connection, int unk_01, int unk_02, int unk_03, int unk_04, const char *userName, int unk_06, int unk_07, GPCallback callback, void *param);
extern int gpGetInfo(GPConnection *connection, int profileId, int unk_02, int unk_03, GPCallback callback, void *param);
extern int gpGetNumBuddies(GPConnection *connection, int *count);
extern int gpGetBuddyStatus(GPConnection *connection, int index, FriendBuddyStatus *status);
extern int gpGetBuddyIndex(GPConnection *connection, int profileId, int *index);
extern int gpDeleteBuddy(GPConnection *connection, int profileId);
extern int gpSetStatusA(GPConnection *connection, int status, const char *statusString, const char *locationString);

BOOL DWCi_IsLoggedIn(void) {
    if (ov00_0221A90C.state && ov00_0221A90C.state->state == 5) {
        return TRUE;
    }
    return FALSE;
}

int DWC_GetFriendStatus(const DwcAccData *friendData, char *locationString) {
    return DWC_GetFriendStatusSC(friendData, NULL, NULL, locationString);
}

// Buddy status of a friend; a and b get its "SCM" / "SCN" matchmaking fields.
u8 DWC_GetFriendStatusSC(const DwcAccData *friendData, u8 *a, u8 *b, char *locationString) {
    FriendBuddyStatus status;
    char value[4];
    if (DWCi_GetFriendBuddyStatus(friendData, &status)) {
        if (status.status == 6) {
            if (a) {
                if (DWC_GetCommonValueString(ov00_02217A08, value, status.statusString, '/') <= 0) {
                    *a = 0;
                } else {
                    *a = strtoul(value, NULL, 10);
                }
            }
            if (b) {
                if (DWC_GetCommonValueString(ov00_02217A0C, value, status.statusString, '/') <= 0) {
                    *b = 0;
                } else {
                    *b = strtoul(value, NULL, 10);
                }
            }
        } else {
            if (a) {
                *a = 0;
            }
            if (b) {
                *b = 0;
            }
        }
        if (locationString) {
            strcpy(locationString, status.locationString);
        }
        return status.status;
    }
    if (a) {
        *a = 0;
    }
    if (b) {
        *b = 0;
    }
    return 0;
}

int DWC_GetFriendStatusData(const DwcAccData *friendData, char *data, int *size) {
    return DWC_GetFriendStatusDataSC(friendData, NULL, NULL, data, size);
}

// Buddy status of a friend with the game data decoded from its location string.
int DWC_GetFriendStatusDataSC(const DwcAccData *friendData, u8 *a, u8 *b, char *data, int *size) {
    char buf[256];
    int status = DWC_GetFriendStatusSC(friendData, a, b, buf);
    if (!status) {
        *size = -1;
        return status;
    }
    *size = DWCi_Base64Decode(buf, strlen(buf), NULL, 0);
    if (!data || *size == -1) {
        return status;
    }
    DWCi_Base64Decode(buf, strlen(buf), data, *size);
    return status;
}

// Sets the game data in the own location string.
BOOL DWC_SetOwnStatusData(const u8 *data, int size) {
    char buf[256];
    int len;
    if (!ov00_0221A918.work || !DWCi_IsLoggedIn()) {
        return FALSE;
    }
    len = DWCi_Base64Encode(data, size, buf, 255);
    if (len == -1) {
        return FALSE;
    }
    buf[len] = 0;
    return DWCi_SetGPStatus(-1, NULL, buf) == 0;
}

BOOL DWC_SetBuddyFriendCallback(void *callback, void *param) {
    if (!ov00_0221A918.work) {
        return FALSE;
    }
    ov00_0221A918.work->buddyCallback = callback;
    ov00_0221A918.work->buddyParam = param;
    return TRUE;
}

int DWC_Base64Encode(const u8 *src, int len, char *dest, int destSize) {
    return DWCi_Base64Encode(src, len, dest, destSize);
}

int DWC_Base64Decode(char *src, int len, char *dest, int destSize) {
    return DWCi_Base64Decode(src, len, dest, destSize);
}

void DWCi_FriendInit(FriendWork *work, GPConnection *connection, u16 *playerName, DwcAccData *list, int count) {
    ov00_0221A918.work = work;
    ov00_0221A918.work->state = 0;
    ov00_0221A918.work->connection = connection;
    ov00_0221A918.work->pollCount = 0;
    {
        FriendWork *current = ov00_0221A918.work;
        current->pollTick = 0;
        current->count = count;
    }
    ov00_0221A918.work->list = list;
    ov00_0221A918.work->updateCount = 0;
    ov00_0221A918.work->changed = 0;
    ov00_0221A918.work->updated = 0;
    ov00_0221A918.work->pendingSteps = 0;
    ov00_0221A918.work->unk_20 = 0;
    ov00_0221A918.work->unk_24 = 0;
    ov00_0221A918.work->playerName = playerName;
    ov00_0221A918.work->updateCallback = NULL;
    ov00_0221A918.work->updateParam = NULL;
    ov00_0221A918.work->statusCallback = NULL;
    ov00_0221A918.work->statusParam = NULL;
    ov00_0221A918.work->deleteCallback = NULL;
    ov00_0221A918.work->deleteParam = NULL;
    ov00_0221A918.work->buddyCallback = NULL;
    ov00_0221A918.work->buddyParam = NULL;
    ov00_0221A918.work->unk_4C[0] = 0;
    ov00_0221A918.work->unk_4C[1] = 0;
    ov00_0221A918.work->unk_4C[2] = 0;
    ov00_0221A918.work->unk_4C[3] = 0;
}

void DWCi_FriendProcess(void) {
    FriendWork *work;
    if (!ov00_0221A918.work) {
        return;
    }
    if (DWCi_IsError()) {
        return;
    }
    if (!ov00_0221A918.work->list) {
        if (ov00_0221A918.work->connection && *ov00_0221A918.work->connection) {
            DWCi_FriendGPProcess();
        }
        return;
    }
    if (ov00_021F1248() || IsStatsConnected()) {
        ov00_0221A918.busy = 1;
        ov00_0221A918.deferred = 0;
        PersistThink();
        ov00_0221A918.busy = 0;
        if (ov00_0221A918.deferred == 1) {
            ov00_0221A918.deferred = 0;
            CloseStatsConnection();
        }
    }
    if (ov00_0221A918.work->connection && *ov00_0221A918.work->connection) {
        if (DWCi_FriendGPProcess()) {
            return;
        }
        work = ov00_0221A918.work;
        if (!work->state) {
            return;
        }
        if (work->list && work->updated != 3 && (u32)work->pollCount > 7) {
            if (work->updated <= 1) {
                DWCi_UpdateFriendList(work->list, work->count);
            }
            if (ov00_0221A918.work->updateCount >= ov00_0221A918.work->count) {
                ov00_0221A918.work->updated = 3;
                ov00_0221A918.work->pendingSteps++;
            }
        }
    }
    if (ov00_0221A918.work->pendingSteps >= 2) {
        ov00_0221A918.work->pendingSteps = 0;
        DWCi_FinishUpdateServers();
    }
}

// Starts the friend list update.
void DWCi_UpdateServersAsync(const char *token, const char *challenge, void *updateCallback, void *updateParam, void *statusCallback, void *statusParam, void *deleteCallback, void *deleteParam) {
    ov00_0221A918.work->updateCallback = updateCallback;
    ov00_0221A918.work->updateParam = updateParam;
    ov00_0221A918.work->statusCallback = statusCallback;
    ov00_0221A918.work->statusParam = statusParam;
    ov00_0221A918.work->deleteCallback = deleteCallback;
    ov00_0221A918.work->deleteParam = deleteParam;
    ov00_0221A918.work->changed = 0;
    ov00_0221A918.work->updated = 0;
    ov00_0221A918.work->pendingSteps = 0;
    ov00_0221A918.work->updateCount = 0;
    ov00_0221A918.work->state = 1;
    if (!ov00_0221A918.work->list) {
        ov00_0221A918.work->pendingSteps++;
    }
    ov00_0221A918.work->pendingSteps++;
}

// Friend list update failed.
void DWCi_StopFriendProcess(int kind, int code) {
    if (!ov00_0221A918.work || !kind) {
        return;
    }
    DWCi_SetError(kind, code);
    if (ov00_0221A918.work->state != 0 && ov00_0221A918.work->state != 2) {
        ov00_0221A918.work->updateCallback(kind, ov00_0221A918.work->changed, ov00_0221A918.work->updateParam);
    }
    DWCi_ResetFriendUpdate();
}

// Buddy request received.
void DWCi_GPRecvBuddyRequestCallback(GPConnection *connection, void *arg, void *param) {
    int *request = arg;
    if (!ov00_0221A918.work->list) {
        return;
    }
    gpGetInfo(connection, *request, 0, 0, (GPCallback)DWCi_GPRecvBuddyRequestInfoCallback, NULL);
}

// Buddy message: the "authorized" message makes the friend a buddy.
BOOL DWCi_GPRecvBuddyAuthCallback(GPConnection *connection, void *arg, void *param) {
    GpBuddyMessage *message = arg;
    if (strcmp(message->message, ov00_02217A10) != 0) {
        return FALSE;
    }
    gpGetInfo(connection, message->profileId, 0, 0, (GPCallback)DWCi_GPRecvBuddyAuthInfoCallback, NULL);
    return TRUE;
}

// Buddy status changed.
void DWCi_GPRecvBuddyStatusCallback(GPConnection *connection, void *arg, void *param) {
    GpBuddyStatusArg *buddy = arg;
    FriendBuddyStatus status;
    int index;
    if (!ov00_0221A918.work->statusCallback) {
        return;
    }
    index = DWCi_GetFriendListIndex(buddy->profileId);
    if (index == -1) {
        return;
    }
    gpGetBuddyStatus(connection, buddy->index, &status);
    ov00_0221A918.work->statusCallback(index, status.status & 0xFF, status.locationString, ov00_0221A918.work->statusParam);
}

// GameSpy profile id of the friend at index, 0 if it has none.
int DWCi_GetProfileIDFromList(int index) {
    DwcAccData *list = ov00_0221A918.work->list;
    int profileId;
    if (!list) {
        return 0;
    }
    profileId = DWC_GetGsProfileId(DWCi_GetUserData(), &list[index]);
    if (profileId != 0 && profileId != -1) {
        return profileId;
    }
    return 0;
}

// Friend list index of a GameSpy profile id, -1 if it is not in the list.
int DWCi_GetFriendListIndex(int profileId) {
    int i;
    if (!ov00_0221A918.work || !profileId) {
        return -1;
    }
    for (i = 0; i < ov00_0221A918.work->count; i++) {
        if (profileId == DWCi_GetProfileIDFromList(i)) {
            return i;
        }
    }
    return -1;
}

void DWCi_ResetFriendPoll(void) {
    if (!ov00_0221A918.work) {
        return;
    }
    ov00_0221A918.work->pollCount = 0;
    ov00_0221A918.work->pollTick = OS_GetTick();
}

// Sets the own buddy status; -1 / NULL keep the current values.
int DWCi_SetGPStatus(int status, const char *statusString, const char *locationString) {
    if (!ov00_0221A918.work || !DWCi_IsLoggedIn()) {
        return 0;
    }
    if (status == -1) {
        status = (*ov00_0221A918.work->connection)->status;
    }
    if (!statusString) {
        statusString = (*ov00_0221A918.work->connection)->statusString;
    }
    if (!locationString) {
        locationString = (*ov00_0221A918.work->connection)->locationString;
    }
    return gpSetStatusA(ov00_0221A918.work->connection, status, statusString, locationString);
}

// The friend at index became a buddy.
void DWCi_FriendBecameBuddy(int index) {
    struct {
        char unused[264];
        char locationString[264];
    } buf;

    if (ov00_0221A918.work->buddyCallback && ov00_0221A918.work->state != 1) {
        ov00_0221A918.work->buddyCallback(index, ov00_0221A918.work->buddyParam);
    }
    if (!ov00_0221A918.work->statusCallback) {
        return;
    }
    {
        int status = DWC_GetFriendStatus(&ov00_0221A918.work->list[index], buf.locationString);
        ov00_0221A918.work->statusCallback(index, status, buf.locationString, ov00_0221A918.work->statusParam);
    }
}

void DWCi_ShutdownFriend(void) {
    ov00_0221A918.work = NULL;
}

// Processes the presence connection every 300 ms.
int DWCi_FriendGPProcess(void) {
    int error = 0;
    FriendWork *work = ov00_0221A918.work;
    if (((OS_GetTick() - work->pollTick) << 6) / 33514 >= 300) {
        work->pollCount++;
        error = gpProcess(ov00_0221A918.work->connection);
        ov00_0221A918.work->pollTick = OS_GetTick();
    }
    return error;
}

void DWCi_ResetFriendUpdate(void) {
    if (!ov00_0221A918.work) {
        return;
    }
    ov00_0221A918.work->state = 0;
    ov00_0221A918.work->updated = 0;
    ov00_0221A918.work->pendingSteps = 0;
}

// Matches the friend list against the buddy list: drops buddies that are not friends, marks friends
// that are buddies, and looks up the profile ids of friends known by login id only.
void DWCi_UpdateFriendList(DwcAccData *list, int count) {
    struct {
        int i;
        volatile int count;
    } local;

    char userName[24];
    FriendBuddyStatus status;
    if (!ov00_0221A918.work->updated) {
        DWCi_FriendHandleGPError(gpGetNumBuddies(ov00_0221A918.work->connection, (int *)&local.count));
        for (local.i = 0; local.i < local.count; local.i++) {
            int j;
            DWCi_FriendHandleGPError(gpGetBuddyStatus(ov00_0221A918.work->connection, *(volatile int *)&local.i, &status));
            for (j = 0; j < count; j++) {
                if (status.profileId == DWCi_GetProfileIDFromList(j)) {
                    if (DWC_IsBuddyFriendData(&list[j])) {
                        break;
                    }
                    DWC_SetGsProfileId(&list[j], status.profileId);
                    DWCi_SetBuddyFriendData(&list[j]);
                    ov00_0221A918.work->changed = 1;
                    break;
                }
            }
            if (j == count) {
                DWCi_FriendHandleGPError(gpDeleteBuddy(ov00_0221A918.work->connection, status.profileId));
                local.count--;
                local.i--;
            }
        }
        ov00_0221A918.work->updated = 1;
    }
    while (ov00_0221A918.work->updateCount < count) {
        u32 profileId = DWCi_GetProfileIDFromList(ov00_0221A918.work->updateCount);
        if (profileId) {
            if (!DWCi_MergeDuplicateFriend(list, ov00_0221A918.work->updateCount, profileId)) {
                DWCi_FriendHandleGPError(gpGetBuddyIndex(ov00_0221A918.work->connection, profileId, (int *)&local.i));
                if (local.i == -1) {
                    DWCi_SendBuddyRequest(profileId);
                }
            }
        } else if (DWC_GetGsProfileId(DWCi_GetUserData(), &list[ov00_0221A918.work->updateCount]) == -1) {
            DWC_LoginIdToUserName(DWCi_GetUserData(), &list[ov00_0221A918.work->updateCount], userName);
            gpProfileSearchA(ov00_0221A918.work->connection, 0, 0, 0, 0, userName, 0, 0, (GPCallback)DWCi_GPProfileSearchCallback, (void *)(u32)ov00_0221A918.work->updateCount);
            ov00_0221A918.work->updated = 2;
            return;
        }
        ov00_0221A918.work->updateCount++;
    }
}

// Friend list update finished.
void DWCi_FinishUpdateServers(void) {
    ov00_0221A918.work->updateCallback(0, ov00_0221A918.work->changed, ov00_0221A918.work->updateParam);
    ov00_0221A918.work->state = 2;
}

// Clears the entry at index (a duplicate of the entry at other).
void DWCi_DeleteFriendEntry(DwcAccData *list, int index, int other) {
    if (!ov00_0221A918.work) {
        return;
    }
    MI_CpuFill8(&list[index], 0, sizeof(DwcAccData));
    if (ov00_0221A918.work->deleteCallback) {
        ov00_0221A918.work->deleteCallback(index, other, ov00_0221A918.work->deleteParam);
    }
}
