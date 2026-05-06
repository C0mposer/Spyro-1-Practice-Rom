"""Quick scan: print non-function file-scope lines in src/*.c (heuristic)."""
import os
import re
import sys

ROOT = os.path.normpath(os.path.join(os.path.dirname(__file__), ".."))
PS2M = [
    "deckard_strings.c", "save_state.c", "save_geo.c", "ghost_menu.c", "deckard.c", "draw_poly.c",
    "ghost_replay.c", "font_testing.c", "og_spyro_skin_data.c", "color_edit_globals.c",
    "color_edit.c", "skin_editor_menu.c",
    "theatre_mode.c", "watch_memory.c",
    "spin_wheel.c",
    "deckard_dkw_usb_payload.c", "deckard_usb_basic_test.c", "hitbox_viewer.c",
]


def main():
    for fn in PS2M:
        path = os.path.join(ROOT, "src", fn)
        if not os.path.isfile(path):
            print("MISSING", fn)
            continue
        with open(path, encoding="utf-8", errors="replace") as f:
            lines = f.readlines()
        brace = 0
        for i, line in enumerate(lines):
            s = line.rstrip()
            st = s.strip()
            if not st or st.startswith("#"):
                continue
            # brace depth
            in_line = st
            in_line2 = in_line
            in_line2 = re.sub(r'"(\\.|[^"])*"', '""', in_line2)
            in_line2 = re.sub(r"'(\\.|[^'])*'", "''", in_line2)
            brace += in_line2.count("{") - in_line2.count("}")
            if brace > 0:
                continue
            if st.startswith("typedef ") or (st.startswith("struct ") and "{" in st):
                continue
            if st.startswith("enum ") and "{" in st:
                continue
            if st.startswith("//") or st.startswith("/*") or st.startswith("*"):
                continue
            if "(" in st and ")" in st:
                if st.startswith("void ") or st.startswith("static void ") or re.match(
                    r"^(static\s+)?(inline\s+)?[a-zA-Z_][\w]*\s+\**[a-zA-Z_][\w]*\s*\(", st
                ):
                    if ";" not in st and st.endswith("{"):
                        continue
                    if st.rstrip().endswith(");") or re.match(r"^[a-zA-Z_].*\)\s*;", st):
                        continue
            if st.startswith("extern "):
                print(f"{fn}:L{i+1}: {st[:140]}")
                continue
            if re.match(r"^(static\s+)?(const\s+)?(unsigned\s+)?(char|int|short|long|void|bool|float|byte|u8|u16|u32|s32|Vec3|Spyro|struct\s+)\s", st):
                if "(" in st.split("=")[0] and st.endswith("{"):
                    continue
                if " = " in st or st.endswith((";", "}", ",")) or "[" in st:
                    if not st.startswith("if ") and not st.startswith("return"):
                        print(f"{fn}:L{i+1}: {st[:140]}")


if __name__ == "__main__":
    main()
