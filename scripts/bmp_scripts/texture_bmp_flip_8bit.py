### Flips the order of rows in a BMP, and reverses the bit-endianess every 4 bits, then outputs to the console

import struct
import sys

def read_bmp_8bit(input_path):
    with open(input_path, 'rb') as f:
        # Read BMP header (first 54 bytes)
        header = f.read(54)

        # Extract width, height, and bit depth information from the BMP header
        width = struct.unpack('<I', header[18:22])[0]  # Little-endian unsigned int
        height = struct.unpack('<I', header[22:26])[0]
        bit_depth = struct.unpack('<H', header[28:30])[0]  # Little-endian unsigned short

        # Ensure it's an 8-bit BMP file
        if bit_depth != 8:
            raise ValueError("The input image is not an 8-bit BMP file.")

        # Calculate the row size (padded to the nearest 4-byte boundary)
        row_size = (width + 3) & ~3  # Each pixel is 1 byte in 8-bit BMP

        # Seek to the start of pixel data (offset is in bytes 10-14 of header)
        pixel_data_offset = struct.unpack('<I', header[10:14])[0]
        f.seek(pixel_data_offset)

        # Read and reverse the pixel data
        raw_data = f.read(row_size * height)
        reversed_data = []

        # Process from bottom to top and reverse
        for y in range(height - 1, -1, -1):
            row_start = y * row_size
            row_end = row_start + row_size
            row_data = raw_data[row_start:row_end]
            reversed_data.append(row_data)

        # Reversed data from top to bottom
        reversed_data = b''.join(reversed_data)
        return reversed_data, width, height, row_size

# def read_bmp_8bit(input_path):
#     with open(input_path, 'rb') as f:
#         # Read BMP header (first 54 bytes)
#         header = f.read(54)

#         # Extract width, height, and bit depth information from the BMP header
#         width = struct.unpack('<I', header[18:22])[0]  # Little-endian unsigned int
#         height = struct.unpack('<I', header[22:26])[0]
#         bit_depth = struct.unpack('<H', header[28:30])[0]  # Little-endian unsigned short

#         # Ensure it's a 4-bit BMP file
#         if bit_depth != 8:
#             raise ValueError("The input image is not a 8-bit BMP file.")

#         # Calculate the row size (padded to the nearest 4-byte boundary)
#         row_size = ((width + 1) // 2 + 3) & ~3

#         # Seek to the start of pixel data (offset is in bytes 10-14 of header)
#         pixel_data_offset = struct.unpack('<I', header[10:14])[0]
#         f.seek(pixel_data_offset)

#         # Read and reverse the pixel data
#         raw_data = f.read(row_size * height)
#         reversed_data = []

#         # Process from bottom to top and reverse
#         for y in range(height - 1, -1, -1):
#             row_start = y * row_size
#             row_end = row_start + row_size
#             row_data = raw_data[row_start:row_end]
#             reversed_data.append(row_data)

#         # Reversed data from top to bottom
#         reversed_data = b''.join(reversed_data)
#         return reversed_data, width, height, row_size

def flip_4bit_endianness(pixel_data):
    # Flip the endianess of 4-bit nibbles in each byte
    flipped_data = bytearray(len(pixel_data))
    for i in range(len(pixel_data)):
        byte = pixel_data[i]
        # Swap the high nibble and low nibble
        flipped_byte = ((byte & 0x0F) << 4) | ((byte & 0xF0) >> 4)
        flipped_data[i] = flipped_byte
    return bytes(flipped_data)

def print_pixel_data_by_row(pixel_data, width, height, row_size):
    # Print the pixel data row by row
    for row in range(height):
        start = row * row_size
        end = start + row_size
        row_data = pixel_data[start:end]
        print(row_data.hex())  # Print the hex representation of each row

if __name__ == "__main__":
    texture_bmp = sys.argv[1]  # Path to your 4-bit BMP file

    try:
        pixel_data, width, height, row_size = read_bmp_8bit(texture_bmp)
        print(f"Pixel data read successfully from {texture_bmp}.")
        print(f"Width: {width} pixels, Height: {height} pixels, Row size: {row_size} bytes.")
        print()
        print("Raw 4-bit pixel data (top to bottom) and bit-endian flipped:")

        # Flip the endianess of the 4-bit data
        # flipped_pixel_data = flip_4bit_endianness(pixel_data)
        print_pixel_data_by_row(pixel_data, width, height, row_size)
    except Exception as e:
        print(f"An error occurred: {e}")