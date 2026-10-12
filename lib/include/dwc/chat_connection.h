#ifndef POKEHEARTGOLD_LIB_INCLUDE_DWC_CHAT_CONNECTION_H
#define POKEHEARTGOLD_LIB_INCLUDE_DWC_CHAT_CONNECTION_H

// Connection object of the chat client in the OVY_45 network library (src/chat_main.c and the socket
// code after it): the server socket with its line buffers, the login data and the user callbacks.
// Included by dwc/chat_internal.h (after ChatCipher).

typedef struct ChatRequest ChatRequest; // defined in dwc/chat_internal.h

// A growing byte buffer of the socket (size is a multiple of 0x2000, data has size + 1 bytes).
typedef struct ChatBuffer {
    char *data;
    int length;
    int size;
} ChatBuffer;

// One line received from the server, split into its parts (heap strings owned by the message):
// [":" prefix] command [middle] [":" trailing], the prefix split into nick "!" user "@" host.
typedef struct ChatMessage {
    char *text; // the whole line
    char *prefix;
    char *nick;
    char *user;
    char *host;
    char *command;
    char *middle;
    char *trailing;
    char **params; // the words of middle and the trailing part
    int paramCount;
} ChatMessage;

// The connection to the chat server.
typedef struct ChatSocket {
    int handle; // -1 = none
    u32 state;  // 0 = not connected, 1 = connected, 2 = disconnected
    char host[256];
    ChatBuffer input;  // received bytes, NUL terminated
    ChatBuffer output; // bytes waiting to be sent
    BOOL encrypted;    // both directions go through the ciphers
    ChatCipher inputCipher;
    ChatCipher outputCipher;
    ChatMessage message; // the last line returned by ov45_02253B5C
} ChatSocket;

// Callbacks given to the connect functions, kept for the whole connection.
typedef struct ChatGlobalCallbacks {
    void *raw;            // callback type 0: every received line
    void *disconnected;   // callback type 1: the connection was lost (with the reason)
    void *privateMessage; // callback type 2: a message from a user
    void *unk_0C;
    void *param; // user data passed to the callbacks
} ChatGlobalCallbacks;

typedef struct ChatConnection {
    BOOL connected;
    BOOL connecting;           // cleared when the connect attempt ends
    BOOL disconnected;         // the disconnected callback was queued
    void *nickErrorCallback;   // callback type 25: the nick was refused
    void *userAddressCallback; // when set, the address of the user is asked for first
    void *connectedCallback;   // (chat, success, failureReason, param)
    void *connectParam;        // user data of the three callbacks above
    ChatSocket socket;
    char nick[64];
    char realName[128];
    char user[128];
    int accountSpace;     // login types 1 and 2
    char email[64];       // login type 2
    char accountNick[32]; // login type 2
    char registeredNick[64];
    char password[32];    // login types 1 and 2
    char authTicket[256]; // login type 3
    char partnerKey[256]; // login type 3
    u32 unk_770;
    char serverAddress[128];
    int serverPort;
    ChatGlobalCallbacks globalCallbacks;
    void *channels;           // hash table of ChatChannel
    void *enteringChannels;   // array of ChatChannel records (name only) of channels being entered
    ChatRequest *requests;    // list of the requests waiting for an answer (chat_requests.c)
    ChatRequest *lastRequest; // its last element
    int nextRequestId;        // starts at 1, wraps from 0x7FFFFFFF to 1
    void *callbacks;          // array of ChatCallback
    int unk_824;
    char gameKey[128];
    int loginType; // 0 = nick only, 1 = registered nick, 2 = email and nick, 3 = pre-authenticated
    u32 unk_8AC[2];
} ChatConnection;

// Handler of one server command (table ov45_02256410, ov45_02255DA0 entries).
typedef struct ChatServerCommand {
    const char *command;
    void (*handler)(ChatConnection *chat, ChatMessage *message);
} ChatServerCommand;

typedef void (*ChatConnectedFunc)(ChatConnection *chat, BOOL success, int failureReason, void *param);

// Arguments of the nick error callback.
typedef struct ChatNickErrorArgs {
    int type;
    const char *nick;
    int unk_08;
    int unk_0C;
} ChatNickErrorArgs;

// Arguments of the channel message callback.
typedef struct ChatChannelMessageArgs {
    const char *channel;
    const char *sender;
    const char *message;
    int type; // 0 .. 4, see ov45_0225089C
} ChatChannelMessageArgs;

// Arguments of the channel mode callback.
typedef struct ChatChannelModeArgs {
    BOOL success;
    const char *channel;
    void *mode;
} ChatChannelModeArgs;

typedef void (*ChatEnumUsersFunc)(ChatConnection *chat, BOOL success, const char *channel, int count, ChatChannelUser **users, int *modes, void *param);

typedef struct ChatEnumUsers {
    ChatEnumUsersFunc callback;
    void *param;
} ChatEnumUsers;

// Mode of a channel (the 0x20 bytes of ChatChannel.mode).
typedef struct ChatChannelMode {
    BOOL inviteOnly;
    BOOL private;
    BOOL secret;
    BOOL moderated;
    BOOL noExternalMessages;
    BOOL onlyOpsChangeTopic;
    BOOL unk_18; // mode letter e
    int limit;   // 0 = none
} ChatChannelMode;

typedef struct ChatBytes2 {
    char bytes[2];
} ChatBytes2;

typedef struct ChatBytes3 {
    char bytes[3];
} ChatBytes3;

// chat_main.c
int ov45_0224F5BC(ChatConnection *chat, ChatMessage *message);
BOOL ov45_0224F660(ChatConnection *chat, int id);
void ov45_0224F69C(ChatConnection *chat, const char *reason);
void ov45_0224F770(ChatConnection *chat, int id);
void ov45_0224F860(ChatConnection *chat);
void ov45_0224F934(ChatConnection *chat);
void ov45_0224F9A0(ChatConnection *chat);
void ov45_0224F9B8(ChatConnection *chat);
ChatConnection *ov45_0224FABC(int loginType, const char *serverAddress, int serverPort, const char *nick, const char *user, const char *realName, int accountSpace, const char *email, const char *accountNick, const char *registeredNick, const char *password, const char *authTicket, const char *partnerKey, const char *gameName, const char *gameKey, const ChatGlobalCallbacks *callbacks, void *nickErrorCallback, void *userAddressCallback, void *connectedCallback, void *connectParam, BOOL blocking);
ChatConnection *ov45_022502D0(const char *serverAddress, int serverPort, const char *nick, const char *realName, const char *gameName, const char *gameKey, const ChatGlobalCallbacks *callbacks, void *nickErrorCallback, void *userAddressCallback, void *connectedCallback, void *connectParam, BOOL blocking);
void ov45_0225035C(ChatConnection *chat, const char *nick);
void ov45_02250424(ChatConnection *chat, int accountSpace, const char *nick, const char *cdKey);
void ov45_022504E0(ChatConnection *chat);
void ov45_02250590(ChatConnection *chat);
void ov45_022505A0(ChatConnection *chat, const char *filter, void *callbackAll, void *callbackEach, void *param, BOOL blocking);
void ov45_02250688(ChatConnection *chat, const char *channel, const char *password, const ChatChannelCallbacks *callbacks, void *callback, void *param, BOOL blocking);
void ov45_022507D8(ChatConnection *chat, const char *channel, const char *reason);
void ov45_0225089C(ChatConnection *chat, const char *channel, const char *message, u32 type);
void ov45_02250A80(ChatConnection *chat, const char *channel, const ChatChannelMode *mode);
void ov45_02250C80(ChatConnection *chat, const char *channel, void *callback, void *param, BOOL blocking);
void ov45_02250E6C(ChatConnection *chat, const char *channel, int limit);
int ov45_02250F5C(ChatConnection *chat, const char *channel);
BOOL ov45_02251024(ChatConnection *chat, const char *channel);
void ov45_022510D4(ChatConnection *chat, const char *channel, int count, ChatChannelUser **users, int *modes, ChatEnumUsers *volatile enumUsers);
void ov45_02251208(ChatConnection *chat, const char *channel, ChatEnumUsersFunc callback, void *param, BOOL blocking);
void ov45_02251350(ChatConnection *chat, const char *nick, const char *message, u32 type);
char *ov45_022514F4(void);
void ov45_02251554(ChatConnection *chat, const char *channel, const char *nick, int count, const char **keys, const char **values);
BOOL ov45_02251650(ChatConnection *chat, const char *channel, const char *nick, const char *cookie, int count, const char **keys);
void ov45_022518F0(ChatConnection *chat, const char *channel, const char *nick, int count, const char **keys, void *callback, void *param, BOOL blocking);
int ov45_02251A8C(const char *nick);
void ov45_02251B0C(ChatConnection *chat, int type, const char *nick, int unk_0C, int unk_10);

// socket code after chat_main.c
int ov45_02251EF0(ChatSocket *socket, const char *name); // name: not used
int ov45_02251F6C(ChatSocket *socket, const char *address, int port);
void ov45_022522B4(ChatSocket *socket);
void ov45_02252AA0(ChatSocket *socket);
BOOL ov45_02252CB4(ChatSocket *socket, const char *line);
BOOL ov45_02252F74(ChatSocket *socket, const char *format, ...);
ChatMessage *ov45_02253B5C(ChatSocket *socket);

// request functions defined before chat_main.c
void ov45_0224A578(ChatConnection *chat);
int ov45_0224A5E4(ChatConnection *chat);
BOOL ov45_0224A630(ChatConnection *chat, int id);
void ov45_0224A6B4(ChatConnection *chat);
int ov45_0224A7F0(ChatConnection *chat, void *callbackAll, void *callbackEach, void *param);
int ov45_0224A874(ChatConnection *chat, const char *channel, void *callback, void *param, const ChatChannelCallbacks *callbacks, const char *password);
int ov45_0224A978(ChatConnection *chat, const char *channel, ChatEnumUsersFunc callback, void *param);
int ov45_0224A9F8(ChatConnection *chat, const char *channel, void *callback, void *param);
int ov45_0224AA2C(ChatConnection *chat, const char *cookie, int count, const char **keys, BOOL allUsers, BOOL broadcastKeys, void *callback, void *param);
int ov45_0224ABA0(ChatConnection *chat, const char *cookie, int count, const char **keys, BOOL broadcastKeys, void *callback, void *param);

// OVY_0 network library functions used by chat_main.c
void ov00_021FF31C(const void *data, int length, char *out);
void ov00_021FF3C0(int milliseconds);
void ov00_021FF3CC(void);
void ov00_021FF3D0(void);

// command handler table
extern ChatServerCommand ov45_02256410[];
extern int ov45_02255DA0;

#endif // POKEHEARTGOLD_LIB_INCLUDE_DWC_CHAT_CONNECTION_H
