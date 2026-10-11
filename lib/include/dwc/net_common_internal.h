#ifndef POKEHEARTGOLD_LIB_INCLUDE_DWC_NET_COMMON_INTERNAL_H
#define POKEHEARTGOLD_LIB_INCLUDE_DWC_NET_COMMON_INTERNAL_H

#include "dwc/auth_internal.h"

// Common part of the OVY_0 network library (lib/NitroDWC/src/net_common.c): field encoding, the last
// error, the tagged allocator, the Wi-Fi connection, access point info, the server-corrected clock,
// service locator and download start-up, string helpers and the random number generator.

// Every block from ov00_021EC2B8 starts 32 bytes after this header.
typedef struct NetAllocHeader {
    u32 magic; // "DWCM"
    u32 size;
    u8 unk_08[24];
} NetAllocHeader;

typedef struct NetAllocator {
    void (*free)(int heap, void *ptr, u32 size);
    void *(*alloc)(int heap, u32 size, int align);
} NetAllocator;

// Wi-Fi connection, 100 bytes, owned by the caller of ov00_021EC3F0.
typedef struct NetConnection {
    int handle;
    u16 state; // 1 = initialised, 2 = starting, 3 = cancelled, 4 = connected, 5 = closing, 6 = lost, 7/8 = error
    u16 flag;
    u16 priority;
    u16 one;
    u8 unk_0C;
    u8 unk_0D[3];
    u32 phase;
    u8 unk_14[32];
    u8 unk_34[32];
    u32 unk_54;
    u8 unk_58[12];
} NetConnection;

typedef struct NetConnectionState {
    int error;
    NetConnection *connection;
} NetConnectionState;

typedef struct NetStartParams {
    void *(*alloc)(int heap, u32 size);
    void (*free)(int heap, void *ptr, u32 size);
    u8 priority;
    u8 one;
    u8 unk_0A[2];
} NetStartParams;

// Access point information, 60 bytes.
typedef struct NetApInfo {
    int apType; // 0-2: the user's connection settings
    int spotType;
    u8 spotInfo[9];
    u8 unk_11;
    u8 essid[33];
    u8 bssid[6];
    u8 unk_39[3];
} NetApInfo;

typedef struct NetErrorState {
    int type;
    int code;
} NetErrorState;

// ov00_021ECD04: authentication with its own connection.
typedef struct NetAuthWork {
    NasHttp http;
    AuthParams params;
    AuthResult result;
} NetAuthWork;

typedef void (*NetDownloadCallback)(int, int, int);

// Download start-up (service locator, then the "download" server), 2688 bytes.
typedef struct NetDownloadWork {
    NetDownloadCallback callback;
    void (*end)(void);
    char user[32];
    char pass[32];
    int active;
    SvcLocResult svcloc;
    u8 threadStack[0x800];
    OSThread thread;
} NetDownloadWork;

typedef struct NetRandom {
    u64 x;
    u64 mul;
    u64 add;
} NetRandom;

int ov00_021EBE08(const u8 *src, u32 len, char *dest, u32 destSize);
u32 ov00_021EBF90(char *src, u32 len, char *dest, u32 destSize);
int ov00_021EC0FC(int *code);
int ov00_021EC11C(int *code, int *category);
void ov00_021EC210(void);
int ov00_021EC230(void);
void ov00_021EC24C(int type, int code);
NetAllocHeader *ov00_021EC264(void *p);
void *ov00_021EC26C(NetAllocHeader *p, u32 size);
u32 ov00_021EC284(void *p);
void ov00_021EC294(void *(*alloc)(int, u32, int), void (*free)(int, void *, u32));
void *ov00_021EC2A8(int heap, u32 size);
void *ov00_021EC2B8(int heap, u32 size, int align);
void ov00_021EC2EC(int heap, void *p, u32 size);
void *ov00_021EC328(int heap, void *p, u32 oldSize, u32 size);
void *ov00_021EC33C(int heap, void *src, u32 oldSize, u32 size, int align);
void *ov00_021EC3A8(u32 size);
void *ov00_021EC3BC(void *p, u32 size);
void ov00_021EC3D8(void *p);
void ov00_021EC3F0(NetConnection *connection, u16 priority, int unused, u16 sslPriority);
void ov00_021EC454(int server);
void ov00_021EC4A4(void);
void ov00_021EC558(void *a, void *b, u32 c, void *d, int e);
int ov00_021EC5B4(void);
void ov00_021EC60C(void);
int ov00_021EC724(void);
void ov00_021EC8D8(void);
int ov00_021EC938(void);
int ov00_021EC9A8(void);
int ov00_021EC9D4(void);
int ov00_021EC9E0(NetApInfo *info);
int ov00_021ECB40(void);
int ov00_021ECB94(RTCDate *date, RTCTime *time);
int ov00_021ECC6C(const char *svc, SvcLocResult *output);
int ov00_021ECCE0(void);
int ov00_021ECD04(void);
int ov00_021ECDC8(void);
void ov00_021ECEC0(void);
void *ov00_021ECECC(u32 size, int align);
void ov00_021ECEE4(void *p);
void ov00_021ECEFC(int a, unsigned result, int c);
void ov00_021ECFF4(void);
void ov00_021ED038(void *arg);
int ov00_021ED1F0(NetDownloadCallback callback, char *user, char *pass);
int ov00_021ED308(void (*end)(void));
int ov00_021ED354(int a, int b, int c);
int ov00_021ED388(int a);
int ov00_021ED3AC(void *a, int b, int c);
int ov00_021ED3F4(int a, int b, int c);
int ov00_021ED428(void);
int ov00_021ED444(int a, int b);
u32 ov00_021ED470(const char *a, const char *b, char *dest, int separator);
u32 ov00_021ED4A8(const char *a, const char *b, char *dest, int separator);
int ov00_021ED4E8(char *key, char *out, char *input, int separator);
u32 ov00_021ED5E4(u32 max);

#endif // POKEHEARTGOLD_LIB_INCLUDE_DWC_NET_COMMON_INTERNAL_H
