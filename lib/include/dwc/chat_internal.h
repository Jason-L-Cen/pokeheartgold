#ifndef POKEHEARTGOLD_LIB_INCLUDE_DWC_CHAT_INTERNAL_H
#define POKEHEARTGOLD_LIB_INCLUDE_DWC_CHAT_INTERNAL_H

// Chat client of the OVY_45 network library (src/chat_callbacks.c, src/chat_channel.c, src/chat_cipher.c,
// ...): the connection object, the queue of pending user callbacks and the table of joined channels with their
// users.

// A user callback waiting to be called: its kind, the function, a heap copy of its arguments (an array
// of words; string arguments are duplicated) and the user data passed back to it.
typedef struct ChatCallback {
    int type; // 0 .. 31, selects the argument layout
    void *callback;
    void *param; // u32 array of the arguments
    void *user;
    int id;        // 0 = not tied to a request
    char *channel; // NULL = not tied to a channel
} ChatCallback;

// The callback signatures by number of word arguments (connection first, user data last).
typedef void (*ChatCallbackFunc1)(void *, u32, void *);
typedef void (*ChatCallbackFunc2)(void *, u32, u32, void *);
typedef void (*ChatCallbackFunc3)(void *, u32, u32, u32, void *);
typedef void (*ChatCallbackFunc4)(void *, u32, u32, u32, u32, void *);
typedef void (*ChatCallbackFunc5)(void *, u32, u32, u32, u32, u32, void *);
typedef void (*ChatCallbackFunc6)(void *, u32, u32, u32, u32, u32, u32, void *);
typedef void (*ChatCallbackFunc7)(void *, u32, u32, u32, u32, u32, u32, u32, void *);

// Success flag of some chat functions (an enum, not BOOL: the conditional selects are emitted in a
// different order for an int result).
typedef enum ChatResult {
    CHAT_FAILURE,
    CHAT_SUCCESS,
} ChatResult;

// Callbacks the user registered for one channel (copied into the channel when it is entered).
typedef struct ChatChannelCallbacks {
    void *message;      // callback type 4: a message to the channel (ChatChannelMessageArgs)
    void *kicked;       // callback type 5: the own user was kicked (ChatKickedArgs)
    void *userJoined;   // callback type 6: a user entered the channel (ChatUserJoinedArgs)
    void *userLeft;     // callback type 7: a user left the channel (ChatUserLeftArgs)
    void *nickChanged;  // callback type 8: a user of the channel changed the nick
    void *topicChanged; // callback type 9: the topic changed (ChatTopicArgs)
    void *unk_18;
    void *userModeChanged; // callback type 11: a user's mode in the channel changed
    void *usersChanged;    // callback type 12: the user list changed (argument: the channel)
    void *unk_24[2];
    void *param; // user data passed to the callbacks
} ChatChannelCallbacks;

// A user in a channel (element of ChatChannel.users, keyed by the nick).
typedef struct ChatChannelUser {
    char nick[128];
    char user[24];
    char address[64];
    BOOL hasUserInfo; // user and address are known
    int mode;         // 0 .. 3
} ChatChannelUser;

// A joined channel (element of ChatConnection.channels, keyed by the name).
typedef struct ChatChannel {
    char name[257];
    u8 unk_101[3];
    ChatChannelCallbacks callbacks;
    void *users; // hash table of ChatChannelUser
    u8 mode[32];
    BOOL hasMode; // mode is valid
    char *topic;
    BOOL joined;
    char key[128];
} ChatChannel;

// State of the connection's stream cipher (chat_cipher.c).
typedef struct ChatCipher {
    u8 state[256];
    u8 x;
    u8 y;
} ChatCipher;

#include "dwc/chat_connection.h"

// The users of a channel, collected for a ChatUserListFunc.
typedef struct ChatUserList {
    ChatConnection *chat;
    int count;
    ChatChannelUser **users;
    int *modes;
} ChatUserList;

typedef void (*ChatUserListFunc)(ChatConnection *chat, const char *channel, int count, ChatChannelUser **users, int *modes, int param);
typedef void (*ChatUserSearchFunc)(ChatConnection *chat, const char *nick, ChatChannel *channel, void *param);

// Search of one nick in all channels.
typedef struct ChatUserSearch {
    ChatConnection *chat;
    char *nick;
    ChatUserSearchFunc callback;
    void *param;
    char buffer[sizeof(ChatChannelUser)];
} ChatUserSearch;

typedef struct ChatNickChange {
    ChatConnection *chat;
    const char *oldNick;
    const char *newNick;
} ChatNickChange;

// Arguments of the nick changed callback.
typedef struct ChatNickChangeArgs {
    const char *channel;
    const char *oldNick;
    const char *newNick;
} ChatNickChangeArgs;

// Arguments of the user mode changed callback.
typedef struct ChatUserModeArgs {
    const char *channel;
    const char *nick;
    int mode;
} ChatUserModeArgs;

typedef struct ChatUserInfo {
    const char *nick;
    const char *user;
    const char *address;
} ChatUserInfo;

// chat_callbacks.c
void ov45_02244500(ChatCallback *callback);
ChatResult ov45_02244B9C(ChatConnection *chat);
void ov45_02244BCC(ChatConnection *chat);
BOOL ov45_02244D30(ChatConnection *chat, int type, void *func, u32 *src, void *user, int id, char *channel, int size);
void ov45_02246BA4(ChatConnection *chat, ChatCallback *callback);
void ov45_022471A0(ChatConnection *chat, int id);
int ov45_0224739C(ChatConnection *chat, int id);
ChatResult ov45_022474FC(ChatConnection *chat, int id);

// Defined before chat_callbacks.c: frees one queued callback (array element destructor).
void ov45_022444C8(void *element);

// chat_channel.c
int ov45_02247518(const signed char *s, int numBuckets);
ChatChannel *ov45_022475B8(ChatConnection *chat, const char *channel);
int ov45_022475F0(const void *element, int numBuckets);
int ov45_02247650(const void *first, const void *second);
void ov45_02247770(void *element);
int ov45_022477B8(const void *element, int numBuckets);
int ov45_02247818(const void *first, const void *second);
void ov45_02247934(void *element);
BOOL ov45_02247968(ChatConnection *chat);
void ov45_022479F0(ChatConnection *chat);
void ov45_02247A44(ChatConnection *chat, const char *channel);
int ov45_02247AD4(const void *a, const void *b);
BOOL ov45_02247AE0(ChatConnection *chat, const char *channel);
void ov45_02247B94(ChatConnection *chat, const char *name, const ChatChannelCallbacks *callbacks);
void ov45_02247D84(ChatConnection *chat, const char *name);
ChatChannelCallbacks *ov45_02247E98(ChatConnection *chat, const char *name);
void ov45_02247F60(void *element, void *arg);
void ov45_022480EC(ChatConnection *chat, const char *name, ChatUserListFunc callback, int param);
BOOL ov45_02248238(ChatConnection *chat, const char *name);
BOOL ov45_02248288(ChatConnection *chat, const char *name, void *mode);
void ov45_02248374(ChatConnection *chat, const char *name, const void *mode);
void ov45_02248454(ChatConnection *chat, const char *name, const char *topic);
void ov45_02248558(ChatConnection *chat, const char *name);
BOOL ov45_0224861C(ChatConnection *chat, const char *name);
void ov45_022486E0(ChatConnection *chat, const char *name, const char *key);
int ov45_022487BC(ChatConnection *chat, const char *name);
void ov45_02248888(ChatConnection *chat, const char *nick, const char *name, int mode, const char *user, const char *address);
void ov45_02248A98(ChatConnection *chat, const char *nick, const char *name);
void ov45_02248BCC(void *element, void *arg);
void ov45_02248CE8(ChatConnection *chat, const char *nick, ChatUserSearchFunc callback, void *param);
void ov45_02248E00(void *element, void *arg);
void ov45_02249054(ChatConnection *chat, const char *oldNick, const char *newNick);
void ov45_02249194(ChatConnection *chat, const char *nick, const char *name, int mode, BOOL set);
void ov45_02249390(void *element, void *arg);
void ov45_02249460(ChatConnection *chat, const char *nick, const char *user, const char *address);

// chat_cipher.c
void ov45_02249554(const u8 *key, int keyLength, ChatCipher *cipher);
void ov45_022495DC(u8 *data, int length, ChatCipher *cipher);
void ov45_0224966C(s8 *data, int length, const s8 *key);

#define CHAT_REQUEST_TYPE_COUNT 17

// A request sent to the server whose answer is still awaited (element of ChatConnection.requests).
struct ChatRequest {
    int type;       // 0 .. 16, selects data and the callback type (ov45_02249A0C)
    u32 expireTime; // ov00_021FF368 time after which the request is ended without an answer
    char *name;     // e.g. the channel or the nick
    char *name2;
    void *callback;
    void *callback2; // request type 0
    void *param;     // user data passed to the callback
    void *data;      // ChatRequestData, results collected so far
    int id;
    ChatRequest *next;
};

// Key of ov45_022496AC.
typedef struct ChatRequestKey {
    int type;
    const char *name;
    const char *name2;
} ChatRequestKey;

// Request type 0: the channels collected so far.
typedef struct ChatChannelListData {
    BOOL unk_00;
    int count;
    char **names;
    int *numUsers;
    char **topics;
} ChatChannelListData;

// Request type 1: entering a channel.
typedef struct ChatEnterData {
    ChatChannelCallbacks callbacks;
    BOOL joined; // the JOIN of the own user was received
    char password[32];
} ChatEnterData;

// Request types 3 and 15: the nicks collected so far.
typedef struct ChatNickListData {
    int capacity; // size of nicks and modes
    int count;
    char **nicks;
    int *modes;
} ChatNickListData;

// Request type 4.
typedef struct ChatStringsData {
    char *unk_00;
    char *unk_04;
    char *unk_08;
    int count;
    char **strings;
} ChatStringsData;

// Request types 8, 12, 13 and 14: keys.
typedef struct ChatKeysData {
    int count;
    char **keys;

    union {
        char *nick;         // type 12
        BOOL allUsers;      // type 13
        BOOL broadcastKeys; // type 14
    } arg;

    BOOL broadcastKeys; // type 13
} ChatKeysData;

typedef union ChatRequestData {
    ChatChannelListData channels;
    ChatEnterData enter;
    ChatNickListData nicks;
    ChatStringsData strings;
    ChatKeysData keys;
} ChatRequestData;

// Channel mode letters (ov45_0224AD24).
enum ChatModeType {
    CHAT_MODE_END = 0,
    CHAT_MODE_BAN,                   // b
    CHAT_MODE_INVITE_ONLY,           // i
    CHAT_MODE_LIMIT,                 // l
    CHAT_MODE_PRIVATE,               // p
    CHAT_MODE_SECRET,                // s
    CHAT_MODE_KEY,                   // k
    CHAT_MODE_MODERATED,             // m
    CHAT_MODE_NO_EXTERNAL_MESSAGES,  // n
    CHAT_MODE_ONLY_OPS_CHANGE_TOPIC, // t
    CHAT_MODE_OP,                    // o
    CHAT_MODE_VOICE,                 // v
    CHAT_MODE_U,                     // u
    CHAT_MODE_W,                     // w
    CHAT_MODE_E,                     // e
};

// One change of a parsed channel mode string.
typedef struct ChatModeChange {
    int type; // ChatModeType
    BOOL set;
    const char *arg; // letters b, k, l, o and v
} ChatModeChange;

// chat_requests.c
ChatRequest *ov45_022496AC(ChatConnection *chat, int count, const ChatRequestKey *keys);
ChatRequest *ov45_0224983C(ChatConnection *chat, const char *nick);
void ov45_02249904(ChatRequest *request);
void ov45_02249958(ChatConnection *chat, ChatRequest *target);
void ov45_02249A0C(ChatConnection *chat, ChatRequest *request, u32 *results);
void ov45_0224A14C(ChatConnection *chat, ChatRequest *request);
int ov45_0224A708(ChatConnection *chat, int type, const char *name, const char *name2, void *callback, void *callback2, void *param, void *data);
ChatModeChange *ov45_0224AD24(const char *modes, const char **args, int argCount);
void ov45_0224AFD8(ChatChannelMode *mode, ChatModeChange *changes);

// Message types of the message callbacks (server commands PRIVMSG, CTCP ACTION, NOTICE, UTM, ATM).
enum ChatMessageType {
    CHAT_MESSAGE_NORMAL,
    CHAT_MESSAGE_ACTION,
    CHAT_MESSAGE_NOTICE,
    CHAT_MESSAGE_UTM,
    CHAT_MESSAGE_ATM,
};

// Why a user left a channel (ChatUserLeftArgs.reason: server commands PART, QUIT, KICK, KILL).
enum ChatUserLeftReason {
    CHAT_LEFT_PART,
    CHAT_LEFT_QUIT,
    CHAT_LEFT_KICK,
    CHAT_LEFT_KILL,
};

// Arguments of the private message callback (ChatGlobalCallbacks.privateMessage).
typedef struct ChatPrivateMessageArgs {
    const char *nick; // NULL for a message from the server
    const char *message;
    int type; // ChatMessageType
} ChatPrivateMessageArgs;

// Arguments of the invited callback (ChatGlobalCallbacks.unk_0C, callback type 3).
typedef struct ChatInviteArgs {
    const char *channel;
    const char *nick;
} ChatInviteArgs;

// Arguments of the kicked callback (ChatChannelCallbacks.kicked).
typedef struct ChatKickedArgs {
    const char *channel;
    const char *kicker;
    const char *reason;
} ChatKickedArgs;

// Arguments of the user joined callback (ChatChannelCallbacks.userJoined).
typedef struct ChatUserJoinedArgs {
    const char *channel;
    const char *nick;
    int mode; // 0 .. 2
} ChatUserJoinedArgs;

// Arguments of the user left callback (ChatChannelCallbacks.userLeft).
typedef struct ChatUserLeftArgs {
    const char *channel;
    const char *nick;
    int reason;          // ChatUserLeftReason
    const char *message; // part / quit message or reason
    const char *kicker;  // CHAT_LEFT_KICK
} ChatUserLeftArgs;

// Arguments of the topic changed callback (ChatChannelCallbacks.topicChanged).
typedef struct ChatTopicArgs {
    const char *channel;
    const char *topic;
} ChatTopicArgs;

// chat_handlers.c (server line handlers, table ov45_02256410)
void ov45_0224B0FC(ChatConnection *chat, ChatMessage *message);
void ov45_0224B310(ChatConnection *chat, ChatMessage *message);
void ov45_0224B474(ChatConnection *chat, ChatMessage *message);
void ov45_0224B5E0(ChatConnection *chat, ChatMessage *message);
void ov45_0224B748(ChatConnection *chat, ChatMessage *message);
void ov45_0224B794(ChatConnection *chat, ChatMessage *message);
void ov45_0224B8D0(ChatConnection *chat, ChatMessage *message);
void ov45_0224BB4C(ChatConnection *chat, ChatMessage *message);
void ov45_0224BC8C(ChatConnection *chat, ChatMessage *message);
void ov45_0224BE74(ChatConnection *chat, const char *nick, ChatChannel *channel, void *param);
void ov45_0224C018(ChatConnection *chat, ChatMessage *message);
void ov45_0224C0A0(ChatConnection *chat, const char *nick, ChatChannel *channel, void *param);
void ov45_0224C244(ChatConnection *chat, ChatMessage *message);
void ov45_0224C2A8(ChatConnection *chat, ChatMessage *message);
void ov45_0224C388(ChatConnection *chat, ChatMessage *message);
void ov45_0224C594(ChatConnection *chat, ChatMessage *message);
void ov45_0224C610(ChatConnection *chat, ChatMessage *message);
void ov45_0224C6DC(ChatConnection *chat, ChatMessage *message);
void ov45_0224C9D8(ChatConnection *chat, ChatMessage *message);

// OVY_0 network library functions used by the chat client
u32 ov00_021FF368(void);
void *ov00_021EC3A8(int size);
void *ov00_021EC3BC(void *ptr, int size);
void ov00_021EC3D8(void *ptr);
void *ov00_021FE5B4(int elemSize, int count, void (*freeElement)(void *));
void ov00_021FE678(void *array);
void *ov00_021FE6EC(void *array, int i);
void ov00_021FE754(void *array, void *elem);
void ov00_021FE8D8(void *array, int i);
void ov00_021FE968(void *array, int i);
int ov00_021FEA40(void *array, const void *key, int (*compare)(const void *, const void *), int start, int sorted);
void *ov00_021FED4C(int elemSize, int numBuckets, int unused, int (*hash)(const void *, int), int (*compare)(const void *, const void *), void (*freeElement)(void *));
void ov00_021FEE8C(void *table);
int ov00_021FEF00(void *table);
void ov00_021FEF74(void *table, void *elem);
int ov00_021FF01C(void *table, void *elem);
void *ov00_021FF0C0(void *table, const void *key);
void ov00_021FF160(void *table, void (*func)(void *element, void *arg), void *arg);
int ArrayLength(void *array);
char *ov00_021FF3D4(const char *str);

// MSL functions without a header here
int strnicmp(const char *a, const char *b);
extern u8 __lower_mapC[];

#endif // POKEHEARTGOLD_LIB_INCLUDE_DWC_CHAT_INTERNAL_H
