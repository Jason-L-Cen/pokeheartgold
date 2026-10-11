#ifndef POKEHEARTGOLD_LIB_INCLUDE_DWC_HTTP_SOCKET_INTERNAL_H
#define POKEHEARTGOLD_LIB_INCLUDE_DWC_HTTP_SOCKET_INTERNAL_H

// Socket HTTP client of OVY_38 (lib/NitroDWC/src/http_socket_queue.c, http_socket_request.c,
// http_socket_worker.c): requests are queued as jobs and processed one at a time by a worker
// thread that resolves the host, connects (optionally over SSL), sends the request line, headers
// and POST body, then receives the response headers and body.

enum {
    HTTP_SOCKET_METHOD_GET = 0,
    HTTP_SOCKET_METHOD_POST = 1,
    HTTP_SOCKET_METHOD_HEAD = 2,
};

// Node of a circular doubly linked header or POST field list; the list pointer is its first node.
typedef struct HttpField {
    struct HttpField *prev; // 0x00, the last node for the first one
    struct HttpField *next; // 0x04
    const char *name;       // 0x08
    const char *value;      // 0x0C
    int length;             // 0x10, POST fields: length of value
    int isFile;             // 0x14, POST fields: sent as a multipart file part
} HttpField;

// One block of the response header storage after the first 1024 bytes.
typedef struct HttpResponseChunk {
    struct HttpResponseChunk *next; // 0x00
    char bytes[512];                // 0x04
} HttpResponseChunk;

typedef struct HttpResponse {
    volatile int headerLength; // 0x00, bytes of response header stored in bytes/chunks
    volatile int bodyLength;   // 0x04, bytes of body stored in body
    int success;               // 0x08
    int bodyCapacity;          // 0x0C
    void *unk_10;              // 0x10
    char *headerValue;         // 0x14, copy returned by ov38_02222F88
    char *body;                // 0x18, caller-supplied storage
    HttpResponseChunk *chunks; // 0x1C
    char bytes[1024];          // 0x20
} HttpResponse;

typedef struct HttpSslConnection {
    u8 unk_000[0x800];
    char *host; // 0x800
    u8 unk_804[0xC];
    int (*unk_810)(int); // 0x810
    int unk_814;         // 0x814
    int unk_818;         // 0x818
    u8 unk_81C[0x14];
} HttpSslConnection;

typedef void (*HttpSocketCallback)(int error, HttpResponse *response, void *param);

typedef struct HttpRequest {
    int queued;                  // 0x00, set once the request is queued; fields are fixed then
    int cancel;                  // 0x04
    int secure;                  // 0x08, https://
    int hostEnd;                 // 0x0C, offset of the end of the host name in url
    int pathStart;               // 0x10, offset of the path in url
    int contentLength;           // 0x14, -1 when the response has no Content-Length
    int unk_18;                  // 0x18
    int unk_1C;                  // 0x1C
    HttpSslConnection *ssl;      // 0x20
    char *url;                   // 0x24, without %-escapes in the host part
    int port;                    // 0x28
    void *param;                 // 0x2C
    HttpResponse *response;      // 0x30
    HttpField *headers;          // 0x34
    HttpField *posts;            // 0x38
    HttpSocketCallback callback; // 0x3C
    int method;                  // 0x40, HTTP_SOCKET_METHOD_*
    char boundary[20];           // 0x44, "--" + multipart boundary, changed until no POST value contains it
} HttpRequest;

// Node of the circular job queue (ov38_02226230) and the job being processed (ov38_02226220).
typedef struct HttpJob {
    struct HttpJob *prev;          // 0x00
    struct HttpJob *next;          // 0x04
    int id;                        // 0x08
    HttpRequest *volatile request; // 0x0C
    int socket;                    // 0x10
} HttpJob;

// Client state at ov38_02226210; the code also addresses most fields through their own labels.
typedef struct HttpSocketWork {
    int running;                             // 0x00
    int error;                               // 0x04, ov38_02226214
    void *(*allocFunc)(int size, int align); // 0x08, ov38_02226218
    void (*freeFunc)(void *ptr);             // 0x0C, ov38_0222621C
    HttpJob *currentJob;                     // 0x10, ov38_02226220
    int quit;                                // 0x14, ov38_02226224
    u8 *threadStack;                         // 0x18
    int nextJobId;                           // 0x1C, ov38_0222622C
    HttpJob *jobs;                           // 0x20, ov38_02226230
} HttpSocketWork;

extern HttpSocketWork ov38_02226210;
extern int ov38_02226214;
extern void *(*ov38_02226218)(int size, int align);
extern void (*ov38_0222621C)(void *ptr);
extern HttpJob *ov38_02226220;
extern int ov38_02226224;
extern volatile int ov38_0222622C;
extern HttpJob *ov38_02226230;

// http_engine_init.c
int ov38_022201A0(int encodedLen);
int ov38_022201AC(int len);
int ov38_022201C8(char *out, const char *in, int len);
int ov38_022202C0(char *out, const char *in, int len);

// http_socket_queue.c
int ov38_022219FC(void *(*allocFunc)(int size, int align), void (*freeFunc)(void *ptr), int priority);
void ov38_02221AC4(void (*callback)(void));
void ov38_02221B0C(void *callback);
int ov38_02221B48(HttpField **list, const char *name, const char *value, int nodeSize);
HttpField *ov38_02221C60(HttpField **list);
int ov38_02221CA4(u8 ch);
int ov38_02221CD8(char *request, const char *value, int length);
int ov38_02221D78(HttpRequest *request, const char *name, const char *value);
int ov38_02221D9C(HttpRequest *request, const char *name, const char *value);
int ov38_02221E10(HttpRequest *request, int unk_1C, int unk_18);
int ov38_02221E2C(HttpRequest *request);
HttpJob *ov38_02221EE0(int id);
int ov38_02221F38(int id);
void ov38_02221FD8(void);
HttpJob *ov38_02222008(void);
int ov38_0222201C(void);
void ov38_02222034(void);
void ov38_02222038(void);
void ov38_0222204C(void);
int ov38_02222060(int priority, u8 *stack);
void ov38_022220D8(void);
void ov38_02222100(void);
int ov38_0222211C(void);
void ov38_02222138(HttpResponse *response, int pos, HttpResponseChunk **chunk, int *offset);
int ov38_02222194(HttpResponse *response, HttpResponseChunk **chunk, int *offset);
int ov38_02222210(HttpResponse *response, int begin, int end, int *colon);
int ov38_022222E0(HttpResponse *response, int begin, int end);
int ov38_02222350(HttpResponse *response, int begin, int end, const char *str, int stop);
int ov38_02222420(HttpResponse *response, void *out, int offset, int size);
int ov38_0222250C(HttpResponse *response, u32 length);
int ov38_02222520(HttpRequest *request, int socket, int offset, int flags);
int ov38_02222548(HttpRequest *request, int socket, int offset, int size, int flags);

// http_socket_request.c
HttpRequest *ov38_022225B8(const char *url, int method, void *body, int bodyCapacity, HttpSocketCallback callback, void *param);
void ov38_02222B1C(HttpRequest *request);
void ov38_02222B44(HttpField *list);
int ov38_02222B98(HttpRequest *request);
int ov38_02222C10(HttpRequest *request);
int ov38_02222C9C(int id);
void ov38_02222D18(void);
u32 ov38_02222D6C(HttpRequest *request);
void ov38_02222E04(HttpResponse *response);
int ov38_02222E7C(HttpResponse *response, const char *name, int *valueStart);
int ov38_02222F88(HttpResponse *response, const char *name, char **value);
int ov38_0222308C(HttpResponse *response, char **body);
int ov38_022230B0(int result);
void ov38_022230BC(void);
int ov38_022230DC(HttpRequest *request);
int ov38_0222315C(HttpRequest *request, int socket);
int ov38_0222316C(HttpRequest *request, int socket);
int ov38_022231A8(HttpRequest *request, int socket, int ip, int port);
int ov38_02223224(HttpRequest *request, int socket, void *buf, int size, int flags);
int ov38_02223270(HttpRequest *request, int socket, const void *buf, int size, int flags);
void ov38_022232BC(HttpRequest *request, int socket);
int ov38_022232D8(const char *a, const char *b);
int ov38_022232E4(const char *a, const char *b, int n);
u32 ov38_0222334C(const char *str);
void ov38_02223358(void *dst, int size);
void ov38_0222336C(void *dst, const void *src, int size);
int ov38_02223384(int *received, int *total);
int ov38_02223424(HttpRequest *request, int socket, int *used, const char *src, int size);
int ov38_022234D8(HttpRequest *request, int socket);
void ov38_02223564(HttpSslConnection *ssl);
void ov38_022235A0(int socket, HttpSslConnection **ssl);
void ov38_02223640(void);

// http_socket_worker.c
void ov38_022236A0(void *arg);
int ov38_022249A0(const char *str);
int ov38_02224A08(char *out, int ch);
int ov38_02224A98(const char *str, int length);
int ov38_02224B80(const char *str, int length);
int ov38_02224C18(char *out, u32 value);
int ov38_02224CD4(const char *a, const char *b);
int ov38_02224D58(const char *str, int length);
int ov38_02224DC8(const char *data, int dataLength, const char *pattern, int patternLength);

#endif // POKEHEARTGOLD_LIB_INCLUDE_DWC_HTTP_SOCKET_INTERNAL_H
