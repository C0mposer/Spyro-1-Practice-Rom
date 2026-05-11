"""
skin_combiner.py
Packs per-skin .bin files into the combined skin archives used by the mod.

Each archive stores skins at fixed-size, evenly-spaced offsets so the game
can index directly into the file by skin slot number.

Usage:
  python skin_combiner.py spyro    # builds skins.bin
  python skin_combiner.py flame    # builds flames.bin
  python skin_combiner.py sparx    # builds sparx.bin
  python skin_combiner.py all      # builds all three
"""

import sys
from pathlib import Path

_BMP_DIR    = Path(__file__).parent / "bmps"
_OUTPUT_DIR = Path(__file__).parent / "final_output"

# ---------------------------------------------------------------------------
# Skin configurations
# ---------------------------------------------------------------------------

_SPYRO_SKINS = [
    "og_s1.bin", "dred.bin", "icy.bin", "emerald.bin", "rblue.bin",
    "midnight.bin", "woke.bin", "lemon_lime.bin", "creamsicle.bin",
    "mint.bin", "tex_pink.bin", "greyspyro_rev2.bin", "goth.bin",
    "gold.bin", "berry.bin", "pixie.bin", "jayo.bin", "night_vision.bin",
    "vampire.bin", "zera.bin", "nestor.bin", "titan.bin", "cosmo.bin",
    "bruno.bin", "lateef.bin", "cerny.bin", "ember.bin", "cyn_cre.bin",
    "peri.bin", "ditto.bin", "custom_spyro.bin",
]

_FLAME_SKINS = [
    "ogflame.bin", "redflame.bin", "iceflame.bin", "jadeflame.bin",
    "blue_flame.bin", "gold_flame.bin", "green_flame.bin", "pink_flame.bin",
    "acid_flame.bin", "light_purple_flame.bin", "light_pink_flame.bin",
    "pasteflame.bin", "rainbowflame.bin", "cyderflame.bin", "smoke.bin",
]

_SPARX_SKINS = [
    "og_sparx.bin", "red_sparx.bin", "icy_sparx.bin", "jade_sparx.bin",
    "darkblue_sparx.bin", "darkpurple_sparx.bin", "orange_sparx.bin",
    "pink_sparx.bin", "turquoise_sparx.bin", "silver_sparx.bin",
    "black_sparx.bin", "custom_sparx.bin",
]

# ---------------------------------------------------------------------------
# Core packer
# ---------------------------------------------------------------------------

def combine_skins(files: list, slot_size: int, output_path: Path,
                  source_dir: Path = _BMP_DIR) -> None:
    """
    Pack a list of .bin files into a single archive.

    Each skin occupies exactly `slot_size` bytes at a fixed offset
    (slot_index * slot_size), zero-padded if the source file is smaller.
    """
    _OUTPUT_DIR.mkdir(exist_ok=True)

    with open(output_path, 'wb') as out:
        for index, filename in enumerate(files):
            slot_start = index * slot_size
            out.write(b'\x00' * (slot_start - out.tell()))

            src = source_dir / filename
            out.write(src.read_bytes()[:slot_size])

    print(f"Packed {len(files)} skins → {output_path}")

# ---------------------------------------------------------------------------
# Named entry points
# ---------------------------------------------------------------------------

def combine_spyro_skins() -> None:
    combine_skins(_SPYRO_SKINS, slot_size=0x200, output_path=_OUTPUT_DIR / "skins.bin")

def combine_flame_skins() -> None:
    combine_skins(_FLAME_SKINS, slot_size=0x20,  output_path=_OUTPUT_DIR / "flames.bin")

def combine_sparx_skins() -> None:
    combine_skins(_SPARX_SKINS, slot_size=0x100, output_path=_OUTPUT_DIR / "sparx.bin")

def combine_all_skins() -> None:
    combine_spyro_skins()
    combine_flame_skins()
    combine_sparx_skins()

# ---------------------------------------------------------------------------
# CLI
# ---------------------------------------------------------------------------

if __name__ == "__main__":
    commands = {"spyro": combine_spyro_skins, "flame": combine_flame_skins,
                "sparx": combine_sparx_skins, "all": combine_all_skins}

    if len(sys.argv) < 2 or sys.argv[1] not in commands:
        print("Usage: python skin_combiner.py <spyro|flame|sparx|all>")
        sys.exit(1)

    commands[sys.argv[1]]()
