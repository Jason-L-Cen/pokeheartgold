#!/usr/bin/env python3
"""Make the two private source-copy boot observers without touching the WT."""
from __future__ import annotations

import argparse
from pathlib import Path


HERE = Path(__file__).resolve().parent
DEFAULT_SOURCE_ROOT = Path(__file__).resolve().parents[2]
TITLE_NAME = "src/title_screen.c"
MAIN_MENU_NAME = "src/application/main_menu/main_menu.c"

TITLE_TYPE_ANCHOR = """enum TitleScreenMainState {
    TITLESCREEN_MAIN_WAIT_FADE,
    TITLESCREEN_MAIN_START_MUSIC,
    TITLESCREEN_MAIN_PLAY,
    TITLESCREEN_MAIN_PROCEED_FLASH,
    TITLESCREEN_MAIN_PROCEED_FLASH_2,
    TITLESCREEN_MAIN_PROCEED_NOFLASH,
    TITLESCREEN_MAIN_FADEOUT,
};"""
TITLE_FADE_RETURN_ANCHOR = """        if (IsPaletteFadeFinished() == TRUE && TitleScreenAnim_UnloadAndRemoveTopScreenResources(&data->animData, data->bgConfig, data->heapID) == TRUE) {
            return TRUE;
        }"""
TITLE_NORMAL_RETURN_ANCHOR = """    return FALSE;
}

static BOOL TitleScreen_Exit"""

MAIN_MENU_TYPE_ANCHOR = """} MainMenuAppData;
"""
MAIN_MENU_NOTICE_RETURN_ANCHOR = """        MainMenu_UpdateBeaconWatch(data);
        HandleScreenScroll(data);
        return FALSE;"""
MAIN_MENU_NORMAL_RETURN_ANCHOR = """    MainMenu_UpdateBeaconWatch(data);
    HandleScreenScroll(data);
    MainMenuUtil_UpdateSprites();
    return FALSE;"""
MAIN_MENU_TERMINAL_ANCHOR = """    case 7:
        MainMenu_FreeGraphics(manager);
        return TRUE;"""
MAIN_MENU_TOUCH_ANCHOR = """    if (gSystem.newKeys & (PAD_BUTTON_Y | PAD_BUTTON_X | PAD_KEY_UP | PAD_KEY_DOWN | PAD_KEY_LEFT | PAD_KEY_RIGHT | PAD_BUTTON_B | PAD_BUTTON_A) && data->menuInputState.state == MENU_INPUT_STATE_TOUCH) {
        data->menuInputState.state = MENU_INPUT_STATE_BUTTONS;
        MainMenu_HighlightOption(data, data->currentOption);
        return FALSE;
    }"""


def require_once(source: str, anchor: str, label: str) -> None:
    count = source.count(anchor)
    if count != 1:
        raise RuntimeError(f"{label} drifted: expected one exact anchor, found {count}")


def include_block(kind: str) -> str:
    selector = "NPCYN_BOOT_TITLE_TU" if kind == "title" else "NPCYN_BOOT_MAIN_MENU_TU"
    return (
        f"\n\n/* HGYN_BOOT_HOOK_BEGIN:{kind} */\n"
        f"#define {selector} 1\n"
        '#include "npc_yesno_boot.inc"\n'
        f"#undef {selector}\n"
        f"/* HGYN_BOOT_HOOK_END:{kind} */"
    )


def call_block(kind: str, label: str, indent: str, call: str) -> str:
    return (
        f"{indent}/* HGYN_BOOT_HOOK_BEGIN:{kind}:{label} */\n"
        f"{indent}{call}\n"
        f"{indent}/* HGYN_BOOT_HOOK_END:{kind}:{label} */\n"
    )


def replace_once(source: str, old: str, new: str, label: str) -> str:
    require_once(source, old, label)
    return source.replace(old, new, 1)


def title_candidate(source: str) -> str:
    require_once(source, TITLE_TYPE_ANCHOR, "title state enum")
    require_once(source, TITLE_FADE_RETURN_ANCHOR, "title fade-out return")
    require_once(source, TITLE_NORMAL_RETURN_ANCHOR, "title normal return")
    source = source.replace(TITLE_TYPE_ANCHOR, TITLE_TYPE_ANCHOR + include_block("title"), 1)
    title_terminal = (
        "        if (IsPaletteFadeFinished() == TRUE && "
        "TitleScreenAnim_UnloadAndRemoveTopScreenResources(&data->animData, data->bgConfig, data->heapID) == TRUE) {\n"
        + call_block("title", "terminal", "            ", "NPCYNBootTitleTerminal(data, *state);")
        + "            return TRUE;\n"
        "        }"
    )
    source = replace_once(source, TITLE_FADE_RETURN_ANCHOR, title_terminal, "title terminal hook")
    title_normal = call_block("title", "normal", "    ", "NPCYNBootPublishTitle(data, *state);") + TITLE_NORMAL_RETURN_ANCHOR
    source = replace_once(source, TITLE_NORMAL_RETURN_ANCHOR, title_normal, "title normal hook")
    return '#line 1 "src/title_screen.c"\n' + source


def main_menu_candidate(source: str) -> str:
    require_once(source, MAIN_MENU_TYPE_ANCHOR, "main-menu data type")
    require_once(source, MAIN_MENU_NOTICE_RETURN_ANCHOR, "main-menu notice early return")
    require_once(source, MAIN_MENU_NORMAL_RETURN_ANCHOR, "main-menu normal update")
    require_once(source, MAIN_MENU_TERMINAL_ANCHOR, "main-menu terminal return")
    require_once(source, MAIN_MENU_TOUCH_ANCHOR, "main-menu touch-to-buttons transition")
    source = source.replace(MAIN_MENU_TYPE_ANCHOR, MAIN_MENU_TYPE_ANCHOR + include_block("main-menu"), 1)
    notice_hook = (
        "        MainMenu_UpdateBeaconWatch(data);\n"
        "        HandleScreenScroll(data);\n"
        + call_block("main-menu", "notice", "        ", "NPCYNBootPublishMainMenu(data, *state);")
        + "        return FALSE;"
    )
    source = replace_once(source, MAIN_MENU_NOTICE_RETURN_ANCHOR, notice_hook, "main-menu notice hook")
    normal_hook = (
        "    MainMenu_UpdateBeaconWatch(data);\n"
        "    HandleScreenScroll(data);\n"
        "    MainMenuUtil_UpdateSprites();\n"
        + call_block("main-menu", "normal", "    ", "NPCYNBootPublishMainMenu(data, *state);")
        + "    return FALSE;"
    )
    source = replace_once(source, MAIN_MENU_NORMAL_RETURN_ANCHOR, normal_hook, "main-menu normal hook")
    terminal_hook = (
        "    case 7:\n"
        + call_block("main-menu", "terminal", "        ", "NPCYNBootMainMenuTerminal(data, *state);")
        + "        MainMenu_FreeGraphics(manager);\n"
        "        return TRUE;"
    )
    source = replace_once(source, MAIN_MENU_TERMINAL_ANCHOR, terminal_hook, "main-menu terminal hook")
    return '#line 1 "src/application/main_menu/main_menu.c"\n' + source


def build_candidates(source_root: Path, output_dir: Path) -> tuple[Path, Path]:
    title_source_path = source_root / TITLE_NAME
    main_menu_source_path = source_root / MAIN_MENU_NAME
    if not title_source_path.is_file() or title_source_path.is_symlink():
        raise RuntimeError(f"Missing or redirected source: {title_source_path}")
    if not main_menu_source_path.is_file() or main_menu_source_path.is_symlink():
        raise RuntimeError(f"Missing or redirected source: {main_menu_source_path}")
    title_out = output_dir / TITLE_NAME
    main_menu_out = output_dir / MAIN_MENU_NAME
    if title_out.exists() or main_menu_out.exists():
        raise FileExistsError("Refusing to overwrite an existing source-copy candidate")
    title_out.parent.mkdir(parents=True, exist_ok=True)
    main_menu_out.parent.mkdir(parents=True, exist_ok=True)
    title_out.write_text(title_candidate(title_source_path.read_text()))
    main_menu_out.write_text(main_menu_candidate(main_menu_source_path.read_text()))
    return title_out, main_menu_out


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--source-root", type=Path, default=DEFAULT_SOURCE_ROOT)
    parser.add_argument("--output-dir", type=Path, required=True)
    args = parser.parse_args()
    for path in build_candidates(args.source_root.resolve(), args.output_dir.resolve()):
        print(path)


if __name__ == "__main__":
    main()
