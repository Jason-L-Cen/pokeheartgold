#!/usr/bin/env python3
"""Check Trainer Card packing, prompt/callback safety, teardown and stroke lifetime."""

import argparse
import re
import shlex
import subprocess
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("--source-root", type=Path, default=ROOT)
parser.add_argument("--candidate-root", type=Path)
args = parser.parse_args()
ROOT = args.source_root.resolve()
SOURCE = args.candidate_root.resolve() if args.candidate_root else ROOT


def extract(text, pattern):
    matches = re.findall(pattern, text, re.MULTILINE | re.DOTALL)
    assert len(matches) == 1, (pattern, len(matches))
    return matches[0]


def function(text, name):
    return extract(text, r"^static void " + name + r"\([^;]*?\) \{.*?^\}")


signature = (SOURCE / "src/application/trainer_card/overlay_trainer_card_signature.c").read_text()
main = (SOURCE / "src/application/trainer_card/overlay_trainer_card_main.c").read_text()
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
typedef uint16_t u16;
typedef uint32_t u32;
typedef int32_t s32;
#define TRUE 1
#define FALSE 0
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
typedef struct { int unused; } TrainerCardMainSprites;
typedef struct { struct { void *menuInputStatePtr; } settings; } TrainerCardMainArgs;
typedef struct {
    BgConfig *bgConfig; int scrollOffset; int scaleChanged;
    TrainerCardMainSprites sprites;
    void *frontCharFile, *badgeFrameScreenFile, *frontScreenFile;
    TrainerCardMainArgs *args; int inTouchMode;
} TrainerCardMain;
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
    TrainerCardMain card = { .bgConfig = &bg, .scrollOffset = 7, .scaleChanged = 0 };
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


exit_function = extract(main, r"^int TrainerCardMainApp_Exit\([^;]*?\) \{.*?^\}")
draw_line = function(signature, "TrainerCardSignature_DrawLine")
assert draw_line.index("if (sample == 0 && hasLast == 0)") < draw_line.index("x = *lastX;")
assert draw_line.index("if (sample == 0 && hasLast == 0)") < draw_line.index("y = *lastY;")
assert exit_function.index("Main_SetVBlankIntrCB(NULL, NULL);") < exit_function.index("TrainerCardMain_FreeStrings(cardMain);")

parts += [r"""
typedef struct { TrainerCardMain *data; } OverlayManager;
typedef int MenuInputState;
#define HEAP_ID_25 25
static GFIntrCB installed_callback;
static void *installed_owner;
static unsigned detach_count, free_count, destroyed_heap;
static TrainerCardMain *OverlayManager_GetData(OverlayManager *manager) { return manager->data; }
static void Main_SetVBlankIntrCB(GFIntrCB callback, void *owner) {
    assert(callback == NULL && owner == NULL && free_count == 0);
    installed_callback = callback; installed_owner = owner; detach_count++;
}
static void free_dependency(void) {
    assert(installed_callback == NULL && installed_owner == NULL && detach_count == 1);
    free_count++;
}
static void TrainerCardMain_ResetBgAffine(void) {}
static void TrainerCardMain_FreeStrings(TrainerCardMain *card) { assert(card == expected_main); free_dependency(); }
static void TrainerCardMain_FreeSprites(TrainerCardMainSprites *sprites) { assert(sprites == &expected_main->sprites); free_dependency(); }
static void Heap_Free(void *data) { assert(data); free_dependency(); }
static void TrainerCardMain_DestroyWindows(TrainerCardMain *card) { assert(card == expected_main); free_dependency(); }
static void TrainerCardMain_FreeBgLayers(BgConfig *bg) { assert(bg == expected_bg); free_dependency(); }
static void GF_TouchpadStopSampling(void) {}
static void TextFlags_SetCanTouchSpeedUpPrint(int enabled) { assert(!enabled); }
static void TextFlags_SetCanABSpeedUpPrint(int enabled) { assert(!enabled); }
static void MenuInputStateMgr_SetState(void *manager, MenuInputState state) { assert(manager && state == 1); }
static void OverlayManager_FreeData(OverlayManager *manager) { assert(manager->data == expected_main); free_dependency(); manager->data = NULL; }
static void Heap_Destroy(int heap) { assert(heap == HEAP_ID_25 && !installed_callback); destroyed_heap++; }
static void GF_SndHandleSetPlayerVolume(int handle, int volume) { assert(handle == 1 && volume == 0x7f && destroyed_heap == 1); }
/* The original exported Exit signature has an intentionally unused state parameter. */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
""", exit_function, r"""
#pragma clang diagnostic pop
static void check_teardown(void) {
    BgConfig bg;
    int payload;
    TrainerCardMainArgs args = { .settings.menuInputStatePtr = &payload };
    TrainerCardMain card = { .bgConfig = &bg, .frontCharFile = &payload,
        .badgeFrameScreenFile = &payload, .frontScreenFile = &payload, .args = &args, .inTouchMode = 1 };
    OverlayManager manager = { &card };
    expected_main = &card; expected_bg = &bg;
    installed_callback = TrainerCardMain_VBlankCB; installed_owner = &card;
    assert(TrainerCardMainApp_Exit(&manager, NULL) == TRUE);
    assert(detach_count == 1 && free_count == 8 && destroyed_heap == 1 && manager.data == NULL);
    puts("PASS teardown: actual Exit detaches VBlank before any dependency or owner is freed");
}
"""]

stroke_header = (ROOT / "include/application/trainer_card/overlay_trainer_card_signature_internal.h").read_text()
stroke_parts = [parts[0], extract(stroke_header, r"^#define TRAINER_CARD_SIGNATURE_PEN_COUNT \d+"),
          extract(stroke_header, r"^typedef struct TrainerCardSignatureStroke \{.*?^\} TrainerCardSignatureStroke;"),
          extract(stroke_header, r"^typedef struct TrainerCardSignaturePenPos \{.*?^\} TrainerCardSignaturePenPos;"),
          extract(signature, r"^static const u8 sTrainerCardSignaturePenBitmaps\[16\]\[16\] = \{.*?^\};"), r"""
typedef struct { int unused; } Window;
static Window *expected_window;
static s32 dots[64][2];
static unsigned dot_count, copies;
static void TrainerCardSignature_BlitClipped(Window *window, const void *src,
    s32 sx, s32 sy, s32 sw, s32 sh, s32 x, s32 y, s32 dw, s32 dh) {
    assert(window == expected_window && src == &sTrainerCardSignaturePenBitmaps[0]);
    assert(sx == 0 && sy == 0 && sw == 4 && sh == 4 && dw == 4 && dh == 4 && dot_count < 64);
    dots[dot_count][0] = x; dots[dot_count++][1] = y;
}
static void CopyWindowToVram(Window *window) { assert(window == expected_window); copies++; }
""", draw_line, function(signature, "TrainerCardSignature_SavePenPositions"),
      function(signature, "TrainerCardSignature_DrawStrokes"), r"""
static void check_strokes(void) {
    Window window;
    TrainerCardSignatureStroke strokes[TRAINER_CARD_SIGNATURE_PEN_COUNT] = {0};
    TrainerCardSignaturePenPos positions[TRAINER_CARD_SIGNATURE_PEN_COUNT] = {0};
    expected_window = &window;
    strokes[0].count = 1; strokes[0].x[0] = 34; strokes[0].y[0] = 75;
    TrainerCardSignature_DrawStrokes(&window, strokes, positions, 1);
    assert(dot_count == 0 && copies == 1 && positions[0].count == 1 && positions[0].x == 34 && positions[0].y == 75);
    assert(strokes[0].count == 0);
    strokes[0].count = 1; strokes[0].x[0] = 36; strokes[0].y[0] = 75;
    TrainerCardSignature_DrawStrokes(&window, strokes, positions, 1);
    assert(dot_count == 3 && copies == 2 && positions[0].x == 36 && positions[0].y == 75);
    for (unsigned i = 0; i < 3; i++) assert(dots[i][0] == (s32)i + 2 && dots[i][1] == 3);
    TrainerCardSignature_DrawStrokes(&window, strokes, positions, 1);
    assert(dot_count == 3 && copies == 2 && positions[0].count == 0);
    strokes[0].count = 2; strokes[0].x[0] = 40; strokes[0].y[0] = 80; strokes[0].x[1] = 41; strokes[0].y[1] = 81;
    TrainerCardSignature_DrawStrokes(&window, strokes, positions, 0);
    assert(dot_count == 5 && copies == 2 && positions[0].count == 2 && positions[0].x == 41 && positions[0].y == 81);
    assert(dots[3][0] == 8 && dots[3][1] == 8 && dots[4][0] == 9 && dots[4][1] == 9);
    for (unsigned i = 0; i < TRAINER_CARD_SIGNATURE_PEN_COUNT; i++) assert(strokes[i].count == 0);
    puts("PASS strokes: actual first-sample, next-frame continuity, release, new stroke and copy/no-copy sequence");
}
"""]

parts += stroke_parts[1:]
parts.append("int main(void) { check_pack(); check_prompt(); check_callbacks(); check_teardown(); check_strokes(); return 0; }\n")

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
    support = subprocess.run(["clang", "-cc1", "-analyzer-checker-help"], capture_output=True, text=True)
    if support.returncode == 0 and "core.uninitialized.Assign" in support.stdout:
        analysis_c = build / "strokes.c"
        analysis_c.write_text("\n".join(stroke_parts) + "\nint main(void) { check_strokes(); return 0; }\n")
        # Scalar uninitialized reads are not detected by ASan/UBSan; do not auto-initialize this analyzer input.
        analyzed = subprocess.run(["clang", "--analyze", "-std=c11", "-Xclang", "-analyzer-checker=core.uninitialized.Assign",
            "-Xclang", "-analyzer-output=text", str(analysis_c), "-o", str(build / "strokes.plist")],
            capture_output=True, text=True, check=True)
        assert "warning:" not in analyzed.stderr, analyzed.stderr
        print("PASS uninitialized: Clang analyzer found no indeterminate reads in the actual stroke sequence")
    else:
        print("SKIP uninitialized analyzer: unavailable; source-order guard and ASan/UBSan stroke checks still ran")
