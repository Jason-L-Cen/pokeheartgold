#ifndef POKEHEARTGOLD_LIB_INCLUDE_DWC_GT_INTERNAL_H
#define POKEHEARTGOLD_LIB_INCLUDE_DWC_GT_INTERNAL_H

// Reliable UDP sockets of OVY_0 (lib/NitroDWC/src/gt_socket.c): sockets, connections, their message
// buffers and the callbacks into the user code.

typedef struct GTSocket GTSocket;

// Why a connection was closed (passed to the closed callback).
typedef enum GTCloseReason {
    GT_CLOSE_LOCAL,  // closed by this side
    GT_CLOSE_REMOTE, // closed by the peer
    GT_CLOSE_ERROR,
    GT_CLOSE_ERROR_3,
    GT_CLOSE_FATAL_ERROR,
} GTCloseReason;

typedef struct GTConn GTConn;

typedef struct GTCallbacks {
    void *connected;
    void *received;
    void *closed;
    void *ping;
} GTCallbacks;

typedef void (*GTConnectedCB)(GTConn *, int, u8 *, int);
typedef void (*GTReceivedCB)(GTConn *, u8 *, int, int);
typedef void (*GTClosedCB)(GTConn *, int);
typedef void (*GTPingCB)(GTConn *, int);
typedef void (*GTFilterCB)(GTConn *, int, u8 *, int, int);
typedef void (*GTSockErrCB)(GTSocket *, GTConn *, u32, u16, int, u8 *, int);
typedef int (*GTUnrecCB)(GTSocket *, u32, u16, u8 *, int);

struct GTSocket {
    int handle;                              // 0x00
    u32 ip;                                  // 0x04
    u16 port;                                // 0x08
    void *connections;                       // 0x0c
    void *closedConnections;                 // 0x10
    int close;                               // 0x14
    int error;                               // 0x18
    int callbackLevel;                       // 0x1c
    int (*connectAttemptCB)();               // 0x20, see ov00_0220C200
    void (*socketErrorCB)(GTSocket *socket); // 0x24, see ov00_0220C164
    GTSockErrCB unk28;                       // 0x28
    GTSockErrCB unk2C;                       // 0x2c
    GTUnrecCB unrecognizedCB;                // 0x30
};

typedef struct GTBuffer {
    u8 *buffer;
    int size;
    int len;
} GTBuffer;

typedef struct GTOutMsg {
    int start;  // 0x0
    int len;    // 0x4
    u16 serial; // 0x8
    u16 pad;
    u32 lastSend; // 0xc
} GTOutMsg;

typedef struct GTInMsg {
    int start;  // 0x0
    int len;    // 0x4
    int type;   // 0x8
    u16 serial; // 0xc
} GTInMsg;

struct GTConn {
    u32 ip;                         // 0x00
    u16 port;                       // 0x04
    GTSocket *socket;               // 0x08
    int state;                      // 0x0c
    int initiated;                  // 0x10
    int freeAtAcceptReject;         // 0x14
    int connectionResult;           // 0x18
    u32 startTime;                  // 0x1c
    u32 timeout;                    // 0x20
    int callbackLevel;              // 0x24
    GTCallbacks callbacks;          // 0x28
    u8 *initialMessage;             // 0x38
    int initialMessageLen;          // 0x3c
    void *data;                     // 0x40
    GTBuffer incomingBuffer;        // 0x44
    GTBuffer outgoingBuffer;        // 0x50
    void *incomingBufferedMessages; // 0x5c
    void *outgoingBufferedMessages; // 0x60
    u16 serialNumber;               // 0x64
    u16 expectedSerialNumber;       // 0x66
    u8 response[32];                // 0x68
    u32 lastSend;                   // 0x88
    u32 challengeTime;              // 0x8c
    int pendingAck;                 // 0x90
    u32 pendingAckTime;             // 0x94
    void *sendFilters;              // 0x98
    void *receiveFilters;           // 0x9c
};

typedef struct GTFilter {
    GTFilterCB callback;
} GTFilter;

typedef struct GTSockAddr {
    u8 len;
    u8 family;
    u16 port;
    u32 addr;
} GTSockAddr;

// gt_socket.c
int ov00_0220C31C(GTConn *conn, int result, u8 *msg, int len);
int ov00_0220C404(GTConn *conn, u8 *msg, int len, int reliable);
int ov00_0220C4E8(GTConn *conn, int reason);
int ov00_0220C5AC(GTConn *conn, int latency);
int ov00_0220C670(GTConn *conn, int index, u8 *msg, int len, int reliable);
int ov00_0220C768(GTConn *conn, int index, u8 *msg, int len, int reliable);
int ov00_0220C860(GTSocket *socket, GTConn *conn, u32 ip, u16 port, int reset, u8 *msg, int len, int fatal);
int ov00_0220C984(GTSocket *socket, u32 ip, u16 port, u8 *msg, int len, int *handled);
int ov00_0220CA7C(GTSocket *socket, GTConn **conn, u32 ip, u16 port);
int ov00_0220CAAC(GTSocket *socket, GTConn **conn, u32 ip, u16 port);
int ov00_0220CADC(GTConn *conn, u8 *msg, int len, GTCallbacks *callbacks);
int ov00_0220CB84(GTConn *conn, GTCallbacks *callbacks);
void ov00_0220CBE0(GTConn *conn, u8 *msg, int len);
int ov00_0220CC38(GTConn *conn, u8 *msg, int len);
int ov00_0220CC80(GTConn *conn, u32 now);
int ov00_0220CD24(GTConn *conn, u32 now);
int ov00_0220CD90(GTConn *conn, u32 now);
int ov00_0220CDCC(GTConn *conn, u32 now);
int ov00_0220CE00(GTConn *conn, u32 now);
void ov00_0220CE68(GTConn *conn, int hard);
void ov00_0220CEB8(GTConn *conn);
void ov00_0220CF14(GTConn *conn);
int ov00_0220CF98(void *a, void *b, void *c, void *d, void *e);
void ov00_0220CFAC(GTSocket *socket);
void ov00_0220CFC4(GTSocket *socket);
int ov00_0220CFF4(void *a, void *b, void *c, void *d);
int ov00_0220D000(GTConn *conn, GTCallbacks *callbacks);
void ov00_0220D00C(GTConn *conn, u8 *msg, int len);
int ov00_0220D018(GTSocket *socket, GTConn **connp, const char *address, u8 *msg, int len, u32 timeout, GTCallbacks *callbacks, int blocking);
void ov00_0220D194(GTConn *conn, u8 *msg, int len, int reliable);
void ov00_0220D218(GTConn *conn);
void ov00_0220D228(GTConn **conn);
void ov00_0220D238(GTSocket *socket);
u16 ov00_0220D27C(GTSocket *socket);
int ov00_0220D284(GTConn *conn);
u32 ov00_0220D294(GTConn *conn);
void ov00_0220D29C(GTSocket *socket, GTUnrecCB cb);
void ov00_0220D2A4(GTConn *conn, void *data);
void *ov00_0220D2E0(GTConn *conn);
u16 ov00_0220D318(u8 *buf, int off);
void ov00_0220D338(u8 *buf, int off, u16 val);
s16 ov00_0220D34C(u16 a, u16 b);
int ov00_0220D35C(GTConn *conn, int result, GTCloseReason reason);
int ov00_0220D3F0(GTConn *conn);
int ov00_0220D404(GTConn *conn);
int ov00_0220D430(GTConn *conn, u16 ack);
int ov00_0220D52C(GTConn *conn, u8 *msg, int len);
int ov00_0220D5BC(GTConn *conn, u8 *msg, int len);
int ov00_0220D668(GTConn *conn, u8 *msg, int len);
int ov00_0220D700(GTConn *conn, u8 *msg, int len);
int ov00_0220D7DC(GTConn *conn, u8 *msg, int len);
int ov00_0220D8EC(GTConn *conn);
int ov00_0220D938(GTConn *conn, u8 *msg, int len);
int ov00_0220D9A4(GTConn *conn);
int ov00_0220D9E8(GTConn *conn, int type, u8 *msg, int len);
s16 ov00_0220DAE8(GTInMsg *a, GTInMsg *b);
int ov00_0220DAFC(GTConn *conn, int type, u16 serial, u8 *msg, int len, int *overflow);
void ov00_0220DCB0(GTConn *conn, GTInMsg *msg, int index);
int ov00_0220DD34(GTConn *conn);
void ov00_0220DDB4(GTConn *conn);
int ov00_0220DDDC(GTConn *conn, int type, u8 *msg, int len);
int ov00_0220DF28(GTConn *conn, u8 *msg, int len);
int ov00_0220DF74(GTConn *conn, u8 *msg, int len);
int ov00_0220E050(GTConn *conn, u8 *msg, int len);
int ov00_0220E05C(GTConn *conn, u8 *msg, int len);
int ov00_0220E0F0(GTConn *conn);
int ov00_0220E128(GTConn *conn, int type, u8 *msg, int len);
int ov00_0220E1E0(GTSocket *socket, u8 *msg, int len, u32 ip, u16 port);
int ov00_0220E490(GTSocket *socket, u32 ip, u16 port);
int ov00_0220E590(GTSocket *socket);

// gt_buffer.c (ov00_0220C200 takes int arguments but gt_socket.c declares it with a u16 port)
BOOL ov00_0220BC50(const u8 *buf);
u8 *ov00_0220BCEC(u8 *buf);
char *ov00_0220BE08(char *out, const char *key);
int ov00_0220BF10(u8 *a, u8 *b);
int ov00_0220BF44(GTBuffer *buffer, int size);
int ov00_0220BF70(GTBuffer *buffer);
void ov00_0220BF80(GTBuffer *buffer, char c);
void ov00_0220BFD0(GTBuffer *buffer, int v);
void ov00_0220C03C(GTBuffer *buffer, const void *src, int n);
void ov00_0220C0C4(GTBuffer *buffer, int pos, int n);
int ov00_0220C164(GTSocket *socket);

#endif // POKEHEARTGOLD_LIB_INCLUDE_DWC_GT_INTERNAL_H
