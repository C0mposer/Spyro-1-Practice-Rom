"""
bmp_reader.py
Raw BMP pixel and palette extraction for PSX texture work.
Handles 4-bit and 8-bit indexed BMPs.
"""

import struct
from pathlib import Path
from PIL import Image


def read_bmp_palette(bmp_path, bit_depth: int) -> list:
    """
    Extract the color table from a 4-bit or 8-bit indexed BMP.
    Returns a list of (R, G, B) tuples.
    """
    if bit_depth not in (4, 8):
        raise ValueError(f"bit_depth must be 4 or 8, got {bit_depth}")

    default_colors = 16 if bit_depth == 4 else 256

    with open(bmp_path, 'rb') as f:
        header = f.read(54)

        actual_depth = struct.unpack('<H', header[28:30])[0]
        if actual_depth != bit_depth:
            raise ValueError(f"Expected {bit_depth}-bit BMP, got {actual_depth}-bit.")

        num_colors = struct.unpack('<I', header[46:50])[0] or default_colors
        palette_data = f.read(num_colors * 4)

    palette = []
    for i in range(num_colors):
        b, g, r, _ = struct.unpack('BBBB', palette_data[i*4 : (i+1)*4])
        palette.append((r, g, b))

    return palette


def save_palette_bmp(palette: list, output_path) -> None:
    """Save a flat list of (R, G, B) colors as a 1-pixel-tall BMP strip."""
    img = Image.new('RGB', (len(palette), 1))
    for i, color in enumerate(palette):
        img.putpixel((i, 0), color)
    img.save(str(output_path), format='BMP')


def read_bmp_pixels_flipped(bmp_path, bit_depth: int) -> tuple:
    """
    Read pixel data from a 4-bit or 8-bit BMP, flipping rows top-to-bottom.
    BMPs are stored bottom-to-top; this corrects the orientation for PSX use.
    Returns (pixel_data, width, height, row_size).
    """
    if bit_depth not in (4, 8):
        raise ValueError(f"bit_depth must be 4 or 8, got {bit_depth}")

    with open(bmp_path, 'rb') as f:
        header = f.read(54)

        width        = struct.unpack('<I', header[18:22])[0]
        height       = struct.unpack('<I', header[22:26])[0]
        actual_depth = struct.unpack('<H', header[28:30])[0]

        if actual_depth != bit_depth:
            raise ValueError(f"Expected {bit_depth}-bit BMP, got {actual_depth}-bit.")

        # 4-bit packs two pixels per byte, so row width is halved (rounded up) then 4-byte aligned
        if bit_depth == 4:
            row_size = ((width + 1) // 2 + 3) & ~3
        else:
            row_size = (width + 3) & ~3

        f.seek(struct.unpack('<I', header[10:14])[0])
        raw_data = f.read(row_size * height)

    # BMPs are stored bottom-to-top; reverse to match PSX top-to-bottom layout
    rows = [raw_data[y * row_size : (y + 1) * row_size] for y in range(height - 1, -1, -1)]
    return b''.join(rows), width, height, row_size


def flip_nibble_endianness(data: bytes) -> bytes:
    """Swap high and low nibbles in every byte. Required for 4-bit PSX texture indices."""
    return bytes(((b & 0x0F) << 4) | ((b & 0xF0) >> 4) for b in data)


def read_bmp_pixels_flipped_24bit(bmp_path) -> tuple:
    """
    Read pixel data from a 24-bit BMP, flipping rows top-to-bottom and converting BGR→RGB.
    Row padding bytes are stripped from the output.
    Returns (pixel_data, width, height).
    """
    with open(bmp_path, 'rb') as f:
        header = f.read(54)

        width        = struct.unpack('<I', header[18:22])[0]
        height       = struct.unpack('<I', header[22:26])[0]
        actual_depth = struct.unpack('<H', header[28:30])[0]

        if actual_depth != 24:
            raise ValueError(f"Expected 24-bit BMP, got {actual_depth}-bit.")

        row_size = (width * 3 + 3) & ~3  # padded row size in file
        row_data_size = width * 3         # actual pixel bytes per row (no padding)

        f.seek(struct.unpack('<I', header[10:14])[0])
        raw_data = f.read(row_size * height)

    result = bytearray()
    for y in range(height - 1, -1, -1):
        row = raw_data[y * row_size : y * row_size + row_data_size]
        for i in range(0, len(row), 3):
            b, g, r = row[i], row[i+1], row[i+2]  # BMP stores BGR
            result += bytes([r, g, b])

    return bytes(result), width, height
