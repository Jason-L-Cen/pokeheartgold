#ifndef POKEHEARTGOLD_LIB_INCLUDE_DWC_HTTP_INTERNAL_H
#define POKEHEARTGOLD_LIB_INCLUDE_DWC_HTTP_INTERNAL_H

// HTTP client of OVY_38 (lib/NitroDWC/src/http_connection.c, http_request.c): connection slots,
// growable text buffers, request/response state machine and POST bodies.

typedef struct GHIConnection GHIConnection;

typedef struct GHIBuffer {
    GHIConnection *connection;
    char *data;
    int size;
    int len;
    int pos;
    int sizeIncrement;
    int fixed;    // user-supplied storage, never grows
    int dontFree; // data is not owned by the buffer
    int encrypted;
} GHIBuffer;

typedef struct GHIEncryptor {
    int unk_00;
    int engine; // 0 = plain HTTP
    int initialized;
    int sessionReady;
    int (*initFunc)(GHIConnection *, struct GHIEncryptor *);
    void (*cleanupFunc)(GHIConnection *, struct GHIEncryptor *);
    int (*encryptFunc)(GHIConnection *, struct GHIEncryptor *, const char *, int *, char *, int *);
    int (*decryptFunc)(GHIConnection *, struct GHIEncryptor *, const char *, int *, char *, int *);
} GHIEncryptor;

enum {
    GHI_POST_DATA_STRING = 0,
    GHI_POST_DATA_FILE_DISK = 1,
    GHI_POST_DATA_FILE_MEMORY = 2,
};

// One field of a POST body.
typedef struct GHIPostData {
    int type;
    char *name;
    union {
        struct {
            char *data;
            int len;
            int encode;     // url-encode unless the body is multipart
            int numEscaped; // characters that become %XX
        } string;
        struct {
            void *unk_08;
            char *fileName;
            char *contentType;
        } disk;
        struct {
            char *data;
            int len;
            char *fileName;
            char *contentType;
        } memory;
    } u;
} GHIPostData;

typedef struct GHIPostState {
    GHIPostData *data;
    int pos;
    void *file;
    int len;
} GHIPostState;

typedef struct GHIPost {
    void *data; // array of GHIPostData
    void *callback;
    void *param;
    int hasFiles; // multipart/form-data body
    int autoFree;
} GHIPost;

typedef int (*CompletedCallback)(int, int, char *, int, void *);
typedef void (*ProgressCallback)(int, int, const char *, int, int, int, void *);
typedef void (*PostCallback)(int, int, int, int, int, void *);

enum {
    GHI_STATE_HOST_LOOKUP = 0,
    GHI_STATE_CONNECTING = 1,
    GHI_STATE_SECURING_SESSION = 2,
    GHI_STATE_SENDING_REQUEST = 3,
    GHI_STATE_POSTING = 4,
    GHI_STATE_WAITING = 5,
    GHI_STATE_RECEIVING_STATUS = 6,
    GHI_STATE_RECEIVING_HEADERS = 7,
    GHI_STATE_RECEIVING_FILE = 8,
};

enum {
    GHI_CHUNK_HEADER = 0,
    GHI_CHUNK_DATA = 1,
    GHI_CHUNK_END = 2,
    GHI_CHUNK_DONE = 3,
};

struct GHIConnection {
    int inUse;                           // 0x00
    int request;                         // 0x04
    int uniqueID;                        // 0x08
    int type;                            // 0x0C, 3 = HEAD
    int state;                           // 0x10
    char *URL;                           // 0x14
    char *serverAddress;                 // 0x18
    int serverIP;                        // 0x1C
    u16 serverPort;                      // 0x20
    char *requestPath;                   // 0x24
    char *sendHeaders;                   // 0x28
    int unk_2C;                          // 0x2C
    int blocking;                        // 0x30
    int keepAlive;                       // 0x34
    int result;                          // 0x38
    ProgressCallback progressCallback;   // 0x3C
    CompletedCallback completedCallback; // 0x40
    void *callbackParam;                 // 0x44
    int socket;                          // 0x48
    int socketError;                     // 0x4C
    GHIBuffer sendBuffer;                // 0x50
    GHIBuffer encodeBuffer;              // 0x74, decoded response text
    GHIBuffer recvBuffer;                // 0x98
    GHIBuffer getFileBuffer;             // 0xBC
    int userBufferSupplied;              // 0xE0
    int statusMajor;                     // 0xE4
    int statusMinor;                     // 0xE8
    int statusCode;                      // 0xEC
    int statusStringIndex;               // 0xF0
    int headerIndex;                     // 0xF4
    int headerLength;                    // 0xF8
    int completed;                       // 0xFC
    int fileBytesReceived;               // 0x100
    int totalSize;                       // 0x104
    char *redirectURL;                   // 0x108
    int redirectCount;                   // 0x10C
    int chunked;                         // 0x110
    char chunkHeader[12];                // 0x114
    int chunkHeaderLen;                  // 0x120
    int chunkBytesLeft;                  // 0x124
    int chunkReadingState;               // 0x128
    int processing;                      // 0x12C
    int connectionClosed;                // 0x130
    int throttle;                        // 0x134
    int lastThrottleRecv;                // 0x138
    GHIPost *post;                       // 0x13C
    void *states;                        // 0x140
    int index;                           // 0x144
    int bytesPosted;                     // 0x148
    int totalBytes;                      // 0x14C
    PostCallback postCallback;           // 0x150
    void *postParam;                     // 0x154
    int maxRecvTime;                     // 0x158, ms per ghiProcessConnection call
    char *proxyAddress;                  // 0x15C
    u16 proxyPort;                       // 0x160
    GHIEncryptor encryptor;              // 0x164
};

typedef struct GHIConnList {
    int count;
    GHIConnection **list;
    int nextUniqueID;
    int capacity;
} GHIConnList;

#define HTTP_ASSERT(expr, exprStr, file, func, line) \
    if (!(expr))                                     \
    __msl_assertion_failed(exprStr, file, func, line)

// MSL_C functions without a header in lib/include
void __msl_assertion_failed(const char *expr, const char *file, const char *func, int line);
char *strchr(const char *str, int c);
int strncmp(const char *a, const char *b, size_t n);
long atol(const char *str);
int sprintf(char *buf, const char *fmt, ...);
int sscanf(const char *str, const char *fmt, ...);
int fseek(void *file, long offset, int whence);
long ftell(void *file);
void rewind(void *file);
int fclose(void *file);
unsigned fread(void *buf, unsigned size, unsigned count, void *file);
extern const u16 __msl_digit[];

// OVY_0 network library functions used by the client
void *ov00_021EC3A8(int size);
void *ov00_021EC3BC(void *ptr, int size);
void ov00_021EC3D8(void *ptr);
void *ov00_021FE5B4(int elemSize, int count, int unused);
void ov00_021FE678(void *array);
void *ov00_021FE6EC(void *array, int i);
void ov00_021FE754(void *array, void *elem);
int ArrayLength(void *array);
u32 ov00_021FF368(void);
void ov00_021FF3C0(int ms);
void ov00_021FF3CC(void);
char *ov00_021FF3D4(const char *str);
int ov00_021FF44C(int socket, int blocking);
void ov00_021FF48C(int socket, int size);
int ov00_021FF59C(int socket, int *readable, int *writable, int *except);
int ov00_021FF804(int domain, int type, int protocol);
void ov00_021FF818(int socket);
void ov00_021FF82C(int socket, int how);
int ov00_021FF92C(int socket, char *buf, int len, int flags);
int ov00_021FF970(int socket, const char *buf, int len, int flags);
int gsSocketInetAddr(const char *address);
int gsSocketGetLastError(int socket);

// http_connection.c
int ghiResizeBuffer(GHIBuffer *buffer, int sizeIncrement);
int ghiInitBuffer(GHIConnection *connection, GHIBuffer *buffer, int initialSize, int sizeIncrement);
int ghiInitFixedBuffer(GHIConnection *connection, GHIBuffer *buffer, char *userBuffer, int size);
void ghiFreeBuffer(GHIBuffer *buffer);
int ghiAppendDataToBuffer(GHIBuffer *buffer, const char *data, int len);
int ov38_0221C5EC(GHIBuffer *buffer, const char *name, const char *value);
int ghiAppendCharToBuffer(GHIBuffer *buffer, int c);
int ov38_0221C6CC(GHIBuffer *buffer, int i);
void ghiResetBuffer(GHIBuffer *buffer);
int ov38_0221C748(GHIConnection *c);
int ghiReadDataFromBuffer(GHIBuffer *buffer, char *out, int *len);
void ghiCallCompletedCallback(GHIConnection *c);
void ghiCallProgressCallback(GHIConnection *c, const char *buffer, int len);
void ghiCallPostCallback(GHIConnection *c);
void ov38_0221CA44(void);
void ov38_0221CA48(void);
void ov38_0221CA4C(void);
void ov38_0221CA50(void);
int ov38_0221CA54(GHIConnection *c);
int ov38_0221CB7C(GHIConnection *c, char *buffer, int *bufferLen);
int ov38_0221CD90(GHIConnection *c, const char *buf, int len);
int ov38_0221CE0C(GHIConnection *c, const char *data, int len);
int ghiFindFreeSlot(void);
GHIConnection *ov38_0221CF8C(void);
int ghiFreeConnection(GHIConnection *c);
GHIConnection *ghiRequestToConnection(int request);
void ov38_0221D3B0(void (*func)(GHIConnection *));
void ov38_0221D414(GHIConnection *c);
void ov38_0221D518(void);
void ov38_0221D590(GHIConnection *c);
int ghiProcessConnection(GHIConnection *c);
void ov38_0221D7B4(void);
void ov38_0221D808(void);
int ov38_0221D864(const char *url, int blocking, CompletedCallback completed, void *param);
int ghttpGetExA(const char *url, const char *headers, char *buffer, int bufferSize, GHIPost *post, int throttle, int blocking, ProgressCallback progress, CompletedCallback completed, void *param);
void ov38_0221DAEC(void);
int ov38_0221DB00(GHIPost *post);
void ov38_0221DB08(GHIPost *post);
char *ghiPostGetContentType(GHIConnection *c);
int ghiPostGetNoFilesContentLength(GHIConnection *c);
int ghiPostGetHasFilesContentLength(GHIConnection *c);
int ghiPostGetContentLength(GHIConnection *c);
int ghiPostStateInit(GHIPostState *s);
void ghiPostStateCleanup(GHIPostState *s);
int ghiPostInitState(GHIConnection *c);
void ov38_0221E0DC(GHIConnection *c);
int ghiPostStringStateDoPosting(GHIPostState *state, GHIConnection *c);
int ghiPostFileDiskStateDoPosting(GHIPostState *state, GHIConnection *c);
int ghiPostFileMemoryStateDoPosting(GHIPostState *state, GHIConnection *c);
int ghiPostStateDoPosting(GHIPostState *state, GHIConnection *c, int first);
int ghiPostDoPosting(GHIConnection *c);

// http_request.c
int ghiParseURL(GHIConnection *c);
void ov38_0221EBB0(GHIConnection *c);
void ov38_0221EC74(GHIConnection *c);
void ghiDoSecuringSession(GHIConnection *c);
void ov38_0221F024(GHIConnection *c);
void ghiDoPosting(GHIConnection *c);
void ov38_0221F330(GHIConnection *c);
void ov38_0221F4FC(GHIConnection *c);
int ghiParseStatus(GHIConnection *c);
int ov38_0221F624(GHIConnection *c, const char *data, int len);
int ghiParseChunkSize(GHIConnection *c);
void ghiAppendToChunkHeaderBuffer(GHIConnection *c, const char *data, int len);
int ghiProcessIncomingFileData(GHIConnection *c, char *data, int len);
void ov38_0221FA38(GHIConnection *c);
void ov38_0221FEA8(GHIConnection *c);

#endif // POKEHEARTGOLD_LIB_INCLUDE_DWC_HTTP_INTERNAL_H
