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
extern int gsSocketGetLastError(int sock);
extern int current_time_seconds(int);
extern void gpiCallErrorCallback(GpConnection **connection, int result, int fatal);
extern int gpiSendOrBufferString(GpConnection **connection, GpPeer *peer, char *string);
extern int gpiAddCallback(GpConnection **connection, GpCallback callback, void *arg, GpOperation *operation, int type);

// Argument of the peer buddy message callback.
typedef struct GpPeerMessageArg {
    int profile;
    int date;
    char *message;
} GpPeerMessageArg;

int gpiPeerStartTransferMessage(GpConnection **connection, GpPeer *peer, int type, int *values);
int gpiPeerFinishTransferMessage(GpConnection **connection, GpPeer *peer, char *buffer, int len);

// "gpiProcessPeerInitiatingConnection": gets the peer's signature, connects and authenticates.
int gpiProcessPeerInitiatingConnection(GpConnection **connection, GpPeer *peer) {
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
        result = gpiPeerStartConnect(connection, peer);
        if (result) {
            return result;
        }
        break;
    case GP_PEER_CONNECTING:
        result = gpiCheckSocketConnect(connection, peer->sock, &local.state);
        if (result) {
            return result;
        }
        if (local.state == 4) {
            gpiSetErrorString(connection, ov00_02219008);
            return 3;
        }
        if (local.state == 3) {
            BOOL freeSig = TRUE;
            GpPeer *other;
            if (!gpiGetProfile(connection, peer->profile, &local.profile)) {
                gpiSetErrorString(connection, ov00_02219008);
                return 3;
            }
            gpiAppendStringToBuffer(connection, &peer->outputBuffer, ov00_02219024);
            gpiAppendStringToBuffer(connection, &peer->outputBuffer, ov00_0221902C);
            gpiAppendIntToBuffer(connection, &peer->outputBuffer, iconnection->profileId);
            gpiAppendStringToBuffer(connection, &peer->outputBuffer, ov00_02219034);
            gpiAppendStringToBuffer(connection, &peer->outputBuffer, iconnection->nick);
            gpiAppendStringToBuffer(connection, &peer->outputBuffer, ov00_0221903C);
            gpiAppendStringToBuffer(connection, &peer->outputBuffer, local.profile->peerSig);
            gpiAppendStringToBuffer(connection, &peer->outputBuffer, ov00_02219044);
            for (other = iconnection->peers; other; other = other->next) {
                if (other->profile == peer->profile && other != peer && other->state <= GP_PEER_CONNECTING) {
                    freeSig = FALSE;
                }
            }
            if (freeSig) {
                ov00_021EC3D8(local.profile->peerSig);
                local.profile->peerSig = 0;
                if (gpiCanFreeProfile(local.profile)) {
                    gpiRemoveProfile(connection, local.profile);
                }
            }
            peer->state = GP_PEER_WAITING;
        }
        break;
    case GP_PEER_WAITING:
        result = gpiRecvToBuffer(connection, peer->sock, &peer->inputBuffer, &local.bytesRead, &local.closed, ov00_0221904C);
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
                        gpiSetErrorString(connection, ov00_02219058);
                        return 3;
                    }
                    result = gpiPeerGetSig(connection, peer);
                    if (result) {
                        return result;
                    }
                } else if (strncmp(line, ov00_0221907C, 6)) {
                    gpiSetErrorString(connection, ov00_02219084);
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
        result = gpiSendFromBuffer(connection, peer->sock, &peer->outputBuffer, &local.closed, 1, ov00_0221904C);
        if (local.closed || result) {
            peer->state = GP_PEER_DISCONNECTED;
        }
    }
    return 0;
}

// "gpiProcessPeerAcceptingConnection": checks the "\auth\" of a peer that connected to us.
int gpiProcessPeerAcceptingConnection(GpConnection **connection, GpPeer *peer) {
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
    result = gpiRecvToBuffer(connection, peer->sock, &peer->inputBuffer, &local.bytesRead, &local.closed, ov00_0221904C);
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
            if (!gpiValueForKey(line, ov00_0221902C, local.number, 16)) {
                peer->state = GP_PEER_DISCONNECTED;
                return 0;
            }
            profileId = atol(local.number);
            if (!gpiValueForKey(peer->inputBuffer.buffer, ov00_02219034, local.nick, 31)) {
                peer->state = GP_PEER_DISCONNECTED;
                return 0;
            }
            if (!gpiValueForKey(peer->inputBuffer.buffer, ov00_0221903C, local.sig, 33)) {
                peer->state = GP_PEER_DISCONNECTED;
                return 0;
            }
            sprintf(local.buffer, ov00_022190D4, iconnection->password, iconnection->profileId, profileId);
            ov00_021FF31C(local.buffer, strlen(local.buffer), local.sigCheck);
            if (strcmp(local.sig, local.sigCheck)) {
                gpiAppendStringToBuffer(connection, &peer->outputBuffer, ov00_02219050);
                gpiAppendStringToBuffer(connection, &peer->outputBuffer, ov00_02219044);
                peer->state = GP_PEER_DISCONNECTED;
                return 0;
            }
            gpiAppendStringToBuffer(connection, &peer->outputBuffer, ov00_0221907C);
            gpiAppendStringToBuffer(connection, &peer->outputBuffer, ov00_02219044);
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
int gpiPeerSendMessages(GpConnection **connection, GpPeer *peer) {
    int closed;
    GpPeerMessage *message;
    int result;
    if (peer->outputBuffer.len) {
        return 0;
    }
    if (ArrayLength(peer->messages)) {
        do {
            message = ov00_021FE6EC(peer->messages, 0);
            result = gpiSendFromBuffer(connection, peer->sock, &message->buffer, &closed, 0, ov00_0221904C);
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
int gpiProcessPeerConnected(GpConnection **connection, GpPeer *peer) {
    int bytesRead;
    int closed;
    GpCallback callback;
    char *buffer;
    int type;
    int len;
    GpConnection *iconnection = *connection;
    if (peer->outputBuffer.len) {
        int result = gpiSendFromBuffer(connection, peer->sock, &peer->outputBuffer, &closed, 1, ov00_0221904C);
        if (closed || result) {
            peer->state = GP_PEER_DISCONNECTED;
            return 0;
        }
    }
    if (!peer->outputBuffer.len) {
        int result = gpiPeerSendMessages(connection, peer);
        if (result) {
            return result;
        }
        if (peer->state == GP_PEER_DISCONNECTED) {
            return 0;
        }
    }
    if (gpiRecvToBuffer(connection, peer->sock, &peer->inputBuffer, &bytesRead, &closed, ov00_0221904C)) {
        peer->state = GP_PEER_DISCONNECTED;
        return 0;
    }
    if (bytesRead > 0) {
        peer->timeout = current_time_seconds(0) + 300;
    }
    do {
        int result = gpiReadMessage(connection, &peer->inputBuffer, &buffer, &type, &len);
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
                        gpiSetErrorString(connection, ov00_022190DC);
                        return 1;
                    }
                    arg->profile = peer->profile;
                    arg->message = ov00_021FF3D4(buffer);
                    arg->date = current_time_seconds(0);
                    result = gpiAddCallback(connection, callback, arg, 0, 2);
                    if (result) {
                        return result;
                    }
                }
                break;
            case 102:
                gpiSendBuddyMessage(connection, peer->profile, 103, ov00_022190EC);
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
                gpiHandleTransferMessage(connection, peer, type, peer->inputBuffer.buffer, buffer, len);
                break;
            }
            gpiClipBufferToPosition(connection, &peer->inputBuffer);
        }
    } while (buffer);
    if (closed) {
        peer->state = GP_PEER_DISCONNECTED;
    }
    return 0;
}

// "gpiProcessPeer"
int gpiProcessPeer(GpConnection **connection, GpPeer *peer) {
    int result = 0;
    if (peer->state == GP_PEER_NOT_CONNECTED) {
        __msl_assertion_failed(ov00_022190F0, ov00_022190A8, ov00_02218F60, 0x1d9);
    }
    if (peer->state != GP_PEER_CONNECTED) {
        if (peer->initiated) {
            result = gpiProcessPeerInitiatingConnection(connection, peer);
        } else {
            result = gpiProcessPeerAcceptingConnection(connection, peer);
        }
    }
    if (!result && peer->state == GP_PEER_CONNECTED) {
        result = gpiProcessPeerConnected(connection, peer);
    }
    return result;
}

// Closes and frees a peer.
void gpiDestroyPeer(GpConnection **connection, GpPeer *peer) {
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
void gpiRemovePeer(GpConnection **connection, GpPeer *peer) {
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
                gpiDebug(connection, ov00_02219128);
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
                gpiSendServerBuddyMessage(connection, peer->profile, message->type, message->buffer.buffer + message->start);
            }
            ov00_021FE968(peer->messages, i);
        } while (ArrayLength(peer->messages));
    }
    gpiDestroyPeer(connection, peer);
}

// Socket buffer sizes for peers.
void gpiSetPeerSocketSizes(int sock) {
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
int gpiProcessPeers(GpConnection **connection) {
    GpConnection *iconnection = *connection;
    GpPeer *peer, *next;
    int sock, result;
    if (iconnection->peerSocket != -1 && ov00_021FF694(iconnection->peerSocket)) {
        sock = ov00_021FF900(iconnection->peerSocket, 0, 0);
        if (sock != -1) {
            peer = gpiAddPeer(connection, -1, 0);
            if (peer) {
                peer->state = GP_PEER_WAITING;
                peer->sock = sock;
                ov00_021FF44C(sock, 0);
                gpiSetPeerSocketSizes(peer->sock);
            } else {
                ov00_021FF818(sock);
            }
        }
    }
    for (peer = iconnection->peers; peer; peer = next) {
        next = peer->next;
        result = gpiProcessPeer(connection, peer);
        if (peer->state == GP_PEER_DISCONNECTED || result != 0 || current_time_seconds(0) > peer->timeout) {
            gpiRemovePeer(connection, peer);
        }
    }
    return 0;
}

// The connected peer of a profile.
GpPeer *gpiGetPeerByProfile(GpConnection **connection, int profile) {
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
void gpiFreeMessage(void *message) {
    ov00_021EC3D8(*(void **)message);
    *(void **)message = 0;
}

GpPeer *gpiAddPeer(GpConnection **connection, int profile, int initiated) {
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
    peer->timeout = current_time_seconds(0) + 300;
    peer->next = iconnection->peers;
    peer->messages = ov00_021FE5B4(sizeof(GpPeerMessage), 0, gpiFreeMessage);
    iconnection->peers = peer;
    return peer;
}

// Requests the profile info of the peer, for its signature.
int gpiPeerGetSig(GpConnection **connection, GpPeer *peer) {
    GpOperation *operation;
    int result = gpiAddOperation(connection, GP_OPERATION_GET_INFO, 0, &operation, 0, 0, 0);
    if (result != 0) {
        return result;
    }
    result = gpiSendGetInfo(connection, peer->profile, operation->id);
    if (result == 0) {
        peer->state = GP_PEER_GETTING_SIG;
        result = 0;
    }
    return result;
}

// Connects to the peer at the address of its buddy status.
int gpiPeerStartConnect(GpConnection **connection, GpPeer *peer) {
    struct {
        GpProfile *profile;
        SocketAddress address;
    } frame;

    if (gpiGetProfile(connection, peer->profile, &frame.profile) == 0) {
        gpiSetErrorString(connection, ov00_02219008);
        return 3;
    }
    peer->sock = ov00_021FF804(2, 1, 0);
    if ((int)peer->sock == -1) {
        gpiSetError(connection, 5, ov00_0221914C);
        gpiCallErrorCallback(connection, 3, 0);
        return 3;
    }
    if (ov00_021FF44C(peer->sock, 0) == 0) {
        gpiSetError(connection, 5, ov00_02219174);
        gpiCallErrorCallback(connection, 3, 0);
        return 3;
    }
    gpiSetPeerSocketSizes(peer->sock);
    {
        SocketAddress *p = &frame.address;
        *(u32 *)p = 0;
        p->addr = 0;
    }
    frame.address.family = 2;
    frame.address.addr = frame.profile->buddyStatus->ip;
    frame.address.port = frame.profile->buddyStatus->port;
    if (ov00_021FF8A0(peer->sock, &frame.address, 8) == -1) {
        int error = gsSocketGetLastError(peer->sock);
        if (error != -6 && error != -26 && error != -76) {
            gpiSetError(connection, 5, ov00_022191A8);
            gpiCallErrorCallback(connection, 3, 1);
            return 3;
        }
    }
    peer->state = GP_PEER_CONNECTING;
    return 0;
}

// "gpiPeerAddMessage": queues "\m\<type>\len\<n>\msg\\n<message>\0".
int gpiPeerAddMessage(GpConnection **connection, GpPeer *peer, int type, const char *message) {
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
    result = gpiAppendStringToBuffer(connection, (GpBuffer *)&queued.buffer, ov00_022191E0);
    if (result) {
        return result;
    }
    result = gpiAppendIntToBuffer(connection, (GpBuffer *)&queued.buffer, type);
    if (result) {
        return result;
    }
    result = gpiAppendStringToBuffer(connection, (GpBuffer *)&queued.buffer, ov00_022191E4);
    if (result) {
        return result;
    }
    result = gpiAppendIntToBuffer(connection, (GpBuffer *)&queued.buffer, len);
    if (result) {
        return result;
    }
    result = gpiAppendStringToBuffer(connection, (GpBuffer *)&queued.buffer, ov00_022191EC);
    if (result) {
        return result;
    }
    queued.start = queued.buffer.len;
    result = gpiAppendStringToBufferLen(connection, (GpBuffer *)&queued.buffer, message, len);
    if (result) {
        return result;
    }
    result = gpiAppendCharToBuffer(connection, (GpBuffer *)&queued.buffer, 0);
    if (result) {
        return result;
    }
    ov00_021FE754(peer->messages, (void *)&queued);
    peer->timeout = current_time_seconds(0) + 300;
    return 0;
}

// "gpiPeerStartTransferMessage"
int gpiPeerStartTransferMessage(GpConnection **connection, GpPeer *peer, int type, int *values) {
    int b, a, c;
    a = *(volatile int *)values;
    b = values[1];
    c = values[2];
    char buffer[64];
    if (values == NULL) {
        __msl_assertion_failed(ov00_022191F4, ov00_022190A8, ov00_02218F84, 0x376);
    }
    sprintf(buffer, ov00_02219200, type, a, b, c);
    return gpiSendOrBufferString(connection, peer, buffer);
}

// "gpiPeerFinishTransferMessage"
int gpiPeerFinishTransferMessage(GpConnection **connection, GpPeer *peer, char *message, int len) {
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
    result = gpiSendOrBufferString(connection, peer, buffer);
    if (result) {
        return result;
    }
    result = gpiSendOrBufferStringLen(connection, peer, message, len);
    if (result) {
        return result;
    }
    result = gpiSendOrBufferChar(connection, peer, 0);
    if (result) {
        return result;
    }
    peer->timeout = current_time_seconds(0) + 300;
    return 0;
}

// Profile table hash.
int gpiProfilesTableHash(GpProfile *profile, int numBuckets) {
    return profile->profileId % numBuckets;
}

// Profile table compare.
int gpiProfilesTableCompare(GpProfile *a, GpProfile *b) {
    return a->profileId - b->profileId;
}

// Profile table element destructor.
void gpiProfilesTableFree(GpProfile *profile) {
    if (profile->buddyStatus) {
        ov00_021EC3D8(profile->buddyStatus->statusString);
        profile->buddyStatus->statusString = 0;
        ov00_021EC3D8(profile->buddyStatus->locationString);
        profile->buddyStatus->locationString = 0;
        ov00_021EC3D8(profile->buddyStatus);
        profile->buddyStatus = 0;
    }
    gpiFreeInfoCache(profile);
    ov00_021EC3D8(profile->authSig);
    profile->authSig = 0;
    ov00_021EC3D8(profile->peerSig);
    profile->peerSig = 0;
}

// Creates the profile table.
int gpiInitProfiles(GpConnection **connection) {
    GpConnection *iconnection = *connection;
    iconnection->numBuddies = 0;
    iconnection->numProfiles = 0;
    iconnection->profileTable = ov00_021FED24(sizeof(GpProfile), 4, gpiProfilesTableHash, gpiProfilesTableCompare, gpiProfilesTableFree);
    return iconnection->profileTable != 0;
}

// Handles the "\npr\" (new profile) reply.
int gpiProcessNewProfile(GpConnection **connection, GpOperation *operation, char *input) {
    char buffer[16];
    GpCallback callback;
    int profileId, result;
    int *arg;
    if (gpiCheckForError(connection, input, 1) != 0) {
        return 4;
    }
    if (strncmp(input, ov00_0221923C, 5) != 0) {
        gpiSetError(connection, 1, ov00_02219244);
        gpiCallErrorCallback(connection, 3, 1);
        return 3;
    }
    if (gpiValueForKey(input, ov00_02219274, buffer, 16) == 0) {
        gpiSetError(connection, 1, ov00_02219244);
        gpiCallErrorCallback(connection, 3, 1);
        return 3;
    }
    profileId = atol(buffer);
    callback.callback = operation->callback.callback;
    callback.param = operation->callback.param;
    if (callback.callback) {
        arg = ov00_021EC3A8(8);
        if (arg == 0) {
            gpiSetErrorString(connection, ov00_02219280);
            return 1;
        }
        arg[1] = profileId;
        arg[0] = 0;
        result = gpiAddCallback(connection, callback, arg, operation, 0);
        if (result != 0) {
            return result;
        }
    }
    gpiRemoveOperation(connection, operation);
    return 0;
}
