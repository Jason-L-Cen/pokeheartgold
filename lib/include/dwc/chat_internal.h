#ifndef POKEHEARTGOLD_LIB_INCLUDE_DWC_CHAT_INTERNAL_H
#define POKEHEARTGOLD_LIB_INCLUDE_DWC_CHAT_INTERNAL_H

// Chat client of the OVY_45 network library (src/chat_callbacks.c, src/chat_channel.c, ...): the connection
// object, the queue of pending user callbacks and the table of joined channels with their users.

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
    void *unk_00[4];
    void *nickChanged; // callback type 8: a user of the channel changed the nick
    void *unk_14[2];
    void *userModeChanged; // callback type 11: a user's mode in the channel changed
    void *unk_20[3];
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

typedef struct ChatConnection {
    u8 unk_000[0x80C];
    void *channels;         // hash table of ChatChannel
    void *enteringChannels; // array of ChatChannel records (name only) of channels being entered
    u8 unk_814[0xC];
    void *callbacks; // array of ChatCallback
} ChatConnection;

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

// OVY_0 network library functions used by the chat client
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
