#ifndef POKEHEARTGOLD_LIB_INCLUDE_DWC_GP_INTERNAL_H
#define POKEHEARTGOLD_LIB_INCLUDE_DWC_GP_INTERNAL_H

// Presence / buddy connection of OVY_0 (lib/NitroDWC/src/gp_*.c): the connection state shared by the GP
// modules (asserts name it "iconnection"), its operations, profiles, buddies and peers.

// A user callback and its parameter.
typedef struct GpCallback {
    void *callback;
    void *param;
} GpCallback;

// Index of GpConnection.callbacks.
enum {
    GP_CALLBACK_ERROR,
    GP_CALLBACK_BUDDY_REQUEST,
    GP_CALLBACK_BUDDY_STATUS,
    GP_CALLBACK_BUDDY_MESSAGE,
    GP_CALLBACK_GAME_INVITE,
    GP_CALLBACK_5,
    GP_CALLBACK_COUNT,
};

// connectState
enum {
    GP_NOT_CONNECTED,
    GP_CONNECTING,
    GP_NEGOTIATING,
    GP_CONNECTED,
    GP_DISCONNECTED,
};

// Growable buffer ("outputBuffer" / "inputBuffer").
typedef struct GpBuffer {
    char *buffer;
    int size;
    int len;
    int pos;
} GpBuffer;

// GpOperation.type
enum {
    GP_OPERATION_CONNECT,
    GP_OPERATION_NEW_PROFILE,
    GP_OPERATION_GET_INFO,
    GP_OPERATION_SEARCH,
    GP_OPERATION_4,
};

// GpSearchData.type
enum {
    GP_SEARCH_PROFILE = 1,
    GP_SEARCH_IS_VALID,
    GP_SEARCH_NICKS,
    GP_SEARCH_PLAYERS,
    GP_SEARCH_CHECK,
    GP_SEARCH_NEW_USER,
    GP_SEARCH_OTHERS,
    GP_SEARCH_SUGGEST_UNIQUE,
};

// A pending request to the server.
typedef struct GpOperation {
    int type;                 // 0x00
    void *data;               // 0x04, GpSearchData for searches
    int blocking;             // 0x08
    GpCallback callback;      // 0x0C
    int state;                // 0x14, 5 = skip
    int id;                   // 0x18
    int result;               // 0x1C, error of the last processing; nonzero = done
    struct GpOperation *next; // 0x20
} GpOperation;

typedef struct GpSearchData {
    int type;              // 0x000, GP_SEARCH_*
    int sock;              // 0x004, connection to the search manager
    GpBuffer inputBuffer;  // 0x008
    GpBuffer outputBuffer; // 0x018
    char nick[31];         // 0x028
    char uniquenick[21];   // 0x047
    char email[51];        // 0x05C
    char firstname[31];    // 0x08F
    char lastname[31];     // 0x0AE
    char password[31];     // 0x0CD
    char cdkey[68];        // 0x0EC
    int icquin;            // 0x130
    int skip;              // 0x134
    int productId;         // 0x138
    int busy;              // 0x13C, set while the searches are being processed
    int done;              // 0x140, the reply has been handled
} GpSearchData;

// Status of a buddy.
typedef struct GpBuddyStatus {
    int buddyIndex;       // 0x00
    int status;           // 0x04
    char *statusString;   // 0x08
    char *locationString; // 0x0C
    u32 ip;               // 0x10
    int port;             // 0x14
} GpBuddyStatus;

// Argument of the connect callback.
typedef struct GpConnectArg {
    int result;
    int profile;
    char uniquenick[21];
} GpConnectArg;

// Profile info parsed from a "\pi\" reply.
typedef struct GpInfo {
    char *nick;
    char *uniquenick;
    char *email;
    char *firstname;
    char *lastname;
    char *homepage;
    int icquin;
    char zipcode[11];
    char countrycode[3];
    float longitude;
    float latitude;
    char place[128];
    int birthday;
    int birthmonth;
    int birthyear;
    int sex;
    int publicmask;
    char *aimname;
    int pic;
    int occupationid;
    int industryid;
    int incomeid;
    int marriedid;
    int childcount;
    int interests1;
    int ownership1;
    int conntypeid;
} GpInfo;

// Argument of the get-info callback.
typedef struct GpInfoArg {
    int result;
    int profile;
    char nick[31];
    char uniquenick[21];
    char email[51];
    char firstname[31];
    char lastname[31];
    char homepage[76];
    int icquin;
    char zipcode[11];
    char countrycode[3];
    float longitude;
    float latitude;
    char place[128];
    int birthday;
    int birthmonth;
    int birthyear;
    int sex;
    int publicmask;
    char aimname[51];
    int pic;
    int occupationid;
    int industryid;
    int incomeid;
    int marriedid;
    int childcount;
    int interests1;
    int ownership1;
    int conntypeid;
} GpInfoArg;

// An entry of the profile table.
typedef struct GpProfile {
    int profileId;              // 0x00
    int userId;                 // 0x04
    GpBuddyStatus *buddyStatus; // 0x08, only for buddies
    GpInfo *cache;              // 0x0C, cached profile info
    char *authSig;              // 0x10, signature of a pending buddy request
    int requestCount;           // 0x14
    char *peerSig;              // 0x18
} GpProfile;

// GpPeer.state
enum {
    GP_PEER_NOT_CONNECTED = 100,
    GP_PEER_GETTING_SIG,
    GP_PEER_GOT_SIG,
    GP_PEER_CONNECTING,
    GP_PEER_WAITING,
    GP_PEER_CONNECTED,
    GP_PEER_DISCONNECTED,
};

// A direct connection to another profile.
typedef struct GpPeer {
    int state;             // 0x00
    int initiated;         // 0x04, we connected to it (else it connected to us)
    int sock;              // 0x08
    int profile;           // 0x0C
    int timeout;           // 0x10, tick after which the idle peer is dropped
    int nackCount;         // 0x14
    GpBuffer inputBuffer;  // 0x18
    GpBuffer outputBuffer; // 0x28
    void *messages;        // 0x38, DArray of GpPeerMessage still to send
    struct GpPeer *next;   // 0x3C
} GpPeer;

// A message queued for a peer.
typedef struct GpPeerMessage {
    GpBuffer buffer;
    int type;
    int start; // offset of the message text in buffer
} GpPeerMessage;

typedef struct GpCallbackData GpCallbackData;

typedef struct GpConnection {
    char errorString[0x100]; // 0x000
    int cacheProfileInfo;    // 0x100, keep received profile info after use
    int unk_104;
    int simulation;                          // 0x108, no network: the API calls succeed without doing anything
    int firewall;                            // 0x10C, no peer listening socket
    char nick[31];                           // 0x110
    char uniquenick[21];                     // 0x12F
    char email[51];                          // 0x144
    char password[31];                       // 0x177
    int sessKey;                             // 0x198, sent as "\sesskey\"
    int userId;                              // 0x19C
    int profileId;                           // 0x1A0
    GpCallback callbacks[GP_CALLBACK_COUNT]; // 0x1A4
    int sock;                                // 0x1D4, connection to the server
    int connectState;                        // 0x1D8
    GpBuffer inputBuffer;                    // 0x1DC
    char *lineBuffer;                        // 0x1EC, the server message being handled
    int lineBufferSize;                      // 0x1F0
    GpBuffer outputBuffer;                   // 0x1F4
    int peerSocket;                          // 0x204, listening socket for peers
    int peerPort;                            // 0x208
    int nextOperationId;                     // 0x20C
    int numSearches;                         // 0x210
    int lastStatus;                          // 0x214, last status sent ("\status\")
    char lastStatusString[0x100];            // 0x218
    char lastLocationString[0x100];          // 0x318
    int errorCode;                           // 0x418
    int fatalError;                          // 0x41C
    int unk_420;
    GpOperation *operations;       // 0x424
    void *profileTable;            // 0x428, hash table of GpProfile
    int numProfiles;               // 0x42C
    int numBuddies;                // 0x430
    GpPeer *peers;                 // 0x434
    GpCallbackData *callbacksHead; // 0x438, queued callbacks
    GpCallbackData *callbacksTail; // 0x43C
    GpBuffer updateproBuffer;      // 0x440, pending "\updatepro\" fields
    GpBuffer updateuiBuffer;       // 0x450, pending "\updateui\" fields
    void *unk_460;
    int unk_464;
    int unk_468;
    int productId;        // 0x46C
    int namespaceId;      // 0x470
    char loginTicket[25]; // 0x474, "\lt\" of the login reply
} GpConnection;           // 0x490

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
void ov00_0220A1C0(GpConnection **connection, void *peer, int type, const char *headers, char *buffer, int len);
int ov00_0220A244(GpConnection **connection, GpOperation *operation, char *message);
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
int ov00_02201FE0(GpConnection **connection, GpBuffer *outputBuffer, const char *string, int stringLen);
int ov00_022020EC(GpConnection **connection, GpBuffer *outputBuffer, const char *string);
int ov00_0220211C(GpConnection **connection, GpBuffer *outputBuffer, int n);
int ov00_02202154(GpConnection **connection, int sock, void *data, int len, int *connClosed, int *bytesSent, const char *tag);
int ov00_02202244(GpConnection **connection, GpPeer *peer, char c, ...);
int ov00_02202324(GpConnection **connection, GpPeer *peer, char *string, int stringLen);
int ov00_02202454(GpConnection **connection, int sock, GpBuffer *inputBuffer, int *bytesRead, int *connClosed, const char *tag);
int ov00_0220267C(GpConnection **connection, int sock, GpBuffer *outputBuffer, int *connClosed, int clear, const char *tag);
int ov00_022027FC(GpConnection **connection, GpBuffer *inputBuffer, char **message, int *type, int *len);
int ov00_02202938(GpConnection **connection, GpBuffer *inputBuffer);
void ov00_02202B28(GpConnection **connection, GpCallbackData *data);

// gp_main.c
int ov00_02200880(GpConnection **connection, int profile, const char *message);
int ov00_022008F4(GpConnection **connection, int arg1, int arg2);
void ov00_02200A3C(GpConnection **connection);
int ov00_02200A7C(GpConnection **connection, GpProfile *profile, void *data);
int ov00_02200A98(GpConnection **connection);
int ov00_02200BC0(GpConnection **connection);
int ov00_02200ED8(GpConnection **connection, int blocking);
int ov00_0220108C(GpConnection **connection, GpProfile *profile);
int ov00_02201134(GpConnection **connection, char *input);
int ov00_02201AE8(GpConnection **connection, int profile, int type, const char *message);
int ov00_02201BC4(GpConnection **connection, int profileId, int type, const char *message);
int ov00_02201CAC(GpConnection **connection, int profileId);
int ov00_02201D78(GpConnection **connection, GpProfile *profile, void *index);
int ov00_02201D9C(GpConnection **connection, int profileId);
// gp_connect.c
int ov00_02202D20(GpConnection **connection, int operationId);
void ov00_02202E08(char *out, int count);
int ov00_02202E70(GpConnection **connection, GpOperation *operation);
int ov00_022031E4(GpConnection **connection, char *nick, char *uniquenick, char *email, char *password, char *authtoken, char *partnerChallenge, char *cdkey, int firewall, int newUser, int blocking, void *callback, void *param);
int ov00_02203A58(GpConnection **connection, GpOperation *operation, char *input);
int ov00_0220411C(GpConnection **connection);
int ov00_022041C0(GpConnection **connection, GpProfile *profile, void *data);
void ov00_02204284(GpConnection **connection, int tearDown);
int ov00_0220444C(int day, int month, int year);
int ov00_022045AC(GpConnection **connection, int date, int *day, int *month, int *year);
void ov00_02204654(GpInfo *info, GpInfoArg *arg);
int ov00_0220484C(GpConnection **connection, GpOperation *operation, char *input);
// gp_operation.c
int ov00_02205064(GpConnection **connection, GpBuffer *outputBuffer);
int ov00_02205128(GpConnection **connection, const char *key, const char *value);
int ov00_02205168(GpConnection **connection, const char *key, const char *value);
int ov00_022051A8(GpConnection **connection, int info, int value);
int ov00_02205678(GpConnection **connection, int info, const char *value);
int ov00_02205E50(GpConnection **connection, int profileId, int id);
int ov00_02205EE8(GpConnection **connection, int profileId, int checkCache, int blocking, void *callback, void *param);
int ov00_0220608C(GpConnection **connection, GpProfile *profile, GpInfo *info);
void ov00_02206164(GpProfile *profile);
int ov00_02206230(GpConnection **connection, GpOperation *operation);
int ov00_0220657C(GpConnection **connection, int type, void *data, GpOperation **operation, int blocking, void *callback, void *param);
void ov00_02206634(GpConnection **connection, GpOperation *operation);
void ov00_022066D0(GpConnection **connection, GpOperation *operation);
int ov00_02206720(GpConnection **connection, GpOperation **operation, int id);
int ov00_0220676C(GpConnection **connection);
int ov00_022067A8(GpConnection **connection, GpOperation *operation, char *input);
// gp_peer.c (ov00_0220792C and ov00_022079A8 are declared by gp_utility.c)
int ov00_0220684C(GpConnection **connection, GpPeer *peer);
int ov00_02206BA8(GpConnection **connection, GpPeer *peer);
int ov00_02206E04(GpConnection **connection, GpPeer *peer);
int ov00_02206ED8(GpConnection **connection, GpPeer *peer);
int ov00_02207158(GpConnection **connection, GpPeer *peer);
void ov00_022071F4(GpConnection **connection, GpPeer *peer);
void ov00_02207254(GpConnection **connection, GpPeer *peer);
void ov00_02207370(int sock);
int ov00_022073E8(GpConnection **connection);
GpPeer *ov00_022074D8(GpConnection **connection, int profile);
void ov00_02207510(void *message);
GpPeer *ov00_0220752C(GpConnection **connection, int profile, int initiated);
int ov00_022075AC(GpConnection **connection, GpPeer *peer);
int ov00_02207610(GpConnection **connection, GpPeer *peer);
int ov00_0220779C(GpConnection **connection, GpPeer *peer, int type, const char *message);
int ov00_02207A94(GpProfile *profile, int numBuckets);
int ov00_02207AA8(GpProfile *a, GpProfile *b);
void ov00_02207AB8(GpProfile *profile);
int ov00_02207B34(GpConnection **connection);
int ov00_02207B8C(GpConnection **connection, GpOperation *operation, char *input);
// gp_search.c
GpProfile *ov00_02207CEC(GpConnection **connection, int profileId);
int ov00_02207DD0(GpConnection **connection, int profileId, GpProfile **profile);
void ov00_02207E0C(GpConnection **connection, int profileId);
void ov00_02207E40(GpConnection **connection, GpProfile *profile);
int ov00_02207EB4(GpConnection **connection, char *nick, char *email, GpProfile **profile);
int ov00_02207F1C(GpConnection **connection, int (*func)(GpConnection **, GpProfile *, void *), void *data);
GpProfile *ov00_02207F88(GpConnection **connection, int index);
int ov00_02207FB8(GpProfile *profile);
int ov00_02207FF0(GpConnection **connection, GpOperation *operation);
int ov00_022081F0(GpConnection **connection, GpSearchData **data, int type);
int ov00_022082A0(GpConnection **connection, GpSearchData *data, int blocking, void *callback, void *param);
int ov00_02208330(GpConnection **connection, char *nick, char *uniquenick, char *email, char *firstname, char *lastname, int icquin, int skip, int blocking, void *callback, void *param);
int ov00_0220851C(GpConnection **connection, GpOperation *operation);

#endif // POKEHEARTGOLD_LIB_INCLUDE_DWC_GP_INTERNAL_H
