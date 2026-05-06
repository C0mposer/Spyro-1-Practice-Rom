"""
wad_patcher.py
Patches WAD.WAD — binary code/data headers, and per-level VRAM textures/CLUTs.
"""

import sys
from pathlib import Path
from PIL import Image

from bmp_scripts.bmp_reader import (
    read_bmp_palette, save_palette_bmp,
    read_bmp_pixels_flipped, flip_nibble_endianness,
)


# ---------------------------------------------------------------------------
# Paths
# ---------------------------------------------------------------------------

_SCRIPT_DIR   = Path(__file__).parent.resolve()
_PROJECT_ROOT = (_SCRIPT_DIR / "..").resolve()

def _wad_path() -> Path:
    return _PROJECT_ROOT / "build" / "spyro1_PracticeCodes" / "WAD.WAD"

def _bin_path(filename: str) -> Path:
    return _PROJECT_ROOT / ".config" / "output" / "bin_files" / filename

def _bmp_path(filename: str) -> Path:
    return _SCRIPT_DIR / "bmp_scripts" / "bmps" / filename


# ---------------------------------------------------------------------------
# Low-level WAD I/O
# ---------------------------------------------------------------------------

def _read_file(path: Path) -> bytes:
    try:
        return path.read_bytes()
    except FileNotFoundError:
        print(f"Error: file not found: {path}")
        sys.exit(1)
    except IOError as e:
        print(f"IOError reading {path}: {e}")
        sys.exit(1)


def _patch_file(path: Path, offset: int, data: bytes) -> None:
    try:
        with path.open("r+b") as f:
            f.seek(offset)
            f.write(data)
        print(f"Patched {path.name} at offset {hex(offset)}")
    except IOError as e:
        print(f"IOError patching {path}: {e}")
        sys.exit(1)


def _patch_wad(offset: int, data: bytes) -> None:
    _patch_file(_wad_path(), offset, data)


# ---------------------------------------------------------------------------
# VRAM level offsets
# ---------------------------------------------------------------------------

LEVEL_VRAM_OFFSETS = {
    "artisans":        0x801000,
    "stone_hill":      0xB94000,
    "dark_hollow":     0xF01800,
    "town_square":     0x119E800,
    "toasty":          0x148A800,
    "sunny_flight":    0x16B3000,

    "peace_keepers":   0x1891000,
    "dry_canyon":      0x1b42800,
    "cliff_town":      0x1e14000,
    "ice_cavern":      0x20a6800,
    "doctor_shemp":    0x238a000,
    "night_flight":    0x2599800,

    "magic_crafter":   0x273b800,
    "alpine_ridge":    0x2a0d800,
    "high_caves":      0x2cbf000,
    "wizard_peak":     0x2f6b800,
    "blowhard":        0x3202800,
    "crystal_flight":  0x33ea000,

    "beast_makers":    0x359d000,
    "terrace_village": 0x3800000,
    "misty_bog":       0x3a7b000,
    "tree_tops":       0x3d88800,
    "metalhead":       0x4035000,
    "wild_flight":     0x4261000,

    "dream_weavers":   0x43ed000,
    "dark_passage":    0x4688000,
    "lofty_castle":    0x4949800,
    "haunted_towers":  0x4b70000,
    "jacques":         0x4dfc800,
    "icy_flight":      0x503a000,

    "gnorc_gnexus":    0x51ef000,
    "gnorc_cove":      0x5458800,
    "twilight_harbor": 0x56d5800,
    "gnasty_gnorc":    0x591d000,
    "gnastys_loot":    0x5af7000,
}


# ---------------------------------------------------------------------------
# PSX VRAM colour conversion  (15-bit BGR 5:5:5)
# ---------------------------------------------------------------------------

def _rgb_to_vram_bgr(pixel: tuple, trans_flag: bool = False) -> int:
    r, g, b = pixel[0] >> 3, pixel[1] >> 3, pixel[2] >> 3
    word = (b << 10) | (g << 5) | r
    if trans_flag:
        word |= 0x8000
    return word


def _image_to_vram_bytes(img_path, trans_flag: bool = False) -> bytes:
    """Convert a 24-bit BMP/PNG to packed 16-bit VRAM words (little-endian)."""
    img = Image.open(img_path).convert("RGB")
    result = bytearray()
    for y in range(img.height):
        for x in range(img.width):
            word = _rgb_to_vram_bgr(img.getpixel((x, y)), trans_flag)
            result += word.to_bytes(2, byteorder="little")
    return bytes(result)


# ---------------------------------------------------------------------------
# Core VRAM write
# ---------------------------------------------------------------------------

# Identity sequence that identifies a palette-strip header (256 bytes, values 0x00–0xFF)
_IDENTITY_256 = bytes(range(256))

# Magic bytes at offset 0 of a CLUT-prefixed BMP
_CLUT_HEADER_MAGIC = b"\x10\x32\x54\x76\x98\xba\xdc\xfe"


def _patch_vram(x_vram: int, y_vram: int, width: int, height: int, level: str, data: bytes) -> None:
    """Write `data` into WAD.WAD at the VRAM coordinates for `level`."""
    y_vram -= 1
    wad_offset = LEVEL_VRAM_OFFSETS[level]

    if data[:256] == _IDENTITY_256:
        start_row = 8           # identity-256 palette strip header
    elif data[:8] == _CLUT_HEADER_MAGIC:
        start_row = 1           # single-row CLUT header
    else:
        start_row = 0

    with _wad_path().open("rb+") as f:
        for y in range(height):
            offset     = (y + y_vram) * 0x400 + (2 * x_vram) + wad_offset
            patch_start = 2 * ((y + start_row) * width)
            patch_end   = 2 * ((y + start_row + 1) * width)
            f.seek(offset)
            f.write(data[patch_start:patch_end])


# ---------------------------------------------------------------------------
# Texture / CLUT conversion
# ---------------------------------------------------------------------------

def _convert_4bit_texture(bmp_path) -> bytes:
    pixel_data, *_ = read_bmp_pixels_flipped(bmp_path, 4)
    return flip_nibble_endianness(pixel_data)


def _convert_8bit_texture(bmp_path) -> bytes:
    pixel_data, *_ = read_bmp_pixels_flipped(bmp_path, 8)
    return pixel_data


def _convert_clut_to_vram_bytes(bmp_path, bit_depth: int, trans_flag: bool) -> bytes:
    """Extract the indexed palette from `bmp_path` and return it as VRAM bytes."""
    palette_bmp = _bmp_path(Path(bmp_path).stem + "_palette.bmp")
    save_palette_bmp(read_bmp_palette(bmp_path, bit_depth), palette_bmp)
    return _image_to_vram_bytes(palette_bmp, trans_flag)


# ---------------------------------------------------------------------------
# Public patch API
# ---------------------------------------------------------------------------

def patch_4bit_texture(x_vram: int, y_vram: int, level: str, bmp,
                       width: int = 8, height: int = 32) -> None:
    _patch_vram(x_vram, y_vram, width, height, level, _convert_4bit_texture(bmp))


def patch_4bit_clut(x_vram: int, y_vram: int, level: str, bmp,
                    num_fades: int = 1, trans_flag: bool = False,
                    width: int = 16, height: int = 1) -> None:
    clut_data = _convert_clut_to_vram_bytes(bmp, 4, trans_flag)
    for i in range(num_fades):
        _patch_vram(x_vram, y_vram + i, width, height, level, clut_data)


def patch_8bit_texture(x_vram: int, y_vram: int, level: str, bmp,
                       width: int = 16, height: int = 32) -> None:
    _patch_vram(x_vram, y_vram, width, height, level, _convert_8bit_texture(bmp))


def patch_8bit_clut(x_vram: int, y_vram: int, level: str, bmp,
                    num_fades: int = 1, trans_flag: bool = False,
                    width: int = 256, height: int = 1) -> None:
    clut_data = _convert_clut_to_vram_bytes(bmp, 8, trans_flag)
    for i in range(num_fades):
        _patch_vram(x_vram, y_vram + i, width, height, level, clut_data)


def patch_bmp_direct(x_vram: int, y_vram: int, width: int, height: int,
                     level: str, bmp, trans_flag: bool = False) -> None:
    _patch_vram(x_vram, y_vram, width, height, level, _image_to_vram_bytes(bmp, trans_flag))


def patch_8bit_texture_multi(num_textures_wide: int, num_textures_tall: int,
                              texture_coords: list, level: str, bmp) -> None:
    """
    Writes an 8-bit texture tiled across a grid of VRAM slots.
    `texture_coords` is a list of [x, y] pairs — one per tile, not mutated.
    """
    patch_data  = _convert_8bit_texture(bmp)
    wad_offset  = LEVEL_VRAM_OFFSETS[level]
    coords      = [[x, y - 1] for x, y in texture_coords]  # local copy, y adjusted

    with _wad_path().open("rb+") as f:
        for texture_row in range(num_textures_tall):
            for pixel_row in range(32):
                for texture_col in range(num_textures_wide):
                    tile_index  = texture_row * num_textures_tall + texture_col
                    tx, ty      = coords[tile_index]
                    offset      = (pixel_row + ty) * 0x400 + (2 * tx) + wad_offset
                    patch_start = 2 * (
                        (texture_row * num_textures_wide * 16 * 32)
                        + (pixel_row * num_textures_wide * 16)
                        + (texture_col * 16)
                    )
                    f.seek(offset)
                    f.write(patch_data[patch_start : patch_start + 32])


# ---------------------------------------------------------------------------
# Build entry points
# ---------------------------------------------------------------------------

_BALLOONIST_PATCHES = [
    ("Artisans",       0x007fa6E8, 0xD4, 0x03000a240780013cf4772aac),
    ("Peace Keepers",  0x01887b84, 0xD4, 0x090002240780013cf47722ac),
    ("Magic Crafters", 0x02730ee8, 0xEC, 0x0f0002240780013cf47722ac),
    ("Beast Makers",   0x035948a0, 0xE4, 0x150002240780013cf47722ac),
    ("Dream Weavers",  0x043e5a74, 0xEC, 0x1b0002240780013cf47722ac),
]

# Maps level name → y_clut row for the "teehee" overlay texture
_TEEHEE_Y_CLUT = {
    "artisans": 112, "stone_hill": 112, "town_square": 112,
    "toasty": 112,   "beast_makers": 112, "twilight_harbor": 112,
    "blowhard": 16,  "tree_tops": 16,
    "peace_keepers": 0, "gnorc_gnexus": 0, "gnasty_gnorc": 0,
}


def patch_wad_head_control() -> None:
    """Patch WAD.WAD with compiled code headers and balloonist any% patches."""
    _patch_wad(0x51E9C98, _read_file(_bin_path("head.bin")))
    _patch_wad(0x51E9C88, _read_file(_bin_path("asmheads.bin")))
    _patch_wad(0x988F68,  _read_file(_SCRIPT_DIR / "bmps" / "FlagLODVertexColors.bin"))

    for name, offset, clear_size, patch_hex in _BALLOONIST_PATCHES:
        _patch_wad(offset, b"\x00" * clear_size)
        _patch_wad(offset, patch_hex.to_bytes(12, "big"))


def patch_artisans_flag() -> None:
    """Patch Artisans flag, teehee overlay, and Gnasty's Loot decorations into WAD.WAD."""
    try:
        # Main flag texture + CLUT
        patch_4bit_texture(912, 256, "artisans", _bmp_path("Comp_Kara_Logo.bmp"))
        patch_4bit_clut   (816, 480, "artisans", _bmp_path("Comp_Kara_Logo.bmp"), num_fades=8)

        # Bottom spiral / background
        patch_4bit_texture(904, 320, "artisans", _bmp_path("CustomFlag.bmp"))
        patch_4bit_texture(904, 352, "artisans", _bmp_path("CustomFlag.bmp"))
        patch_4bit_clut   (784, 480, "artisans", _bmp_path("CustomFlag.bmp"), num_fades=8)
        patch_4bit_clut   (800, 480, "artisans", _bmp_path("CustomFlag.bmp"), num_fades=8)

        # Teehee overlay — only patched into levels that share a compatible CLUT row
        for level in LEVEL_VRAM_OFFSETS:
            y_clut = _TEEHEE_Y_CLUT.get(level)
            if y_clut is None:
                continue
            patch_8bit_texture(768,  0,      level, _bmp_path("teehee.bmp"), 96, 128)
            patch_8bit_clut   (512,  y_clut, level, _bmp_path("teehee.bmp"), num_fades=16)

        # Gnasty's Loot decorations
        patch_4bit_texture(808, 448, "gnastys_loot", _bmp_path("bgnasty1.bmp"))
        patch_4bit_texture(808, 480, "gnastys_loot", _bmp_path("bgnasty2.bmp"))
        patch_4bit_texture(816, 448, "gnastys_loot", _bmp_path("bgnasty3.bmp"))
        patch_4bit_texture(816, 480, "gnastys_loot", _bmp_path("bgnasty4.bmp"))
        patch_4bit_clut(928, 128, "gnastys_loot", _bmp_path("bgnasty1.bmp"), num_fades=16)
        patch_4bit_clut(928, 144, "gnastys_loot", _bmp_path("bgnasty2.bmp"), num_fades=16)
        patch_4bit_clut(944, 112, "gnastys_loot", _bmp_path("bgnasty3.bmp"), num_fades=16)
        patch_4bit_clut(944, 128, "gnastys_loot", _bmp_path("bgnasty4.bmp"), num_fades=16)

        gnasty_coords = [
            [608, 480], [624, 448], [544, 480], [560, 448],
            [624, 480], [640, 448], [560, 480], [576, 448],
            [576, 480], [592, 448], [528, 448], [512, 480],
            [592, 480], [608, 448], [528, 480], [544, 448],
        ]
        patch_8bit_texture_multi(4, 4, gnasty_coords, "gnastys_loot", _bmp_path("coop.bmp"))
        for i in range(4):
            patch_8bit_clut(512, 144 + i, "gnastys_loot", _bmp_path("coop.bmp"))

        print("\nWAD VRAM patch successful\n")

    except Exception as e:
        print(f"Error in patch_artisans_flag: {e}")


def patch_font() -> None:
    """Patch the custom font texture and CLUT into WAD.WAD."""
    print("Patching font texture...")
    patch_4bit_texture(1000, 364, "artisans", _bmp_path("Font5.bmp"), 24, 36)
    print("Patching font CLUT...")
    patch_bmp_direct(1008, 395, 16, 5, "artisans", _bmp_path("FontCluts3.bmp"))


if __name__ == "__main__":
    patch_artisans_flag()
