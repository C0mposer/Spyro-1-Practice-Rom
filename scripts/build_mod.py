"""
Full release build for PracitceRomPort (matches upstream Spyro1_PracticeCodes flow).

1. Code Injection Toolchain: `compile` + patch into `build/patched_*` (no psxbuild / no GUI ISO).
2. Materialize `build/spyro1_PracticeCodes/` from `.config/game_files/<Build>/`.
3. Overlay patched main exe + KERN/HEAD/ASMHEADS (and PS2M when built) from `.config/output/bin_files/`.
4. Skin/splash bins from `scripts/bmp_scripts/final_output/`, `custom_build/bundled_assets/`, or game files.
5. WAD/VRAM patches (`patch_wad`, `patch_vram_wad`), same as upstream scripts.
6. `mkpsxiso` on XML from `custom_build/` (same role as `games/Spyro1_PracticeCodes/build/*.xml`).
7. Final `.bin/.cue` output is written to `dist/`.

Run from project root:
  python scripts/build_mod.py NTSC
  python scripts/build_mod.py REDUX
  python scripts/build_mod.py DUCKSTATION
  python scripts/build_mod.py DECKARD_PS2_NTSC
  python scripts/build_mod.py IOP_PS2_NTSC

Requires: game files under `.config/game_files/<Build>/` (per modproj).
"""
from __future__ import annotations

import json
import os
import shutil
import subprocess
import sys
import xml.etree.ElementTree as ET
from typing import Optional

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.normpath(os.path.join(SCRIPT_DIR, ".."))
TOOLCHAIN_ROOT = os.path.normpath(os.path.join(ROOT, "..", ".."))
CLI = os.path.join(TOOLCHAIN_ROOT, "CLI.py")
PROJECT = "PracitceRomPort"
MODPROJ = os.path.join(ROOT, "PracitceRomPort.modproj")
OUTPUT_DIR = os.path.join(ROOT, ".config", "output")
BIN_OUT = os.path.join(ROOT, ".config", "output", "bin_files")
BUNDLED_ASSETS = os.path.join(ROOT, "custom_build", "bundled_assets")
DIST_DIR = os.path.join(ROOT, "dist")
MKPSXISO_EXE = os.path.join(ROOT, "scripts", "mkpsxiso.exe")

MKPSXISO_XML = {
    "NTSC": "spyro1_PracticeCodes_Manual.xml",
    "REDUX": "spyro1_PracticeCodes_Manual.xml",
    "DUCKSTATION": "spyro1_PracticeCodes_Manual_DUCKSTATION.xml",
    "DECKARD_PS2_NTSC": "spyro1_PracticeCodes_Manual_DECKARD.xml",
    "IOP_PS2_NTSC": "spyro1_PracticeCodes_Manual.xml",
}

ISO_OUT = {
    "NTSC": ("Spyro 1 Practice Rom PS1.bin", "Spyro 1 Practice Rom PS1.cue"),
    "REDUX": ("Spyro 1 Practice Rom REDUX.bin", "Spyro 1 Practice Rom REDUX.cue"),
    "DUCKSTATION": ("Spyro 1 Practice Rom Duckstation.bin", "Spyro 1 Practice Rom Duckstation.cue"),
    "DECKARD_PS2_NTSC": ("Spyro 1 Practice Rom PS2 Deckard.bin", "Spyro 1 Practice Rom PS2 Deckard.cue"),
    "IOP_PS2_NTSC": ("Spyro 1 Practice Rom PS2 IOP.bin", "Spyro 1 Practice Rom PS2 IOP.cue"),
}

# Toolchain codecave output -> disc names expected by custom mkpsxiso XML
DISC_KERNEL_FILES = [
    (("kern.bin",), "KERN.BIN"),
    (("kern2.bin",), "KERN2.BIN"),
    (("head.bin",), "HEAD.BIN"),
    (("asmheads.bin",), "ASMHEADS.BIN"),
]

OPTIONAL_SKIN_NAMES = ("flames.bin", "sparx.bin", "skins.bin", "splash1.bin", "splash2.bin")
DUCKSTATION_EXTRA_DISC_SIZE = 0x400000


def _load_build_config(build: str) -> dict:
    with open(MODPROJ, encoding="utf-8") as f:
        cfg = json.load(f)
    for b in cfg.get("build_versions") or []:
        if b.get("build_name") == build:
            return b
    raise SystemExit(f"Build {build!r} not found in PracitceRomPort.modproj")


def _game_files_dir(build: str) -> str:
    rel = _load_build_config(build).get("extracted_game_folder", "./.config/game_files/NTSC")
    gf = rel.replace("./", "").lstrip("/\\")
    return os.path.normpath(os.path.join(ROOT, gf))


def _run_cli_build(build: str) -> None:
    cmd = [sys.executable, CLI, "build", PROJECT, "--build", build]
    print("Running:", " ".join(cmd))
    subprocess.check_call(cmd, cwd=ROOT)


def _ensure_shared_ps1_xml(build: str) -> None:
    cfg = _load_build_config(build)
    rel = cfg.get("extracted_game_folder", "./.config/game_files/NTSC")
    shared_build = os.path.basename(os.path.normpath(rel))
    if not shared_build or shared_build == build:
        return

    dst = os.path.join(OUTPUT_DIR, f"psxbuild_{build}.xml")
    if os.path.isfile(dst):
        return

    src = os.path.join(OUTPUT_DIR, f"psxbuild_{shared_build}.xml")
    if not os.path.isfile(src):
        raise FileNotFoundError(
            f"Missing shared PS1 build XML for {build}: {src}\n"
            "Re-extract the shared source build first."
        )

    shutil.copy2(src, dst)



def _materialize_disc_staging(build: str) -> str:
    """Populate build/spyro1_PracticeCodes like upstream `build/` before mkpsxiso."""
    gf = _game_files_dir(build)
    if not os.path.isdir(gf):
        raise FileNotFoundError(
            f"Game folder missing for {build}: {gf}\n"
            "Extract the PS1 game in the toolchain GUI for this build, or copy files there."
        )
    staging = os.path.join(ROOT, "build", "spyro1_PracticeCodes")
    if os.path.isdir(staging):
        shutil.rmtree(staging)
    shutil.copytree(gf, staging)

    cfg = _load_build_config(build)
    generated_disc_files = {
        cave.get("injection_file")
        for cave in (cfg.get("code_caves") or [])
        if cave.get("enabled") and cave.get("injection_file")
    }
    main_exe = cfg.get("main_executable") or "SCUS_420.67"
    patched = os.path.join(ROOT, "build", f"patched_{main_exe}")
    if not os.path.isfile(patched):
        raise FileNotFoundError(
            f"Expected patched main binary missing: {patched}\n"
            "CLI build must succeed first."
        )
    shutil.copy2(patched, os.path.join(staging, main_exe))

    for src_names, disc_name in DISC_KERNEL_FILES:
        if disc_name not in generated_disc_files:
            continue
        for src_name in src_names:
            bp = os.path.join(BIN_OUT, src_name)
            if os.path.isfile(bp) and os.path.getsize(bp) > 0:
                shutil.copy2(bp, os.path.join(staging, disc_name))
                break
    if build == "DUCKSTATION":
        extra_src = os.path.join(BIN_OUT, "extra.bin")
        extra_dst = os.path.join(staging, "EXTRA.BIN")
        if not os.path.isfile(extra_src):
            raise FileNotFoundError(
                f"Expected extra codecave binary missing for {build}: {extra_src}"
            )
        shutil.copy2(extra_src, extra_dst)
        extra_size = os.path.getsize(extra_dst)
        if extra_size < DUCKSTATION_EXTRA_DISC_SIZE:
            with open(extra_dst, "ab") as f:
                f.write(b"\0" * (DUCKSTATION_EXTRA_DISC_SIZE - extra_size))
    if build == "DECKARD_PS2_NTSC":
        for src_name in ("deckard_ps2m.bin", "ps2m.bin"):
            bp = os.path.join(BIN_OUT, src_name)
            if os.path.isfile(bp) and os.path.getsize(bp) > 0:
                shutil.copy2(bp, os.path.join(staging, "BE.BIN"))
                break
        mstr = os.path.join(BIN_OUT, "ps2m_strings.bin")
        if os.path.isfile(mstr) and os.path.getsize(mstr) > 0:
            shutil.copy2(mstr, os.path.join(staging, "PS2MSTR.BIN"))
    return staging


def _optional_asset_search_paths(game_files: str) -> tuple[str, ...]:
    return (
        os.path.join(ROOT, "scripts", "bmp_scripts", "final_output"),
        BUNDLED_ASSETS,
        game_files,
    )


def _resolve_optional_asset(name: str, game_files: str) -> Optional[str]:
    for base in _optional_asset_search_paths(game_files):
        p = os.path.join(base, name)
        if os.path.isfile(p):
            return p
    return None


def _copy_optional_skins_and_splashes(staging: str, game_files: str) -> None:
    """Copy assets when present; splash bins are often build-only artifacts upstream."""
    for name in OPTIONAL_SKIN_NAMES:
        dst = os.path.join(staging, name)
        if os.path.isfile(dst):
            continue
        src = _resolve_optional_asset(name, game_files)
        if src:
            shutil.copy2(src, dst)
        else:
            print(f"Note: optional asset not found (will drop from ISO XML if still missing): {name}")


def _require_skins_and_splashes(staging: str, game_files: str) -> None:
    missing = [name for name in OPTIONAL_SKIN_NAMES if not os.path.isfile(os.path.join(staging, name))]
    if not missing:
        return

    searched = "\n  ".join(_optional_asset_search_paths(game_files))
    raise FileNotFoundError(
        "Missing required skin/splash assets for ISO build: "
        + ", ".join(missing)
        + "\nSearched:\n  "
        + searched
    )


def _resolve_deckard_extra(name: str, game_files: str) -> Optional[str]:
    for base in (BUNDLED_ASSETS, game_files):
        p = os.path.join(base, name)
        if os.path.isfile(p):
            return p
    return None


def _prune_mkpsxiso_xml_missing_sources(build_dir: str, xml_name: str) -> None:
    """Remove <file> entries (and optional <license>) whose sources are not under build_dir."""
    xml_path = os.path.join(build_dir, xml_name)
    tree = ET.parse(xml_path)
    root = tree.getroot()
    track = root.find(".//track")
    if track is not None:
        lic = track.find("license")
        if lic is not None and lic.get("file"):
            lf = os.path.normpath(os.path.join(build_dir, lic.attrib["file"].replace("/", os.sep)))
            if not os.path.isfile(lf):
                print("Note: dropping <license> (missing):", lic.attrib["file"])
                track.remove(lic)
    for parent in root.iter():
        for child in list(parent):
            if child.tag != "file":
                continue
            src = child.get("source")
            if not src:
                continue
            abs_path = os.path.normpath(os.path.join(build_dir, src.replace("/", os.sep)))
            if not os.path.isfile(abs_path):
                print("Note: dropping <file> from ISO XML (missing):", src)
                parent.remove(child)
    tree.write(xml_path, encoding="utf-8", xml_declaration=True)


def _prepare_license_and_deckard_extras(build: str, game_files: str, staging: str) -> None:
    build_dir = os.path.join(ROOT, "build")
    os.makedirs(build_dir, exist_ok=True)
    lic_src = os.path.join(game_files, "license_data.dat")
    if os.path.isfile(lic_src):
        spyro1 = os.path.join(build_dir, "spyro1")
        os.makedirs(spyro1, exist_ok=True)
        shutil.copy2(lic_src, os.path.join(spyro1, "license_data.dat"))
    if build == "DECKARD_PS2_NTSC":
        bepad = _resolve_deckard_extra("BEPAD.BIN", game_files)
        if bepad:
            shutil.copy2(bepad, os.path.join(build_dir, "BEPAD.BIN"))
        be = _resolve_deckard_extra("BE.BIN", game_files)
        if be and not os.path.isfile(os.path.join(staging, "BE.BIN")):
            shutil.copy2(be, os.path.join(staging, "BE.BIN"))


def _run_mkpsxiso(xml_name: str, bin_name: str, cue_name: str) -> None:
    build_dir = os.path.join(ROOT, "build")
    os.makedirs(build_dir, exist_ok=True)
    os.makedirs(DIST_DIR, exist_ok=True)
    xml_src = os.path.join(ROOT, "custom_build", xml_name)
    if not os.path.isfile(xml_src):
        raise FileNotFoundError(xml_src)
    shutil.copy2(xml_src, os.path.join(build_dir, xml_name))
    _prune_mkpsxiso_xml_missing_sources(build_dir, xml_name)
    print("Running mkpsxiso:", xml_name, "(cwd=build/)")
    subprocess.check_call([MKPSXISO_EXE, xml_name], cwd=build_dir)
    mk_bin = os.path.join(build_dir, "mkpsxiso.bin")
    mk_cue = os.path.join(build_dir, "mkpsxiso.cue")
    out_bin = os.path.join(DIST_DIR, bin_name)
    out_cue = os.path.join(DIST_DIR, cue_name)
    if os.path.isfile(out_bin):
        os.remove(out_bin)
    shutil.move(mk_bin, out_bin)
    cue_body = f'FILE "{bin_name}" BINARY\n TRACK 01 MODE2/2352\n  INDEX 01 00:00:00\n'
    with open(out_cue, "w", encoding="utf-8") as f:
        f.write(cue_body)
    if os.path.isfile(mk_cue):
        os.remove(mk_cue)


def main() -> None:
    if len(sys.argv) < 2:
        print("Usage: python scripts/build_mod.py <NTSC|REDUX|DUCKSTATION|DECKARD_PS2_NTSC|IOP_PS2_NTSC>")
        sys.exit(2)
    build = sys.argv[1].strip().upper()
    # Hacky to match old psx modding toolchain naming. Fix eventually
    if build == "PS2_DECKARD":
        build = "DECKARD_PS2_NTSC"
    if build == "PS2_IOP":
        build = "IOP_PS2_NTSC"

    if build not in MKPSXISO_XML:
        print("Unknown build:", build)
        sys.exit(2)

    os.chdir(ROOT)
    game_files = _game_files_dir(build)

    _ensure_shared_ps1_xml(build)
    _run_cli_build(build)

    if build == "DECKARD_PS2_NTSC":
        map_path = os.path.join(ROOT, ".config", "memory_map", "MyMod.map")
        ps2m_bin = os.path.join(BIN_OUT, "ps2m_strings.bin")
        if os.path.isfile(map_path) and os.path.isfile(ps2m_bin):
            flip_script = os.path.join(SCRIPT_DIR, "flip_ps2m_strings_data_from_map.py")
            subprocess.check_call(
                [sys.executable, flip_script, map_path, ps2m_bin],
                cwd=ROOT,
            )
        else:
            print(
                "Note: skip ps2m_strings .data flip (missing map or ps2m_strings.bin):",
                map_path,
                ps2m_bin,
            )

    staging = _materialize_disc_staging(build)
    _copy_optional_skins_and_splashes(staging, game_files)
    _require_skins_and_splashes(staging, game_files)
    _prepare_license_and_deckard_extras(build, game_files, staging)

    os.chdir(SCRIPT_DIR)
    sys.path.insert(0, SCRIPT_DIR)
    from patch_wad import PatchWadMiscASMPatches
    from patch_vram_wad import PatchArtisansFlag, PatchFont

    PatchWadMiscASMPatches()
    PatchArtisansFlag()
    PatchFont()

    os.chdir(ROOT)
    if build == "DECKARD_PS2_NTSC":
        from reverse_file_endianess import ReverseFileEndianness

        be = os.path.join(staging, "BE.BIN")
        if os.path.isfile(be):
            ReverseFileEndianness(be)

    xml = MKPSXISO_XML[build]
    bin_name, cue_name = ISO_OUT[build]
    _run_mkpsxiso(xml, bin_name, cue_name)
    print("Done:", os.path.join(DIST_DIR, bin_name))


if __name__ == "__main__":
    main()
