#ifndef POKEHEARTGOLD_LIB_INCLUDE_DWC_PERSIST_INTERNAL_H
#define POKEHEARTGOLD_LIB_INCLUDE_DWC_PERSIST_INTERNAL_H

// Named value buckets and the persistent data client of OVY_0 (lib/NitroDWC/src/persist_data.c).

typedef enum BucketType {
    BUCKET_INT,
    BUCKET_DOUBLE,
    BUCKET_STRING,
} BucketType;

// One named value ("gbucket.c").
typedef struct Bucket {
    const char *name;
    int type;  // BucketType
    int count; // number of values averaged in

    union {
        int i;
        double d;
        char *s;
    } val;
} Bucket;

// A hash table of buckets, looked up by name.
typedef struct BucketSet {
    void *table;
} BucketSet;

// ov00_0221B1F0: the values returned by pointer, and the default set.
typedef struct BucketGlobals {
    int intValue;          // 0x0
    BucketSet *defaultSet; // 0x4
    double doubleValue;    // 0x8
} BucketGlobals;

// ov00_022198D8: the persistent data server socket.
typedef struct PersistSocket {
    int socket;      // -1 when closed
    int port;        // 0x4
    const char *key; // 0x8, key of the XOR scrambling
} PersistSocket;

// A pending persistent data request: the reply is matched on type, localId and profileId.
typedef struct PersistRequest {
    int type; // 0 = authentication, 1 = get data, 2 = set data, 3 = get profile id
    int localId;
    int profileId;
    int persistType;
    int index;
    int param;      // passed back to the callback
    void *callback; // signature depends on type
} PersistRequest;

// A game whose values are kept in a bucket set.
typedef struct StatsGame {
    u8 unk00[0xC];
    BucketSet *buckets; // 0x0C
    u8 unk10[0xC];
    void *players; // 0x1C, DArray of player numbers
    void *teams;   // 0x20, DArray of team numbers
} StatsGame;

// ov00_0221B200: persistent data client state.
typedef struct PersistWork {
    int recvLen;    // 0x00
    void *requests; // 0x04, DArray of PersistRequest
    int unk08;      // 0x08
    char *recvBuf;  // 0x0C
    int state;      // 0x10, 5 = connected
    u8 unk14[0x10];
    int valueIndex;         // 0x24, alternates between the two value buffers
    StatsGame *currentGame; // 0x28, used when no game is given
    int recvCap;            // 0x2C
    char values[2][0x100];  // 0x30
} PersistWork;

typedef void *(*BucketGetter)(void *buckets, const char *name, void *value);
typedef char *(*BucketStringGetter)(void *buckets, const char *name, char *value);

// persist_data.c
void *ov00_0220A8A4(BucketSet *set, const char *name, int type, void *value);
void *ov00_0220A940(BucketSet *set, const char *name, void *value);
void *ov00_0220A96C(BucketSet *set, const char *name, void *value);
void *ov00_0220AA00(BucketSet *set, const char *name, void *value);
void *ov00_0220AA84(BucketSet *set, const char *name, void *value);
void *ov00_0220AB04(BucketSet *set, const char *name, void *value);
void *ov00_0220AB88(BucketSet *set, const char *name, void *value);
void *ov00_0220AC2C(BucketSet *set, const char *name, void *value);
int *ov00_0220AD00(int value);
double *ov00_0220AD18(double value);
char *ov00_0220AD34(char *s);
void *ov00_0220AD64(Bucket *bucket, void *value);
void *ov00_0220ADE0(Bucket *bucket);
Bucket *ov00_0220AE00(BucketSet *set, const char *name);
void ov00_0220AE60(void);
int ov00_0220AED0(void);
int ov00_0220AEF0(void);
void ov00_0220B040(char *buf, int len);
char *ov00_0220B088(const char *str, const char *key);
char *ov00_0220B15C(const char *str, const char *key);
int ov00_0220B174(int socket);
char *ov00_0220B180(char *buf, int len);
int ov00_0220B1E8(int type, int localId, int profileId);
void ov00_0220B26C(char *buf, int len);
void ov00_0220B31C(char *buf, int len);
void ov00_0220B3A8(char *buf, int len);
void ov00_0220B48C(char *buf, int len);
void ov00_0220B524(char *buf, int len);
int ov00_0220B60C(char *buf, int len);
void ov00_0220B694(int index, int success, int modified, char *data, int len);
void ov00_0220B798(void);
int ov00_0220B88C(StatsGame *game, int index);
int ov00_0220B8BC(StatsGame *game, int index);
int ov00_0220B8EC(StatsGame *game, const char *name, BucketGetter get, int value, ...);
double ov00_0220B954(StatsGame *game, const char *name, BucketGetter get, double value, ...);
char *ov00_0220B9C4(StatsGame *game, const char *name, BucketStringGetter get, char *value, int unused);
int ov00_0220BA20(StatsGame *game, const char *name, BucketGetter get, int value, int index);
double ov00_0220BA7C(StatsGame *game, const char *name, BucketGetter get, double value, int index);
char *ov00_0220BADC(StatsGame *game, const char *name, BucketStringGetter get, char *value, int index);
int ov00_0220BB38(StatsGame *game, const char *name, BucketGetter get, int value, int index);
double ov00_0220BB94(StatsGame *game, const char *name, BucketGetter get, double value, int index);
char *ov00_0220BBF4(StatsGame *game, const char *name, BucketStringGetter get, char *value, int index);

#endif // POKEHEARTGOLD_LIB_INCLUDE_DWC_PERSIST_INTERNAL_H
