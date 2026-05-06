#!/usr/bin/env python3
"""
Decode PPC hook #defines in deckard_ppc_byte_patch_test.c and sanity-check the address gate.

  python scripts/verify_deckard_ppc_hooks.py

Requires Python 3 only (no Ghidra / hardware).
"""

from __future__ import annotations

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
C_FILE = ROOT / "src" / "deckard_ppc_byte_patch_test.c"


def _rlwinm(rs: int, ra: int, sh: int, mb: int, me: int, rc: int = 0) -> int:
    return (21 << 26) | (rs << 21) | (ra << 16) | (sh << 11) | (mb << 6) | (me << 1) | rc


def _cmpwi(crf: int, ra: int, simm: int) -> int:
    return (11 << 26) | ((crf & 7) << 23) | (0 << 22) | ((ra & 0x1F) << 16) | (simm & 0xFFFF)


def _ppc_rol32(x: int, n: int) -> int:
    x &= 0xFFFFFFFF
    n &= 31
    return ((x << n) | (x >> (32 - n))) & 0xFFFFFFFF


def emulate_rlwinm(rs: int, sh: int, mb: int, me: int) -> int:
    rot = _ppc_rol32(rs, sh)
    out = 0
    for i in range(32):
        if mb <= i <= me:
            bit = (rot >> (31 - i)) & 1
            out |= bit << (31 - i)
    return out & 0xFFFFFFFF


def parse_int_define(text: str, name: str, default: int) -> int:
    m = re.search(rf"^#define\s+{name}\s+(\d+)\s*$", text, re.M)
    if not m:
        return default
    return int(m.group(1))


def _active_hook_define_block(text: str, gate_wide: int) -> str:
    """Return #if/#else slice that matches DECKARD_PPC_HOOK_GATE_WIDE (source has both branches)."""
    m = re.search(
        r"#if\s+DECKARD_PPC_HOOK_GATE_WIDE\s*(.*?)#else\s*(.*?)#endif",
        text,
        re.S | re.DOTALL,
    )
    if not m:
        return text
    return m.group(1) if gate_wide else m.group(2)


def parse_hex_define(block: str, name: str) -> int | None:
    m = re.search(rf"^#define\s+{name}\s+(0x[0-9A-Fa-f]+)u?\s", block, re.M)
    if not m:
        return None
    return int(m.group(1), 16)


def main() -> int:
    text = C_FILE.read_text(encoding="utf-8", errors="replace")

    gate_wide = parse_int_define(text, "DECKARD_PPC_HOOK_GATE_WIDE", 0)
    xor_imm = parse_int_define(text, "DECKARD_PPC_BYTE_ADDR_XOR", 3)

    hook_block = _active_hook_define_block(text, gate_wide)
    h0 = parse_hex_define(hook_block, "PPC_HOOK0_RLWINM_R11")
    h1 = parse_hex_define(hook_block, "PPC_HOOK1_CMPWI")
    if h0 is None or h1 is None:
        print("Could not find PPC_HOOK0_RLWINM_R11 / PPC_HOOK1_CMPWI in", C_FILE, file=sys.stderr)
        return 1

    errors = []
    want_h0 = _rlwinm(11, 0, 12, 20, 31) if gate_wide else _rlwinm(11, 0, 16, 16, 31)
    want_h1 = _cmpwi(1, 0, 0x80A) if gate_wide else _cmpwi(1, 0, 0x80A7)
    if h0 != want_h0:
        errors.append(f"PPC_HOOK0_RLWINM_R11 {h0:#010x} != recomputed {want_h0:#010x}")
    if h1 != want_h1:
        errors.append(f"PPC_HOOK1_CMPWI {h1:#010x} != recomputed {want_h1:#010x}")

    want_xori = 0x696B0000 | (xor_imm & 0xFFFF)

    hh0 = parse_hex_define(hook_block, "PPC_HOOK0_RLWINM_R10")
    if hh0 is not None:
        want_hh0 = _rlwinm(10, 0, 12, 20, 31) if gate_wide else _rlwinm(10, 0, 16, 16, 31)
        if hh0 != want_hh0:
            errors.append(f"PPC_HOOK0_RLWINM_R10 {hh0:#010x} != recomputed {want_hh0:#010x}")

    samples = (
        (0x80A71234, 0x80A7, 0x80A),
        (0x80A70000, 0x80A7, 0x80A),
        (0x80A1FFFF, 0x80A1, 0x80A),
        (0x80A11234, 0x80A1, 0x80A),
    )
    for ea, hi16, hi20 in samples:
        r16 = emulate_rlwinm(ea, 16, 16, 31)
        r20 = emulate_rlwinm(ea, 12, 20, 31)
        if r16 != (ea >> 16) & 0xFFFF:
            errors.append(f"srwi16 mismatch ea={ea:#010x}")
        if r20 != (ea >> 20) & 0xFFF:  # rlwinm 12,20,31 extracts top 12 bits of >>20 style?
            # Actually for 0x80A71234 >>20 = 0x80A; verify r20
            if r20 != ((ea >> 20) & 0xFFF):
                errors.append(f"srwi20 rlwinm mismatch ea={ea:#010x}: {r20:#x} vs >>20 {((ea>>20)&0xfff):#x}")

    if errors:
        print("FAIL:")
        for e in errors:
            print(" ", e)
        return 1

    mode = "WIDE (0x80A megabyte)" if gate_wide else "NARROW (0x80A6 64K prefix)"
    print("OK:", C_FILE.name, "|", mode, "| XOR imm", xor_imm)
    print(f"  PPC_HOOK0_RLWINM_R11 = {h0:#010x}")
    print(f"  PPC_HOOK1_CMPWI      = {h1:#010x}")
    print("  xor_path if gate matches (narrow: (ea>>16)==0x80A6, wide: (ea>>20)==0x80A):")
    for ea, hi16, hi20 in samples:
        if gate_wide:
            ok = hi20 == 0x80A
        else:
            ok = hi16 == 0x80A6
        print(f"    ea={ea:#010x}  xor_path={ok}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
