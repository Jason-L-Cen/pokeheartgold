#!/usr/bin/env python3
"""Check the Trainer Card signature packer, prompt template and VBlank callbacks."""

import re
import shlex
import subprocess
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]


def extract(text, pattern):
    matches = re.findall(pattern, text, re.MULTILINE | re.DOTALL)
    assert len(matches) == 1, (pattern, len(matches))
    return matches[0]


def function(text, name):
    return extract(text, r"^static void " + name + r"\([^;]*?\) \{.*?^\}")


signature = (ROOT / "src/application/trainer_card/overlay_trainer_card_signature.c").read_text()
main = (ROOT / "src/application/trainer_card/overlay_trainer_card_main.c").read_text()
prompt_header = (ROOT / "include/yes_no_prompt.h").read_text()
save_header = (ROOT / "include/save_trainer_card.h").read_text()
system_header = (ROOT / "include/system.h").read_text()

assert "Main_SetVBlankIntrCB(TrainerCardSignature_VBlankCB, signature->bgConfig);" in signature
assert "(GFIntrCB)TrainerCardSignature_VBlankCB" not in signature
assert "BgConfig *bgConfig = data;" in signature
assert "Main_SetVBlankIntrCB(TrainerCardMain_VBlankCB, cardMain);" in main
assert "(GFIntrCB)TrainerCardMain_VBlankCB" not in main
assert "TrainerCardMain *cardMain = data;" in main

parts = [r"""
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef uint8_t u8;
typedef uint32_t u32;
typedef struct { int unused; } BgConfig;
typedef struct { int unused; } YesNoPrompt;
#define GF_ASSERT assert
#define GF_BG_LYR_MAIN_0 0
"""]

parts += [extract(save_header, r"^struct SaveTrainerCard \{.*?^\};"),
          function(signature, "TrainerCardSignature_PackCanvas"),
          function(main, "TrainerCardMain_UnpackSignature"), r"""
static void TrainerCardSignature_PackCanvasReference(u8 *signature, const u8 *pixels) {
    for (int i = 0; i < 6144; i++) {
        int bit = (i % 4) * 2;
        if ((pixels[i] & 15) == 1) {
            signature[i / 4] |= 1 << bit;
        }
        if ((pixels[i] >> 4) == 1) {
            signature[i / 4] |= 1 << (bit + 1);
        }
    }
}

_Static_assert(sizeof(struct SaveTrainerCard) == 0x620, "save size");
_Static_assert(offsetof(struct SaveTrainerCard, signaturePointData) == 0x20, "signature offset");
_Static_assert(sizeof(((struct SaveTrainerCard *)0)->signaturePointData) == 0x600, "signature size");

static void check_pack(void) {
    enum { INPUT_BYTES = 6144, SIGNATURE_BYTES = 1536, PIXELS = 12288 };
    struct {
        u8 before[16];
        struct SaveTrainerCard card;
        u8 after[16];
    } guarded;
    u8 *pixels = malloc(INPUT_BYTES);
    u8 *packed = malloc(SIGNATURE_BYTES);
    u8 *unpacked = malloc(PIXELS);
    u8 reference[SIGNATURE_BYTES];
    const u8 patterns[] = { 0x00, 0x11, 0x22, 0xff, 0x01, 0x10 };
    uint32_t random = 1;
    assert(pixels && packed && unpacked);

    for (unsigned test = 0; test < 38; test++) {
        for (unsigned i = 0; i < INPUT_BYTES; i++) {
            random = random * 1664525u + 1013904223u;
            pixels[i] = test < sizeof(patterns) ? patterns[test] : (u8)(random >> 24);
        }
        if (test >= 36) {
            memset(pixels, 0x22, INPUT_BYTES);
            pixels[INPUT_BYTES - 1] = test == 36 ? 0x21 : 0x12;
        }

        memset(packed, 0, SIGNATURE_BYTES);
        memset(reference, 0, sizeof(reference));
        TrainerCardSignature_PackCanvas(packed, pixels);
        TrainerCardSignature_PackCanvasReference(reference, pixels);
        assert(memcmp(packed, reference, SIGNATURE_BYTES) == 0);
        memset(&guarded, 0xa5, sizeof(guarded));
        TrainerCardSignature_PackCanvas((u8 *)guarded.card.signaturePointData, pixels);
        assert(memcmp(packed, guarded.card.signaturePointData, SIGNATURE_BYTES) == 0);
        for (unsigned i = 0; i < sizeof(guarded.before); i++) {
            assert(guarded.before[i] == 0xa5 && guarded.after[i] == 0xa5);
        }
        for (unsigned i = 0; i < sizeof(guarded.card.badgeShininess); i++) {
            assert(((u8 *)guarded.card.badgeShininess)[i] == 0xa5);
        }
        TrainerCardMain_UnpackSignature(packed, unpacked);
        for (unsigned i = 0; i < INPUT_BYTES; i++) {
            assert(unpacked[2 * i] == ((pixels[i] & 15) == 1));
            assert(unpacked[2 * i + 1] == ((pixels[i] >> 4) == 1));
        }
        if (test >= 36) {
            assert(packed[SIGNATURE_BYTES - 1] == (test == 36 ? 0x40 : 0x80));
        }
    }
    free(unpacked);
    free(packed);
    free(pixels);
    puts("PASS packing: exact 1536-byte save region, prefix/guards, 38 patterns, actual unpack roundtrip");
}
"""]

parts += [extract(prompt_header, r"^typedef struct YesNoPromptTemplate \{.*?^\} YesNoPromptTemplate;"), r"""
static YesNoPromptTemplate captured;
static void YesNoPrompt_InitFromTemplate(YesNoPrompt *prompt, const YesNoPromptTemplate *template) {
    assert(prompt);
    captured = *template;
}
""", function(signature, "TrainerCardSignature_ShowYesNoPrompt"), r"""

static void check_prompt(void) {
    BgConfig bg;
    YesNoPrompt prompt;
    TrainerCardSignature_ShowYesNoPrompt(&bg, &prompt);
    assert(captured.bgConfig == &bg);
    assert(captured.bgId == GF_BG_LYR_MAIN_0 && captured.tileStart == 0xb4 && captured.plttSlot == 8);
    assert(captured.x == 0x19 && captured.y == 6);
    assert(captured.ignoreTouchFlag == 0 && captured.initialCursorPos == 0 && captured.shapeParam == 0);
    puts("PASS prompt: all YesNoPromptTemplate fields are initialized");
}
"""]

parts += [extract(system_header, r"^typedef void \(\*GFIntrCB\)\(void \*\);"), r"""
#define GF_BG_LYR_SUB_1 5
#define GF_BG_LYR_MAIN_3 3
#define BG_POS_OP_SET_X 0
#define BG_POS_OP_SET_Y 3
#define OS_IE_VBLANK 1
typedef struct { BgConfig *bgConfig; int scrollOffset; int scaleChanged; } TrainerCardMain;
static BgConfig *expected_bg;
static TrainerCardMain *expected_main;
static unsigned events[16];
static unsigned event_count;
static unsigned pos_count;
static void GF_RunVramTransferTasks(void) { events[event_count++] = 1; }
static void OamManager_ApplyAndResetBuffers(void) { events[event_count++] = 2; }
static void DoScheduledBgGpuUpdates(BgConfig *bg) { assert(bg == expected_bg); events[event_count++] = 3; }
static void OS_SetIrqCheckFlag(unsigned flag) { assert(flag == OS_IE_VBLANK); events[event_count++] = 4; }
static void BgSetPosTextAndCommit(BgConfig *bg, int layer, int op, int pos) {
    assert(bg == expected_bg && pos == -7);
    assert(layer == (pos_count < 2 ? GF_BG_LYR_SUB_1 : GF_BG_LYR_MAIN_3));
    assert(op == (pos_count % 2 ? BG_POS_OP_SET_Y : BG_POS_OP_SET_X));
    pos_count++;
    events[event_count++] = 5;
}
static void TrainerCardMain_ApplyFlipScale(TrainerCardMain *card) {
    assert(card == expected_main);
    events[event_count++] = 6;
}
""", function(signature, "TrainerCardSignature_VBlankCB"),
              function(main, "TrainerCardMain_VBlankCB"), r"""
static void check_callbacks(void) {
    BgConfig bg;
    TrainerCardMain card = { &bg, 7, 0 };
    GFIntrCB signature_callback = TrainerCardSignature_VBlankCB;
    GFIntrCB main_callback = TrainerCardMain_VBlankCB;
    const unsigned signature_events[] = { 1, 2, 3, 4 };
    const unsigned main_events[] = { 5, 5, 5, 5, 3, 1, 2, 4 };
    const unsigned scaled_events[] = { 5, 5, 5, 5, 6, 3, 1, 2, 4 };
    expected_bg = &bg;
    expected_main = &card;
    signature_callback(&bg);
    assert(event_count == sizeof(signature_events) / sizeof(*signature_events));
    assert(memcmp(events, signature_events, sizeof(signature_events)) == 0);
    event_count = 0;
    main_callback(&card);
    assert(pos_count == 4 && event_count == sizeof(main_events) / sizeof(*main_events));
    assert(memcmp(events, main_events, sizeof(main_events)) == 0);
    event_count = pos_count = 0;
    card.scaleChanged = 1;
    main_callback(&card);
    assert(pos_count == 4 && event_count == sizeof(scaled_events) / sizeof(*scaled_events));
    assert(memcmp(events, scaled_events, sizeof(scaled_events)) == 0);
    puts("PASS callbacks: exact GFIntrCB types, assignments, invocations and order");
}
"""]

parts.append("int main(void) { check_pack(); check_prompt(); check_callbacks(); return 0; }\n")

with tempfile.TemporaryDirectory(prefix="trainer-signature-check-") as directory:
    build = Path(directory)
    c_file = build / "check.c"
    binary = build / "check"
    c_file.write_text("\n".join(parts))
    command = ["clang", "-std=c11", "-Wall", "-Wextra", "-Werror", "-O1", "-g",
               "-fsanitize=address,undefined", "-fno-sanitize-recover=all",
               "-fno-omit-frame-pointer", "-ftrivial-auto-var-init=pattern",
               str(c_file), "-o", str(binary)]
    print(shlex.join(command), flush=True)
    subprocess.run(command, check=True)
    subprocess.run([str(binary)], check=True)
