#pragma force_active on
#include <string.h>

#include "global.h"

#include "dwc/gp_internal.h"

// The code below is compiled as ARM (32-bit) code.
#include <nitro/code32.h>

// Presence connection: the rest of "gpiInfo.c" (setting the own profile info through "\updatepro\" /
// "\updateui\", "\getprofile\", the profile info cache) and "gpiOperation.c" (the pending operations:
// create, find, process the replies, fail, destroy).

extern const char ov00_02218BAC[]; // "\\profileid\\"
extern const char ov00_02218BC8[]; // "\\nick\\"
extern const char ov00_02218BD0[]; // "\\uniquenick\\"
extern const char ov00_02218BE0[]; // "\\email\\"
extern const char ov00_02218BE8[]; // "\\firstname\\"
extern const char ov00_02218BF4[]; // "\\lastname\\"
extern const char ov00_02218C00[]; // "\\icquin\\"
extern const char ov00_02218C0C[]; // "\\homepage\\"
extern const char ov00_02218C18[]; // "\\zipcode\\"
extern const char ov00_02218C24[]; // "\\countrycode\\"
extern const char ov00_02218C58[]; // "\\sex\\"
extern const char ov00_02218C68[]; // "\\aim\\"
extern const char ov00_02218C70[]; // "\\pic\\"
extern const char ov00_02218C78[]; // "\\occ\\"
extern const char ov00_02218C80[]; // "\\ind\\"
extern const char ov00_02218C88[]; // "\\inc\\"
extern const char ov00_02218C90[]; // "\\mar\\"
extern const char ov00_02218C98[]; // "\\chc\\"
extern const char ov00_02218CA0[]; // "\\i1\\"
extern const char ov00_02218CC0[]; // "Out of memory."
extern const char ov00_02218CD0[]; // "\\updatepro\\\\sesskey\\"
extern const char ov00_02218CE8[]; // "\\final\\"
extern const char ov00_02218CF0[]; // "\\updateui\\\\sesskey\\"
extern const char ov00_02218D04[]; // "Invalid zipcode."
extern const char ov00_02218D18[]; // "%d"
extern const char ov00_02218D1C[]; // "0"
extern const char ov00_02218D20[]; // "1"
extern const char ov00_02218D24[]; // "2"
extern const char ov00_02218D28[]; // "Invalid sex."
extern const char ov00_02218D38[]; // "\\cpubrandid\\"
extern const char ov00_02218D48[]; // "\\cpuspeed\\"
extern const char ov00_02218D54[]; // "\\memory\\"
extern const char ov00_02218D60[]; // "\\videocard1ram\\"
extern const char ov00_02218D70[]; // "\\videocard2ram\\"
extern const char ov00_02218D80[]; // "\\connectionid\\"
extern const char ov00_02218D90[]; // "\\connectionspeed\\"
extern const char ov00_02218DA4[]; // "\\hasnetwork\\"
extern const char ov00_02218DB4[]; // "Invalid info."
extern const char ov00_02218DC4[]; // "Invalid value."
extern const char ov00_02218DD4[]; // "\\password\\"
extern const char ov00_02218DE0[]; // "Invalid countrycode."
extern const char ov00_02218DF8[]; // "\\videocard1string\\"
extern const char ov00_02218E0C[]; // "\\videocard2string\\"
extern const char ov00_02218E20[]; // "\\osstring\\"
extern const char ov00_02218E2C[]; // "\\getprofile\\\\sesskey\\"
extern const char ov00_02218E44[]; // "\\id\\"
extern const char ov00_02218E4C[]; // "gpiDestroyOperation"
extern const char ov00_02218E60[]; // "gpiFailedOpCallback"
extern const char ov00_02218E74[]; // "gpiProcessOperation"
extern const char ov00_02218E88[]; // "connection != NULL"
extern const char ov00_02218E9C[]; // "gpiOperation.c"
extern const char ov00_02218EAC[]; // "*connection != NULL"
extern const char ov00_02218EC0[]; // "operation != NULL"
extern const char ov00_02218ED4[]; // "Out of memory."
extern const char ov00_02218EE4[]; // "0"
extern const char ov00_02218EE8[]; // "iconnection->numSearches >= 0"
extern const char ov00_02218F08[]; // "gpiProcessOperation was passed an operation with an invalid type (%d)\n"
extern const u8 __upper_mapC[];

extern int sprintf(char *, const char *, ...);
extern long atol(const char *);
extern void __msl_assertion_failed(const char *, const char *, const char *, int);
extern void *ov00_021EC3A8(u32 size);
extern void ov00_021EC3D8(void *p);
extern char *ov00_021FF3D4(const char *string);
extern void ov00_021FF408(char *string);
extern void ov00_021FF818(int sock);
extern void ov00_021FF82C(int sock, int how);
extern int ov00_02202A84(GpConnection **connection, GpCallback callback, void *arg, GpOperation *operation, int type);

// Profile info fields of ov00_022051A8 / ov00_02205678.
enum {
    GP_INFO_NICK = 1792,
    GP_INFO_UNIQUENICK,
    GP_INFO_EMAIL,
    GP_INFO_PASSWORD,
    GP_INFO_FIRSTNAME,
    GP_INFO_LASTNAME,
    GP_INFO_ICQUIN,
    GP_INFO_HOMEPAGE,
    GP_INFO_ZIPCODE,
    GP_INFO_COUNTRYCODE,
    GP_INFO_1802,
    GP_INFO_SEX,
    GP_INFO_CPUBRANDID,
    GP_INFO_CPUSPEED,
    GP_INFO_MEMORY,
    GP_INFO_VIDEOCARD1STRING,
    GP_INFO_VIDEOCARD1RAM,
    GP_INFO_VIDEOCARD2STRING,
    GP_INFO_VIDEOCARD2RAM,
    GP_INFO_CONNECTIONID,
    GP_INFO_CONNECTIONSPEED,
    GP_INFO_HASNETWORK,
    GP_INFO_OSSTRING,
    GP_INFO_AIM,
    GP_INFO_PIC,
    GP_INFO_OCCUPATION,
    GP_INFO_INDUSTRY,
    GP_INFO_INCOME,
    GP_INFO_MARRIED,
    GP_INFO_CHILDCOUNT,
    GP_INFO_INTERESTS1,
};

// sex values of the profile info
enum {
    GP_MALE = 1280,
    GP_FEMALE,
    GP_PAT,
};

// Failed-operation callback arguments of the types that carry only the result.
typedef struct GpResultArg8 {
    int result;
    int unk_04;
} GpResultArg8;

typedef struct GpResultArg16 {
    int result;
    int unk_04;
    int unk_08;
    int unk_0C;
} GpResultArg16;

// Appends the pending "\updatepro\" / "\updateui\" requests to outputBuffer.
int ov00_02205064(GpConnection **connection, GpBuffer *outputBuffer) {
    GpConnection *iconnection = *connection;
    if (iconnection->updateproBuffer.len > 0) {
        ov00_022020EC(connection, outputBuffer, ov00_02218CD0);
        ov00_0220211C(connection, outputBuffer, iconnection->sessKey);
        ov00_022020EC(connection, outputBuffer, iconnection->updateproBuffer.buffer);
        ov00_022020EC(connection, outputBuffer, ov00_02218CE8);
        iconnection->updateproBuffer.len = 0;
    }
    if (iconnection->updateuiBuffer.len > 0) {
        ov00_022020EC(connection, outputBuffer, ov00_02218CF0);
        ov00_0220211C(connection, outputBuffer, iconnection->sessKey);
        ov00_022020EC(connection, outputBuffer, iconnection->updateuiBuffer.buffer);
        ov00_022020EC(connection, outputBuffer, ov00_02218CE8);
        iconnection->updateuiBuffer.len = 0;
    }
    return 0;
}

int ov00_02205128(GpConnection **connection, const char *key, const char *value) {
    GpConnection *iconnection = *connection;
    int result = ov00_022020EC(connection, &iconnection->updateproBuffer, key);
    if (result) {
        return result;
    }
    result = ov00_022020EC(connection, &iconnection->updateproBuffer, value);
    if (result) {
        return result;
    }
    return 0;
}

int ov00_02205168(GpConnection **connection, const char *key, const char *value) {
    GpConnection *iconnection = *connection;
    int result = ov00_022020EC(connection, &iconnection->updateuiBuffer, key);
    if (result) {
        return result;
    }
    result = ov00_022020EC(connection, &iconnection->updateuiBuffer, value);
    if (result) {
        return result;
    }
    return 0;
}

// Sets an integer profile info field.
int ov00_022051A8(GpConnection **connection, int info, int value) {
    char buffer[16];
    int result;
    switch (info) {
    case GP_INFO_ZIPCODE:
        if (value < 0) {
            ov00_0220A890(connection, ov00_02218D04);
            return 2;
        }
        sprintf(buffer, ov00_02218D18, value);
        result = ov00_02205128(connection, ov00_02218C18, buffer);
        if (result) {
            return result;
        }
        break;
    case GP_INFO_SEX:
        switch (value) {
        case GP_MALE:
            result = ov00_02205128(connection, ov00_02218C58, ov00_02218D1C);
            if (result) {
                return result;
            }
            break;
        case GP_FEMALE:
            result = ov00_02205128(connection, ov00_02218C58, ov00_02218D20);
            if (result) {
                return result;
            }
            break;
        case GP_PAT:
            result = ov00_02205128(connection, ov00_02218C58, ov00_02218D24);
            if (result) {
                return result;
            }
            break;
        default:
            ov00_0220A890(connection, ov00_02218D28);
            return 2;
        }
        break;
    case GP_INFO_ICQUIN:
        sprintf(buffer, ov00_02218D18, value);
        result = ov00_02205128(connection, ov00_02218C00, buffer);
        if (result) {
            return result;
        }
        break;
    case GP_INFO_CPUBRANDID:
        sprintf(buffer, ov00_02218D18, value);
        result = ov00_02205168(connection, ov00_02218D38, buffer);
        if (result) {
            return result;
        }
        break;
    case GP_INFO_CPUSPEED:
        sprintf(buffer, ov00_02218D18, value);
        result = ov00_02205168(connection, ov00_02218D48, buffer);
        if (result) {
            return result;
        }
        break;
    case GP_INFO_MEMORY:
        sprintf(buffer, ov00_02218D18, value / 16);
        result = ov00_02205168(connection, ov00_02218D54, buffer);
        if (result) {
            return result;
        }
        break;
    case GP_INFO_VIDEOCARD1RAM:
        sprintf(buffer, ov00_02218D18, value / 4);
        result = ov00_02205168(connection, ov00_02218D60, buffer);
        if (result) {
            return result;
        }
        break;
    case GP_INFO_VIDEOCARD2RAM:
        sprintf(buffer, ov00_02218D18, value / 4);
        result = ov00_02205168(connection, ov00_02218D70, buffer);
        if (result) {
            return result;
        }
        break;
    case GP_INFO_CONNECTIONID:
        sprintf(buffer, ov00_02218D18, value);
        result = ov00_02205168(connection, ov00_02218D80, buffer);
        if (result) {
            return result;
        }
        break;
    case GP_INFO_CONNECTIONSPEED:
        sprintf(buffer, ov00_02218D18, value);
        result = ov00_02205168(connection, ov00_02218D90, buffer);
        if (result) {
            return result;
        }
        break;
    case GP_INFO_HASNETWORK:
        if (value) {
            value = 1;
        }
        sprintf(buffer, ov00_02218D18, value);
        result = ov00_02205168(connection, ov00_02218DA4, buffer);
        if (result) {
            return result;
        }
        break;
    case GP_INFO_PIC:
        sprintf(buffer, ov00_02218D18, value);
        result = ov00_02205128(connection, ov00_02218C70, buffer);
        if (result) {
            return result;
        }
        break;
    case GP_INFO_OCCUPATION:
        sprintf(buffer, ov00_02218D18, value);
        result = ov00_02205128(connection, ov00_02218C78, buffer);
        if (result) {
            return result;
        }
        break;
    case GP_INFO_INDUSTRY:
        sprintf(buffer, ov00_02218D18, value);
        result = ov00_02205128(connection, ov00_02218C80, buffer);
        if (result) {
            return result;
        }
        break;
    case GP_INFO_INCOME:
        sprintf(buffer, ov00_02218D18, value);
        result = ov00_02205128(connection, ov00_02218C88, buffer);
        if (result) {
            return result;
        }
        break;
    case GP_INFO_MARRIED:
        sprintf(buffer, ov00_02218D18, value);
        result = ov00_02205128(connection, ov00_02218C90, buffer);
        if (result) {
            return result;
        }
        break;
    case GP_INFO_CHILDCOUNT:
        sprintf(buffer, ov00_02218D18, value);
        result = ov00_02205128(connection, ov00_02218C98, buffer);
        if (result) {
            return result;
        }
        break;
    case GP_INFO_INTERESTS1:
        sprintf(buffer, ov00_02218D18, value);
        result = ov00_02205128(connection, ov00_02218CA0, buffer);
        if (result) {
            return result;
        }
        break;
    default:
        ov00_0220A890(connection, ov00_02218DB4);
        return 2;
    }
    return 0;
}

static inline int ToUpper(int c) {
    return (c < 0 || c >= 128) ? c : __upper_mapC[c];
}

// Sets a string profile info field (nick / uniquenick / email / password are also kept in the connection).
int ov00_02205678(GpConnection **connection, int info, const char *value) {
    GpConnection *iconnection = *connection;
    char buffer[256];
    int result;
    if (value == NULL) {
        ov00_0220A890(connection, ov00_02218DC4);
        return 2;
    }
    switch (info) {
    case GP_INFO_NICK:
        if (value[0] == 0) {
            ov00_0220A890(connection, ov00_02218DC4);
            return 2;
        }
        ov00_0220A348(buffer, value, 31);
        ov00_0220A348(iconnection->nick, buffer, 31);
        result = ov00_02205128(connection, ov00_02218BC8, buffer);
        if (result != 0) {
            return result;
        }
        break;
    case GP_INFO_UNIQUENICK:
        if (value[0] == 0) {
            ov00_0220A890(connection, ov00_02218DC4);
            return 2;
        }
        ov00_0220A348(buffer, value, 21);
        ov00_0220A348(iconnection->uniquenick, buffer, 21);
        result = ov00_02205128(connection, ov00_02218BD0, buffer);
        if (result != 0) {
            return result;
        }
        break;
    case GP_INFO_EMAIL:
        if (value[0] == 0) {
            ov00_0220A890(connection, ov00_02218DC4);
            return 2;
        }
        ov00_0220A348(buffer, value, 51);
        ov00_021FF408(buffer);
        ov00_0220A348(iconnection->email, buffer, 51);
        result = ov00_02205168(connection, ov00_02218BE0, buffer);
        if (result != 0) {
            return result;
        }
        break;
    case GP_INFO_PASSWORD:
        if (value[0] == 0) {
            ov00_0220A890(connection, ov00_02218DC4);
            return 2;
        }
        ov00_0220A348(buffer, value, 31);
        ov00_0220A348(iconnection->password, buffer, 31);
        result = ov00_02205168(connection, ov00_02218DD4, buffer);
        if (result != 0) {
            return result;
        }
        break;
    case GP_INFO_FIRSTNAME:
        ov00_0220A348(buffer, value, 31);
        result = ov00_02205128(connection, ov00_02218BE8, buffer);
        if (result != 0) {
            return result;
        }
        break;
    case GP_INFO_LASTNAME:
        ov00_0220A348(buffer, value, 31);
        result = ov00_02205128(connection, ov00_02218BF4, buffer);
        if (result != 0) {
            return result;
        }
        break;
    case GP_INFO_HOMEPAGE:
        ov00_0220A348(buffer, value, 76);
        result = ov00_02205128(connection, ov00_02218C0C, buffer);
        if (result != 0) {
            return result;
        }
        break;
    case GP_INFO_ZIPCODE:
        ov00_0220A348(buffer, value, 11);
        result = ov00_02205128(connection, ov00_02218C18, buffer);
        if (result != 0) {
            return result;
        }
        break;
    case GP_INFO_COUNTRYCODE:
        if (strlen(value) != 2) {
            ov00_0220A890(connection, ov00_02218DE0);
            return 2;
        }
        ov00_0220A348(buffer, value, 3);
        result = ov00_02205128(connection, ov00_02218C24, buffer);
        if (result != 0) {
            return result;
        }
        break;
    case GP_INFO_SEX: {
        char sex = ToUpper(value[0]);
        if (sex == 'M') {
            memcpy(buffer, ov00_02218D1C, 2);
        } else if (sex == 'F') {
            memcpy(buffer, ov00_02218D20, 2);
        } else {
            memcpy(buffer, ov00_02218D24, 2);
        }
        result = ov00_02205128(connection, ov00_02218C58, buffer);
        if (result != 0) {
            return result;
        }
        break;
    }
    case GP_INFO_ICQUIN:
        ov00_0220A348(buffer, value, 256);
        result = ov00_02205128(connection, ov00_02218C00, buffer);
        if (result != 0) {
            return result;
        }
        break;
    case GP_INFO_CPUSPEED:
        result = ov00_022051A8(connection, GP_INFO_CPUSPEED, atol(value));
        if (result != 0) {
            return result;
        }
        break;
    case GP_INFO_MEMORY:
        result = ov00_022051A8(connection, GP_INFO_MEMORY, atol(value));
        if (result != 0) {
            return result;
        }
        break;
    case GP_INFO_VIDEOCARD1STRING:
        ov00_0220A348(buffer, value, 256);
        result = ov00_02205128(connection, ov00_02218DF8, buffer);
        if (result != 0) {
            return result;
        }
        break;
    case GP_INFO_VIDEOCARD1RAM:
        result = ov00_022051A8(connection, GP_INFO_VIDEOCARD1RAM, atol(value));
        if (result != 0) {
            return result;
        }
        break;
    case GP_INFO_VIDEOCARD2STRING:
        ov00_0220A348(buffer, value, 256);
        result = ov00_02205128(connection, ov00_02218E0C, buffer);
        if (result != 0) {
            return result;
        }
        break;
    case GP_INFO_VIDEOCARD2RAM:
        result = ov00_022051A8(connection, GP_INFO_VIDEOCARD2RAM, atol(value));
        if (result != 0) {
            return result;
        }
        break;
    case GP_INFO_CONNECTIONSPEED:
        result = ov00_022051A8(connection, GP_INFO_CONNECTIONSPEED, atol(value));
        if (result != 0) {
            return result;
        }
        break;
    case GP_INFO_HASNETWORK:
        result = ov00_022051A8(connection, GP_INFO_HASNETWORK, atol(value));
        if (result != 0) {
            return result;
        }
        break;
    case GP_INFO_OSSTRING:
        ov00_0220A348(buffer, value, 256);
        result = ov00_02205128(connection, ov00_02218E20, buffer);
        if (result != 0) {
            return result;
        }
        break;
    case GP_INFO_AIM:
        ov00_0220A348(buffer, value, 51);
        result = ov00_02205128(connection, ov00_02218C68, buffer);
        if (result != 0) {
            return result;
        }
        break;
    case GP_INFO_PIC:
        ov00_0220A348(buffer, value, 256);
        result = ov00_02205128(connection, ov00_02218C70, buffer);
        if (result != 0) {
            return result;
        }
        break;
    case GP_INFO_OCCUPATION:
        ov00_0220A348(buffer, value, 256);
        result = ov00_02205128(connection, ov00_02218C78, buffer);
        if (result != 0) {
            return result;
        }
        break;
    case GP_INFO_INDUSTRY:
        ov00_0220A348(buffer, value, 256);
        result = ov00_02205128(connection, ov00_02218C80, buffer);
        if (result != 0) {
            return result;
        }
        break;
    case GP_INFO_INCOME:
        ov00_0220A348(buffer, value, 256);
        result = ov00_02205128(connection, ov00_02218C88, buffer);
        if (result != 0) {
            return result;
        }
        break;
    case GP_INFO_MARRIED:
        ov00_0220A348(buffer, value, 256);
        result = ov00_02205128(connection, ov00_02218C90, buffer);
        if (result != 0) {
            return result;
        }
        break;
    case GP_INFO_CHILDCOUNT:
        ov00_0220A348(buffer, value, 256);
        result = ov00_02205128(connection, ov00_02218C98, buffer);
        if (result != 0) {
            return result;
        }
        break;
    case GP_INFO_INTERESTS1:
        ov00_0220A348(buffer, value, 256);
        result = ov00_02205128(connection, ov00_02218CA0, buffer);
        if (result != 0) {
            return result;
        }
        break;
    default:
        ov00_0220A890(connection, ov00_02218DB4);
        return 2;
    }
    return 0;
}

// Sends "\getprofile\".
int ov00_02205E50(GpConnection **connection, int profileId, int id) {
    GpConnection *iconnection = *connection;
    ov00_022020EC(connection, &iconnection->outputBuffer, ov00_02218E2C);
    ov00_0220211C(connection, &iconnection->outputBuffer, iconnection->sessKey);
    ov00_022020EC(connection, &iconnection->outputBuffer, ov00_02218BAC);
    ov00_0220211C(connection, &iconnection->outputBuffer, profileId);
    ov00_022020EC(connection, &iconnection->outputBuffer, ov00_02218E44);
    ov00_0220211C(connection, &iconnection->outputBuffer, id);
    ov00_022020EC(connection, &iconnection->outputBuffer, ov00_02218CE8);
    return 0;
}

// Gets the info of a profile: answered from the cache when allowed (checkCache == 1), else requested.
int ov00_02205EE8(GpConnection **connection, int profileId, int checkCache, int blocking, void *callback, void *param) {
    struct {
        GpOperation *operation;
        GpProfile *profile;
    } frame;

    GpCallback pair;
    int useCache, id, result;
    GpConnection *iconnection = *connection;
    frame.operation = 0;
    useCache = (checkCache == 1);
    if (!iconnection->cacheProfileInfo) {
        useCache = 0;
    }
    if (callback && useCache && ov00_02207DD0(connection, profileId, &frame.profile) && frame.profile->cache) {
        GpInfoArg *arg = ov00_021EC3A8(sizeof(GpInfoArg));
        if (!arg) {
            ov00_0220A890(connection, ov00_02218CC0);
            return 1;
        }
        ov00_02204654(frame.profile->cache, arg);
        arg->result = 0;
        arg->profile = profileId;
        pair.callback = callback;
        pair.param = param;
        result = ov00_0220657C(connection, GP_OPERATION_GET_INFO, 0, &frame.operation, 1, callback, param);
        if (result) {
            return result;
        }
        id = frame.operation->id;
        result = ov00_02202A84(connection, pair, arg, frame.operation, 0);
        if (result) {
            return result;
        }
        ov00_022066D0(connection, frame.operation);
    } else {
        result = ov00_0220657C(connection, GP_OPERATION_GET_INFO, 0, &frame.operation, blocking, callback, param);
        if (result) {
            return result;
        }
        id = frame.operation->id;
        result = ov00_02205E50(connection, profileId, id);
        if (result) {
            return result;
        }
    }
    if (blocking) {
        result = ov00_02200ED8(connection, id);
        if (result) {
            return result;
        }
    }
    return 0;
}

// Caches a copy of the received profile info (with its strings).
int ov00_0220608C(GpConnection **connection, GpProfile *profile, GpInfo *info) {
    GpInfo *cache;
    if (!(*connection)->cacheProfileInfo) {
        return 1;
    }
    ov00_02206164(profile);
    cache = profile->cache = ov00_021EC3A8(sizeof(GpInfo));
    if (cache) {
        *cache = *info;
        profile->cache->nick = ov00_021FF3D4(info->nick);
        profile->cache->uniquenick = ov00_021FF3D4(info->uniquenick);
        profile->cache->email = ov00_021FF3D4(info->email);
        profile->cache->firstname = ov00_021FF3D4(info->firstname);
        profile->cache->lastname = ov00_021FF3D4(info->lastname);
        profile->cache->homepage = ov00_021FF3D4(info->homepage);
        profile->cache->aimname = ov00_021FF3D4(info->aimname);
    }
    return profile->cache != 0;
}

void ov00_02206164(GpProfile *profile) {
    if (!profile->cache) {
        return;
    }
    ov00_021EC3D8(profile->cache->nick);
    profile->cache->nick = 0;
    ov00_021EC3D8(profile->cache->uniquenick);
    profile->cache->uniquenick = 0;
    ov00_021EC3D8(profile->cache->email);
    profile->cache->email = 0;
    ov00_021EC3D8(profile->cache->firstname);
    profile->cache->firstname = 0;
    ov00_021EC3D8(profile->cache->lastname);
    profile->cache->lastname = 0;
    ov00_021EC3D8(profile->cache->homepage);
    profile->cache->homepage = 0;
    ov00_021EC3D8(profile->cache->aimname);
    profile->cache->aimname = 0;
    ov00_021EC3D8(profile->cache);
    profile->cache = 0;
}

// "gpiFailedOpCallback": tells the operation's callback about its error.
int ov00_02206230(GpConnection **connection, GpOperation *operation) {
    GpConnection *iconnection = *connection;
    GpCallback callback;
    if (!connection) {
        __msl_assertion_failed(ov00_02218E88, ov00_02218E9C, ov00_02218E60, 34);
    }
    if (!*connection) {
        __msl_assertion_failed(ov00_02218EAC, ov00_02218E9C, ov00_02218E60, 35);
    }
    if (!operation) {
        __msl_assertion_failed(ov00_02218EC0, ov00_02218E9C, ov00_02218E60, 36);
    }
    callback = operation->callback;
    if (callback.callback) {
        switch (operation->type) {
        case GP_OPERATION_CONNECT: {
            GpConnectArg *arg = ov00_021EC3A8(sizeof(GpConnectArg));
            if (!arg) {
                ov00_0220A890(connection, ov00_02218ED4);
                return 1;
            }
            memset(arg, 0, sizeof(GpConnectArg));
            arg->result = operation->result;
            if (iconnection->errorCode == 0x201) {
                arg->profile = iconnection->profileId;
                iconnection->profileId = 0;
            }
            {
                int result = ov00_02202A84(connection, callback, arg, operation, 0);
                if (result) {
                    return result;
                }
            }
            break;
        }
        case GP_OPERATION_NEW_PROFILE: {
            GpResultArg8 *arg = ov00_021EC3A8(sizeof(GpResultArg8));
            if (!arg) {
                ov00_0220A890(connection, ov00_02218ED4);
                return 1;
            }
            memset(arg, 0, sizeof(GpResultArg8));
            arg->result = operation->result;
            {
                int result = ov00_02202A84(connection, callback, arg, operation, 0);
                if (result) {
                    return result;
                }
            }
            break;
        }
        case GP_OPERATION_GET_INFO: {
            GpInfoArg *arg = ov00_021EC3A8(sizeof(GpInfoArg));
            if (!arg) {
                ov00_0220A890(connection, ov00_02218ED4);
                return 1;
            }
            memset(arg, 0, sizeof(GpInfoArg));
            arg->result = operation->result;
            {
                int result = ov00_02202A84(connection, callback, arg, operation, 0);
                if (result) {
                    return result;
                }
            }
            break;
        }
        case GP_OPERATION_SEARCH: {
            GpResultArg16 *arg = ov00_021EC3A8(sizeof(GpResultArg16));
            if (!arg) {
                ov00_0220A890(connection, ov00_02218ED4);
                return 1;
            }
            memset(arg, 0, sizeof(GpResultArg16));
            arg->result = operation->result;
            arg->unk_0C = 0;
            {
                int result = ov00_02202A84(connection, callback, arg, operation, 0);
                if (result) {
                    return result;
                }
            }
            break;
        }
        case GP_OPERATION_4: {
            int *arg = ov00_021EC3A8(sizeof(int));
            if (!arg) {
                ov00_0220A890(connection, ov00_02218ED4);
                return 1;
            }
            memset(arg, 0, sizeof(int));
            *arg = operation->result;
            {
                int result = ov00_02202A84(connection, callback, arg, operation, 0);
                if (result) {
                    return result;
                }
            }
            break;
        }
        default:
            __msl_assertion_failed(ov00_02218EE4, ov00_02218E9C, ov00_02218E60, 107);
        }
    }
    return 0;
}

// Adds an operation (id 1 for the connect operation).
int ov00_0220657C(GpConnection **connection, int type, void *data, GpOperation **operation, int blocking, void *callback, void *param) {
    GpConnection *iconnection = *connection;
    GpOperation *op = ov00_021EC3A8(sizeof(GpOperation));
    if (!op) {
        ov00_0220A890(connection, ov00_02218ED4);
        return 1;
    }
    op->type = type;
    op->data = data;
    op->blocking = blocking;
    op->state = 0;
    if (type == GP_OPERATION_CONNECT) {
        op->id = 1;
    } else {
        op->id = iconnection->nextOperationId++;
        if (iconnection->nextOperationId < 2) {
            iconnection->nextOperationId = 2;
        }
    }
    op->result = 0;
    op->callback.callback = callback;
    op->callback.param = param;
    op->next = iconnection->operations;
    iconnection->operations = op;
    *operation = op;
    return 0;
}

// "gpiDestroyOperation"
void ov00_02206634(GpConnection **connection, GpOperation *operation) {
    GpConnection *iconnection = *connection;
    if (operation->type == GP_OPERATION_SEARCH) {
        GpSearchData *data = operation->data;
        if (--iconnection->numSearches < 0) {
            __msl_assertion_failed(ov00_02218EE8, ov00_02218E9C, ov00_02218E4C, 0xb6);
        }
        ov00_021FF82C(data->sock, 2);
        ov00_021FF818(data->sock);
        ov00_021EC3D8(data->outputBuffer.buffer);
        data->outputBuffer.buffer = 0;
        ov00_021EC3D8(data->inputBuffer.buffer);
        data->inputBuffer.buffer = 0;
    }
    ov00_021EC3D8(operation->data);
    operation->data = 0;
    ov00_021EC3D8(operation);
}

// Unlinks and destroys an operation.
void ov00_022066D0(GpConnection **connection, GpOperation *operation) {
    GpConnection *iconnection = *connection;
    GpOperation *op = iconnection->operations, *prev = 0;
    while (op) {
        if (op == operation) {
            if (!prev) {
                iconnection->operations = op->next;
            } else {
                prev->next = operation->next;
            }
            ov00_02206634(connection, operation);
            return;
        }
        prev = op;
        op = op->next;
    }
}

// Finds the operation with the given id.
int ov00_02206720(GpConnection **connection, GpOperation **operation, int id) {
    GpOperation *op = (*connection)->operations;
    while (op) {
        if (op->id == id) {
            if (operation) {
                *operation = op;
            }
            return 1;
        }
        op = op->next;
    }
    if (operation) {
        *operation = 0;
    }
    return 0;
}

// Whether a blocking operation (other than a search) is pending.
int ov00_0220676C(GpConnection **connection) {
    GpOperation *op = (*connection)->operations;
    while (op) {
        if (op->blocking && op->type != GP_OPERATION_SEARCH) {
            return 1;
        }
        op = op->next;
    }
    return 0;
}

// "gpiProcessOperation": hands a server reply to its operation.
int ov00_022067A8(GpConnection **connection, GpOperation *operation, char *input) {
    int result = 0;
    switch (operation->type) {
    case GP_OPERATION_CONNECT:
        result = ov00_02203A58(connection, operation, input);
        break;
    case GP_OPERATION_NEW_PROFILE:
        result = ov00_02207B8C(connection, operation, input);
        break;
    case GP_OPERATION_GET_INFO:
        result = ov00_0220484C(connection, operation, input);
        break;
    case GP_OPERATION_4:
        result = ov00_0220A244(connection, operation, input);
        break;
    default:
        ov00_0220A3BC(connection, ov00_02218F08, operation->type);
        __msl_assertion_failed(ov00_02218EE4, ov00_02218E9C, ov00_02218E74, 326);
    }
    if (result) {
        operation->result = result;
    }
    return result;
}
