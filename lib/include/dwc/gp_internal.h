#ifndef POKEHEARTGOLD_LIB_INCLUDE_DWC_GP_INTERNAL_H
#define POKEHEARTGOLD_LIB_INCLUDE_DWC_GP_INTERNAL_H

// Presence / buddy connection of OVY_0 (lib/NitroDWC/src/gp_utility.c): the connection state shared by
// the GP modules (asserts name it "iconnection"), its pending searches, and the string helpers.

typedef struct GpSearchOwner {
    u8 unk00[0x13C];
    int busy;    // 0x13C, set while its searches are being processed
    int pending; // 0x140, completion deferred until processing ends
} GpSearchOwner;

typedef struct GpSearch {
    int state;            // 0x00, 3 = ready to process
    GpSearchOwner *owner; // 0x04
    u8 unk08[0xC];
    int unk14;             // 0x14, 5 = skip
    int unk18;             // 0x18
    int result;            // 0x1C, error of the last processing
    struct GpSearch *next; // 0x20
} GpSearch;

typedef struct GpConnection {
    char errorString[0x100]; // 0x000
    u8 unk100[0x110];
    int numSearches; // 0x210
    u8 unk214[0x204];
    int errorCode; // 0x418
    u8 unk41C[0x8];
    GpSearch *searches; // 0x424
} GpConnection;

// Two ints passed by value to ov00_02202A84.
typedef struct GpPair {
    int a;
    int b;
} GpPair;

// gp_utility.c
int ov00_02209FCC(GpConnection **connection);
int ov00_0220A11C(GpConnection **connection, int *values, void *operation, int result, const char *message);
void ov00_0220A1C0(GpConnection **connection, void *operation, int unused, const char *message);
int ov00_0220A244(GpConnection **connection, char *operation, char *message);
void ov00_0220A348(char *dest, const char *src, int len);
void ov00_0220A3BC(GpConnection **connection, const char *format, ...);
int ov00_0220A3C8(GpConnection **connection, const char *message, int callback);
int ov00_0220A4A0(const char *command, const char *key, char *value, int len);
int ov00_0220A5A8(GpConnection **connection, int socket, int *result);
int ov00_0220A69C(GpConnection **connection, const char *buffer, int *index, char *key, char *value);
// persist_data.c
void ov00_0220A86C(GpConnection **connection, int code, const char *message);
void ov00_0220A890(GpConnection **connection, const char *message);

#endif // POKEHEARTGOLD_LIB_INCLUDE_DWC_GP_INTERNAL_H
