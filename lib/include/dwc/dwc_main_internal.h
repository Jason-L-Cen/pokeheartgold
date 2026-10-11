#ifndef POKEHEARTGOLD_LIB_INCLUDE_DWC_DWC_MAIN_INTERNAL_H
#define POKEHEARTGOLD_LIB_INCLUDE_DWC_DWC_MAIN_INTERNAL_H

// Main control block of the OVY_0 network library (lib/NitroDWC/src/dwc_main.c, dwc_login.c,
// friend_manager.c): the GT2 socket, the GameSpy presence connection, the user callbacks and the
// login, friend list, matchmaking and data transport states. The caller hands the memory to
// ov00_021ED730, which keeps it in ov00_0221A784.

#include "dwc/auth_internal.h"
#include "dwc/dwc_account_internal.h"
#include "dwc/friend_internal.h"
#include "dwc/match_internal.h"
#include "dwc/transport_internal.h"

// Reply to the GameSpy connect request (ov00_021FFE20), 0x20 bytes.
typedef struct LoginConnectResult {
    int result;
    int profileId; // 0x04
    u32 unk_08[6];
} LoginConnectResult;

// Log-in to the GameSpy presence server, 0x280 bytes. Authenticates with the Nintendo server first
// (ov00_021E7B70) unless the user data already holds an authenticated login id.
typedef struct LoginState {
    GPConnection *connection;
    int state;                                               // 0x04, 1 authenticating, 2 connecting, 3/4 checking the profile, 5 logged in
    int productId;                                           // 0x08
    u32 gameCode;                                            // 0x0C
    u16 *playerName;                                         // 0x10
    void (*callback)(int error, int profileId, void *param); // 0x14
    void *param;                                             // 0x18
    DwcUserData *userData;                                   // 0x1C
    u32 unk_20;
    NasHttp *auth;       // 0x24, work of the authentication request
    u64 authTick;        // 0x28
    int pending;         // 0x30, connect request sent
    u64 connectTick;     // 0x34
    DwcAccData loginId;  // 0x3C
    char token[256];     // 0x48, from the authentication server
    char challenge[256]; // 0x148
    char userName[9];    // 0x248, made from the login id
    char gsbrcd[12];     // 0x251
    u8 unk_25D[3];
    LoginConnectResult response; // 0x260
} LoginState;

// ov00_0221A90C
typedef struct LoginGlobal {
    void *authParam;
    LoginState *state;                                                           // 0x04
    void (*authCallback)(const char *token, const char *challenge, void *param); // 0x08, called after the authentication
} LoginGlobal;

// The GameSpy buddy message / status notification arguments.
typedef struct GpBuddyMessage {
    int profileId;
    u32 date;      // 0x04
    char *message; // 0x08
} GpBuddyMessage;

typedef struct GpBuddyStatusArg {
    int profileId;
    u32 date;  // 0x04
    int index; // 0x08, buddy index
} GpBuddyStatusArg;

// Index and aid of a GT2 connection (ov00_0220D2E0).
typedef struct DwcConnectionInfo {
    u8 index; // into ov00_0221A78C
    u8 aid;   // 0x01
} DwcConnectionInfo;

typedef void (*DwcResultCallback)(int error, int value, void *param);
typedef void (*DwcMatchedCallback)(int error, BOOL cancelled, BOOL self, BOOL isServer, int index, void *param);
typedef void (*DwcClosedCallback)(int error, BOOL isLocal, BOOL isServer, u32 aid, int index, void *param);

// ov00_0221A784, 0xE0C bytes
typedef struct DwcControl {
    void *socket;             // GT2 socket
    void *socketCallbacks[4]; // 0x04, see ov00_021ED730
    u32 bufferSize[2];        // 0x14, GT2 socket buffer sizes, 0x2000 by default
    GPConnection connection;  // 0x1C, GameSpy presence connection
    DwcUserData *userData;    // 0x20
    int state;                // 0x24, see ov00_021EDB1C
    int prevState;            // 0x28
    u8 aid;                   // 0x2C, own aid
    u8 closing;               // 0x2D, set while ov00_021EE278 closes the connections
    u16 playerName[26];       // 0x2E
    u8 unk_62[2];
    int profileId;                          // 0x64, own GameSpy profile id
    const char *gameName;                   // 0x68, ov00_0221B430
    const char *secretKey;                  // 0x6C, ov00_0221B530
    DwcResultCallback loginCallback;        // 0x70
    void *loginParam;                       // 0x74
    DwcResultCallback updateCallback;       // 0x78, friend list update
    void *updateParam;                      // 0x7C
    DwcResultCallback matchCallback;        // 0x80, match with anybody
    void *matchParam;                       // 0x84
    DwcMatchedCallback friendMatchCallback; // 0x88, match with friends / server-client match
    void *friendMatchParam;                 // 0x8C
    DwcClosedCallback closedCallback;       // 0x90
    void *closedParam;                      // 0x94
    LoginState login;                       // 0x98
    FriendWork friends;                     // 0x318
    MatchWork match;                        // 0x374
    Transport transport;                    // 0x7F8
} DwcControl;

extern DwcControl *ov00_0221A784; // the second word holds the last GT2 socket error (ov00_021EF1B4)
extern void *ov00_0221A78C[32];   // GT2 connections, by DwcConnectionInfo.index
extern u32 ov00_0221A80C[32][2];
extern u8 ov00_0221B430[256]; // game name
extern u8 ov00_0221B530[256]; // secret key
extern LoginGlobal ov00_0221A90C;

// dwc_main.c
u32 ov00_021ED704(const u16 *s);
void ov00_021ED730(DwcControl *control, DwcUserData *userData, int productId, const char *gameName, const char *secretKey, int sendBufferSize, int recvBufferSize, DwcAccData *friends, int friendCount);
void ov00_021ED9B4(void);
void ov00_021EDB1C(void);
int ov00_021EDD6C(const u16 *playerName, int unused, DwcResultCallback callback, void *param);
int ov00_021EDE74(int unused, DwcResultCallback callback, void *param, void *statusCallback, void *statusParam, void *deleteCallback, void *deleteParam);
int ov00_021EDF1C(int count, const char *filter, DwcResultCallback callback, void *param, void *evalCallback, void *evalParam);
int ov00_021EDFBC(int count, DwcMatchedCallback callback, void *param, void *evalCallback, void *evalParam);
int ov00_021EE054(int index, DwcMatchedCallback callback, void *param, void *newPeerCallback, void *newPeerParam);
int ov00_021EE24C(DwcClosedCallback callback, void *param);
int ov00_021EE278(void);
int ov00_021EE31C(int aid);
int ov00_021EE384(u32 *aidMask);
int ov00_021EE448(void);
int ov00_021EE490(void);
int ov00_021EE4AC(u8 **aids);
u32 ov00_021EE4FC(void);
int ov00_021EE530(int aid);
int ov00_021EE568(void);
int ov00_021EE584(void);
void *ov00_021EE64C(int aid);
int ov00_021EE6B0(void *connection);
int ov00_021EE6C0(void *connection);
int ov00_021EE6D0(void);
void ov00_021EE6FC(void);
void **ov00_021EE72C(int index);
void **ov00_021EE73C(int profileId, int count);
u32 *ov00_021EE7A4(int index);
BOOL ov00_021EE7B4(int aid);
void ov00_021EE7FC(void);
void ov00_021EE800(int state);
int ov00_021EE820(int aid);
u32 ov00_021EE880(const u8 *aids, int count);
int ov00_021EE8B4(int error);
int ov00_021EE9D0(int error);
void ov00_021EEA74(int error, int profileId, void *param);
void ov00_021EEAD4(int error, BOOL changed, void *param);
void ov00_021EEB1C(int error, BOOL cancelled, BOOL self, BOOL isServer, int index, void *param);
void ov00_021EEC94(GPConnection *connection, void *arg, void *param);
void ov00_021EECCC(GPConnection *connection, void *arg, void *param);
void ov00_021EEE34(void *connection, void *data, int size, int reliable);
void ov00_021EEE40(void *connection, int reason);

// dwc_login.c
void ov00_021EF1A8(void *connection, void *data, int size, int unused);
void ov00_021EF1B4(void *socket);
void ov00_021EF1F0(LoginState *login, DwcUserData *userData, GPConnection *connection, int productId, u32 gameCode, u16 *playerName, DwcResultCallback callback, void *param);
void ov00_021EF274(void);
void ov00_021EF2AC(void);
DwcUserData *ov00_021EF3A4(void);
void ov00_021EF3C0(int kind, int code);
void ov00_021EF410(void);
void ov00_021EF46C(void);
int ov00_021EF494(int error);
void ov00_021EF508(GPConnection *connection, void *arg, void *param);
void ov00_021EF640(const char *token, const char *challenge, void *param);
void ov00_021EF658(const char *token, const char *challenge, GPCallback callback, int state);
void ov00_021EF6F4(void (*callback)(const char *token, const char *challenge, void *param), void *param);
void ov00_021EF8C0(void);
void ov00_021EFAC8(GPConnection *connection, void *arg, void *param);

// friend_manager.c
BOOL ov00_021EFC6C(void);
int ov00_021EFC98(const DwcAccData *friendData, char *locationString);
u8 ov00_021EFCB0(const DwcAccData *friendData, u8 *a, u8 *b, char *locationString);
int ov00_021EFDCC(const DwcAccData *friendData, char *data, int *size);
int ov00_021EFDE8(const DwcAccData *friendData, u8 *a, u8 *b, char *data, int *size);
BOOL ov00_021EFE78(const u8 *data, int size);
BOOL ov00_021EFF04(void *callback, void *param);
int ov00_021EFF30(const u8 *src, int len, char *dest, int destSize);
int ov00_021EFF3C(char *src, int len, char *dest, int destSize);
void ov00_021EFF48(FriendWork *work, GPConnection *connection, u16 *playerName, DwcAccData *list, int count);
void ov00_021F0028(void);
void ov00_021F018C(const char *token, const char *challenge, void *updateCallback, void *updateParam, void *statusCallback, void *statusParam, void *deleteCallback, void *deleteParam);
void ov00_021F0238(int kind, int code);
void ov00_021F0290(GPConnection *connection, void *arg, void *param);
BOOL ov00_021F02DC(GPConnection *connection, void *arg, void *param);
void ov00_021F0338(GPConnection *connection, void *arg, void *param);
int ov00_021F03BC(int index);
int ov00_021F0404(int profileId);
void ov00_021F0468(void);
int ov00_021F04A0(int status, const char *statusString, const char *locationString);
void ov00_021F0550(int index);
void ov00_021F05D8(void);
int ov00_021F05EC(void);
void ov00_021F0678(void);
void ov00_021F06A8(DwcAccData *list, int count);
void ov00_021F0914(void);
void ov00_021F094C(DwcAccData *list, int index, int other);

#endif // POKEHEARTGOLD_LIB_INCLUDE_DWC_DWC_MAIN_INTERNAL_H
