"""
convert_folder_to_bin.py
Batch-convert every .bmp in a folder into a same-name .bin file.

Default input folder:
  scripts/bmp_scripts/bmps

Conversion rules:
  - *_palette.bmp -> palette_to_clut.convert_to_clut()
  - 4-bit BMP     -> indexed pixel data, rows flipped, nibble-swapped
  - 8-bit BMP     -> indexed pixel data, rows flipped
  - 24-bit BMP    -> RGB pixel data, rows flipped
"""

import argparse
import struct
from pathlib import Path

from bmp_reader import (
    flip_nibble_endianness,
    read_bmp_pixels_flipped,
    read_bmp_pixels_flipped_24bit,
)
from palette_to_clut import convert_to_clut


DEFAULT_BMP_DIR = Path(__file__).parent / "bmps"


def get_bmp_bit_depth(bmp_path: Path) -> int:
    with bmp_path.open("rb") as f:
        header = f.read(54)
    return struct.unpack("<H", header[28:30])[0]


def convert_texture_bmp(input_path: Path, output_path: Path) -> None:
    bit_depth = get_bmp_bit_depth(input_path)

    if bit_depth == 4:
        pixel_data, _, _, _ = read_bmp_pixels_flipped(input_path, 4)
        output_path.write_bytes(flip_nibble_endianness(pixel_data))
        return

    if bit_depth == 8:
        pixel_data, _, _, _ = read_bmp_pixels_flipped(input_path, 8)
        output_path.write_bytes(pixel_data)
        return

    if bit_depth == 24:
        pixel_data, _, _ = read_bmp_pixels_flipped_24bit(input_path)
        output_path.write_bytes(pixel_data)
        return

    raise ValueError(f"Unsupported BMP bit depth for {input_path.name}: {bit_depth}")


def convert_bmp(input_path: Path, transparent_palettes: bool) -> Path:
    output_path = input_path.with_suffix(".bin")

    if input_path.stem.endswith("_palette"):
        convert_to_clut(input_path, output_path, transparent=transparent_palettes)
    else:
        convert_texture_bmp(input_path, output_path)

    return output_path


def main() -> int:
    parser = argparse.ArgumentParser(description="Convert every BMP in a folder to BIN.")
    parser.add_argument(
        "folder",
        nargs="?",
        default=str(DEFAULT_BMP_DIR),
        help="Folder containing BMP files. Defaults to scripts/bmp_scripts/bmps.",
    )
    parser.add_argument(
        "--transparent-palettes",
        action="store_true",
        help="Set the STP bit when converting *_palette.bmp files.",
    )
    args = parser.parse_args()

    folder = Path(args.folder).expanduser().resolve()
    if not folder.is_dir():
        raise SystemExit(f"Folder not found: {folder}")

    bmp_files = sorted(folder.glob("*.bmp"))
    if not bmp_files:
        print(f"No .bmp files found in: {folder}")
        return 0

    converted = 0
    for bmp_path in bmp_files:
        output_path = convert_bmp(bmp_path, args.transparent_palettes)
        print(f"Written: {output_path}")
        converted += 1

    print(f"Converted {converted} BMP file(s) in {folder}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
