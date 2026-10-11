#pragma force_active on
#include <string.h>

#include "global.h"

#include "dwc/gp_internal.h"

// The code below is compiled as ARM (32-bit) code.
#include <nitro/code32.h>

// Presence connection: direct connections to other profiles ("gpiPeer.c": connect / accept with the
// "\auth\" handshake, buddy messages and transfers, queued messages) and the start of "gpiProfile.c" (the
// profile table, the "\npr\" reply).

extern const char ov00_02218F50[]; // "gpiRemovePeer"
extern const char ov00_02218F60[]; // "gpiProcessPeer"
extern const char ov00_02218F70[]; // "gpiPeerAddMessage"
extern const char ov00_02218F84[]; // "gpiPeerStartTransferMessage"
extern const char ov00_02218FA0[]; // "gpiPeerFinishTransferMessage"
extern const char ov00_02218FC0[]; // "gpiProcessPeerAcceptingConnection"
extern const char ov00_02218FE4[]; // "gpiProcessPeerInitiatingConnection"
extern const char ov00_02219008[]; // "Error connecting to a peer."
extern const char ov00_02219024[]; // "\\auth\\"
extern const char ov00_0221902C[]; // "\\pid\\"
extern const char ov00_02219034[]; // "\\nick\\"
extern const char ov00_0221903C[]; // "\\sig\\"
extern const char ov00_02219044[]; // "\\final\\"
extern const char ov00_0221904C[]; // "PR"
extern const char ov00_02219050[]; // "\\anack\\"
extern const char ov00_02219058[]; // "Error getting buddy authorization."
extern const char ov00_0221907C[]; // "\\aack\\"
extern const char ov00_02219084[]; // "Error parsing buddy message."
extern const char ov00_022190A4[]; // "0"
extern const char ov00_022190A8[]; // "gpiPeer.c"
extern const char ov00_022190B4[]; // "peer->state == GPI_PEER_WAITING"
extern const char ov00_022190D4[]; // "%s%d%d"
extern const char ov00_022190DC[]; // "Out of memory."
extern const char ov00_022190EC[]; // "1"
extern const char ov00_022190F0[]; // "peer->state != GPI_PEER_NOT_CONNECTED"
extern const char ov00_02219118[]; // "peer != NULL"
extern const char ov00_02219128[]; // "Tried to remove peer not in list."
extern const char ov00_0221914C[]; // "There was an error creating a socket."
extern const char ov00_02219174[]; // "There was an error making a socket non-blocking."
extern const char ov00_022191A8[]; // "There was an error connecting a socket."
extern const char ov00_022191D0[]; // "message != NULL"
extern const char ov00_022191E0[]; // "\\m\\"
extern const char ov00_022191E4[]; // "\\len\\"
extern const char ov00_022191EC[]; // "\\msg\\\n"
extern const char ov00_022191F4[]; // "transferID"
extern const char ov00_02219200[]; // "\\m\\%d\\xfer\\%d %u %u"
extern char ov00_02219214[];       // ""
extern const char ov00_02219218[]; // "\\len\\%d\\msg\\\n"
extern const char ov00_0221923C[]; // "\\npr\\"
extern const char ov00_02219244[]; // "Unexpected data was received from the server."
extern const char ov00_02219274[]; // "\\profileid\\"
extern const char ov00_02219280[]; // "Out of memory."

typedef struct SocketAddress {
    u8 len;
    u8 family;
    u16 port;
    u32 addr;
} SocketAddress;

extern int strncmp(const char *, const char *, unsigned);
extern int sprintf(char *, const char *, ...);
extern long atol(const char *);
extern char *strstr(const char *, const char *);
extern void __msl_assertion_failed(const char *, const char *, const char *, int);
extern void *ov00_021EC3A8(u32 size);
extern void ov00_021EC3D8(void *p);
extern void *ov00_021FE5B4(int elemSize, int count, void (*freeElem)(void *));
extern void ov00_021FE678(void *array);
extern void *ov00_021FE6EC(void *array, int i);
extern void ov00_021FE754(void *array, void *elem);
extern void ov00_021FE968(void *array, int i);
extern void *ov00_021FED24(int elemSize, int numBuckets, int (*hash)(GpProfile *, int), int (*compare)(GpProfile *, GpProfile *), void (*freeElem)(GpProfile *));
extern int ArrayLength(void *array);
extern void ov00_021FF31C(const char *data, int len, char *md5);
extern char *ov00_021FF3D4(const char *string);
extern int ov00_021FF44C(int sock, int blocking);
extern void ov00_021FF48C(int sock, int size);
extern void ov00_021FF4D0(int sock, int size);
extern void ov00_021FF510(int sock);
extern void ov00_021FF558(int sock);
extern int ov00_021FF694(int sock);
extern int ov00_021FF804(int domain, int type, int protocol);
extern void ov00_021FF818(int sock);
extern void ov00_021FF82C(int sock, int how);
extern int ov00_021FF8A0(int sock, SocketAddress *address, int len);
extern int ov00_021FF900(int sock, void *address, int *len);
extern int ov00_021FFA60(int sock);
extern int ov00_021FFA70(int);
extern void ov00_02202990(GpConnection **connection, int result, int fatal);
extern int ov00_02202424(GpConnection **connection, GpPeer *peer, char *string);
extern int ov00_02202A84(GpConnection **connection, GpCallback callback, void *arg, GpOperation *operation, int type);

// Argument of the peer buddy message callback.
typedef struct GpPeerMessageArg {
    int profile;
    int date;
    char *message;
} GpPeerMessageArg;

int ov00_0220792C(GpConnection **connection, GpPeer *peer, int type, int *values);
int ov00_022079A8(GpConnection **connection, GpPeer *peer, char *buffer, int len);

// "gpiProcessPeerInitiatingConnection": gets the peer's signature, connects and authenticates.
int ov00_0220684C(GpConnection **connection, GpPeer *peer) {
    GpConnection *iconnection = *connection;

    struct {
        GpProfile *profile;
        int closed, bytesRead, state;
    } local;

    int result;
    switch (peer->state) {
    case GP_PEER_GETTING_SIG:
        break;
    case GP_PEER_GOT_SIG:
        result = ov00_02207610(connection, peer);
        if (result) {
            return result;
        }
        break;
    case GP_PEER_CONNECTING:
        result = ov00_0220A5A8(connection, peer->sock, &local.state);
        if (result) {
            return result;
        }
        if (local.state == 4) {
            ov00_0220A890(connection, ov00_02219008);
            return 3;
        }
        if (local.state == 3) {
            BOOL freeSig = TRUE;
            GpPeer *other;
            if (!ov00_02207DD0(connection, peer->profile, &local.profile)) {
                ov00_0220A890(connection, ov00_02219008);
                return 3;
            }
            ov00_022020EC(connection, &peer->outputBuffer, ov00_02219024);
            ov00_022020EC(connection, &peer->outputBuffer, ov00_0221902C);
            ov00_0220211C(connection, &peer->outputBuffer, iconnection->profileId);
            ov00_022020EC(connection, &peer->outputBuffer, ov00_02219034);
            ov00_022020EC(connection, &peer->outputBuffer, iconnection->nick);
            ov00_022020EC(connection, &peer->outputBuffer, ov00_0221903C);
            ov00_022020EC(connection, &peer->outputBuffer, local.profile->peerSig);
            ov00_022020EC(connection, &peer->outputBuffer, ov00_02219044);
            for (other = iconnection->peers; other; other = other->next) {
                if (other->profile == peer->profile && other != peer && other->state <= GP_PEER_CONNECTING) {
                    freeSig = FALSE;
                }
            }
            if (freeSig) {
                ov00_021EC3D8(local.profile->peerSig);
                local.profile->peerSig = 0;
                if (ov00_02207FB8(local.profile)) {
                    ov00_02207E40(connection, local.profile);
                }
            }
            peer->state = GP_PEER_WAITING;
        }
        break;
    case GP_PEER_WAITING:
        result = ov00_02202454(connection, peer->sock, &peer->inputBuffer, &local.bytesRead, &local.closed, ov00_0221904C);
        if (result) {
            return result;
        }
        {
            char *end = strstr(peer->inputBuffer.buffer, ov00_02219044);
            if (end) {
                char *line;
                *end = 0;
                line = peer->inputBuffer.buffer;
                if (!strncmp(line, ov00_02219050, 7)) {
                    if (++peer->nackCount > 1) {
                        ov00_0220A890(connection, ov00_02219058);
                        return 3;
                    }
                    result = ov00_022075AC(connection, peer);
                    if (result) {
                        return result;
                    }
                } else if (strncmp(line, ov00_0221907C, 6)) {
                    ov00_0220A890(connection, ov00_02219084);
                    return 3;
                }
                peer->state = GP_PEER_CONNECTED;
                peer->inputBuffer.len = 0;
            }
        }
        break;
    default:
        __msl_assertion_failed(ov00_022190A4, ov00_022190A8, ov00_02218FE4, 159);
        break;
    }
    if (peer->outputBuffer.len > 0) {
        result = ov00_0220267C(connection, peer->sock, &peer->outputBuffer, &local.closed, 1, ov00_0221904C);
        if (local.closed || result) {
            peer->state = GP_PEER_DISCONNECTED;
        }
    }
    return 0;
}

// "gpiProcessPeerAcceptingConnection": checks the "\auth\" of a peer that connected to us.
int ov00_02206BA8(GpConnection **connection, GpPeer *peer) {
    GpConnection *iconnection = *connection;

    struct {
        int closed, bytesRead;
        char sigCheck[33], sig[33], nick[31], number[16], buffer[256];
    } local;

    char *end, *line;
    int result, profileId;
    if (peer->state != GP_PEER_WAITING) {
        __msl_assertion_failed(ov00_022190B4, ov00_022190A8, ov00_02218FC0, 193);
    }
    result = ov00_02202454(connection, peer->sock, &peer->inputBuffer, &local.bytesRead, &local.closed, ov00_0221904C);
    if (result) {
        return result;
    }
    if (local.closed) {
        peer->state = GP_PEER_DISCONNECTED;
        return 0;
    }
    end = strstr(peer->inputBuffer.buffer, ov00_02219044);
    if (end) {
        *end = 0;
        line = peer->inputBuffer.buffer;
        if (!strncmp(line, ov00_02219024, 6)) {
            if (!ov00_0220A4A0(line, ov00_0221902C, local.number, 16)) {
                peer->state = GP_PEER_DISCONNECTED;
                return 0;
            }
            profileId = atol(local.number);
            if (!ov00_0220A4A0(peer->inputBuffer.buffer, ov00_02219034, local.nick, 31)) {
                peer->state = GP_PEER_DISCONNECTED;
                return 0;
            }
            if (!ov00_0220A4A0(peer->inputBuffer.buffer, ov00_0221903C, local.sig, 33)) {
                peer->state = GP_PEER_DISCONNECTED;
                return 0;
            }
            sprintf(local.buffer, ov00_022190D4, iconnection->password, iconnection->profileId, profileId);
            ov00_021FF31C(local.buffer, strlen(local.buffer), local.sigCheck);
            if (strcmp(local.sig, local.sigCheck)) {
                ov00_022020EC(connection, &peer->outputBuffer, ov00_02219050);
                ov00_022020EC(connection, &peer->outputBuffer, ov00_02219044);
                peer->state = GP_PEER_DISCONNECTED;
                return 0;
            }
            ov00_022020EC(connection, &peer->outputBuffer, ov00_0221907C);
            ov00_022020EC(connection, &peer->outputBuffer, ov00_02219044);
            peer->state = GP_PEER_CONNECTED;
            peer->profile = profileId;
        } else {
            peer->state = GP_PEER_DISCONNECTED;
            return 0;
        }
        peer->inputBuffer.len = 0;
    }
    return 0;
}

// Sends the queued messages until one does not go out completely.
int ov00_02206E04(GpConnection **connection, GpPeer *peer) {
    int closed;
    GpPeerMessage *message;
    int result;
    if (peer->outputBuffer.len) {
        return 0;
    }
    if (ArrayLength(peer->messages)) {
        do {
            message = ov00_021FE6EC(peer->messages, 0);
            result = ov00_0220267C(connection, peer->sock, &message->buffer, &closed, 0, ov00_0221904C);
            if (closed || result) {
                peer->state = GP_PEER_DISCONNECTED;
                return 0;
            }
            if (message->buffer.pos != message->buffer.len) {
                break;
            }
            ov00_021FE968(peer->messages, 0);
        } while (ArrayLength(peer->messages));
    }
    return 0;
}

// Sends and receives on a connected peer and handles its messages.
int ov00_02206ED8(GpConnection **connection, GpPeer *peer) {
    int bytesRead;
    int closed;
    GpCallback callback;
    char *buffer;
    int type;
    int len;
    GpConnection *iconnection = *connection;
    if (peer->outputBuffer.len) {
        int result = ov00_0220267C(connection, peer->sock, &peer->outputBuffer, &closed, 1, ov00_0221904C);
        if (closed || result) {
            peer->state = GP_PEER_DISCONNECTED;
            return 0;
        }
    }
    if (!peer->outputBuffer.len) {
        int result = ov00_02206E04(connection, peer);
        if (result) {
            return result;
        }
        if (peer->state == GP_PEER_DISCONNECTED) {
            return 0;
        }
    }
    if (ov00_02202454(connection, peer->sock, &peer->inputBuffer, &bytesRead, &closed, ov00_0221904C)) {
        peer->state = GP_PEER_DISCONNECTED;
        return 0;
    }
    if (bytesRead > 0) {
        peer->timeout = ov00_021FFA70(0) + 300;
    }
    do {
        int result = ov00_022027FC(connection, &peer->inputBuffer, &buffer, &type, &len);
        if (result) {
            return result;
        }
        if (buffer) {
            switch (type) {
            case 1:
                callback = iconnection->callbacks[GP_CALLBACK_BUDDY_MESSAGE];
                if (callback.callback) {
                    GpPeerMessageArg *arg = ov00_021EC3A8(sizeof(GpPeerMessageArg));
                    if (!arg) {
                        ov00_0220A890(connection, ov00_022190DC);
                        return 1;
                    }
                    arg->profile = peer->profile;
                    arg->message = ov00_021FF3D4(buffer);
                    arg->date = ov00_021FFA70(0);
                    result = ov00_02202A84(connection, callback, arg, 0, 2);
                    if (result) {
                        return result;
                    }
                }
                break;
            case 102:
                ov00_02201BC4(connection, peer->profile, 103, ov00_022190EC);
                break;
            case 200:
            case 201:
            case 202:
            case 203:
            case 204:
            case 205:
            case 206:
            case 207:
            case 208:
                ov00_0220A1C0(connection, peer, type, peer->inputBuffer.buffer, buffer, len);
                break;
            }
            ov00_02202938(connection, &peer->inputBuffer);
        }
    } while (buffer);
    if (closed) {
        peer->state = GP_PEER_DISCONNECTED;
    }
    return 0;
}

// "gpiProcessPeer"
int ov00_02207158(GpConnection **connection, GpPeer *peer) {
    int result = 0;
    if (peer->state == GP_PEER_NOT_CONNECTED) {
        __msl_assertion_failed(ov00_022190F0, ov00_022190A8, ov00_02218F60, 0x1d9);
    }
    if (peer->state != GP_PEER_CONNECTED) {
        if (peer->initiated) {
            result = ov00_0220684C(connection, peer);
        } else {
            result = ov00_02206BA8(connection, peer);
        }
    }
    if (!result && peer->state == GP_PEER_CONNECTED) {
        result = ov00_02206ED8(connection, peer);
    }
    return result;
}

// Closes and frees a peer.
void ov00_022071F4(GpConnection **connection, GpPeer *peer) {
    ov00_021FF82C(peer->sock, 2);
    ov00_021FF818(peer->sock);
    ov00_021EC3D8(peer->inputBuffer.buffer);
    peer->inputBuffer.buffer = 0;
    ov00_021EC3D8(peer->outputBuffer.buffer);
    peer->outputBuffer.buffer = 0;
    if (peer->messages) {
        ov00_021FE678(peer->messages);
        peer->messages = 0;
    }
    ov00_021EC3D8(peer);
}

// "gpiRemovePeer": unlinks a peer; its unsent buddy messages go through the server instead.
void ov00_02207254(GpConnection **connection, GpPeer *peer) {
    GpConnection *iconnection = *connection;
    GpPeer *prev, *next;
    int i, j;
    if (!peer) {
        __msl_assertion_failed(ov00_02219118, ov00_022190A8, ov00_02218F50, 0x213);
    }
    prev = iconnection->peers;
    if (prev == peer) {
        iconnection->peers = peer->next;
    } else {
        next = prev->next;
        while (next != peer) {
            if (!next) {
                __msl_assertion_failed(ov00_022190A4, ov00_022190A8, ov00_02218F50, 0x225);
                ov00_0220A3BC(connection, ov00_02219128);
                return;
            }
            prev = next;
            next = next->next;
        }
        prev->next = peer->next;
    }
    if (ArrayLength(peer->messages)) {
        i = 0;
        j = 0;
        do {
            GpPeerMessage *message = ov00_021FE6EC(peer->messages, j);
            if (message->type < 100) {
                ov00_02201AE8(connection, peer->profile, message->type, message->buffer.buffer + message->start);
            }
            ov00_021FE968(peer->messages, i);
        } while (ArrayLength(peer->messages));
    }
    ov00_022071F4(connection, peer);
}

// Socket buffer sizes for peers.
void ov00_02207370(int sock) {
    ov00_021FF48C(sock, 16384);
    ov00_021FF48C(sock, 32768);
    ov00_021FF48C(sock, 65536);
    ov00_021FF48C(sock, 131072);
    ov00_021FF48C(sock, 262144);
    ov00_021FF4D0(sock, 16384);
    ov00_021FF4D0(sock, 32768);
    ov00_021FF4D0(sock, 65536);
    ov00_021FF510(sock);
    ov00_021FF558(sock);
}

// Accepts new peers and processes the existing ones, removing those that are done or timed out.
int ov00_022073E8(GpConnection **connection) {
    GpConnection *iconnection = *connection;
    GpPeer *peer, *next;
    int sock, result;
    if (iconnection->peerSocket != -1 && ov00_021FF694(iconnection->peerSocket)) {
        sock = ov00_021FF900(iconnection->peerSocket, 0, 0);
        if (sock != -1) {
            peer = ov00_0220752C(connection, -1, 0);
            if (peer) {
                peer->state = GP_PEER_WAITING;
                peer->sock = sock;
                ov00_021FF44C(sock, 0);
                ov00_02207370(peer->sock);
            } else {
                ov00_021FF818(sock);
            }
        }
    }
    for (peer = iconnection->peers; peer; peer = next) {
        next = peer->next;
        result = ov00_02207158(connection, peer);
        if (peer->state == GP_PEER_DISCONNECTED || result != 0 || ov00_021FFA70(0) > peer->timeout) {
            ov00_02207254(connection, peer);
        }
    }
    return 0;
}

// The connected peer of a profile.
GpPeer *ov00_022074D8(GpConnection **connection, int profile) {
    GpPeer *peer = (*connection)->peers;
    while (peer) {
        if (peer->profile == profile && peer->state == GP_PEER_CONNECTED) {
            return peer;
        }
        peer = peer->next;
    }
    return 0;
}

// Frees the buffer of a queued message (message array element destructor).
void ov00_02207510(void *message) {
    ov00_021EC3D8(*(void **)message);
    *(void **)message = 0;
}

GpPeer *ov00_0220752C(GpConnection **connection, int profile, int initiated) {
    GpConnection *iconnection = *connection;
    GpPeer *peer = ov00_021EC3A8(sizeof(GpPeer));
    if (!peer) {
        return 0;
    }
    memset(peer, 0, sizeof(GpPeer));
    peer->state = GP_PEER_NOT_CONNECTED;
    peer->initiated = initiated;
    peer->sock = -1;
    peer->profile = profile;
    peer->timeout = ov00_021FFA70(0) + 300;
    peer->next = iconnection->peers;
    peer->messages = ov00_021FE5B4(sizeof(GpPeerMessage), 0, ov00_02207510);
    iconnection->peers = peer;
    return peer;
}

// Requests the profile info of the peer, for its signature.
int ov00_022075AC(GpConnection **connection, GpPeer *peer) {
    GpOperation *operation;
    int result = ov00_0220657C(connection, GP_OPERATION_GET_INFO, 0, &operation, 0, 0, 0);
    if (result != 0) {
        return result;
    }
    result = ov00_02205E50(connection, peer->profile, operation->id);
    if (result == 0) {
        peer->state = GP_PEER_GETTING_SIG;
        result = 0;
    }
    return result;
}

// Connects to the peer at the address of its buddy status.
int ov00_02207610(GpConnection **connection, GpPeer *peer) {
    struct {
        GpProfile *profile;
        SocketAddress address;
    } frame;

    if (ov00_02207DD0(connection, peer->profile, &frame.profile) == 0) {
        ov00_0220A890(connection, ov00_02219008);
        return 3;
    }
    peer->sock = ov00_021FF804(2, 1, 0);
    if ((int)peer->sock == -1) {
        ov00_0220A86C(connection, 5, ov00_0221914C);
        ov00_02202990(connection, 3, 0);
        return 3;
    }
    if (ov00_021FF44C(peer->sock, 0) == 0) {
        ov00_0220A86C(connection, 5, ov00_02219174);
        ov00_02202990(connection, 3, 0);
        return 3;
    }
    ov00_02207370(peer->sock);
    {
        SocketAddress *p = &frame.address;
        *(u32 *)p = 0;
        p->addr = 0;
    }
    frame.address.family = 2;
    frame.address.addr = frame.profile->buddyStatus->ip;
    frame.address.port = frame.profile->buddyStatus->port;
    if (ov00_021FF8A0(peer->sock, &frame.address, 8) == -1) {
        int error = ov00_021FFA60(peer->sock);
        if (error != -6 && error != -26 && error != -76) {
            ov00_0220A86C(connection, 5, ov00_022191A8);
            ov00_02202990(connection, 3, 1);
            return 3;
        }
    }
    peer->state = GP_PEER_CONNECTING;
    return 0;
}

// "gpiPeerAddMessage": queues "\m\<type>\len\<n>\msg\\n<message>\0".
int ov00_0220779C(GpConnection **connection, GpPeer *peer, int type, const char *message) {
    int len, result;
    volatile GpPeerMessage queued;
    if (peer == NULL) {
        __msl_assertion_failed(ov00_02219118, ov00_022190A8, ov00_02218F70, 0x340);
    }
    if (message == NULL) {
        __msl_assertion_failed(ov00_022191D0, ov00_022190A8, ov00_02218F70, 0x341);
    }
    len = strlen(message);
    memset((void *)&queued, 0, sizeof(queued));
    queued.type = type;
    result = ov00_022020EC(connection, (GpBuffer *)&queued.buffer, ov00_022191E0);
    if (result) {
        return result;
    }
    result = ov00_0220211C(connection, (GpBuffer *)&queued.buffer, type);
    if (result) {
        return result;
    }
    result = ov00_022020EC(connection, (GpBuffer *)&queued.buffer, ov00_022191E4);
    if (result) {
        return result;
    }
    result = ov00_0220211C(connection, (GpBuffer *)&queued.buffer, len);
    if (result) {
        return result;
    }
    result = ov00_022020EC(connection, (GpBuffer *)&queued.buffer, ov00_022191EC);
    if (result) {
        return result;
    }
    queued.start = queued.buffer.len;
    result = ov00_02201FE0(connection, (GpBuffer *)&queued.buffer, message, len);
    if (result) {
        return result;
    }
    result = ov00_02201F48(connection, (GpBuffer *)&queued.buffer, 0);
    if (result) {
        return result;
    }
    ov00_021FE754(peer->messages, (void *)&queued);
    peer->timeout = ov00_021FFA70(0) + 300;
    return 0;
}

// "gpiPeerStartTransferMessage"
int ov00_0220792C(GpConnection **connection, GpPeer *peer, int type, int *values) {
    int b, a, c;
    a = *(volatile int *)values;
    b = values[1];
    c = values[2];
    char buffer[64];
    if (values == NULL) {
        __msl_assertion_failed(ov00_022191F4, ov00_022190A8, ov00_02218F84, 0x376);
    }
    sprintf(buffer, ov00_02219200, type, a, b, c);
    return ov00_02202424(connection, peer, buffer);
}

// "gpiPeerFinishTransferMessage"
int ov00_022079A8(GpConnection **connection, GpPeer *peer, char *message, int len) {
    char buffer[32];
    int result;
    if (!peer) {
        __msl_assertion_failed(ov00_02219118, ov00_022190A8, ov00_02218FA0, 0x389);
    }
    if (!message) {
        message = ov00_02219214;
    }
    if (len == -1) {
        len = strlen(message);
    }
    sprintf(buffer, ov00_02219218, len);
    result = ov00_02202424(connection, peer, buffer);
    if (result) {
        return result;
    }
    result = ov00_02202324(connection, peer, message, len);
    if (result) {
        return result;
    }
    result = ov00_02202244(connection, peer, 0);
    if (result) {
        return result;
    }
    peer->timeout = ov00_021FFA70(0) + 300;
    return 0;
}

// Profile table hash.
int ov00_02207A94(GpProfile *profile, int numBuckets) {
    return profile->profileId % numBuckets;
}

// Profile table compare.
int ov00_02207AA8(GpProfile *a, GpProfile *b) {
    return a->profileId - b->profileId;
}

// Profile table element destructor.
void ov00_02207AB8(GpProfile *profile) {
    if (profile->buddyStatus) {
        ov00_021EC3D8(profile->buddyStatus->statusString);
        profile->buddyStatus->statusString = 0;
        ov00_021EC3D8(profile->buddyStatus->locationString);
        profile->buddyStatus->locationString = 0;
        ov00_021EC3D8(profile->buddyStatus);
        profile->buddyStatus = 0;
    }
    ov00_02206164(profile);
    ov00_021EC3D8(profile->authSig);
    profile->authSig = 0;
    ov00_021EC3D8(profile->peerSig);
    profile->peerSig = 0;
}

// Creates the profile table.
int ov00_02207B34(GpConnection **connection) {
    GpConnection *iconnection = *connection;
    iconnection->numBuddies = 0;
    iconnection->numProfiles = 0;
    iconnection->profileTable = ov00_021FED24(sizeof(GpProfile), 4, ov00_02207A94, ov00_02207AA8, ov00_02207AB8);
    return iconnection->profileTable != 0;
}

// Handles the "\npr\" (new profile) reply.
int ov00_02207B8C(GpConnection **connection, GpOperation *operation, char *input) {
    char buffer[16];
    GpCallback callback;
    int profileId, result;
    int *arg;
    if (ov00_0220A3C8(connection, input, 1) != 0) {
        return 4;
    }
    if (strncmp(input, ov00_0221923C, 5) != 0) {
        ov00_0220A86C(connection, 1, ov00_02219244);
        ov00_02202990(connection, 3, 1);
        return 3;
    }
    if (ov00_0220A4A0(input, ov00_02219274, buffer, 16) == 0) {
        ov00_0220A86C(connection, 1, ov00_02219244);
        ov00_02202990(connection, 3, 1);
        return 3;
    }
    profileId = atol(buffer);
    callback.callback = operation->callback.callback;
    callback.param = operation->callback.param;
    if (callback.callback) {
        arg = ov00_021EC3A8(8);
        if (arg == 0) {
            ov00_0220A890(connection, ov00_02219280);
            return 1;
        }
        arg[1] = profileId;
        arg[0] = 0;
        result = ov00_02202A84(connection, callback, arg, operation, 0);
        if (result != 0) {
            return result;
        }
    }
    ov00_022066D0(connection, operation);
    return 0;
}
