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

// A user callback and its parameter.
typedef struct GpCallback {
    void *callback;
    void *param;
} GpCallback;

typedef struct GpCallbackData GpCallbackData;

typedef struct GpConnection {
    char errorString[0x100]; // 0x000
    u8 unk_100[0xA4];
    GpCallback errorCallback; // 0x1A4
    u8 unk_1AC[0x64];
    int numSearches; // 0x210
    u8 unk_214[0x204];
    int errorCode;  // 0x418
    int fatalError; // 0x41C
    u8 unk_420[0x4];
    GpSearch *searches; // 0x424
    u8 unk_428[0x10];
    GpCallbackData *callbacksHead; // 0x438, queued callbacks
    GpCallbackData *callbacksTail; // 0x43C
} GpConnection;

// Growable buffer ("outputBuffer" / "inputBuffer").
typedef struct GpBuffer {
    char *buffer;
    int size;
    int len;
    int pos;
} GpBuffer;

typedef struct GpPeer {
    u8 unk_00[0x8];
    int sock; // 0x08
    u8 unk_0C[0x1C];
    GpBuffer outputBuffer; // 0x28
    void *messages;        // 0x38, DArray of queued messages
} GpPeer;

typedef struct GpOperation {
    u8 unk_00[0x18];
    int id; // 0x18
} GpOperation;

// A queued callback (ov00_02202A84); type tells which parts of arg it owns.
struct GpCallbackData {
    void *callback;
    void *param;
    void *arg;
    int type;
    int operationId;
    struct GpCallbackData *next;
};

// Argument of the error callback.
typedef struct GpErrorArg {
    int result;
    int errorCode;
    char *errorString;
    int fatal;
} GpErrorArg;

// Callback argument of type 3: two string lists.
typedef struct GpListArg {
    u32 unk_00[14];
    int count;
    void **a;
    void **b;
} GpListArg;

// The GpCallback pair as gp_utility.c passes it by value to ov00_02202A84.
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

// gp_buffer.c (ov00_02202424, ov00_02202990 and ov00_02202A84 are declared by their users)
int ov00_02201F48(GpConnection **connection, GpBuffer *outputBuffer, int c);
int ov00_02201FE0(GpConnection **connection, GpBuffer *outputBuffer, char *string, int stringLen);
int ov00_022020EC(GpConnection **connection, GpBuffer *outputBuffer, char *string);
int ov00_0220211C(GpConnection **connection, GpBuffer *outputBuffer, int n);
int ov00_02202154(GpConnection **connection, int sock, void *data, int len, int *connClosed, int *bytesSent, const char *tag);
int ov00_02202244(GpConnection **connection, GpPeer *peer, char c, ...);
int ov00_02202324(GpConnection **connection, GpPeer *peer, char *string, int stringLen);
int ov00_02202454(GpConnection **connection, int sock, GpBuffer *inputBuffer, int *bytesRead, int *connClosed, const char *tag);
int ov00_0220267C(GpConnection **connection, int sock, GpBuffer *outputBuffer, int *connClosed, int clear, const char *tag);
int ov00_022027FC(GpConnection **connection, GpBuffer *inputBuffer, char **message, int *type, int *len);
int ov00_02202938(GpConnection **connection, GpBuffer *inputBuffer);
void ov00_02202B28(GpConnection **connection, GpCallbackData *data);

#endif // POKEHEARTGOLD_LIB_INCLUDE_DWC_GP_INTERNAL_H
