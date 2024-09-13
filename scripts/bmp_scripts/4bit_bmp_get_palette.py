# Gets the 16 colors from a 4bit BMP palette, and exports it to a new bmp file for viewing

import struct
import sys
from get_file_name import *
from PIL import Image

def extract_palette_from_bmp(input_path):
    with open(input_path, 'rb') as f:
        # Read BMP header (first 54 bytes)
        header = f.read(54)

        # Extract bit depth information from the BMP header
        bit_depth = struct.unpack('<H', header[28:30])[0]  # Little-endian unsigned short

        # Ensure it's a 4-bit BMP file
        if bit_depth != 4:
            raise ValueError("The input image is not a 4-bit BMP file.")

        # Extract the number of colors in the palette (offset 46-50)
        num_colors = struct.unpack('<I', header[46:50])[0]
        if num_colors == 0:
            num_colors = 16  # Default to 16 colors for a 4-bit BMP

        # Extract palette data (start from byte 54, 4 bytes per color)
        palette_data = f.read(num_colors * 4)

        # Parse the palette colors
        palette = []
        for i in range(num_colors):
            blue, green, red, _ = struct.unpack('BBBB', palette_data[i*4:(i+1)*4])
            palette.append((red, green, blue))  # Convert to RGB format

        return palette

def create_palette_bmp(palette, output_path):
    # Create a new 16x1 image for the palette
    img = Image.new('RGB', (16, 1))

    # Set the pixels of the image to the colors from the palette
    for i, color in enumerate(palette):
        img.putpixel((i, 0), color)

    # Save the palette image as a BMP
    img.save(output_path, format='BMP')

if __name__ == "__main__":
    input_file = sys.argv[1]            # Path to your 4-bit BMP file
    input_file_name = GetFileNameFromPathNoExt(input_file)
    output_file = "bmps/" + input_file_name + "_palette.bmp"  # Output path for the palette BMP file

    try:
        # Extract the palette from the BMP
        palette = extract_palette_from_bmp(input_file)
        print(f"Extracted {len(palette)} colors from the palette.")

        # Create and save the palette BMP
        create_palette_bmp(palette, output_file)
        print(f"Palette exported as a BMP to {output_file}.")
    except Exception as e:
        print(f"An error occurred: {e}")