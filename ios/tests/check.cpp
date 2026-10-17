// SPDX-License-Identifier: GPL-3.0-or-later
#include "Host.h"
#include "Args.h"
#include "NDS.h"
#include "NDSCart.h"
#include "NDSCart/CartCommon.h"
#include "Platform.h"
#include <cassert>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <unistd.h>

int main() {
    std::array<uint8_t, 0x8000> header{};
    assert(!HGIsSupportedROM(header.data(), header.size()));
    std::memcpy(header.data() + 12, "IPKE", 4);
    header[0x21] = 0x40; header[0x2c] = 4;
    header[0x31] = 0x50; header[0x3c] = 4;
    assert(HGIsSupportedROM(header.data(), header.size()));
    assert(!HGIsSupportedROM(header.data(), 16));
    assert(!HGIsSupportedROM(nullptr, header.size()));
    assert(!HGIsSupportedROM(header.data(), 512ull * 1024 * 1024 + 1));
    std::memcpy(header.data() + 12, "IPGE", 4);
    assert(HGIsSupportedROM(header.data(), header.size()));
    header[0x2f] = 0x80; assert(!HGIsSupportedROM(header.data(), header.size())); header[0x2f] = 0;
    header[0x47] = 0x80; assert(!HGIsSupportedROM(header.data(), header.size())); header[0x47] = 0;

    auto rect = HGAspectFit(256, 192, {40, 12, 600, 360});
    assert(rect.x == 100 && rect.y == 12 && rect.width == 480 && rect.height == 360);
    uint16_t tx = 999, ty = 999;
    assert(HGTouchPoint(rect.x, rect.y, rect, tx, ty) && tx == 0 && ty == 0);
    assert(HGTouchPoint(rect.x + rect.width - .001, rect.y + rect.height - .001, rect, tx, ty));
    assert(tx == 255 && ty == 191);
    assert(!HGTouchPoint(rect.x - 1, rect.y, rect, tx, ty));
    assert(!HGTouchPoint(rect.x + rect.width, rect.y, rect, tx, ty));
    assert(!HGTouchPoint(0, 0, HGAspectFit(0, 192, {0, 0, 100, 100}), tx, ty));

    double deadline = 0; int frames = 0;
    for (int tick = 0; tick < 6000; ++tick) frames += HGFrameDue(tick / 60.0, deadline);
    assert(frames >= 5981 && frames <= 5984);
    assert(HGFrameDue(10000, deadline) && !HGFrameDue(10000, deadline));

    HGAudioRing audio;
    std::vector<int16_t> input(HGAudioRing::capacity + 20, 1234), output(input.size(), -1);
    audio.push(input.data(), input.size()); audio.pop(output.data(), output.size());
    assert(output[0] == 1234 && output[HGAudioRing::capacity - 1] == 1234 && output.back() == 0);
    for (int i = 0; i < 100; ++i) {
        input[0] = i; audio.push(input.data(), 1); audio.pop(output.data(), 2);
        assert(output[0] == i && output[1] == 0);
    }

    char folder[] = "/tmp/heartgold-check-XXXXXX";
    assert(mkdtemp(folder)); HGSetStorageRoot(folder);
    using namespace melonDS::Platform;
    assert(!OpenLocalFile("absent", ReadWriteExisting));
    HGSaveContext save; save.path = std::string(folder) + "/game.sav";
    uint8_t bytes[] = {1, 2, 3, 4};
    WriteNDSSave(bytes, 4, 0, 4, &save); assert(save.dirty && HGFlushSave(save));
    auto file = OpenLocalFile("game.sav", ReadWriteExisting); assert(file);
    assert(FileLength(file) == 4 && FilePosition(file) == 0);
    uint8_t readback[4]{}; assert(FileRead(readback, 1, 4, file) == 4);
    assert(!std::memcmp(bytes, readback, 4)); CloseFile(file);
    bytes[2] = 9; WriteNDSSave(bytes, 4, 2, 1, &save);
    assert(save.bytes[0] == 1 && save.bytes[2] == 9);
    WriteNDSSave(bytes, 4, 3, 2, &save); assert(save.bytes[3] == 4);
    assert(HGFlushSave(save));
    save.path = std::string(folder) + "/missing/game.sav";
    save.dirty = true; assert(!HGFlushSave(save) && save.dirty && !save.error.empty());
    assert(std::filesystem::file_size(std::string(folder) + "/game.sav") == 4);
    auto sema = Semaphore_Create(); assert(!Semaphore_TryWait(sema, 0));
    auto thread = Thread_Create([&] { Semaphore_Post(sema); });
    Semaphore_Wait(sema); Thread_Free(thread); Semaphore_Free(sema);

    // Exercise the actual interpreter, software renderer and built-in FreeBIOS, without a game ROM.
    melonDS::NDSArgs args; args.JIT = std::nullopt;
    // Poisoning memory catches constructors that read GPU state before initializing it.
    void* memory = ::operator new(sizeof(melonDS::NDS));
    std::memset(memory, 0xcc, sizeof(melonDS::NDS));
    auto console = std::unique_ptr<melonDS::NDS>(new (memory) melonDS::NDS(std::move(args))); console->Reset();
    console->ARM9Write32(0x02000000, 0x1234abcd);
    assert(console->ARM9Read32(0x02000000) == 0x1234abcd);
    // A tiny original homebrew program writes a marker through the real ARM9 interpreter.
    auto rom = std::make_unique<uint8_t[]>(0x8000);
    std::fill(rom.get(), rom.get() + 0x8000, 0);
    std::memcpy(rom.get() + 12, "####", 4);
    auto put = [&](size_t offset, uint32_t value) { std::memcpy(rom.get() + offset, &value, 4); };
    put(0x20, 0x200); put(0x24, 0x02000000); put(0x28, 0x02000000); put(0x2c, 24);
    put(0x30, 0x300); put(0x34, 0x03800000); put(0x38, 0x03800000); put(0x3c, 4);
    put(0x200, 0xe59f0008); put(0x204, 0xe59f1008); put(0x208, 0xe5801000);
    put(0x20c, 0xeafffffe); put(0x210, 0x02000100); put(0x214, 0xc0dec0de);
    put(0x300, 0xeafffffe);
    auto cart = melonDS::NDSCart::ParseROM(std::move(rom), 0x8000); assert(cart);
    console->SetNDSCart(std::move(cart)); console->Reset(); console->SetupDirectBoot("self-check.nds");
    console->Start(); console->RunFrame();
    assert(console->ARM9Read32(0x02000100) == 0xc0dec0de);
    void *top = nullptr, *bottom = nullptr;
    assert(console->GPU.GetFramebuffers(&top, &bottom) && top && bottom);
    assert(console->NumFrames > 0); console->Stop();
    std::filesystem::remove_all(folder);
    std::cout << "PASS: ROM boundaries, landscape coordinates, audio, atomic saves, file/thread adapter, FreeBIOS + ARM9 execution + software frame\n";
}
