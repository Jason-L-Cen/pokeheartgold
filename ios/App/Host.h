// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include <algorithm>
#include <array>
#include <atomic>
#include <cstdint>
#include <string>
#include <vector>

struct HGSaveContext {
    std::string path;
    std::vector<uint8_t> bytes;
    std::string error;
    bool dirty = false;
    bool stopped = false;
};

void HGSetStorageRoot(const std::string& path);
bool HGFlushSave(HGSaveContext& save);
bool HGIsSupportedROM(const uint8_t* header, size_t size);
bool HGHasPlainSecureArea(const uint8_t* rom, size_t size);

struct HGButtons {
    std::atomic<uint32_t> held{0}, pressed{0};
    void down(uint32_t bit) { held.fetch_or(bit); pressed.fetch_or(bit); }
    void release(uint32_t bit) { held.fetch_and(~bit); }
    void finish(uint32_t bit) {
        // Accessibility activation can send touch-up without a preceding touch-down.
        if (!(held.fetch_and(~bit) & bit)) pressed.fetch_or(bit);
    }
    uint32_t poll() { const auto pulse = pressed.exchange(0); return held.load() | pulse; }
    void clear() { held.store(0); pressed.store(0); }
};

inline uint32_t HGPollTouch(std::atomic<uint32_t>& held, std::atomic<uint32_t>& pending) {
    const auto tap = pending.exchange(0), current = held.load();
    return current ? current : tap;
}

inline bool HGFrameDue(double now, double& deadline) {
    // Same cadence as melonDS's frontend; treating DS frames as 60 Hz makes its clock drift.
    constexpr double interval = 1.0 / 59.8260982880808;
    if (now - deadline > 2 * interval) deadline = now;
    if (now < deadline) return false;
    deadline += interval;
    return true;
}

struct HGRect { double x, y, width, height; };
inline HGRect HGAspectFit(double width, double height, HGRect bounds) {
    if (width <= 0 || height <= 0 || bounds.width <= 0 || bounds.height <= 0) return {0, 0, 0, 0};
    const double scale = std::min(bounds.width / width, bounds.height / height);
    return {bounds.x + (bounds.width - width * scale) / 2,
            bounds.y + (bounds.height - height * scale) / 2, width * scale, height * scale};
}
inline bool HGTouchPoint(double x, double y, HGRect rect, uint16_t& tx, uint16_t& ty) {
    if (rect.width <= 0 || rect.height <= 0 || x < rect.x || y < rect.y ||
        x >= rect.x + rect.width || y >= rect.y + rect.height) return false;
    tx = std::min(255, int((x - rect.x) * 256 / rect.width));
    ty = std::min(191, int((y - rect.y) * 192 / rect.height));
    return true;
}

// One producer (core queue), one consumer (audio callback); overflow drops new audio.
struct HGAudioRing {
    static constexpr uint32_t capacity = 32768;
    std::array<int16_t, capacity> samples{};
    std::atomic<uint32_t> read{0}, written{0};
    void push(const int16_t* input, uint32_t count) {
        auto w = written.load(std::memory_order_relaxed);
        const auto r = read.load(std::memory_order_acquire);
        count = std::min(count, capacity - (w - r));
        for (uint32_t i = 0; i < count; ++i) samples[(w + i) % capacity] = input[i];
        written.store(w + count, std::memory_order_release);
    }
    void pop(int16_t* output, uint32_t count) {
        const auto r = read.load(std::memory_order_relaxed);
        const auto w = written.load(std::memory_order_acquire);
        const auto available = std::min(count, w - r);
        for (uint32_t i = 0; i < available; ++i) output[i] = samples[(r + i) % capacity];
        std::fill(output + available, output + count, 0);
        read.store(r + available, std::memory_order_release);
    }
};
