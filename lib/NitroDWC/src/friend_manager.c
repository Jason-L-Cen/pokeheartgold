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
extern void ov00_0220AE60(void);
extern int ov00_0220AED0(void);
extern void ov00_0220AEF0(void);
extern int ov00_021FFD9C(GPConnection *connection);
extern int ov00_021FFF58(GPConnection *connection, int unk_01, int unk_02, int unk_03, int unk_04, const char *userName, int unk_06, int unk_07, GPCallback callback, void *param);
extern int ov00_02200020(GPConnection *connection, int profileId, int unk_02, int unk_03, GPCallback callback, void *param);
extern int ov00_022003D0(GPConnection *connection, int *count);
extern int ov00_02200400(GPConnection *connection, int index, FriendBuddyStatus *status);
extern int ov00_02200540(GPConnection *connection, int profileId, int *index);
extern int ov00_022005FC(GPConnection *connection, int profileId);
extern int ov00_02200654(GPConnection *connection, int status, const char *statusString, const char *locationString);

BOOL ov00_021EFC6C(void) {
    if (ov00_0221A90C.state && ov00_0221A90C.state->state == 5) {
        return TRUE;
    }
    return FALSE;
}

int ov00_021EFC98(const DwcAccData *friendData, char *locationString) {
    return ov00_021EFCB0(friendData, NULL, NULL, locationString);
}

// Buddy status of a friend; a and b get its "SCM" / "SCN" matchmaking fields.
u8 ov00_021EFCB0(const DwcAccData *friendData, u8 *a, u8 *b, char *locationString) {
    FriendBuddyStatus status;
    char value[4];
    if (ov00_021F0B8C(friendData, &status)) {
        if (status.status == 6) {
            if (a) {
                if (ov00_021ED4E8(ov00_02217A08, value, status.statusString, '/') <= 0) {
                    *a = 0;
                } else {
                    *a = strtoul(value, NULL, 10);
                }
            }
            if (b) {
                if (ov00_021ED4E8(ov00_02217A0C, value, status.statusString, '/') <= 0) {
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

int ov00_021EFDCC(const DwcAccData *friendData, char *data, int *size) {
    return ov00_021EFDE8(friendData, NULL, NULL, data, size);
}

// Buddy status of a friend with the game data decoded from its location string.
int ov00_021EFDE8(const DwcAccData *friendData, u8 *a, u8 *b, char *data, int *size) {
    char buf[256];
    int status = ov00_021EFCB0(friendData, a, b, buf);
    if (!status) {
        *size = -1;
        return status;
    }
    *size = ov00_021EBF90(buf, strlen(buf), NULL, 0);
    if (!data || *size == -1) {
        return status;
    }
    ov00_021EBF90(buf, strlen(buf), data, *size);
    return status;
}

// Sets the game data in the own location string.
BOOL ov00_021EFE78(const u8 *data, int size) {
    char buf[256];
    int len;
    if (!ov00_0221A918.work || !ov00_021EFC6C()) {
        return FALSE;
    }
    len = ov00_021EBE08(data, size, buf, 255);
    if (len == -1) {
        return FALSE;
    }
    buf[len] = 0;
    return ov00_021F04A0(-1, NULL, buf) == 0;
}

BOOL ov00_021EFF04(void *callback, void *param) {
    if (!ov00_0221A918.work) {
        return FALSE;
    }
    ov00_0221A918.work->buddyCallback = callback;
    ov00_0221A918.work->buddyParam = param;
    return TRUE;
}

int ov00_021EFF30(const u8 *src, int len, char *dest, int destSize) {
    return ov00_021EBE08(src, len, dest, destSize);
}

int ov00_021EFF3C(char *src, int len, char *dest, int destSize) {
    return ov00_021EBF90(src, len, dest, destSize);
}

void ov00_021EFF48(FriendWork *work, GPConnection *connection, u16 *playerName, DwcAccData *list, int count) {
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

void ov00_021F0028(void) {
    FriendWork *work;
    if (!ov00_0221A918.work) {
        return;
    }
    if (ov00_021EC230()) {
        return;
    }
    if (!ov00_0221A918.work->list) {
        if (ov00_0221A918.work->connection && *ov00_0221A918.work->connection) {
            ov00_021F05EC();
        }
        return;
    }
    if (ov00_021F1248() || ov00_0220AED0()) {
        ov00_0221A918.busy = 1;
        ov00_0221A918.deferred = 0;
        ov00_0220AEF0();
        ov00_0221A918.busy = 0;
        if (ov00_0221A918.deferred == 1) {
            ov00_0221A918.deferred = 0;
            ov00_0220AE60();
        }
    }
    if (ov00_0221A918.work->connection && *ov00_0221A918.work->connection) {
        if (ov00_021F05EC()) {
            return;
        }
        work = ov00_0221A918.work;
        if (!work->state) {
            return;
        }
        if (work->list && work->updated != 3 && (u32)work->pollCount > 7) {
            if (work->updated <= 1) {
                ov00_021F06A8(work->list, work->count);
            }
            if (ov00_0221A918.work->updateCount >= ov00_0221A918.work->count) {
                ov00_0221A918.work->updated = 3;
                ov00_0221A918.work->pendingSteps++;
            }
        }
    }
    if (ov00_0221A918.work->pendingSteps >= 2) {
        ov00_0221A918.work->pendingSteps = 0;
        ov00_021F0914();
    }
}

// Starts the friend list update.
void ov00_021F018C(const char *token, const char *challenge, void *updateCallback, void *updateParam, void *statusCallback, void *statusParam, void *deleteCallback, void *deleteParam) {
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
void ov00_021F0238(int kind, int code) {
    if (!ov00_0221A918.work || !kind) {
        return;
    }
    ov00_021EC24C(kind, code);
    if (ov00_0221A918.work->state != 0 && ov00_0221A918.work->state != 2) {
        ov00_0221A918.work->updateCallback(kind, ov00_0221A918.work->changed, ov00_0221A918.work->updateParam);
    }
    ov00_021F0678();
}

// Buddy request received.
void ov00_021F0290(GPConnection *connection, void *arg, void *param) {
    int *request = arg;
    if (!ov00_0221A918.work->list) {
        return;
    }
    ov00_02200020(connection, *request, 0, 0, (GPCallback)ov00_021F0EF8, NULL);
}

// Buddy message: the "authorized" message makes the friend a buddy.
BOOL ov00_021F02DC(GPConnection *connection, void *arg, void *param) {
    GpBuddyMessage *message = arg;
    if (strcmp(message->message, ov00_02217A10) != 0) {
        return FALSE;
    }
    ov00_02200020(connection, message->profileId, 0, 0, (GPCallback)ov00_021F10A8, NULL);
    return TRUE;
}

// Buddy status changed.
void ov00_021F0338(GPConnection *connection, void *arg, void *param) {
    GpBuddyStatusArg *buddy = arg;
    FriendBuddyStatus status;
    int index;
    if (!ov00_0221A918.work->statusCallback) {
        return;
    }
    index = ov00_021F0404(buddy->profileId);
    if (index == -1) {
        return;
    }
    ov00_02200400(connection, buddy->index, &status);
    ov00_0221A918.work->statusCallback(index, status.status & 0xFF, status.locationString, ov00_0221A918.work->statusParam);
}

// GameSpy profile id of the friend at index, 0 if it has none.
int ov00_021F03BC(int index) {
    DwcAccData *list = ov00_0221A918.work->list;
    int profileId;
    if (!list) {
        return 0;
    }
    profileId = DWC_GetGsProfileId(ov00_021EF3A4(), &list[index]);
    if (profileId != 0 && profileId != -1) {
        return profileId;
    }
    return 0;
}

// Friend list index of a GameSpy profile id, -1 if it is not in the list.
int ov00_021F0404(int profileId) {
    int i;
    if (!ov00_0221A918.work || !profileId) {
        return -1;
    }
    for (i = 0; i < ov00_0221A918.work->count; i++) {
        if (profileId == ov00_021F03BC(i)) {
            return i;
        }
    }
    return -1;
}

void ov00_021F0468(void) {
    if (!ov00_0221A918.work) {
        return;
    }
    ov00_0221A918.work->pollCount = 0;
    ov00_0221A918.work->pollTick = OS_GetTick();
}

// Sets the own buddy status; -1 / NULL keep the current values.
int ov00_021F04A0(int status, const char *statusString, const char *locationString) {
    if (!ov00_0221A918.work || !ov00_021EFC6C()) {
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
    return ov00_02200654(ov00_0221A918.work->connection, status, statusString, locationString);
}

// The friend at index became a buddy.
void ov00_021F0550(int index) {
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
        int status = ov00_021EFC98(&ov00_0221A918.work->list[index], buf.locationString);
        ov00_0221A918.work->statusCallback(index, status, buf.locationString, ov00_0221A918.work->statusParam);
    }
}

void ov00_021F05D8(void) {
    ov00_0221A918.work = NULL;
}

// Processes the presence connection every 300 ms.
int ov00_021F05EC(void) {
    int error = 0;
    FriendWork *work = ov00_0221A918.work;
    if (((OS_GetTick() - work->pollTick) << 6) / 33514 >= 300) {
        work->pollCount++;
        error = ov00_021FFD9C(ov00_0221A918.work->connection);
        ov00_0221A918.work->pollTick = OS_GetTick();
    }
    return error;
}

void ov00_021F0678(void) {
    if (!ov00_0221A918.work) {
        return;
    }
    ov00_0221A918.work->state = 0;
    ov00_0221A918.work->updated = 0;
    ov00_0221A918.work->pendingSteps = 0;
}

// Matches the friend list against the buddy list: drops buddies that are not friends, marks friends
// that are buddies, and looks up the profile ids of friends known by login id only.
void ov00_021F06A8(DwcAccData *list, int count) {
    struct {
        int i;
        volatile int count;
    } local;

    char userName[24];
    FriendBuddyStatus status;
    if (!ov00_0221A918.work->updated) {
        ov00_021F0C48(ov00_022003D0(ov00_0221A918.work->connection, (int *)&local.count));
        for (local.i = 0; local.i < local.count; local.i++) {
            int j;
            ov00_021F0C48(ov00_02200400(ov00_0221A918.work->connection, *(volatile int *)&local.i, &status));
            for (j = 0; j < count; j++) {
                if (status.profileId == ov00_021F03BC(j)) {
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
                ov00_021F0C48(ov00_022005FC(ov00_0221A918.work->connection, status.profileId));
                local.count--;
                local.i--;
            }
        }
        ov00_0221A918.work->updated = 1;
    }
    while (ov00_0221A918.work->updateCount < count) {
        u32 profileId = ov00_021F03BC(ov00_0221A918.work->updateCount);
        if (profileId) {
            if (!ov00_021F09A4(list, ov00_0221A918.work->updateCount, profileId)) {
                ov00_021F0C48(ov00_02200540(ov00_0221A918.work->connection, profileId, (int *)&local.i));
                if (local.i == -1) {
                    ov00_021F0B58(profileId);
                }
            }
        } else if (DWC_GetGsProfileId(ov00_021EF3A4(), &list[ov00_0221A918.work->updateCount]) == -1) {
            DWC_LoginIdToUserName(ov00_021EF3A4(), &list[ov00_0221A918.work->updateCount], userName);
            ov00_021FFF58(ov00_0221A918.work->connection, 0, 0, 0, 0, userName, 0, 0, (GPCallback)ov00_021F0CBC, (void *)(u32)ov00_0221A918.work->updateCount);
            ov00_0221A918.work->updated = 2;
            return;
        }
        ov00_0221A918.work->updateCount++;
    }
}

// Friend list update finished.
void ov00_021F0914(void) {
    ov00_0221A918.work->updateCallback(0, ov00_0221A918.work->changed, ov00_0221A918.work->updateParam);
    ov00_0221A918.work->state = 2;
}

// Clears the entry at index (a duplicate of the entry at other).
void ov00_021F094C(DwcAccData *list, int index, int other) {
    if (!ov00_0221A918.work) {
        return;
    }
    MI_CpuFill8(&list[index], 0, sizeof(DwcAccData));
    if (ov00_0221A918.work->deleteCallback) {
        ov00_0221A918.work->deleteCallback(index, other, ov00_0221A918.work->deleteParam);
    }
}
