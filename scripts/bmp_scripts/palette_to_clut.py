"""
palette_to_clut.py
Converts a palette BMP to a PSX CLUT binary file (15-bit BGR 5:5:5, 2 bytes per color).

Usage:
  python palette_to_clut.py <palette.bmp>              # no transparency
  python palette_to_clut.py <palette.bmp> --transparent # sets STP bit (0x8000)

Output is written to bmps/<name>.bin alongside this script.
"""

import sys
from pathlib import Path
from PIL import Image

_BMP_DIR = Path(__file__).parent / "bmps"


def _rgb_to_vram_bgr(pixel: tuple, transparent: bool) -> int:
    r, g, b = pixel[0] >> 3, pixel[1] >> 3, pixel[2] >> 3
    word = (b << 10) | (g << 5) | r
    if transparent:
        word |= 0x8000
    return word


def convert_to_clut(input_path, output_path, transparent: bool = False) -> None:
    """
    Convert a palette BMP to a PSX CLUT .bin file.

    transparent=True sets the STP (semi-transparency) bit on every color entry,
    which is required for flame palettes.
    """
    img = Image.open(input_path).convert('RGB')

    result = bytearray()
    pixels = [img.getpixel((x, y)) for y in range(img.height) for x in range(img.width)]

    for i, pixel in enumerate(pixels):
        # Alignment padding inserted after pixel 44 to satisfy PSX CLUT memory layout
        if i == 45:
            result += b'\x00\x00'
        result += _rgb_to_vram_bgr(pixel, transparent).to_bytes(2, byteorder='little')

    Path(output_path).write_bytes(bytes(result))


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python palette_to_clut.py <palette.bmp> [--transparent]")
        sys.exit(1)

    input_path  = Path(sys.argv[1])
    transparent = "--transparent" in sys.argv

    output_path = _BMP_DIR / (input_path.stem + ".bin")
    _BMP_DIR.mkdir(exist_ok=True)

    convert_to_clut(input_path, output_path, transparent)
    print(f"Written: {output_path}")
