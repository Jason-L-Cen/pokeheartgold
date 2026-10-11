#pragma force_active on
#include "global.h"

#include "dwc/gp_internal.h"

// The code below is compiled as ARM (32-bit) code.
#include <nitro/code32.h>

// Presence connection: the rest of "gpiProfile.c" (profile table lookups and iteration) and "gpiSearch.c"
// (requests to the search manager: profile searches, nick checks, new users, ... and their replies).

extern const char ov00_02219228[]; // "gpiProfileListAdd"
extern const char ov00_02219290[]; // "id > 0"
extern const char ov00_02219298[]; // "gpiProfile.c"
extern const char ov00_022192A8[]; // "gpiProcessSearch"
extern const char ov00_022192D0[]; // "gpiStartProfileSearch"
extern const char ov00_022192E8[]; // "gpsp.gs.nintendowifi.net"
extern const char ov00_02219328[]; // "Out of memory."
extern const char ov00_02219338[]; // "There was an error creating a socket."
extern const char ov00_02219360[]; // "There was an error making a socket non-blocking."
extern const char ov00_02219394[]; // "Could not resolve search mananger host name."
extern const char ov00_022193C4[]; // "address.sin_addr.s_addr != 0"
extern const char ov00_022193E4[]; // "gpiSearch.c"
extern const char ov00_022193F0[]; // "There was an error connecting a socket."
extern const char ov00_02219418[]; // "No search criteria."
extern const char ov00_0221942C[]; // "SM"
extern const char ov00_02219430[]; // "Could not connect to the search manager."
extern const char ov00_0221945C[]; // "\\search\\"
extern const char ov00_02219468[]; // "\\sesskey\\"
extern const char ov00_02219474[]; // "\\profileid\\"
extern const char ov00_02219480[]; // "\\namespaceid\\"
extern const char ov00_02219490[]; // "\\nick\\"
extern const char ov00_02219498[]; // "\\uniquenick\\"
extern const char ov00_022194A8[]; // "\\email\\"
extern const char ov00_022194B0[]; // "\\firstname\\"
extern const char ov00_022194BC[]; // "\\lastname\\"
extern const char ov00_022194C8[]; // "\\icquin\\"
extern const char ov00_022194D4[]; // "\\skip\\"
extern const char ov00_022194DC[]; // "\\valid\\"
extern const char ov00_022194E4[]; // "\\nicks\\"
extern const char ov00_022194EC[]; // "\\pass\\"
extern const char ov00_022194F4[]; // "\\pmatch\\"
extern const char ov00_02219500[]; // "\\productid\\"
extern const char ov00_0221950C[]; // "\\check\\"
extern const char ov00_02219514[]; // "\\newuser\\"
extern const char ov00_02219520[]; // "\\productID\\"
extern const char ov00_0221952C[]; // "\\cdkey\\"
extern const char ov00_02219534[]; // "\\others\\"
extern const char ov00_02219540[]; // "\\uniquesearch\\"
extern const char ov00_02219550[]; // "\\preferrednick\\"
extern const char ov00_02219560[]; // "0"
extern const char ov00_02219564[]; // "\\gamename\\"
extern const char ov00_02219570[]; // "\\final\\"
extern const char ov00_02219578[]; // "There was an error reading from the server."
extern const char ov00_022195A4[]; // "bsrdone"
extern const char ov00_022195AC[]; // "more"
extern const char ov00_022195B4[]; // "bsr"
extern const char ov00_022195B8[]; // "nick"
extern const char ov00_022195C0[]; // "uniquenick"
extern const char ov00_022195CC[]; // "firstname"
extern const char ov00_022195D8[]; // "lastname"
extern const char ov00_022195E4[]; // "email"
extern const char ov00_022195EC[]; // "Error reading from the search server."
extern const char ov00_02219614[]; // "vr"
extern const char ov00_02219618[]; // "nr"
extern const char ov00_0221961C[]; // "ndone"
extern const char ov00_02219624[]; // "psrdone"
extern const char ov00_0221962C[]; // "psr"
extern const char ov00_02219630[]; // "status"
extern const char ov00_02219638[]; // "statuscode"
extern const char ov00_02219644[]; // "cur"
extern const char ov00_02219648[]; // "\\pid\\"
extern const char ov00_02219650[]; // "nur"
extern const char ov00_02219654[]; // "others"
extern const char ov00_0221965C[]; // "odone"
extern const char ov00_02219664[]; // "o"
extern const char ov00_02219668[]; // "first"
extern const char ov00_02219670[]; // "last"
extern const char ov00_02219678[]; // "us"
extern const char ov00_0221967C[]; // "usdone"
extern const char ov00_02219684[]; // "count == arg->numSuggestedNicks"
extern char ov00_0221B16C[];       // game name

typedef struct HostEnt {
    char *name;
    char **aliases;
    int addrType;
    u32 **addrList;
} HostEnt;

typedef union SocketAddress {
    struct {
        u8 len;
        u8 family;
        u16 port;
        u32 addr;
    } in;

    u32 words[2];
} SocketAddress;

// Context of ov00_02207F1C.
typedef struct GpProfileMapData {
    GpConnection **connection;
    int (*func)(GpConnection **, GpProfile *, void *);
    void *data;
} GpProfileMapData;

// Context of ov00_02207EB4.
typedef struct GpFindByNickData {
    char *nick;
    char *email;
    GpProfile **profile;
    int found;
} GpFindByNickData;

// Context of ov00_02207F88.
typedef struct GpFindByBuddyIndexData {
    int index;
    GpProfile *profile;
} GpFindByBuddyIndexData;

// A profile found by a profile search / "\others\" request.
typedef struct GpSearchMatch {
    int profile;
    char nick[31];
    char uniquenick[21];
    char firstname[31];
    char lastname[31];
    char email[51];
} GpSearchMatch;

// Callback arguments of the searches.
typedef struct GpProfileSearchArg {
    int result;
    int numMatches;
    int more; // 0x600 more matches, 0x601 done
    GpSearchMatch *matches;
} GpProfileSearchArg;

typedef struct GpIsValidEmailArg {
    int result;
    char email[52];
    int isValid;
} GpIsValidEmailArg;

typedef struct GpNicksArg {
    int result;
    char email[52];
    int numNicks;
    char **nicks;
    char **uniquenicks;
} GpNicksArg;

typedef struct GpPlayerMatch {
    int profile;
    char nick[31];
    int playerStatus;
    char status[256];
} GpPlayerMatch;

typedef struct GpFindPlayersArg {
    int result;
    int productId;
    int numMatches;
    GpPlayerMatch *matches;
} GpFindPlayersArg;

typedef struct GpCheckArg {
    int result;
    int profile;
} GpCheckArg;

typedef struct GpSuggestUniqueArg {
    int result;
    int numSuggestedNicks;
    char **suggestedNicks;
} GpSuggestUniqueArg;

typedef struct GpOthersArg {
    int result;
    int numProfiles;
    GpSearchMatch *profiles;
} GpOthersArg;

extern int strcmp(const char *, const char *);
extern long atol(const char *);
extern char *strstr(const char *, const char *);
extern char *strcpy(char *, const char *);
extern void *memset(void *, int, u32);
extern void __msl_assertion_failed(const char *expr, const char *file, const char *func, int line);
extern HostEnt *SOC_GetHostByName(const char *name);
extern void *ov00_021EC3A8(int size);
extern void *ov00_021EC3BC(void *p, int size);
extern void ov00_021EC3D8(void *p);
extern void ov00_021FEF74(void *table, GpProfile *profile);
extern void ov00_021FF01C(void *table, GpProfile *profile);
extern GpProfile *ov00_021FF0C0(void *table, GpProfile *key);
extern int ov00_021FF264(void *table, int (*func)(GpProfile *, GpProfileMapData *), GpProfileMapData *data);
extern void ov00_021FF3C0(int ms);
extern void ov00_021FF408(char *string);
extern int ov00_021FF44C(int sock, int blocking);
extern int ov00_021FF804(int domain, int type, int protocol);
extern int ov00_021FF8A0(int sock, SocketAddress *address, int len);
extern int ov00_021FFA60(int sock);
extern void ov00_02202990(GpConnection **connection, int result, int fatal);
extern int ov00_02202A84(GpConnection **connection, GpCallback callback, void *arg, GpOperation *operation, int type);

int ov00_02207E54(GpConnection **connection, GpProfile *profile, GpFindByNickData *data);
int ov00_02207F00(GpProfile *profile, GpProfileMapData *data);
int ov00_02207F5C(GpConnection **connection, GpProfile *profile, GpFindByBuddyIndexData *data);

// "gpiProfileListAdd": the profile table entry of profileId, added when missing.
GpProfile *ov00_02207CEC(GpConnection **connection, int profileId) {
    GpConnection *iconnection = *connection;
    GpProfile *profile;
    if (!(profileId > 0)) {
        __msl_assertion_failed(ov00_02219290, ov00_02219298, ov00_02219228, 0x2B5);
    }
    if (profileId <= 0) {
        return 0;
    }
    if (ov00_02207DD0(connection, profileId, &profile)) {
        return profile;
    }
    {
        GpProfile entry = { 0 };
        entry.profileId = profileId;
        entry.userId = 0;
        entry.cache = 0;
        entry.authSig = 0;
        entry.peerSig = 0;
        entry.requestCount = 0;
        ov00_021FEF74(iconnection->profileTable, &entry);
    }
    iconnection->numProfiles++;
    if (ov00_02207DD0(connection, profileId, &profile)) {
        return profile;
    }
    return 0;
}

// Looks up a profile.
int ov00_02207DD0(GpConnection **connection, int profileId, GpProfile **profile) {
    GpConnection *iconnection = *connection;
    GpProfile key, *found;
    key.profileId = profileId;
    found = ov00_021FF0C0(iconnection->profileTable, &key);
    if (profile) {
        *profile = found;
    }
    return found != 0;
}

// Removes a profile by id.
void ov00_02207E0C(GpConnection **connection, int profileId) {
    GpConnection *iconnection = *connection;
    GpProfile *profile;
    if (!ov00_02207DD0(connection, profileId, &profile)) {
        return;
    }
    ov00_021FF01C(iconnection->profileTable, profile);
}

void ov00_02207E40(GpConnection **connection, GpProfile *profile) {
    ov00_021FF01C((*connection)->profileTable, profile);
}

int ov00_02207E54(GpConnection **connection, GpProfile *profile, GpFindByNickData *data) {
    GpInfo *info = profile->cache;
    if (info && strcmp(data->nick, info->nick) == 0 && strcmp(data->email, info->email) == 0) {
        *data->profile = profile;
        data->found = 1;
        return 0;
    }
    return 1;
}

// Finds the cached profile with this nick and email.
int ov00_02207EB4(GpConnection **connection, char *nick, char *email, GpProfile **profile) {
    GpFindByNickData data;
    data.nick = nick;
    data.email = email;
    data.profile = profile;
    data.found = 0;
    ov00_02207F1C(connection, (int (*)(GpConnection **, GpProfile *, void *))ov00_02207E54, &data);
    if (!data.found) {
        *profile = 0;
    }
    return 0;
}

int ov00_02207F00(GpProfile *profile, GpProfileMapData *data) {
    return data->func(data->connection, profile, data->data);
}

// Calls func for every profile until it returns 0; returns whether it went through all of them.
int ov00_02207F1C(GpConnection **connection, int (*func)(GpConnection **, GpProfile *, void *), void *data) {
    GpConnection *iconnection = *connection;
    GpProfileMapData map;
    map.func = func;
    map.data = data;
    map.connection = connection;
    return ov00_021FF264(iconnection->profileTable, ov00_02207F00, &map) == 0;
}

int ov00_02207F5C(GpConnection **connection, GpProfile *profile, GpFindByBuddyIndexData *data) {
    if (profile->buddyStatus && data->index == profile->buddyStatus->buddyIndex) {
        data->profile = profile;
        return 0;
    }
    return 1;
}

// The buddy with this index.
GpProfile *ov00_02207F88(GpConnection **connection, int index) {
    GpFindByBuddyIndexData data;
    data.index = index;
    data.profile = 0;
    ov00_02207F1C(connection, (int (*)(GpConnection **, GpProfile *, void *))ov00_02207F5C, &data);
    return data.profile;
}

// Whether the profile holds nothing worth keeping.
int ov00_02207FB8(GpProfile *profile) {
    if (profile && !profile->cache && !profile->buddyStatus && !profile->peerSig && !profile->authSig) {
        return 1;
    }
    return 0;
}

// "gpiStartProfileSearch": connects to the search manager.
int ov00_02207FF0(GpConnection **connection, GpOperation *operation) {
    GpSearchData *data = operation->data;
    HostEnt *host;
    int error;
    data->inputBuffer.size = 4096;
    data->inputBuffer.buffer = ov00_021EC3A8(4097);
    if (!data->inputBuffer.buffer) {
        ov00_0220A890(connection, ov00_02219328);
        return 1;
    }
    data->sock = ov00_021FF804(2, 1, 0);
    if (data->sock == -1) {
        ov00_0220A86C(connection, 5, ov00_02219338);
        ov00_02202990(connection, 3, 1);
        return 3;
    }
    if (ov00_021FF44C(data->sock, 0) == 0) {
        ov00_0220A86C(connection, 5, ov00_02219360);
        ov00_02202990(connection, 3, 1);
        return 3;
    }
    host = SOC_GetHostByName(ov00_022192E8);
    if (!host) {
        ov00_0220A86C(connection, 5, ov00_02219394);
        ov00_02202990(connection, 3, 1);
        return 3;
    }
    {
        SocketAddress address = { 0 };
        address.in.family = 2;
        address.in.addr = **host->addrList;
        if (!address.in.addr) {
            __msl_assertion_failed(ov00_022193C4, ov00_022193E4, ov00_022192D0, 89);
        }
        address.in.port = 0xcd74; // 29901, big-endian
        if (ov00_021FF8A0(data->sock, &address, 8) == -1) {
            error = ov00_021FFA60(data->sock);
            if (error != -6 && error != -26 && error != -76) {
                ov00_0220A86C(connection, 5, ov00_022193F0);
                ov00_02202990(connection, 3, 1);
                return 3;
            }
        }
        operation->state = 1;
        return 0;
    }
}

// Allocates the data of a search operation.
int ov00_022081F0(GpConnection **connection, GpSearchData **data, int type) {
    GpSearchData *search = ov00_021EC3A8(sizeof(GpSearchData));
    if (!search) {
        ov00_0220A890(connection, ov00_02219328);
        return 1;
    }
    memset(search, 0, sizeof(GpSearchData));
    search->type = type;
    search->sock = -1;
    search->inputBuffer.buffer = 0;
    search->inputBuffer.len = 0;
    search->inputBuffer.pos = 0;
    search->inputBuffer.size = 0;
    search->outputBuffer.len = 0;
    search->outputBuffer.pos = 0;
    search->outputBuffer.size = 0x1000;
    search->outputBuffer.buffer = ov00_021EC3A8(0x1001);
    if (!search->outputBuffer.buffer) {
        ov00_0220A890(connection, ov00_02219328);
        return 1;
    }
    search->busy = 0;
    search->done = 0;
    *data = search;
    return 0;
}

// Starts a search operation.
int ov00_022082A0(GpConnection **connection, GpSearchData *data, int blocking, void *callback, void *param) {
    GpConnection *iconnection = *connection;
    GpOperation *operation;
    int result;
    iconnection->numSearches++;
    result = ov00_0220657C(connection, GP_OPERATION_SEARCH, data, &operation, blocking, callback, param);
    if (result) {
        return result;
    }
    result = ov00_02207FF0(connection, operation);
    if (result) {
        return result;
    }
    if (operation->blocking) {
        result = ov00_02200ED8(connection, operation->id);
        if (result) {
            return result;
        }
    }
    return 0;
}

// Searches for profiles matching the given fields.
int ov00_02208330(GpConnection **connection, char *nick, char *uniquenick, char *email, char *firstname, char *lastname, int icquin, int skip, int blocking, void *callback, void *param) {
    GpSearchData *data;
    int result;
    if ((!nick || !*nick) && (!email || !*email) && (!firstname || !*firstname) && (!lastname || !*lastname) && !icquin && (!uniquenick || !*uniquenick)) {
        ov00_0220A890(connection, ov00_02219418);
        return 2;
    }
    result = ov00_022081F0(connection, &data, GP_SEARCH_PROFILE);
    if (result != 0) {
        return result;
    }
    if (nick == 0) {
        data->nick[0] = 0;
    } else {
        ov00_0220A348(data->nick, nick, 31);
    }
    if (uniquenick == 0) {
        data->uniquenick[0] = 0;
    } else {
        ov00_0220A348(data->uniquenick, uniquenick, 21);
    }
    if (email == 0) {
        data->email[0] = 0;
    } else {
        ov00_0220A348(data->email, email, 51);
    }
    ov00_021FF408(data->email);
    if (firstname == 0) {
        data->firstname[0] = 0;
    } else {
        ov00_0220A348(data->firstname, firstname, 31);
    }
    if (lastname == 0) {
        data->lastname[0] = 0;
    } else {
        ov00_0220A348(data->lastname, lastname, 31);
    }
    data->icquin = icquin;
    if (skip < 0) {
        skip = 0;
    }
    data->skip = skip;
    result = ov00_022082A0(connection, data, blocking, callback, param);
    if (result == 0) {
        return 0;
    }
    return result;
}

#define SEARCH_SEND(string) ov00_022020EC(connection, &data->outputBuffer, (string))
#define SEARCH_SEND_INT(n)  ov00_0220211C(connection, &data->outputBuffer, (n))

// Reads the next "\key\value" pair of the reply.
#define SEARCH_READ_PAIR()                                                              \
    do {                                                                                \
        result = ov00_0220A69C(connection, data->inputBuffer.buffer, &pos, key, value); \
        if (result)                                                                     \
            return result;                                                              \
    } while (0)

#define SEARCH_PARSE_ERROR()                         \
    do {                                             \
        ov00_0220A86C(connection, 1, ov00_022195EC); \
        ov00_02202990(connection, 3, 1);             \
        return 3;                                    \
    } while (0)

#define SEARCH_OUT_OF_MEMORY()                    \
    do {                                          \
        ov00_0220A890(connection, ov00_02219328); \
        return 1;                                 \
    } while (0)

#define SEARCH_CALLBACK(callback, arg, type)                                      \
    do {                                                                          \
        result = ov00_02202A84(connection, (callback), (arg), operation, (type)); \
        if (result)                                                               \
            return result;                                                        \
    } while (0)

// "gpiProcessSearch": sends the request once connected, then handles the reply.
int ov00_0220851C(GpConnection **connection, GpOperation *operation) {
    char key[512], value[512];
    GpProfileSearchArg profileSearch;
    int state, pos;
    GpCallback profileSearchCallback;
    int bytesRead, closed;
    GpCallback suggestCallback, othersCallback, newUserCallback, checkCallback, playersCallback, nicksCallback, isValidCallback;
    int done;
    GpConnection *iconnection = *connection;
    GpSearchData *data = operation->data;
    int loop = operation->blocking != 0, result, mark, stop;
    do {
        result = ov00_0220267C(connection, data->sock, &data->outputBuffer, &closed, 1, ov00_0221942C);
        if (result) {
            return result;
        }
        if (operation->state == 1) {
            result = ov00_0220A5A8(connection, data->sock, &state);
            if (result) {
                return result;
            }
            if (state == 4) {
                ov00_0220A86C(connection, 0xd01, ov00_02219430);
                ov00_02202990(connection, 4, 0);
                return 4;
            }
            if (state == 3) {
                if (data->type == GP_SEARCH_PROFILE) {
                    SEARCH_SEND(ov00_0221945C);
                    SEARCH_SEND(ov00_02219468);
                    SEARCH_SEND_INT(iconnection->sessKey);
                    SEARCH_SEND(ov00_02219474);
                    SEARCH_SEND_INT(iconnection->profileId);
                    SEARCH_SEND(ov00_02219480);
                    SEARCH_SEND_INT(iconnection->namespaceId);
                    if (data->nick[0] != 0) {
                        SEARCH_SEND(ov00_02219490);
                        SEARCH_SEND(data->nick);
                    }
                    if (data->uniquenick[0] != 0) {
                        SEARCH_SEND(ov00_02219498);
                        SEARCH_SEND(data->uniquenick);
                    }
                    if (data->email[0] != 0) {
                        SEARCH_SEND(ov00_022194A8);
                        SEARCH_SEND(data->email);
                    }
                    if (data->firstname[0] != 0) {
                        SEARCH_SEND(ov00_022194B0);
                        SEARCH_SEND(data->firstname);
                    }
                    if (data->lastname[0] != 0) {
                        SEARCH_SEND(ov00_022194BC);
                        SEARCH_SEND(data->lastname);
                    }
                    if (data->icquin != 0) {
                        SEARCH_SEND(ov00_022194C8);
                        SEARCH_SEND_INT(data->icquin);
                    }
                    if (data->skip > 0) {
                        SEARCH_SEND(ov00_022194D4);
                        SEARCH_SEND_INT(data->skip);
                    }
                } else if (data->type == GP_SEARCH_IS_VALID) {
                    SEARCH_SEND(ov00_022194DC);
                    SEARCH_SEND(ov00_022194A8);
                    SEARCH_SEND(data->email);
                } else if (data->type == GP_SEARCH_NICKS) {
                    SEARCH_SEND(ov00_022194E4);
                    SEARCH_SEND(ov00_022194A8);
                    SEARCH_SEND(data->email);
                    SEARCH_SEND(ov00_022194EC);
                    SEARCH_SEND(data->password);
                    SEARCH_SEND(ov00_02219480);
                    SEARCH_SEND_INT(iconnection->namespaceId);
                } else if (data->type == GP_SEARCH_PLAYERS) {
                    SEARCH_SEND(ov00_022194F4);
                    SEARCH_SEND(ov00_02219468);
                    SEARCH_SEND_INT(iconnection->sessKey);
                    SEARCH_SEND(ov00_02219474);
                    SEARCH_SEND_INT(iconnection->profileId);
                    SEARCH_SEND(ov00_02219500);
                    SEARCH_SEND_INT(data->productId);
                } else if (data->type == GP_SEARCH_CHECK) {
                    SEARCH_SEND(ov00_0221950C);
                    SEARCH_SEND(ov00_02219490);
                    SEARCH_SEND(data->nick);
                    SEARCH_SEND(ov00_022194A8);
                    SEARCH_SEND(data->email);
                    SEARCH_SEND(ov00_022194EC);
                    SEARCH_SEND(data->password);
                } else if (data->type == GP_SEARCH_NEW_USER) {
                    SEARCH_SEND(ov00_02219514);
                    SEARCH_SEND(ov00_02219490);
                    SEARCH_SEND(data->nick);
                    SEARCH_SEND(ov00_022194A8);
                    SEARCH_SEND(data->email);
                    SEARCH_SEND(ov00_022194EC);
                    SEARCH_SEND(data->password);
                    SEARCH_SEND(ov00_02219520);
                    SEARCH_SEND_INT(iconnection->productId);
                    SEARCH_SEND(ov00_02219480);
                    SEARCH_SEND_INT(iconnection->namespaceId);
                    SEARCH_SEND(ov00_02219498);
                    SEARCH_SEND(data->uniquenick);
                    if (data->cdkey[0] != 0) {
                        SEARCH_SEND(ov00_0221952C);
                        SEARCH_SEND(data->cdkey);
                    }
                } else if (data->type == GP_SEARCH_OTHERS) {
                    SEARCH_SEND(ov00_02219534);
                    SEARCH_SEND(ov00_02219468);
                    SEARCH_SEND_INT(iconnection->sessKey);
                    SEARCH_SEND(ov00_02219474);
                    SEARCH_SEND_INT(iconnection->profileId);
                    SEARCH_SEND(ov00_02219480);
                    SEARCH_SEND_INT(iconnection->namespaceId);
                } else if (data->type == GP_SEARCH_SUGGEST_UNIQUE) {
                    SEARCH_SEND(ov00_02219540);
                    SEARCH_SEND(ov00_02219550);
                    SEARCH_SEND(data->uniquenick);
                    SEARCH_SEND(ov00_02219480);
                    SEARCH_SEND_INT(iconnection->namespaceId);
                } else {
                    __msl_assertion_failed(ov00_02219560, ov00_022193E4, ov00_022192A8, 0x275);
                }
                SEARCH_SEND(ov00_02219564);
                SEARCH_SEND(ov00_0221B16C);
                SEARCH_SEND(ov00_02219570);
                operation->state = 4;
            }
        } else if (operation->state == 4) {
            result = ov00_02202454(connection, data->sock, &data->inputBuffer, &bytesRead, &closed, ov00_0221942C);
            if (result) {
                if (result != 3) {
                    return result;
                }
                ov00_0220A86C(connection, 0xd01, ov00_02219578);
                ov00_02202990(connection, 3, 0);
                return 3;
            }
            if (strstr(data->inputBuffer.buffer, ov00_02219570)) {
                pos = 0;
                operation->state = 5;
                if (ov00_0220A3C8(connection, data->inputBuffer.buffer, 1)) {
                    data->done = 1;
                    return 4;
                }
                if (data->type == GP_SEARCH_PROFILE) {
                    done = 0;
                    profileSearch.result = 0;
                    profileSearch.numMatches = 0;
                    profileSearch.matches = 0;
                    profileSearch.more = 0x601;
                    do {
                        SEARCH_READ_PAIR();
                        if (!strcmp(key, ov00_022195A4)) {
                            SEARCH_READ_PAIR();
                            if (!strcmp(key, ov00_022195AC)) {
                                if (strcmp(value, ov00_02219560)) {
                                    profileSearch.more = 0x600;
                                }
                            }
                            done = 1;
                        } else if (!strcmp(key, ov00_022195B4)) {
                            GpSearchMatch *matches, *match;
                            int offset;
                            profileSearch.numMatches++;
                            matches = ov00_021EC3BC(profileSearch.matches, profileSearch.numMatches * sizeof(GpSearchMatch));
                            profileSearch.matches = matches;
                            if (!matches) {
                                SEARCH_OUT_OF_MEMORY();
                            }
                            offset = (profileSearch.numMatches - 1) * sizeof(GpSearchMatch);
                            match = (GpSearchMatch *)((u8 *)matches + offset);
                            memset(match, 0, sizeof(GpSearchMatch));
                            *(int *)((u8 *)matches + offset) = atol(value);
                            stop = 0;
                            do {
                                mark = pos;
                                SEARCH_READ_PAIR();
                                if (!strcmp(key, ov00_022195B8)) {
                                    ov00_0220A348(match->nick, value, 31);
                                } else if (!strcmp(key, ov00_022195C0)) {
                                    ov00_0220A348(match->uniquenick, value, 21);
                                } else if (!strcmp(key, ov00_022195CC)) {
                                    ov00_0220A348(match->firstname, value, 31);
                                } else if (!strcmp(key, ov00_022195D8)) {
                                    ov00_0220A348(match->lastname, value, 31);
                                } else if (!strcmp(key, ov00_022195E4)) {
                                    ov00_0220A348(match->email, value, 51);
                                } else if (!strcmp(key, ov00_022195B4) || !strcmp(key, ov00_022195A4)) {
                                    pos = mark;
                                    stop = 1;
                                }
                            } while (!stop);
                        } else {
                            SEARCH_PARSE_ERROR();
                        }
                    } while (!done);
                    profileSearchCallback = operation->callback;
                    {
                        int more = profileSearch.more;
                        if (profileSearchCallback.callback) {
                            ((void (*)(GpConnection **, void *, void *))profileSearchCallback.callback)(connection, &profileSearch, profileSearchCallback.param);
                        }
                        if (more == 0x600 && profileSearch.more == 0x600) {
                            result = ov00_02208330(connection, data->nick, data->uniquenick, data->email, data->firstname, data->lastname, data->icquin, profileSearch.numMatches + data->skip, operation->blocking, operation->callback.callback, operation->callback.param);
                            if (result) {
                                return result;
                            }
                        }
                    }
                    ov00_021EC3D8(profileSearch.matches);
                    profileSearch.matches = 0;
                } else if (data->type == GP_SEARCH_IS_VALID) {
                    isValidCallback = operation->callback;
                    if (isValidCallback.callback) {
                        GpIsValidEmailArg *arg;
                        SEARCH_READ_PAIR();
                        if (strcmp(key, ov00_02219614)) {
                            SEARCH_PARSE_ERROR();
                        }
                        arg = ov00_021EC3A8(sizeof(GpIsValidEmailArg));
                        if (!arg) {
                            SEARCH_OUT_OF_MEMORY();
                        }
                        arg->result = 0;
                        ov00_0220A348(arg->email, data->email, 51);
                        if (value[0] == '0') {
                            arg->isValid = 0;
                        } else {
                            arg->isValid = 1;
                        }
                        SEARCH_CALLBACK(isValidCallback, arg, 0);
                    }
                } else if (data->type == GP_SEARCH_NICKS) {
                    nicksCallback = operation->callback;
                    if (nicksCallback.callback) {
                        GpNicksArg *arg = ov00_021EC3A8(sizeof(GpNicksArg));
                        if (!arg) {
                            SEARCH_OUT_OF_MEMORY();
                        }
                        arg->result = 0;
                        strcpy(arg->email, data->email);
                        arg->numNicks = 0;
                        arg->nicks = 0;
                        arg->uniquenicks = 0;
                        SEARCH_READ_PAIR();
                        if (strcmp(key, ov00_02219618)) {
                            SEARCH_PARSE_ERROR();
                        }
                        done = 0;
                        do {
                            SEARCH_READ_PAIR();
                            if (!strcmp(key, ov00_022195B8)) {
                                char **nicks = ov00_021EC3BC(arg->nicks, (arg->numNicks + 1) * 4);
                                char *nick;
                                if (!nicks) {
                                    SEARCH_OUT_OF_MEMORY();
                                }
                                arg->nicks = nicks;
                                nick = ov00_021EC3A8(31);
                                if (!nick) {
                                    SEARCH_OUT_OF_MEMORY();
                                }
                                arg->nicks[arg->numNicks] = nick;
                                ov00_0220A348(arg->nicks[arg->numNicks], value, 31);
                                arg->numNicks++;
                            } else if (!strcmp(key, ov00_022195C0)) {
                                if (arg->numNicks > 0) {
                                    char **uniquenicks = ov00_021EC3BC(arg->uniquenicks, arg->numNicks * 4);
                                    char *uniquenick;
                                    if (!uniquenicks) {
                                        SEARCH_OUT_OF_MEMORY();
                                    }
                                    arg->uniquenicks = uniquenicks;
                                    uniquenick = ov00_021EC3A8(21);
                                    if (!uniquenick) {
                                        SEARCH_OUT_OF_MEMORY();
                                    }
                                    arg->uniquenicks[arg->numNicks - 1] = uniquenick;
                                    ov00_0220A348(arg->uniquenicks[arg->numNicks - 1], value, 21);
                                }
                            } else if (!strcmp(key, ov00_0221961C)) {
                                done = 1;
                            } else {
                                SEARCH_PARSE_ERROR();
                            }
                        } while (!done);
                        SEARCH_CALLBACK(nicksCallback, arg, 3);
                    }
                } else if (data->type == GP_SEARCH_PLAYERS) {
                    playersCallback = operation->callback;
                    if (playersCallback.callback) {
                        GpFindPlayersArg *arg = ov00_021EC3A8(sizeof(GpFindPlayersArg));
                        if (!arg) {
                            SEARCH_OUT_OF_MEMORY();
                        }
                        arg->productId = data->productId;
                        arg->result = 0;
                        arg->numMatches = 0;
                        arg->matches = 0;
                        done = 0;
                        do {
                            SEARCH_READ_PAIR();
                            if (!strcmp(key, ov00_02219624)) {
                                done = 1;
                            } else if (!strcmp(key, ov00_0221962C)) {
                                GpPlayerMatch *matches;
                                int i;
                                arg->numMatches++;
                                matches = arg->matches = ov00_021EC3BC(arg->matches, arg->numMatches * sizeof(GpPlayerMatch));
                                if (!matches) {
                                    SEARCH_OUT_OF_MEMORY();
                                }
                                i = arg->numMatches - 1;
                                memset(&matches[i], 0, sizeof(GpPlayerMatch));
                                matches[i].playerStatus = 1;
                                matches[i].profile = atol(value);
                                stop = 0;
                                do {
                                    mark = pos;
                                    SEARCH_READ_PAIR();
                                    if (!strcmp(key, ov00_02219630)) {
                                        ov00_0220A348(matches[i].status, value, 256);
                                    } else if (!strcmp(key, ov00_022195B8)) {
                                        ov00_0220A348(matches[i].nick, value, 31);
                                    }
                                    if (!strcmp(key, ov00_02219638)) {
                                        matches[i].playerStatus = atol(value);
                                    } else if (!strcmp(key, ov00_0221962C) || !strcmp(key, ov00_02219624)) {
                                        pos = mark;
                                        stop = 1;
                                    }
                                } while (!stop);
                            } else {
                                SEARCH_PARSE_ERROR();
                            }
                        } while (!done);
                        SEARCH_CALLBACK(playersCallback, arg, 4);
                    }
                } else if (data->type == GP_SEARCH_CHECK) {
                    checkCallback = operation->callback;
                    if (checkCallback.callback) {
                        int code, profile;
                        GpCheckArg *arg;
                        SEARCH_READ_PAIR();
                        if (strcmp(key, ov00_02219644)) {
                            SEARCH_PARSE_ERROR();
                        }
                        code = atol(value);
                        if (code) {
                            iconnection->errorCode = code;
                            profile = 0;
                        } else {
                            if (!ov00_0220A4A0(data->inputBuffer.buffer, ov00_02219648, value, 512)) {
                                SEARCH_PARSE_ERROR();
                            }
                            profile = atol(value);
                        }
                        arg = ov00_021EC3A8(sizeof(GpCheckArg));
                        if (!arg) {
                            SEARCH_OUT_OF_MEMORY();
                        }
                        arg->result = code;
                        arg->profile = profile;
                        SEARCH_CALLBACK(checkCallback, arg, 0);
                    }
                } else if (data->type == GP_SEARCH_NEW_USER) {
                    newUserCallback = operation->callback;
                    if (newUserCallback.callback) {
                        int code, profile;
                        GpCheckArg *arg;
                        SEARCH_READ_PAIR();
                        if (strcmp(key, ov00_02219650)) {
                            SEARCH_PARSE_ERROR();
                        }
                        code = atol(value);
                        if (code) {
                            iconnection->errorCode = code;
                        }
                        if (!ov00_0220A4A0(data->inputBuffer.buffer, ov00_02219648, value, 512)) {
                            if (!code) {
                                SEARCH_PARSE_ERROR();
                            }
                            profile = 0;
                        } else {
                            profile = atol(value);
                        }
                        arg = ov00_021EC3A8(sizeof(GpCheckArg));
                        if (!arg) {
                            SEARCH_OUT_OF_MEMORY();
                        }
                        arg->result = code;
                        arg->profile = profile;
                        SEARCH_CALLBACK(newUserCallback, arg, 0);
                    }
                } else if (data->type == GP_SEARCH_OTHERS) {
                    othersCallback = operation->callback;
                    if (othersCallback.callback) {
                        GpOthersArg *arg = ov00_021EC3A8(sizeof(GpOthersArg));
                        if (!arg) {
                            SEARCH_OUT_OF_MEMORY();
                        }
                        arg->result = 0;
                        arg->numProfiles = 0;
                        arg->profiles = 0;
                        SEARCH_READ_PAIR();
                        if (strcmp(key, ov00_02219654)) {
                            SEARCH_PARSE_ERROR();
                        }
                        done = 0;
                        do {
                            SEARCH_READ_PAIR();
                            if (!strcmp(key, ov00_0221965C)) {
                                done = 1;
                            } else if (!strcmp(key, ov00_02219664)) {
                                GpSearchMatch *profiles, *p;
                                int i;
                                p = ov00_021EC3BC(arg->profiles, (arg->numProfiles + 1) * sizeof(GpSearchMatch));
                                if (!p) {
                                    SEARCH_OUT_OF_MEMORY();
                                }
                                arg->profiles = p;
                                profiles = arg->profiles;
                                i = arg->numProfiles;
                                memset(&profiles[i], 0, sizeof(GpSearchMatch));
                                arg->numProfiles++;
                                profiles[i].profile = atol(value);
                                stop = 0;
                                do {
                                    mark = pos;
                                    SEARCH_READ_PAIR();
                                    if (!strcmp(key, ov00_022195B8)) {
                                        ov00_0220A348(profiles[i].nick, value, 31);
                                    } else if (!strcmp(key, ov00_022195C0)) {
                                        ov00_0220A348(profiles[i].uniquenick, value, 21);
                                    } else if (!strcmp(key, ov00_02219668)) {
                                        ov00_0220A348(profiles[i].firstname, value, 31);
                                    } else if (!strcmp(key, ov00_02219670)) {
                                        ov00_0220A348(profiles[i].lastname, value, 31);
                                    } else if (!strcmp(key, ov00_022195E4)) {
                                        ov00_0220A348(profiles[i].email, value, 51);
                                    } else if (!strcmp(key, ov00_02219664) || !strcmp(key, ov00_0221965C)) {
                                        pos = mark;
                                        stop = 1;
                                    }
                                } while (!stop);
                            } else {
                                SEARCH_PARSE_ERROR();
                            }
                        } while (!done);
                        SEARCH_CALLBACK(othersCallback, arg, 8);
                    }
                } else if (data->type == GP_SEARCH_SUGGEST_UNIQUE) {
                    suggestCallback = operation->callback;
                    if (suggestCallback.callback) {
                        int count = 0;
                        GpSuggestUniqueArg *arg = ov00_021EC3A8(sizeof(GpSuggestUniqueArg));
                        if (!arg) {
                            SEARCH_OUT_OF_MEMORY();
                        }
                        arg->result = 0;
                        arg->numSuggestedNicks = 0;
                        arg->suggestedNicks = 0;
                        SEARCH_READ_PAIR();
                        if (strcmp(key, ov00_02219678)) {
                            SEARCH_PARSE_ERROR();
                        }
                        arg->numSuggestedNicks = atol(value);
                        arg->suggestedNicks = ov00_021EC3A8(arg->numSuggestedNicks * 4);
                        if (!arg->suggestedNicks) {
                            SEARCH_OUT_OF_MEMORY();
                        }
                        done = count;
                        do {
                            SEARCH_READ_PAIR();
                            if (!strcmp(key, ov00_022195B8)) {
                                arg->suggestedNicks[count] = ov00_021EC3A8(21);
                                if (!arg->suggestedNicks[count]) {
                                    SEARCH_OUT_OF_MEMORY();
                                }
                                ov00_0220A348(arg->suggestedNicks[count], value, 21);
                                count++;
                            } else if (!strcmp(key, ov00_0221967C)) {
                                if (count != arg->numSuggestedNicks) {
                                    __msl_assertion_failed(ov00_02219684, ov00_022193E4, ov00_022192A8, 0x515);
                                }
                                arg->numSuggestedNicks = count;
                                done = 1;
                            } else {
                                SEARCH_PARSE_ERROR();
                            }
                        } while (!done);
                        SEARCH_CALLBACK(suggestCallback, arg, 9);
                    }
                } else {
                    __msl_assertion_failed(ov00_02219560, ov00_022193E4, ov00_022192A8, 0x52a);
                }
                data->done = 1;
                loop = 0;
            }
        }
        if (loop) {
            ov00_021FF3C0(10);
        }
    } while (loop);
    return 0;
}
