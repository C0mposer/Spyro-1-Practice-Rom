"""
In-place 32-bit word endian swap for the .data slice of ps2m_strings.bin only.

Uses the GNU ld map (MyMod.map): finds the .data contribution from
deckard_strings.o inside output section .ps2m_strings, computes file offset
relative to that section's load VMA, then byteswaps each 4-byte word in that
range. .rodata and the rest of the extracted section are unchanged.

Requires string bodies to live in .rodata (const char[]). Writable .data is
then only linker artifacts you intend to fix for Deckard word loads.

Usage:
  python scripts/flip_ps2m_strings_data_from_map.py <MyMod.map> <ps2m_strings.bin>
"""
from __future__ import annotations

import re
import struct
import sys


def _parse_hex(s: str) -> int:
    return int(s, 16)


def find_ps2m_strings_data_slice(map_text: str) -> tuple[int, int] | None:
    """
    Returns (offset_in_section, length_bytes) for .data from deckard_strings.o,
    or None if not found / zero-sized.
    """
    lines = map_text.splitlines()
    section_vma: int | None = None

# Header: ".ps2m_strings  0x80a8f000     0x850" (optional "load address ...")
    header_re = re.compile(
        r"^\.ps2m_strings\s+0x([0-9a-fA-F]+)\s+0x([0-9a-fA-F]+)"
    )
    # Subsection: " .data          0x80a5f800      0x10 ... deckard_strings.o"
    data_re = re.compile(
        r"^\s+\.data\s+0x([0-9a-fA-F]+)\s+0x([0-9a-fA-F]+)\s+.*deckard_strings\.o"
    )

    start_scan = -1
    for i, line in enumerate(lines):
        m = header_re.match(line.lstrip())
        if m:
            section_vma = _parse_hex(m.group(1))
            start_scan = i + 1
            break

    if section_vma is None or start_scan < 0:
        return None

    for line in lines[start_scan:]:
        # Next top-level output section: starts with '.' at column 0 (no indent)
        if line and line[0] == "." and not line.startswith(". "):
            # e.g. ".deckard_ps2m" — end of .ps2m_strings block
            if not line.startswith(".ps2m_strings"):
                break
        m = data_re.match(line)
        if m and section_vma is not None:
            data_vma = _parse_hex(m.group(1))
            data_len = _parse_hex(m.group(2))
            offset = data_vma - section_vma
            if offset < 0:
                continue
            if data_len <= 0:
                return (offset, 0)
            return (offset, data_len)

    return None


def flip_data_words_inplace(bin_path: str, offset: int, length: int) -> None:
    if length == 0:
        return
    with open(bin_path, "r+b") as f:
        f.seek(0, 2)
        file_size = f.tell()
        if offset + length > file_size:
            raise SystemExit(
                f"Map .data slice [{offset:#x}, +{length:#x}) exceeds file size {file_size:#x}: {bin_path}"
            )
        if length % 4 != 0:
            raise SystemExit(
                f".data length {length:#x} is not a multiple of 4; adjust linker or padding"
            )
        f.seek(offset)
        chunk = f.read(length)
        words = length // 4
        unpacked = struct.unpack(f"<{words}I", chunk)
        swapped = struct.pack(f">{words}I", *unpacked)
        f.seek(offset)
        f.write(swapped)


def main() -> None:
    if len(sys.argv) != 3:
        print(__doc__.strip())
        sys.exit(2)
    map_path, bin_path = sys.argv[1], sys.argv[2]
    with open(map_path, encoding="utf-8", errors="replace") as f:
        text = f.read()
    sl = find_ps2m_strings_data_slice(text)
    if sl is None:
        print("flip_ps2m_strings_data: no .data line for deckard_strings.o under .ps2m_strings (check map)")
        return
    offset, length = sl
    if length == 0:
        print("flip_ps2m_strings_data: .data size 0 for deckard_strings.o (skip flip; const pool has no .data image)")
        return
    flip_data_words_inplace(bin_path, offset, length)
    print(f"flip_ps2m_strings_data: byteswapped .data at offset {offset:#x} len {length:#x} in {bin_path}")


if __name__ == "__main__":
    main()
