#ifndef POKEHEARTGOLD_LIB_INCLUDE_DWC_STATS_UPLOAD_INTERNAL_H
#define POKEHEARTGOLD_LIB_INCLUDE_DWC_STATS_UPLOAD_INTERNAL_H

// Web stats upload client of the OVY_45 network library (src/stats_upload.c, with its key setup and random
// number generator at the end of src/ov45_02253968.c). An upload is one URL
//     <server><game name><name>?pid=<pid>&hash=<40 hex digits>&data=<base64 data>
// fetched over HTTP: first without the hash part to get a 32 byte challenge, whose SHA-1 with the secret key
// gives the hash (kept for later uploads in ov45_022576A0), then the whole URL. The response ends with the
// SHA-1 of the salted base64 body, which is checked before it is handed to the callback.

// Returns FALSE to reject the response.
typedef BOOL (*StatsUploadResponseFunc)(char *response, int length);

// Steps of StatsUpload.state (StatsUpload_Process advances them).
enum StatsUploadState {
    STATS_UPLOAD_NONE,           // 0: not set by this code
    STATS_UPLOAD_FAILED,         // 1
    STATS_UPLOAD_STOPPED,        // 2: initial value, after StatsUpload_Stop
    STATS_UPLOAD_READY,          // 3: after StatsUpload_Start
    STATS_UPLOAD_GET_CHALLENGE,  // 4
    STATS_UPLOAD_WAIT_CHALLENGE, // 5
    STATS_UPLOAD_SEND,           // 6
    STATS_UPLOAD_WAIT_RESPONSE,  // 7
    STATS_UPLOAD_DONE,           // 8: the response is in body/bodyLength
};

// ov45_0225762C (0x74 bytes, in the library's data). gameName and secretKey are also labelled on their own
// (ov45_0225763C, ov45_0225765C) and the code reaches them through those labels.
typedef struct StatsUpload {
    int state;
    BOOL active;
    int request;   // HTTP request id of ov00_021FA488
    BOOL keyReady; // set by ov45_02253DDC
    char gameName[32];
    u8 secretKey[20];
    u32 multiplier; // random number generator of the data scrambling: next = (multiplier * x + increment) % modulus
    u32 increment;
    u32 modulus;
    u32 checksumKey; // xored into the data checksum
    char *url;
    char *hash;     // the 40 hex digits in url
    char *data;     // the end of url, where the data is appended
    char *response; // the last response (heap block) and its length
    int responseLength;
    const char *body; // the response without its hash
    int bodyLength;
    StatsUploadResponseFunc callback;
} StatsUpload;

extern StatsUpload ov45_0225762C;
extern char ov45_0225763C[]; // ov45_0225762C.gameName
extern u8 ov45_0225765C[];   // ov45_0225762C.secretKey
extern char *ov45_02257628;  // server address (one of the three below)
extern char ov45_022576A0[]; // hash of the last challenge, 40 hex digits
extern u32 ov45_022587F8;    // random number generator state

// ov45_02253968.c
u32 ov45_02253D50(u32 length);
void ov45_02253D88(u32 seed);
int ov45_02253D9C(void);
BOOL ov45_02253DDC(char *gameName, char *secretKey, u32 multiplier, u32 increment, u32 modulus, u32 checksumKey);

// stats_upload.c
BOOL StatsUpload_CheckResponseHash(const char *response, int length);
void StatsUpload_OnRequestProgress(void);
void StatsUpload_OnRequestCompleted(char *response, int length, int error);
int StatsUpload_EncodeData(char *out, const u8 *first, int firstLength, const u8 *second, int secondLength);
void StatsUpload_Start(int server, char *key);
void StatsUpload_SetFailed(void);
int StatsUpload_Process(void);
void StatsUpload_Stop(void);
const char *StatsUpload_GetResponseBody(int *length);
int StatsUpload_PrepareUpload(char *name, int pid, const void *data, int size, StatsUploadResponseFunc callback, BOOL useLastHash);
int StatsUpload_Upload(char *name, int pid, const void *data, int size, StatsUploadResponseFunc callback);
int StatsUpload_UploadWithLastHash(char *name, int pid, const void *data, int size, StatsUploadResponseFunc callback);

// OVY_0 network library functions used by the client
int ov00_021FA488(const char *url, int unk_04, int unk_08, void (*progress)(void), void (*completed)(char *response, int length, int error), int unk_14);
BOOL ov00_021FA118(void);
void ov00_021FA0B4(int unk_00);
void ov00_021FA0D8(void);
int B64Encode(const void *src, void *dst, int length, int variant); // base64 encoding

#endif // POKEHEARTGOLD_LIB_INCLUDE_DWC_STATS_UPLOAD_INTERNAL_H
