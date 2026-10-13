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
extern void *DWCi_GsMalloc(u32 size);
extern void DWCi_GsFree(void *p);
extern char *goastrdup(const char *string);
extern void _strlwr(char *string);
extern void gsSocketClose(int sock);
extern void gsSocketShutdown(int sock, int how);
extern int gpiAddCallback(GpConnection **connection, GpCallback callback, void *arg, GpOperation *operation, int type);

// Profile info fields of gpiSetInfoi / gpiSetInfos.
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
int gpiAddLocalInfo(GpConnection **connection, GpBuffer *outputBuffer) {
    GpConnection *iconnection = *connection;
    if (iconnection->updateproBuffer.len > 0) {
        gpiAppendStringToBuffer(connection, outputBuffer, ov00_02218CD0);
        gpiAppendIntToBuffer(connection, outputBuffer, iconnection->sessKey);
        gpiAppendStringToBuffer(connection, outputBuffer, iconnection->updateproBuffer.buffer);
        gpiAppendStringToBuffer(connection, outputBuffer, ov00_02218CE8);
        iconnection->updateproBuffer.len = 0;
    }
    if (iconnection->updateuiBuffer.len > 0) {
        gpiAppendStringToBuffer(connection, outputBuffer, ov00_02218CF0);
        gpiAppendIntToBuffer(connection, outputBuffer, iconnection->sessKey);
        gpiAppendStringToBuffer(connection, outputBuffer, iconnection->updateuiBuffer.buffer);
        gpiAppendStringToBuffer(connection, outputBuffer, ov00_02218CE8);
        iconnection->updateuiBuffer.len = 0;
    }
    return 0;
}

int gpiSendLocalInfo(GpConnection **connection, const char *key, const char *value) {
    GpConnection *iconnection = *connection;
    int result = gpiAppendStringToBuffer(connection, &iconnection->updateproBuffer, key);
    if (result) {
        return result;
    }
    result = gpiAppendStringToBuffer(connection, &iconnection->updateproBuffer, value);
    if (result) {
        return result;
    }
    return 0;
}

int gpiSendUserInfo(GpConnection **connection, const char *key, const char *value) {
    GpConnection *iconnection = *connection;
    int result = gpiAppendStringToBuffer(connection, &iconnection->updateuiBuffer, key);
    if (result) {
        return result;
    }
    result = gpiAppendStringToBuffer(connection, &iconnection->updateuiBuffer, value);
    if (result) {
        return result;
    }
    return 0;
}

// Sets an integer profile info field.
int gpiSetInfoi(GpConnection **connection, int info, int value) {
    char buffer[16];
    int result;
    switch (info) {
    case GP_INFO_ZIPCODE:
        if (value < 0) {
            gpiSetErrorString(connection, ov00_02218D04);
            return 2;
        }
        sprintf(buffer, ov00_02218D18, value);
        result = gpiSendLocalInfo(connection, ov00_02218C18, buffer);
        if (result) {
            return result;
        }
        break;
    case GP_INFO_SEX:
        switch (value) {
        case GP_MALE:
            result = gpiSendLocalInfo(connection, ov00_02218C58, ov00_02218D1C);
            if (result) {
                return result;
            }
            break;
        case GP_FEMALE:
            result = gpiSendLocalInfo(connection, ov00_02218C58, ov00_02218D20);
            if (result) {
                return result;
            }
            break;
        case GP_PAT:
            result = gpiSendLocalInfo(connection, ov00_02218C58, ov00_02218D24);
            if (result) {
                return result;
            }
            break;
        default:
            gpiSetErrorString(connection, ov00_02218D28);
            return 2;
        }
        break;
    case GP_INFO_ICQUIN:
        sprintf(buffer, ov00_02218D18, value);
        result = gpiSendLocalInfo(connection, ov00_02218C00, buffer);
        if (result) {
            return result;
        }
        break;
    case GP_INFO_CPUBRANDID:
        sprintf(buffer, ov00_02218D18, value);
        result = gpiSendUserInfo(connection, ov00_02218D38, buffer);
        if (result) {
            return result;
        }
        break;
    case GP_INFO_CPUSPEED:
        sprintf(buffer, ov00_02218D18, value);
        result = gpiSendUserInfo(connection, ov00_02218D48, buffer);
        if (result) {
            return result;
        }
        break;
    case GP_INFO_MEMORY:
        sprintf(buffer, ov00_02218D18, value / 16);
        result = gpiSendUserInfo(connection, ov00_02218D54, buffer);
        if (result) {
            return result;
        }
        break;
    case GP_INFO_VIDEOCARD1RAM:
        sprintf(buffer, ov00_02218D18, value / 4);
        result = gpiSendUserInfo(connection, ov00_02218D60, buffer);
        if (result) {
            return result;
        }
        break;
    case GP_INFO_VIDEOCARD2RAM:
        sprintf(buffer, ov00_02218D18, value / 4);
        result = gpiSendUserInfo(connection, ov00_02218D70, buffer);
        if (result) {
            return result;
        }
        break;
    case GP_INFO_CONNECTIONID:
        sprintf(buffer, ov00_02218D18, value);
        result = gpiSendUserInfo(connection, ov00_02218D80, buffer);
        if (result) {
            return result;
        }
        break;
    case GP_INFO_CONNECTIONSPEED:
        sprintf(buffer, ov00_02218D18, value);
        result = gpiSendUserInfo(connection, ov00_02218D90, buffer);
        if (result) {
            return result;
        }
        break;
    case GP_INFO_HASNETWORK:
        if (value) {
            value = 1;
        }
        sprintf(buffer, ov00_02218D18, value);
        result = gpiSendUserInfo(connection, ov00_02218DA4, buffer);
        if (result) {
            return result;
        }
        break;
    case GP_INFO_PIC:
        sprintf(buffer, ov00_02218D18, value);
        result = gpiSendLocalInfo(connection, ov00_02218C70, buffer);
        if (result) {
            return result;
        }
        break;
    case GP_INFO_OCCUPATION:
        sprintf(buffer, ov00_02218D18, value);
        result = gpiSendLocalInfo(connection, ov00_02218C78, buffer);
        if (result) {
            return result;
        }
        break;
    case GP_INFO_INDUSTRY:
        sprintf(buffer, ov00_02218D18, value);
        result = gpiSendLocalInfo(connection, ov00_02218C80, buffer);
        if (result) {
            return result;
        }
        break;
    case GP_INFO_INCOME:
        sprintf(buffer, ov00_02218D18, value);
        result = gpiSendLocalInfo(connection, ov00_02218C88, buffer);
        if (result) {
            return result;
        }
        break;
    case GP_INFO_MARRIED:
        sprintf(buffer, ov00_02218D18, value);
        result = gpiSendLocalInfo(connection, ov00_02218C90, buffer);
        if (result) {
            return result;
        }
        break;
    case GP_INFO_CHILDCOUNT:
        sprintf(buffer, ov00_02218D18, value);
        result = gpiSendLocalInfo(connection, ov00_02218C98, buffer);
        if (result) {
            return result;
        }
        break;
    case GP_INFO_INTERESTS1:
        sprintf(buffer, ov00_02218D18, value);
        result = gpiSendLocalInfo(connection, ov00_02218CA0, buffer);
        if (result) {
            return result;
        }
        break;
    default:
        gpiSetErrorString(connection, ov00_02218DB4);
        return 2;
    }
    return 0;
}

static inline int ToUpper(int c) {
    return (c < 0 || c >= 128) ? c : __upper_mapC[c];
}

// Sets a string profile info field (nick / uniquenick / email / password are also kept in the connection).
int gpiSetInfos(GpConnection **connection, int info, const char *value) {
    GpConnection *iconnection = *connection;
    char buffer[256];
    int result;
    if (value == NULL) {
        gpiSetErrorString(connection, ov00_02218DC4);
        return 2;
    }
    switch (info) {
    case GP_INFO_NICK:
        if (value[0] == 0) {
            gpiSetErrorString(connection, ov00_02218DC4);
            return 2;
        }
        strzcpy(buffer, value, 31);
        strzcpy(iconnection->nick, buffer, 31);
        result = gpiSendLocalInfo(connection, ov00_02218BC8, buffer);
        if (result != 0) {
            return result;
        }
        break;
    case GP_INFO_UNIQUENICK:
        if (value[0] == 0) {
            gpiSetErrorString(connection, ov00_02218DC4);
            return 2;
        }
        strzcpy(buffer, value, 21);
        strzcpy(iconnection->uniquenick, buffer, 21);
        result = gpiSendLocalInfo(connection, ov00_02218BD0, buffer);
        if (result != 0) {
            return result;
        }
        break;
    case GP_INFO_EMAIL:
        if (value[0] == 0) {
            gpiSetErrorString(connection, ov00_02218DC4);
            return 2;
        }
        strzcpy(buffer, value, 51);
        _strlwr(buffer);
        strzcpy(iconnection->email, buffer, 51);
        result = gpiSendUserInfo(connection, ov00_02218BE0, buffer);
        if (result != 0) {
            return result;
        }
        break;
    case GP_INFO_PASSWORD:
        if (value[0] == 0) {
            gpiSetErrorString(connection, ov00_02218DC4);
            return 2;
        }
        strzcpy(buffer, value, 31);
        strzcpy(iconnection->password, buffer, 31);
        result = gpiSendUserInfo(connection, ov00_02218DD4, buffer);
        if (result != 0) {
            return result;
        }
        break;
    case GP_INFO_FIRSTNAME:
        strzcpy(buffer, value, 31);
        result = gpiSendLocalInfo(connection, ov00_02218BE8, buffer);
        if (result != 0) {
            return result;
        }
        break;
    case GP_INFO_LASTNAME:
        strzcpy(buffer, value, 31);
        result = gpiSendLocalInfo(connection, ov00_02218BF4, buffer);
        if (result != 0) {
            return result;
        }
        break;
    case GP_INFO_HOMEPAGE:
        strzcpy(buffer, value, 76);
        result = gpiSendLocalInfo(connection, ov00_02218C0C, buffer);
        if (result != 0) {
            return result;
        }
        break;
    case GP_INFO_ZIPCODE:
        strzcpy(buffer, value, 11);
        result = gpiSendLocalInfo(connection, ov00_02218C18, buffer);
        if (result != 0) {
            return result;
        }
        break;
    case GP_INFO_COUNTRYCODE:
        if (strlen(value) != 2) {
            gpiSetErrorString(connection, ov00_02218DE0);
            return 2;
        }
        strzcpy(buffer, value, 3);
        result = gpiSendLocalInfo(connection, ov00_02218C24, buffer);
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
        result = gpiSendLocalInfo(connection, ov00_02218C58, buffer);
        if (result != 0) {
            return result;
        }
        break;
    }
    case GP_INFO_ICQUIN:
        strzcpy(buffer, value, 256);
        result = gpiSendLocalInfo(connection, ov00_02218C00, buffer);
        if (result != 0) {
            return result;
        }
        break;
    case GP_INFO_CPUSPEED:
        result = gpiSetInfoi(connection, GP_INFO_CPUSPEED, atol(value));
        if (result != 0) {
            return result;
        }
        break;
    case GP_INFO_MEMORY:
        result = gpiSetInfoi(connection, GP_INFO_MEMORY, atol(value));
        if (result != 0) {
            return result;
        }
        break;
    case GP_INFO_VIDEOCARD1STRING:
        strzcpy(buffer, value, 256);
        result = gpiSendLocalInfo(connection, ov00_02218DF8, buffer);
        if (result != 0) {
            return result;
        }
        break;
    case GP_INFO_VIDEOCARD1RAM:
        result = gpiSetInfoi(connection, GP_INFO_VIDEOCARD1RAM, atol(value));
        if (result != 0) {
            return result;
        }
        break;
    case GP_INFO_VIDEOCARD2STRING:
        strzcpy(buffer, value, 256);
        result = gpiSendLocalInfo(connection, ov00_02218E0C, buffer);
        if (result != 0) {
            return result;
        }
        break;
    case GP_INFO_VIDEOCARD2RAM:
        result = gpiSetInfoi(connection, GP_INFO_VIDEOCARD2RAM, atol(value));
        if (result != 0) {
            return result;
        }
        break;
    case GP_INFO_CONNECTIONSPEED:
        result = gpiSetInfoi(connection, GP_INFO_CONNECTIONSPEED, atol(value));
        if (result != 0) {
            return result;
        }
        break;
    case GP_INFO_HASNETWORK:
        result = gpiSetInfoi(connection, GP_INFO_HASNETWORK, atol(value));
        if (result != 0) {
            return result;
        }
        break;
    case GP_INFO_OSSTRING:
        strzcpy(buffer, value, 256);
        result = gpiSendLocalInfo(connection, ov00_02218E20, buffer);
        if (result != 0) {
            return result;
        }
        break;
    case GP_INFO_AIM:
        strzcpy(buffer, value, 51);
        result = gpiSendLocalInfo(connection, ov00_02218C68, buffer);
        if (result != 0) {
            return result;
        }
        break;
    case GP_INFO_PIC:
        strzcpy(buffer, value, 256);
        result = gpiSendLocalInfo(connection, ov00_02218C70, buffer);
        if (result != 0) {
            return result;
        }
        break;
    case GP_INFO_OCCUPATION:
        strzcpy(buffer, value, 256);
        result = gpiSendLocalInfo(connection, ov00_02218C78, buffer);
        if (result != 0) {
            return result;
        }
        break;
    case GP_INFO_INDUSTRY:
        strzcpy(buffer, value, 256);
        result = gpiSendLocalInfo(connection, ov00_02218C80, buffer);
        if (result != 0) {
            return result;
        }
        break;
    case GP_INFO_INCOME:
        strzcpy(buffer, value, 256);
        result = gpiSendLocalInfo(connection, ov00_02218C88, buffer);
        if (result != 0) {
            return result;
        }
        break;
    case GP_INFO_MARRIED:
        strzcpy(buffer, value, 256);
        result = gpiSendLocalInfo(connection, ov00_02218C90, buffer);
        if (result != 0) {
            return result;
        }
        break;
    case GP_INFO_CHILDCOUNT:
        strzcpy(buffer, value, 256);
        result = gpiSendLocalInfo(connection, ov00_02218C98, buffer);
        if (result != 0) {
            return result;
        }
        break;
    case GP_INFO_INTERESTS1:
        strzcpy(buffer, value, 256);
        result = gpiSendLocalInfo(connection, ov00_02218CA0, buffer);
        if (result != 0) {
            return result;
        }
        break;
    default:
        gpiSetErrorString(connection, ov00_02218DB4);
        return 2;
    }
    return 0;
}

// Sends "\getprofile\".
int gpiSendGetInfo(GpConnection **connection, int profileId, int id) {
    GpConnection *iconnection = *connection;
    gpiAppendStringToBuffer(connection, &iconnection->outputBuffer, ov00_02218E2C);
    gpiAppendIntToBuffer(connection, &iconnection->outputBuffer, iconnection->sessKey);
    gpiAppendStringToBuffer(connection, &iconnection->outputBuffer, ov00_02218BAC);
    gpiAppendIntToBuffer(connection, &iconnection->outputBuffer, profileId);
    gpiAppendStringToBuffer(connection, &iconnection->outputBuffer, ov00_02218E44);
    gpiAppendIntToBuffer(connection, &iconnection->outputBuffer, id);
    gpiAppendStringToBuffer(connection, &iconnection->outputBuffer, ov00_02218CE8);
    return 0;
}

// Gets the info of a profile: answered from the cache when allowed (checkCache == 1), else requested.
int gpiGetInfo(GpConnection **connection, int profileId, int checkCache, int blocking, void *callback, void *param) {
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
    if (callback && useCache && gpiGetProfile(connection, profileId, &frame.profile) && frame.profile->cache) {
        GpInfoArg *arg = DWCi_GsMalloc(sizeof(GpInfoArg));
        if (!arg) {
            gpiSetErrorString(connection, ov00_02218CC0);
            return 1;
        }
        gpiInfoCacheToArg(frame.profile->cache, arg);
        arg->result = 0;
        arg->profile = profileId;
        pair.callback = callback;
        pair.param = param;
        result = gpiAddOperation(connection, GP_OPERATION_GET_INFO, 0, &frame.operation, 1, callback, param);
        if (result) {
            return result;
        }
        id = frame.operation->id;
        result = gpiAddCallback(connection, pair, arg, frame.operation, 0);
        if (result) {
            return result;
        }
        gpiRemoveOperation(connection, frame.operation);
    } else {
        result = gpiAddOperation(connection, GP_OPERATION_GET_INFO, 0, &frame.operation, blocking, callback, param);
        if (result) {
            return result;
        }
        id = frame.operation->id;
        result = gpiSendGetInfo(connection, profileId, id);
        if (result) {
            return result;
        }
    }
    if (blocking) {
        result = gpiProcess(connection, id);
        if (result) {
            return result;
        }
    }
    return 0;
}

// Caches a copy of the received profile info (with its strings).
int gpiSetInfoCache(GpConnection **connection, GpProfile *profile, GpInfo *info) {
    GpInfo *cache;
    if (!(*connection)->cacheProfileInfo) {
        return 1;
    }
    gpiFreeInfoCache(profile);
    cache = profile->cache = DWCi_GsMalloc(sizeof(GpInfo));
    if (cache) {
        *cache = *info;
        profile->cache->nick = goastrdup(info->nick);
        profile->cache->uniquenick = goastrdup(info->uniquenick);
        profile->cache->email = goastrdup(info->email);
        profile->cache->firstname = goastrdup(info->firstname);
        profile->cache->lastname = goastrdup(info->lastname);
        profile->cache->homepage = goastrdup(info->homepage);
        profile->cache->aimname = goastrdup(info->aimname);
    }
    return profile->cache != 0;
}

void gpiFreeInfoCache(GpProfile *profile) {
    if (!profile->cache) {
        return;
    }
    DWCi_GsFree(profile->cache->nick);
    profile->cache->nick = 0;
    DWCi_GsFree(profile->cache->uniquenick);
    profile->cache->uniquenick = 0;
    DWCi_GsFree(profile->cache->email);
    profile->cache->email = 0;
    DWCi_GsFree(profile->cache->firstname);
    profile->cache->firstname = 0;
    DWCi_GsFree(profile->cache->lastname);
    profile->cache->lastname = 0;
    DWCi_GsFree(profile->cache->homepage);
    profile->cache->homepage = 0;
    DWCi_GsFree(profile->cache->aimname);
    profile->cache->aimname = 0;
    DWCi_GsFree(profile->cache);
    profile->cache = 0;
}

// "gpiFailedOpCallback": tells the operation's callback about its error.
int gpiFailedOpCallback(GpConnection **connection, GpOperation *operation) {
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
            GpConnectArg *arg = DWCi_GsMalloc(sizeof(GpConnectArg));
            if (!arg) {
                gpiSetErrorString(connection, ov00_02218ED4);
                return 1;
            }
            memset(arg, 0, sizeof(GpConnectArg));
            arg->result = operation->result;
            if (iconnection->errorCode == 0x201) {
                arg->profile = iconnection->profileId;
                iconnection->profileId = 0;
            }
            {
                int result = gpiAddCallback(connection, callback, arg, operation, 0);
                if (result) {
                    return result;
                }
            }
            break;
        }
        case GP_OPERATION_NEW_PROFILE: {
            GpResultArg8 *arg = DWCi_GsMalloc(sizeof(GpResultArg8));
            if (!arg) {
                gpiSetErrorString(connection, ov00_02218ED4);
                return 1;
            }
            memset(arg, 0, sizeof(GpResultArg8));
            arg->result = operation->result;
            {
                int result = gpiAddCallback(connection, callback, arg, operation, 0);
                if (result) {
                    return result;
                }
            }
            break;
        }
        case GP_OPERATION_GET_INFO: {
            GpInfoArg *arg = DWCi_GsMalloc(sizeof(GpInfoArg));
            if (!arg) {
                gpiSetErrorString(connection, ov00_02218ED4);
                return 1;
            }
            memset(arg, 0, sizeof(GpInfoArg));
            arg->result = operation->result;
            {
                int result = gpiAddCallback(connection, callback, arg, operation, 0);
                if (result) {
                    return result;
                }
            }
            break;
        }
        case GP_OPERATION_SEARCH: {
            GpResultArg16 *arg = DWCi_GsMalloc(sizeof(GpResultArg16));
            if (!arg) {
                gpiSetErrorString(connection, ov00_02218ED4);
                return 1;
            }
            memset(arg, 0, sizeof(GpResultArg16));
            arg->result = operation->result;
            arg->unk_0C = 0;
            {
                int result = gpiAddCallback(connection, callback, arg, operation, 0);
                if (result) {
                    return result;
                }
            }
            break;
        }
        case GP_OPERATION_4: {
            int *arg = DWCi_GsMalloc(sizeof(int));
            if (!arg) {
                gpiSetErrorString(connection, ov00_02218ED4);
                return 1;
            }
            memset(arg, 0, sizeof(int));
            *arg = operation->result;
            {
                int result = gpiAddCallback(connection, callback, arg, operation, 0);
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
int gpiAddOperation(GpConnection **connection, int type, void *data, GpOperation **operation, int blocking, void *callback, void *param) {
    GpConnection *iconnection = *connection;
    GpOperation *op = DWCi_GsMalloc(sizeof(GpOperation));
    if (!op) {
        gpiSetErrorString(connection, ov00_02218ED4);
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
void gpiDestroyOperation(GpConnection **connection, GpOperation *operation) {
    GpConnection *iconnection = *connection;
    if (operation->type == GP_OPERATION_SEARCH) {
        GpSearchData *data = operation->data;
        if (--iconnection->numSearches < 0) {
            __msl_assertion_failed(ov00_02218EE8, ov00_02218E9C, ov00_02218E4C, 0xb6);
        }
        gsSocketShutdown(data->sock, 2);
        gsSocketClose(data->sock);
        DWCi_GsFree(data->outputBuffer.buffer);
        data->outputBuffer.buffer = 0;
        DWCi_GsFree(data->inputBuffer.buffer);
        data->inputBuffer.buffer = 0;
    }
    DWCi_GsFree(operation->data);
    operation->data = 0;
    DWCi_GsFree(operation);
}

// Unlinks and destroys an operation.
void gpiRemoveOperation(GpConnection **connection, GpOperation *operation) {
    GpConnection *iconnection = *connection;
    GpOperation *op = iconnection->operations, *prev = 0;
    while (op) {
        if (op == operation) {
            if (!prev) {
                iconnection->operations = op->next;
            } else {
                prev->next = operation->next;
            }
            gpiDestroyOperation(connection, operation);
            return;
        }
        prev = op;
        op = op->next;
    }
}

// Finds the operation with the given id.
int gpiFindOperationByID(GpConnection **connection, GpOperation **operation, int id) {
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
int gpiOperationsAreBlocking(GpConnection **connection) {
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
int gpiProcessOperation(GpConnection **connection, GpOperation *operation, char *input) {
    int result = 0;
    switch (operation->type) {
    case GP_OPERATION_CONNECT:
        result = gpiProcessConnect(connection, operation, input);
        break;
    case GP_OPERATION_NEW_PROFILE:
        result = gpiProcessNewProfile(connection, operation, input);
        break;
    case GP_OPERATION_GET_INFO:
        result = gpiProcessGetInfo(connection, operation, input);
        break;
    case GP_OPERATION_4:
        result = gpiProcessRegisterUniqueNick(connection, operation, input);
        break;
    default:
        gpiDebug(connection, ov00_02218F08, operation->type);
        __msl_assertion_failed(ov00_02218EE4, ov00_02218E9C, ov00_02218E74, 326);
    }
    if (result) {
        operation->result = result;
    }
    return result;
}
