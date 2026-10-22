// PPW lobby core: sending and receiving lobby messages, main and sub channel handling, player tracking and the
// questionnaire, schedule, VIP and profile web requests. Kept as one translation unit because its functions share
// one block of file-scope data.
#include <nitro/types.h>

#include "ppw_lobby_state.h"
#include "ppw_lobby_timers.h"

#include <nitro/code32.h>

#include "lobby_mem.h"

#include <size_t.h>
#include <stdarg.h>
#include <string.h>
using std::va_list;
extern "C" {
#include <nitro/os/printf.h>
}
extern "C" {
#include <nitro/rtc.h>
}

#include "overlay45_format_buffer_private.h"
#include "overlay45_string_search.h"
#include "lobby_chat_request.h"
#include "lobby_cpp_types.h"
#include "ppw_lobby_work.h"

#include "../../lib/NitroDWC/src/tree-iterator4.h"
using std::memcmp;
using std::memcpy;
using std::memmove;
using std::memset;
using std::strlen;

inline void *operator new(std::size_t, void *p) {
    return p;
}

template <class T>
inline const T &Min(const T &a, const T &b) {
    return a < b ? a : b;
}

extern "C" const char gLobbyStr_b_lib_u_user[], gLobbyStr_b_lib_u_system[], gLobbyStr_b_lib_c_time[], gLobbyStr_b_lib_c_lobby[];

extern "C" {
int printf(const char *, ...);
void abort(void);
void DWC_Free(int, void *, u32);

int LobbyChat_GetChannelKeysAsync(const char *, int, const char *, LobbyGetChannelKeysCallback, u32);
int LobbyString_CompareChars(const Str *, u32, u32, const void *, u32);
u32 LobbyString_FindChars(const StringSearch *, const s8 *, u32, u32);
void LobbyTreeIterator_Advance(Iterator *);
}

// The strings of the PPW lobby code: empty when made without a value, released with Destroy().
class PPWString {
public:
    Str str;

    PPWString() {
        u32 i;
        u32 *p = str.words;
        for (i = 0; i < 3; i++) {
            p[i] = 0;
        }
    }

    PPWString(const char *text) {
        LobbyString_InitFromCStr(&str, text);
    }

    PPWString(const PPWString &other) {
        LobbyString_InitCopy(&str, &other.str);
    }

    PPWString &operator=(const PPWString &other) {
        LobbyString_Assign(&str, &other.str);
        return *this;
    }

    void Destroy() {
        LobbyString_Destroy(&str);
    }

    const char *CStr() const {
        return str.c_str();
    }
};

extern "C" char gLobbyStr_WebEncLobbySubmitQuestionnaireAsp[], gLobbyStr_WebEncLobbyGetQuestionnaireAsp[], gLobbyStr_StatsUploadKey[], gLobbyStr_WebEncLobbyGetScheduleAsp[], gLobbyStr_WebEncLobbyGetVIP_Asp[], gLobbyStr_WebEncLobbyCheckProfileAsp[];
#pragma explicit_zero_data on
extern "C" u8 gLobbyTimerMapInserted = 1;
extern "C" const char *gPPWLobbyGetScheduleUrl = gLobbyStr_WebEncLobbyGetScheduleAsp;
extern "C" const char *gPPWLobbyGetVipUrl = gLobbyStr_WebEncLobbyGetVIP_Asp;
extern "C" const char *gPPWLobbyGetQuestionnaireUrl = gLobbyStr_WebEncLobbyGetQuestionnaireAsp;
extern "C" const char *gPPWLobbySubmitQuestionnaireUrl = gLobbyStr_WebEncLobbySubmitQuestionnaireAsp;
extern "C" signed char gLobbyStringTerminator = 0;
extern "C" const char *gPPWLobbyCheckProfileUrl = gLobbyStr_WebEncLobbyCheckProfileAsp;
extern "C" const char *gPPWLobbyStatsUploadKey = gLobbyStr_StatsUploadKey;
extern "C" signed char gLobbyStringInPlaceTerminator = 0;
extern "C" u8 gLobbyTimerMapNotInserted = 0;
#pragma explicit_zero_data reset

// Where a web request goes (StatsUpload_Start): the release server, or the development server for a game made with
// a non-zero prefix.
enum WebServer {
    WEB_SERVER_RELEASE,
    WEB_SERVER_DEVELOPMENT
};


extern "C" void LobbyCharVector_AssignIterRange(LobbyCharVector *, ByteIterator, ByteIterator, RangeTag);
extern "C" void StatsUpload_Start(int server, const char *key);
extern "C" int StatsUpload_Upload(const char *page, int pid, const void *data, u32 size, int callback);
extern "C" int StatsUpload_UploadWithLastHash(const char *page, int pid, const void *data, u32 size, int callback);
extern "C" void StatsUpload_Stop(void);

// A byte vector of the PPW lobby code: empty when made, released with Destroy().
struct ByteVector : LobbyCharVector {
    ByteVector() {
        data = NULL;
        size = 0;
        capacity = 0;
    }

    void Assign(ByteIterator first, ByteIterator last) {
        Assign(first, last, AssignTag(), IteratorTag());
    }

    void Assign(ByteIterator first, ByteIterator last, const AssignTag &, const IteratorTag &) {
        LobbyCharVector_AssignIterRange(this, first, last, RangeTag());
    }

    void Destroy() {
        LobbyCharVector_Destroy(this);
    }
};

struct WebRequest;

struct WebRequestVtable {
    void (*unused)(WebRequest *request);
    void (*destroy)(WebRequest *request);
};

extern "C" const WebRequestVtable gPPWLobbyWebRequestVtable;

// The PPWLobbyWebRequest the lobby builds in place for gPPWLobbyCurrentRequest: tried three times, its body a copy of
// the bytes from first to last.
struct WebRequest {
    const WebRequestVtable *vtable;
    int retries;
    int server;
    PPWString key;
    PPWString page;
    int pid;
    ByteVector body;
    PPWLobbyWebCallback callback;
    void *param;
    BOOL useLastHash;

    WebRequest(int server, const char *key, const char *page, int pid, const u8 *first, const u8 *last, PPWLobbyWebCallback callback, BOOL useLastHash)
        : vtable(&gPPWLobbyWebRequestVtable)
        , retries(3)
        , server(server)
        , key(key)
        , page(page)
        , pid(pid)
        , body()
        , callback(callback)
        , param(NULL)
        , useLastHash(useLastHash) {
        body.Assign(ByteIterator(first), ByteIterator(last));
    }
};

extern "C" WebRequest *gPPWLobbyCurrentRequest;


extern "C" void LobbyCharVector_Allocate(LobbyCharVector *vector, u32 size, ByteAllocator);

// The C++ view of PPWLobbySystemProfile, compared and copied as a whole.
struct SystemProfileHead {
    int channel;
};

struct SystemProfile : SystemProfileHead {
    u64 time;
    u32 unk_0C;

    SystemProfile() {
    }

    SystemProfile(const SystemProfile &other)
        : SystemProfileHead(other) {
        *this = other;
    }

    bool operator==(const SystemProfile &other) const {
        return memcmp(this, &other, sizeof(SystemProfile)) == 0;
    }
};

// PPWLobbyPlayer.systemProfile and systemProfileChanged.
struct SystemProfileEntry {
    SystemProfile profile;
    int changed;

    SystemProfileEntry(const SystemProfile &newProfile)
        : profile(newProfile)
        , changed(1) {
    }

    void Assign(const SystemProfile &newProfile) {
        profile = newProfile;
        changed = 1;
    }
};

// The C++ view of PPWLobbyPlayer.
struct PlayerView {
    const void *vtable;
    SystemProfileEntry systemProfile;
    LobbyCharVector userProfile;
    int userProfileChanged;
    int state;
    int id;
    int systemProfileReceived;
    int userProfileReceived;
    int entered;
};

// What onPlayerRecordChanged reports of a player: the channel time and the channel.
struct PlayerRecordData {
    u64 time;
    int channel;
};

struct PlayerRecord {
    PlayerRecordData d;

    PlayerRecord(const SystemProfile &profile) {
        d.channel = profile.channel;
        d.time = profile.time;
    }

    PlayerRecord() {
    }

    PlayerRecord(const PlayerRecord &other)
        : d(other.d) {
    }
};

// Starts the upload of the current request; one the stats upload client refuses is deleted at once.
inline void SendWebRequest(WebRequest *request) {
    gPPWLobbyCurrentRequest = request;
    StatsUpload_Start(request->server, request->key.CStr());
    int ret;
    if (request->useLastHash) {
        u32 size = request->body.size;
        ret = StatsUpload_UploadWithLastHash(request->page.CStr(), request->pid, LobbyByteVector_Data(&request->body), size, 0);
    } else {
        u32 size = request->body.size;
        ret = StatsUpload_Upload(request->page.CStr(), request->pid, LobbyByteVector_Data(&request->body), size, 0);
    }
    int ok;
    if (ret == 0) {
        ok = 1;
    } else {
        StatsUpload_Stop();
        ok = 0;
    }
    if (ok) {
        return;
    }
    if (gPPWLobbyCurrentRequest != 0) {
        WebRequest *current = gPPWLobbyCurrentRequest;
        if (current != 0) {
            current->vtable->destroy(current);
        }
        gPPWLobbyCurrentRequest = 0;
    }
}

namespace SubmitQuestionnaire {
extern "C" void OS_GetMacAddress(u8 *);

struct Info {
    int id;
    u16 one;
    u8 version;
    u8 language;
    u8 mac[6];
    u16 pad;
    int pad2;
};

struct Packet {
    Info info;
    u32 out0, out1;
    int mode;
    u32 trainerId;
    u32 gender;
    u16 country;
    u16 region;
};

struct Out {
    u32 a, b;
    u8 rest[0x2d0];
};

struct Data {
    u8 pad0[4];
    u32 trainerId;
    u8 pad8[0x38 - 8];
    u8 gender;
    u8 language;
    u8 pad3a[2];
    u16 country;
    u8 region;
    u8 pad3f[3];
    u8 version;
};

inline const Data *GetData(PPWLobbyWork *w) {
    return (const Data *)LobbyByteVector_Data(&w->players.self.userProfile);
}

extern "C" int PPW_LobbySubmitQuestionnaire(int mode);

extern "C" int PPW_LobbySubmitQuestionnaire(int mode) {
    const char *key;
    const char *page;
    WebRequest *request;
    int pid;
    WebServer server;
    PPWLobbyWork *w = gPPWLobbyWork;
    if (w == 0) {
        return PPW_LOBBY_RESULT_ERROR_STATE;
    }
    if (w != 0 && w->state == PPW_LOBBY_STATE_ERROR) {
        return PPW_LOBBY_RESULT_ERROR_CONDITION;
    }
    if (mode < 0 || mode > 2) {
        return PPW_LOBBY_RESULT_ERROR_PARAM;
    }
    Out out;
    if (!PPWLobbySchedule_GetQuestionnaire(&w->schedule, &out) || gPPWLobbyWork->submittingQuestionnaire != 0) {
        return PPW_LOBBY_RESULT_ERROR_STATE;
    }
    const Data *d = GetData(gPPWLobbyWork);
    Packet p = { 0 };
    {
        Info info;
        const Data *d2 = GetData(gPPWLobbyWork);
        info.id = gPPWLobbyWork->userId;
        info.one = 1;
        info.version = d2->version;
        info.language = d2->language;
        OS_GetMacAddress(info.mac);
        p.info = info;
    }
    p.out0 = out.a;
    p.out1 = out.b;
    p.mode = mode;
    p.trainerId = d->trainerId;
    p.gender = d->gender;
    p.country = d->country;
    p.region = d->region;
    pid = gPPWLobbyWork->userId;
    page = gPPWLobbySubmitQuestionnaireUrl;
    key = gPPWLobbyStatsUploadKey;
    server = gPPWLobbyWork->prefix == 0 ? WEB_SERVER_RELEASE : WEB_SERVER_DEVELOPMENT;
    if (gPPWLobbyCurrentRequest == 0) {
        request = (WebRequest *)LobbyMem_Alloc(sizeof(WebRequest));
        new (request) WebRequest(server, key, page, pid, (const u8 *)&p, (const u8 *)&p + sizeof(p), PPWLobby_OnSubmitQuestionnaire, FALSE);
        SendWebRequest(request);
    }
    gPPWLobbyWork->submittingQuestionnaire = 1;
    return PPW_LOBBY_RESULT_SUCCESS;
}
}

extern "C" {
extern int LobbyChat_ToErrorCode(int error);

int PPW_LobbyGetLastError(void) {
    PPWLobbyWork *work = gPPWLobbyWork;
    if (work) {
        return work->error;
    }
    return PPW_LOBBY_ERROR_NONE;
}

// The error code shown to the player: the web request errors (PPW_LOBBY_ERROR_SCHEDULE on) from 10624 on, the
// others from 42000 on
int PPW_LobbyToErrorCode(int error) {
    if (error >= PPW_LOBBY_ERROR_SCHEDULE) {
        return error + 10620;
    }
    return LobbyChat_ToErrorCode(error);
}

int PPW_LobbyInitResultToLastError(int result) {
    switch (result) {
    case PPW_LOBBY_RESULT_SUCCESS:
        return PPW_LOBBY_ERROR_NONE;
    case PPW_LOBBY_RESULT_ERROR_ALLOC:
        return PPW_LOBBY_ERROR_ALLOC;
    case PPW_LOBBY_RESULT_ERROR_SESSION:
        return PPW_LOBBY_ERROR_SESSION;
    default:
        return PPW_LOBBY_ERROR_UNKNOWN;
    }
}
}

extern "C" char gLobbyStr_Empty_15[];
extern "C" int PPWLobby_EncodeMessage(int, int, int, const void *, u32, Str *);
extern "C" int LobbyChat_SendUserMessage(int, const char *);
extern "C" int LobbyChat_SendChannelMessage(const char *, const char *);

inline bool IsEmptyChannelName(const Str &name) {
    return LobbyString_CompareCStr(&name, gLobbyStr_Empty_15) == 0;
}

// A failed chat call that leaves the session unusable puts the library into the error state.
inline void SetError(int result) {
    PPWLobbyWork *work = gPPWLobbyWork;
    int error = PPWLobbyErrorMap().Map(result);
    if (error != PPW_LOBBY_ERROR_NONE) {
        work->error = error;
        work->state = PPW_LOBBY_STATE_ERROR;
    }
}

extern "C" int PPWLobby_SendMessage(int id, int key, int a, int b, int c, const void *d, u32 e) {
    int result;
    if (id == -1 && key == 5) {
        return PPW_LOBBY_RESULT_ERROR_PARAM;
    }
    PPWString value;
    if (!PPWLobby_EncodeMessage(a, b, c, d, e, &value.str)) {
        value.Destroy();
        return PPW_LOBBY_RESULT_ERROR_PARAM;
    }
    if (key == 5) {
        result = LobbyChat_SendUserMessage(id, value.CStr());
        SetError(result);
        value.Destroy();
        return result;
    }
    Str name;
    PPWLobby_GetChannelName(&name, &gPPWLobbyWork->mainChannel, key);
    if (IsEmptyChannelName(name)) {
        LobbyString_Destroy(&name);
        value.Destroy();
        return PPW_LOBBY_RESULT_ERROR_CHANNEL;
    }
    result = LobbyChat_SendChannelMessage(name.c_str(), value.CStr());
    SetError(result);
    LobbyString_Destroy(&name);
    value.Destroy();
    return result;
}

extern "C" {
extern char gLobbyStr_Empty_15[];
extern int LobbyChat_SetChannelKey(const char *, int, const char *, const char *);
extern void LobbyCharVector_Grow(LobbyCharVector *, u32);
extern int DWC_Base64Encode(const void *, u32, char *, u32);

int PPWLobby_SetChannelData(int id, int key, const char *name, const void *data, u32 length) {
    Str channel;
    LobbyCharVector encoded;
    int result;
    const char *text;
    PPWLobby_GetChannelName(&channel, &gPPWLobbyWork->mainChannel, key);
    if (IsEmptyChannelName(channel)) {
        LobbyString_Destroy(&channel);
        return PPW_LOBBY_RESULT_ERROR_CHANNEL;
    }
    if (length > 300) {
        LobbyString_Destroy(&channel);
        return PPW_LOBBY_RESULT_ERROR_PARAM;
    }
    encoded.data = 0;
    encoded.size = 0;
    encoded.capacity = 0;
    LobbyCharVector_Grow(&encoded, (length + 2) / 3 * 4 + 1);
    {
        char *out = LobbyCharVector_Data(&encoded);
        int end = DWC_Base64Encode(data, length, out, encoded.size);
        encoded.data[end] = 0;
    }
    text = channel.c_str();
    result = LobbyChat_SetChannelKey(text, id, name, LobbyCharVector_Data(&encoded));
    SetError(result);
    LobbyCharVector_Destroy(&encoded);
    LobbyString_Destroy(&channel);
    return result;
}
}

namespace StartCheckProfile {
extern "C" void OS_GetMacAddress(u8 *);

typedef u64 W8;

struct B48 {
    u32 w[12];
};

struct Config {
    u32 a, b;
    u16 h08[8];
    u32 h18, h1C;
    u16 h20[6];
    u8 b2C[6];
    u8 b32[6];
    u8 b38, b39;
    u16 h3A, h3C;
    u8 b3E, b3F, b40, b41, b42, b43;
    W8 b44;
    u8 b4C[12];
    B48 b58;

    struct {
        u16 h88, h8A;
    } t1;

    struct {
        u32 h8C, h90;
    } t2;
};

struct Info {
    u32 userId;
    u16 one;
    u8 version;
    u8 language;
    u8 mac[6];
    u8 rest[6];
    Config cfg;
};

extern "C" void PPWLobby_StartCheckProfile(void);

extern "C" void PPWLobby_StartCheckProfile(void) {
    Info info;
    const Config *c = (const Config *)LobbyByteVector_Data(&gPPWLobbyWork->players.self.userProfile);
    const PPWLobbyUserProfile *f = (const PPWLobbyUserProfile *)LobbyByteVector_Data(&gPPWLobbyWork->players.self.userProfile);
    const char *key;
    const char *page;
    WebRequest *request;
    u32 pid;
    WebServer server;
    info.userId = gPPWLobbyWork->userId;
    info.one = 1;
    info.version = f->version;
    info.language = f->language;
    OS_GetMacAddress(info.mac);
    info.cfg = *c;
    pid = gPPWLobbyWork->userId;
    page = gPPWLobbyCheckProfileUrl;
    key = gPPWLobbyStatsUploadKey;
    server = gPPWLobbyWork->prefix == 0 ? WEB_SERVER_RELEASE : WEB_SERVER_DEVELOPMENT;
    if (gPPWLobbyCurrentRequest != 0) {
        return;
    }
    request = (WebRequest *)LobbyMem_Alloc(sizeof(WebRequest));
    new (request) WebRequest(server, key, page, pid, (const u8 *)&info, (const u8 *)&info + sizeof(Info), PPWLobby_OnCheckProfile, FALSE);
    SendWebRequest(request);
}
}

extern "C" {
extern char *gPPWLobbyChannelNamePrefixes[];
extern char gLobbyStr_Bang[];

void PPWLobby_MakeChannelName(Str *out, u32 id, Str *s) {
    Str b, a;
    LobbyString_InitFromCStr(&b, gPPWLobbyChannelNamePrefixes[id]);
    LobbyString_ConcatCStr(&a, &b, gLobbyStr_Bang);
    LobbyString_Concat(out, &a, s);
    LobbyString_Destroy(&a);
    LobbyString_Destroy(&b);
}
}

using namespace LobbyStringTypes;

extern "C" Str *LobbyString_Concat(Str *out, Str *left, Str *right) {
    u32 i, n1, n2;
    char *p1, *p2;
    RangeDispatch first, second;
    for (i = 0; i < 3; i++) {
        out->words[i] = 0;
    }
    if (left->l.isLong) {
        n1 = left->l.size;
        p1 = left->l.ptr;
    } else {
        n1 = left->s.len;
        p1 = left->s.buf;
    }
    if (right->l.isLong) {
        p2 = right->l.ptr;
        n2 = right->l.size;
    } else {
        p2 = right->s.buf;
        n2 = right->s.len;
    }
    LobbyString_Reserve(out, n1 + n2);
    out = LobbyString_ReplaceRange(out, 0, 0, p1, p1 + n1, first);
    return LobbyString_ReplaceRange(out, n1, 0, p2, p2 + n2, second);
}

using namespace LobbyStringTypes;

extern "C" Str *LobbyString_ConcatCStr(Str *out, Str *left, char *right) {
    u32 i, n1;
    char *p1;
    RangeDispatch first, second;
    for (i = 0; i < 3; i++) {
        out->words[i] = 0;
    }
    const u32 n2 = strlen(right);
    if (left->l.isLong) {
        p1 = left->l.ptr;
        n1 = left->l.size;
    } else {
        p1 = left->s.buf;
        n1 = left->s.len;
    }
    LobbyString_Reserve(out, n1 + n2);
    out = LobbyString_ReplaceRange(out, 0, 0, p1, p1 + n1, first);
    return LobbyString_ReplaceRange(out, n1, 0, right, right + n2, second);
}

extern "C" {
extern u32 current_time(void);
extern char gLobbyFmt_C_IntUnderscoreUint[];

void PPWLobby_GenerateChannelBaseName(Str *out) {
    u32 id = gPPWLobbyWork->userId;
    u32 tick = current_time();
    LobbyString_Format(out, gLobbyFmt_C_IntUnderscoreUint, id, tick % 1000);
}
}

using namespace LobbyCharVectorTypes;

namespace StringFormat {
struct Vector {
    LobbyCharVector repr;

    Vector() {
        repr.data = 0;
        repr.size = 0;
        repr.capacity = 0;
    }

    ~Vector() {
        LobbyCharVector_Destroy(&repr);
    }

    void shrink(u32 count, ResizeDispatch) {
        repr.size -= count;
    }

    void resize(u32 count) {
        if (count > repr.size) {
            LobbyCharVector_AppendZeros(&repr, count - repr.size, ResizeDispatch());
        } else if (count < repr.size) {
            shrink(repr.size - count, ResizeDispatch());
        }
    }

    char *text() {
        return LobbyCharVector_Data(&repr);
    }
};

typedef char StringSize[sizeof(Str) == 12 ? 1 : -1];
typedef char VectorSize[sizeof(Vector) == 12 ? 1 : -1];
typedef char DispatchSize[sizeof(ResizeDispatch) == 1 ? 1 : -1];
}

using namespace StringFormat;

extern "C" void LobbyString_Format(Str *output, const char *format, ...) {
    Vector vector;
    va_list args;
    u32 count;
    va_start(args, format);
    count = OS_VSNPrintf(0, 0, format, args) + 1;
    vector.resize(count);
    OS_VSNPrintf(vector.text(), count, format, args);
    vector.repr.data[count - 1] = 0;
    va_end(args);
    new (output) Str;
    LobbyString_InitFromCStr(output, vector.text());
}

namespace EnterMainChannel {
typedef LobbyChannelCallbacks Callbacks;
extern "C" int LobbyChat_EnterChannelAsync(const char *, int, int, Callbacks *, LobbyEnterCallback, int);
extern "C" int PPWLobby_EnterMainChannel(void);


struct Channel {
    int kind;
    u32 nm[3];
    int usersEnumerated;
    int modeReady;

    PPWString &name() {
        return *(PPWString *)nm;
    }

    void Destroy() {
        name().Destroy();
    }
};

inline void SetSelfChannel(const PPWString &n) {
    Channel c;
    PPWLobbyWork *w;
    Channel *p = &c;
    c.kind = 0;
    LobbyString_InitCopy(&c.name().str, &n.str);
    p->usersEnumerated = 0;
    p->modeReady = 0;
    w = gPPWLobbyWork;
    w->mainChannel.kind = c.kind;
    LobbyString_Assign(&w->mainChannel.name, &c.name().str);
    w->mainChannel.usersEnumerated = c.usersEnumerated;
    w->mainChannel.modeReady = c.modeReady;
    c.Destroy();
}

#pragma push
#pragma opt_propagation off

extern "C" int PPWLobby_EnterMainChannel(void) {
    Callbacks cb = {PPWLobby_OnPlayerJoined, PPWLobby_OnPlayerLeft, PPWLobby_OnMessage, PPWLobby_OnChannelDataChanged, PPWLobby_OnTopicChanged, NULL};
    PPWString base;
    PPWLobby_GetChannelBaseName(&base.str, gPPWLobbyWork);
    PPWString name;
    PPWLobby_MakeChannelName(&name.str, 0, &base.str);
    base.Destroy();
    int result = LobbyChat_EnterChannelAsync(name.CStr(), 0, 0x14, &cb, PPWLobby_OnEnterMainChannel, 0);
    {
        PPWLobbyWork *w = gPPWLobbyWork;
        int error = PPWLobbyErrorMap().Map(result);
        if (error != PPW_LOBBY_ERROR_NONE) {
            w->error = error;
            w->state = PPW_LOBBY_STATE_ERROR;
        }
    }
    if (result != 0) {
        name.Destroy();
        return result;
    }
    {
        SetSelfChannel(name);
    }
    name.Destroy();
    return result;
}

#pragma pop
}

namespace HandleReceivedData {
struct Vec;
extern "C" Vec *LobbyCharVector_Destroy(Vec *);

struct Vec {
    char *ptr;
    u32 size, cap;

    void Init() {
        ptr = 0;
        size = 0;
        cap = 0;
    }

    void Destroy() {
        LobbyCharVector_Destroy(this);
    }
};

extern "C" int PPWLobby_DecodeMessageData(const char *, Vec *);
extern "C" const u8 *LobbyByteVector_Data(Vec *);
extern "C" void PPWLobby_HandleReceivedData(BOOL, BOOL, const char *, int, const char *, const char *, u32);
extern "C" PPWLobbyPlayer *PPWLobbyPlayers_Find(void *, int);

inline int Find(PPWLobbyWork *w, const PPWString &name) {
    const PPWString &n = name;
    int eq;
    {
        Str t;
        PPWLobbyChannel_GetName(&t, &w->mainChannel);
        eq = LobbyString_Equals(t, n.str);
        LobbyString_Destroy(&t);
    }
    if (eq) {
        return w->mainChannel.kind;
    }
    {
        Str t;
        PPWLobbyChannel_GetName(&t, &w->subChannel);
        eq = LobbyString_Equals(t, n.str);
        LobbyString_Destroy(&t);
    }
    if (eq) {
        return w->subChannel.kind;
    }
    return 5;
}

inline int IsActive(PPWLobbyPlayer *s) {
    return s != 0 && s->entered != 0;
}

#pragma opt_common_subs off

extern "C" void PPWLobby_HandleReceivedData(BOOL a, BOOL b, const char *channel, int id, const char *arg, const char *text, u32 extra) {
    int kind;
    Vec data;
    {
        PPWString name(channel);
        kind = Find(gPPWLobbyWork, name);
        name.Destroy();
    }
    if (kind == 5) {
        return;
    }
    data.Init();
    if (!PPWLobby_DecodeMessageData(text, &data)) {
        data.Destroy();
        return;
    }
    u32 len = data.size;
    if (len == 0) {
        data.Destroy();
        return;
    }
    if (PPWLobby_HandleLibraryData(a, b, kind, id, arg, LobbyByteVector_Data(&data), len, extra)) {
        data.Destroy();
        return;
    }
    if (id != -1) {
        if (!IsActive(PPWLobbyPlayers_Find(&gPPWLobbyWork->players, id))) {
            data.Destroy();
            return;
        }
    }
    PPWLobbyWork *w = gPPWLobbyWork;
    w->callbacks.onLocationsChanged(a, b, kind, id, arg, LobbyByteVector_Data(&data), data.size, extra);
    data.Destroy();
}

#pragma opt_common_subs reset
}

namespace DecodeMessageData {
class Decoder {
public:
    int Decode(const char *src, u32 len, LobbyCharVector *dst) const;
};

extern "C" int DWC_Base64Decode(const char *, u32, char *, u32);
extern "C" int PPWLobby_DecodeMessageData(const char *, LobbyCharVector *);

inline void Shrink(LobbyCharVector *v, u32 count, ResizeDispatch) {
    v->size -= count;
}

inline void Resize(LobbyCharVector *v, u32 count) {
    u32 size = v->size;
    if (count > size) {
        LobbyCharVector_AppendZeros(v, count - size, ResizeDispatch());
    } else if (count < size) {
        Shrink(v, size - count, ResizeDispatch());
    }
}

inline int Decoder::Decode(const char *src, u32 len, LobbyCharVector *dst) const {
    return DWC_Base64Decode(src, len, LobbyByteVector_Data(dst), dst->size);
}

extern "C" int PPWLobby_DecodeMessageData(const char *text, LobbyCharVector *out) {
    Decoder codec = Decoder();
    u32 len = strlen(text);
    int n;
    Resize(out, len);
    if (len == 0) {
        return 1;
    }
    n = codec.Decode(text, strlen(text), out);
    if (n == -1) {
        return 0;
    }
    Resize(out, n);
    return 1;
}
}

extern "C" int LobbyString_Equals(const Str &a, const Str &b) {
    return a.size() == b.size() && LobbyString_Compare(&a, &b) == 0;
}

namespace HandleLibraryData {
extern "C" int strcmp(const char *, const char *);

// The empty tags of the byte vector's copy constructor (its allocator) and of its assignment from another vector.
struct AllocatorTag {};

struct CopyTag {};

struct Vec;
extern "C" void LobbyCharVector_AssignIterRange(Vec *, ByteIterator, ByteIterator, RangeTag);
extern "C" void LobbyCharVector_InitRange(Vec *, const u8 *, const u8 *, AllocatorTag);
extern "C" void LobbyCharVector_AssignRange(Vec *, const u8 *, const u8 *, CopyTag);
extern "C" const u8 *LobbyByteVector_Data(const Vec *);
extern "C" void LobbyCharVector_Destroy(const Vec *);

struct Vec {
    u8 *ptr;
    u32 size, cap;

    void Init() {
        ptr = 0;
        size = 0;
        cap = 0;
    }

    void Assign(const u8 *first, const u8 *last) {
        Assign(ByteIterator(first), ByteIterator(last));
    }

    void Assign(ByteIterator first, ByteIterator last) {
        Assign(first, last, AssignTag(), IteratorTag());
    }

    void Assign(ByteIterator first, ByteIterator last, const AssignTag &, const IteratorTag &) {
        LobbyCharVector_AssignIterRange(this, first, last, RangeTag());
    }

    void InitCopy(const Vec &o, AllocatorTag al) {
        ptr = 0;
        size = 0;
        cap = 0;
        LobbyCharVector_InitRange(this, o.ptr, o.ptr + o.size, al);
    }

    void AssignVec(const Vec &o) {
        if (this != &o) {
            LobbyCharVector_AssignRange(this, o.ptr, o.ptr + o.size, CopyTag());
        }
    }
};

inline bool operator==(const Vec &a, const Vec &b) {
    return a.size == b.size && (a.size == 0 || memcmp(LobbyByteVector_Data(&a), LobbyByteVector_Data(&b), a.size) == 0);
}

struct Quad {
    u32 a;
    u64 b;
    u32 c;
};

struct Triple {
    s64 b;
    u32 a;
};

struct VecBox {
    Vec v;
    int changed;

    void Init(const Vec &x, AllocatorTag al) {
        v.InitCopy(x, al);
        changed = 1;
    }

    void InitM(const Vec &x, AllocatorTag al) {
        v.InitCopy(x, al);
        Mark();
    }

    void Mark() {
        if (this) {
            changed = 1;
        }
    }

    void Update(const VecBox &o) {
        if (!(v == o.v)) {
            v.AssignVec(o.v);
            changed = 1;
        }
    }
};

inline bool operator==(const Quad &a, const Quad &b) {
    return memcmp(&a, &b, 16) == 0;
}

struct QuadBox {
    Quad q;
    int changed;

    void Init(const Quad &x) {
        q = x;
        changed = 1;
    }

    void Update(const QuadBox &o) {
        if (!(q == o.q)) {
            q = o.q;
            changed = 1;
        }
    }
};

struct Rec {
    u32 x0;
    QuadBox q;
    VecBox vec;
    u32 x28, x2c;
    int qRecv;
    int vecRecv;
    int valid;
    void SetVec(const u8 *d, u32 n, AllocatorTag al);
    void SetQuad(const u8 *d);

    Triple GetTriple() {
        Triple t;
        t.a = q.q.a;
        t.b = q.q.b;
        return t;
    }
};

struct Val : PPWLobbyTime {
    Val(const u8 *d) {
        ((u32 *)&seconds)[0] = ((const u32 *)d)[0];
        ((u32 *)&seconds)[1] = ((const u32 *)d)[1];
    }

    Val(const Val &o) {
        seconds = o.seconds;
    }
};

extern "C" Rec *PPWLobbyPlayers_Find(void *, int);

inline void Rec::SetVec(const u8 *d, u32 n, AllocatorTag al) {
    Vec v;
    VecBox box;
    v.Init();
    v.Assign(ByteIterator(d), ByteIterator(d + n));
    VecBox *pb = &box;
    pb->InitM(v, al);
    VecBox *pc = &box;
    vec.Update(*pc);
    LobbyCharVector_Destroy(&box.v);
    vecRecv = 1;
    LobbyCharVector_Destroy(&v);
}

inline void Rec::SetQuad(const u8 *d) {
    QuadBox qbox;
    QuadBox *pq = &qbox;
    pq->Init(*(const Quad *)d);
    q.Update(*pq);
    qRecv = 1;
}

static inline void SetChannelTime(PPWLobbyWork *work, Val time) {
    work->schedule.hasTime = 1;
    work->schedule.time = time;
}

static inline void OnPairFn(const u8 *d) {
    SetChannelTime(gPPWLobbyWork, Val(d));
}

extern "C" int PPWLobby_HandleLibraryData(BOOL, BOOL, int, int, const char *, const u8 *, u32, u32);

extern "C" int PPWLobby_HandleLibraryData(BOOL a0, BOOL a1, int sel, int id, const char *cmd, const u8 *data, u32 len, u32 param) {
    if (sel) {
        return 0;
    }
    if (strcmp(cmd, gLobbyStr_b_lib_u_user) == 0) {
        if (id == gPPWLobbyWork->userId) {
            return 1;
        }
        Rec *r = PPWLobbyPlayers_Find(&gPPWLobbyWork->players, id);
        if (!r) {
            return 1;
        }
        AllocatorTag al;
        r->SetVec(data, len, al);
        if (r->valid) {
            gPPWLobbyWork->callbacks.onPlayerUpdate(id, data, len);
        }
        return 1;
    }
    if (strcmp(cmd, gLobbyStr_b_lib_u_system) == 0) {
        if (id == gPPWLobbyWork->userId) {
            return 1;
        }
        if (len != 16) {
            return 1;
        }
        Rec *r = PPWLobbyPlayers_Find(&gPPWLobbyWork->players, id);
        if (!r) {
            return 1;
        }
        r->SetQuad(data);
        if (r->valid && r->q.changed) {
            r->q.changed = 0;
            Triple t = r->GetTriple();
            gPPWLobbyWork->callbacks.onPlayerRecordChanged(id, &t);
        }
        return 1;
    }
    if (strcmp(cmd, gLobbyStr_b_lib_c_time) == 0) {
        if (len != 8) {
            return 1;
        }
        OnPairFn(data);
        return 1;
    }
    if (strcmp(cmd, gLobbyStr_b_lib_c_lobby) == 0) {
        if (!PPWLobbySchedule_Load(&gPPWLobbyWork->schedule, data, len)) {
            return 1;
        }
        gPPWLobbyWork->schedule.state = 4;
        return 1;
    }
    return 0;
}
}

namespace HandleSystemMessage {
extern "C" {
typedef struct {
    u32 pad[2];
    int mode;
    u32 gap;
    int selector;
} Obj;

PPWLobbyPlayer *PPWLobbyPlayers_Find(void *, int);

static inline BOOL RecordValid(PPWLobbyPlayer *r) {
    return r && r->entered != 0;
}

int PPWLobby_HandleSystemMessage(Obj *, int, int, void *, int);

int PPWLobby_HandleSystemMessage(Obj *obj, int id, int flag, void *payload, int message) {
    PPWLobbyPlayer *r;
    BOOL valid;
    if (flag) {
        return 1;
    }
    switch (obj->selector) {
    case 1:
        if (id == gPPWLobbyWork->userId) {
            break;
        }
        r = PPWLobbyPlayers_Find(&gPPWLobbyWork->players, id);
        valid = RecordValid(r);
        switch (valid) {
        case 0:
            goto done;
        default:
            break;
        }
        if (obj->mode == 1 && message == 0x44) {
            gPPWLobbyWork->callbacks.onMatchInfo(id, payload);
        }
        break;
    case 2:
        if (id == gPPWLobbyWork->userId) {
            break;
        }
        r = PPWLobbyPlayers_Find(&gPPWLobbyWork->players, id);
        valid = RecordValid(r);
        switch (valid) {
        case 0:
            goto done;
        default:
            break;
        }
        if (obj->mode == 1 && message == 0x44) {
            gPPWLobbyWork->callbacks.onMatchClosed(id, payload);
        }
        break;
    case 0:
        if (obj->mode == 1 && message == 4) {
            PPWLobbySchedule_Start(&gPPWLobbyWork->schedule);
        }
        break;
    }
done:
    return 1;
}
}
}

extern "C" {
extern int LobbyChat_IsInChannel(char *), PPWLobbyPlayers_AllHaveProfiles(void *);

static inline int enough(s32 n) {
    int result;
    if (n >= 4) {
        result = 1;
    } else {
        result = 0;
    }
    return result;
}

int PPWLobby_IsMainChannelReady(void) {
    Str local;
    int empty;
    PPWLobbyWork *c;
    PPWLobbyChannel_GetName(&local, &gPPWLobbyWork->mainChannel);
    empty = LobbyChat_IsInChannel(!local.l.isLong ? local.s.buf : local.l.ptr) == 0;
    LobbyString_Destroy(&local);
    if (empty) {
        return 0;
    }
    c = gPPWLobbyWork;
    if (c->mainChannel.usersEnumerated == 0) {
        return 0;
    }
    if (c->mainChannel.modeReady == 0) {
        return 0;
    }
    if (c->schedule.hasTime == 0) {
        return 0;
    }
    if (!enough(c->schedule.state)) {
        return 0;
    }
    return PPWLobbyPlayers_AllHaveProfiles(&c->players) != 0;
}
}

#pragma push

extern "C" {
#pragma opt_common_subs off

static inline int flush(PPWLobbyWork *work) {
    u32 userId;
    void *buf;
    if (work->players.self.userProfileChanged == 0) {
        return 1;
    }
    work->players.self.userProfileChanged = 0;
    userId = gPPWLobbyWork->userId;
    buf = LobbyByteVector_Data(&work->players.self.userProfile);
    PPWLobby_SetChannelData(userId, 0, gLobbyStr_b_lib_u_user, buf, work->players.self.userProfile.size);
    return 1;
}

int PPWLobby_TimerUploadUserProfile(void *unusedArg) {
    return flush(gPPWLobbyWork);
}
}

#pragma pop

namespace ChannelTimers {
extern "C" {
extern int LobbyChat_GetChannelModeAsync(const char *, LobbyChannelModeCallback, u32);
int PPWLobby_TimerUploadSystemProfile(void *);
int PPWLobby_TimerSendRecruitInfo(void *);
int PPWLobby_TimerPollChannelMode(void *);
int PPWLobby_TimerRequestChannelTime(void *);

int PPWLobby_TimerUploadSystemProfile(void *unusedArg) {
    PPWLobbyWork *work = gPPWLobbyWork;
    if (!work->players.self.systemProfileChanged) {
        return 1;
    }
    work->players.self.systemProfileChanged = 0;
    PPWLobby_SetChannelData(gPPWLobbyWork->userId, 0, gLobbyStr_b_lib_u_system, &work->players.self.systemProfile, 16);
    return 1;
}

int PPWLobby_TimerSendRecruitInfo(void *unusedArg) {
    PPWLobbyWork *work = gPPWLobbyWork;
    if (!work->recruiting) {
        return 0;
    }
    PPWLobby_SendMessage(work->userId, 0, 1, 0, 1, work->recruitInfo, 0x44);
    return 1;
}

int PPWLobby_TimerPollChannelMode(void *unusedArg) {
    Str s;
    PPWLobbyChannel_GetName(&s, &gPPWLobbyWork->mainChannel);
    LobbyChat_GetChannelModeAsync(!s.l.isLong ? s.s.buf : s.l.ptr, PPWLobby_OnGetChannelMode, 0);
    LobbyString_Destroy(&s);
    return 1;
}

int PPWLobby_TimerRequestChannelTime(void *unusedArg) {
    Str s;
    if (gPPWLobbyWork->schedule.hasTime != 0) {
        return 0;
    }
    PPWLobbyChannel_GetName(&s, &gPPWLobbyWork->mainChannel);
    LobbyChat_GetChannelKeysAsync(!s.l.isLong ? s.s.buf : s.l.ptr, -1, gLobbyStr_b_lib_c_time, PPWLobby_OnGetChannelTime, 0);
    LobbyString_Destroy(&s);
    return 1;
}
}
}

extern "C" {
static inline BOOL limit(int value) {
    return value >= 4;
}

int PPWLobby_TimerRequestLobbyData(void *unusedArg) {
    Str s;
    switch (limit(gPPWLobbyWork->schedule.state)) {
    case 0:
        break;
    default:
        return 0;
    }
    PPWLobbyChannel_GetName(&s, &gPPWLobbyWork->mainChannel);
    LobbyChat_GetChannelKeysAsync(s.l.isLong == 0 ? s.s.buf : s.l.ptr, -1, gLobbyStr_b_lib_c_lobby, PPWLobby_OnGetLobbyData, 0);
    LobbyString_Destroy(&s);
    return 1;
}
}

extern "C" {
int PPWLobby_GetWebResultError(u32 type) {
    switch (type) {
    case 0:
        return 0;
    case 6:
        return 1;
    case 7:
    case 8:
    case 9:
    case 10:
        return 2;
    default:
        return 2;
    }
}

int PPWLobby_ParseWebResponse(BOOL valid, u8 *data, u32 size, void **output, u32 *length) {
    int e;
    *output = 0;
    *length = 0;
    if (!valid) {
        return 2;
    }
    if (size < 4) {
        return 2;
    }
    e = PPWLobby_GetWebResultError(*data);
    if (e == 2) {
        return e;
    }
    if (e == 1) {
        return e;
    }
    *output = data + 4;
    *length = size - 4;
    return e;
}
}

namespace OnCheckProfile {
extern "C" void OS_GetMacAddress(u8 *);

struct Info {
    u32 userId;
    u16 one;
    u8 version;
    u8 language;
    u8 mac[6];
    u8 rest[6];
};
extern "C" int PPW_LobbyUpdateMyProfile(const void *, u32, int);
extern "C" void PPWLobby_OnCheckProfile(BOOL, u8 *, u32, void *);

extern "C" void PPWLobby_OnCheckProfile(BOOL a0, u8 *a1, u32 a2, void *param) {
    Info info;
    const char *key;
    const char *page;
    WebRequest *request;
    u32 pid;
    WebServer server;
    void *buf;
    u32 len;
    PPWLobbyWork *w = gPPWLobbyWork;
    if (w == 0 || w->state == PPW_LOBBY_STATE_CLOSED) {
        return;
    }
    buf = 0;
    len = 0;
    int r = PPWLobby_ParseWebResponse(a0, a1, a2, &buf, &len);
    if (r == 0 && len != 0x94) {
        r = 2;
    }
    if (r == 0) {
        PPW_LobbyUpdateMyProfile(buf, len, 0);
    }
    gPPWLobbyWork->callbacks.onOwnRecord(r, buf, len);
    if (r != 0) {
        PPWLobbyWork *g = gPPWLobbyWork;
        g->error = PPW_LOBBY_ERROR_CHECK_PROFILE;
        g->state = PPW_LOBBY_STATE_ERROR;
        return;
    }
    const PPWLobbyUserProfile *f = (const PPWLobbyUserProfile *)LobbyByteVector_Data(&gPPWLobbyWork->players.self.userProfile);
    info.userId = gPPWLobbyWork->userId;
    info.one = 1;
    info.version = f->version;
    info.language = f->language;
    OS_GetMacAddress(info.mac);
    pid = gPPWLobbyWork->userId;
    page = gPPWLobbyGetScheduleUrl;
    key = gPPWLobbyStatsUploadKey;
    server = gPPWLobbyWork->prefix == 0 ? WEB_SERVER_RELEASE : WEB_SERVER_DEVELOPMENT;
    if (gPPWLobbyCurrentRequest != 0) {
        return;
    }
    request = (WebRequest *)LobbyMem_Alloc(sizeof(WebRequest));
    new (request) WebRequest(server, key, page, pid, (const u8 *)&info, (const u8 *)&info + sizeof(Info), PPWLobby_OnGetSchedule, TRUE);
    SendWebRequest(request);
}
}

#pragma push

namespace OnGetSchedule {
extern "C" void OS_GetMacAddress(u8 *);

struct Vec;
extern "C" void LobbyCharVector_AssignIterRange(Vec *, ByteIterator, ByteIterator, RangeTag);
extern "C" const char *LobbyByteVector_Data(const void *);

struct Vec {
    char *ptr;
    u32 size, cap;

    Vec()
        : ptr(0)
        , size(0)
        , cap(0) {
    }

    void Assign(const ByteIterator &first, const ByteIterator &last) {
        Assign(first, last, AssignTag(), IteratorTag());
    }

    void Assign(const ByteIterator &first, const ByteIterator &last, const AssignTag &, const IteratorTag &) {
        LobbyCharVector_AssignIterRange(this, first, last, RangeTag());
    }

    void Assign(const u8 *first, const u8 *last) {
        Assign(ByteIterator(first), ByteIterator(last));
    }
};

struct Info {
    u32 userId;
    u16 one;
    u8 version;
    u8 language;
    u8 mac[6];
    u16 r14;
    u32 r16;
};

struct Info2 {
    Info info;
    u32 extra;
};
struct Obj;

struct ObjVtable {
    void (*unused)(Obj *request);
    void (*destroy)(Obj *request);
};

extern "C" const ObjVtable gPPWLobbyWebRequestVtable;

// The VIP list request the schedule answer starts: a WebRequest whose body is the Info2 record.
struct Obj {
    const ObjVtable *vtable;
    int retries;
    int server;
    PPWString key;
    PPWString page;
    u32 pid;
    Vec body;
    PPWLobbyWebCallback callback;
    void *param;
    BOOL useLastHash;

    Obj(int server, const char *key, const char *page, u32 pid, const Info2 &info)
        : vtable(&gPPWLobbyWebRequestVtable)
        , retries(3)
        , server(server)
        , key(key)
        , page(page)
        , pid(pid)
        , body()
        , callback(PPWLobby_OnGetVip)
        , param(NULL)
        , useLastHash(TRUE) {
        body.Assign(ByteIterator((const u8 *)&info), ByteIterator((const u8 *)&info + sizeof(Info2)));
    }
};

extern "C" Obj *gPPWLobbyCurrentRequest;
extern "C" void PPWLobby_OnGetSchedule(BOOL, u8 *, u32, void *);
#pragma opt_dead_assignments off

extern "C" void PPWLobby_OnGetSchedule(BOOL a0, u8 *a1, u32 a2, void *param) {
    Info2 i2;
    Info info;
    const char *key;
    const char *page;
    Obj *o;
    u32 pid;
    WebServer server;
    void *buf;
    u32 len;
    PPWLobbyWork *w = gPPWLobbyWork;
    if (w == 0 || w->state == PPW_LOBBY_STATE_CLOSED) {
        return;
    }
    buf = 0;
    len = 0;
    int r = PPWLobby_ParseWebResponse(a0, a1, a2, &buf, &len);
    if (r == 0 && !PPWLobbySchedule_IsSizeValid(&gPPWLobbyWork->schedule, buf, len)) {
        r = 2;
    }
    int ok2 = gPPWLobbyWork->callbacks.onScheduleReceived(r, buf);
    if (r != 0) {
        PPWLobbyWork *g = gPPWLobbyWork;
        g->error = PPW_LOBBY_ERROR_SCHEDULE;
        g->state = PPW_LOBBY_STATE_ERROR;
        return;
    }
    if (ok2) {
        PPWLobbySchedule_Load(&gPPWLobbyWork->schedule, buf, len);
    }
    const PPWLobbyUserProfile *p = (const PPWLobbyUserProfile *)LobbyByteVector_Data(&gPPWLobbyWork->players.self.userProfile);
    int ret = 0;
    std::memset(&i2, 0, sizeof(i2));
    const PPWLobbyUserProfile *f = (const PPWLobbyUserProfile *)LobbyByteVector_Data(&gPPWLobbyWork->players.self.userProfile);
    info.userId = gPPWLobbyWork->userId;
    info.one = 1;
    info.version = f->version;
    info.language = f->language;
    OS_GetMacAddress(info.mac);
    i2.info = info;
    i2.extra = p->trainerId;
    pid = gPPWLobbyWork->userId;
    page = gPPWLobbyGetVipUrl;
    key = gPPWLobbyStatsUploadKey;
    server = gPPWLobbyWork->prefix == 0 ? WEB_SERVER_RELEASE : WEB_SERVER_DEVELOPMENT;
    if (gPPWLobbyCurrentRequest != 0) {
        return;
    }
    o = (Obj *)LobbyMem_Alloc(sizeof(Obj));
    new (o) Obj(server, key, page, pid, i2);
    gPPWLobbyCurrentRequest = o;
    StatsUpload_Start(o->server, o->key.CStr());
    if (o->useLastHash) {
        u32 n = o->body.size;
        ret = StatsUpload_UploadWithLastHash(o->page.CStr(), o->pid, LobbyByteVector_Data(&o->body), n, 0);
    } else {
        u32 n = o->body.size;
        ret = StatsUpload_Upload(o->page.CStr(), o->pid, LobbyByteVector_Data(&o->body), n, 0);
    }
    int ok;
    if (ret == 0) {
        ok = 1;
    } else {
        StatsUpload_Stop();
        ok = 0;
    }
    if (ok) {
        return;
    }
    if (gPPWLobbyCurrentRequest != 0) {
        Obj *q = gPPWLobbyCurrentRequest;
        if (q != 0) {
            q->vtable->destroy(q);
        }
        gPPWLobbyCurrentRequest = 0;
    }
}
}

#pragma pop

namespace OnGetVip {
extern "C" void OS_GetMacAddress(u8 *);

struct Info {
    u32 userId;
    u16 one;
    u8 version;
    u8 language;
    u8 mac[6];
    u8 rest[6];
};
extern "C" void PPWLobby_OnGetVip(BOOL, u8 *, u32, void *);

extern "C" void PPWLobby_OnGetVip(BOOL a0, u8 *a1, u32 a2, void *param) {
    Info info;
    const char *key;
    const char *page;
    WebRequest *request;
    u32 pid;
    WebServer server;
    void *buf;
    u32 len;
    PPWLobbyWork *w = gPPWLobbyWork;
    if (w == 0 || w->state == PPW_LOBBY_STATE_CLOSED) {
        return;
    }
    buf = 0;
    len = 0;
    int r = PPWLobby_ParseWebResponse(a0, a1, a2, &buf, &len);
    if (!PPWLobbySchedule_ValidateVipRecords(&gPPWLobbyWork->schedule, buf, len)) {
        r = 2;
    }
    if (r != 0) {
        PPWLobbyWork *g = gPPWLobbyWork;
        g->error = PPW_LOBBY_ERROR_VIP;
        g->state = PPW_LOBBY_STATE_ERROR;
        return;
    }
    PPWLobbySchedule_SetVipRecords(&gPPWLobbyWork->schedule, buf, len);
    gPPWLobbyWork->callbacks.onSpecialVisitorsReceived(r, buf, len >> 3);
    const PPWLobbyUserProfile *f = (const PPWLobbyUserProfile *)LobbyByteVector_Data(&gPPWLobbyWork->players.self.userProfile);
    info.userId = gPPWLobbyWork->userId;
    info.one = 1;
    info.version = f->version;
    info.language = f->language;
    OS_GetMacAddress(info.mac);
    pid = gPPWLobbyWork->userId;
    page = gPPWLobbyGetQuestionnaireUrl;
    key = gPPWLobbyStatsUploadKey;
    server = gPPWLobbyWork->prefix == 0 ? WEB_SERVER_RELEASE : WEB_SERVER_DEVELOPMENT;
    if (gPPWLobbyCurrentRequest != 0) {
        return;
    }
    request = (WebRequest *)LobbyMem_Alloc(sizeof(WebRequest));
    new (request) WebRequest(server, key, page, pid, (const u8 *)&info, (const u8 *)&info + sizeof(Info), PPWLobby_OnGetQuestionnaire, TRUE);
    SendWebRequest(request);
}
}

extern "C" {

void PPWLobby_OnGetQuestionnaire(BOOL a, u8 *b, u32 c, void *param) {
    void *data;
    u32 size;
    int result;
    PPWLobbyWork *w = gPPWLobbyWork;
    if (!w || w->state == PPW_LOBBY_STATE_CLOSED) {
        return;
    }
    data = 0;
    size = 0;
    result = PPWLobby_ParseWebResponse(a, b, c, &data, &size);
    if (size != 0x2d8) {
        result = 2;
    }
    if (result) {
        {
            PPWLobbyWork *target = gPPWLobbyWork;
            target->error = PPW_LOBBY_ERROR_QUESTIONNAIRE;
            target->state = PPW_LOBBY_STATE_ERROR;
        }
        return;
    }
    PPWLobbySchedule_SetQuestionnaire(&gPPWLobbyWork->schedule, data);
    gPPWLobbyWork->callbacks.onSurveyReceived(result, data);
    gPPWLobbyWork->schedule.state = 2;
}
}

extern "C" {
void PPWLobby_OnSubmitQuestionnaire(BOOL status, u8 *data, u32 size, void *param) {
    if (!gPPWLobbyWork || gPPWLobbyWork->state == PPW_LOBBY_STATE_CLOSED) {
        return;
    }
    gPPWLobbyWork->submittingQuestionnaire = 0;
    gPPWLobbyWork->callbacks.onSurveySubmitted(status ? 0 : 2);
}
}

namespace OnEnterMainChannel {
using std::memcmp;

extern "C" int DWC_GetDateTime(RTCDate *date, RTCTime *time);

struct TrueType {};

struct FalseType {};

template <class T, class U>
inline void Assign(T &d, const U &s, TrueType, FalseType) {
    d = s;
}

template <class T, class U>
inline void Assign(T &d, const U &s) {
    Assign(d, s, TrueType(), FalseType());
}

struct Str {
    u32 words[3];

    Str() {
        u32 *p = words;
        u32 i;
        for (i = 0; i < 3; i++) {
            p[i] = 0;
        }
    }
};

extern "C" void LobbyString_Destroy(Str *s);
extern "C" void LobbyString_Assign(Str *dst, const Str *src);

static inline s64 GetNow(void) {
    RTCDate date;
    RTCTime time;
    DWC_GetDateTime(&date, &time);
    return RTC_ConvertDateTimeToSecond(&date, &time);
}

struct Pair {
    s64 v;

    Pair() {
    }

    Pair(s64 x)
        : v(x) {
    }
};

struct Stamp {
    u32 code;
    Pair pair;
    u32 extra;

    Stamp(u32 c)
        : code(c)
        , pair(0)
        , extra(0) {
        Pair *d = &pair;
        *d = Pair(GetNow());
    }

    Stamp(const Stamp &o)
        : code(o.code) {
        Pair *d = &pair;
        *d = o.pair;
        extra = o.extra;
    }

    Stamp &operator=(const Stamp &o) {
        code = o.code;
        pair = o.pair;
        extra = o.extra;
        return *this;
    }

    bool operator==(const Stamp &o) const {
        return memcmp(this, &o, sizeof(Stamp)) == 0;
    }
};

struct Tracked {
    Stamp stamp;
    int active;

    Tracked(u32 c, int a)
        : stamp(Stamp(c))
        , active(a) {
    }

    void Set(const Tracked &t) {
        const Stamp *s = &t.stamp;
        if (!(stamp == *s)) {
            stamp = *s;
            active = 1;
        }
    }
};

struct Task {
    s64 time;
    s64 period;
    void *arg;
    PPWLobbyTimerCallback func;

    Task() {
    }

    Task(s64 t, u32 p, PPWLobbyTimerCallback f) {
        time = t;
        period = p;
        arg = NULL;
        func = f;
        time -= period;
    }
};

template <class A, class B>
struct KV {
    A first;
    B second;

    template <class C, class D>
    KV(const KV<C, D> &p) {
        Assign(first, p.first);
        const D *src = &p.second;
        B *dst = &second;
        *dst = *src;
    }
};

struct InsertResult {
    void *node;
    bool inserted;
};
struct Map;
extern "C" void LobbyTimerMap_Insert(InsertResult *out, Map *m, const KV<u32, Task> *v);

struct Map {
    u32 words[4];

    InsertResult insert(const KV<u32, Task> &v) {
        InsertResult r;
        LobbyTimerMap_Insert(&r, this, &v);
        return r;
    }
};

extern "C" KV<int, Task> LobbyTimerMap_MakeEntry(int k, const Task &t);

struct Arguments {
    s64 time;
    u32 code;
};

struct Sub {
    void *vt;
    u32 tree[4];
    Tracked tracked;
    LobbyCharVector vec;
    u8 pad34[0x3c - 0x34];
    u32 key;
    int flag;

    void SetFlag(int f) {
        flag = 0;
        if (f) {
            flag = 1;
        }
    }
};

struct Callbacks {
    void *cb8;
    void (*notify)(u32 key, Arguments *args, void *data, u32 size);
};

struct Peer {
    int kind;
    Str name;
    int a;
    int b;

    void Reset() {
        Str empty;
        kind = 5;
        LobbyString_Assign(&name, &empty);
        a = 0;
        b = 0;
        LobbyString_Destroy(&empty);
    }
};

typedef struct Lobby {
    int state;
    int err;
    Callbacks cbs;
    u8 pad10[0x70 - 0x10];
    int retry;
    u8 pad74[0xb8 - 0x74];
    Peer peer;
    u8 padd0[0xe8 - 0xd0];
    Sub sub;
    u8 pad12c[0x134 - 0x12c];
    Map timers;
} Lobby;

extern "C" Lobby *gPPWLobbyWork;
extern "C" int LobbyChat_EnumUsersAsync(const char *handle, LobbyEnumUsersCallback cb, u32 code);

static inline void Lobby_ResetStamp(void) {
    gPPWLobbyWork->retry = 0;
    Tracked t(5, 1);
    Sub *sub = &gPPWLobbyWork->sub;
    sub->tracked.Set(t);
    sub->SetFlag(1);
}

static inline void Lobby_AddTimer(int kind, u32 delay, PPWLobbyTimerCallback cb) {
    Map *m = &gPPWLobbyWork->timers;
    Task task(GetNow(), delay, cb);
    KV<int, Task> d = LobbyTimerMap_MakeEntry(kind, task);
    m->insert(d);
}

static inline Arguments Lobby_GetArgs(void) {
    const Stamp &s = gPPWLobbyWork->sub.tracked.stamp;
    Arguments a;
    a.code = s.code;
    a.time = s.pair.v;
    return a;
}

static inline void Lobby_Notify(Arguments *args) {
    Callbacks *cbs = &gPPWLobbyWork->cbs;
    Sub *sub = &gPPWLobbyWork->sub;
    u32 key = sub->key;
    cbs->notify(key, args, LobbyByteVector_Data(&sub->vec), sub->vec.size);
}

static inline int Lobby_Retry(Lobby *w) {
    if (w->retry >= 3) {
        return 0;
    }
    w->retry++;
    return 1;
}

static inline void Lobby_SetError(int code) {
    Lobby *w = gPPWLobbyWork;
    if (code != 0) {
        w->err = code;
        w->state = PPW_LOBBY_STATE_ERROR;
    }
}

static inline void Lobby_OnFail(void) {
    gPPWLobbyWork->peer.Reset();
    if (Lobby_Retry(gPPWLobbyWork)) {
        gPPWLobbyWork->state = PPW_LOBBY_STATE_ENUM_CHANNELS;
    } else {
        Lobby_SetError(3);
    }
}

extern "C" void PPWLobby_OnEnterMainChannel(int success, int unused, const char *handle, u32 code) {
    if (code == 5) {
        return;
    }
    if (success) {
        Lobby_ResetStamp();
        LobbyChat_EnumUsersAsync(handle, PPWLobby_OnEnumUsers, code);
        Lobby_AddTimer(1, 3, PPWLobby_TimerUploadSystemProfile);
        Lobby_AddTimer(2, 4, PPWLobby_TimerUploadUserProfile);
        Lobby_AddTimer(5, 4, PPWLobby_TimerRequestChannelTime);
        Lobby_AddTimer(4, 3, PPWLobby_TimerPollChannelMode);
        Lobby_AddTimer(6, 4, PPWLobby_TimerRequestLobbyData);
        Arguments args = Lobby_GetArgs();
        Lobby_Notify(&args);
    } else {
        Lobby_OnFail();
    }
}
}

namespace OnEnterSubChannel {
struct Peer {
    int kind;
    PPWString name;
    int a;
    int b;

    void Reset() {
        PPWString empty;
        kind = 5;
        name = empty;
        a = 0;
        b = 0;
        empty.Destroy();
    }
};

struct Work {
    int state;
    int sub;
    void (*callback)(int, int);
    u8 padc[0xd0 - 0xc];
    Peer peer;
    u8 pade8[0xfc - 0xe8];
    SystemProfileEntry systemProfile;
    u8 pad110[0x128 - 0x110];
    int systemProfileReceived;
};

extern "C" Work *gPPWLobbyWork;
extern "C" char gPPWLobbyFootChannelMode[];
extern "C" char gPPWLobbyClockChannelMode[];
extern "C" char gPPWLobbyNewsChannelMode[];
extern "C" int LobbyChat_SetChannelMode(const char *, const void *);
#pragma opt_common_subs off

extern "C" void PPWLobby_OnEnterSubChannel(int result, int unused, const char *arg, u32 kind) {
    if (kind == 5) {
        return;
    }
    if (result != 0) {
        const char *name;
        switch (kind) {
        case 1:
        case 2:
            name = gPPWLobbyFootChannelMode;
            break;
        case 3:
            name = gPPWLobbyClockChannelMode;
            break;
        case 4:
            name = gPPWLobbyNewsChannelMode;
            break;
        default:
            name = 0;
            break;
        }
        if (name == 0) {
            Work *w = gPPWLobbyWork;
            w->sub = 1;
            w->state = PPW_LOBBY_STATE_ERROR;
        }
        LobbyChat_SetChannelMode(arg, name);
        {
            Work *w = gPPWLobbyWork;
            w->systemProfile.changed = 0;
            SystemProfile info;
            info = w->systemProfile.profile;
            info.channel = kind;
            SystemProfileEntry e(info);
            const SystemProfile &ni = e.profile;
            if (!(w->systemProfile.profile == ni)) {
                w->systemProfile.Assign(ni);
            }
            w->systemProfileReceived = 1;
        }
    } else {
        gPPWLobbyWork->peer.Reset();
    }
    gPPWLobbyWork->callback(result, kind);
}

#pragma opt_common_subs reset
}

namespace OnPlayerJoined {
extern "C" PlayerView *PPWLobbyPlayers_Find(void *, int);
extern "C" BOOL PPWLobbyPlayers_Add(void *, int);

inline int Match(PPWLobbyChannel *c, const Str &name) {
    Str t;
    int r;
    PPWLobbyChannel_GetName(&t, c);
    r = LobbyString_Equals(t, name);
    LobbyString_Destroy(&t);
    return r;
}

inline int Find(PPWLobbyWork *w, const Str &name) {
    const Str &n = name;
    if (Match(&w->mainChannel, n)) {
        return w->mainChannel.kind;
    }
    if (Match(&w->subChannel, n)) {
        return w->subChannel.kind;
    }
    return 5;
}

inline void UpdatePlayerChannel(PlayerView *s, SystemProfile &info, PlayerRecord &arg, int kind) {
    s->systemProfile.changed = 0;
    info = s->systemProfile.profile;
    info.channel = kind;
    SystemProfileEntry e(info);
    const SystemProfile &ni = e.profile;
    if (!(s->systemProfile.profile == ni)) {
        s->systemProfile.Assign(ni);
    }
    s->systemProfileReceived = 1;
    s->systemProfile.changed = 0;
}

inline void MakeSummary(PlayerView *s, PlayerRecord &arg) {
    PlayerRecord sum(s->systemProfile.profile);
    arg = sum;
}

#pragma opt_common_subs off

extern "C" void PPWLobby_OnPlayerJoined(const char *channel, int id, void *param) {
    SystemProfile info;
    PlayerRecord arg;
    int kind;
    {
        Str name;
        LobbyString_InitFromCStr(&name, channel);
        kind = Find(gPPWLobbyWork, name);
        LobbyString_Destroy(&name);
    }
    if (kind == 5) {
        return;
    }
    if (kind == 0) {
        PPWLobbyPlayers_Add(&gPPWLobbyWork->players, id);
        return;
    }
    PlayerView *s = PPWLobbyPlayers_Find(&gPPWLobbyWork->players, id);
    if (s == 0) {
        PPWLobbyPlayers_Add(&gPPWLobbyWork->players, id);
        s = PPWLobbyPlayers_Find(&gPPWLobbyWork->players, id);
    }
    UpdatePlayerChannel(s, info, arg, kind);
    MakeSummary(s, arg);
    if (s->entered) {
        gPPWLobbyWork->callbacks.onPlayerRecordChanged(id, &arg);
    }
}

#pragma opt_common_subs reset
}

namespace OnPlayerLeft {
extern "C" PlayerView *PPWLobbyPlayers_Find(void *, int);
extern "C" BOOL PPWLobbyPlayers_Remove(void *, int);

inline int Match(PPWLobbyChannel *c, const Str &name) {
    Str t;
    int r;
    PPWLobbyChannel_GetName(&t, c);
    r = LobbyString_Equals(t, name);
    LobbyString_Destroy(&t);
    return r;
}

inline int Find(PPWLobbyWork *w, const Str &name) {
    const Str &n = name;
    if (Match(&w->mainChannel, n)) {
        return w->mainChannel.kind;
    }
    if (Match(&w->subChannel, n)) {
        return w->subChannel.kind;
    }
    return 5;
}

inline void UpdatePlayerChannel(PlayerView *s, SystemProfile &info, PlayerRecord &arg) {
    s->systemProfile.changed = 0;
    info = s->systemProfile.profile;
    info.channel = 5;
    SystemProfileEntry e(info);
    const SystemProfile &ni = e.profile;
    if (!(s->systemProfile.profile == ni)) {
        s->systemProfile.Assign(ni);
    }
    s->systemProfileReceived = 1;
    s->systemProfile.changed = 0;
}

inline void MakeSummary(PlayerView *s, PlayerRecord &arg) {
    PlayerRecord sum(s->systemProfile.profile);
    arg = sum;
}

#pragma opt_common_subs off

extern "C" void PPWLobby_OnPlayerLeft(const char *channel, int id, void *param) {
    SystemProfile info;
    PlayerRecord arg;
    int kind;
    {
        Str name;
        LobbyString_InitFromCStr(&name, channel);
        kind = Find(gPPWLobbyWork, name);
        LobbyString_Destroy(&name);
    }
    if (kind == 5) {
        return;
    }
    if (kind == 0) {
        PlayerView *s = PPWLobbyPlayers_Find(&gPPWLobbyWork->players, id);
        if (s == 0 || s->entered == 0) {
            return;
        }
        gPPWLobbyWork->callbacks.onPlayerLeave(id);
        PPWLobbyPlayers_Remove(&gPPWLobbyWork->players, id);
        return;
    }
    PlayerView *s = PPWLobbyPlayers_Find(&gPPWLobbyWork->players, id);
    if (s == 0) {
        return;
    }
    UpdatePlayerChannel(s, info, arg);
    MakeSummary(s, arg);
    if (s->entered) {
        gPPWLobbyWork->callbacks.onPlayerRecordChanged(id, &arg);
    }
}

#pragma opt_common_subs reset
}

extern "C" {
void PPWLobby_OnPlayerMessage(int a, void *b, void *c) {
    PPWLobby_OnMessage(0, a, b, c);
}
}

#pragma push

namespace OnMessage {
extern "C" {
typedef struct {
    int a, b, type, kind, arg, spare;
    LobbyCharVector text;
} Message;

extern void PPWLobby_HandleSystemMessage(Message *, int, int, void *, u32);
extern int PPWLobby_ParseMessage(Str *, Message *);
extern PPWLobbyPlayer *PPWLobbyPlayers_Find(void *, int);

static inline void zeroString(LobbyCharVector *s) {
    u32 *p = (u32 *)s;
    *p++ = 0;
    *p++ = 0;
    *p = 0;
}

#pragma opt_common_subs off

static inline int validEntry(PPWLobbyPlayer *p) {
    if (p && p->entered) {
        return 1;
    }
    return 0;
}

void PPWLobby_OnMessage(const char *key, int index, void *name, void *param) {
    Message m;
    Str k, tmp, b, a;
    int channel = 5;
    if (key) {
        PPWLobbyWork *c;
        Str *kp;
        int match;
        LobbyString_InitFromCStr(&k, key);
        c = gPPWLobbyWork;
        kp = &k;
        PPWLobbyChannel_GetName(&a, &c->mainChannel);
        match = LobbyString_Equals(a, *kp);
        LobbyString_Destroy(&a);
        if (match) {
            channel = c->mainChannel.kind;
        } else {
            PPWLobbyChannel_GetName(&b, &c->subChannel);
            match = LobbyString_Equals(b, *kp);
            LobbyString_Destroy(&b);
            if (match) {
                channel = c->subChannel.kind;
            } else {
                channel = 5;
            }
        }
        LobbyString_Destroy(&k);
    }
    if (key && channel == 5) {
        return;
    }
    {
        int match;
        int zero = 0;
        m.a = zero;
        m.b = 6;
        m.type = 0;
        m.kind = 0;
        m.arg = 0;
        m.spare = 0;
        zeroString(&m.text);
        LobbyString_InitFromCStr(&tmp, (const char *)name);
        match = PPWLobby_ParseMessage(&tmp, &m) == 0 ? 1 : zero;
        LobbyString_Destroy(&tmp);
        if (match) {
            LobbyCharVector_Destroy(&m.text);
            return;
        }
        switch (m.kind) {
        case 1: {
            PPWLobbyPlayer *p = PPWLobbyPlayers_Find(&gPPWLobbyWork->players, index);
            if (!validEntry(p)) {
                LobbyCharVector_Destroy(&m.text);
                return;
            }
            switch (m.type) {
            case 1: {
                PPWLobbyWork *c = gPPWLobbyWork;
                void *t = LobbyByteVector_Data(&m.text);
                c->callbacks.onMessage(index, channel, m.arg, t, m.text.size);
                break;
            }
            case 0: {
                PPWLobbyWork *c = gPPWLobbyWork;
                void *t = LobbyByteVector_Data(&m.text);
                c->callbacks.onEvent3(index, channel, m.arg, t);
                break;
            }
            }
            break;
        }
        case 0: {
            void *t = LobbyByteVector_Data(&m.text);
            PPWLobby_HandleSystemMessage(&m, index, channel, t, m.text.size);
            break;
        }
        }
        LobbyCharVector_Destroy(&m.text);
    }
}
}
}

#pragma pop

extern "C" {
void PPWLobby_OnExcessFlood(int a, void *param) {
    gPPWLobbyWork->callbacks.onExcessFlood(a);
}

void PPWLobby_OnChannelDataChanged(const char *a, int b, void *c, void *d, void *param) {
    PPWLobby_HandleReceivedData(1, 1, a, b, (const char *)c, (const char *)d, 0);
}

void PPWLobby_OnGetChannelKeys(BOOL a, const char *b, int c, const char *d, const char *e, u32 f) {
    PPWLobby_HandleReceivedData(a, 0, b, c, d, e, f);
}
}

extern "C" {
extern char gLobbyStr_Bang[];
extern void LobbyString_Split(LobbyStringVector *, Str *, Str *, int);

void PPWLobby_OnEnumChannels(int valid, u32 count, const char *const *names, u32 unused, const int *types, u32 param) {
    LobbyStringVector parts;
    Str item;
    Str delimiter;
    Str text;
    Str empty;
    int i;
    if (!valid) {
        return;
    }
    for (i = 0; i < count; i++) {
        if (types[i] >= 20) {
            continue;
        }
        LobbyString_InitFromCStr(&item, names[i]);
        LobbyString_InitFromCStr(&delimiter, gLobbyStr_Bang);
        LobbyString_Split(&parts, &item, &delimiter, 0);
        LobbyString_Destroy(&delimiter);
        LobbyString_Destroy(&item);
        if (parts.size < 2) {
            LobbyStringVector_Destroy(&parts);
            continue;
        }
        {
            Str *s = parts.data + 1;
            char *p = s->l.isLong == 0 ? s->s.buf : s->l.ptr;
            LobbyString_InitFromCStr(&text, p);
        }
        LobbyString_Assign(&gPPWLobbyWork->channelBaseName, &text);
        LobbyString_Destroy(&text);
        gPPWLobbyWork->isNewChannel = 0;
        gPPWLobbyWork->state = PPW_LOBBY_STATE_ENTER_MAIN_CHANNEL;
        LobbyStringVector_Destroy(&parts);
        return;
    }
    PPWLobby_GenerateChannelBaseName(&empty);
    LobbyString_Assign(&gPPWLobbyWork->channelBaseName, &empty);
    LobbyString_Destroy(&empty);
    gPPWLobbyWork->isNewChannel = 1;
    gPPWLobbyWork->state = PPW_LOBBY_STATE_ENTER_MAIN_CHANNEL;
}
}

namespace StringSplit {
struct Str;
extern "C" Str *LobbyString_InitCopy(Str *, const Str *);
extern "C" void LobbyString_Destroy(Str *);
extern "C" void LobbyString_Assign(Str *, const Str *);
extern "C" int LobbyString_Find(const Str *, const Str *, int);
extern "C" void LobbyString_Substr(Str *, const Str *, int, int);

struct Str {
    union {
        struct {
            u32 flag : 1;
            u32 capacity : 31;
            u32 size;
            char *data;
        } l;

        struct {
            u8 flag : 1;
            u8 size : 7;
            char data[11];
        } s;

        u32 words[3];
    } raw;

    Str() {
    }

    Str(const Str &o) {
        LobbyString_InitCopy(this, &o);
    }

    u32 size() const {
        return !raw.l.flag ? raw.s.size : raw.l.size;
    }

    void Destroy() {
        LobbyString_Destroy(this);
    }
};

class Alloc {};
struct Vec;
extern "C" void LobbyStringVector_PushBackSlow(Vec *, const Str &, Alloc);

struct Vec {
    Str *data;
    u32 count, cap;

    Alloc get_alloc() const {
        return Alloc();
    }

    Vec()
        : data(0)
        , count(0)
        , cap(0) {
    }

    void push_back(const Str &x) {
        Alloc a = get_alloc();
        (void)a;
        if (count < cap) {
            new (data + count) Str(x);
            count++;
        } else {
            LobbyStringVector_PushBackSlow(this, x, Alloc());
        }
    }
};

extern "C" Vec LobbyString_Split(const Str &s, const Str &delim, int max);

extern "C" Vec LobbyString_Split(const Str &s, const Str &delim, int max) {
    Vec out;
    int n = 1;
    Str rest(s);
    int pos;
    while (n != max && (pos = LobbyString_Find(&rest, &delim, 0)) != -1) {
        if (pos != 0) {
            Str piece;
            LobbyString_Substr(&piece, &rest, 0, pos);
            out.push_back(piece);
            piece.Destroy();
            n++;
        }
        Str tmp;
        LobbyString_Substr(&tmp, &rest, pos + delim.size(), -1);
        LobbyString_Assign(&rest, &tmp);
        tmp.Destroy();
    }
    if (rest.size() != 0) {
        out.push_back(rest);
    }
    rest.Destroy();
    return out;
}
}

extern "C" {
void LobbyString_Substr(Str *, Str *, u32, u32);

void LobbyString_Substr(Str *a, Str *b, u32 c, u32 d) {
    Str *volatile copy = b;
    LobbyString_InitSubstring(a, b, c, d);
}
}

extern "C" BOOL PPWLobbyPlayers_Add(void *, int);

inline int Find(PPWLobbyWork *w, const PPWString &name) {
    const PPWString &n = name;
    int eq;
    {
        Str t;
        PPWLobbyChannel_GetName(&t, &w->mainChannel);
        eq = LobbyString_Equals(t, n.str);
        LobbyString_Destroy(&t);
    }
    if (eq) {
        return w->mainChannel.kind;
    }
    {
        Str t;
        PPWLobbyChannel_GetName(&t, &w->subChannel);
        eq = LobbyString_Equals(t, n.str);
        LobbyString_Destroy(&t);
    }
    if (eq) {
        return w->subChannel.kind;
    }
    return 5;
}

inline bool IsSelf(int id) {
    return id == gPPWLobbyWork->userId;
}

#pragma opt_common_subs off

extern "C" void PPWLobby_OnEnumUsers(int success, const char *channel, u32 count, int *ids, u32 param) {
    int result, i;
    if (success == 0) {
        return;
    }
    {
        PPWString name(channel);
        PPWLobbyWork *w = gPPWLobbyWork;
        result = Find(w, name);
        name.Destroy();
    }
    if (result == 5) {
        return;
    }
    if (result != 0) {
        return;
    }
    for (i = 0; i < count; i++) {
        if (!IsSelf(ids[i])) {
            PPWLobbyPlayers_Add(&gPPWLobbyWork->players, ids[i]);
            LobbyChat_GetChannelKeysAsync(channel, ids[i], gLobbyStr_b_lib_u_user, PPWLobby_OnGetChannelKeys, 0);
            LobbyChat_GetChannelKeysAsync(channel, ids[i], gLobbyStr_b_lib_u_system, PPWLobby_OnGetChannelKeys, 0);
        }
    }
    gPPWLobbyWork->mainChannel.usersEnumerated = 1;
}

#pragma opt_common_subs reset

extern "C" {
extern u8 gPPWLobbyMainChannelMode[];
extern int LobbyChat_SetChannelMode(const char *, const void *);

void PPWLobby_OnGetChannelMode(int a, const char *b, u32 mode, u32 param) {
    const u32 *c = (const u32 *)mode;
    u32 timerId;
    if (!a) {
        return;
    }
    if (c[7] == 20) {
        gPPWLobbyWork->mainChannel.modeReady = 1;
        timerId = 4;
        LobbyTimerMap_EraseKey(&gPPWLobbyWork->timers, &timerId);
        return;
    }
    LobbyChat_SetChannelMode(b, (char *)gPPWLobbyMainChannelMode);
}
}

namespace OnGetChannelTime {
struct Stamp {
    s64 v;
};

struct DateTime {
    RTCDate date;
    RTCTime time;
};

extern "C" int PPWLobby_DecodeMessageData(const char *, LobbyCharVector *);
extern "C" int DWC_GetDateTime(RTCDate *, RTCTime *);

extern "C" void PPWLobby_OnGetChannelTime(BOOL result, const char *channel, int userId, const char *key, const char *value, u32 param) {
    if (result == 0) {
        return;
    }
    ByteVector data;
    if (PPWLobby_DecodeMessageData(value, &data) == 0 || data.size != 8) {
        PPWLobbyWork *w = gPPWLobbyWork;
        DateTime now;
        DWC_GetDateTime(&now.date, &now.time);
        w->schedule.time.seconds = RTC_ConvertDateTimeToSecond(&now.date, &now.time);
        Stamp s;
        s.v = w->schedule.time.seconds;
        PPWLobby_SetChannelData(-1, 0, gLobbyStr_b_lib_c_time, &s, 8);
        data.Destroy();
        return;
    }
    {
        Stamp s = *(Stamp *)LobbyByteVector_Data(&data);
        PPWLobbyWork *w = gPPWLobbyWork;
        w->schedule.hasTime = 1;
        w->schedule.time.seconds = s.v;
        u32 timerId = 5;
        LobbyTimerMap_EraseKey(&gPPWLobbyWork->timers, &timerId);
    }
    data.Destroy();
}
}

extern "C" {
extern int PPWLobby_DecodeMessageData(const char *, LobbyCharVector *);

static inline int appendBlob(LobbyCharVector *blob) {
    PPWLobbyWork *w = gPPWLobbyWork;
    return PPWLobbySchedule_Load(&w->schedule, LobbyByteVector_Data(blob), blob->size);
}

void PPWLobby_OnGetLobbyData(BOOL success, const char *channel, int userId, const char *key, const char *value, u32 param) {
    LobbyCharVector blob, copy;
    u32 timerId;
    if (success == FALSE) {
        return;
    }
    blob.data = 0;
    blob.size = 0;
    blob.capacity = 0;
    if (!PPWLobby_DecodeMessageData(value, &blob) || blob.size == 0 || !appendBlob(&blob)) {
        PPWLobbySchedule_CopyToVector(&copy, &gPPWLobbyWork->schedule);
        PPWLobby_SetChannelData(-1, 0, gLobbyStr_b_lib_c_lobby, LobbyByteVector_Data(&copy), copy.size);
        LobbyCharVector_Destroy(&copy);
        LobbyCharVector_Destroy(&blob);
        return;
    }
    gPPWLobbyWork->schedule.state = 4;
    timerId = 6;
    LobbyTimerMap_EraseKey(&gPPWLobbyWork->timers, &timerId);
    LobbyCharVector_Destroy(&blob);
}
}

extern "C" {
extern u8 gPPWLobbyPlayersVtable[];
void PPWLobby_OnTopicChanged(const char *channel, void *topic, void *param) {
}

void *PPWLobbyWebRequest_Destroy(PPWLobbyWebRequest *request) {
    LobbyCharVector_Destroy(&request->body);
    LobbyString_Destroy(&request->page);
    LobbyString_Destroy(&request->key);
    return request;
}

void *PPWLobbyPlayers_Delete(PPWLobbyPlayers *players) {
    players->vtable = gPPWLobbyPlayersVtable;
    PPWLobbyPlayers_Clear(players);
    LobbyCharVector_Destroy(&players->self.userProfile);
    PPWLobbyPlayerSet_Destroy(&players->playerSet);
    if (players) {
        DWC_Free(0, players, 0);
    }
    return players;
}

void *PPWLobbyPlayer_Delete(PPWLobbyPlayer *player) {
    LobbyCharVector_Destroy(&player->userProfile);
    if (player) {
        DWC_Free(0, player, 0);
    }
    return player;
}
}

extern "C" {
typedef struct Queue {
    u32 a;
    volatile u32 b;
    void **base;
    u32 start;
    u32 count, capacity;
} Queue;

extern void *LobbyEventDequeMap_Destroy(void *);
Queue *LobbyEventDeque_Destroy(Queue *);

Queue *LobbyEventDeque_Destroy(Queue *p) {
    void **end, **cursor, **base, **limit;
    u32 capacity, start;
    u32 index = 0;
    volatile u8 tag = 0;
    p->b = 0;
    if (p->count) {
        if (p->capacity) {
            index = (p->start + ((volatile Queue *)p)->count) % p->capacity;
        }
        base = p->base;
        start = p->start;
        capacity = ((volatile Queue *)p)->capacity;
        end = base + index;
        cursor = base + start;
        limit = base + capacity;
        while (cursor != end) {
            if (*cursor) {
                DWC_Free(0, *cursor, 0);
            }
            if (++cursor == limit) {
                cursor = base;
            }
        }
    }
    LobbyEventDequeMap_Destroy(&p->base);
    return p;
}
}

extern "C" {
void *PPWLobbyPlayerSet_Destroy(PPWLobbyPlayerSet *set) {
    if (set->root) {
        PPWLobbyPlayerSet_DestroyNodes(set, set->root);
    }
    return set;
}
}

extern "C" {
typedef struct Result {
    LobbyTimerMapNode *node;
    u8 flag;
    char pad[3];
} Result;

extern LobbyTimerMapNode *LobbyTimerMap_InsertNode(LobbyTimerMap *, LobbyTimerMapNode *, int, int, u32 *);
void LobbyTimerMap_Insert(Result *, LobbyTimerMap *, u32 *);

void LobbyTimerMap_Insert(Result *r, LobbyTimerMap *map, u32 *v) {
    u8 second, first;
    LobbyTimerMapNode *node;
    LobbyTimerMapNode *parent = LobbyTimerMap_FindInsertPos(map, v, &node, &second, &first);
    if (!node || node->id < *v) {
        r->node = LobbyTimerMap_InsertNode(map, parent, second, first, v);
        r->flag = gLobbyTimerMapInserted;
    } else {
        r->node = node;
        r->flag = gLobbyTimerMapNotInserted;
    }
}
}

extern "C" {
void LobbyTimerMap_DestroyNodes(LobbyTimerMap *map, LobbyTimerMapNode *node);
LobbyTimerMapNode *LobbyTree_EraseRebalance(LobbyTimerMapNode *node, LobbyTimerMapNode *root);
void LobbyTimerMap_Erase(Iterator *result, LobbyTimerMap *map, Iterator position);

void LobbyTimerMap_Erase(Iterator *result, LobbyTimerMap *map, Iterator position) {
    LobbyTimerMapNode *node = (LobbyTimerMapNode *)position.current;
    if (node == map->leftmost) {
        LobbyTreeIterator_Advance(&position);
        map->leftmost = (LobbyTimerMapNode *)position.current;
    }
    result->current = LobbyTree_EraseRebalance(node, map->root);
    if (node) {
        DWC_Free(0, node, 0);
    }
    map->count--;
}

void LobbyTimerMap_Clear(LobbyTimerMap *map) {
    if (!map->root) {
        return;
    }
    LobbyTimerMap_DestroyNodes(map, map->root);
    map->count = 0;
    map->root = NULL;
    map->leftmost = (LobbyTimerMapNode *)&map->root;
}
}

namespace TimerMapInsertNode {
extern "C" char gLobbyStr_TreeInsertLengthError[];

class Alloc {};

struct Inner {
    u64 x;
    u64 y;
    u32 p;
    u32 q;
};

template <class A, class B>
struct Pair {
    A first;
    B second;

    Pair(const A &a, const B &b)
        : first(a) {
        B *dst = &second;
        *dst = b;
    }
};

typedef Pair<u32, Inner> Value;

struct Node {
    Node *left, *right, *parent;
    Value val;
};

struct Tree {
    u32 count;
    Node *root;
    u32 unk8;
    Node *leftmost;
};

class Holder {
public:
    Node *a;
    Node *b;
    Node **alloc;
    Node *volatile ptr;

    Holder(Node *n, Node **al)
        : a(n)
        , b(n)
        , alloc(al)
        , ptr(n) {
    }

    void Destroy() {
        Node *p = ptr;
        if (p) {
            DWC_Free(0, p, 0);
        }
    }

    void Release() {
        ptr = 0;
    }
};

inline Node *Allocate(Alloc) {
    return (Node *)LobbyMem_Alloc(sizeof(Node));
}

extern "C" void LobbyTree_InsertRebalance(Node *, Node *);
extern "C" Node *LobbyTimerMap_InsertNode(Tree *, Node *, int, int, Value *);

extern "C" Node *LobbyTimerMap_InsertNode(Tree *t, Node *parent, int left, int leftmost, Value *v) {
    if (t->count == (u32)-1) {
        printf(gLobbyStr_TreeInsertLengthError);
        abort();
    }
    Node *n = Allocate(Alloc());
    Holder h(n, &t->root);
    new (&n->val) Value(v->first, v->second);
    n->right = 0;
    h.Release();
    n->left = 0;
    new (&n->parent) Node *(parent);
    if (left) {
        parent->left = n;
    } else {
        parent->right = n;
    }
    t->count++;
    LobbyTree_InsertRebalance(n, t->root);
    if (leftmost) {
        t->leftmost = n;
    }
    h.Destroy();
    return n;
}
}

extern "C" {
void LobbyTimerMap_DestroyNodes(LobbyTimerMap *map, LobbyTimerMapNode *node) {
    if (node->left) {
        LobbyTimerMap_DestroyNodes(map, node->left);
    }
    if (node->right) {
        LobbyTimerMap_DestroyNodes(map, node->right);
    }
    if (node) {
        DWC_Free(0, node, 0);
    }
}

void *LobbyTimerMap_Destroy(LobbyTimerMap *map) {
    if (map->root) {
        LobbyTimerMap_DestroyNodes(map, map->root);
    }
    return map;
}
}

namespace TreeIterator {
struct TreeNode {
    void *left;
    void *right;
    u32 parent;
};

static inline void *Left(void *node) {
    return static_cast<TreeNode *>(node)->left;
}

static inline void *Right(void *node) {
    return static_cast<TreeNode *>(node)->right;
}

static inline void *Parent(void *node) {
    return reinterpret_cast<void *>(static_cast<TreeNode *>(node)->parent & ~1UL);
}

extern "C" void LobbyTreeIterator_Advance(Iterator *it);

extern "C" void LobbyTreeIterator_Advance(Iterator *it) {
    if (Right(it->current)) {
        void *next = Right(it->current);
        void *left;
        while ((left = Left(next)) != 0) {
            next = left;
        }
        it->current = next;
        return;
    }
    void *node = it->current;
    void *parent = Parent(node);
    while (node != Left(parent)) {
        it->current = parent;
        node = it->current;
        parent = Parent(node);
    }
    it->current = parent;
}
}

namespace TreeInsertRebalance {
extern "C" {
typedef struct Node {
    void *left, *right;
    u32 parent;
} Node;

static inline Node *Parent(Node *n) {
    return (Node *)(n->parent & ~1);
}

extern void LobbyTree_RotateLeft(Node *, void **);
extern void LobbyTree_RotateRight(Node *, void **);

static inline int Red(Node *n) {
    return (n->parent & 1) ? 1 : 0;
}

static inline void Black(Node *n) {
    n->parent &= ~1;
}

static inline void Paint(Node *n) {
    n->parent |= 1;
}

void LobbyTree_InsertRebalance(Node *, void *);

void LobbyTree_InsertRebalance(Node *n, void *volatile root) {
    Paint(n);
    while (n != root && Red(Parent(n)) == 1) {
        Node *p = Parent(n), *g = Parent(p);
        if (p == g->left) {
            Node *u = (Node *)g->right;
            if (u && Red(u) == 1) {
                Black(p);
                Black(u);
                n = Parent(Parent(n));
                Paint(n);
            } else {
                if (n == p->right) {
                    n = p;
                    LobbyTree_RotateLeft(n, (void **)&root);
                }
                Black(Parent(n));
                Paint(Parent(Parent(n)));
                LobbyTree_RotateRight(Parent(Parent(n)), (void **)&root);
            }
        } else {
            Node *u = (Node *)g->left;
            if (u && Red(u) == 1) {
                Black(p);
                Black(u);
                n = Parent(Parent(n));
                Paint(n);
            } else {
                if (n == p->left) {
                    n = p;
                    LobbyTree_RotateRight(n, (void **)&root);
                }
                Black(Parent(n));
                Paint(Parent(Parent(n)));
                LobbyTree_RotateLeft(Parent(Parent(n)), (void **)&root);
            }
        }
    }
    Black((Node *)root);
}
}
}

namespace TreeEraseRebalance {
extern "C" {
typedef struct Node {
    struct Node *left, *right;
    u32 tagged;
} Node;

extern void LobbyTree_RotateLeft(Node *, void **);
extern void LobbyTree_RotateRight(Node *, void **);
Node *LobbyTree_EraseRebalance(Node *, void *);

static inline Node *parent(Node *n) {
    return (Node *)(n->tagged & ~1);
}

static inline int red(Node *n) {
    if (*(volatile u32 *)&n->tagged & 1) {
        return 1;
    }
    return 0;
}

static inline void setParent(Node *n, Node *p) {
    n->tagged = (u32)p | (n->tagged & 1);
}

static inline void setRed(Node *n, int r) {
    u32 t = n->tagged;
    if (r) {
        t |= 1;
    } else {
        t &= ~1;
    }
    n->tagged = t;
}

static inline int redLoaded(Node *n, u32 *tag) {
    u32 t = n->tagged;
    *tag = t;
    if (t & 1) {
        return 1;
    }
    return 0;
}

Node *LobbyTree_EraseRebalance(Node *node, void *root) {
    Iterator iter = { node };
    Node *removed, *child, *up;
    int left, fix;
    LobbyTreeIterator_Advance(&iter);
    removed = node;
    if (node->left && node->right) {
        removed = (Node *)iter.current;
    }
    child = removed->left;
    if (!child) {
        child = removed->right;
    }
    up = parent(removed);
    if (child) {
        setParent(child, up);
    }
    {
        Node *p = parent(removed);
        if (removed == p->left) {
            p->left = child;
            left = 1;
        } else {
            p->right = child;
            left = 0;
        }
    }
    fix = !red(removed);
    if (removed != node) {
        Node *p = parent(node);
        setParent(removed, p);
        p = parent(removed);
        if (node == p->left) {
            p->left = removed;
        } else {
            p->right = removed;
        }
        {
            Node *c = node->left;
            removed->left = c;
            if (!c) {
                up = removed;
            } else {
                setParent(c, removed);
            }
        }
        {
            Node *c = node->right;
            removed->right = c;
            if (!c) {
                up = removed;
            } else {
                setParent(c, removed);
            }
        }
        setRed(removed, red(node));
        if (root == node) {
            root = removed;
        }
    } else if (!child && node == root) {
        fix = 0;
    }
    if (fix) {
        while (child != root && (!child || !red(child))) {
            Node *sibling;
            u32 colorword;
            if (left) {
                sibling = up->right;
                if (redLoaded(sibling, &colorword) == 1) {
                    sibling->tagged = colorword & ~1;
                    setRed(up, 1);
                    LobbyTree_RotateLeft(up, &root);
                    sibling = up->right;
                }
                if ((!sibling->left || !red(sibling->left)) && (!sibling->right || !red(sibling->right))) {
                    setRed(sibling, 1);
                    child = up;
                    up = parent(up);
                    left = (child == up->left);
                } else {
                    if (!sibling->right || !red(sibling->right)) {
                        setRed(sibling->left, 0);
                        setRed(sibling, 1);
                        LobbyTree_RotateRight(sibling, &root);
                        sibling = up->right;
                    }
                    setRed(sibling, red(up));
                    setRed(up, 0);
                    setRed(sibling->right, 0);
                    LobbyTree_RotateLeft(up, &root);
                    child = (Node *)root;
                }
            } else {
                sibling = up->left;
                if (redLoaded(sibling, &colorword) == 1) {
                    sibling->tagged = colorword & ~1;
                    setRed(up, 1);
                    LobbyTree_RotateRight(up, &root);
                    sibling = up->left;
                }
                if ((!sibling->left || !red(sibling->left)) && (!sibling->right || !red(sibling->right))) {
                    setRed(sibling, 1);
                    child = up;
                    up = parent(up);
                    left = (child == up->left);
                } else {
                    if (!sibling->left || !red(sibling->left)) {
                        setRed(sibling->right, 0);
                        setRed(sibling, 1);
                        LobbyTree_RotateLeft(sibling, &root);
                        sibling = up->left;
                    }
                    setRed(sibling, red(up));
                    setRed(up, 0);
                    setRed(sibling->left, 0);
                    LobbyTree_RotateRight(up, &root);
                    child = (Node *)root;
                }
            }
        }
        if (child) {
            setRed(child, 0);
        }
    }
    return (Node *)iter.current;
}
}
}

extern "C" {
struct Vec {
    void *ptr;
    u32 size, cap;
};

struct TreeNode {
    void *left, *right;
    u32 parent;
    u32 key;
    u32 data[6];
};

struct Alloc {
    u8 tag;
};

u32 LobbyStringVector_RecommendSize(struct Vec *, u32);
void LobbyStringVector_AppendCopiesSlow(struct Vec *, u32, void *);
void LobbyTree_RotateLeft(struct TreeNode *a, void **root);
void LobbyTree_RotateRight(struct TreeNode *a, void **root);
void LobbyStringVector_PushBackSlow(struct Vec *a, void *b, struct Alloc unused);

void LobbyTree_RotateLeft(struct TreeNode *a, void **root) {
    struct TreeNode *b = (struct TreeNode *)a->right, *c, *parent;
    if (*root == a) {
        *root = b;
    }
    a->right = b->left;
    c = (struct TreeNode *)b->left;
    if (c) {
        c->parent = (u32)a | (c->parent & 1);
    }
    b->parent = (a->parent & ~1) | (b->parent & 1);
    parent = (struct TreeNode *)(a->parent & ~1);
    if (a == parent->left) {
        parent->left = b;
    } else {
        parent->right = b;
    }
    b->left = a;
    a->parent = (u32)b | (a->parent & 1);
}

void LobbyTree_RotateRight(struct TreeNode *a, void **root) {
    struct TreeNode *b = (struct TreeNode *)a->left, *c, *parent;
    if (*root == a) {
        *root = b;
    }
    a->left = b->right;
    c = (struct TreeNode *)b->right;
    if (c) {
        c->parent = (u32)a | (c->parent & 1);
    }
    b->parent = (a->parent & ~1) | (b->parent & 1);
    parent = (struct TreeNode *)(a->parent & ~1);
    if (a == parent->left) {
        parent->left = b;
    } else {
        parent->right = b;
    }
    b->right = a;
    a->parent = (u32)b | (a->parent & 1);
}

void LobbyStringVector_PushBackSlow(struct Vec *a, void *b, struct Alloc unused) {
    struct Alloc alloc = { 0 };
    LobbyStringVector_RecommendSize(a, 1);
    LobbyStringVector_AppendCopiesSlow(a, 1, b);
}
}

namespace StringVectorDestroy {
extern "C" {
#ifdef HEARTGOLD_NATIVE
void LobbyStringVector_EraseAtEnd(LobbyStringVector *, u32, ByteAllocator);
#else
void LobbyStringVector_EraseAtEnd(LobbyStringVector *, u32, ByteAllocator, u32);
#endif
LobbyStringVector *LobbyStringVector_Destroy(LobbyStringVector *);

LobbyStringVector *LobbyStringVector_Destroy(LobbyStringVector *vector) {
    if (vector->data) {
        u32 current = vector->size;
        ByteAllocator allocator = { 0 };
#ifdef HEARTGOLD_NATIVE
        LobbyStringVector_EraseAtEnd(vector, current, allocator);
#else
        LobbyStringVector_EraseAtEnd(vector, current, allocator, 0);
#endif
        if (vector->data) {
            DWC_Free(0, vector->data, 0);
        }
    }
    return vector;
}
}
}

extern "C" {
LobbyU32Vector *LobbyU32Vector_Destroy(LobbyU32Vector *vector);

LobbyU32Vector *LobbyU32Vector_Destroy(LobbyU32Vector *vector) {
    if (vector->data) {
        u32 size = vector->size;
        ByteAllocator allocator = { 0 };
        vector->size -= size;
        if (vector->data) {
            DWC_Free(0, vector->data, 0);
        }
    }
    return vector;
}

PPWLobbyVipVector *LobbyVipVector_Destroy(PPWLobbyVipVector *vector) {
    if (vector->data) {
        u32 size = vector->size;
        ByteAllocator allocator = { 0 };
        vector->size -= size;
        if (vector->data) {
            DWC_Free(0, vector->data, 0);
        }
    }
    return vector;
}

LobbyEventVector *LobbyEventVector_Destroy(LobbyEventVector *vector) {
    if (vector->data) {
        u32 size = vector->size;
        ByteAllocator allocator = { 0 };
        vector->size -= size;
        if (vector->data) {
            DWC_Free(0, vector->data, 0);
        }
    }
    return vector;
}
}

// Fills n characters with c.
inline void FillN(char *p, u32 n, char c, TrueTag) {
    memset(p, c, n);
}

extern "C" void LobbyCharVector_Grow(LobbyCharVector *vector, u32 n) {
    if (n) {
        LobbyCharVector_Allocate(vector, n, ByteAllocator());
        FillN(vector->data + vector->size, n, char(), TrueTag());
        vector->size += n;
    }
}

extern "C" {
void LobbyCharVector_Allocate(LobbyCharVector *vector, u32 size, ByteAllocator) {
    vector->data = (char *)LobbyMem_Alloc(size);
    vector->capacity = size;
}
}

extern "C" {
void LobbyCharVector_Reallocate(LobbyCharVector *vector, ByteAllocator, u32 size);

void LobbyCharVector_Reallocate(LobbyCharVector *vector, ByteAllocator incoming, u32 size) {
    u32 count = vector->size;
    ByteAllocator old = { 0 };
    volatile ByteAllocator copy;
    (void)incoming;
    (void)old;
    vector->size -= count;
    if (vector->data) {
        if (vector->data) {
            DWC_Free(0, vector->data, 0);
        }
        vector->data = NULL;
        vector->capacity = 0;
    }
    ByteAllocator fresh = { 0 };
    copy.unused = 0;
    LobbyCharVector_Allocate(vector, size, fresh);
}
}

extern "C" {
extern void _ZNSt15__vector_commonILb1EE18throw_length_errorEv(void);

u32 LobbyCharVector_RecommendSize(LobbyCharVector *a, u32 n) {
    u32 cap = a->capacity;
    u32 growth;
    if (n > ~0u - cap) {
        _ZNSt15__vector_commonILb1EE18throw_length_errorEv();
    }
    if (cap < 0x55555555) {
        growth = (cap + 1) * 3 / 5;
        return cap + *(n > growth ? &n : &growth);
    }
    if (cap < 0xaaaaaaaa) {
        u32 g = (cap + 1) / 2;
        return cap + *(n > g ? &n : &g);
    }
    return ~0u;
}
}

extern "C" void LobbyCharVector_AppendZeros(LobbyCharVector *vector, u32 count, ResizeDispatch) {
    u32 capacity = vector->capacity;
    if (count > capacity || vector->size > capacity - count) {
        u32 additional = vector->size + count - capacity;
        Allocator allocator = Allocator();
        allocator.recommend(vector, additional);
    } else {
        memset(vector->data + vector->size, 0, count);
        vector->size += count;
        return;
    }
    LobbyCharVector_AppendZerosSlow(vector, count);
}

using LobbyStringTypes::RangeDispatch;

struct CharacterAllocator {
    void release(char *storage) {
        if (storage) {
            DWC_Free(0, storage, 0);
        }
    }
};

extern "C" LobbyCharVector *LobbyCharVector_Destroy(LobbyCharVector *vector) {
    if (vector->data) {
        u32 count = vector->size;
        CharacterAllocator allocator = CharacterAllocator();
        vector->size -= count;
        allocator.release(vector->data);
    }
    return vector;
}

extern "C" Str *LobbyString_InitCopy(Str *output, const Str *input) {
    if (!input->l.isLong) {
        *output = *input;
    } else {
        u32 index;
        RangeDispatch dispatch;
        for (index = 0; index < 3; index++) {
            output->words[index] = 0;
        }
        LobbyString_Reserve(output, input->l.size);
        LobbyString_ReplaceRange(output, 0, 0, input->l.ptr, input->l.ptr + input->l.size, dispatch);
    }
    return output;
}

extern "C" Str *LobbyString_InitSubstring(Str *self, Str *str, u32 pos, u32 n) {
    u32 i;
    for (i = 0; i < 3; i++) {
        self->words[i] = 0;
    }
    u32 size = str->size();
    LobbyString_Reserve(self, pos <= size ? Min(size - pos, n) : 0);
    LobbyString_ReplaceSubstring(self, 0, 0, str, pos, n);
    return self;
}

extern "C" Str *LobbyString_InitFromCStr(Str *output, const char *text) {
    u32 index, length;
    RangeDispatch dispatch;
    for (index = 0; index < 3; index++) {
        output->words[index] = 0;
    }
    length = strlen(text);
    LobbyString_Reserve(output, length);
    LobbyString_ReplaceRange(output, 0, 0, text, text + length, dispatch);
    return output;
}

extern "C" Str *LobbyString_Destroy(Str *output) {
    if (output->l.isLong && output->l.ptr) {
        DWC_Free(0, output->l.ptr, 0);
    }
    return output;
}

extern "C" {
extern char gLobbyStr_BasicStringReserveLengthError[];

void LobbyString_Reserve(Str *w, u32 size) {
    u32 len, capacity;
    char *dst, *old;
    int wasHeap, makeHeap;
    if (size > 0x7ffffffe) {
        printf(gLobbyStr_BasicStringReserveLengthError);
        abort();
    }
    wasHeap = w->l.isLong != 0;
    if (wasHeap) {
        capacity = w->l.capacity;
        len = w->l.size;
    } else {
        capacity = 11;
        len = w->s.len;
    }
    if (size < len) {
        size = len;
    }
    if (size + 1 <= 11) {
        size = 11;
    } else {
        size = (size + 16) & ~15;
    }
    if (size == capacity) {
        return;
    }
    if (size == 11) {
        dst = w->s.buf;
        old = w->l.ptr;
        makeHeap = 0;
    } else {
        if (size > capacity) {
            dst = (char *)LobbyMem_Alloc(size);
        } else {
            dst = (char *)LobbyMem_Alloc(size);
            if (!dst) {
                return;
            }
        }
        old = wasHeap ? w->l.ptr : w->s.buf;
        makeHeap = 1;
    }
    memmove(dst, old, len);
    dst[len] = 0;
    if (wasHeap && old) {
        DWC_Free(0, old, 0);
    }
    w->l.isLong = makeHeap;
    if (!makeHeap) {
        w->s.len = len;
    } else {
        w->l.size = len;
        w->l.ptr = dst;
        w->l.capacity = size;
    }
}
}

extern "C" {
Str *LobbyString_Assign(Str *dst, const Str *src) {
    BOOL isLong = dst->l.isLong ? TRUE : FALSE;
    if (!isLong && !src->l.isLong) {
        *dst = *src;
        return dst;
    }
    return LobbyString_Replace(dst, 0, isLong ? dst->l.size : dst->s.len, src);
}
}

using namespace LobbyStringTypes;

extern "C" Str *LobbyString_Replace(Str *out, u32 pos, u32 remove, const Str *input) {
    const char *data;
    u32 size;
    RangeDispatch dispatch;
    if (input->l.isLong) {
        data = input->l.ptr;
        size = input->l.size;
    } else {
        data = input->s.buf;
        size = input->s.len;
    }
    return LobbyString_ReplaceRange(out, pos, remove, data, data + size, dispatch);
}

extern "C" char gLobbyStr_BasicStringOutOfRange[];

extern "C" Str *LobbyString_ReplaceSubstring(Str *self, u32 pos, u32 n1, Str *str, u32 pos2, u32 n2) {
    char *data;
    u32 size;
    RangeDispatch dispatch;
    if (str->l.isLong) {
        data = str->l.ptr;
        size = str->l.size;
    } else {
        data = str->s.buf;
        size = str->s.len;
    }
    if (pos2 > size) {
        printf(gLobbyStr_BasicStringOutOfRange);
        abort();
    }
    data += pos2;
    n2 = Min(size - pos2, n2);
    return LobbyString_ReplaceRange(self, pos, n1, data, data + n2, dispatch);
}

inline StringSearchRange StringSearch::GetRange() const {
    if (raw.longForm.flag) {
        return StringSearchRange(raw.longForm.data, raw.longForm.size);
    }
    return StringSearchRange(raw.shortForm.data, raw.shortForm.size);
}

inline int StringSearch::Find(const StringSearch &other, u32 pos) const {
    StringSearchRange needle = other.GetRange();
    return (int)LobbyString_FindChars(this, needle.data, pos, needle.size);
}

extern "C" int LobbyString_Find(const StringSearch *a, const StringSearch *b, u32 pos) {
    return a->Find(*b, pos);
}

extern "C" {
u32 LobbyString_FindChars(const StringSearch *s, const s8 *needle, u32 start, u32 count) {
    u32 len, left;
    const s8 *data, *end, *at, *p, *q;
    if (s->raw.longForm.flag) {
        data = s->raw.longForm.data;
        len = s->raw.longForm.size;
    } else {
        data = s->raw.shortForm.data;
        len = s->raw.shortForm.size;
    }
    if (start > len) {
        goto fail;
    }
    left = len - start;
    end = needle + count;
    at = data + start;
    if (left < count) {
        goto fail;
    }
    do {
        p = at;
        q = needle;
        while (q < end) {
            if (*q != *p) {
                goto next;
            }
            q++;
            p++;
        }
        return at - data;
next:
        left--;
        at++;
    } while (left >= count);
fail:
    return ~0U;
}
}

static inline int Compare(const Str *self, const Str &other) {
    const char *d;
    u32 n;
    if (!other.l.isLong) {
        d = other.s.buf;
        n = other.s.len;
    } else {
        d = other.l.ptr;
        n = other.l.size;
    }
    return LobbyString_CompareChars(self, 0, self->size(), d, n);
}

static inline int Compare(const Str *self, const char *text) {
    u32 n = strlen(text);
    return LobbyString_CompareChars(self, 0, self->size(), text, n);
}

extern "C" int LobbyString_Compare(const Str *a, const Str *b) {
    return Compare(a, *b);
}

extern "C" int LobbyString_CompareCStr(const Str *a, const char *b) {
    return Compare(a, b);
}

extern "C" {
extern const char gLobbyStr_StringComparePosGtLen[];

static inline const u32 *MinPtr(const u32 *a, const u32 *b) {
    return *a < *b ? a : b;
}

int LobbyString_CompareChars(const Str *s, u32 pos, u32 length, const void *other, u32 otherLength) {
    const char *data;
    u32 size, n, remaining;
    int r;
    if (s->l.isLong) {
        data = s->l.ptr;
        size = s->l.size;
    } else {
        data = s->s.buf;
        size = s->s.len;
    }
    if (pos > size) {
        printf(gLobbyStr_StringComparePosGtLen);
        abort();
    }
    remaining = size - pos;
    n = *MinPtr(&length, &remaining);
    r = memcmp(data + pos, other, *MinPtr(&otherLength, &n));
    if (r) {
        return r;
    }
    if (n < otherLength) {
        return -1;
    }
    return n != otherLength;
}
}

namespace StringReplaceRange {
extern "C" const char gLobbyStr_BasicStringOutOfRange[];
extern "C" const char gLobbyStr_BasicStringLengthError[];

class Tag {};
#ifdef HEARTGOLD_NATIVE
extern "C" Str *LobbyString_AssignRange(Str *, const char *, const char *);
#else
extern "C" Str *LobbyString_AssignRange(Str *, const char *, const char *, int);
#endif

inline const u32 &Min(const u32 &a, const u32 &b) {
    return b < a ? b : a;
}

extern "C" Str *LobbyString_ReplaceRange(Str *s, u32 pos, u32 n1, const char *first, const char *last, Tag);

#pragma opt_common_subs off

extern "C" Str *LobbyString_ReplaceRange(Str *s, u32 pos, u32 n1, const char *first, const char *last, Tag) {
    bool isLong = s->l.isLong;
    u32 cap;
    char *data;
    u32 size;
    if (!isLong) {
        cap = 11;
        data = s->s.buf;
        size = s->s.len;
    } else {
        cap = s->l.capacity;
        size = s->l.size;
        data = s->l.ptr;
    }
    if (pos > size) {
        printf(gLobbyStr_BasicStringOutOfRange);
        abort();
    }
    u32 rem = size - pos;
    u32 n = Min(n1, rem);
    u32 n2 = last - first;
    u32 len;
    if (n2 > 0x7ffffffe || (len = size - n) > 0x7ffffffe - n2) {
        printf(gLobbyStr_BasicStringLengthError);
        abort();
    }
    u32 newSize = n2 + len;
    u32 tail = size - (pos + n);
    if (newSize >= cap) {
        u32 newCap = (cap + 15) & ~15;
        while (newCap < newSize + 1) {
            newCap = (newCap * 2 + 15) & ~15;
        }
        char *p = (char *)LobbyMem_Alloc(newCap);
        if (pos) {
            memcpy(p, data, pos);
        }
        char *q = p + pos;
        memmove(q, first, last - first);
        if (tail) {
            memcpy(q + n2, data + pos + n, tail);
        }
        p[newSize] = gLobbyStringTerminator;
        if (isLong) {
            if (data) {
                DWC_Free(0, data, 0);
            }
        } else {
            s->l.isLong = 1;
        }
        s->l.ptr = p;
        s->l.size = newSize;
        s->l.capacity = newCap;
    } else {
        PPWString tmp;
        if (tail && data + pos + n2 < last && last <= data + size) {
#ifdef HEARTGOLD_NATIVE
            LobbyString_AssignRange(&tmp.str, first, last);
#else
            LobbyString_AssignRange(&tmp.str, first, last, 0);
#endif
            const Str *text = &tmp.str;
            first = text->c_str();
        }
        if (tail) {
            memmove(data + pos + n2, data + pos + n, tail);
        }
        memmove(data + pos, first, n2);
        data[newSize] = gLobbyStringInPlaceTerminator;
        if (isLong) {
            s->l.size = newSize;
        } else {
            s->s.len = newSize;
        }
        tmp.Destroy();
    }
    return s;
}

#pragma opt_common_subs reset
}
