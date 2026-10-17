// SPDX-License-Identifier: GPL-3.0-or-later
#include "Host.h"
#include "Platform.h"
#include <chrono>
#include <condition_variable>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <mutex>
#include <thread>
#include <unistd.h>

static std::string storageRoot;
void HGSetStorageRoot(const std::string& path) { storageRoot = path; }

bool HGIsSupportedROM(const uint8_t* header, size_t size) {
    if (!header || size < 0x4000 || size > 512 * 1024 * 1024 || header[0x14] > 12 ||
        (std::memcmp(header + 12, "IPKE", 4) && std::memcmp(header + 12, "IPGE", 4) &&
         std::memcmp(header + 12, "IPKJ", 4) && std::memcmp(header + 12, "IPGJ", 4))) return false;
    auto word = [&](size_t at) {
        return uint32_t(header[at]) | uint32_t(header[at + 1]) << 8 |
               uint32_t(header[at + 2]) << 16 | uint32_t(header[at + 3]) << 24;
    };
    for (size_t section : {0x20, 0x30}) {
        const auto offset = word(section), length = word(section + 12);
        if (offset < 0x4000 || length < 4 || offset > size || length > size - offset) return false;
    }
    for (size_t section : {0x40, 0x48, 0x50, 0x58}) {
        const auto offset = word(section), length = word(section + 4);
        if (offset > size || length > size - offset) return false;
    }
    return true;
}

bool HGHasPlainSecureArea(const uint8_t* rom, size_t size) {
    if (!HGIsSupportedROM(rom, size)) return false;
    const uint32_t offset = uint32_t(rom[0x20]) | uint32_t(rom[0x21]) << 8 |
                            uint32_t(rom[0x22]) << 16 | uint32_t(rom[0x23]) << 24;
    if (offset >= 0x8000) return true;
    if (size - offset < 0x800) return false;
    const uint8_t marker[] = {0xff, 0xde, 0xff, 0xe7};
    // melonDS can re-encrypt plaintext with its FreeBIOS key, then direct-boot it.
    return !std::memcmp(rom + offset, marker, 4) && !std::memcmp(rom + offset + 4, marker, 4) &&
            std::memcmp(rom + offset + 0x10, marker, 4);
}

bool HGFlushSave(HGSaveContext& save) {
    if (!save.dirty) return true;
    const auto temp = save.path + ".tmp";
    FILE* file = std::fopen(temp.c_str(), "wb");
    bool ok = false;
    if (file) {
        ok = std::fwrite(save.bytes.data(), 1, save.bytes.size(), file) == save.bytes.size();
        if (std::fflush(file) || fsync(fileno(file))) ok = false;
        if (std::fclose(file)) ok = false;
        if (ok) ok = !std::rename(temp.c_str(), save.path.c_str());
    }
    if (ok) {
        save.dirty = false;
        save.error.clear();
    } else {
        save.error = "Could not save progress. The previous save was preserved; new progress remains in memory.";
    }
    return ok;
}

namespace melonDS::Platform {
struct FileHandle { FILE* stream; };
struct Thread { std::thread thread; };
struct Mutex { std::mutex mutex; };
struct Semaphore { std::mutex mutex; std::condition_variable ready; unsigned count = 0; };

std::string GetLocalFilePath(const std::string& name) { return storageRoot + "/" + name; }
FileHandle* OpenFile(const std::string& path, FileMode mode) {
    const bool write = mode & Write, read = mode & Read;
    if (!read && !write) return nullptr;
    int flags = write ? (read ? O_RDWR : O_WRONLY) : O_RDONLY;
    if (write) {
        if (!(mode & NoCreate)) flags |= O_CREAT;
        if (mode & Append) flags |= O_APPEND;
        else if (!(mode & Preserve)) flags |= O_TRUNC;
    }
    int fd = open(path.c_str(), flags, 0600);
    if (fd < 0) return nullptr;
    const char* access = !write ? "rb" : (mode & Append) ? (read ? "a+b" : "ab") : (read ? "r+b" : "wb");
    FILE* stream = fdopen(fd, access);
    if (!stream) { close(fd); return nullptr; }
    return new FileHandle{stream};
}
FileHandle* OpenLocalFile(const std::string& path, FileMode mode) { return OpenFile(GetLocalFilePath(path), mode); }
bool FileExists(const std::string& name) { return access(name.c_str(), F_OK) == 0; }
bool LocalFileExists(const std::string& name) { return FileExists(GetLocalFilePath(name)); }
bool CheckFileWritable(const std::string& name) {
    auto file = OpenFile(name, FileMode(Write | Preserve));
    return file ? CloseFile(file) : false;
}
bool CheckLocalFileWritable(const std::string& name) { return CheckFileWritable(GetLocalFilePath(name)); }
bool CloseFile(FileHandle* file) { bool ok = !std::fclose(file->stream); delete file; return ok; }
bool IsEndOfFile(FileHandle* file) { return std::feof(file->stream); }
bool FileReadLine(char* out, int count, FileHandle* file) { return std::fgets(out, count, file->stream); }
u64 FilePosition(FileHandle* file) { auto p = ftello(file->stream); return p < 0 ? 0 : p; }
bool FileSeek(FileHandle* file, s64 offset, FileSeekOrigin origin) {
    int whence = origin == FileSeekOrigin::Start ? SEEK_SET : origin == FileSeekOrigin::Current ? SEEK_CUR : SEEK_END;
    return !fseeko(file->stream, offset, whence);
}
void FileRewind(FileHandle* file) { std::rewind(file->stream); }
u64 FileRead(void* data, u64 size, u64 count, FileHandle* file) { return std::fread(data, size, count, file->stream); }
bool FileFlush(FileHandle* file) { return !std::fflush(file->stream); }
u64 FileWrite(const void* data, u64 size, u64 count, FileHandle* file) { return std::fwrite(data, size, count, file->stream); }
u64 FileWriteFormatted(FileHandle* file, const char* fmt, ...) {
    va_list args; va_start(args, fmt); int n = std::vfprintf(file->stream, fmt, args); va_end(args);
    return n < 0 ? 0 : n;
}
u64 FileLength(FileHandle* file) {
    auto position = ftello(file->stream);
    if (position < 0 || fseeko(file->stream, 0, SEEK_END)) return 0;
    auto length = ftello(file->stream);
    if (fseeko(file->stream, position, SEEK_SET) || length < 0) return 0;
    return length;
}
void Log(LogLevel level, const char* fmt, ...) {
    if (level < Warn) return;
    va_list args; va_start(args, fmt); std::vfprintf(stderr, fmt, args); va_end(args);
}
void SignalStop(StopReason, void* data) { if (data) static_cast<HGSaveContext*>(data)->stopped = true; }
Thread* Thread_Create(std::function<void()> fn) { return new Thread{std::thread(std::move(fn))}; }
void Thread_Wait(Thread* thread) { if (thread->thread.joinable()) thread->thread.join(); }
void Thread_Free(Thread* thread) { Thread_Wait(thread); delete thread; }
Semaphore* Semaphore_Create() { return new Semaphore; }
void Semaphore_Free(Semaphore* sema) { delete sema; }
void Semaphore_Reset(Semaphore* sema) { std::lock_guard<std::mutex> lock(sema->mutex); sema->count = 0; }
void Semaphore_Wait(Semaphore* sema) {
    std::unique_lock<std::mutex> lock(sema->mutex);
    sema->ready.wait(lock, [&] { return sema->count > 0; }); --sema->count;
}
bool Semaphore_TryWait(Semaphore* sema, int timeout) {
    std::unique_lock<std::mutex> lock(sema->mutex);
    if (!sema->ready.wait_for(lock, std::chrono::milliseconds(timeout), [&] { return sema->count > 0; })) return false;
    --sema->count; return true;
}
void Semaphore_Post(Semaphore* sema, int count) {
    { std::lock_guard<std::mutex> lock(sema->mutex); sema->count += count; }
    sema->ready.notify_all();
}
Mutex* Mutex_Create() { return new Mutex; }
void Mutex_Free(Mutex* mutex) { delete mutex; }
void Mutex_Lock(Mutex* mutex) { mutex->mutex.lock(); }
void Mutex_Unlock(Mutex* mutex) { mutex->mutex.unlock(); }
bool Mutex_TryLock(Mutex* mutex) { return mutex->mutex.try_lock(); }
void Sleep(u64 usecs) { std::this_thread::sleep_for(std::chrono::microseconds(usecs)); }
u64 GetUSCount() { return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count(); }
u64 GetMSCount() { return GetUSCount() / 1000; }
void WriteNDSSave(const u8* bytes, u32 size, u32 offset, u32 count, void* data) {
    if (!data || !bytes || offset > size || count > size - offset) return;
    auto& save = *static_cast<HGSaveContext*>(data);
    if (save.bytes.size() != size) save.bytes.assign(bytes, bytes + size);
    else std::copy(bytes + offset, bytes + offset + count, save.bytes.begin() + offset);
    save.dirty = true;
}
// The first milestone supports offline NDS games; DSi, networking and accessories are excluded.
void WriteGBASave(const u8*, u32, u32, u32, void*) {}
void WriteFirmware(const Firmware&, u32, u32, void*) {}
void WriteDateTime(int, int, int, int, int, int, void*) {}
void MP_Begin(void*) {}
void MP_End(void*) {}
int MP_SendPacket(u8*, int, u64, void*) { return 0; }
int MP_RecvPacket(u8*, u64*, void*) { return 0; }
int MP_SendCmd(u8*, int, u64, void*) { return 0; }
int MP_SendReply(u8*, int, u64, u16, void*) { return 0; }
int MP_SendAck(u8*, int, u64, void*) { return 0; }
int MP_RecvHostPacket(u8*, u64*, void*) { return 0; }
u16 MP_RecvReplies(u8*, u64, u16, void*) { return 0; }
int Net_SendPacket(u8*, int, void*) { return 0; }
int Net_RecvPacket(u8*, void*) { return 0; }
void Camera_Start(int, void*) {}
void Camera_Stop(int, void*) {}
void Camera_CaptureFrame(int, u32* frame, int width, int height, bool, void*) { std::fill(frame, frame + width * height, 0); }
void Mic_Start(void*) {}
void Mic_Stop(void*) {}
int Mic_ReadInput(s16* data, int count, void*) { std::fill(data, data + count, 0); return count; }
AACDecoder* AAC_Init() { return nullptr; }
void AAC_DeInit(AACDecoder*) {}
bool AAC_Configure(AACDecoder*, int, int) { return false; }
bool AAC_DecodeFrame(AACDecoder*, const void*, int, void*, int) { return false; }
bool Addon_KeyDown(KeyType, void*) { return false; }
void Addon_RumbleStart(u32, void*) {}
void Addon_RumbleStop(void*) {}
float Addon_MotionQuery(MotionQueryType, void*) { return 0; }
DynamicLibrary* DynamicLibrary_Load(const char*) { return nullptr; }
void DynamicLibrary_Unload(DynamicLibrary*) {}
void* DynamicLibrary_LoadFunction(DynamicLibrary*, const char*) { return nullptr; }
}
